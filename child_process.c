#include <stdio.h>

#include <stdbool.h>
#include <uv.h>

uv_loop_t *loop;
uv_process_t child_req;
uv_process_options_t options;

/*
 * 
 */
bool 
file_exist(char file_path[])
{
    if (access(file_path, F_OK) != -1)
        return(true);
    else
        return(false);
}

void on_exit(uv_process_t *req, int exit_status, int term_signal) {
    fprintf(stderr, "Process exited with status %d, signal %d\n", exit_status, term_signal);
    uv_close((uv_handle_t*) req, NULL);
}

int main() {
    loop = uv_default_loop();
    char path[] = {"/home/eduard/workspace/sandbox/hello"};

    char* args[3];
    args[0] = path;
    args[1] = " ";
    args[2] = NULL;

    options.exit_cb = on_exit;
    options.file = args[0];
    options.args = args;

    if (uv_spawn(loop, &child_req, options)) {
        fprintf(stderr, "%s\n", uv_strerror(uv_last_error(loop)));
        return 1;
    }

    return uv_run(loop, UV_RUN_DEFAULT);
}