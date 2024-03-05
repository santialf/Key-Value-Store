#include <sys/time.h>
/* linked list with the groups on the local server */
typedef struct _group {
    char *nameGroup;
    struct _group *nextGroup;
    struct _key *nextKey;
} group;

/* linked key list of a certain group */
typedef struct _key {
    char *nameKey;
    char *valueKey;
    struct _key *nextKey;
} key;

/* linked client info list */
typedef struct _cStatus {
    int pid;
    struct timeval start;
    struct timeval end;
    struct _cStatus *nexClient;
} cStatus;

/* linked keywatcher list */
typedef struct _keyWatcher {
    int socket;
    char *nameGroup;
    char *nameKey;
    struct _keyWatcher *nextKey;
} keyWatcher;

group *allocateGroup(group *);
group *verifyGroupName(char *);
group *freeGroup(group *);
key * allocateKey(key *);
key *verifyKeyName(group *, char *);
key *freeKey(key *);
cStatus *allocateClientStatus(cStatus *);
keyWatcher *allocateKeyWatcher(keyWatcher *);
keyWatcher *verifyKeyWatcher(char *, char *);
keyWatcher *freeKeyWatcher(keyWatcher *);
void insertGroup(group *);
void insertKey(group *, key *);
void insertClientStatus(cStatus *);
void insertKeyWatcher(keyWatcher *);
void removeGroup(char *);
void removeKeyWatcher(int);
void removeKeyWatcherSingle(char *, char *);
void updateKeyValue(key *, char *);
void endClient(int, struct timeval);
void showApplicationStatus();
void printGroupList();
char *createSecret();