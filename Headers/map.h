#ifndef _MAP_BASE_H_
#define _MAP_BASE_H_

#pragma once

#include "node.h"
#include "path.h"
#include "problem_object.h"
#include "leenode.h"
#include <vector>

using namespace std;
using namespace Utilities;
using namespace Lee;

struct Route {
    Route(){};
    LeeNode source;
    LeeNode sink;
};

class Map {

public:
    // Constructors
    Map();
    Map(int, int);
    Map(ProblemObject*);
    ~Map();

    // Methods
    vector<vector<LeeNode*> > *get_map();
    Route get_next_route();
    void print_map();
    void print_connections();
    int get_size_of_routes();
    string connection_to_string(Route);
    void add_sinks(vector<LeeNode*>);
    void add_sources(vector<LeeNode*>);
    void zero_map();

private:
    // Methods
    void initialize_map(int, int);
    Map set_blockages(vector<Blocker>);
    Map set_sources_and_sinks(vector<Connection>);
    // Variables
    vector<vector<LeeNode*> > kMap;
    deque<Connection> kConnections;
    deque<Route> kRoutes;
    deque<Path> kPaths;
    int kDefaultSize = 10;
    int kHeight;
    int kWidth;
};

#endif  //_MAP_BASE_H_