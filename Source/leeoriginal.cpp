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

void LeeOriginal::start(Route r) {
    LeeBase::start(r);

    kWaveFront.push_front(kSource);

    if(is_valid()) {
        Path* p = new Path();
        p->set_source(kSource.get_coord());
        p->set_sink(kSink.get_coord());
        kPathBack.push_back(p);

        solve_recursive(1);

        //string output = "Traceback: \n";
        for(int x = 0; x < kTraceBack.size();x++) {
        //    output += kTraceBack.at(x).to_string() + "\n";
        }
        //claim(output, kDebug);
    } else {
        claim("We cannot route path: " + r.source.coords_to_string()
                + ", " + r.sink.coords_to_string(), kWarning);
    }
}

int LeeOriginal::solve_recursive(int iteration) {
    //claim("Queue size: " + to_string(kWaveFront.size()), kNote);

    // Base case 1: Not finding a solution
    if (kWaveFront.size() < 1) {
        claim("We could not find a solution for: " + kSource.coords_to_string()
                        + ", " + kSink.coords_to_string(), kWarning);
        return iteration;
    }

    // Grab the first record
    LeeNode curr = kWaveFront.front();
    // pop off the first record
    kWaveFront.pop_front();

    //claim("Curr Coordinates: " + curr.to_string(), kNote);

    // Base case 2: We found the sink
    if (is_sink(curr)){// || kMap->get_map()->at(curr.get_x()).at(curr.get_y())->get_type() == LeeNode::NodeType::SINK) {
        // add the sink to the trace_back
        kTraceBack.push_back(curr);
        kMap->get_map()->at(curr.get_x()).at(curr.get_y())->set_type(LeeNode::NodeType::TRACEBACK);
        claim("We found the sink: " + curr.to_string(), kDebug);
        //kMap->get_map()->at(curr.x).at(curr.y) = kTraceback;
        return iteration;
    }

    // Case 3: We still have places on the map to visit
    /**
    * Check each possibility of the next wavefront
    */
    vector<LeeNode> adjacent = get_adjacent_coordinates(curr, iteration);

    for (int x = 0; x < adjacent.size(); x++) {
        kWaveFront.push_back(adjacent.at(x));
    }
    //kMap->print_map();
    //claim("=========================", kNote);
    solve_recursive(iteration + 1);

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

vector<LeeNode> LeeOriginal::get_adjacent_coordinates(LeeNode c, int iteration) {
    vector<LeeNode> results;
    LeeNode temp(c.get_x(), c.get_y());

    // (x, y+1)
    if (is_placeable(c.get_x(), c.get_y() + 1)) {
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
    if (is_placeable(c.get_x(), c.get_y() - 1)) {
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
    if (is_placeable(c.get_x() + 1, c.get_y())) {
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
    if (is_placeable(c.get_x() - 1, c.get_y())) {
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

LeeNode LeeOriginal::calculate_metric(LeeNode a, int i) {
    LeeNode temp = a;

    // Mutate the internal node, so if we add it to the
    // traceback we save the values
    temp.set_cost(LeeBase::calculate_lees_distance(a));
    temp.set_output(LeeBase::calculate_lees_distance(a));

    //claim(temp.to_string(), kNote);

    // Mutate the node's data on the map
    kMap->get_map()->at(temp.get_x()).at(temp.get_y())
            ->set_cost(LeeBase::calculate_lees_distance(a));

    kMap->get_map()->at(temp.get_x()).at(temp.get_y())
            ->set_wave(LeeBase::calculate_lees_distance(a));

    kMap->get_map()->at(temp.get_x()).at(temp.get_y())
            ->set_output(LeeBase::calculate_lees_distance(a));

    return temp;
}