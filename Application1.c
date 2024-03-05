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
    char secret[256] = "vaar";

    char *clientValues = NULL;

    establish_connection(group_id, secret);
    put_value("key1", "bitcoinTOZERO");
    put_value("key2", "DOGE");
    put_value("key5", "ENJ");

    get_value("key2", &clientValues);
    printf("%s\n", clientValues);

    register_callback("key1", callback_function);
    register_callback("key2", callback_function);
    put_value("key2", "DOGE");
    register_callback("key5", callback_function);
    sleep(5);
    delete_value("key1");
    put_value("key1", "BLABLABLABLA");
    delete_value("key6");
    put_value("key2", "DOGE2");
    put_value("key2", "DOGE3");
    put_value("key2", "DOGE4");
    put_value("key2", "DOGE5");
    put_value("key2", "DOGE6");

    put_value("key1", "BANA1");
    put_value("key1", "BANA2");
    put_value("key1", "BANA3");
    put_value("key1", "BANA4");
    put_value("key1", "Did you ever hear the Tragedy of Darth Plagueis the wise? I thought not. It's not a story the Jedi would tell you. It's a Sith legend. Darth Plagueis was a Dark Lord of the Sith, so powerful and so wise he could use the Force to influence the midichlorians to create life... He had such a knowledge of the dark side that he could even keep the ones he cared about from dying. The dark side of the Force is a pathway to many abilities some consider to be unnatural. He became so powerful... the only thing he was afraid of was losing his power, which eventually, of course, he did. Unfortunately, he taught his apprentice everything he knew, then his apprentice killed him in his sleep. It's ironic he could save others from death, but not himself.");
    put_value("key4", "GOT IT");
    free(clientValues);
    close_connection();

    return 0;
}