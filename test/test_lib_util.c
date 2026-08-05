#include <p101_env/env.h>
#include <p101_error/error.h>
#include <p101_util/endian.h>
#include <p101_util/tool_run.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

static int failures;

#define EXPECT(condition)                                                                                                                                                                                                                                          \
    do                                                                                                                                                                                                                                                             \
    {                                                                                                                                                                                                                                                              \
        if(!(condition))                                                                                                                                                                                                                                           \
        {                                                                                                                                                                                                                                                          \
            fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #condition);                                                                                                                                                                                   \
            failures++;                                                                                                                                                                                                                                            \
        }                                                                                                                                                                                                                                                          \
    } while(0)

struct event_counts
{
    int enters;
    int exits;
};

static void observe_calls(const struct p101_env *env, p101_env_call_event event, const char *call_name, const char *arguments, const char *result, const char *file_name, const char *function_name, int line_number, void *user_data)
{
    struct event_counts *counts;

    (void)env;
    (void)call_name;
    (void)arguments;
    (void)result;
    (void)function_name;
    (void)line_number;
    counts = user_data;
    if(file_name == NULL || strstr(file_name, "lib_util/src/") == NULL)
    {
        return;
    }
    if(event == P101_ENV_CALL_ENTER)
    {
        counts->enters++;
    }
    else
    {
        counts->exits++;
    }
}

static int native_is_little_endian(void)
{
    const uint16_t value = UINT16_C(0x0102);
    const uint8_t *bytes;

    bytes = (const uint8_t *)&value;
    return bytes[0] == UINT8_C(0x02);
}

static void test_byte_swaps(const struct p101_env *env)
{
    uint32_t value;

    EXPECT(p101_bswap16(env, UINT16_C(0x1234)) == UINT16_C(0x3412));
    EXPECT(p101_bswap32(env, UINT32_C(0x12345678)) == UINT32_C(0x78563412));
    EXPECT(p101_bswap64(env, UINT64_C(0x0123456789abcdef)) == UINT64_C(0xefcdab8967452301));

    for(value = 0; value <= UINT16_MAX; value++)
    {
        uint16_t original;

        original = (uint16_t)value;
        EXPECT(p101_bswap16(NULL, p101_bswap16(NULL, original)) == original);
    }
}

static void test_conversions(const struct p101_env *env)
{
    const uint16_t value16 = UINT16_C(0x1234);
    const uint32_t value32 = UINT32_C(0x12345678);
    const uint64_t value64 = UINT64_C(0x0123456789abcdef);
    uint16_t       once;

    EXPECT(p101_le16toh(env, p101_htole16(env, value16)) == value16);
    EXPECT(p101_le32toh(env, p101_htole32(env, value32)) == value32);
    EXPECT(p101_le64toh(env, p101_htole64(env, value64)) == value64);
    EXPECT(p101_be16toh(env, p101_htobe16(env, value16)) == value16);
    EXPECT(p101_be32toh(env, p101_htobe32(env, value32)) == value32);
    EXPECT(p101_be64toh(env, p101_htobe64(env, value64)) == value64);
    EXPECT(p101_is_little_endian(env) == native_is_little_endian());

    once = value16;
    EXPECT(p101_bswap16(env, once++) == UINT16_C(0x3412));
    EXPECT(once == (uint16_t)(value16 + 1U));
}

static void test_tool_run(const struct p101_env *env, struct p101_error *err)
{
    struct p101_tool_argv        arguments;
    struct p101_tool_read_pipe   pipe_state;
    struct p101_tool_run_options options = {0};
    char                         line[64];
    char                        *echo_argv[]    = {"/bin/echo", "hello", NULL};
    char                        *success_argv[] = {"/usr/bin/true", NULL};
    char                        *missing_argv[] = {"/definitely/missing/p101-tool", NULL};
    int                          status;

    options.stdout_path     = "/dev/null";
    options.stderr_path     = "/dev/null";
    options.diagnostic_name = "test tool";
    options.output_mode     = 0600;

    status = p101_tool_run_capture(env, err, success_argv, &options);
    EXPECT(p101_error_has_no_error(err));
    EXPECT(WIFEXITED(status));
    EXPECT(WEXITSTATUS(status) == 0);

    status = p101_tool_run_capture(env, err, missing_argv, &options);
    EXPECT(p101_error_has_no_error(err));
    EXPECT(WIFEXITED(status));
    EXPECT(WEXITSTATUS(status) == 127);

    p101_tool_argv_init(&arguments);
    EXPECT(p101_tool_argv_append(env, err, &arguments, "/bin/echo"));
    EXPECT(p101_tool_argv_append_prefixed(env, err, &arguments, "--name=", "p101"));
    EXPECT(arguments.count == 2U);
    EXPECT(strcmp(arguments.values[0], "/bin/echo") == 0);
    EXPECT(strcmp(arguments.values[1], "--name=p101") == 0);
    EXPECT(arguments.values[2] == NULL);
    p101_tool_argv_destroy(env, &arguments);
    EXPECT(arguments.values == NULL);
    EXPECT(arguments.count == 0U);

    EXPECT(p101_tool_read_pipe_open(env, err, echo_argv, "test pipe", true, &pipe_state));
    EXPECT(fgets(line, sizeof(line), pipe_state.stream) != NULL);
    EXPECT(strcmp(line, "hello\n") == 0);
    status = p101_tool_read_pipe_close(env, err, &pipe_state);
    EXPECT(p101_error_has_no_error(err));
    EXPECT(WIFEXITED(status));
    EXPECT(WEXITSTATUS(status) == 0);
}

int main(void)
{
    struct event_counts counts = {0};
    struct p101_error  *err;
    struct p101_env    *env;
    struct p101_env    *observer_env;

    err = p101_error_create(false);
    if(err == NULL)
    {
        return EXIT_FAILURE;
    }
    env = p101_env_create(err, NULL);
    if(env == NULL)
    {
        p101_error_destroy(err);
        return EXIT_FAILURE;
    }

    observer_env = p101_env_create(err, NULL);
    if(observer_env == NULL)
    {
        p101_env_destroy(env);
        p101_error_destroy(err);
        return EXIT_FAILURE;
    }

    test_byte_swaps(env);
    p101_env_set_call_observer(observer_env, observe_calls, &counts);
    test_conversions(observer_env);
    p101_env_set_call_observer(observer_env, NULL, NULL);
    p101_env_destroy(observer_env);
    EXPECT(counts.enters > 0);
    EXPECT(counts.enters == counts.exits);
    test_conversions(env);
    test_tool_run(env, err);

    p101_env_destroy(env);
    p101_error_destroy(err);
    return (failures == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
