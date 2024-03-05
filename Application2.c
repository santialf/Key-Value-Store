#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include "KVS-lib.h"
#include "list.h"

void callback_function(char * key) {
    printf("The value of the key with name %s was changed\n", key);

    return;
}

int main(){
    char group_id[256] = "group2";
    char secret[256] = "brrp";

    char *clientValues = NULL;

    establish_connection(group_id, secret);

    put_value("key1", "bitcoinTOMOON");
    put_value("key2", "ADA");
    put_value("key4", "olaola");
    put_value("key5", "testeteste");
    get_value("key2", &clientValues);
    printf("%s\n", clientValues);

    register_callback("key3", callback_function);
    register_callback("key4", callback_function);
    delete_value("key4");
    delete_value("key5");
    put_value("key2", "pizza");
    sleep(5);
    free(clientValues);
    close_connection();

    return 0;
}