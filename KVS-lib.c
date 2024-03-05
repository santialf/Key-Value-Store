#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <pthread.h>
#include "KVS-lib.h"

#define SOCKNAME "/tmp/KVS-LocalServer"
#define MAX 256

typedef struct _cb {
    void (*callback_function)(char *);
} cb;

int client_socket, callback_socket, i = 0;
pthread_t k1Threads;

/******************************************************************************
* establish_connection()
*
* Arguments: group_id - name of the group that client wants to connect
*            secret - group secret
* Returns: int - 0 if connection was successful 
*              - -1 if socket connection is not successful
*              - -2 if authentication fails
*
* Description:
*   Establishes socket connections with local server and tries to join a group
*****************************************************************************/
int establish_connection (char * group_id, char * secret){
    int pid;
    char error[2];

    /* connects client socket */
    if ((client_socket = socket(AF_UNIX, SOCK_STREAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_un server_address;
    memset(&server_address, 0, sizeof(struct sockaddr_un));
    server_address.sun_family = AF_UNIX;
    strcpy(server_address.sun_path, SOCKNAME);

    int connection_status = connect(client_socket, (struct sockaddr *) &server_address, sizeof(server_address));
    if (connection_status == -1) {
        printf("There was an error making a connection to the remote socket\n");
        return -1;
    }

    /* connects client callback socket */
    if ((callback_socket = socket(AF_UNIX, SOCK_STREAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_un callback_server_address;
    memset(&callback_server_address, 0, sizeof(struct sockaddr_un));
    callback_server_address.sun_family = AF_UNIX;
    strcpy(callback_server_address.sun_path, SOCKNAME);

    connection_status = connect(callback_socket, (struct sockaddr *) &callback_server_address, sizeof(callback_server_address));
    if (connection_status == -1) {
        printf("There was an error making a connection to the remote socket\n");
        return -1;
    }

    pid = getpid();
    /* sends authentication information and client ID to local server*/
    send(client_socket, &pid, sizeof(int), 0);
    send(client_socket, group_id, MAX, 0);
    send(client_socket, secret, MAX, 0);
    
    /* verifies if login was successful */
    recv(client_socket, error, 2, 0);
    if (strcmp(error, "0") == 0) {
        return 0;
    }
    printf("Login was not successful\n");
    return -2;
}

/******************************************************************************
* put_value()
*
* Arguments: key - name of the key to be created/altered
*            value - information to be associated with the key
* Returns: int - 0 if operation was successful 
*              - -1 if error sending data to server
*
* Description:
*   Creates a new key with its own value, or changes the value associated to
*an already existing key
*****************************************************************************/
int put_value(char * key, char * value){

    char len[MAX];
    int d = strlen(value);
    if (sprintf(len, "%d", d) < 0) {
        exit (0);
    }

    if (send(client_socket, "1", 2, 0) != 2) {
        return -1;
    }
    if (send(client_socket, key, MAX, 0) != MAX) {
        return -1;
    }

    if (send(client_socket, len, MAX, 0) != MAX) {
        return -1;
    }
    if (send(client_socket, value, strlen(value) + 1, 0) != strlen(value) + 1) {
        return -1;
    }

    return 0;
}

/******************************************************************************
* get_value()
*
* Arguments: key - name of the key to interact with
*            value - place to store information associated with key
* Returns: int - 0 if operation was successful 
*              - -1 if error sending data to server
*              - -2 if key does not exist
*
* Description:
*   Obtains the information associated with specified key from the local server
*****************************************************************************/
int get_value(char * key, char ** value){

    char *valueX = NULL;
    char len[MAX];
    char error[2];
    int valueSize;

    if (send(client_socket, "2", 2, 0) != 2) {
        return -1;
    }
    send(client_socket, key, MAX, 0);

    /* checks if key exists */
    recv(client_socket, error, 2, 0);
    if (strcmp(error, "1") != 0) {
        recv(client_socket, len, MAX, 0);
        valueSize = atoi(len);

        /* memory allocation for value to be received */
        valueX = (char*) malloc (valueSize * sizeof(char));
        recv(client_socket, valueX, valueSize + 1, 0);
        if (*value == NULL) {
            /* if no value is in store */
            *value = (char *) malloc(strlen(valueX) * sizeof(char*));
            strcpy(*value, valueX);
        } else {
            /* if is an old value is in store, it is replaced */
            *value = realloc(*value, strlen(valueX) * sizeof(char*));
            strcpy(*value, valueX);
        }
        free(valueX);
        return 0;
    }
    return -2;
}

/******************************************************************************
* delete_value()
*
* Arguments: key - name of the key to be deleted
* Returns: int - 0 if operation was successful
*              - -1 if error sending data to server
*              - -2 if key does not exist
*
* Description:
*   Deletes specified key and associated information
*****************************************************************************/
int delete_value(char * key){
    char error[2];

    send(client_socket, "3", 2, 0);
    send(client_socket, key, MAX, 0);

    recv(client_socket, error, 2, 0);
    if (strcmp(error, "0") == 0) {
        return 0;
    }
    return -2;
}

/******************************************************************************
* callbackThread()
*
* Arguments: input - struct with key to be monitorized and callback function
* Returns: (none)
* Description:
*   Thread that watches if any change has been made to specified keys
*****************************************************************************/
void * callbackThread(void * input) {
    char keyName[MAX];
    char threadTerminator[2];
    cb * threadValues = (cb*)input;
    while(1) {
        if (recv(callback_socket, threadTerminator, 2, 0) != 2) {
            free(threadValues);
            break;
        }
        if (strcmp(threadTerminator, "1") == 0) {
            free(threadValues);
            break;
        }
        if (recv(callback_socket, keyName, MAX, 0) != MAX) {
            free(threadValues);
            break;
        }
        /* calls callback function */
        threadValues->callback_function(keyName);
    }
    pthread_exit(NULL);
}

/******************************************************************************
* register_callback()
*
* Arguments: key - name of the key to be monitored
*            callback_function - function to be executed when key information is changed
* Returns: int - 0 if operation was successful
*              - -1 if error sending data to server
*              - -2 if key does not exist
*              - -3 if error creating thread
*
* Description:
*   Creates thread to monitor specified keys
*****************************************************************************/
int register_callback(char * key, void (*callback_function)(char *)){
    
    int err;
    char error[2];

    if (send(client_socket, "4", 2, 0) != 2) {
        return -1;
    }
    send(client_socket, key, MAX, 0);

    /* creates only 1 thread per client */
    if (i == 0) {
        cb * threadValues = (cb*)NULL;

        /* stores callback function in the struct */
        threadValues = (cb*) malloc (sizeof(cb));
        threadValues->callback_function = callback_function;
        
        err = pthread_create(&k1Threads, NULL, callbackThread, (void*)threadValues);
        if (err) {
            printf("Error:unable to create thread, %d\n", err);
            return -3;
        }
    }
    i++;
    /* checks if key exists */
    recv(client_socket, error, 2, 0);
    if (strcmp(error, "1") == 0) {
        return -2;
    }
    return 0;
}

/******************************************************************************
* close_connection()
*
* Arguments: (none)
* Returns: int - 0 if operation was successful
*              - -1 if error sending data to server
*
* Description:
*   Terminates client connection to server
*****************************************************************************/
int close_connection(){
    if (send(client_socket, "5", 2, 0) != 2) {
        return -1;
    }
    pthread_join(k1Threads, NULL);
    close(callback_socket);
    close(client_socket);

    return 0;
}