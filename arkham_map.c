#include <limits.h>
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

Lk* lkinit() {
    Lk* lk = (Lk*) malloc(sizeof(Lk));
    lk->count = 0;
    lk->first = NULL;
    return lk;
}

LkNode* lknodeinit(void* value) {
    LkNode* lkn = (LkNode*) malloc(sizeof(LkNode));
    lkn->value = value;
    lkn->next = NULL;
    return lkn;
}

LkNode* lkInsertStart(Lk* lk, void* value) {
    LkNode* newLkn = lknodeinit(value);
    newLkn->next = lk->first;
    lk->first = newLkn;
    lk->count++;
    return newLkn;
}

LkNode* lkInsertEnd(Lk* lk, void* value) {
    if (lk->first == NULL) {
        return lkInsertStart(lk, value);
    }
    LkNode* newLkn = lknodeinit(value);
    LkNode* cur = lk->first;
    while(cur->next != NULL) {
        cur = cur->next;
    }
    cur->next = newLkn;
    lk->count++;
    return newLkn;
}

LkNode* lkRemoveFirst(Lk* lk) {
    if(lk->first == NULL) {
        return NULL;
    }
    LkNode* start = lk->first;
    lk->first = lk->first->next;
    return start;
}

typedef struct Location {
    int id;
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
    graph->locations = lkinit();
    return graph;
}

Connection* initConnection(Location* dest, uint cost) {
    Connection* connection = (Connection*) malloc(sizeof(Connection));
    connection->dest = dest;
    connection->cost = cost;
    return connection;
}

Lk* cloneConnections(Lk* connections) {
    Lk* clone = (Lk*) malloc(sizeof(Lk));
    for(LkNode* lkn = (LkNode *) connections->first; lkn != NULL; lkn = lkn->next) {
        Connection* baseCon = (Connection*) lkn->value;
        Connection* newCon = initConnection(baseCon->dest, baseCon->cost);
        lkInsertStart(clone, newCon);
        printf("\nnewcon: %s", newCon->dest->name);
    }
    return clone;
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

void addBalanceConnection(Location* start, Location* dest, uint cost) {
    Lk* lk = start->connections;
    Connection* connection = initConnection(dest, cost);
    if (lk->first == NULL ||
        ((Connection *)(lk->first->value))->cost >= cost){
        lkInsertStart(lk, connection);
        return;
    }
    LkNode* newLkn = lknodeinit(connection);
    LkNode* cur;
    for(cur = lk->first;
        cur->next != NULL && ((Connection*)cur->next->value)->cost <= cost;
        cur=cur->next) {
    }

    LkNode* next = cur->next;
    cur->next = newLkn;
    newLkn->next = next;
    lk->count++;
}

void addBalanceBiConnection(Location* a, Location* b, uint abcost, uint bacost) {
    addBalanceConnection(a, b, abcost);
    addBalanceConnection(b, a, bacost);
}

bool removeConnection(Location* start, Location* dest) {
    printf("\n(c: %zu) %s removig %s", start->connections->count, start->name, dest->name);
    if(start->connections->first == NULL) {
        printf("\n**The first is missing**\n");
        return false;
    }
    LkNode *prev = NULL;

    for(LkNode* curN = start->connections->first; curN != NULL; curN = curN->next) {
        Connection* connect = (Connection*) curN->value;

        if(connect->dest->id != dest->id) {
            prev = curN;
            continue;
        }

        start->connections->count--;

        if(prev==NULL) {
            start->connections->first=curN->next;
            return true;
        }

        prev->next = curN->next;
        return true;
    }
    return false;
}

void removeBiConnection(Location* a, Location* b) {
    bool aa = removeConnection(a, b);
    bool ab = removeConnection(b, a);
    printf("(aa)%s (ab)%s", aa ? "True" : "False", ab ? "True" : "False");
}

Location* addLocation(Graph* map, int id, char name[MAXNAME]) {
    Location* location = (Location*) malloc(sizeof(Location));
    location->id = id;
    location->connections = lkinit();
    strncpy(location->name, name, MAXNAME);
    lkInsertStart(map->locations, location);
    return location;
}

Location* getByName(Lk* locations, char* name) {
    LkNode* curN = locations->first;
    while(curN != NULL) {
        Location* location = (Location*) curN->value;
        int diff = strcmp(location->name, name);
        if(diff == 0) {
            return location;
        }
        curN = curN->next;
    }
    return NULL;
}

void printConnections(Lk* connections) {
    LkNode* curN = connections->first;
    while(curN != NULL) {
        if(curN->value == NULL) {
            break;
        }
        Connection* connection = (Connection*) curN->value;
        printf("-(%02d)-> %-20s | ", connection->cost, connection->dest->name);
        curN = curN->next;
    }
}

void printGraph(Graph* map) {
    for(LkNode* lkn = map->locations->first; lkn != NULL; lkn = lkn->next) {
        Location* location = (Location*) lkn->value;
        printf("\n%-20s ", location->name);
        printConnections(location->connections);
        printf("\n");
    }
    printf("\n");
}

#define LOCCOUNT 28
#define CONCOUNT 117

Location* glocs[LOCCOUNT];

Graph* initArkhamMap() {
    Graph* map = initg();

    char names[LOCCOUNT][MAXNAME] = {
        // 0 // 1
        "ProcessingCenter", "ArkhamMainGate",

        // 2 // 3 // 4
        "Bowery", "SubwayEntry1", "Museum",

        // 5
        "ParkRow", "TigerVault", "AceChemicals", "MadHatterHideout",
        // 9
        "HushHideout", "Courthouse", "Church",

        // 12
        "AmusementMile", "EveHideout", "GCPD", "BaneHideout", "AmIdBridge",

        // 17
        "IndustrialDistrict", "ZazzHideout", "SteelMill", "SubwayEntry2",

        // 21
        "Subway",

        // 22
        "UndergroundStreets1", "UndergroundStreets2", "WonderTowerBase", "WonderTowerTop",

        // 26
        "WonderCityEntry", "WonderCity",
    };

    char connects[CONCOUNT] = {
        0,
          1, 10,
          23, 3, 'b',
        1,
          2, 5, 'b',
        2,
          3, 9,
          4, 6,
          5, 10, 'b',
        3,
          21, 50, 'b',
        5,
          6, 50,
          7, 4,
          8, 7,
          9, 10,
          10, 5,
          11, 10, 'b',
        6,
          11, 8, 'b',
        10,
          9, 5,
          11, 8, 'b',
        11,
          12, 8, 'b',
        12,
          13, 8,
          14, 6,
          15, 7,
          16, 7, 'b',
        13,
          14, 7, 'b',
        14,
          15, 5, 'b',
        15,
          16, 8, 'b',
        16,
          17, 6,
          18, 6, 'b',
        17,
          18, 5,
          19, 6,
          20, 8, 'b',
        19,
          20, 5, 'b',
        20,
          21, 50, 'b',
        21,
          22, 5, 'b',
        22,
          24, 30, 'b',
        23,
          24, 15, 'b',
        24,
          25, 60,
          26, 5, 'b',
        26,
          27, 40, 'b',
        'e',
    };

    for(int i = 0; i < LOCCOUNT; i++) {
        Location* loc = addLocation(map, i, names[i]);
        glocs[i] = loc;
    }

    for(int i = 0; i< CONCOUNT; i++) {
        char locIndex = connects[i];
        i++;
        if(locIndex == 'e') {
            break;
        }
        while(connects[i] != 'b') {
            char destIndex = connects[i++];
            char cost = connects[i++];
            // printf("\norigin: %d destindex: %d, cost: %d", locIndex, destIndex, cost);
            // addBiConnection(glocs[locIndex], glocs[destIndex], cost, cost);
            addBalanceBiConnection(glocs[locIndex], glocs[destIndex], cost, cost);
        }
    }

    return map;
}

bool lkContainsLoc(Lk* lk, Location* target) {
    bool found = false;
    for(LkNode* lkn = lk->first; lkn != NULL; lkn = lkn->next) {
        Location* loc = (Location*) lkn->value;
        if(loc->id == target->id) {
            return true;
        }
    } 
    return found;
}

bool lkConnectionContainsLoc(Lk* lk, Location* target) {
    bool found = false;
    for(LkNode* lkn = lk->first; lkn != NULL; lkn = lkn->next) {
        Connection* con = (Connection*) lkn->value;
        if(con->dest->id == target->id) {
            return true;
        }
    } 
    return found;
}

void printq(Lk* q) {
  printf("\n");
  for(LkNode* lkn = q->first; lkn != NULL; lkn = lkn->next) {
    Location* loc = (Location *) lkn->value;
    printf("%s -> ", loc->name);
  }
}

void bfs(Graph* map, Location* start) {
    Lk* q = lkinit();
    Lk* verified = lkinit();
    lkInsertStart(q, start);
    //printf("\nstarting bfs2. Start: %s\n", start->name);

    while(q->first != NULL) {
    printq(q);
    Location* loc = (Location*) lkRemoveFirst(q)->value;

    printf(" %s\n", start->name);

    LkNode* lkn = loc->connections->first;
        while(lkn) {
            Connection* con = (Connection*) lkn->value;
            Location* loc = con->dest;
            if(!lkContainsLoc(verified, loc)) {
                if(loc->id == start->id){
                  lkn = lkn->next;
                  continue;
                }
                lkInsertStart(verified, loc);
                lkInsertStart(q, loc);
            }
          lkn = lkn->next;
        }
    }
}

Connection* getConn(Location* start, Location* dest) {
    for(LkNode* lkn = start->connections->first; lkn != NULL; lkn = lkn->next) {
        Connection* con = lkn->value;
        if(con->dest->id == dest->id) {
            return con;
        }
    }
    return NULL;
}

int getDistance(Location* start, Location* dest) {
    Connection* con = getConn(start, dest);
    if(con == NULL) {
        return 0;
    }
    return con->cost;
}

void printDijk(int dist[], int n) {
    printf("\nDistances!\n");
    for(int i = 0; i< LOCCOUNT; i++) {
        printf("i: %d dist: %d \n", i, dist[i]);
    }
}

int minDistance(int dist[], bool sptSet[]) {
    int min = INT_MAX, min_index;
    for(int v = 0; v < LOCCOUNT; v++) {
        if(sptSet[v] == false && dist[v] <= min) {
            min = dist[v];
            min_index = v;
        }
    }
    return min_index;
}

void printSols(Lk* sols[], int originId, int DestId) {
    printf("\n%s ", glocs[originId]->name);
    int sum = 0;
    for(LkNode* lkn = sols[DestId]->first; lkn != NULL; lkn = lkn->next) {
        Connection* scons = (Connection *) lkn->value;
        printf(" -(%02d)-> %s ", scons->cost, scons->dest->name);
        sum += scons->cost;
    }
    printf("\nTotal Seconds: %d", sum);
    printf("\n");
}

void dijktra(Graph* map, Location* start, Location* dest, Lk** sCons) {
    int dist[LOCCOUNT];

    bool sptSet[LOCCOUNT];

    for(int i = 0; i < LOCCOUNT; i++) {
        dist[i] = INT_MAX;
        sptSet[i] = false;
    }

    dist[start->id] = 0;

    for(int c = 0; c < LOCCOUNT- 1; c++) {
        int u = minDistance(dist, sptSet);
        sptSet[u] = true;
        for(int v = 0; v < LOCCOUNT; v++) {
            if(!sptSet[v] && getDistance(glocs[u], glocs[v])
                && dist[u] != INT_MAX
                && dist[u] + getDistance(glocs[u], glocs[v]) < dist[v]
            ) {
                dist[v] = dist[u] + getDistance(glocs[u], glocs[v]);
                Lk* uCons = sCons[u];
                for(LkNode* lkn = uCons->first; lkn != NULL; lkn = lkn->next) {
                    Connection* con = (Connection*)lkn->value;
                    lkInsertEnd(sCons[v], con);
                }
                lkInsertEnd(sCons[v], getConn(glocs[u], glocs[v]));
            }
        }
    }
}

Location* askForLocation(Graph* map, const char* msg) {
    char* name = malloc(sizeof(char) * MAXNAME);
    printf("\n%s", msg);
    scanf("%s", name);
    Location* loc = getByName(map->locations, name);
    if(loc == NULL) {
        printf("\n**Location Not Found **\n");
    }
    return loc;
}

void pathing(Graph* map) {
    Location* start = askForLocation(map, "Start: ");
    if(start == NULL)
        return;
    Location* dest = askForLocation(map, "Dest: ");
    if(dest == NULL)
        return;

    Lk* sols[LOCCOUNT] = {};

    for(int i = 0; i < LOCCOUNT; i++)
        sols[i] = lkinit();

    dijktra(map, start, dest, sols);

    printSols(sols, start->id, dest->id);

    for(int i = 0; i < LOCCOUNT; i++)
        free(sols[i]);
}

void pbfs(Graph* map) {
    Location* start = askForLocation(map, "Start: ");
    if(start == NULL)
        return;
    bfs(map, start);
}


void removeAllConnections(Location* loc) {
    for(LkNode* lkn = loc->connections->first; lkn != NULL; lkn = lkn->next) {
        Connection* con = (Connection *) lkn->value;
        removeBiConnection(loc, con->dest);
    }
    loc->connections->first = NULL;
}


void removeLocation(Graph* map) {
    Location* loc = askForLocation(map, "\nLocation: ");
    if(loc == NULL) {
        return;
    }
    LkNode *prev = NULL;
    for(LkNode* lkn = map->locations->first; lkn != NULL; lkn = lkn->next) {
        Location* curLoc = ((Location*)lkn->value);
        if(curLoc->id == loc->id) {
            prev->next = lkn->next;
            removeAllConnections(curLoc);
            free(lkn);
            map->locations->count--;
            break;
        }
        prev = lkn;
    }
}

void askRemoveLocation(Graph* map) {
    Location* a = askForLocation(map, "Location A: ");
    if(a == NULL) {
        return;
    }
    Location* b = askForLocation(map, "Location B: ");
    if(b == NULL) {
        return;
    }
    removeBiConnection(a, b);
}

void editMap(Graph* map) {
    int editOp = -1;
    while(editOp != 0) {
        printf("\nOptions: [1] Remove Location [2] Remove Connection [0] ExitEdit \n:");
        scanf("%d", &editOp);
        switch(editOp) {
            case 1:
                removeLocation(map);
                break;
            case 2:
                askRemoveLocation(map);
                break;
        }
    }
}

int main() {
    Graph* map = initArkhamMap();

    int op = -1;

    while(op != 0) {
        printf("\nOptions: [1] Print Map Connections [2] Pathing (dijktra) [3] Pathing? (BFS) [4] Edit [0] Exit \n:");
        scanf("%d", &op);
        switch(op) {
            case 1:
                printGraph(map);
                break;
            case 2:
                pathing(map);
                break;
            case 3:
                pbfs(map);
                break;
            case 4:
                editMap(map);
                break;
        }
    }

    return 0;
}
