#include <vector>
#include <string>
#include "../Headers/leenode.h"
#include "../Headers/Map.h"
#include "../Headers/edge.h"
#include "../Headers/claim.h"

using namespace std;
using namespace Utilities;
using namespace Lee;

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
    claim("Placing blockages", kNote);
    for(int x = 0;x < (int)b.size();x++) {
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
    int y,z;
    Route route;

    for(int x = 0;x < v.size();x++) {

        // Declare the source(s)
        LeeNode temp = LeeNode(v.at(x).source);
        temp.set_type(LeeNode::NodeType::SOURCE);
        temp.set_found_by(LeeNode::FoundBy::FSOURCE);
        //claim("Source: " + temp.to_string(), kDebug);
        //kMap.at(temp.get_x()).at(temp.get_y())->set_type(LeeNode::NodeType::SOURCE);
        route.source = temp;

        // Declare the sink(s)
        temp = LeeNode(v.at(x).sink);
        temp.set_type(LeeNode::NodeType::SINK);
        temp.set_found_by(LeeNode::FoundBy::FSINK);
        //claim("Sink: " + temp.to_string(), kDebug);
        //kMap.at(temp.get_x()).at(temp.get_y())->set_type(LeeNode::NodeType::SINK);
        route.sink = temp;

        // Add our connections
        kConnections.push_back(v.at(x));
        kRoutes.push_back(route);
    }
    return *this;
}

Route Map::get_next_route() {
    if (kRoutes.size() > 0) {
        Route r = kRoutes.front();
        kRoutes.pop_front();
        return r;
    }
}

int Map::get_size_of_routes() {
    return (int) kRoutes.size();
}

void Map::print_connections() {
    string output = "\n";
    for(int x = 0; x < kConnections.size(); x++) {
        output += "Source: " + to_string(kConnections.at(x).source.x)
                + ", " + to_string(kConnections.at(x).source.y) + "\n";
        output += "Sink: " + to_string(kConnections.at(x).sink.x)
                + ", " + to_string(kConnections.at(x).sink.y) + "\n";
        output += "=========================\n";
    }

    claim(output, kDebug);
}

string Map::connection_to_string(Route c) {
    string output = "\n";
        output += "Source: " + c.source.to_string() + "\n";
        output += "Sink: " + c.sink.to_string();
    return output;
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
            /*if (x > 0) {
                Edge* west = new Edge(new_node,temp_row.at(temp_row.size()-1));
                new_node->add_connection(west);
            }
            if (y > 0) {
                Edge* north = new Edge(new_node,kMap.at(y-1).at(x));
                new_node->add_connection(north);
            }*/
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

void Map::zero_map() {
    for(int y = 0; y < kHeight; y++) {
        for(int x = 0; x < kWidth; x++) {
            //if(kMap.at(x).at(y)->get_type() == LeeNode::NodeType::NONE) {
                kMap.at(x).at(y)->set_output(0);
                kMap.at(x).at(y)->set_cost(0);
                kMap.at(x).at(y)->set_leewave(0);
                kMap.at(x).at(y)->set_lee3bitwave(0);
                kMap.at(x).at(y)->set_lee2bitwave(0);
                kMap.at(x).at(y)->set_hadlock(0);
                kMap.at(x).at(y)->set_korn(0);
                kMap.at(x).at(y)->set_ruben(0);
            kMap.at(x).at(y)->set_found_by(LeeNode::FoundBy::FNULL);
            //}
            //if(kMap.at(x).at(y)->get_type() == LeeNode::NodeType::TRACEBACK) {
            //    kMap.at(x).at(y)->set_output(0);
            //    kMap.at(x).at(y)->set_cost(0);
            //    kMap.at(x).at(y)->set_detour(0);
            //    kMap.at(x).at(y)->set_wave(0);
            //}
        }
    }
}

void Map::print_map() {
    string output = "\t";
    for(int x = 0; x < kHeight; x++) {
        output += to_string(x) + "\t";
    }

    int size = (int) output.size();
    output += "\n\t";

    for(int x = 0; x<size;x++) {
            output += "-";
    }

    output += "\n";

    for(int y = 0; y < kHeight; y++) {
        if(y < 10) {
            output += to_string(y) + " |\t";
        } else {
            output += to_string(y) + "|\t";
        }
        for(int x = 0; x < kWidth; x++) {
            if(kMap.at(x).at(y)->get_type() == LeeNode::NodeType::BLOCKED ||
                    kMap.at(x).at(y)->get_type() == LeeNode::NodeType::SINK ||
                    kMap.at(x).at(y)->get_type() == LeeNode::NodeType::SOURCE ||
                    kMap.at(x).at(y)->get_type() == LeeNode::NodeType::TRACEBACK) {
                output += LeeNode::convert_type_to_string(kMap.at(x).at(y)->get_type()) + "\t";
            } else {
                string t = "";
                switch (kMap.at(x).at(y)->get_found_by()) {
                    case LeeNode::FoundBy::FSINK:
                        t = "*";
                        break;
                    case LeeNode::FoundBy::FSOURCE:
                        t = "^";
                        break;
                }
                output += to_string(kMap.at(x).at(y)->get_output()) + t + "\t";
            }
        }
        output += "\n";
    }
    claim("\n" + output, kDebug);
}