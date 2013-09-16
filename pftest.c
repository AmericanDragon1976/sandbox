#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <json-c/json.h>

#include <event2/bufferevent_ssl.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>

struct txt_one{
	char *buffer_one, *buffer_two;
};

int main()
{
	struct txt_one  test_msg;

	test_msg.buffer_one = malloc(5);
	strncpy(test_msg.buffer_one, "hi", 5);
	test_msg.buffer_two = malloc(5);
	strncpy(test_msg.buffer_two, "HI", 5);
	strncpy(test_msg.buffer_two, " ", 5);

	printf("**%s**\n##%s##\n", test_msg.buffer_one, test_msg.buffer_two);
}