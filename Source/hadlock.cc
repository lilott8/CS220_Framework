#include <deque>
#include <queue>
#include "../Headers/leebase.h"
#include "../Headers/claim.h"
#include "../Headers/hadlock.h"

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

    // reset the priority queue for our algorithm
    kWaveFrontSourcePQ = priority_queue<LeeNode, vector<LeeNode>, CompareNodesHadlock>();
    kWaveFrontSourcePQ.push(kSource);
    kWaveFrontSource.push_back(kSource);

    Path *p = new Path();
    p->set_source(kSource.get_coord());
    p->set_sink(kSink.get_coord());
    kPathBack.push_back(p);

    // Solve the problem!
    if (kBiDirectionEnabled) {
        // reset the priority queue for our algorithm
        kWaveFrontSinkPQ = priority_queue<LeeNode, vector<LeeNode>, CompareNodesHadlock>();
        kWaveFrontSinkPQ.push(kSink);
        kWaveFrontSink.push_front(kSink);
        solve_recursive_bi_directional(1);
        int x = kTraceBackSource.size()-1;
        if(!kPathBack.back()->contains(kSource.get_coord())) {
            kPathBack.back()->add_segment(new PathSegment(kSource.get_coord(), kTraceBackSource.at(x).get_coord()));
        }
        while(x > 0) {
            if(x-1 > 0) {
                PathSegment *ps = new PathSegment(kTraceBackSource.at(x).get_coord(), kTraceBackSource.at(x -1).get_coord());
                kPathBack.back()->add_segment(ps);
            } else {
                PathSegment *ps = new PathSegment(kTraceBackSink.front().get_coord(), kTraceBackSource.at(x).get_coord());
                kPathBack.back()->add_segment(ps);
            }
            x--;
        }
        x = kTraceBackSink.size()-1;
        for(int x = 0;x < kTraceBackSink.size(); x++) {
            //while(x > 0) {
            if(x+1 < kTraceBackSink.size()) {
                PathSegment *ps = new PathSegment(kTraceBackSink.at(x).get_coord(), kTraceBackSink.at(x+1).get_coord());
                kPathBack.back()->add_segment(ps);
            }
        }
        if(!kPathBack.back()->contains(kSink.get_coord())) {
            kPathBack.back()->add_segment(new PathSegment(kTraceBackSink.at(x).get_coord(), kSink.get_coord()));
        }
    } else {
        solve_recursive(1);
        for(int x = kTraceBackSource.size()-1;x>=0;x--) {
            if(x-1 >= 0) {
                kPathBack.back()->add_segment(new PathSegment(kTraceBackSource.at(x).get_coord(), kTraceBackSource.at(x-1).get_coord()));
            } else {
                kPathBack.back()->add_segment(new PathSegment(kTraceBackSource.front().get_coord(), kTraceBackSource.at(x).get_coord()));
            }
        }
    }
}

int Hadlock::solve_recursive(int iteration) {

    //claim("size of queue: " + to_string(kWaveFrontSourcePQ.size()), kDebug);
    // Base case 1: Not finding a solution
    if (kWaveFrontSourcePQ.size() < 1) {
        claim("We could not successfully route: "
                + kSource.coords_to_string() + "->"
                + kSink.coords_to_string(), kWarning);
        return iteration;
    }

    // Grab the first record
    LeeNode curr = kWaveFrontSourcePQ.top();
    // pop off the first record
    kWaveFrontSourcePQ.pop();

    // Base case 2: We found the sink
    if (is_sink(curr)) {
        // add the sink to the trace_back
        kTraceBackSource.push_back(curr);
        //kMap->get_map()->at(curr.get_x()).at(curr.get_y())->set_type(LeeNode::NodeType::TRACEBACK);
        claim("We found the sink!", kDebug);
        return iteration;
    }

    // Case 3: We still have places on the map to visit
    /**
    * Check each possibility of the next wavefront
    */
    vector<LeeNode> adjacent = get_adjacent_coordinates(curr, LeeNode::FoundBy::FNULL);

    for (int x = 0; x < adjacent.size(); x++) {
        kWaveFrontSourcePQ.push(adjacent.at(x));
        // TODO: Figure out how to iterate PQ's
        kWaveFrontSource.push_back(adjacent.at(x));
    }

    /*if (iteration % 10 == 0) {
        claim("******************************", kDebug);
        kMap->print_map();
    }*/
    solve_recursive(iteration + 1);


    // Handle the trace_back generation for the algorithm
    if (kTraceBackSource.size() > 0
            && is_adjacent(curr, kTraceBackSource.back())
            && curr.get_leewave() <= kTraceBackSource.back().get_leewave()
            ) {
        kTraceBackSource.push_back(curr);
        kMap->get_map()->at(curr.get_x()).at(curr.get_y())->set_type(LeeNode::NodeType::TRACEBACK);
        //PathSegment *ps = new PathSegment(curr.get_coord(), kTraceBackSource.at(kTraceBackSource.size() - 2).get_coord());
        //kPathBack.back()->add_segment(ps);
    }

    return iteration;
}

int Hadlock::solve_recursive_bi_directional(int iteration) {
    bool found_intersection = false;
    //claim("wave front sizes: source: " + to_string(kWaveFrontSourcePQ.size()) + "\t sink: " + to_string(kWaveFrontSinkPQ.size()), kNote);

    // Base case 1: Not finding a solution
    if (kWaveFrontSourcePQ.size() < 1 || kWaveFrontSinkPQ.size() < 1) {
        claim("We could not successfully route: "
                + kSource.coords_to_string() + "->"
                + kSink.coords_to_string(), kWarning);
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

    /*if (iteration % 25 == 0) {
        claim("This ends iteration " + to_string(iteration), kNote);
        kMap->print_map();
        claim("*************************", kNote);
    }*/

    //claim("=========================", kNote);

    /*if (iteration > 25) {
        claim("Leaving after N iterations", kWarning);
        claim(LeeNode::convert_found_by_to_string(found_by) + " is searching for: " + curr.to_string(), kWarning);
        return iteration;
    }*/

    solve_recursive_bi_directional(iteration + 1);

    // Handle the trace_back generation for the algorithm
    //claim("Looking at: " + curr.to_string(), kNote);
    if (iteration % 2 == 0) {
        if (kTraceBackSource.size() > 0 && curr.get_leewave() <= kTraceBackSource.back().get_leewave()
                && is_adjacent(curr, kTraceBackSource.back()) && curr.get_found_by() == kFoundByFlag) {
            kTraceBackSource.push_back(curr);
            kMap->get_map()->at(curr.get_x()).at(curr.get_y())->set_type(LeeNode::NodeType::TRACEBACK);
        }
    } else {
        if (kTraceBackSink.size() > 0 && curr.get_leewave() <= kTraceBackSink.back().get_leewave()
                && is_adjacent(curr, kTraceBackSink.back()) && curr.get_found_by() == kFoundByFlag) {
            kTraceBackSink.push_back(curr);
            kMap->get_map()->at(curr.get_x()).at(curr.get_y())->set_type(LeeNode::NodeType::TRACEBACK);
        }
    }
    return iteration;
}


vector<LeeNode> Hadlock::get_adjacent_coordinates(LeeNode c, LeeNode::FoundBy fb) {
    vector<LeeNode> results;
    LeeNode temp = c;

    // (x, y+1)
    if (is_placeable_router(c.get_x(), c.get_y() + 1, fb)) {
        temp.set_x_coord(c.get_x());
        temp.set_y_coord(c.get_y() + 1);
        if (!LeeBase::is_in_vector(temp, fb)) {
            temp = calculate_metric(temp, c, fb);
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
        if (!LeeBase::is_in_vector(temp, fb)) {
            temp = calculate_metric(temp, c, fb);
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
        if (!LeeBase::is_in_vector(temp, fb)) {
            temp = calculate_metric(temp, c, fb);
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
        if (!LeeBase::is_in_vector(temp, fb)) {
            temp = calculate_metric(temp, c, fb);
            if (temp.get_found_by() != kMap->get_map()->at(temp.get_x()).at(temp.get_y())->get_found_by()) {
                temp.set_found_by(kMap->get_map()->at(temp.get_x()).at(temp.get_y())->get_found_by());
            }
            results.push_back(temp);
        }
    }
    return results;
}

LeeNode Hadlock::calculate_metric(LeeNode curr, LeeNode prev, LeeNode::FoundBy fb) {
    LeeNode temp = curr;

    // We use this quite a bit, so we will save it
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
    temp.set_cost(temp.get_leewave());
    double previous_distance = calculate_euclidean_distance(prev, opposite_searcher);

    if (calculate_euclidean_distance(temp, opposite_searcher) <= previous_distance) {
        //claim("Previous hadlock: " + to_string(prev.get_hadlock()), kNote);
        //claim(temp.coords_to_string() + "\t " + prev.coords_to_string() + " are the same.", kNote);
        temp.set_hadlock(prev.get_hadlock());
    } else {
        //claim(temp.coords_to_string() + "\t " + prev.coords_to_string() + " are not the same.", kNote);
        temp.set_hadlock(prev.get_hadlock() + 1);
    }

    kMap->get_map()->at(temp.get_x()).at(temp.get_y())->set_hadlock(temp.get_hadlock());
    // There is never a time where it will hurt to have this
    kMap->get_map()->at(temp.get_x()).at(temp.get_y())->set_leewave(temp.get_leewave());
    kMap->get_map()->at(temp.get_x()).at(temp.get_y())->set_output(temp.get_hadlock());
    //kMap->get_map()->at(temp.get_x()).at(temp.get_y())->set_output(temp.get_leewave());
    kMap->get_map()->at(temp.get_x()).at(temp.get_y())->set_cost(temp.get_leewave());

    if (kMap->get_map()->at(temp.get_x()).at(temp.get_y())->get_found_by() == LeeNode::FoundBy::FNULL) {
        kMap->get_map()->at(temp.get_x()).at(temp.get_y())
                ->set_found_by(temp.get_found_by());
    }
    return temp;
}