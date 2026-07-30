#ifndef P101_UTIL_TOOL_RUN_H
#define P101_UTIL_TOOL_RUN_H

#include <p101_env/env.h>
#include <p101_error/error.h>
#include <sys/types.h>

typedef void (*p101_tool_run_child_setup)(const struct p101_env *env, struct p101_error *err, void *context);

struct p101_tool_run_options
{
    const char               *stdout_path;
    const char               *stderr_path;
    const char               *diagnostic_name;
    mode_t                    output_mode;
    p101_tool_run_child_setup child_setup;
    void                     *child_setup_context;
};

int  p101_tool_run_capture(const struct p101_env *env, struct p101_error *err, char *const argv[], const struct p101_tool_run_options *options);
void p101_tool_run_redirect(const struct p101_env *env, struct p101_error *err, const char *stdout_path, const char *stderr_path, mode_t output_mode);

#endif    // P101_UTIL_TOOL_RUN_H
