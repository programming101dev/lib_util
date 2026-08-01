#include <errno.h>
#include <fcntl.h>
#include <p101_c/p101_stdio.h>
#include <p101_c/p101_stdlib.h>
#include <p101_c/p101_string.h>
#include <p101_io/io.h>
#include <p101_ipc/ipc.h>
#include <p101_process/process.h>
#include <p101_util/tool_run.h>
#include <spawn.h>
#include <stdio.h>
#include <unistd.h>

#ifdef __APPLE__
    #include <crt_externs.h>
#endif

enum
{
    P101_TOOL_RUN_EXEC_FAILURE = 127,
    P101_TOOL_RUN_MESSAGE_LEN  = 512,
    P101_TOOL_ARGV_INITIAL     = 8
};

static _Noreturn void exit_child_failure(const struct p101_env *env, struct p101_error *err, const char *diagnostic_name, const char *operation_name);
static bool           tool_argv_reserve(const struct p101_env *env, struct p101_error *err, struct p101_tool_argv *arguments);
static char         **tool_environment(void);

static char **tool_environment(void)
{
#ifdef __APPLE__
    return *_NSGetEnviron();
#else
    return environ;
#endif
}

void p101_tool_argv_init(struct p101_tool_argv *arguments)
{
    arguments->values   = NULL;
    arguments->count    = 0U;
    arguments->capacity = 0U;
}

static bool tool_argv_reserve(const struct p101_env *env, struct p101_error *err, struct p101_tool_argv *arguments)
{
    size_t new_capacity;
    char **new_values;

    if(arguments->count + 1U < arguments->capacity)
    {
        return true;
    }

    new_capacity = arguments->capacity == 0U ? P101_TOOL_ARGV_INITIAL : arguments->capacity * 2U;
    if(new_capacity <= arguments->capacity || new_capacity > SIZE_MAX / sizeof(*arguments->values))
    {
        P101_ERROR_RAISE_ERRNO(err, ERANGE);
        return false;
    }
    new_values = (char **)p101_realloc(env, err, (void *)arguments->values, new_capacity * sizeof(*arguments->values));
    if(new_values == NULL)
    {
        return false;
    }
    arguments->values   = new_values;
    arguments->capacity = new_capacity;
    return true;
}

bool p101_tool_argv_append(const struct p101_env *env, struct p101_error *err, struct p101_tool_argv *arguments, const char *value)
{
    char  *copy;
    size_t length;

    P101_TRACE_SCOPE(env);
    length = p101_strlen(env, value);
    copy   = (char *)p101_malloc(env, err, length + 1U);
    if(copy == NULL)
    {
        return false;
    }
    p101_memcpy(env, copy, value, length + 1U);
    if(!tool_argv_reserve(env, err, arguments))
    {
        p101_free(env, copy);
        return false;
    }
    arguments->values[arguments->count++] = copy;
    arguments->values[arguments->count]   = NULL;
    return true;
}

bool p101_tool_argv_append_prefixed(const struct p101_env *env, struct p101_error *err, struct p101_tool_argv *arguments, const char *prefix, const char *value)
{
    char  *copy;
    size_t prefix_length;
    size_t value_length;

    P101_TRACE_SCOPE(env);
    prefix_length = p101_strlen(env, prefix);
    value_length  = p101_strlen(env, value);
    if(prefix_length > SIZE_MAX - value_length - 1U)
    {
        P101_ERROR_RAISE_ERRNO(err, ERANGE);
        return false;
    }
    copy = (char *)p101_malloc(env, err, prefix_length + value_length + 1U);
    if(copy == NULL)
    {
        return false;
    }
    p101_memcpy(env, copy, prefix, prefix_length);
    p101_memcpy(env, copy + prefix_length, value, value_length + 1U);
    if(!tool_argv_reserve(env, err, arguments))
    {
        p101_free(env, copy);
        return false;
    }
    arguments->values[arguments->count++] = copy;
    arguments->values[arguments->count]   = NULL;
    return true;
}

void p101_tool_argv_destroy(const struct p101_env *env, struct p101_tool_argv *arguments)
{
    P101_TRACE_SCOPE(env);
    if(arguments != NULL)
    {
        for(size_t index = 0U; index < arguments->count; index++)
        {
            p101_free(env, arguments->values[index]);
        }
        p101_free(env, (void *)arguments->values);
        p101_tool_argv_init(arguments);
    }
}

int p101_tool_run_capture(const struct p101_env *env, struct p101_error *err, char *const argv[], const struct p101_tool_run_options *options)
{
    const char *diagnostic_name;
    int         status;
    pid_t       pid;

    P101_TRACE_SCOPE(env);
    status          = 0;
    diagnostic_name = (options->diagnostic_name == NULL) ? "p101 tool" : options->diagnostic_name;
    p101_fflush(env, err, stdout);
    if(p101_error_has_no_error(err))
    {
        p101_fflush(env, err, stderr);
    }
    if(p101_error_has_error(err))
    {
        goto done;
    }
    pid = p101_fork(env, err);
    if(p101_error_has_error(err))
    {
        goto done;
    }

    if(pid == 0)
    {
        p101_tool_run_redirect(env, err, options->stdout_path, options->stderr_path, options->output_mode);
        if(options->child_setup != NULL)
        {
            options->child_setup(env, err, options->child_setup_context);
        }
        if(p101_error_has_error(err))
        {
            exit_child_failure(env, err, diagnostic_name, "child setup");
        }

        p101_execvp(env, err, argv[0], argv);
        exit_child_failure(env, err, diagnostic_name, "exec");
    }

    p101_waitpid(env, err, pid, &status, 0);

done:
    return status;
}

static _Noreturn void exit_child_failure(const struct p101_env *env, struct p101_error *err, const char *diagnostic_name, const char *operation_name)
{
    char message[P101_TOOL_RUN_MESSAGE_LEN];

    p101_strncpy(env, message, p101_error_get_message(err), sizeof(message) - 1U);
    message[sizeof(message) - 1U] = '\0';
    p101_error_reset(err);
    p101_fprintf(env, err, stderr, "%s: %s failed: %s\n", diagnostic_name, operation_name, message);
    p101_posix_exit_immediately(env, P101_TOOL_RUN_EXEC_FAILURE);
}

void p101_tool_run_redirect(const struct p101_env *env, struct p101_error *err, const char *stdout_path, const char *stderr_path, mode_t output_mode)
{
    int stdout_fd;
    int stderr_fd;

    P101_TRACE_SCOPE(env);
    stdout_fd = p101_open(env, err, stdout_path, O_WRONLY | O_CREAT | O_TRUNC, output_mode);
    if(p101_error_has_error(err))
    {
        goto done;
    }

    stderr_fd = p101_open(env, err, stderr_path, O_WRONLY | O_CREAT | O_TRUNC, output_mode);
    if(p101_error_has_error(err))
    {
        p101_close(env, err, stdout_fd);
        goto done;
    }

    p101_dup2(env, err, stdout_fd, STDOUT_FILENO);
    p101_dup2(env, err, stderr_fd, STDERR_FILENO);
    p101_close(env, err, stdout_fd);
    p101_close(env, err, stderr_fd);

done:
    return;
}

bool p101_tool_read_pipe_open(const struct p101_env *env, struct p101_error *err, char *const argv[], const char *diagnostic_name, bool merge_stderr, struct p101_tool_read_pipe *pipe_state)
{
    int                        descriptors[2];
    pid_t                      pid;
    posix_spawn_file_actions_t file_actions;

    P101_TRACE_SCOPE(env);
    (void)diagnostic_name;
    pipe_state->stream = NULL;
    pipe_state->pid    = -1;
    p101_pipe(env, err, descriptors);
    if(p101_error_has_error(err))
    {
        return false;
    }
    p101_posix_spawn_file_actions_init(env, err, &file_actions);
    if(p101_error_has_error(err))
    {
        p101_close(env, NULL, descriptors[0]);    // P101_ERROR_CONTRACT_ALLOW_NO_ERROR: cleanup preserves the fork failure.
        p101_close(env, NULL, descriptors[1]);    // P101_ERROR_CONTRACT_ALLOW_NO_ERROR: cleanup preserves the fork failure.
        return false;
    }
    p101_posix_spawn_file_actions_addclose(env, err, &file_actions, descriptors[0]);
    if(descriptors[1] != STDOUT_FILENO)
    {
        p101_posix_spawn_file_actions_adddup2(env, err, &file_actions, descriptors[1], STDOUT_FILENO);
    }
    if(merge_stderr && descriptors[1] != STDERR_FILENO)
    {
        p101_posix_spawn_file_actions_adddup2(env, err, &file_actions, descriptors[1], STDERR_FILENO);
    }
    if(descriptors[1] != STDOUT_FILENO && (!merge_stderr || descriptors[1] != STDERR_FILENO))
    {
        p101_posix_spawn_file_actions_addclose(env, err, &file_actions, descriptors[1]);
    }
    if(p101_error_has_error(err))
    {
        (void)p101_posix_spawn_file_actions_destroy(env, NULL, &file_actions);    // P101_ERROR_CONTRACT_ALLOW_NO_ERROR: cleanup preserves the file-action failure.
        p101_close(env, NULL, descriptors[0]);                                    // P101_ERROR_CONTRACT_ALLOW_NO_ERROR: cleanup preserves the close failure.
        p101_close(env, NULL, descriptors[1]);                                    // P101_ERROR_CONTRACT_ALLOW_NO_ERROR: cleanup preserves the file-action failure.
        return false;
    }
    p101_posix_spawnp(env, err, &pid, argv[0], &file_actions, NULL, argv, tool_environment());
    (void)p101_posix_spawn_file_actions_destroy(env, NULL, &file_actions);    // P101_ERROR_CONTRACT_ALLOW_NO_ERROR: the spawn result is authoritative.
    if(p101_error_has_error(err))
    {
        p101_close(env, NULL, descriptors[0]);    // P101_ERROR_CONTRACT_ALLOW_NO_ERROR: cleanup preserves the spawn failure.
        p101_close(env, NULL, descriptors[1]);    // P101_ERROR_CONTRACT_ALLOW_NO_ERROR: cleanup preserves the spawn failure.
        return false;
    }
    p101_close(env, err, descriptors[1]);
    if(p101_error_has_error(err))
    {
        p101_close(env, NULL, descriptors[0]);          // P101_ERROR_CONTRACT_ALLOW_NO_ERROR: cleanup preserves the close failure.
        (void)p101_waitpid(env, NULL, pid, NULL, 0);    // P101_ERROR_CONTRACT_ALLOW_NO_ERROR: cleanup preserves the close failure.
        return false;
    }
    pipe_state->stream = p101_fdopen(env, err, descriptors[0], "r");
    if(pipe_state->stream == NULL)
    {
        p101_close(env, NULL, descriptors[0]);          // P101_ERROR_CONTRACT_ALLOW_NO_ERROR: cleanup preserves the fdopen failure.
        (void)p101_waitpid(env, NULL, pid, NULL, 0);    // P101_ERROR_CONTRACT_ALLOW_NO_ERROR: cleanup preserves the fdopen failure.
        return false;
    }
    pipe_state->pid = pid;
    return true;
}

int p101_tool_read_pipe_close(const struct p101_env *env, struct p101_error *err, struct p101_tool_read_pipe *pipe_state)
{
    int status;

    P101_TRACE_SCOPE(env);
    status = 0;
    if(pipe_state->stream != NULL)
    {
        p101_fclose(env, err, pipe_state->stream);
        pipe_state->stream = NULL;
    }
    if(pipe_state->pid >= 0)
    {
        struct p101_error *wait_error;

        /*
         * Always reap the child, but do not overwrite a stream-close error
         * with a later wait failure.
         */
        wait_error = err;
        if(p101_error_has_error(err))
        {
            wait_error = NULL;
        }
        (void)p101_waitpid(env, wait_error, pipe_state->pid, &status, 0);
        pipe_state->pid = -1;
    }
    return status;
}
