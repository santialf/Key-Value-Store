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
#include <sys/time.h>
#include <pthread.h>
#include "KVS-lib.h"
#include "list.h"

#define SOCKNAME "/tmp/KVS-LocalServer"
#define SOCKNAME2 "/tmp/KVS-lib"
#define NTHREADS 6
#define MAX 256

typedef struct _cb {
    int socket1;            /* coordinates to position */
    int socket2;
} cb;

int auth_socket;
struct sockaddr_in server_address_auth;

/******************************************************************************
* createGroup()
*
* Arguments: groupID - name of the group to create
* Returns: int - 0 if creation is successful
*              - 1 if group already exists
*
* Description:
*   Creates group on local server and calls functions to allocate and link the struct to the rest
*of the list, also sends group description to add a new element on the auth server
*****************************************************************************/
int createGroup(char * groupID){
    group * groupX = (group*)NULL;
    char authPermission[2];
    char *secret;

    /* checks if group already exists in local server */
    if (verifyGroupName(groupID) != NULL) {
        return 1;
    }
    sendto(auth_socket, "1", 2, 0, (struct sockaddr *) &server_address_auth, (socklen_t )sizeof(server_address_auth));
    sendto(auth_socket, groupID, MAX, 0, (struct sockaddr *) &server_address_auth, (socklen_t )sizeof(server_address_auth));
    /* calls function to generate random key */
    secret = createSecret();
    sendto(auth_socket, secret, MAX, 0, (struct sockaddr *) &server_address_auth, (socklen_t )sizeof(server_address_auth));
    /* checks if group with the same name already exists in auth server */
    recvfrom(auth_socket, authPermission, 2, 0, (struct sockaddr *) &server_address_auth, (socklen_t * )sizeof(struct sockaddr_in));
    if (strcmp(authPermission, "1") == 0) {
        return 1;
    }
    /* memory allocation and inicialization */
    groupX = allocateGroup(groupX);
    groupX->nameGroup = (char*) malloc (strlen(groupID) + 1);
    strcpy(groupX->nameGroup, groupID);
    /* links group to the end of the list */
    insertGroup(groupX);

    printf("Group secret: %s\n", secret);

    free(secret);
    return 0;
}

/******************************************************************************
* deleteGroup()
*
* Arguments: groupID - name of the group to be deleted
* Returns: int - 0 if creation is successful
*              - 1 if group does not exist
* Description:
*   Deletes and removes specified group from the local server list, also sends 
*group description to remove said group from the auth server
*****************************************************************************/
int deleteGroup(char * groupID){

    /* checks if group does not exist */
    if (verifyGroupName(groupID) == NULL) {
        return 1;
    }
    removeGroup(groupID);
    /* sends group information to auth server */
    sendto(auth_socket, "2", 2, 0, (struct sockaddr *) &server_address_auth, (socklen_t )sizeof(server_address_auth));
    sendto(auth_socket, groupID, MAX, 0, (struct sockaddr *) &server_address_auth, (socklen_t )sizeof(server_address_auth));

    return 0;
}

/******************************************************************************
* showGroup()
*
* Arguments: groupID - name of the group to view information
* Returns: int - 0 if operation is successful
*              - 1 if group does not exist
* Description:
*   Prints group secret and its key value pairs, obtains the secret by asking
*the auth server
*****************************************************************************/
int showGroup(char * groupID){
    group * groupX = (group*)NULL;
    key * keyX = (key*)NULL;
    char secret[MAX];
    int i = 1;

    /* checks if group does not exist */
    if ((groupX = verifyGroupName(groupID)) == NULL) {
        return 1;
    }
    /* sends group name and receives associated secret */
    sendto(auth_socket, "3", 2, 0, (struct sockaddr *) &server_address_auth, (socklen_t )sizeof(server_address_auth));
    sendto(auth_socket, groupID, MAX, 0, (struct sockaddr *) &server_address_auth, (socklen_t )sizeof(server_address_auth));
    recvfrom(auth_socket, secret, MAX, 0, (struct sockaddr *) &server_address_auth, (socklen_t * )sizeof(struct sockaddr_in));

    /* prints information */
    printf("secret: %s\n", secret);
    keyX = groupX->nextKey;
    
    if (keyX == NULL) {
        printf("Group has no keys\n");
        return 0;
    }
    while (keyX != NULL) {
        printf("key %d: %s value: %s\n", i, keyX->nameKey, keyX->valueKey);
        keyX = keyX->nextKey;
        i++;
    }
    return 0;
}

/******************************************************************************
* createKey()
*
* Arguments: groupX - group structure where key is created
*            keyID - name of the key
*            keyValue - key information
* Returns: int - 0 if creation is successful
*
* Description:
*   Creates key inside specified group structure and calls functions to allocate
*memory and link it to the end of the key list
*****************************************************************************/
int createKey(group *groupX, char * keyID, char * keyValue) {

    key * keyX = (key*)NULL;

    /* memory allocation and inicialization */
    keyX = allocateKey(keyX);
    keyX->nameKey = (char*) malloc (strlen(keyID) + 1);
    keyX->valueKey = (char*) malloc (strlen(keyValue) + 1);
    strcpy(keyX->nameKey, keyID);
    strcpy(keyX->valueKey, keyValue);
    /* links key to the end of the key list */
    insertKey(groupX, keyX);

    return 0;
}

/******************************************************************************
* deleteKey()
*
* Arguments: groupX - name of the group where the key is located
*            keyID - name of the key to be deleted
* Returns: int - 0 if deletion is successful
* Description:
*   Deletes and removes specified key from the corresponding group key list
*****************************************************************************/
int deleteKey(group *groupX, char * keyID) {

    key * keyX = (key*)NULL;

    keyX = groupX->nextKey;
    /* checks if specified key is the header of the key list */
    if (strcmp(keyX->nameKey, keyID) == 0) {
        groupX->nextKey = freeKey(keyX);
        return 0;
    }
    /* searches the key list for the specified key */       
    while (keyX->nextKey != NULL) {
        if (strcmp(keyX->nextKey->nameKey, keyID) == 0) {
            keyX->nextKey = freeKey(keyX->nextKey);
            break;
        }
        keyX = keyX->nextKey;
    }
    return 0;
}

/******************************************************************************
* createClient()
*
* Arguments: pid - client pid
*            start - struct with start connection time
* Returns: int - 0 if creation is successful
*
* Description:
*   Creates client ID, saves client pid and start connection time, also calls functions to allocate
*memory and link it to the end of the client list
*****************************************************************************/
int createClient(int pid, struct timeval start){
    cStatus * statusX = (cStatus*)NULL;
    /* memory allocation and inicialization */
    statusX = allocateClientStatus(statusX);
    statusX->pid = pid;
    statusX->start = start;
    /* links clientID to the end of the client list */
    insertClientStatus(statusX);

    return 0;
}

/******************************************************************************
* createKeyWatcher()
*
* Arguments: keyID - name of key to be monitorized
*            grouID - name of the group with key
*            socket - socket ID of the client
* Returns: int - 0 if creation is successful
*
* Description:
*   Creates client ID, saves client pid and start connection time, also calls functions to allocate
*memory and link it to the end of the client list
*****************************************************************************/
int createKeyWatcher(char * keyID, char * groupID, int socket) {
    keyWatcher * keyX = (keyWatcher*)NULL;

    /* checks if keyWatch already exists */
    if (verifyKeyWatcher(keyID, groupID) != NULL) {
        return 1;
    }
    /* memory allocation and inicialization */
    keyX = allocateKeyWatcher(keyX);
    keyX->nameGroup = (char*) malloc (strlen(groupID) + 1);
    keyX->nameKey = (char*) malloc (strlen(keyID) + 1);
    keyX->socket = socket;
    strcpy(keyX->nameGroup, groupID);
    strcpy(keyX->nameKey, keyID);
    /* links keyWatcher to the end of the keyWatcher list */
    insertKeyWatcher(keyX);

    return 0;
}

/******************************************************************************
* readKeyboard()
*
* Arguments: (none)
* Returns: (none)
*
* Description:
*   Routine that runs in paralel with the local server, accepts input from the
*keyboard to perform administrative functions
*****************************************************************************/
void *readKeyboard(void *threadid) {
    char str[MAX];
    printf("Commands:\n(1) Create Group\n(2) Delete group\n(3) Show group info\n(4) Show application status\n");
    while(1) {
        /* receives information about what function to execute */
        if (fgets(str, MAX, stdin) != NULL) {
            str[strcspn(str, "\n")] = 0;
            /* create group routine */
            if (strcmp(str, "1") == 0) {
                printf("Type new group name:\n");
                if (fgets(str, MAX, stdin) != NULL) {
                    str[strcspn(str, "\n")] = 0;
                    if (createGroup(str) == 1) {
                        printf("Name already exists...\n");
                    }
                }
            /* delete group routine */
            } else if (strcmp(str, "2") == 0) {
                printf("Type group to be deleted:\n");
                if (fgets(str, MAX, stdin) != NULL) {
                    str[strcspn(str, "\n")] = 0;
                    if (deleteGroup(str) == 1) {
                        printf("Group does not exist...\n");
                    }
                }
            /* show group info routine */
            } else if (strcmp(str, "3") == 0) {
                printf("Type group to show info\n");
                if (fgets(str, MAX, stdin) != NULL) {
                    str[strcspn(str, "\n")] = 0;
                    if (showGroup(str) == 1) {
                        printf("Group does not exist...\n");
                    }
                }
            /* show client information routine */
            } else if (strcmp(str, "4") == 0) {
                str[strcspn(str, "\n")] = 0;
                showApplicationStatus();
            } else {
                printf("Wrong input...\n");
            }
        }
        printf("\nNew command:\n");
    }
    pthread_exit(NULL);
}

/******************************************************************************
* multiThreadClient()
*
* Arguments: input - structure with both client and callback sockets ID
* Returns: (none)
*
* Description:
*   Routine that runs in paralel with the local server, accepts commands from a
*single client to perform client related functions
*****************************************************************************/
void * multiThreadClient(void * input) {
    char group_id[MAX];
    char keyID[MAX];
    char *keyValue = NULL;
    char secret[MAX];
    char functionID[2];
    char len[MAX];
    struct timeval time;
    group * groupX = (group*)NULL;
    key * keyX = (key*)NULL;
    keyWatcher * keyWactherX = (keyWatcher*)NULL;
    cb * socketX = (cb*)input;

    int client_socket, callback_client_socket, clientPid, i = 0, j = 0;

    /* extracts socket ID's from the struct */
    client_socket = socketX->socket1;
    callback_client_socket = socketX->socket2;

    /* receives client pid, registers its connection start time and creates new element on the client list*/
    recv(client_socket, &clientPid, sizeof(int), 0);
    gettimeofday(&time, NULL);
    createClient(clientPid, time);

    /* recieves groupID and secret from the client */
    recv(client_socket, group_id, MAX, 0);
    recv(client_socket, secret, MAX, 0);
    
    /* verifies with the auth server if the secret corresponds to the provided group */
    sendto(auth_socket, "4", 2, 0, (struct sockaddr *) &server_address_auth, (socklen_t )sizeof(server_address_auth));
    sendto(auth_socket, group_id, MAX, 0, (struct sockaddr *) &server_address_auth, (socklen_t )sizeof(server_address_auth));
    sendto(auth_socket, secret, MAX, 0, (struct sockaddr *) &server_address_auth, (socklen_t )sizeof(server_address_auth));
    recvfrom(auth_socket, functionID, 2, 0, (struct sockaddr *) &server_address_auth, (socklen_t * )sizeof(struct sockaddr_in));

    /* tells client if authentication was successful */
    send(client_socket, functionID, 2, 0);
    if (strcmp(functionID, "0") == 0) {
        /* verifies if group exists on the local server */
        groupX = verifyGroupName(group_id);
        if (groupX != NULL) {
                    
            while(1) {
                /* receives information about what function to execute */
                recv(client_socket, functionID, 2, 0);
                /* put_value routine */
                if (strcmp(functionID, "1") == 0) {
                    recv(client_socket, keyID, MAX, 0);
                    recv(client_socket, len, MAX, 0);
                    int valueSize = atoi(len);
                    keyValue = (char*) malloc (valueSize * sizeof(char));
                    recv(client_socket, keyValue, valueSize + 1, 0);
                    /* checks if key already exists */
                    keyX = verifyKeyName(groupX, keyID);
                    if (keyX == NULL) {
                        createKey(groupX, keyID, keyValue);
                    } else {
                        updateKeyValue(keyX, keyValue);
                        /* checks if key is being monitored by any client */
                        keyWactherX = verifyKeyWatcher(keyID, groupX->nameGroup);
                        if (keyWactherX != NULL) {
                            /* warns the monitoring server that said key information was updated */
                            send(keyWactherX->socket, "0", 2, 0);
                            send(keyWactherX->socket, keyID, MAX, 0);
                        }
                    }
                    free(keyValue);
                }
                /* get_value routine */
                if (strcmp(functionID, "2") == 0) {
                    recv(client_socket, keyID, MAX, 0);
                    /* checks if key exists */
                    keyX = verifyKeyName(groupX, keyID);
                    if (keyX == NULL) {
                        /* tells client key does not exist */
                        send(client_socket, "1", 2, 0);
                    } else {
                        /* sends key information to the server */
                        send(client_socket, "0", 2, 0);
                        sprintf(len, "%ld", strlen(keyX->valueKey));
                        send(client_socket, len, MAX, 0);
                        send(client_socket, keyX->valueKey, strlen(keyX->valueKey) + 1, 0);
                    }
                }
                /* delete_value routine */
                if (strcmp(functionID, "3") == 0) {
                    recv(client_socket, keyID, MAX, 0);
                    /* checks if key exists */
                    keyX = verifyKeyName(groupX, keyID);
                    if (keyX == NULL) {
                        /* tells client key does not exist */
                        send(client_socket, "1", 2, 0);
                    } else {
                        /* deletes key */
                        send(client_socket, "0", 2, 0);
                        deleteKey(groupX, keyID);
                        keyWactherX = verifyKeyWatcher(keyID, groupX->nameGroup);
                        if (keyWactherX != NULL) {
                            /* warns the monitoring server that said key information was updated */
                            send(keyWactherX->socket, "0", 2, 0);
                            send(keyWactherX->socket, keyID, MAX, 0);
                            removeKeyWatcherSingle(keyID, groupX->nameGroup);
                        }
                    }
                }
                /* register_callback routine */
                if (strcmp(functionID, "4") == 0) { 
                    recv(client_socket, keyID, MAX, 0);
                    /* checks if key exists */
                    keyX = verifyKeyName(groupX, keyID);
                    if (keyX == NULL) {
                        /* tells client key does not exist */
                        send(client_socket, "1", 2, 0);
                    } else {
                        /* creates a keyWatcher */
                        send(client_socket, "0", 2, 0);
                        createKeyWatcher(keyID, groupX->nameGroup, callback_client_socket);
                    }
                    j++;
                }
                /* close_connection routine */
                if (strcmp(functionID, "5") == 0) {
                    if (j != 0) {
                        send(callback_client_socket, "1", 2, 0);
                    }
                    /* removes keys being monitored by the client from the keyWatcher list */
                    removeKeyWatcher(callback_client_socket);
                    break;
                }
                i++;
            }
        }
    }
    /* registers time client disconnects from the local server */
    gettimeofday(&time, NULL);
    endClient(clientPid, time);
    /* closes sockets */
    close(client_socket);
    close(callback_client_socket);
    pthread_exit(NULL);
}

int main(){
    
    int error, i = 0;
    pthread_t cThreads[NTHREADS];
    pthread_t threads;
    cb * socketX = (cb*)NULL;
    socketX = (cb*) malloc (sizeof(cb));

    /* creates and binds UNIX server socket */
    struct sockaddr_un server_address;
    memset(&server_address, 0, sizeof(struct sockaddr_un));
    server_address.sun_family = AF_UNIX;
    strcpy(server_address.sun_path, SOCKNAME);

    unlink(SOCKNAME);
    int server_socket;
    
    if ((server_socket = socket(AF_UNIX, SOCK_STREAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    if (bind(server_socket, (struct sockaddr *) &server_address, sizeof(struct sockaddr_un)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    listen(server_socket, 5);
    
    long callback_client_socket;
    long client_socket;

    /* creates the socket to connect with the auth server */
    if ((auth_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_address_auth, 0, sizeof(struct sockaddr_in));
    server_address_auth.sin_family = AF_INET;
    server_address_auth.sin_port = htons(8080);
    server_address_auth.sin_addr.s_addr = INADDR_ANY;
    
    /* starts the keyboard reading routine */
    error = pthread_create(&threads, NULL, readKeyboard, NULL);
    if (error) {
        printf("Error:unable to create thread, %d\n", error);
        exit(-1);
    }

    while(1) {
        /* accepts client socket and callback socket */
        client_socket = accept(server_socket, NULL, NULL);
        callback_client_socket = accept(server_socket, NULL, NULL);
        socketX->socket1 = client_socket;
        socketX->socket2 = callback_client_socket;
        /* starts client thread */
        error = pthread_create(&cThreads[i], NULL, multiThreadClient, (void*)socketX);
        if (error) {
            printf("Error:unable to create thread, %d\n", error);
            exit(-1);
        }
        i++;
    }
    close(server_socket);
    return 0;
}