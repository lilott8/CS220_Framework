#include <deque>
#include <leebase.h>
#include <claim.h>
#include "lee3bit.h"

using namespace Lee;
using namespace std;
using namespace Utilities;

Lee3Bit::Lee3Bit() {
    //LeeBase::LeeBase();
}

Lee3Bit::Lee3Bit(Map *m) {
    //LeeBase::LeeBase();
    LeeBase::set_map(m);
}

Lee3Bit::~Lee3Bit() {
    //LeeBase::~LeeBase();
}

void Lee3Bit::start(Route r) {
    LeeBase::start(r);

    claim("Using 3bit!", kWarning);

    if(is_valid()) {
        kWaveFrontSource.empty();
        kTraceBackSource.empty();
        kWaveFrontSource.push_front(kSource);

        Path* p = new Path();
        p->set_source(kSource.get_coord());
        p->set_sink(kSink.get_coord());
        kPathBack.push_back(p);

        // Solve the problem!
        if (kBiDirectionEnabled) {
            kWaveFrontSink.push_front(kSink);
            solve_recursive_bi_directional(1);
        } else {
            solve_recursive(1);
        }
    } else {
        claim("We cannot route path: " + r.source.coords_to_string()
                + ", " + r.sink.coords_to_string(), kWarning);
    }
}

int Lee3Bit::solve_recursive(int iteration) {
    //claim("Queue size: " + to_string(kWaveFrontSource.size()), kNote);

    // Base case 1: Not finding a solution
    if (kWaveFrontSource.size() < 1) {
        claim("We could not successfully route: "
                + kSource.coords_to_string() + "->"
                + kSink.coords_to_string(), kWarning);
        return iteration;
    }

    // Grab the first record
    LeeNode curr = kWaveFrontSource.front();
    // pop off the first record
    kWaveFrontSource.pop_front();

    claim("Curr Coordinates: " + curr.to_string(), kNote);

    // Base case 2: We found the sink
    if (is_sink(curr)) {
        // add the sink to the trace_back
        kTraceBackSource.push_back(curr);
        kMap->get_map()->at(curr.get_x()).at(curr.get_y())->set_type(LeeNode::NodeType::TRACEBACK);
        claim("We found the sink: " + curr.to_string(), kDebug);
        return iteration;
    }

    // Case 3: We still have places on the map to visit
    /**
    * Check each possibility of the next wavefront
    */
    vector<LeeNode> adjacent = get_adjacent_coordinates(curr, iteration);

    for (int x = 0; x < adjacent.size(); x++) {
        kWaveFrontSource.push_back(adjacent.at(x));
        claim("Adding: " + adjacent.at(x).to_string(), kDebug);
    }
    if (iteration % 10 == 0) {
        kMap->print_map();
        claim("=========================", kNote);
    }

    solve_recursive(iteration + 1);

    // Handle the trace_back generation for the algorithm
    if (kTraceBackSource.size() > 0) {
        if (kTraceBackSource.back().get_cost() == 1) {
            // Handle the 3->1 hand off
            if (curr.get_cost() == 3 && is_adjacent(curr, kTraceBackSource.back())) {
                kTraceBackSource.push_back(curr);
                kMap->get_map()->at(curr.get_x()).at(curr.get_y())->set_type(LeeNode::NodeType::TRACEBACK);
            }
            // Otherwise just decrement as you should
        } else {
            if (curr.get_cost() < kTraceBackSource.back().get_cost() && is_adjacent(curr, kTraceBackSource.back())) {
                kTraceBackSource.push_back(curr);
                kMap->get_map()->at(curr.get_x()).at(curr.get_y())->set_type(LeeNode::NodeType::TRACEBACK);
            }
        }
    }
    return iteration;
}

int Lee3Bit::solve_recursive_bi_directional(int iteration) {
    return iteration;
}

vector<LeeNode> Lee3Bit::get_adjacent_coordinates(LeeNode c, int iteration) {
    vector<LeeNode> results;
    LeeNode temp;

    // (x, y+1)
    if (is_placeable_router(c.get_x(), c.get_y() + 1)) {
        temp.set_x_coord( c.get_x());
        temp.set_y_coord(c.get_y() + 1);
        //if(temp.get_output() < 1) {
        if (!is_in_vector(temp)) {
            temp = calculate_metric(temp, iteration);
            results.push_back(temp);
            //claim("Adding (x,y+1): " + temp.to_string(), kDebug);
        }
    }// else {printf("WE AREN'T PLACING: %d, %d ON THE QUEUE!!\n", c.get_x(), c.get_y() + 1);}
    // (x, y-1)
    if (is_placeable_router(c.get_x(), c.get_y() - 1)) {
        temp.set_x_coord(c.get_x());
        temp.set_y_coord(c.get_y() - 1);
        //if(temp.get_output() < 1) {
        if (!is_in_vector(temp)) {
            temp = calculate_metric(temp, iteration);
            results.push_back(temp);
            //claim("Adding (x,y-1): " + temp.to_string(), kDebug);
        }
    }// else {printf("WE AREN'T PLACING: %d, %d ON THE QUEUE!!\n", c.get_x(), c.get_y() - 1);}
    // (x+1, y)
    if (is_placeable_router(c.get_x() + 1, c.get_y())) {
        temp.set_x_coord(c.get_x() + 1);
        temp.set_y_coord(c.get_y());
        //if(temp.get_output() < 1) {
        if (!is_in_vector(temp)) {
            temp = calculate_metric(temp, iteration);
            results.push_back(temp);
            //claim("Adding (x+1,y): " + temp.to_string(), kDebug);
        }
    }// else {printf("WE AREN'T PLACING: %d, %d ON THE QUEUE!!\n", c.get_x() + 1, c.get_y());}
    // (x-1, y)
    if (is_placeable_router(c.get_x() - 1, c.get_y())) {
        temp.set_x_coord(c.get_x() - 1);
        temp.set_y_coord(c.get_y());
        //if(temp.get_output() < 1) {
        if (!is_in_vector(temp)) {
            temp = calculate_metric(temp, iteration);
            results.push_back(temp);
            //claim("Adding (x-1,y): "+temp.to_string(), kDebug);
        }
    }// else {printf("WE AREN'T PLACING: %d, %d ON THE QUEUE!!\n", c.get_x() - 1, c.get_y());}
    return results;
}

LeeNode Lee3Bit::calculate_metric(LeeNode a, int iteration) {
    LeeNode temp = a;

    int dist = calculate_lees_distance(temp);

    if (dist % 3 == 0) {
        temp.set_cost(3);
        temp.set_leewave(dist);
        temp.set_output(3);
        temp.set_lee3bitwave(3);
        kMap->get_map()->at(a.get_x()).at(a.get_y())->set_leewave(dist);
        kMap->get_map()->at(a.get_x()).at(a.get_y())->set_lee3bitwave(3);
        kMap->get_map()->at(a.get_x()).at(a.get_y())->set_output(3);
        kMap->get_map()->at(a.get_x()).at(a.get_y())->set_cost(3);
    } else {
        temp.set_cost(dist % 3);
        temp.set_leewave(dist);
        temp.set_lee3bitwave(dist % 3);
        temp.set_output(dist % 3);
        kMap->get_map()->at(a.get_x()).at(a.get_y())->set_leewave(dist);
        kMap->get_map()->at(a.get_x()).at(a.get_y())->set_lee3bitwave(dist % 3);
        kMap->get_map()->at(a.get_x()).at(a.get_y())->set_output(dist % 3);
        kMap->get_map()->at(a.get_x()).at(a.get_y())->set_cost(dist % 3);
    }
    return temp;
}