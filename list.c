#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "KVS-lib.h"
#include "list.h"

group * header = (group*)NULL;
cStatus * cHeader = (cStatus*)NULL;
keyWatcher * kHeader = (keyWatcher*)NULL;

/******************************************************************************
* allocateGroup()
*
* Arguments: groupX - struct to be allocated
* Returns: groupX - allocated struct
*
* Description:
*   Allocates memory for a new group's structure
*****************************************************************************/
group *allocateGroup(group *groupX) {
    /* allocation of memory */
    groupX = (group*) malloc (sizeof(group));
    if (groupX == NULL) {
        exit (0);
    }
    /* initialization */
    groupX->nextGroup = NULL;
    groupX->nextKey = NULL;
    
    return(groupX);
}

/******************************************************************************
* allocateKey()
*
* Arguments: keyX - struct to be allocated
* Returns: keyX - allocated struct
*
* Description:
*   Allocates memory for a new key's structure
*****************************************************************************/
key * allocateKey(key * keyX) {
    /* allocation of memory */
    keyX = (key*) malloc (sizeof(key));
    if (keyX == NULL) {
        exit (0);
    }
    /* initialization */
    keyX->nextKey = NULL;
    
    return(keyX);
}

/******************************************************************************
* allocateClientStatus()
*
* Arguments: statusX - struct to be allocated
* Returns: statusX - allocated struct
*
* Description:
*   Allocates memory for a new cStatus's structure
*****************************************************************************/
cStatus *allocateClientStatus(cStatus *statusX) {
    /* allocation of memory */
    statusX = (cStatus*) malloc (sizeof(cStatus));
    if (statusX == NULL) {
        exit (0);
    }
    /* initialization */
    statusX->nexClient = NULL;
    
    return(statusX);
}

/******************************************************************************
* allocateKeyWatcher()
*
* Arguments: keyX - struct to be allocated
* Returns: keyX - allocated struct
*
* Description:
*   Allocates memory for a new keyWatcher's structure
*****************************************************************************/
keyWatcher *allocateKeyWatcher(keyWatcher *keyX) {
    /* allocation of memory */
    keyX = (keyWatcher*) malloc (sizeof(keyWatcher));
    if (keyX == NULL) {
        exit (0);
    }
    /* initialization */
    keyX->nextKey = NULL;
    
    return(keyX);
}

/******************************************************************************
* insertGroup()
*
* Arguments: groupX - group to be linked to the list
* Returns: (none)
*
* Description:
*   Adds new group structure to the end of the linked list
*****************************************************************************/
void insertGroup(group *groupX) {
    group * groupY = (group*)NULL;

    /* checks if header doesnt exist */
    if (header == NULL) {
        header = groupX;
        return;
    }
    groupY = header;
    /* finds the end of the linked list */
    while (groupY->nextGroup != NULL) {
        groupY = groupY->nextGroup;
    }
    /* inserts group */  
    groupY->nextGroup = groupX;
    return;
}

/******************************************************************************
* insertKey()
*
* Arguments: groupX - group where key is located
*            keyX - key to be linked to the key list
* Returns: (none)
*
* Description:
*   Adds new key structure to the end of the group's key list
*****************************************************************************/
void insertKey(group *groupX, key *keyX) {
    key * keyY = (key*)NULL;

    /* checks if header of the key list doesnt exist */
    if (groupX->nextKey == NULL) {
        groupX->nextKey = keyX;
        return;
    }
    keyY = groupX->nextKey;
    /* finds the end of the key list */
    while (keyY->nextKey != NULL) {
        keyY = keyY->nextKey;
    }
    /* inserts key */ 
    keyY->nextKey = keyX;
    return;
}

/******************************************************************************
* insertClientStatus()
*
* Arguments: statusX - client info to be linked to the cStatus list
* Returns: (none)
*
* Description:
*   Adds new client info structure to the end of the cStatus list
*****************************************************************************/
void insertClientStatus(cStatus * statusX) {
    cStatus * statusY = (cStatus*)NULL;

    /* checks if header of the cStatus list doesnt exist */
    if (cHeader == NULL) {
        cHeader = statusX;
        return;
    }
    statusY = cHeader;
    /* finds the end of the cStatus list */
    while (statusY->nexClient != NULL) {
        statusY = statusY->nexClient;
    }   
    /* inserts client info */  
    statusY->nexClient = statusX;

    return;
}

/******************************************************************************
* insertKeyWatcher()
*
* Arguments: keyX - monitored key to be linked to the keyWatchers list
* Returns: (none)
*
* Description:
*   Adds new key structure to the end of the keyWatchers list, to be monitorized
*****************************************************************************/
void insertKeyWatcher(keyWatcher *keyX) {
    keyWatcher * keyY = (keyWatcher*)NULL;

    /* checks if header of the keyWatchers list doesnt exist */
    if (kHeader == NULL) {
        kHeader = keyX;
        return;
    }
    keyY = kHeader;
    /* finds the end of the keyWatchers list */
    while (keyY->nextKey != NULL) {
        keyY = keyY->nextKey;
    }
    /* inserts key */ 
    keyY->nextKey = keyX;
    
    return;
}

/******************************************************************************
* verifyGroupName()
*
* Arguments: groupID - group name
* Returns: groupY - pointer to structure with the same name
*          NULL - if there's no structure with the same name
*
* Description:
*   Goes through the list in search for a group with the argument's name
*****************************************************************************/
group *verifyGroupName(char *groupID) {

    group * groupY = (group*)NULL;
    groupY = header;
    /* checks if there's no list */
    if (groupY == NULL) {
        return (NULL);
    }
    /* checks if the wanted group is the header */
    if (strcmp(groupY->nameGroup, groupID) == 0) {
        return (groupY);
    }
    /* searches the list for the specified group */
    while (groupY->nextGroup != NULL) {
        if (strcmp(groupY->nextGroup->nameGroup, groupID) == 0) {
            return (groupY->nextGroup);
        }
        groupY = groupY->nextGroup;
    }
    
    return(NULL);
}

/******************************************************************************
* verifyKeyName()
*
* Arguments: groupX - group struct with key
*            keyID - key name to find
* Returns: keyY - pointer to structure with the same name
*          NULL - if there's no structure with the same name
*
* Description:
*   Goes through the key list in search for a key with the argument's name
*****************************************************************************/
key *verifyKeyName(group *groupX, char *keyID) {
    key * keyY = (key*)NULL;
    keyY = groupX->nextKey;
    /* checks if there's no key list */
    if (keyY == NULL) {
        return (NULL);
    }
    /* checks if the wanted key is the header */
    if (strcmp(keyY->nameKey, keyID) == 0) {
        return (keyY);
    }
    /* searches the key list for the specified key */
    while (keyY->nextKey != NULL) {
        if (strcmp(keyY->nextKey->nameKey, keyID) == 0) {
            return (keyY->nextKey);
        }
        keyY = keyY->nextKey;
    }
    return(NULL);
}

/******************************************************************************
* verifyKeyWatcher()
*
* Arguments: keyID - key name
*            groupID - group name
* Returns: keyY - pointer to structure with the same key name and same group name
*          NULL - if there's no structure with the same key name and same group name
*
* Description:
*   Goes through the keyWatchers list in search for a key with the argument's names
*****************************************************************************/
keyWatcher *verifyKeyWatcher(char *keyID, char *groupID) {

    keyWatcher * keyY = (keyWatcher*)NULL;
    keyY = kHeader;
    /* checks if there's no keyWatchers list */
    if (keyY == NULL) {
        return (NULL);
    }
    /* checks if the wanted key is the header */
    if ((strcmp(keyY->nameKey, keyID) == 0) && (strcmp(keyY->nameGroup, groupID) == 0)) {
        return (keyY);
    }
    /* searches the keyWatchers list for the specified key */
    while (keyY->nextKey != NULL) {  
        if ((strcmp(keyY->nextKey->nameKey, keyID) == 0) && (strcmp(keyY->nextKey->nameGroup, groupID) == 0)) {
            return (keyY->nextKey);
        }
        keyY = keyY->nextKey;
    }
    
    return(NULL);
}

/******************************************************************************
* removeGroup()
*
* Arguments: groupID - group to be removed from the list
* Returns: (none)
*
* Description:
*   Removes the specified group structure from the linked list
*****************************************************************************/
void removeGroup(char *groupID) {

    group * groupY = (group*)NULL;
    groupY = header;
    /* checks if specified group is the header */
    if (strcmp(groupY->nameGroup, groupID) == 0) {
        header = freeGroup(groupY);
        return;
    }
    /* searches the list for the specified group */
    while (groupY->nextGroup != NULL) {
        if (strcmp(groupY->nextGroup->nameGroup, groupID) == 0) {
            groupY->nextGroup = freeGroup(groupY->nextGroup);
            break;
        }
        groupY = groupY->nextGroup;
    }
    return;
}

/******************************************************************************
* removeKeyWatcher()
*
* Arguments: socket - client socket 
* Returns: (none)
*
* Description:
*   Removes all keys from the keyWatchers list associated with the client socket
*****************************************************************************/
void removeKeyWatcher(int socket) {
    keyWatcher * keyY = (keyWatcher*)NULL;
    keyY = kHeader;

    /* checks if specified keys are the header when removed */
    while (1) {
        if (keyY != NULL) {
            if (keyY->socket == socket) {
                kHeader = freeKeyWatcher(keyY);
            } else {
                break;
            }
        } else {
            return;
        }
        keyY = kHeader;
    }
    /* searches the keyWatchers list for the specified keys */
    while (keyY->nextKey != NULL) {
        if (keyY->nextKey->socket == socket) {
            keyY->nextKey = freeKeyWatcher(keyY->nextKey);
        } else {
            keyY = keyY->nextKey;
        }
    }
    return;
}

/******************************************************************************
* removeKeyWatcher()
*
* Arguments: keyID - key name to be found
*            groupID - group name to be found 
* Returns: (none)
*
* Description:
*   Removes specified key from the keyWatchers list
*****************************************************************************/
void removeKeyWatcherSingle(char * keyID, char * groupID) {
    keyWatcher * keyY = (keyWatcher*)NULL;
    keyY = kHeader;
    int * socketArray;

    /* checks if there's no keyWatchers list */
    if (keyY == NULL) {
        return;
    }
    /* checks if the wanted key is the header */
    if ((strcmp(keyY->nameKey, keyID) == 0) && (strcmp(keyY->nameGroup, groupID) == 0)) {
        kHeader = freeKeyWatcher(keyY);
    } else {
        return;
    } 
    /* searches the keyWatchers list for the specified key */
    while (keyY->nextKey != NULL) {  
        if ((strcmp(keyY->nextKey->nameKey, keyID) == 0) && (strcmp(keyY->nextKey->nameGroup, groupID) == 0)) {
            keyY->nextKey = freeKeyWatcher(keyY->nextKey);
            return;
        }
        keyY = keyY->nextKey;
    }
    return;
}

/******************************************************************************
* freeGroup()
*
* Arguments: groupX - structure to be released
* Returns: groupY - pointer to next group structure in the list
*
* Description:
*   Releases the memory allocated for the specified structure and returns the next
*element in the list
*****************************************************************************/
group *freeGroup(group *groupX) {

    key * keyX = (key*)NULL;
    group * groupY = (group*)NULL;
    groupY = groupX->nextGroup;

    free(groupX->nameGroup);
    /* frees associated keys */
    if (groupX->nextKey != NULL) {
        keyX = freeKey(groupX->nextKey);
        while (keyX != NULL){
            keyX = freeKey(keyX);
        }
    }
    free(groupX);

    return(groupY);
}

/******************************************************************************
* freeKey()
*
* Arguments: keyHeader - structure to be released
* Returns: keyX - pointer to next key structure in the key list
*
* Description:
*   Releases the memory allocated for the specified structure and returns the next
*element in the list
*****************************************************************************/
key *freeKey(key *keyHeader) {
    key * keyX = (key*)NULL;
    keyX = keyHeader->nextKey;

    free(keyHeader->nameKey);
    free(keyHeader->valueKey);
    free(keyHeader);

    return(keyX);
}

/******************************************************************************
* freeKeyWatcher()
*
* Arguments: keyX - structure to be released
* Returns: keyY - pointer to next key structure in the keyWatchers list
*
* Description:
*   Releases the memory allocated for the specified structure and returns the next
*element in the list
*****************************************************************************/
keyWatcher *freeKeyWatcher(keyWatcher *keyX) {

    keyWatcher * keyY = (keyWatcher*)NULL;
    keyY = keyX->nextKey;

    free(keyX->nameGroup);
    free(keyX->nameKey);
    
    free(keyX);

    return(keyY);
}

/******************************************************************************
* updateKeyValue()
*
* Arguments: keyX - structure to be updated
*            keyValue - new key information
* Returns: (none)
*
* Description:
*   Updates the key information associated with the provided key
*****************************************************************************/
void updateKeyValue(key *keyX, char *keyValue) {
    free(keyX->valueKey);
    keyX->valueKey = (char*) malloc (strlen(keyValue));
    strcpy(keyX->valueKey, keyValue);
    
    return;
}

/******************************************************************************
* endClient()
*
* Arguments: pid - client pid
*            end - struct with end connection time
* Returns: (none)
*
* Description:
*   Locates client in the cStatus list and updates the connection end time
*****************************************************************************/
void endClient(int pid, struct timeval end) {
    cStatus * statusX = (cStatus*)NULL;
    statusX = cHeader;

    /* checks if specified client is the header */
    if (statusX->pid == pid) {
        statusX->end = end;
        return;
    }
    /* searches the cStatus list for the specified client */
    while (statusX->nexClient != NULL) {
        if (statusX->nexClient->pid == pid) {
            statusX->nexClient->end = end;
        }
        statusX = statusX->nexClient;
    }
    return;
}

/******************************************************************************
* showApplicationStatus()
*
* Arguments: (none)
* Returns: (none)
*
* Description:
*   Prints every client that has established a connection with the local server
*****************************************************************************/
void showApplicationStatus() {
    cStatus * statusX = (cStatus*)NULL;
    statusX = cHeader;
    int i = 1;
    /* checks if cStatus list exists */
    if (statusX == NULL) {
        return;
    }
    while (statusX != NULL) {
        printf("Client %d\n PID: %d\n Start time: %ld.%ld\n End time: %ld.%ld\n\n", i, statusX->pid, statusX->start.tv_sec, statusX->start.tv_usec, statusX->end.tv_sec, statusX->end.tv_usec);
        statusX = statusX->nexClient;
        i++;
    }
}

/******************************************************************************
* printGroupList()
*
* Arguments: (none)
* Returns: (none)
*
* Description:
*   Prints the list elements in the terminal, mainly used for debugging purposes
*****************************************************************************/
void printGroupList() {
    group * statusX = (group*)NULL;
    statusX = header;
    int i = 1;

    if (statusX == NULL) {
        return;
    }
    while (statusX != NULL) {
        printf("Group %d: %s\n", i, statusX->nameGroup);
        statusX = statusX->nextGroup;
        i++;
    }
}

/******************************************************************************
* createSecret()
*
* Arguments: (none)
* Returns: secret - random generated 4 letter key
*
* Description:
*   Generates a random 4 letter key
*****************************************************************************/
char *createSecret() {
    char *secret;
    int i;

    secret = (char*) malloc (5*sizeof(char));
    /* generate random key */
    srand(time(NULL));
    for (i=0; i<4; i++){
        secret[i] = rand() %26 + 97;
    }
    secret[4] = '\0';
    return(secret);
}