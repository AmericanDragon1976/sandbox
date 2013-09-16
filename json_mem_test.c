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

int
main ()
{
	int 	iterations = 0, len = 0;
	char 	text[500];

	while(iterations < 10000000){
		char 						*reply = NULL;
		struct json_object          *reply_json_object = json_object_new_object();
		struct json_object          *temp_hook_json_object = json_object_new_string("hooky.test.makedir");
    	struct json_object          *temp_payload_json_object = json_object_new_string("");

    	json_object_object_add (reply_json_object, "hook", temp_hook_json_object); 
    	json_object_object_add (reply_json_object, "payload", temp_payload_json_object);

    	len = strlen(json_object_to_json_string(reply_json_object));
    	reply = (char *) malloc(len + 1);
    	strcpy(reply, json_object_to_json_string(reply_json_object)); 

    	json_object_put(temp_hook_json_object); 
	    json_object_put(temp_payload_json_object); 
    	json_object_put(reply_json_object);

    	if (++iterations % 10000 == 0)
    		printf("json string built %d times.\n", iterations);

    	strcpy(text, reply);
    	free(reply);
    	reply = NULL;
	}

    iterations = 0;

	while (iterations < 10000000){
		json_object         *jobj = json_tokener_parse(text);
	    json_object         *hook_jobj = json_object_object_get(jobj, "hook");
    	json_object         *payload_jobj = json_object_object_get(jobj, "payload");
    	char 				*hook = NULL;
    	char 				*payload = NULL;
    	int 				len = 0;

    	len = json_object_get_string_len(hook_jobj) + 1; 
    	hook = (char *) malloc(len);                     
    	strcpy(hook, json_object_get_string(hook_jobj)); 
    	json_object_put(hook_jobj);				         

    	len = json_object_get_string_len(payload_jobj) + 1;
    	payload = (char *) malloc(len);
    	strcpy(payload, json_object_get_string(payload_jobj));
    	json_object_put(payload_jobj);

    	if (++iterations % 10000 == 0)
    		printf("hook: %s, payload: %s, iteration: %d\n", hook, payload, iterations);

    	free(hook);
    	hook = NULL;
    	free(payload);
    	payload = NULL;
    	json_object_put(jobj);
    	json_object_put(hook_jobj);
    	json_object_put(payload_jobj);
	}
}