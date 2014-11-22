#include <vector>
#include <string>
#include <leenode.h>
#include "../Headers/Map.h"
#include "../Headers/edge.h"
#include "../Headers/claim.h"

using namespace std;
using namespace Utilities;

/**
* Call init map with defaults
*/
Map::Map() {
    kHeight = kWidth = kDefaultSize;
    initialize_map(kDefaultSize, kDefaultSize);
}

/**
* Call init map with arguments
*/
Map::Map(int height, int width) {
    kHeight = height;
    kWidth = width;
    initialize_map(height, width);
}

Map::~Map() {

}

Map Map::set_blockages(vector<Blocker> b) {
    int height;
    int width;
    Point start;
    claim("Attempting to place blockages", kNote);
    for(int x = 0;x < (int)b.size();x++) {
        claim("blocker details:", kNote);
        claim("blocker name: " + b.at(x).name, kNote);
        claim("blocker start coords: " + to_string(b.at(x).location.x) +
                ", " + to_string(b.at(x).location.y), kNote);
        claim("blocker height, width: " + to_string(b.at(x).height) +
                ", " + to_string(b.at(x).width), kNote);
        claim("*************************", kNote);
        height = b.at(x).height;
        width = b.at(x).width;
        start = b.at(x).location;

        int h = 0;
        int w = 0;
        while (w < width) {
            while (h < height) {
                kMap.at(start.x + w).at(start.y + h)->set_type(LeeNode::BLOCKED);
                h++;
            }
            h = 0;
            w++;
        }
    }
    return *this;
}

Map Map::set_sources_and_sinks(vector<Connection> v) {
    claim("Map::set_sources_and_sinks is probably a deprecated method...", kWarning);
    return *this;
}

/**
* @method: initialize a map object
* @var1: height of our map
* @var2: width of our map
*/
void Map::initialize_map(int height, int width) {
    for(int y = 0; y < height; y++) {
        vector<LeeNode*> temp_row;
        for(int x = 0; x < width; x++) {
            LeeNode* new_node = new LeeNode(x,y);
            if (x > 0) {
                Edge* west = new Edge(new_node,temp_row.at(temp_row.size()-1));
                new_node->add_connection(west);
            }
            if (y > 0) {
                Edge* north = new Edge(new_node,kMap.at(y-1).at(x));
                new_node->add_connection(north);
            }
            temp_row.push_back(new_node);
        }
        this->kMap.push_back(temp_row);
    }
}

/**
* @method: return our map object
*/
vector< vector<LeeNode*> > *Map::get_map() {
    return &kMap;
}

void Map::print_map() {
    string output = "";
    vector< vector<LeeNode*> >::iterator row;
    vector<LeeNode*>::iterator column;
    for(int y = 0; y < kHeight; y++) {
        for(int x = 0; x < kWidth; x++) {

            output += LeeNode::convert_type_to_string(kMap.at(x).at(y)->get_type()) + "\t";
        }
        output += "\n";
    }
    claim("\n" + output, kDebug);
}