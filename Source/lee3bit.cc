#include <deque>
#include "../Headers/leebase.h"
#include "../Headers/claim.h"
#include "../Headers/lee3bit.h"

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

    // TODO: this base case doesn't trigger
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
    vector<LeeNode> adjacent = get_adjacent_coordinates(curr, iteration, LeeNode::FoundBy::FNULL);

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
    // TODO: Get the pathsegment implemented
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
    bool found_intersection = false;

    // Base case 1: Not finding a solution
    if (kWaveFrontSource.size() < 1 && kWaveFrontSink.size() < 1) {
        claim("We could not successfully route: "
                + kSource.coords_to_string() + "->"
                + kSink.coords_to_string(), kWarning);
        return iteration;
    }

    LeeNode curr;
    LeeNode::FoundBy found_by;
    if (iteration % 2 == 0) {
        // Grab the first record
        curr = kWaveFrontSource.front();
        // pop off the first record
        kWaveFrontSource.pop_front();

        //claim("Source Queue size: " + to_string(kWaveFrontSource.size()), kNote);
        //claim("Source is searching: " + curr.to_string(), kNote);
        found_by = LeeNode::FoundBy::FSOURCE;
        if (curr.get_found_by() == LeeNode::FoundBy::FSINK) {
            claim("We have converged!", kWarning);
            found_intersection = true;
            kTraceBackSource.push_back(curr);
            kTraceBackSink.push_back(curr);
        }
    } else {
        // Grab the first record
        curr = kWaveFrontSink.front();
        // pop off the first record
        kWaveFrontSink.pop_front();

        //claim("Sink Queue size: " + to_string(kWaveFrontSink.size()), kNote);
        //claim("Sink is searching: " + curr.to_string(), kNote);
        found_by = LeeNode::FoundBy::FSINK;
        if (curr.get_found_by() == LeeNode::FoundBy::FSOURCE) {
            claim("We have converged!", kWarning);
            found_intersection = true;
            kTraceBackSink.push_back(curr);
            kTraceBackSource.push_back(curr);
        }
    }

    // Base case 2: We found an intersection of our search
    if (found_intersection) {
        // add the sink to the trace_back
        kMap->get_map()->at(curr.get_x()).at(curr.get_y())->set_type(LeeNode::NodeType::TRACEBACK);
        claim("We found the point of convergence: " + curr.to_string(), kDebug);
        return iteration;
    }

    vector<LeeNode> adjacent = get_adjacent_coordinates(curr, iteration, found_by);

    for (int x = 0; x < adjacent.size(); x++) {
        if (iteration % 2 == 0) {
            kWaveFrontSource.push_back(adjacent.at(x));
        } else {
            kWaveFrontSink.push_back(adjacent.at(x));
        }
    }
    if (iteration % 15 == 0) {
        claim("This ends iteration " + to_string(iteration), kNote);
        kMap->print_map();
        claim("*************************", kNote);
    }

    //claim("=========================", kNote);

    /*if (iteration > 3) {
        claim("Leaving after N iterations", kWarning);
        claim(LeeNode::convert_found_by_to_string(foundBy) + " is searching for: " + curr.to_string(), kWarning);
        return iteration;
    }*/

    solve_recursive_bi_directional(iteration + 1);

    // Handle the trace_back generation for the algorithm
    if (iteration % 2 == 0) {
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
    } else {
        // Handle the trace_back generation for the algorithm
        if (kTraceBackSink.size() > 0) {
            if (kTraceBackSink.back().get_cost() == 1) {
                // Handle the 3->1 hand off
                if (curr.get_cost() == 3 && is_adjacent(curr, kTraceBackSink.back())) {
                    kTraceBackSink.push_back(curr);
                    kMap->get_map()->at(curr.get_x()).at(curr.get_y())->set_type(LeeNode::NodeType::TRACEBACK);
                }
                // Otherwise just decrement as you should
            } else {
                if (curr.get_cost() < kTraceBackSink.back().get_cost() && is_adjacent(curr, kTraceBackSink.back())) {
                    kTraceBackSink.push_back(curr);
                    kMap->get_map()->at(curr.get_x()).at(curr.get_y())->set_type(LeeNode::NodeType::TRACEBACK);
                }
            }
        }
    }
    return iteration;
}

vector<LeeNode> Lee3Bit::get_adjacent_coordinates(LeeNode c, int iteration, LeeNode::FoundBy fb) {
    vector<LeeNode> results;
    LeeNode temp = c;

    // (x, y+1)
    if (is_placeable_router(c.get_x(), c.get_y() + 1, fb)) {
        temp.set_x_coord( c.get_x());
        temp.set_y_coord(c.get_y() + 1);
        //if(temp.get_output() < 1) {
        if (!is_in_vector(temp, fb)) {
            temp = calculate_metric(temp, iteration);
            if (temp.get_found_by() != kMap->get_map()->at(temp.get_x()).at(temp.get_y())->get_found_by()) {
                temp.set_found_by(kMap->get_map()->at(temp.get_x()).at(temp.get_y())->get_found_by());
            }
            results.push_back(temp);
            //claim("Adding (x,y+1): " + temp.to_string(), kDebug);
        }
    }
    // (x, y-1)
    if (is_placeable_router(c.get_x(), c.get_y() - 1, fb)) {
        temp.set_x_coord(c.get_x());
        temp.set_y_coord(c.get_y() - 1);
        //if(temp.get_output() < 1) {
        if (!is_in_vector(temp, fb)) {
            temp = calculate_metric(temp, iteration);
            if (temp.get_found_by() != kMap->get_map()->at(temp.get_x()).at(temp.get_y())->get_found_by()) {
                temp.set_found_by(kMap->get_map()->at(temp.get_x()).at(temp.get_y())->get_found_by());
            }
            results.push_back(temp);
            //claim("Adding (x,y-1): " + temp.to_string(), kDebug);
        }
    }
    // (x+1, y)
    if (is_placeable_router(c.get_x() + 1, c.get_y(), fb)) {
        temp.set_x_coord(c.get_x() + 1);
        temp.set_y_coord(c.get_y());
        //if(temp.get_output() < 1) {
        if (!is_in_vector(temp, fb)) {
            temp = calculate_metric(temp, iteration);
            if (temp.get_found_by() != kMap->get_map()->at(temp.get_x()).at(temp.get_y())->get_found_by()) {
                temp.set_found_by(kMap->get_map()->at(temp.get_x()).at(temp.get_y())->get_found_by());
            }
            results.push_back(temp);
            //claim("Adding (x+1,y): " + temp.to_string(), kDebug);
        }
    }
    // (x-1, y)
    if (is_placeable_router(c.get_x() - 1, c.get_y(), fb)) {
        temp.set_x_coord(c.get_x() - 1);
        temp.set_y_coord(c.get_y());
        //if(temp.get_output() < 1) {
        if (!is_in_vector(temp, fb)) {
            temp = calculate_metric(temp, iteration);
            if (temp.get_found_by() != kMap->get_map()->at(temp.get_x()).at(temp.get_y())->get_found_by()) {
                temp.set_found_by(kMap->get_map()->at(temp.get_x()).at(temp.get_y())->get_found_by());
            }
            results.push_back(temp);
            //claim("Adding (x-1,y): "+temp.to_string(), kDebug);
        }
    }
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

    if (kMap->get_map()->at(temp.get_x()).at(temp.get_y())->get_found_by() == LeeNode::FoundBy::FNULL) {
        kMap->get_map()->at(temp.get_x()).at(temp.get_y())
                ->set_found_by(temp.get_found_by());
    }
    return temp;
}