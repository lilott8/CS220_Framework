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

struct Coordinates {
    int x;
    int y;
};

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
    void add_sinks(vector<LeeNode*>);
    void add_sources(vector<LeeNode*>);

    // Variables
private:
    // Methods
    void initialize_map(int, int);
    // Variables
    vector<vector<LeeNode*> > kMap;
    vector<LeeNode*> kSources;
    vector<LeeNode*> kSinks;
    vector<Path> kPaths;
    int kDefaultSize = 10;
    int kHeight;
    int kWidth;
};

#endif  //_MAP_BASE_H_