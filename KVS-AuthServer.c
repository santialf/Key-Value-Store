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
#include "authList.h"

#define SOCKNAME2 "/tmp/KVS-AuthServer"
#define MAX 256

int auth_socket;
struct sockaddr_in server_address_auth;

/******************************************************************************
* createGroup()
*
* Arguments: groupID - name of the group to create
*            secret - group secret
* Returns: int - 0 if creation is successful
*              - 1 if group already exists
*
* Description:
*   Creates group and calls functions to allocate and link the struct to the rest
*of the list
*****************************************************************************/
int createGroup(char * groupID, char * secret){
    groupSecret * groupX = (groupSecret*)NULL;

    /* checks if group already exists */
    if (verifyGroupName(groupID) != NULL) {
        return 1;
    }
    /* memory allocation */
    groupX = allocateGroup(groupX);
    /* inicialization */ 
    groupX->nameGroup = (char*) malloc (strlen(groupID));
    groupX->secret = (char*) malloc (strlen(secret));
    strcpy(groupX->nameGroup, groupID);
    strcpy(groupX->secret, secret);
    /* links group to the end of the list */
    insertGroup(groupX);

    return 0;
}

/******************************************************************************
* deleteGroup()
*
* Arguments: groupID - name of the group to be deleted
* Returns: int - 0 if creation is successful
*              - 1 if group does not exist
* Description:
*   Deletes and removes specified group from the secret list
*****************************************************************************/
int deleteGroup(char * groupID) {

    /* checks if group does not exist */
    if (verifyGroupName(groupID) == NULL) {
        return 1;
    }
    removeGroup(groupID);

    return 0;
}

int main(){

    char group_id[MAX];
    char secret[MAX];
    char functionID[2];

    /* creates and binds INET server socket */
    int auth_socket;
    if ( (auth_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_address_client;
    memset(&server_address_auth, 0, sizeof(server_address_auth));
    memset(&server_address_client, 0, sizeof(server_address_client));
    server_address_auth.sin_family    = AF_INET;
    server_address_auth.sin_addr.s_addr = INADDR_ANY;
    server_address_auth.sin_port = htons(8080);

    if (bind(auth_socket, (const struct sockaddr *)&server_address_auth, sizeof(server_address_auth)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    
    int len;
    len = sizeof(server_address_client);
    groupSecret * groupX = (groupSecret*)NULL;
    /* loop that waits for local servers requests */
    while(1) {
        /* receives information about what function to execute */
        recvfrom(auth_socket, functionID, 2, 0, (struct sockaddr *) &server_address_client, &len);

        /* create group routine */
        if (strcmp(functionID, "1") == 0) {
            recvfrom(auth_socket, group_id, MAX, 0, (struct sockaddr *) &server_address_client, (socklen_t * )sizeof(struct sockaddr_in));
            recvfrom(auth_socket, secret, MAX, 0, (struct sockaddr *) &server_address_client, (socklen_t * )sizeof(struct sockaddr_in));
            if (createGroup(group_id, secret) == 1) {
                sendto(auth_socket, "1", 2, 0, (struct sockaddr *) &server_address_client, (socklen_t )sizeof(server_address_client));
            } else {
                sendto(auth_socket, "0", 2, 0, (struct sockaddr *) &server_address_client, (socklen_t )sizeof(server_address_client));
            }
        }
        /* delete group routine */
        if (strcmp(functionID, "2") == 0) {
            recvfrom(auth_socket, group_id, MAX, 0, (struct sockaddr *) &server_address_client, (socklen_t * )sizeof(struct sockaddr_in));
            deleteGroup(group_id);
        }
        /* replies with group secret routine */
        if (strcmp(functionID, "3") == 0) {
            recvfrom(auth_socket, group_id, MAX, 0, (struct sockaddr *) &server_address_client, (socklen_t * )sizeof(struct sockaddr_in));
            groupX = verifyGroupName(group_id);
            sendto(auth_socket, groupX->secret, MAX, 0, (struct sockaddr *) &server_address_client, (socklen_t )sizeof(server_address_client));
        }
        /* checks if secret corresponds to the group's name */
        if (strcmp(functionID, "4") == 0) {
            recvfrom(auth_socket, group_id, MAX, 0, (struct sockaddr *) &server_address_client, (socklen_t * )sizeof(struct sockaddr_in));
            recvfrom(auth_socket, secret, MAX, 0, (struct sockaddr *) &server_address_client, (socklen_t * )sizeof(struct sockaddr_in));
            groupX = verifyGroupName(group_id);
            if ((groupX == NULL) || (strcmp(groupX->secret, secret) != 0)) {
                sendto(auth_socket, "1", 2, 0, (struct sockaddr *) &server_address_client, (socklen_t )sizeof(server_address_client));
            } else {
                sendto(auth_socket, "0", 2, 0, (struct sockaddr *) &server_address_client, (socklen_t )sizeof(server_address_client));
            }
        }
    }
    close(auth_socket);

    return 0;
}