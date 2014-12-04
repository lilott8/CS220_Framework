#include <leebase.h>
#include <leeoriginal.h>
#include <claim.h>

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
        kTraceBack.push_back(curr);
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
    if (kTraceBack.size() > 0 && curr.get_cost() <= kTraceBack.back().get_cost()
            && is_adjacent(curr, kTraceBack.back())) {
        kTraceBack.push_back(curr);
        kMap->get_map()->at(curr.get_x()).at(curr.get_y())->set_type(LeeNode::NodeType::TRACEBACK);
        PathSegment *ps = new PathSegment(curr.get_coord(), kTraceBack.at(kTraceBack.size() - 2).get_coord());
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
    if (kWaveFrontSource.size() < 1 && kWaveFrontSink.size() < 1) {
        claim("We could not successfully route: "
                + kSource.coords_to_string() + "->"
                + kSink.coords_to_string(), kWarning);
        return iteration;
    }

    LeeNode curr;
    LeeNode::FoundBy foundBy;
    if (iteration % 2 == 0) {
        // Grab the first record
        curr = kWaveFrontSource.front();
        // pop off the first record
        kWaveFrontSource.pop_front();

        claim("Source Queue size: " + to_string(kWaveFrontSource.size()), kNote);
        claim("Source is searching: " + curr.to_string(), kNote);
        foundBy = LeeNode::FoundBy::FSOURCE;
        if (curr.get_found_by() == LeeNode::FoundBy::FSINK) {
            claim("We have converged!", kWarning);
            found_intersection = true;
        }
    } else {
        // Grab the first record
        curr = kWaveFrontSink.front();
        // pop off the first record
        kWaveFrontSink.pop_front();

        claim("Sink Queue size: " + to_string(kWaveFrontSink.size()), kNote);
        claim("Sink is searching: " + curr.to_string(), kNote);
        foundBy = LeeNode::FoundBy::FSINK;
        if (curr.get_found_by() == LeeNode::FoundBy::FSOURCE) {
            claim("We have converged!", kWarning);
            found_intersection = true;
        }
    }


    //claim("Curr Coordinates: " + curr.to_string(), kNote);

    // Base case 2: We found an intersection of our search
    if (found_intersection) {
        // add the sink to the trace_back
        kTraceBack.push_back(curr);
        kMap->get_map()->at(curr.get_x()).at(curr.get_y())->set_type(LeeNode::NodeType::TRACEBACK);
        claim("We found the sink: " + curr.to_string(), kDebug);
        return iteration;
    }

    vector<LeeNode> adjacent = get_adjacent_coordinates(curr, iteration, foundBy);

    for (int x = 0; x < adjacent.size(); x++) {
        if (iteration % 2 == 0) {
            kWaveFrontSource.push_back(adjacent.at(x));
        } else {
            kWaveFrontSink.push_back(adjacent.at(x));
        }
        //claim("Adding: " + adjacent.at(x).to_string(), kDebug);
    }
    claim("This ends iteration " + to_string(iteration), kNote);
    claim("*************************", kNote);

    kMap->print_map();
        claim("=========================", kNote);

    if (iteration > 10) {
        claim("Leaving after 10 iterations", kWarning);
        claim(LeeNode::convert_found_by_to_string(foundBy) + " is searching for: " + curr.to_string(), kWarning);
        return iteration;
    }

    solve_recursive_bi_directional(iteration + 1);

    // Handle the trace_back generation for the algorithm
    if (kTraceBack.size() > 0 && curr.get_cost() <= kTraceBack.back().get_cost()
            && is_adjacent(curr, kTraceBack.back())) {
        kTraceBack.push_back(curr);
        kMap->get_map()->at(curr.get_x()).at(curr.get_y())->set_type(LeeNode::NodeType::TRACEBACK);
        PathSegment* ps = new PathSegment(curr.get_coord(), kTraceBack.at(kTraceBack.size()-2).get_coord());
        kPathBack.back()->add_segment(ps);
    }
    return iteration;
}

vector<LeeNode> LeeOriginal::get_adjacent_coordinates(LeeNode c, int iteration, LeeNode::FoundBy fb) {
    vector<LeeNode> results;
    LeeNode temp(c.get_x(), c.get_y());
    if (c.get_found_by() == LeeNode::FoundBy::FNULL) {
        claim("setting the foundby to: " + LeeNode::convert_found_by_to_string(fb) + ", because it's null", kDebug);
        temp.set_found_by(fb);
    }
    claim("=========================", kNote);
    // (x, y+1)
    if (is_placeable_router(c.get_x(), c.get_y() + 1, fb)) {
        temp.set_x_coord(c.get_x());
        temp.set_y_coord(c.get_y() + 1);
        //if(temp.get_output() < 1) {
        if (!is_in_vector(temp)) {
            temp = calculate_metric(temp, iteration, fb);
            results.push_back(temp);
            claim("Adding (x,y+1): " + temp.to_string(), kDebug);
        }
    } else {claim("gac: WE AREN'T PLACING: " + to_string(c.get_x()) + ", " + to_string(c.get_y() + 1), kNote);}
    // (x, y-1)
    if (is_placeable_router(c.get_x(), c.get_y() - 1, fb)) {
        temp.set_x_coord(c.get_x());
        temp.set_y_coord(c.get_y() - 1);
        //if(temp.get_output() < 1) {
        if (!is_in_vector(temp)) {
            temp = calculate_metric(temp, iteration, fb);
            results.push_back(temp);
            claim("Adding (x,y-1): " + temp.to_string(), kDebug);
        }
    } else {claim("gac: WE AREN'T PLACING: " + to_string(c.get_x()) + ", " + to_string(c.get_y() - 1), kNote);}
    // (x+1, y)
    if (is_placeable_router(c.get_x() + 1, c.get_y(), fb)) {
        temp.set_x_coord(c.get_x() + 1);
        temp.set_y_coord(c.get_y());
        //if(temp.get_output() < 1) {
        if (!is_in_vector(temp)) {
            temp = calculate_metric(temp, iteration, fb);
            results.push_back(temp);
            claim("Adding (x+1,y): " + temp.to_string(), kDebug);
        }
    } else {claim("gac: WE AREN'T PLACING: " + to_string(c.get_x() + 1) + ", " + to_string(c.get_y()), kNote);}
    // (x-1, y)
    if (is_placeable_router(c.get_x() - 1, c.get_y(), fb)) {
        temp.set_x_coord(c.get_x() - 1);
        temp.set_y_coord(c.get_y());
        //if(temp.get_output() < 1) {
        if (!is_in_vector(temp)) {
            temp = calculate_metric(temp, iteration, fb);
            results.push_back(temp);
            claim("Adding (x-1,y): " + temp.to_string(), kDebug);
        }
    } else {claim("gac: WE AREN'T PLACING: " + to_string(c.get_x() - 1) + ", " + to_string(c.get_y()), kNote);}
    claim("=========================", kNote);
    return results;
}

LeeNode LeeOriginal::calculate_metric(LeeNode a, int i, LeeNode::FoundBy fb) {
    LeeNode temp = a;

    // Mutate the internal node, so if we add it to the
    // traceback we save the values
    temp.set_leewave(LeeBase::calculate_lees_distance(a));
    temp.set_cost(temp.get_leewave());
    temp.set_output(temp.get_leewave());
    // This should check that we haven't seen this before and that we are actually
    // using the foundby enum.  If both are null than we don't want to change anything
    if (a.get_found_by() == LeeNode::FoundBy::FNULL && fb != LeeNode::FoundBy::FNULL) {
        temp.set_found_by(fb);
    }

    // Mutate the node's data on the map
    kMap->get_map()->at(temp.get_x()).at(temp.get_y())
            ->set_cost(temp.get_leewave());

    kMap->get_map()->at(temp.get_x()).at(temp.get_y())
            ->set_leewave(temp.get_leewave());
    // We must always set the output, even if it gets overridden!
    kMap->get_map()->at(temp.get_x()).at(temp.get_y())
            ->set_output(temp.get_leewave());
    // Who found the node?
    if (a.get_found_by() == LeeNode::FoundBy::FNULL &&
            kMap->get_map()->at(temp.get_x()).at(temp.get_y())->get_found_by() == LeeNode::FoundBy::FNULL) {
        claim("Setting " + temp.coords_to_string() + " to " + LeeNode::convert_found_by_to_string(fb), kDebug);
        kMap->get_map()->at(temp.get_x()).at(temp.get_y())
                ->set_found_by(temp.get_found_by());
    }

    return temp;
}