#include <deque>
#include <stdlib.h>
#include <cmath>
#include <claim.h>
#include "LeeBase.h"

using namespace Lee;
using namespace Utilities;
using namespace std;

LeeBase::LeeBase() {
}

LeeBase::~LeeBase() {

}

LeeBase::LeeBase(Map *m) {
    kMap = m;
}

LeeBase &LeeBase::set_map(Map *m) {
    kMap = m;
    return *this;
}

void LeeBase::enable_intersections() {
    this->kIntersectionEnabled = true;
}

void LeeBase::enable_bi_direction() {
    this->kBiDirectionEnabled = true;
}

void LeeBase::start(Route route) {
    // Reset our queues so that previous entries don't show up!
    clear_queues();
    if(is_valid_placement(route.source)) {
        //claim("Source has a valid placement", kDebug);
        kSource = route.source;
        kMap->get_map()->at(kSource.get_x()).at(kSource.get_y())->set_type(LeeNode::NodeType::SOURCE);
        kValidPlacement = true;
    } else {
        kValidPlacement = false;
    }
    if(is_valid_placement(route.sink)) {
        //claim("Sink has a valid placement", kDebug);
        kSink = route.sink;
        kMap->get_map()->at(kSink.get_x()).at(kSink.get_y())->set_type(LeeNode::NodeType::SINK);
        kValidPlacement = true;
    } else {
        kValidPlacement = false;
    }

    //claim("We are starting to run our algorithm", kDebug);
}

void LeeBase::start() {/** Not Used **/}

bool LeeBase::is_sink(LeeNode c) {
    return kSink.get_coord() == c.get_coord();
}

bool LeeBase::is_sink(int x, int y) {
    LeeNode ln = LeeNode();
    ln.set_coord(x, y);
    return is_sink(ln);
}

bool LeeBase::is_source(LeeNode c) {
    return kSource.get_coord() == c.get_coord();
}

bool LeeBase::is_source(int x, int y) {
    LeeNode ln = LeeNode();
    ln.set_coord(x, y);
    return is_source(ln);
}

int LeeBase::calculate_manhattan_distance(LeeNode a, LeeNode b) {
    int order1, order2;
    order1 = abs(a.get_x() - b.get_x());
    order2 = abs(a.get_y() - b.get_y());
    return order1 + order2;
}

double LeeBase::calculate_euclidean_distance(LeeNode a, LeeNode b) {
    int order1, order2;
    order1 = (a.get_x() - b.get_x()) * (a.get_x() - b.get_x());
    order2 = (a.get_y() - b.get_y()) * (a.get_y() - b.get_y());
    // http://stackoverflow.com/questions/9695329/c-how-to-round-a-double-to-an-int
    return (sqrt(order1 + order2) + .5);
}

int LeeBase::calculate_lees_distance(LeeNode c) {
    // take care of our seed case
    vector<int> answer;
    // (x, y+1)
    if (is_in_bounds(c.get_x(), c.get_y() + 1)) {
        answer.push_back(kMap->get_map()
                ->at(c.get_x()).at(c.get_y() + 1)->get_leewave());
    }
    // (x, y-1)
    if (is_in_bounds(c.get_x(), c.get_y() - 1)) {
        answer.push_back(kMap->get_map()
                ->at(c.get_x()).at(c.get_y() - 1)->get_leewave());
    }
    // (x+1, y)
    if (is_in_bounds(c.get_x() + 1, c.get_y())) {
        answer.push_back(kMap->get_map()
                ->at(c.get_x() + 1).at(c.get_y())->get_leewave());
    }
    // (x-1, y)
    if (is_in_bounds(c.get_x() - 1, c.get_y())) {
        answer.push_back(kMap->get_map()
                 ->at(c.get_x() - 1).at(c.get_y())->get_leewave());
    }
    //claim("Our options are:", kDebug);
    int new_low = 0;
    for (int x = 0; x < answer.size(); x++) {
        if (answer.at(x) >= 1 && answer.at(x) > new_low) {
            //claim("Changing new_low from: " + to_string(new_low) + " to "
            //        + to_string(answer.at(x)), kDebug);
            new_low = answer.at(x);
        }
        //claim(to_string(x) + ": " + to_string(answer.at(x)), kDebug);
    }
    //claim("Done with options", kDebug);
    //return min_element(begin(answer), end(answer)) + 1;
    return new_low + 1;
}

bool LeeBase::is_adjacent(LeeNode a, LeeNode b) {
    if (a.get_cost() == 0) {
        return true;
    }
    int delta_x, delta_y;
    double result = calculate_manhattan_distance(a, b);

    // Calculate the deltas between x and y,
    // this will prevent the diagonal jumping
    delta_x = abs(a.get_x() - b.get_x());
    delta_y = abs(a.get_y() - b.get_y());
    return result <= 1 && (delta_x == 1 || delta_y == 1);
}


bool LeeBase::is_placeable_router(LeeNode c, LeeNode::FoundBy fb) {
    if(this->kIntersectionEnabled) {
        if (!this->kBiDirectionEnabled) {
            return is_placeable_intersection(c.get_x(), c.get_y());
        } else {
            return is_placeable_intersection_bd(c.get_x(), c.get_y(), fb);
        }
    } else {
        if (!this->kBiDirectionEnabled) {
            return is_placeable_no_intersection(c.get_x(), c.get_y());
        } else {
            return is_placeable_no_intersection_bd(c.get_x(), c.get_y(), fb);
        }
    }
}

bool LeeBase::is_placeable_router(int x, int y) {
    is_placeable_router(x, y, LeeNode::FoundBy::FNULL);
}

bool LeeBase::is_placeable_router(LeeNode c) {
    is_placeable_router(c, LeeNode::FoundBy::FNULL);
}

bool LeeBase::is_placeable_router(int x, int y, LeeNode::FoundBy fb) {
    if(this->kIntersectionEnabled) {
        if (!this->kBiDirectionEnabled) {
            return is_placeable_intersection(x, y);
        } else {
            return is_placeable_intersection_bd(x, y, fb);
        }
    } else {
        if (!this->kBiDirectionEnabled) {
            return is_placeable_no_intersection(x, y);
        } else {
            return is_placeable_no_intersection_bd(x, y, fb);
        }
    }
}

bool LeeBase::is_placeable_no_intersection(LeeNode c) {
    return is_placeable_no_intersection(c.get_x(), c.get_y());
}

bool LeeBase::is_placeable_no_intersection(int x, int y) {
    // Order matters here!
    if (x > kMap->get_map()->size() - 1 || x < 0) {
        return false;
    }
    if (y > kMap->get_map()->at(0).size() - 1 || y < 0) {
        return false;
    }
    // check with the node itself if it's placeable, it knows
    if(!kMap->get_map()->at(x).at(y)->is_placeable()) {
        return false;
    }
    if (kMap->get_map()->at(x).at(y)->get_type() == LeeNode::BLOCKED) {
        return false;
    }
    if (kMap->get_map()->at(x).at(y)->get_type() == LeeNode::SOURCE) {
        return false;
    }
    if(kMap->get_map()->at(x).at(y)->get_type() == LeeNode::TRACEBACK) {
        return false;
    }
    return true;
}

bool LeeBase::is_placeable_intersection(LeeNode c) {
    return is_placeable_intersection(c.get_x(), c.get_y());
}

bool LeeBase::is_placeable_intersection(int x, int y) {
    // Order matters here!
    if (x > kMap->get_map()->size() - 1 || x < 0) {
        return false;
    }
    if (y > kMap->get_map()->at(0).size() - 1 || y < 0) {
        return false;
    }
    // check with the node itself if it's placeable, it knows
    if(!kMap->get_map()->at(x).at(y)->is_placeable()) {
        return false;
    }
    if (kMap->get_map()->at(x).at(y)->get_type() == LeeNode::BLOCKED) {
        return false;
    }
    if (kMap->get_map()->at(x).at(y)->get_type() == LeeNode::SOURCE) {
        return false;
    }
    return true;
}

bool LeeBase::is_placeable_no_intersection_bd(int x, int y, LeeNode::FoundBy fb) {
    // Order matters here!
    if (x > kMap->get_map()->size() - 1 || x < 0) {
        return false;
    }
    if (y > kMap->get_map()->at(0).size() - 1 || y < 0) {
        return false;
    }
    // check with the node itself if it's placeable, it knows
    /*if(!kMap->get_map()->at(x).at(y)->is_placeable()) {
        claim(kMap->get_map()->at(x).at(y)->to_string(), kWarning);
        //return false;
    }*/
    if (kMap->get_map()->at(x).at(y)->get_type() == LeeNode::BLOCKED) {
        return false;
    }
    if (kMap->get_map()->at(x).at(y)->get_type() == LeeNode::SOURCE) {
        return false;
    }
    if (kMap->get_map()->at(x).at(y)->get_type() == LeeNode::TRACEBACK) {
        return false;
    }
    if (kMap->get_map()->at(x).at(y)->get_found_by() == fb) {
        return false;
    }
    return true;
}

bool LeeBase::is_placeable_intersection_bd(int x, int y, LeeNode::FoundBy fb) {
    // Order matters here!
    if (x > kMap->get_map()->size() - 1 || x < 0) {
        return false;
    }
    if (y > kMap->get_map()->at(0).size() - 1 || y < 0) {
        return false;
    }
    // check with the node itself if it's placeable, it knows
    /*if(!kMap->get_map()->at(x).at(y)->is_placeable()) {
        return false;
    }*/
    if (kMap->get_map()->at(x).at(y)->get_type() == LeeNode::BLOCKED) {
        return false;
    }
    if (kMap->get_map()->at(x).at(y)->get_type() == LeeNode::SOURCE) {
        return false;
    }
    if (kMap->get_map()->at(x).at(y)->get_found_by() == fb) {
        return false;
    }
    return true;
}

bool LeeBase::is_valid_placement(LeeNode c) {
    return (kMap->get_map()->at(c.get_x()).at(c.get_y())->get_type() ==
            LeeNode::NONE);
}

bool LeeBase::is_in_bounds(int x, int y) {
    return (x < kMap->get_map()->size() && x >= 0) &&
            (y < kMap->get_map()->at(x).size() && y >= 0);
}

bool LeeBase::is_adjacent_to_source(LeeNode c) {
    return calculate_manhattan_distance(c, kSource) == 1;
}

bool LeeBase::is_in_vector(LeeNode c) {
    for (int x = 0; x < kWaveFrontSource.size(); x++) {
        if (kWaveFrontSource.at(x).get_coord() == c.get_coord()) {
            //claim("We have " + c.to_string() + " in vector", kDebug);
            return true;
        }
    }
    for (int x = 0; x < kWaveFrontSink.size(); x++) {
        if (kWaveFrontSink.at(x).get_coord() == c.get_coord()) {
            return true;
        }
    }
    return false;
}

bool LeeBase::is_in_sink_vector(LeeNode c) {
    for (int x = 0; x < kWaveFrontSink.size(); x++) {
        if (kWaveFrontSink.at(x).get_coord() == c.get_coord()) {
            return true;
        }
    }
    return false;
}

bool LeeBase::is_in_source_vector(LeeNode c) {
    for (int x = 0; x < kWaveFrontSource.size(); x++) {
        if (kWaveFrontSource.at(x).get_coord() == c.get_coord()) {
            //claim("We have " + c.to_string() + " in vector", kDebug);
            return true;
        }
    }
    return false;
}

bool LeeBase::is_same_coordinate(LeeNode a, LeeNode b) {
    return (a.get_coord() == b.get_coord());
}

bool LeeBase::is_valid() {
    return kValidPlacement;
}

string LeeBase::get_path_back() {
    string output = "\n";
    for(int x =0; x< kPathBack.size();x++) {
        Path* p = kPathBack.at(x);
        output += "Route " + to_string(x) + ": ";
        for(int y = 0; y<p->size(); y++) {
            output += "(" + to_string(p->at(y)->get_sink().x) + ", "
                    + to_string(p->at(y)->get_sink().y) + ") -> ("
                    + to_string(p->at(y)->get_source().x) + ", "
                    + to_string(p->at(y)->get_source().y) + ")\t|\t";
        }
        output += "\n";
    }
    return output;
}

void LeeBase::clear_queues() {
    this->kTraceBack.clear();
    this->kWaveFrontSource.clear();
    this->kWaveFrontSink.clear();
}

void LeeBase::set_korn(double d) {
    kKornModifier = d;
}

double LeeBase::get_korn() {
    return kKornModifier;
}
