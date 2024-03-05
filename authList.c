#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "KVS-lib.h"
#include "authList.h"

groupSecret * header = (groupSecret*)NULL;

/******************************************************************************
* allocateGroup()
*
* Arguments: groupX - struct to be allocated
* Returns: groupX - allocated struct
*
* Description:
*   Allocates memory for a new group's structure
*****************************************************************************/
groupSecret *allocateGroup(groupSecret *groupX) {
    /* allocation of memory */
    groupX = (groupSecret*) malloc (sizeof(groupSecret));
    if (groupX == NULL) {
        exit (0);
    }
    /* initialization */
    groupX->nextGroup = NULL;
    
    return(groupX);
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
void insertGroup(groupSecret *groupX) {
    groupSecret * groupY = (groupSecret*)NULL;

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
* removeGroup()
*
* Arguments: groupID - group to be removed from the list
* Returns: (none)
*
* Description:
*   Removes the specified group structure from the linked list
*****************************************************************************/
void removeGroup(char *groupID) {

    groupSecret * groupY = (groupSecret*)NULL;
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
* freeGroup()
*
* Arguments: groupX - structure to be released
* Returns: groupY - pointer to next group structure in the list
*
* Description:
*   Releases the memory allocated for the specified structure and returns the next
*element in the list
*****************************************************************************/
groupSecret *freeGroup(groupSecret *groupX) {

    groupSecret * groupY = (groupSecret*)NULL;
    groupY = groupX->nextGroup;

    free(groupX->nameGroup);
    free(groupX->secret);
    free(groupX);

    return(groupY);
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
groupSecret *verifyGroupName(char *groupID) {

    groupSecret * groupY = (groupSecret*)NULL;
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
* printGroup()
*
* Arguments: (none)
* Returns: (none)
*
* Description:
*   Prints the list elements in the terminal, mainly used for debugging purposes
*****************************************************************************/
void printGroup() {
    groupSecret * groupX = (groupSecret*)NULL;
    groupX = header;

    if (groupX == NULL) {
        return;
    }
    while (groupX != NULL) {
        printf("%s %s\n", groupX->nameGroup, groupX->secret);
        groupX = groupX->nextGroup;
    }
    return;
}