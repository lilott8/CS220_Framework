#include <deque>
#include <stdlib.h>
#include <cmath>
#include <claim.h>
#include "LeeBase.h"

using namespace Lee;
using namespace Utilities;
using namespace std;

Map *kMap;
deque<LeeNode> kWaveFront;
vector<LeeNode> kTraceBack;

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

void LeeBase::start(Route route) {
    kSink = route.sink;
    kSource = route.source;

    claim("We are starting to run our algorithm", kDebug);
}

void LeeBase::start() {
    //kSink = kMap->get_sink_coordinates();
    //kSource = kMap->get_source_coordinates();
    // init the distances
    //kSink.dist = calculate_manhattan_distance(kSink, kSource);
    //kSource.dist = 0;
    //kSource.detour = 0;

    //printf("Starting to run our algorithm\n");
}

bool LeeBase::is_sink(LeeNode c) {
    return c.get_x() == kSink.get_x() && c.get_y() == kSink.get_y();
}

bool LeeBase::is_sink(int x, int y) {
    return kSink.get_x() == x && kSink.get_y() == y;
}

bool LeeBase::is_source(LeeNode c) {
    return c.get_x() == kSource.get_x() && c.get_y() == kSource.get_y();
}

bool LeeBase::is_source(int x, int y) {
    return kSource.get_x() == x && kSource.get_y() == y;
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
                ->at(c.get_x()).at(c.get_y() + 1)->get_cost());
    }
    // (x, y-1)
    if (is_in_bounds(c.get_x(), c.get_y() - 1)) {
        answer.push_back(kMap->get_map()
                ->at(c.get_x()).at(c.get_y() - 1)->get_cost());
    }
    // (x+1, y)
    if (is_in_bounds(c.get_x() + 1, c.get_y())) {
        answer.push_back(kMap->get_map()
                ->at(c.get_x() + 1).at(c.get_y())->get_cost());
    }
    // (x-1, y)
    if (is_in_bounds(c.get_x() - 1, c.get_y())) {
        answer.push_back(kMap->get_map()
                 ->at(c.get_x() - 1).at(c.get_y())->get_cost());
    }
    claim("Our options are:", kDebug);
    int new_low = 0;
    for (int x = 0; x < answer.size(); x++) {
        if (answer.at(x) >= 1 && answer.at(x) > new_low) {
            claim("Changing new_low from: " + to_string(new_low) + " to "
                    + to_string(answer.at(x)), kDebug);
            new_low = answer.at(x);
        }
        claim(to_string(x) + ": " + to_string(answer.at(x)), kDebug);
    }
    claim("Done with options", kDebug);
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
    // Just for debugging!

    /*if ((delta_x == 0 && delta_y == 1) || (delta_x == 1 && delta_y == 0)) {
        printf("Delta_x: %d, Delta_y: %d\n", delta_x, delta_y);
    } else {
        printf("Delta_x: %d, Delta_y: %d\n", delta_x, delta_y);
        printf("it's greater than one!\n");
    }*/

    //printf("---------------------\n");
    // make sure the result is 1 or less and the delta on x or y is 1
    return result <= 1 && (delta_x == 1 || delta_y == 1);
}

bool LeeBase::is_placeable(LeeNode c) {
    // Order matters here!
    if (c.get_x() > kMap->get_map()->size() - 1 || c.get_x() < 0) {
        return false;
    }
    if (c.get_y() > kMap->get_map()->at(0).size() - 1 || c.get_y() < 0) {
        return false;
    }
    /**
    * TODO: implement the functionality for
    */
    if (kMap->get_map()->at(c.get_x()).at(c.get_y())->get_type() ==
            LeeNode::BLOCKED) {
        return false;
    }
    if (kMap->get_map()->at(c.get_x()).at(c.get_y())->get_type() ==
            LeeNode::SINK) {
        return false;
    }
    if (kMap->get_map()->at(c.get_x()).at(c.get_y())->get_type() ==
            LeeNode::SOURCE) {
        return false;
    }
    if (kMap->get_map()->at(c.get_x()).at(c.get_y())->get_type() !=
            LeeNode::NONE) {
        return false;
    }
    return true;
}

bool LeeBase::is_placeable(int x, int y) {
    return is_placeable(LeeNode(x,y));
}

bool LeeBase::is_in_bounds(int x, int y) {
    return (x < kMap->get_map()->size() && x >= 0) &&
            (y < kMap->get_map()->at(x).size() && y >= 0);
}

bool LeeBase::is_adjacent_to_source(LeeNode c) {
    return calculate_manhattan_distance(c, kSource) == 1;
}

bool LeeBase::is_in_vector(LeeNode c) {
    for (int x = 0; x < kWaveFront.size(); x++) {
        if (kWaveFront.at(x).get_x() == c.get_x() &&
                kWaveFront.at(x).get_y() == c.get_y()) {
            return true;
        }
    }
    return false;
}

bool LeeBase::is_same_coordinate(LeeNode a, LeeNode b) {
    return (a.get_x() == b.get_x()) && (a.get_y() == b.get_y());
}