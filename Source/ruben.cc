#include <leenode.h>
#include <leebase.h>
#include <claim.h>
#include <ruben.h>

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

    if (is_valid()) {
        LeeBase::clear_queues();
        kWaveFrontPQ = priority_queue<LeeNode, vector<LeeNode>, CompareNodesRuben>();
        kWaveFrontPQ.push(kSource);

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

int Ruben::solve_recursive(int iteration) {

    //printf("size of queue: %lu\n", kWaveFront.size());
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

    claim("Curr Coordinates: " + curr.to_string(), kNote);

    // Base case 2: We found the sink
    if (is_sink(curr)) {
        // add the sink to the trace_back
        kTraceBack.push_back(curr);
        kMap->get_map()->at(curr.get_x()).at(curr.get_y())->set_type(LeeNode::NodeType::TRACEBACK);
        return iteration;
    }

    // Case 3: We still have places on the map to visit
    /**
    * Check each possibility of the next wavefront
    */
    vector<LeeNode> adjacent = get_adjacent_coordinates(curr, iteration);

    for (int x = 0; x < adjacent.size(); x++) {
        kWaveFrontPQ.push(adjacent.at(x));
    }

    if (iteration % 10 == 0) {
        kMap->print_map();
        claim("=========================", kNote);
    }
    solve_recursive(iteration + 1);

    // Handle the trace_back generation for the algorithm
    if (kTraceBack.size() > 0 && curr.get_leewave() <= kTraceBack.back().get_leewave()
            && is_adjacent(curr, kTraceBack.back())) {
        kTraceBack.push_back(curr);
        kMap->get_map()->at(curr.get_x()).at(curr.get_y())->set_type(LeeNode::NodeType::TRACEBACK);
    }
    return iteration;
}

int Ruben::solve_recursive_bi_directional(int iteration) {
    return iteration;
}

vector<LeeNode> Ruben::get_adjacent_coordinates(LeeNode c, int iteration) {
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

LeeNode Ruben::calculate_metric(LeeNode c, int i) {
    LeeNode temp = c;

    temp.set_leewave(calculate_lees_distance(temp));
    temp.set_ruben(temp.get_leewave() + (kKornModifier * calculate_manhattan_distance(temp, kSink)));
    temp.set_output((int)temp.get_ruben());
    
    kMap->get_map()->at(temp.get_x()).at(temp.get_y())->set_leewave(temp.get_leewave());
    kMap->get_map()->at(temp.get_x()).at(temp.get_y())->set_output(temp.get_ruben());
    //kMap->get_map()->at(temp.get_x()).at(temp.get_y())->set_output(temp.get_leewave());
    kMap->get_map()->at(temp.get_x()).at(temp.get_y())->set_ruben(temp.get_ruben());
    return temp;
}