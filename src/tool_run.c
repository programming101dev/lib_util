#include <fcntl.h>
#include <p101_c/p101_stdio.h>
#include <p101_c/p101_string.h>
#include <p101_io/io.h>
#include <p101_process/process.h>
#include <p101_util/tool_run.h>
#include <stdio.h>
#include <unistd.h>

enum
{
    P101_TOOL_RUN_EXEC_FAILURE = 127,
    P101_TOOL_RUN_MESSAGE_LEN  = 512
};

static _Noreturn void exit_child_failure(const struct p101_env *env, struct p101_error *err, const char *diagnostic_name, const char *operation_name);

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
