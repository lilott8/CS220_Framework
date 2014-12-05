#include "../Headers/leebase.h"
#include "../Headers/leeoriginal.h"
#include "../Headers/claim.h"

using namespace Lee;
using namespace Utilities;
using namespace std;

LeeOriginal::LeeOriginal() {

}

LeeOriginal::LeeOriginal(Map *m) {
    LeeBase::kMap = m;
}

LeeOriginal::~LeeOriginal() {}

void LeeOriginal::start(Route r) {
    LeeBase::start(r);

    if(is_valid()) {
        kWaveFrontSource.push_front(kSource);

        Path* p = new Path();
        p->set_source(kSource.get_coord());
        p->set_sink(kSink.get_coord());
        kPathBack.push_back(p);

        // Solve the problem!
        if (kBiDirectionEnabled) {
            kWaveFrontSink.push_front(kSink);
            claim("Solving via bidirectional", kWarning);
            solve_recursive_bi_directional(1);
        } else {
            solve_recursive(1);
        }
    } else {
        claim("We cannot route path: " + r.source.coords_to_string()
                + ", " + r.sink.coords_to_string(), kWarning);
    }
}

int LeeOriginal::solve_recursive(int iteration) {
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

    //claim("Curr Coordinates: " + curr.to_string(), kNote);

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
        //claim("Adding: " + adjacent.at(x).to_string(), kDebug);
    }
    //claim("*************************", kNote);
    /*if (iteration % 10 == 0) {
        kMap->print_map();
        claim("=========================", kNote);
    }*/

    solve_recursive(iteration + 1);

    // Handle the trace_back generation for the algorithm
    if (kTraceBackSource.size() > 0 && curr.get_cost() <= kTraceBackSource.back().get_cost()
            && is_adjacent(curr, kTraceBackSource.back())) {
        kTraceBackSource.push_back(curr);
        kMap->get_map()->at(curr.get_x()).at(curr.get_y())->set_type(LeeNode::NodeType::TRACEBACK);
        PathSegment *ps = new PathSegment(curr.get_coord(), kTraceBackSource.at(kTraceBackSource.size() - 2).get_coord());
        kPathBack.back()->add_segment(ps);
    }
    return iteration;
}

/**
* @method:
*   recursive method which searches a map for a sink from both the sink and source
* @int:
*   monitors which iteration we are on
* @return:
*   arbitrary and deprecated
*/
int LeeOriginal::solve_recursive_bi_directional(int iteration) {
    bool found_intersection = false;

    // Base case 1: Not finding a solution
    if (kWaveFrontSource.size() < 1 || kWaveFrontSink.size() < 1) {
        claim("We could not successfully route: "
                + kSource.coords_to_string() + "->"
                + kSink.coords_to_string(), kWarning);
        return iteration;
    }

    LeeNode curr;
    LeeNode::FoundBy found_by;
    if (iteration % 2 == 0) {
        // This catches the case where a vector is empty before the other one.
        if (kWaveFrontSource.size() > 0) {
            // Grab the first record
            curr = kWaveFrontSource.front();
            // pop off the first record
            kWaveFrontSource.pop_front();


            claim("Source Queue size: " + to_string(kWaveFrontSource.size()), kNote);
            //claim("Source is searching: " + curr.to_string(), kNote);
            found_by = LeeNode::FoundBy::FSOURCE;
            if (curr.get_found_by() == LeeNode::FoundBy::FSINK) {
                claim("We have converged!", kWarning);
                found_intersection = true;
                kTraceBackSource.push_back(curr);
                kTraceBackSink.push_back(curr);
            }
        } else {
            // just move to the other search space, we have exhausted our search space
            solve_recursive_bi_directional(iteration + 1);
        }
    } else {
        if (kWaveFrontSink.size() > 0) {
            // Grab the first record
            curr = kWaveFrontSink.front();
            // pop off the first record
            kWaveFrontSink.pop_front();

            claim("Sink Queue size: " + to_string(kWaveFrontSink.size()), kNote);
            //claim("Sink is searching: " + curr.to_string(), kNote);
            found_by = LeeNode::FoundBy::FSINK;
            if (curr.get_found_by() == LeeNode::FoundBy::FSOURCE) {
                claim("We have converged!", kWarning);
                found_intersection = true;
                kTraceBackSink.push_back(curr);
                kTraceBackSource.push_back(curr);
            }
        } else {
            // just move to the other search space, we have exhausted our search space
            solve_recursive_bi_directional(iteration + 1);
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
    if (iteration % 75 == 0) {
        claim("This ends iteration " + to_string(iteration), kNote);
        kMap->print_map();
        claim("*************************", kNote);
    }

    //claim("=========================", kNote);

    /*if (iteration > 3) {
        claim("Leaving after N iterations", kWarning);
        claim(LeeNode::convert_found_by_to_string(found_by) + " is searching for: " + curr.to_string(), kWarning);
        return iteration;
    }*/

    // This covers the appropriate alternating search space
    solve_recursive_bi_directional(iteration + 1);

    // Handle the trace_back generation for the algorithm
    // TODO: Get the pathsegment implemented
    if (iteration % 2 == 0) {
        if (kTraceBackSource.size() > 0 && curr.get_cost() <= kTraceBackSource.back().get_cost()
                && is_adjacent(curr, kTraceBackSource.back())) {
            kTraceBackSource.push_back(curr);
            kMap->get_map()->at(curr.get_x()).at(curr.get_y())->set_type(LeeNode::NodeType::TRACEBACK);
            PathSegment *ps = new PathSegment(curr.get_coord(), kTraceBackSource.at(kTraceBackSource.size() - 2).get_coord());
            kPathBack.back()->add_segment(ps);
        }
    } else {
        if (kTraceBackSink.size() > 0 && curr.get_cost() <= kTraceBackSink.back().get_cost()
                && is_adjacent(curr, kTraceBackSink.back())) {
            kTraceBackSink.push_back(curr);
            kMap->get_map()->at(curr.get_x()).at(curr.get_y())->set_type(LeeNode::NodeType::TRACEBACK);
            PathSegment *ps = new PathSegment(curr.get_coord(), kTraceBackSink.at(kTraceBackSink.size() - 2).get_coord());
            kPathBack.back()->add_segment(ps);
        }
    }
    return iteration;
}

vector<LeeNode> LeeOriginal::get_adjacent_coordinates(LeeNode c, int iteration, LeeNode::FoundBy fb) {
    vector<LeeNode> results;
    LeeNode temp = c;
    //claim("=========================", kNote);
    // (x, y+1)
    if (is_placeable_router(c.get_x(), c.get_y() + 1, fb)) {
        temp.set_x_coord(c.get_x());
        temp.set_y_coord(c.get_y() + 1);
        if (!is_in_vector(temp, fb)) {
            temp = calculate_metric(temp, iteration);
            if (temp.get_found_by() != kMap->get_map()->at(temp.get_x()).at(temp.get_y())->get_found_by()) {
                temp.set_found_by(kMap->get_map()->at(temp.get_x()).at(temp.get_y())->get_found_by());
            }
            results.push_back(temp);
            //claim("Adding (x,y+1): " + kMap->get_map()->at(temp.get_x()).at(temp.get_y())->to_string(), kDebug);
        }
    }
    // (x, y-1)
    if (is_placeable_router(c.get_x(), c.get_y() - 1, fb)) {
        temp.set_x_coord(c.get_x());
        temp.set_y_coord(c.get_y() - 1);
        if (!is_in_vector(temp, fb)) {
            temp = calculate_metric(temp, iteration);
            if (temp.get_found_by() != kMap->get_map()->at(temp.get_x()).at(temp.get_y())->get_found_by()) {
                temp.set_found_by(kMap->get_map()->at(temp.get_x()).at(temp.get_y())->get_found_by());
            }
            results.push_back(temp);
            //claim("Adding (x,y-1): " + kMap->get_map()->at(temp.get_x()).at(temp.get_y())->to_string(), kDebug);
        }
    }
    // (x+1, y)
    if (is_placeable_router(c.get_x() + 1, c.get_y(), fb)) {
        temp.set_x_coord(c.get_x() + 1);
        temp.set_y_coord(c.get_y());
        if (!is_in_vector(temp, fb)) {
            temp = calculate_metric(temp, iteration);
            if (temp.get_found_by() != kMap->get_map()->at(temp.get_x()).at(temp.get_y())->get_found_by()) {
                temp.set_found_by(kMap->get_map()->at(temp.get_x()).at(temp.get_y())->get_found_by());
            }
            results.push_back(temp);
            //claim("Adding (x+1,y): " + kMap->get_map()->at(temp.get_x()).at(temp.get_y())->to_string(), kDebug);
        }
    }
    // (x-1, y)
    if (is_placeable_router(c.get_x() - 1, c.get_y(), fb)) {
        temp.set_x_coord(c.get_x() - 1);
        temp.set_y_coord(c.get_y());
        if (!is_in_vector(temp, fb)) {
            temp = calculate_metric(temp, iteration);
            if (temp.get_found_by() != kMap->get_map()->at(temp.get_x()).at(temp.get_y())->get_found_by()) {
                temp.set_found_by(kMap->get_map()->at(temp.get_x()).at(temp.get_y())->get_found_by());
            }
            results.push_back(temp);
            //claim("Adding (x-1,y): " + kMap->get_map()->at(temp.get_x()).at(temp.get_y())->to_string(), kDebug);
        }
    }
    //claim("=========================", kNote);
    return results;
}

/**
* This actually mutates the value on the map!
*/
LeeNode LeeOriginal::calculate_metric(LeeNode c, int i) {
    LeeNode temp = c;

    // Mutate the internal node, so if we add it to the
    // traceback we save the values
    temp.set_leewave(LeeBase::calculate_lees_distance(c));
    temp.set_cost(temp.get_leewave());
    temp.set_output(temp.get_leewave());

    // Mutate the node's data on the map
    kMap->get_map()->at(temp.get_x()).at(temp.get_y())
            ->set_cost(temp.get_leewave());

    kMap->get_map()->at(temp.get_x()).at(temp.get_y())
            ->set_leewave(temp.get_leewave());
    // We must always set the output, even if it gets overridden!
    kMap->get_map()->at(temp.get_x()).at(temp.get_y())
            ->set_output(temp.get_leewave());

    if (kMap->get_map()->at(temp.get_x()).at(temp.get_y())->get_found_by() == LeeNode::FoundBy::FNULL) {
        kMap->get_map()->at(temp.get_x()).at(temp.get_y())
                ->set_found_by(temp.get_found_by());
    }

    return temp;
}