#include "../Headers/leenode.h"
#include "../Headers/leebase.h"
#include "../Headers/claim.h"
#include "../Headers/ruben.h"

using namespace Lee;
using namespace Utilities;
using namespace std;

Ruben::Ruben() {
    //LeeBase::LeeBase();
    kKornModifier = 1;
}

Ruben::Ruben(Map *m, double korn) {
    //LeeBase::LeeBase();
    LeeBase::set_map(m);
    kKornModifier = korn;
}

Ruben::~Ruben() {
    //LeeBase::~LeeBase();
}

void Ruben::start(Route r) {
    LeeBase::start(r);

    // reset the priority queue for our algorithm
    kWaveFrontSourcePQ = priority_queue<LeeNode, vector<LeeNode>, CompareNodesRuben>();
    kWaveFrontSourcePQ.push(kSource);
    kWaveFrontSource.push_back(kSource);
    //kTraceBackSource.push_back(kSource);

    Path *p = new Path();
    p->set_source(kSource.get_coord());
    p->set_sink(kSink.get_coord());
    kPathBack.push_back(p);

    // Solve the problem!
    if (kBiDirectionEnabled) {
        // reset the priority queue for our algorithm
        kWaveFrontSinkPQ = priority_queue<LeeNode, vector<LeeNode>, CompareNodesRuben>();
        kWaveFrontSinkPQ.push(kSink);
        kWaveFrontSink.push_front(kSink);
        solve_recursive_bi_directional(1);
        //kTraceBackSink.push_back(kSink);
        kTraceBackSource.push_back(kSource);
        for(int x = 0; x< kTraceBackSink.size();x++) {
            kTraceBackSource.push_front(kTraceBackSink.at(x));
        }
    } else {
        solve_recursive(1);
    }
    if(kTraceBackSource.size() > 0) {
        create_path_back();
    }}

int Ruben::solve_recursive(int iteration) {

    //printf("size of queue: %lu\n", kWaveFront.size());
    // Base case 1: Not finding a solution
    if (kWaveFrontSourcePQ.size() < 1) {
        claim("We could not successfully route: "
                + kSource.coords_to_string() + "->"
                + kSink.coords_to_string(), kWarning);
        clear_queues();
        return iteration;
    }

    // Grab the first record
    LeeNode curr = kWaveFrontSourcePQ.top();
    // pop off the first record
    kWaveFrontSourcePQ.pop();

    //claim("Curr Coordinates: " + curr.to_string(), kNote);

    // Base case 2: We found the sink
    if (is_sink(curr)) {
        // add the sink to the trace_back
        kTraceBackSource.push_back(curr);
        successful_routing = true;
        return iteration;
    }

    // Case 3: We still have places on the map to visit
    /**
    * Check each possibility of the next wavefront
    */
    vector<LeeNode> adjacent = get_adjacent_coordinates(curr, LeeNode::FoundBy::FNULL);

    for (int x = 0; x < adjacent.size(); x++) {
        kWaveFrontSourcePQ.push(adjacent.at(x));
    }

    solve_recursive(iteration + 1);

    // Handle the trace_back generation for the algorithm
    if (kTraceBackSource.size() > 0 && curr.get_leewave() <= kTraceBackSource.back().get_leewave()
            && is_adjacent(curr, kTraceBackSource.back())) {
        kTraceBackSource.push_back(curr);
        if(kMap->get_map()->at(curr.get_x()).at(curr.get_y())->get_type() != LeeNode::NodeType::SINK &&
                kMap->get_map()->at(curr.get_x()).at(curr.get_y())->get_type() != LeeNode::NodeType::SOURCE) {
            kMap->get_map()->at(curr.get_x()).at(curr.get_y())->set_type(LeeNode::NodeType::TRACEBACK);
        }
    }
    return iteration;
}

int Ruben::solve_recursive_bi_directional(int iteration) {
    bool found_intersection = false;
    //claim("wave front sizes: source: " + to_string(kWaveFrontSourcePQ.size()) + "\t sink: " + to_string(kWaveFrontSinkPQ.size()), kNote);

    // Base case 1: Not finding a solution
    if (kWaveFrontSourcePQ.size() < 1 || kWaveFrontSinkPQ.size() < 1) {
        claim("We could not successfully route: "
                + kSource.coords_to_string() + "->"
                + kSink.coords_to_string(), kWarning);
        clear_queues();
        return iteration;
    }

    LeeNode curr;
    LeeNode::FoundBy found_by;
    if (iteration % 2 == 0) {
        // Grab the first record
        curr = kWaveFrontSourcePQ.top();
        // pop off the first record
        kWaveFrontSourcePQ.pop();

        //claim("Source Queue size: " + to_string(kWaveFrontSourcePQ.size()), kNote);
        //claim("Source: " + curr.to_string(), kNote);
        found_by = LeeNode::FoundBy::FSOURCE;
        if (curr.get_found_by() == LeeNode::FoundBy::FSINK || is_sink(curr)) {
            //claim("We have converged! source", kWarning);
            found_intersection = true;
            kFoundByFlag = LeeNode::FoundBy::FSOURCE;
            kTraceBackSource.push_back(curr);
            kTraceBackSink.push_back(curr);
        }
    } else {
        // Grab the first record
        curr = kWaveFrontSinkPQ.top();
        // pop off the first record
        kWaveFrontSinkPQ.pop();

        //claim("Sink Queue size: " + to_string(kWaveFrontSinkPQ.size()), kNote);
        //claim("Sink  : " + curr.to_string(), kNote);
        found_by = LeeNode::FoundBy::FSINK;
        if (curr.get_found_by() == LeeNode::FoundBy::FSOURCE || is_source(curr)) {
            //claim("We have converged! sink", kWarning);
            found_intersection = true;
            kFoundByFlag = LeeNode::FoundBy::FSINK;
            kTraceBackSink.push_back(curr);
            kTraceBackSource.push_back(curr);
        }
    }

    // Base case 2: We found an intersection of our search
    if (found_intersection) {
        // add the sink to the trace_back
        kMap->get_map()->at(curr.get_x()).at(curr.get_y())->set_type(LeeNode::NodeType::TRACEBACK);
        successful_routing = true;
        //claim("We found the point of convergence: " + curr.to_string(), kDebug);
        return iteration;
    }

    vector<LeeNode> adjacent = get_adjacent_coordinates(curr, found_by);

    for (int x = 0; x < adjacent.size(); x++) {
        if (iteration % 2 == 0) {
            kWaveFrontSourcePQ.push(adjacent.at(x));
            kWaveFrontSource.push_back(adjacent.at(x));
        } else {
            kWaveFrontSinkPQ.push(adjacent.at(x));
            kWaveFrontSink.push_back(adjacent.at(x));
        }
    }

    solve_recursive_bi_directional(iteration + 1);

    // Handle the trace_back generation for the algorithm
    if (iteration % 2 == 0) {
        if (kTraceBackSource.size() > 0 && curr.get_leewave() <= kTraceBackSource.back().get_leewave()
                && is_adjacent(curr, kTraceBackSource.back()) && curr.get_found_by() == kFoundByFlag) {
            kTraceBackSource.push_back(curr);
            if(kMap->get_map()->at(curr.get_x()).at(curr.get_y())->get_type() != LeeNode::NodeType::SINK &&
                    kMap->get_map()->at(curr.get_x()).at(curr.get_y())->get_type() != LeeNode::NodeType::SOURCE) {
                kMap->get_map()->at(curr.get_x()).at(curr.get_y())->set_type(LeeNode::NodeType::TRACEBACK);
            }
        }
    } else {
        if (kTraceBackSink.size() > 0 && curr.get_leewave() <= kTraceBackSink.back().get_leewave()
                && is_adjacent(curr, kTraceBackSink.back()) && curr.get_found_by() == kFoundByFlag) {
            kTraceBackSink.push_back(curr);
            if(kMap->get_map()->at(curr.get_x()).at(curr.get_y())->get_type() != LeeNode::NodeType::SINK &&
                    kMap->get_map()->at(curr.get_x()).at(curr.get_y())->get_type() != LeeNode::NodeType::SOURCE) {
                kMap->get_map()->at(curr.get_x()).at(curr.get_y())->set_type(LeeNode::NodeType::TRACEBACK);
            }
        }
    }
    return iteration;
}

vector<LeeNode> Ruben::get_adjacent_coordinates(LeeNode c, LeeNode::FoundBy fb) {
    vector<LeeNode> results;
    LeeNode temp = c;

    // (x, y+1)
    if (is_placeable_router(c.get_x(), c.get_y() + 1, fb)) {
        temp.set_x_coord( c.get_x());
        temp.set_y_coord(c.get_y() + 1);
        if (!is_in_vector(temp, fb)) {
            temp = calculate_metric(temp, fb);
            if (temp.get_found_by() != kMap->get_map()->at(temp.get_x()).at(temp.get_y())->get_found_by()) {
                temp.set_found_by(kMap->get_map()->at(temp.get_x()).at(temp.get_y())->get_found_by());
            }
            results.push_back(temp);
        }
    }
    // (x, y-1)
    if (is_placeable_router(c.get_x(), c.get_y() - 1, fb)) {
        temp.set_x_coord(c.get_x());
        temp.set_y_coord(c.get_y() - 1);
        if (!is_in_vector(temp, fb)) {
            temp = calculate_metric(temp, fb);
            if (temp.get_found_by() != kMap->get_map()->at(temp.get_x()).at(temp.get_y())->get_found_by()) {
                temp.set_found_by(kMap->get_map()->at(temp.get_x()).at(temp.get_y())->get_found_by());
            }
            results.push_back(temp);
        }
    }
    // (x+1, y)
    if (is_placeable_router(c.get_x() + 1, c.get_y(), fb)) {
        temp.set_x_coord(c.get_x() + 1);
        temp.set_y_coord(c.get_y());
        if (!is_in_vector(temp, fb)) {
            temp = calculate_metric(temp, fb);
            if (temp.get_found_by() != kMap->get_map()->at(temp.get_x()).at(temp.get_y())->get_found_by()) {
                temp.set_found_by(kMap->get_map()->at(temp.get_x()).at(temp.get_y())->get_found_by());
            }
            results.push_back(temp);
        }
    }
    // (x-1, y)
    if (is_placeable_router(c.get_x() - 1, c.get_y(), fb)) {
        temp.set_x_coord(c.get_x() - 1);
        temp.set_y_coord(c.get_y());
        if (!is_in_vector(temp, fb)) {
            temp = calculate_metric(temp, fb);
            if (temp.get_found_by() != kMap->get_map()->at(temp.get_x()).at(temp.get_y())->get_found_by()) {
                temp.set_found_by(kMap->get_map()->at(temp.get_x()).at(temp.get_y())->get_found_by());
            }
            results.push_back(temp);
        }
    }
    return results;
}

LeeNode Ruben::calculate_metric(LeeNode c, LeeNode::FoundBy fb) {
    LeeNode temp = c;
    LeeNode opposite_searcher;

    switch (fb) {
        case LeeNode::FoundBy::FSINK:
            opposite_searcher = kSource;
            //claim("Opposite searcher is source", kNote);
            break;
        default:
        case LeeNode::FoundBy::FNULL:
        case LeeNode::FoundBy::FSOURCE:
            opposite_searcher = kSink;
            //claim("Opposite searcher is sink", kNote);
            break;
    }

    temp.set_leewave(calculate_lees_distance(temp));
    temp.set_ruben(temp.get_leewave() + (kKornModifier * calculate_manhattan_distance(temp, opposite_searcher)));
    temp.set_cost((int) temp.get_ruben());
    temp.set_output((int)temp.get_ruben());

    kMap->get_map()->at(temp.get_x()).at(temp.get_y())->set_leewave(temp.get_leewave());
    kMap->get_map()->at(temp.get_x()).at(temp.get_y())->set_output(temp.get_leewave());
    //kMap->get_map()->at(temp.get_x()).at(temp.get_y())->set_output((int) temp.get_ruben());
    kMap->get_map()->at(temp.get_x()).at(temp.get_y())->set_ruben(temp.get_ruben());
    kMap->get_map()->at(temp.get_x()).at(temp.get_y())->set_cost(temp.get_leewave());

    if (kMap->get_map()->at(temp.get_x()).at(temp.get_y())->get_found_by() == LeeNode::FoundBy::FNULL) {
        kMap->get_map()->at(temp.get_x()).at(temp.get_y())
                ->set_found_by(temp.get_found_by());
    }
    return temp;
}