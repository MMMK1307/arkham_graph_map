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
        return lkInsertStart(NULL, value);
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

bool removeConnection(Location* start, Location* dest) {
    LkNode *curC = start->connections->first;
    if(curC == NULL) {
        printf("\n**The first is missing**\n");
        return false;
    }
    LkNode *prev = NULL;
    while(curC != NULL) {
        Connection* connect = (Connection*) curC->value;
        int d = strcmp(connect->dest->name, dest->name);
        if(d == 0) {
            LkNode* next = curC->next;
            free(curC);
            if(prev == NULL) {
                start->connections->first = NULL;
                start->connections->count--;
                return true;
            }
            prev->next = next;
            start->connections->count--;
            return true;
        }
        prev = curC;
        curC = curC->next;
        break;
    }
    return false;
}

void removeBiConnection(Location* a, Location* b) {
    removeConnection(a, b);
    removeConnection(b, a);
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

typedef struct {
    bool success;
    int totalCost;
    Lk* path;
} Result;

Result* initResult() {
    Result* result = (Result*) malloc(sizeof(Result));
    result->success=false;
    result->totalCost=0;
    result->path = lkinit();
    return result;
}

Result* findPathBad(Location* start, Location* dest, Lk* visited) {
    Result* result = initResult();

    LkNode* curN = start->connections->first;
    while(curN != NULL) {
        Connection* curConnection = (Connection *) curN->value;
        int diff = strcmp(curConnection->dest->name, dest->name);
        printf("\n %s", curConnection->dest->name);

        if(diff == 0) {
            result->success = true;
            return result;
        }

        Location* foud = getByName(visited, curConnection->dest->name);
        if(foud != NULL) {
            curN = curN->next;
            continue;
        }
        lkInsertStart(visited, curConnection->dest);
        Result* partialResult = findPathBad(curConnection->dest, dest, visited);

        if(partialResult->success) {
            return partialResult;
        }

        curN = curN->next;
    }

    return result;
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
    LkNode* curN = map->locations->first;
    while(curN != NULL) {
        if(curN->value == NULL) {
            break;
        }
        Location* location = (Location*) curN->value;
        printf("\n%-20s ", location->name);
        printConnections(location->connections);
        curN = curN->next;
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
            //printf("\norigin: %d destindex: %d, cost: %d", locIndex, destIndex, cost);
            addBiConnection(glocs[locIndex], glocs[destIndex], cost, cost);
        }
    }

    return map;
}

/*
private static void example(Map<String, List<String>> graph) {
    var filaDeBusca = new ArrayList<>(graph.get("Voce"));
    var verificados = new HashSet<String>();
    while (!filaDeBusca.isEmpty()) {
        String first = filaDeBusca.remove(0);
        // Se o nó já foi verificado, pulamos para o próximo
        if (verificados.contains(first)) {
            continue;
        }
        // Lógica de verificação do elemento desejado
        if (first.startsWith("Jon")) {
            System.out.println("Encontrado: " + first);
            break;
        }
        verificados.add(first);
        filaDeBusca.addAll(
            Optional.ofNullable(graph.get(first)).orElseGet(List::of)
        );
    }
}*/

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

void bfs(Graph* map, Location* start, Location* end) {
    Lk* search = cloneConnections(start->connections);
    Lk* verified = lkinit();
    while(search->first!=NULL) {
        LkNode* lkn = lkRemoveFirst(search);
        Location* loc = (Location*) lkn->value;

        printf("\n %s", loc->name);

        if(!lkContainsLoc(verified, loc)) {
            continue;
        }
        if(loc->id == end->id) {
            printf("bfs Found!");
            break;
        }
        lkInsertStart(verified, loc);
        search = cloneConnections(getByName(start->connections, loc->name)->connections);
    }
}

int main() {
    Graph* map = initArkhamMap();
    printGraph(map);

    while(1) {
        char* name = malloc(sizeof(char) * MAXNAME);

        printf("\nStart: ");
        scanf("%s", name);
        Location* start = getByName(map->locations, name);

        printf("\nDest: ");
        scanf("%s", name);
        Location* dest = getByName(map->locations, name);

        printf("\nStart: %s -> Dest: %s", start->name, dest->name);

        bfs(map, start, dest);
    }

    return 0;
}