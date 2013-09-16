#include "client.h"

	client_list 	*clients = NULL;

void 
main()
{
	int 			iterations = 0, i = 0, j = 0;
	char 			path[] = {"/home/eduard/workspace/"};
	client_node 	*cli_nod = NULL;
	client 			*cli = NULL;
	process_node 	*proc_nod = NULL, *temp_node = NULL, *temp_node_two = NULL;
	process 		*proc = NULL;

//printf("two \n");
	while(iterations < 10000){
		clients = new_client_list(NULL ,path);
		for (j = 0; j < 10; j++){
		cli = new_client((uv_tcp_t *) malloc(sizeof(uv_tcp_t)), new_process_node(new_null_process(), NULL));

			for(i = 0; i < 500; i++){ //printf ("%d \n", i);
				proc = new_null_process(); 
				cli->processes = new_process_node(proc, cli->processes);
				if (i % 100 == 110)
					printf("i = %d ", i);
			} //printf(" \n");
			clients->head = new_client_node(cli, clients->head);
			//printf("j = %d ", j);
		}

//printf(" \n");

//printf("client node ptr ");
		clients = free_client_list(clients); 
//printf("one \n");
		if (++iterations % 1000 == 0)
			printf("looped %d times\n", iterations);
	}
}