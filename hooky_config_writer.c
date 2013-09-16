/*
 * Purpose: Creates json format config files for hooky
 *
 * Author: Edward Moroni Goodson
 */

#include <stdio.h>
#include <json/json.h>
#include <stdlib.h>
#include <string.h>

typedef struct hook_definition{
	char 	*handle;
	char 	*path;
} hook_def;

typedef struct hook_node{
	struct hook_node 		*next;
	struct hook_node 		*previous;
	hook_def 				*hook;
}hook_node; 


typedef struct hook_list{
  char 			 host[16];
  int 			 port;
  hook_node*     head;
  hook_node*     tail;
} hook_list;

 int
 menu()
 {
 	int choice = 0;

 	printf("      MENU     \n\n");
 	printf("1. add/change host\n");
 	printf("2. add/change port\n");
 	printf("3. add new hook\n");
 	printf("4. change existing hook\n");
 	printf("5. save config file\n");
 	printf("6. about\n");
 	printf("7. exit program.\n\n: ");
 	scanf("   %d", &choice);
 	return (choice);
 }

void
add_change_host(hook_list *config_data)
{
	char 	change = 'Y';
	if (strcmp(config_data->host, "000.000.000.000") != 0){
		printf("The current host is %s do you wish to replace it? \n(y or n):", config_data->host);
		scanf("  %c", &change);
	} 

	if (toupper(change) == 'Y'){
		printf("Please enter host: ");
		scanf("%s", config_data->host);
	}
}

void
add_change_port(hook_list *config_data)
{
	char 	change = 'Y';
	if (config_data->port != 0){
		printf("The current port is %d do you wish to replace it? \n(y or n):", config_data->port);
		scanf("  %c", &change);
	} 

	if (toupper(change) == 'Y'){
		printf("Please enter port: ");
		scanf("%d", &config_data->port);
	}
}

void
add_hook_to_list(char *handle, char *path, hook_list *config_data)
{
	hook_node 		*new_node = malloc(sizeof(hook_node));
	new_node->hook = malloc(sizeof(hook_def));
	new_node->next = NULL;
	new_node->previous = NULL;
	new_node->hook->handle = malloc(strlen(handle) + 1);
	strcpy(new_node->hook->handle, handle);
	new_node->hook->path = malloc(strlen(path) + 1);
	strcpy(new_node->hook->path, path);

	if(config_data->head == NULL){
		config_data->head = new_node;
		config_data->tail = new_node;
	} 
	else {
		new_node->previous = config_data->tail;
		config_data->tail = new_node;
		new_node->previous->next = new_node;
	}
}

void
add_new_hook(hook_list *config_data)
{
	char 	handle[500];
	char 	path[500];

	printf("Enter handle for the new hook: ");
	scanf("%s", handle);
	printf("Enter path for the new hook: ");
	scanf("%s", path);
	add_hook_to_list(handle, path, config_data);
}

void
change_existing_hook(hook_list *config_data)
{
	printf("stub \n");
}

void
save_config_file(hook_list *config_data)
{printf("save_config_file\n");
	if (config_data->head == NULL){
		printf("Error: no hooks present, unable to create config file.\n");
	} 
	else {
		FILE 				*file_ptr = NULL;
		hook_node 			*temp_node = NULL;
		char 				*config = NULL;
		json_object      	*output_object = json_object_new_object ();
		json_object 		*hook_list_object = json_object_new_array ();
		json_object 		*port_json_object = json_object_new_int64 (config_data->port);
		json_object 		*host_json_object = json_object_new_string(config_data->host);
		json_object 		*hook_json_object = json_object_new_object();
		json_object 		*handle_json_object = NULL;
		json_object 		*path_json_object = NULL;
		json_object 		*temp_jobj = NULL;

		json_object_object_add (output_object, "host", host_json_object);
		json_object_object_add (output_object, "port", port_json_object);

		temp_node = config_data->head; 
		while (temp_node != NULL){printf("loop %s\n", temp_node->hook->handle);printf("one -- ");
			handle_json_object = json_object_new_string (temp_node->hook->handle);printf("two -- ");
			json_object_object_add (hook_json_object, "handle", json_object_get(handle_json_object));

			path_json_object = json_object_new_string (temp_node->hook->path);printf("three\n");
			json_object_object_add (hook_json_object, "path", json_object_get(path_json_object));

			temp_jobj = json_object_new_object();
			json_object_object_add(temp_jobj, "hook", json_object_get(hook_json_object));
			json_object_array_add(hook_list_object, temp_jobj);

			json_object_put(handle_json_object);
			json_object_put(path_json_object); 
			json_object_put(hook_json_object); 
//			json_object_put(temp_jobj); 

			temp_node = temp_node->next; 
		}printf("after loop \n");

		json_object_object_add (output_object, "hook-list", hook_list_object); printf("1 -- ");
		config = malloc(strlen(json_object_to_json_string(output_object)) + 1);
		strcpy(config, json_object_to_json_string(output_object));

		file_ptr = fopen("config.txt", "w"); 
		fprintf(file_ptr, "%s", config); 
		fclose(file_ptr); 

		free(config);
		json_object_put(output_object);
		json_object_put(hook_list_object);
		json_object_put(host_json_object);
		json_object_put(port_json_object);
	}printf("exit save \n");
}

void
info_about_program()
{
	printf("stub \n");
}

void
free_data(hook_list *config_data)
{
	hook_node	*temp = config_data->head;

	while(temp != NULL){
		config_data->head = config_data->head->next;
		free(temp->hook->handle);
		free(temp->hook->path);
		free(temp);
		temp = config_data->head;
	}
}

void 
main()
{
	int 		menu_choice = 1;
	hook_list 	*config_data = malloc(sizeof(hook_list));
	strcpy(config_data->host, "000.000.000.000");
	config_data->port = 0;
	config_data->head = NULL;
	config_data->tail = NULL;

	while (menu_choice != 0){ // loop till menu choice is quit
		menu_choice = menu();
	 	switch (menu_choice){
	 		case 1:
				add_change_host(config_data);
	 			break;
	 		case 2:
				add_change_port(config_data);
	 			break;
	 		case 3:
				add_new_hook(config_data);
	 			break;
	 		case 4:
	 			change_existing_hook(config_data);
	 			break;
	 		case 5:
	 			save_config_file(config_data);
	 			break;
	 		case 6:
				info_about_program();
	 			break;
	 		case 7:
				menu_choice = 0;
	 			break;
	 		default:
	 			printf("Invalid menu choice please choose an option on the menu. \n");
	 	}
	}
	free_data(config_data);
	free(config_data);
}