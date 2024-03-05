/* linked secret group list */
typedef struct _groupSecret {
    char *nameGroup;
    char *secret;
    struct _groupSecret *nextGroup;
} groupSecret;

groupSecret *allocateGroup(groupSecret *groupX);
void insertGroup(groupSecret *groupX);
void removeGroup(char *groupID);
groupSecret *freeGroup(groupSecret *groupX);
groupSecret *verifyGroupName(char *groupID);
void printGroup();