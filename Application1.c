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
    char group_id[256] = "group1";
    char secret[256] = "jxuk";

    char *clientValues = NULL;

    establish_connection(group_id, secret);
    put_value("key1", "hello");
    put_value("key2", "world");
    put_value("key5", "ENJ");

    get_value("key2", &clientValues);
    printf("%s\n", clientValues);

    register_callback("key1", callback_function);
    register_callback("key2", callback_function);
    put_value("key2", "DOGE");
    register_callback("key5", callback_function);
    sleep(5);
    delete_value("key1");
    put_value("key1", "spaceman");
    delete_value("key6");
    put_value("key2", "DOGE2");
    put_value("key1", "BANA1");
    put_value("key4", "GOT IT");
    free(clientValues);
    close_connection();

    return 0;
}