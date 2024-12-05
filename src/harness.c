#include <stdio.h>
#include "xml.h"
#include "cwmp.h"

void receive_rpc_request_harness(char *source){
    char *msg_out;
    xml_handle_message(source, &msg_out);
}

void receive_inform_response_harness(char *source){
    xml_parse_inform_response_message(source);
}

void receive_transfer_complete_response_harness(char *source){
    xml_parse_transfer_complete_response_message(source);
}

void receive_rpc_methods_response_harness(char *source){
    xml_parse_get_rpc_methods_response_message(source);
}

int main(int argc, char** argv) {

    // Check if we have the required argument count

    if(argc != 3)
    {
        printf("argc count not 3\n");
        return 1;
    }

    // Load the file argument

    char *source = NULL;
    FILE *fp = fopen(argv[2], "r");

    if (fp != NULL) {

        /* Go to the end of the file. */
        if (fseek(fp, 0L, SEEK_END) == 0) {

            /* Get the size of the file. */
            long bufsize = ftell(fp);
            if (bufsize == -1) { /* Error */ }

            /* Allocate our buffer to that size. */
            source = malloc(sizeof(char) * (bufsize + 1));

            /* Go back to the start of the file. */
            if (fseek(fp, 0L, SEEK_SET) != 0) { /* Error */ }

            /* Read the entire file into memory. */
            size_t newLen = fread(source, sizeof(char), bufsize, fp);
            if ( ferror( fp ) != 0 ) {
                fputs("Error reading file", stderr);
            } else {
                source[newLen++] = '\0'; /* Just to be safe. */
            }
        }
        fclose(fp);
    }
    else {
        printf("Error: File supplied by argument not found.\n");
        return -1;
    }

    /* run early cwmp initialization */
	cwmp = calloc(1, sizeof(struct cwmp_internal));
	if (!cwmp) return -1;

	INIT_LIST_HEAD(&cwmp->events);
	INIT_LIST_HEAD(&cwmp->notifications);
	INIT_LIST_HEAD(&cwmp->downloads);
	INIT_LIST_HEAD(&cwmp->uploads);
	INIT_LIST_HEAD(&cwmp->scheduled_informs);

    uloop_init();
	backup_init();

    if (external_init()) {
		printf("external scripts initialization failed\n");
		return -1;
	}

    // Invoke the corresponding fuzz harness to the supplied argument

    char *target_harness = argv[1];

    printf("%s\n\n", target_harness);
    printf("%s\n\n", source);

    int* retval;


    if (strcmp(target_harness, "rpc_request") == 0){
        receive_rpc_request_harness(source);
    } 
    else if (strcmp(target_harness, "inform_response") == 0){

        receive_inform_response_harness(source);
    }
    else if (strcmp(target_harness, "transfer_complete_response") == 0){

        receive_transfer_complete_response_harness(source);
    }
    else if (strcmp(target_harness, "rpc_methods_response") == 0){

        receive_rpc_methods_response_harness(source);
    }
    else{

        printf("Error: test harness argument not recognised.\n");
        return -1;
    }

    free(source);

    return 0;
}


