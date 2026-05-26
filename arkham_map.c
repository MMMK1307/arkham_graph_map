#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#define MAXNAME 49

typedef unsigned int uint;

typedef struct LkNode {
    void* value;
    struct LkNode *next;
} LkNode;

typedef struct Lk {
    LkNode* first;
    size_t count;
} Lk;

Lk* lkinit(void* value) {
    Lk* lk = malloc(sizeof(lkinit));
    lk->count = 0;
    lk->first = NULL;
    return lk;
}

LkNode* lknodeinit(void* value) {
    LkNode* lkn = malloc(sizeof(LkNode));
    lkn->value = value;
    return lkn;
}

LkNode* lkInsertStart(Lk* lk, void* value) {
    LkNode* newLkn = lknodeinit(value);
    newLkn->next = lk->first;
    lk->first = newLkn;
    return newLkn;
}

LkNode* lkInsertEnd(Lk* lk, void* value) {
    if (lk->first == NULL) {
        return lkInsertStart(NULL, value);
    }

    LkNode* newLkn = lknodeinit(value);
    LkNode* cur = lk->first;
    while(cur->next != NULL) {
        cur = cur->next;
    }
    cur->next = newLkn;
    return newLkn;
}

typedef struct Location {
    char name[MAXNAME];
    Lk* connections;
} Location;

typedef struct {
    Location* dest;
    uint cost;
} Connection;

typedef struct {
    Lk* locations;
} Graph;

Graph* initg() {
    Graph* graph = (Graph*) malloc(sizeof(Graph));
    graph->locations = NULL; 
    return graph;
}

Connection* initConnection(Location* dest, uint cost) {
    Connection* connection = (Connection*) malloc(sizeof(Connection));
    connection->dest = dest;
    connection->cost = cost;
    return connection;
}

Connection* addConnection(Location* start, Location* dest, uint cost) {
    Connection* connection = initConnection(dest, cost);
    lkInsertStart(start->connections, connection);
    return connection;
}

void addBiConnection(Location* a, Location* b, uint abcost, uint bacost) {
    addConnection(a, b, abcost);
    addConnection(b, a, bacost);
}

bool removeConnection(Location* start, Location* dest) {
    if(start->connections->first == NULL) {
        return false;
    }
    LkNode *curC = start->connections->first;
    LkNode *prev = NULL;
    while(curC != NULL) {
        Connection* connect = (Connection*) curC;
        int d = strcmp(connect->dest->name, dest->name);
        if(d == 0) {
            LkNode* next = curC->next;
            free(curC);
            if(prev == NULL) {
                return true;
            }
            prev->next = next;
            return true;
        }
        prev = curC;
        curC = curC->next;
    }
    return false;
}

void removeBiConnection(Location* a, Location* b) {
    removeConnection(a, b);
    removeConnection(b, a);
}

Location* addLocation(Graph* map, char* name) {
    Location* location = (Location*) malloc(sizeof(Location));
    strncpy(location->name, name, MAXNAME);
    lkInsertStart(map->locations, location);
    return location;
}


void printConnections(Lk* connections) {
    LkNode* curN = connections->first;

    while(curN != NULL) {

        if(curN->value == NULL) {
            break;
        }

        Connection* connection = (Connection*) curN->value;
        printf("-(%d)-> %s ", connection->cost, connection->dest->name);
        curN = curN->next;
    }
}

void printGraph(Graph* map) {
    LkNode* curN = map->locations->first;
    printf("printing...\n");
    while(curN != NULL) {
        if(curN->value == NULL) {
            break;
        }
        Location* location = (Location*) curN->value;
        printf("%-20s ", location->name);
        printConnections(location->connections);
        curN = curN->next;
        printf("\n");
    }
    printf("\n");
}


int main() {
    Graph* map = initg();
    Location* loc1 = addLocation(map, "Bowery");
    Location* loc2 = addLocation(map, "Industrial Disc");
    Location* loc3 = addLocation(map, "KK");

    addConnection(loc1, loc2, 5);
    addConnection(loc2, loc3, 10);
    addConnection(loc3, loc1, 500);

    printGraph(map);
    printf("asdflkjasdf");
    removeConnection(loc1, loc2);
    printGraph(map);

    return 0;
}