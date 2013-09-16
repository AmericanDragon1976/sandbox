#include <json/json.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <unistd.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <uv.h>
#include <signal.h>
#include <ctype.h>

uv_loop_t *loop;
char      *data;
int       data_length;
int       data_position;

void 
process_data_from_client(ssize_t nread, uv_buf_t buf)
{printf("process data: \n"); 
    if (nread < 0) return;

    int             i = 0, j = 0; 
    char*           num;

    num = (char *) &data_length;

    if (data_length == 0){ //printf("Reciving data length: ");
    for (i = 0; i < 4; i++)
        num[i] = buf.base[j++];

//printf("Current data length: %d\n", data_length);
//printf("Data length: %d\n", data_length);
        data = (char *) malloc(data_length + 1);
        data[data_length] = '\0';
        i = 4;
    }
//printf("Recording data from packet if any, data_length %d data_position %d\n", data_length, data_position);
//printf("data position + nread = %d\n", (int) (data_position + nread));
    for ( ; (data_position < data_length) && (i < nread); )
        data[data_position++] = buf.base[i++];
                
//printf("Finished processing this data read: data_length %d, data_position %d\n", data_length, data_position);
//    int   j = 0;
//    printf("data: ");
//    for (j = 0; j < data_length; j++)
//        printf("%c", data[j]);
//    printf("\n%s\n", data);
    printf("leaving process data\n");
}

void
package_reply() 
{printf("package_reply \n"); 
    int                         i = 0, j = 0;
    char                        *prereply;
    struct json_object          *reply_json_object = json_object_new_object();
    struct json_object          *temp_data_json_object = json_object_new_string(data);

    json_object_object_add (reply_json_object, "data", temp_data_json_object);

    data_length = strlen(json_object_to_json_string(reply_json_object));
    prereply = (char *) malloc (data_length + 1); 
    prereply[data_length] = '\0';
    strcpy(prereply, json_object_to_json_string(reply_json_object)); 
//printf("json string: ");
//for(i = 0; i < data_length + 1; i++) printf("%c", prereply[i]); printf("\n");
    free(data);
    data = (char *) malloc(data_length + 5);
    data[data_length + 4] = '\0';

    for (i = 3; i >= 0; i--){
        data[i] = ((char *) &data_length)[i];
    } printf("len: %d ", data_length);
    for (i = 4, j = 0; i < data_length + 5; i++)
        data[i] = prereply[j++];
printf("\nNumber of chars written into reply: %d \n", i);

    free(prereply);
    json_object_put(reply_json_object);
    json_object_put(temp_data_json_object);
//printf("reply: %s\n", reply);
printf("leaving package reply\n");
}

uv_buf_t alloc_buffer(uv_handle_t *handle, size_t suggested_size) {
    return uv_buf_init((char*) malloc(suggested_size), suggested_size);
}

void echo_write(uv_write_t *req, int status) {
    if (status == -1) {
        //fprintf(stderr, "Write error %s\n", uv_err_name(uv_last_error(loop)));
    }
    free(data);
    data = NULL;
    data_length = 0;
    data_position = 0;
    free(req);
}

void echo_read(uv_stream_t *client, ssize_t nread, uv_buf_t buf) {
    if (nread < 0) {
        //if (uv_last_error(loop).code != UV_EOF)
            //fprintf(stderr, "Read error %s\n", uv_err_name(uv_last_error(loop)));
        uv_close((uv_handle_t*) client, NULL);
        return;
    }//printf("buf.base %s\n", buf.base);

    process_data_from_client(nread, buf);
    free(buf.base);
    buf.base = NULL;

    if (data_length > 0 && (data_length == data_position)){

        package_reply(); //printf("one half -- ");
        buf.base = data;
//printf ("one -- ");
        uv_write_t *req = (uv_write_t *) malloc(sizeof(uv_write_t));
        req->data = (void*) data;
        buf.len = data_length + 4;
        uv_write(req, client, &buf, 1, echo_write);
//printf("two -- ");
    } 
}

void on_new_connection(uv_stream_t *server, int status) { 
    if (status == -1) {
        // error!
        return;
    }

    uv_tcp_t *client = (uv_tcp_t*) malloc(sizeof(uv_tcp_t));
    
    uv_tcp_init(loop, client);
    if (uv_accept(server, (uv_stream_t*) client) == 0) {
        uv_read_start((uv_stream_t*) client, alloc_buffer, echo_read);
    }
    else {
        uv_close((uv_handle_t*) client, NULL);
        free(client);
    }
}

int main() {
    loop = uv_default_loop();
    data = NULL;
    data_length = 0;
    data_position = 0;

    uv_tcp_t server;
    uv_tcp_init(loop, &server);

    struct sockaddr_in bind_addr = uv_ip4_addr("0.0.0.0", 4000);
    uv_tcp_bind(&server, bind_addr);
    int r = uv_listen((uv_stream_t*) &server, 128, on_new_connection);
    if (r) {
        //fprintf(stderr, "Listen error %s\n", uv_err_name(uv_last_error(loop)));
        return 1;
    }
    return uv_run(loop, UV_RUN_DEFAULT);
}