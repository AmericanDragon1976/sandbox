#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/types.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <json-c/json.h>
#include <event2/bufferevent_ssl.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>

#include <signal.h>
#include <ctype.h>


void 
event_cb(struct bufferevent *buffer_event, short what, void *ctx)
{ 
    if (what & BEV_EVENT_ERROR) {
        unsigned long err;

        if (errno)
            perror("connection error");
    }

    if (what & BEV_EVENT_EOF)
        printf("EOF\n");

    if (what & BEV_EVENT_CONNECTED)
        printf("CONNECTION SUCCESSFUL\n");

    if (what & BEV_EVENT_TIMEOUT)
        printf("TIMEOUT\n");
}

/* 
 * Call back for information comming in from the monitor, in the buffer info. Function 
 * should recive a c-string that contains the name of the service and the ip address and 
 * port number in the format a.b.c.d:port_number.
 * Call back will verify is the right service, and store the address including port num
 * in the serv member of the service struct, and then if the address is new will terminate
 * all clients connected and free memory. 
 */
void 
read_cb(struct bufferevent *bev, void *passed)
{
  struct evbuffer *input = bufferevent_get_input(bev);
  int             len;
  char            *text, temp_name[100], temp_address[100];

  len = evbuffer_get_length(input);
  text = (char *) malloc(len);
  bzero(text, len); 
  evbuffer_remove(input, text, len); 

  printf("recived %s\n", text);
}

/* 
 * Sets the information in an addrinfo structure to be used as the critera 
 * stuctrue passed into getaddrinfo().
 */
struct addrinfo* 
set_criteria_addrinfo() 
{
  struct addrinfo     *hints = (struct addrinfo *) malloc(sizeof(struct addrinfo));

  hints->ai_family = AF_INET;
  hints->ai_socktype = SOCK_STREAM;
  hints->ai_flags = 0;
  hints->ai_protocol = 0;

  return (hints);
}

/*
 * Connets to monitor and request service address for current service
 */
void 
init_svc(struct event_base *event_loop, int port_num, char *host, struct bufferevent *bev)
{
  struct addrinfo     *hints = set_criteria_addrinfo();
  struct addrinfo     *server = NULL;
  char                *port = (char *) malloc(7), name[] = "mysql";
  int                 get_address_result, connect_result;

  sprintf(port, "%d", port_num); 

  bev = bufferevent_socket_new(event_loop, -1, BEV_OPT_CLOSE_ON_FREE|EV_PERSIST);
  printf("host: %s port: %s \n", host, port);
  get_address_result = getaddrinfo(host, port, hints, &server);
  connect_result = bufferevent_socket_connect(bev, server->ai_addr, server->ai_addrlen);
  bufferevent_setcb(bev, read_cb, NULL, event_cb, NULL);
  printf("connect_result %d name: %s\n", connect_result, name);
  bufferevent_enable(bev, EV_READ|EV_WRITE);
  bufferevent_write(bev, &name, sizeof(name));
}

void main()
{
  struct event_base   *event_loop = event_base_new();
  struct bufferevent  *bev = NULL;
  int 				  port_num = 4000;

  init_svc(event_loop, port_num, "127.0.0.1", bev);
  event_base_dispatch(event_loop);
}