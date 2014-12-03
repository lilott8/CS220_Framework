#include <deque>
#include <queue>
#include <leebase.h>
#include <claim.h>
#include "hadlock.h"

Hadlock::Hadlock() {
    //LeeBase::LeeBase();
}

Hadlock::Hadlock(Map *m) {
    //LeeBase::LeeBase();
    LeeBase::set_map(m);
}

Hadlock::~Hadlock() {
    //LeeBase::~LeeBase();
}

void Hadlock::start(Route r) {
    LeeBase::start(r);

    claim("Using Hadlock!", kWarning);

    if (is_valid()) {
        LeeBase::clear_queues();
        kWaveFrontPQ = priority_queue<LeeNode, vector<LeeNode>, CompareNodesHadlock>();

        kWaveFrontPQ.push(kSource);
        kWaveFrontSource.push_back(kSource);

        Path *p = new Path();
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

int Hadlock::solve_recursive(int iteration) {

    //claim("size of queue: " + to_string(kWaveFrontPQ.size()), kDebug);
    // Base case 1: Not finding a solution
    if (kWaveFrontPQ.size() < 1) {
        claim("We could not successfully route: "
                + kSource.coords_to_string() + "->"
                + kSink.coords_to_string(), kWarning);
        return iteration;
    }

    // Grab the first record
    LeeNode curr = kWaveFrontPQ.top();
    // pop off the first record
    kWaveFrontPQ.pop();

    // Base case 2: We found the sink
    if (is_sink(curr)) {
        // add the sink to the trace_back
        kTraceBack.push_back(curr);
        kMap->get_map()->at(curr.get_x()).at(curr.get_y())
                ->set_type(LeeNode::NodeType::TRACEBACK);
        claim("We found the sink!", kDebug);
        return iteration;
    }

    // Case 3: We still have places on the map to visit
    /**
    * Check each possibility of the next wavefront
    * TODO: Change the logic that calculates the cost
    */
    vector<LeeNode> adjacent = get_adjacent_coordinates(curr);

    for (int x = 0; x < adjacent.size(); x++) {
        kWaveFrontPQ.push(adjacent.at(x));
        // TODO: Figure out how to iterate PQ's
        kWaveFrontSource.push_back(adjacent.at(x));
    }

    if (iteration % 10 == 0) {
        claim("******************************", kDebug);
        kMap->print_map();
    }
    solve_recursive(iteration + 1);

    // Handle the trace_back generation for the algorithm
    /**
    * TODO: Implement traceback logic for hadlock
    */
    if (kTraceBack.size() > 0 && curr.get_hadlock() <= kTraceBack.back().get_hadlock()
            && is_adjacent(curr, kTraceBack.back())) {
        kTraceBack.push_back(curr);
        kMap->get_map()->at(curr.get_x()).at(curr.get_y())->set_type(LeeNode::NodeType::TRACEBACK);
    }

    return iteration;
}

int Hadlock::solve_recursive_bi_directional(int iteration) {
    return iteration;
}

vector<LeeNode> Hadlock::get_adjacent_coordinates(LeeNode c) {
    vector<LeeNode> results;
    LeeNode temp;

    // We use this quite a bit, so we will save it
    double source_distance = calculate_euclidean_distance(c, kSink);

    // (x, y+1)
    if (is_placeable_router(c.get_x(), c.get_y() + 1)) {
        temp.set_x_coord(c.get_x());
        temp.set_y_coord(c.get_y() + 1);
        if (!LeeBase::is_in_vector(temp)) {
            temp.set_cost(LeeBase::calculate_manhattan_distance(temp, kSource));
            if (calculate_euclidean_distance(temp, kSink) <=
                    source_distance) {
                //claim("Temp: " + to_string(temp.get_x()) + ", " + to_string(temp.get_y()) + ") is closer to sink than our current!", kDebug);
                temp.set_hadlock(c.get_hadlock());
            } else {
                //claim("Temp is not closer to sink", kDebug);
                temp.set_hadlock(c.get_hadlock() + 1);
            }
            kMap->get_map()->at(temp.get_x()).at(temp.get_y())->set_hadlock(temp.get_hadlock());
            kMap->get_map()->at(temp.get_x()).at(temp.get_y())->set_output(temp.get_hadlock());
            results.push_back(temp);
            //claim("Adding (x,y+1): (" + to_string(temp.get_x()) + ", " + to_string(temp.get_y()) + ")", kDebug);
        }
    }
    // (x, y-1)
    if (is_placeable_router(c.get_x(), c.get_y() - 1)) {
        temp.set_x_coord(c.get_x());
        temp.set_y_coord(c.get_y() - 1);
        if (!LeeBase::is_in_vector(temp)) {
            temp.set_cost(calculate_manhattan_distance(temp, kSource));
            if (calculate_euclidean_distance(temp, kSink) <=
                    source_distance) {
                //claim("Temp: " + to_string(temp.get_x()) + ", " + to_string(temp.get_y()) + ") is closer to sink than our current!", kDebug);
                temp.set_hadlock(c.get_hadlock());
            } else {
                //claim("Temp is not closer to sink", kDebug);
                temp.set_hadlock(c.get_hadlock() + 1);
            }
            kMap->get_map()->at(temp.get_x()).at(temp.get_y())->set_hadlock(temp.get_hadlock());
            kMap->get_map()->at(temp.get_x()).at(temp.get_y())->set_output(temp.get_hadlock());
            results.push_back(temp);
            //claim("Adding (x,y-1): (" + to_string(temp.get_x()) + ", " + to_string(temp.get_y()) + ")", kDebug);
        }
    }
    // (x+1, y)
    if (is_placeable_router(c.get_x() + 1, c.get_y())) {
        temp.set_x_coord(c.get_x() + 1);
        temp.set_y_coord(c.get_y());
        if (!LeeBase::is_in_vector(temp)) {
            temp.set_cost(calculate_manhattan_distance(temp, kSource));
            if (calculate_euclidean_distance(temp, kSink) <=
                    source_distance) {
                //claim("Temp: " + to_string(temp.get_x()) + ", " + to_string(temp.get_y()) + ") is closer to sink than our current!", kDebug);
                temp.set_hadlock(c.get_hadlock());
            } else {
                //claim("Temp is not closer to sink", kDebug);
                temp.set_hadlock(c.get_hadlock() + 1);
            }
            kMap->get_map()->at(temp.get_x()).at(temp.get_y())->set_hadlock(temp.get_hadlock());
            kMap->get_map()->at(temp.get_x()).at(temp.get_y())->set_output(temp.get_hadlock());
            results.push_back(temp);
            //claim("Adding (x+1,y): (" + to_string(temp.get_x()) + ", " + to_string(temp.get_y()) + ")", kDebug);
        }
    }
    // (x-1, y)
    if (is_placeable_router(c.get_x() - 1, c.get_y())) {
        temp.set_x_coord(c.get_x() - 1);
        temp.set_y_coord(c.get_y());
        if (!LeeBase::is_in_vector(temp)) {
            temp.set_cost(calculate_manhattan_distance(temp, kSource));
            if (calculate_euclidean_distance(temp, kSink) <=
                    source_distance) {
                //claim("Temp: " + to_string(temp.get_x()) + ", " + to_string(temp.get_y()) + ") is closer to sink than our current!", kDebug);
                temp.set_hadlock(c.get_hadlock());
            } else {
                //claim("Temp is not closer to sink", kDebug);
                temp.set_hadlock(c.get_hadlock() + 1);
            }
            kMap->get_map()->at(temp.get_x()).at(temp.get_y())->set_hadlock(temp.get_hadlock());
            kMap->get_map()->at(temp.get_x()).at(temp.get_y())->set_output(temp.get_hadlock());
            results.push_back(temp);
            //claim("Adding (x+1,y): (" + to_string(temp.get_x()) + ", " + to_string(temp.get_y()) + ")", kDebug);
        }
    }
    return results;
}