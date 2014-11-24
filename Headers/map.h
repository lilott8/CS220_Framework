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

class Map {
public:
    // Constructors
    Map();
    Map(int, int);
    ~Map();

    // Methods
    vector<vector<LeeNode*> > *get_map();
    Point* get_source_coordinates();
    Point* get_sink_coordinates();
    Map set_blockages(vector<Blocker>);
    Map set_sources_and_sinks(vector<Connection>);
    void print_map();
    void print_connections();
    Connection get_next_connection();
    int get_connections_size();
    string connection_to_string(Connection);
    void add_sinks(vector<LeeNode*>);
    void add_sources(vector<LeeNode*>);

private:
    // Methods
    void initialize_map(int, int);
    // Variables
    vector<vector<LeeNode*> > kMap;
    deque<Connection> kConnections;
    deque<LeeNode*> kSources;
    deque<LeeNode*> kSinks;
    deque<Path> kPaths;
    int kDefaultSize = 10;
    int kHeight;
    int kWidth;
};

#endif  //_MAP_BASE_H_