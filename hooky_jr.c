#include <uv.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

uv_loop_t *loop;
uv_process_t child_req;
uv_process_options_t options;


uv_buf_t 
alloc_buffer(uv_handle_t *handle, size_t suggested_size) 
{
    return uv_buf_init((char*) calloc(suggested_size, 1), suggested_size);
}

void my_on_exit(uv_process_t *req, int exit_status, int term_signal) 
{
    fprintf(stderr, "Process exited with status %d, signal %d\n", exit_status, term_signal);
    uv_close((uv_handle_t*) req, NULL);
}

void
stdout_on_read(uv_stream_t *out_pipe, ssize_t nread, uv_buf_t buf)
{
	printf("stdout *%s*\n", buf.base);
    uv_close((uv_handle_t *) out_pipe, NULL);
}

void
stderr_on_read(uv_stream_t *err_pipe, ssize_t nread, uv_buf_t buf)
{	
    printf("stderr *%s*\n", buf.base);
}

int main(int argc, char **argv) {
    uv_stdio_container_t 	child_stdio[3];
	size_t 					size = 500;
    char* 					args[2], path[size];
    uv_pipe_t 				stderr_pipe, stdout_pipe;

    if (argc != 2)
      exit(1);

    loop = uv_default_loop(); //printf("argv[0]: %s\n", argv[0]);
    uv_exepath(path, &size);  //printf("path: %s\n", path);
    strcpy(path + (strlen(path) - strlen(argv[0]) + 2), argv[1]); //printf("path: %s\n", path);

    uv_pipe_init(loop, &stderr_pipe, 1);
    uv_pipe_init(loop, &stdout_pipe, 1);
    uv_pipe_open(&stderr_pipe, 0);
    uv_pipe_open(&stdout_pipe, 1);

    args[0] = path;
    args[1] = NULL;

    options.exit_cb = my_on_exit;
    options.file = args[0];
    options.args = args;
 
    options.stdio_count = 3;
    child_stdio[0].flags = UV_IGNORE;
    child_stdio[1].flags = UV_CREATE_PIPE | UV_WRITABLE_PIPE;
    child_stdio[1].data.stream = (uv_stream_t*) &stdout_pipe;
    child_stdio[2].flags = UV_CREATE_PIPE | UV_WRITABLE_PIPE;
    child_stdio[2].data.stream = (uv_stream_t*) &stderr_pipe;
    options.stdio = child_stdio;

    int ret = uv_spawn(loop, &child_req, options);
    if (ret != 0) {
        fprintf(stderr, "%s\n", uv_strerror(ret));
        return 1;
    } 
    uv_read_start((uv_stream_t*) &stdout_pipe, alloc_buffer, stdout_on_read);
    uv_read_start((uv_stream_t*) &stderr_pipe, alloc_buffer, stderr_on_read);

    return uv_run(loop, UV_RUN_DEFAULT); 
}
