#include <deque>
#include "../Headers/leebase.h"
#include "../Headers/claim.h"
#include "../Headers/lee2bit.h"

using namespace Lee;
using namespace std;
using namespace Utilities;

Lee2Bit::Lee2Bit() {
}

Lee2Bit::Lee2Bit(Map *m) {
    LeeBase::set_map(m);
}

Lee2Bit::~Lee2Bit() {}

void Lee2Bit::start(Route r) {
    LeeBase::start(r);

    kWaveFrontSource.push_front(kSource);

    Path* p = new Path();
    p->set_source(kSource.get_coord());
    p->set_sink(kSink.get_coord());
    kPathBack.push_back(p);

    // Solve the problem!
    if (kBiDirectionEnabled) {
        kWaveFrontSink.push_front(kSink);
        solve_recursive_bi_directional(1);
        for(int x = 0; x< kTraceBackSink.size();x++) {
            kTraceBackSource.push_front(kTraceBackSink.at(x));
        }
    }
    else {
        solve_recursive(1);
    }
    if(kTraceBackSource.size() > 0) {
        kTraceBackSource.push_back(kSource);
        create_path_back();
    }

}
int next_val = 1;
int first = true;
int Lee2Bit::solve_recursive(int iteration) {
    //claim("Queue size: " + to_string(kWaveFrontSource.size()), kNote);

    // Base case 1: Not finding a solution
    if (kWaveFrontSource.size() < 1) {
        claim("We could not successfully route: "
                + kSource.coords_to_string() + "->"
                + kSink.coords_to_string(), kWarning);
        successful_routing = false;
        clear_queues();
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
        successful_routing = true;
        //kMap->get_map()->at(curr.get_x()).at(curr.get_y())->set_type(LeeNode::NodeType::TRACEBACK);
        //claim("We found the sink: " + curr.to_string(), kDebug);
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
    //printing map every 10 interations
    if (iteration % 10 == 0) {
       //kMap->print_map();
        //claim("=========================", kNote);
    }
    solve_recursive(iteration + 1);

    // Handle the trace_back generation for the algorithm [change]

    if (kTraceBackSource.size() > 0) {
        //claim("Handling traceback::"+ to_string(kTraceBackSource.size())+"::("+to_string(curr.get_x())+","+to_string(curr.get_y())+")", kNote);
        //Check to see if adjacent node is sink
        //If yes, then set up next expected value
        if(is_sink(kTraceBackSource.back())){
            if(is_adjacent(curr,kTraceBackSource.back())){
                next_val = curr.get_lee2bitwave();
                first = true;
                //claim("SINK::TRACE::VALUE::"+to_string(curr.get_lee2bitwave()), kNote);
            }
        }
        int back_val = kTraceBackSource.back().get_lee2bitwave();
        if(is_adjacent(curr,kTraceBackSource.back())){
            if(first){
                first == false;
                if(back_val == next_val){
                    if(back_val == 1){
                        next_val = 2;
                    }
                    else{
                        next_val = 1;
                    }
                }
                else{
                    next_val = back_val;
                }
                kTraceBackSource.push_back(curr);
                if(kMap->get_map()->at(curr.get_x()).at(curr.get_y())->get_type() !=LeeNode::NodeType::SINK &&
                        kMap->get_map()->at(curr.get_x()).at(curr.get_y())->get_type() !=LeeNode::NodeType::SOURCE){
                    kMap->get_map()->at(curr.get_x()).at(curr.get_y())->set_type(LeeNode::NodeType::TRACEBACK);
                }
                kTraceBackSource.push_back(curr);
            }
            else{
                if(back_val == next_val){
                    kTraceBackSource.push_back(curr);
                    if(kMap->get_map()->at(curr.get_x()).at(curr.get_y())->get_type() !=LeeNode::NodeType::SINK &&
                            kMap->get_map()->at(curr.get_x()).at(curr.get_y())->get_type() !=LeeNode::NodeType::SOURCE){
                        kMap->get_map()->at(curr.get_x()).at(curr.get_y())->set_type(LeeNode::NodeType::TRACEBACK);
                    }
                    kTraceBackSource.push_back(curr);
                    if(back_val == 1){
                        next_val = 2;
                    }
                    else{
                        next_val = 1;
                    }
                }
            }
        }

        if(is_source(curr)) {
            //kTraceBackSource.push_back(curr);
        }
    }
    return iteration;
}

//S->T
int bi_next_val = 1;
int bi_first = true;
//T->S
int bi_next_val2 = 1;
int bi_first2 = true;
//
//LeeNode meet;
int Lee2Bit::solve_recursive_bi_directional(int iteration) {
    bool found_intersection = false;
    //Base case 1: Not finding a solution
    if(kWaveFrontSource.size() < 1 || kWaveFrontSink.size() < 1){
        claim("We could not successfully route: "
                + kSource.coords_to_string() + "->"
                + kSink.coords_to_string(), kWarning);
        clear_queues();
        return iteration;
    }

    LeeNode curr;
    LeeNode::FoundBy found_by;
    //if iteration is even, do work on the source, if odd work on sink
    if(iteration %2 == 0){
        //Grab the first record
        curr = kWaveFrontSource.front();
        //pop off the first record
        kWaveFrontSource.pop_front();

        found_by = LeeNode::FoundBy::FSOURCE;
        if(curr.get_found_by() == LeeNode::FoundBy::FSINK ){
            //claim("We have converged",kDebug);
            found_intersection = true;
            kTraceBackSource.push_back(curr);
            kTraceBackSink.push_back(curr);
            //meet = curr;
        }
    }
    else{
        //Grab the first record
        curr = kWaveFrontSink.front();
        //pop off the first record
        kWaveFrontSink.pop_front();

        found_by = LeeNode::FoundBy::FSINK;
        if(curr.get_found_by() == LeeNode::FoundBy::FSOURCE ){
            //claim("We have converged",kDebug);
            found_intersection = true;
            kTraceBackSink.push_back(curr);
            kTraceBackSource.push_back(curr);
        }
    }

    //Base case 2: We found an interection of our search
    if(found_intersection){
        //add the sink to the traceback
        kMap->get_map()->at(curr.get_x()).at(curr.get_y())->set_type(LeeNode::NodeType::TRACEBACK);
        successful_routing = true;
        return iteration;
    }

    vector<LeeNode> adjacent = get_adjacent_coordinates(curr, iteration, found_by);


    for(int x = 0; x < adjacent.size(); x++){
        if(iteration %2 == 0){
            kWaveFrontSource.push_back(adjacent.at(x));
        }
        else{
            kWaveFrontSink.push_back(adjacent.at(x));
        }
    }
    //printing map every 10 interations
    if (iteration % 10 == 0) {
        //kMap->print_map();
        //claim("=========================", kNote);
    }
    //recursion
    solve_recursive_bi_directional(iteration+1);
    //claim("TraceBackSourceSize: " + to_string((kTraceBackSource.size())),kDebug);
    //claim("TraceBackSinkSize: " + to_string((kTraceBackSink.size())),kDebug);


    bool do_lee = true;
    if(do_lee){
        // Handle the trace_back generation for the algorithm
        if (iteration % 2 == 0) {
            // Source exploring
            if (kTraceBackSource.size() > 0 && curr.get_cost() <= kTraceBackSource.back().get_cost()
                    && is_adjacent(curr, kTraceBackSource.back())) {
                kTraceBackSource.push_back(curr);
                //claim("adding: "+ curr.coords_to_string(), kDebug);
                if(kMap->get_map()->at(curr.get_x()).at(curr.get_y())->get_type() != LeeNode::NodeType::SINK &&
                        kMap->get_map()->at(curr.get_x()).at(curr.get_y())->get_type() != LeeNode::NodeType::SOURCE) {
                    kMap->get_map()->at(curr.get_x()).at(curr.get_y())->set_type(LeeNode::NodeType::TRACEBACK);
                }
            }
        } else {
            // Sink exploring
            if (kTraceBackSink.size() > 0 && curr.get_cost() <= kTraceBackSink.back().get_cost()
                    && is_adjacent(curr, kTraceBackSink.back())) {
                kTraceBackSink.push_back(curr);
                //claim("adding: "+ curr.coords_to_string(), kDebug);
                if(kMap->get_map()->at(curr.get_x()).at(curr.get_y())->get_type() != LeeNode::NodeType::SINK &&
                        kMap->get_map()->at(curr.get_x()).at(curr.get_y())->get_type() != LeeNode::NodeType::SOURCE) {
                    kMap->get_map()->at(curr.get_x()).at(curr.get_y())->set_type(LeeNode::NodeType::TRACEBACK);
                }
            }
        }
    }
    else{
        // /*
        //Handle the Traceback generation for algorithm
        //Source
        if(iteration %2 == 0){
            if(kTraceBackSource.size() > 0){
                int back_val = kTraceBackSource.back().get_lee2bitwave();
                //found intersection
                if(is_adjacent(curr,kTraceBackSource.back())){
                    if(kTraceBackSource.size() == 1){
                        if(curr.get_lee2bitwave() == back_val){
                            if(back_val == 1){
                                bi_next_val = 2;
                            }
                            else{
                                bi_next_val = 1;
                            }
                        }
                        if(curr.get_lee2bitwave() == 1){
                            bi_next_val = 1;
                        }
                        else{
                            bi_next_val = 2;
                        }
                        kTraceBackSource.push_back(curr);
                        if(kMap->get_map()->at(curr.get_x()).at(curr.get_y())->get_type() !=LeeNode::NodeType::SINK &&
                                kMap->get_map()->at(curr.get_x()).at(curr.get_y())->get_type() !=LeeNode::NodeType::SOURCE){
                            kMap->get_map()->at(curr.get_x()).at(curr.get_y())->set_type(LeeNode::NodeType::TRACEBACK);
                        }
                        kTraceBackSource.push_back(curr);
                    }
                    else{
                        if(bi_next_val == back_val){
                            kTraceBackSource.push_back(curr);
                            if(kMap->get_map()->at(curr.get_x()).at(curr.get_y())->get_type() !=LeeNode::NodeType::SINK &&
                                    kMap->get_map()->at(curr.get_x()).at(curr.get_y())->get_type() !=LeeNode::NodeType::SOURCE){
                                kMap->get_map()->at(curr.get_x()).at(curr.get_y())->set_type(LeeNode::NodeType::TRACEBACK);
                            }
                            kTraceBackSource.push_back(curr);
                            if(back_val == 1){
                                bi_next_val = 2;
                            }
                            else{
                                bi_next_val = 1;
                            }
                        }
                    }
                }
            }
        }
            //Sink
        else{
            if(kTraceBackSink.size() > 0){
                int back_val = kTraceBackSink.back().get_lee2bitwave();
                //found intersection
                if(is_adjacent(curr,kTraceBackSink.back())){
                    if(kTraceBackSink.size() == 1){
                        if(curr.get_lee2bitwave() == back_val){
                            if(back_val == 1){
                                bi_next_val2 = 2;
                            }
                            else{
                                bi_next_val2 = 1;
                            }
                        }
                        if(curr.get_lee2bitwave() == 1){
                            bi_next_val2 = 1;
                        }
                        else{
                            bi_next_val2 = 2;
                        }
                        kTraceBackSink.push_back(curr);
                        if(kMap->get_map()->at(curr.get_x()).at(curr.get_y())->get_type() !=LeeNode::NodeType::SINK &&
                                kMap->get_map()->at(curr.get_x()).at(curr.get_y())->get_type() !=LeeNode::NodeType::SOURCE){
                            kMap->get_map()->at(curr.get_x()).at(curr.get_y())->set_type(LeeNode::NodeType::TRACEBACK);
                        }
                        kTraceBackSink.push_back(curr);
                    }
                    else{
                        if(bi_next_val2 == back_val){
                            kTraceBackSink.push_back(curr);
                            if(kMap->get_map()->at(curr.get_x()).at(curr.get_y())->get_type() !=LeeNode::NodeType::SINK &&
                                    kMap->get_map()->at(curr.get_x()).at(curr.get_y())->get_type() !=LeeNode::NodeType::SOURCE){
                                kMap->get_map()->at(curr.get_x()).at(curr.get_y())->set_type(LeeNode::NodeType::TRACEBACK);
                            }
                            kTraceBackSink.push_back(curr);
                            if(back_val == 1){
                                bi_next_val2 = 2;
                            }
                            else{
                                bi_next_val2 = 1;
                            }
                        }
                    }
                }
            }
        }
    }
    // */
    return iteration;
}

vector<LeeNode> Lee2Bit::get_adjacent_coordinates(LeeNode c, int iteration,LeeNode::FoundBy fb) {
    vector<LeeNode> results;
    LeeNode temp = c;

    // (x, y+1) SOUTH
    if (is_placeable_router(c.get_x(), c.get_y() + 1, fb)) {
        temp.set_x_coord( c.get_x());
        temp.set_y_coord(c.get_y() + 1);
        //if(temp.get_output() < 1) {
        if (!is_in_vector(temp, fb)) {
            temp = calculate_metric(temp, iteration,SOUTH);
            if (temp.get_found_by() != kMap->get_map()->at(temp.get_x()).at(temp.get_y())->get_found_by()) {
                temp.set_found_by(kMap->get_map()->at(temp.get_x()).at(temp.get_y())->get_found_by());
            }
            results.push_back(temp);
            //claim("Adding (x,y+1): " + temp.to_string(), kDebug);
        }
    }
    // (x, y-1) NORTH
    if (is_placeable_router(c.get_x(), c.get_y() - 1, fb)) {
        temp.set_x_coord(c.get_x());
        temp.set_y_coord(c.get_y() - 1);
        //if(temp.get_output() < 1) {
        if (!is_in_vector(temp, fb)) {
            temp = calculate_metric(temp, iteration,NORTH);
            if (temp.get_found_by() != kMap->get_map()->at(temp.get_x()).at(temp.get_y())->get_found_by()) {
                temp.set_found_by(kMap->get_map()->at(temp.get_x()).at(temp.get_y())->get_found_by());
            }
            results.push_back(temp);
            //claim("Adding (x,y-1): " + temp.to_string(), kDebug);
        }
    }
    // (x+1, y) EAST
    if (is_placeable_router(c.get_x() + 1, c.get_y(), fb)) {
        temp.set_x_coord(c.get_x() + 1);
        temp.set_y_coord(c.get_y());
        //if(temp.get_output() < 1) {
        if (!is_in_vector(temp, fb)) {
            temp = calculate_metric(temp, iteration,EAST);
            if (temp.get_found_by() != kMap->get_map()->at(temp.get_x()).at(temp.get_y())->get_found_by()) {
                temp.set_found_by(kMap->get_map()->at(temp.get_x()).at(temp.get_y())->get_found_by());
            }
            results.push_back(temp);
            //claim("Adding (x+1,y): " + temp.to_string(), kDebug);
        }
    }
    // (x-1, y) WEST
    if (is_placeable_router(c.get_x() - 1, c.get_y(), fb)) {
        temp.set_x_coord(c.get_x() - 1);
        temp.set_y_coord(c.get_y());
        //if(temp.get_output() < 1) {
        if (!is_in_vector(temp, fb)) {
            temp = calculate_metric(temp, iteration,WEST);
            if (temp.get_found_by() != kMap->get_map()->at(temp.get_x()).at(temp.get_y())->get_found_by()) {
                temp.set_found_by(kMap->get_map()->at(temp.get_x()).at(temp.get_y())->get_found_by());
            }
            results.push_back(temp);
            //claim("Adding (x-1,y): "+temp.to_string(), kDebug);
        }
    }
    return results;
}

LeeNode Lee2Bit::calculate_metric(LeeNode a, int iteration, LeeDirection d) {
   // claim("calculating_metric", kDebug);
   // claim("x="+to_string(a.get_x())+" y="+to_string(a.get_y()) ,kDebug);
    LeeNode source = kSource;
    LeeNode sink = kSink;
    if(kBiDirectionEnabled and iteration %2 == 1){
        LeeNode tmp = source;
        source = sink;
        sink = tmp;
    }
    //Values
    LeeNode temp = a;
    int dist = calculate_lees_distance(temp);
    int cost = -1;

    //Calculate coordinates for check
    int x1,x2,x3,x4,y1,y2,y3,y4;
    if(d == SOUTH){
        //claim("SOUTH", kDebug);
        //Point 1
        x1 = a.get_x();
        y1 = a.get_y()-1;
        //Point 2
        x2 = a.get_x()+1;
        y2 = a.get_y()-1;
        //Point 3
        x3 = a.get_x();
        y3 = a.get_y()-2;
        //Point 4
        x4 = a.get_x()-1;
        y4 = a.get_y()-1;

    }
    else if( d == NORTH){
        //claim("NORTH", kDebug);
        //Point 1
        x1 = a.get_x();
        y1 = a.get_y()+1;
        //Point 2
        x2 = a.get_x();
        y2 = a.get_y()+2;
        //Point 3
        x3 = a.get_x()+1;
        y3 = a.get_y()+1;
        //Point 4
        x4 = a.get_x()-1;
        y4 = a.get_y()+1;
    }
    else if(d == WEST){
        //claim("WEST", kDebug);
        //Point 1
        x1 = a.get_x()+1;
        y1 = a.get_y();
        //Point 2
        x2 = a.get_x()+1;
        y2 = a.get_y()+1;
        //Point 3
        x3 = a.get_x()+2;
        y3 = a.get_y();
        //Point 4
        x4 = a.get_x()+1;
        y4 = a.get_y()-1;
    }
    else if(d == EAST){
        //claim("EAST", kDebug);
        //Point 1
        x1 = a.get_x()-1;
        y1 = a.get_y();
        //Point 2
        x2 = a.get_x()-1;
        y2 = a.get_y()+1;
        //Point 3
        x3 = a.get_x()-1;
        y3 = a.get_y()-1;
        //Point 4
        x4 = a.get_x()-2;
        y4 = a.get_y();
    }
    else{
        //claim("NOT A VALID DIRECTION", kDebug);
    }
    //Check values of points
    //Assumption Point 1 is always valid
    //claim("x1="+to_string(x1)+" y1="+to_string(y1) ,kDebug);
    int p1_val = kMap->get_map()->at(x1).at(y1)->get_lee2bitwave();
    //p1_val = (p1_val == 0)? 1:p1_val;
    bool NS = false;
    bool EW = false;
    bool north = false;
    bool south = false;
    int nodesFound = 1;
    //check point 2
    bool p2_match = false;
    if(is_in_bounds(x2,y2) &&
            kMap->get_map()->at(x2).at(y2)->get_lee2bitwave() != 0){
        nodesFound++;
        int p2_val = kMap->get_map()->at(x2).at(y2)->get_lee2bitwave();
        if(p1_val == p2_val){
            p2_match = true;
            if(d != SOUTH){
                NS = true;
                south = true;
            }
            else{
                EW = true;
            }
        }
    }
    //check point 3
    bool p3_match = false;
    if(is_in_bounds(x3,y3) &&
            kMap->get_map()->at(x3).at(y3)->get_lee2bitwave() != 0){
        nodesFound++;
        int p3_val = kMap->get_map()->at(x3).at(y3)->get_lee2bitwave();
        if(p1_val == p3_val){
            p3_match = true;
            if(d == SOUTH or  d == EAST){
                NS = true;
                north = true;
            }
            else{
                EW = true;
            }
        }

    }

    //check point 4
    bool p4_match = false;
    if(is_in_bounds(x4,y4) &&
            kMap->get_map()->at(x4).at(y4)->get_lee2bitwave() != 0){
        nodesFound++;
        int p4_val = kMap->get_map()->at(x4).at(y4)->get_lee2bitwave();
        if(p1_val == p4_val){
            p4_match = true;
            if(d == WEST){
                NS = true;
            }
            else{
                EW = true;
            }
        }
    }
    bool sourceFound = false;
    if(is_source(x2,y2) or is_source(x3,y3) or is_source(x4,y4))
    {
        sourceFound = true;
        nodesFound++;
    }
    int matchCount = 0;
    if(p2_match){
        //claim("P2 Match", kDebug);
        matchCount++;
    }
    if(p3_match){
        //claim("P3 Match", kDebug);
        matchCount++;
    }
    if(p4_match){
        //claim("P4 Match", kDebug);
        matchCount++;
    }

    //swap north and south
    //if( ( kSource.get_x() <= kSink.get_x() && kSource.get_y() <= kSink.get_y() ) or  ( kSource.get_x() >= kSink.get_x() && kSource.get_y() >= kSink.get_y() )  ){
    ///*
    if( ( source.get_x() <= sink.get_x() && source.get_y() <= sink.get_y() ) ){
        bool tmp = north;
        north = south;
        south = tmp;
    }

    bool West_to_East = false;
    bool East_to_West = false;
    bool North_to_South = false;
    bool South_to_North = false;
    //Set West_to_East
    if(source.get_x() < sink.get_x() ){
        West_to_East = true;
    }
//Set East_to_West
    if(source.get_x() > sink.get_x() ){
        East_to_West = true;
    }
    if(source.get_y() > sink.get_y() ){
        South_to_North= true;
    }
    if(source.get_y() < sink.get_y() ){
        North_to_South = true;
    }

    //*/
    //set staySame
    bool staySame = false;
    if(matchCount == 0){
        staySame = true;
    }
    else if(matchCount == 1){
        if(nodesFound == 1){
            staySame = false;
        }
        if(nodesFound == 2){
            staySame = false;
        }
        else if(nodesFound == 3 ){
            if(NS and sourceFound){
                staySame = true;
            }
            else if(NS){
                ///*
                if(West_to_East){
                    if(south){
                        staySame = false;
                    }
                    else if(north){
                        staySame = true;
                    }
                }
                else{
                    if(north){
                        staySame = false;
                    }
                    else if(south){
                        staySame = true;
                    }
                }
                //*/

            }
            else{
                staySame = false;
            }

        }
        else {
            if(West_to_East){
                if(north){
                    staySame = true;
                }
                else if(south){
                    staySame = false;
                }
            }
            else{
                if(north){
                    //staySame = false;
                    staySame = true;
                }
                else if(south){
                    //staySame = true;
                    staySame = true;
                }
            }
            //staySame = true;
        }

    }
    if(staySame){
        //claim("TRUE, matchCount: " + to_string(matchCount), kDebug);
    }
    else{
        //claim("FALSE, matchCount: " + to_string(matchCount), kDebug);
    }
    //claim("Number of found Nodes" + to_string(nodesFound), kDebug);
    //Set cost value
    if(p1_val == 1){
        if(staySame){
            cost = p1_val;
        }
        else{
            cost = 2;
        }
    }
    else{
        if(staySame){
            cost = p1_val;
        }
        else{
            cost = 1;
        }
    }
    //check to see if cost == 0, if so set to 1
    cost = (cost == 0)? 1:cost;

    //Set values
    if(!is_source(a)){
        //claim("Changed value to " + to_string(cost), kDebug);
        kMap->get_map()->at(a.get_x()).at(a.get_y())->set_leewave(dist);
        kMap->get_map()->at(a.get_x()).at(a.get_y())->set_lee2bitwave(cost);
        kMap->get_map()->at(a.get_x()).at(a.get_y())->set_output(cost);
        kMap->get_map()->at(a.get_x()).at(a.get_y())->set_cost(cost);
    }
    //updating Foundby
    if (kMap->get_map()->at(temp.get_x()).at(temp.get_y())->get_found_by() == LeeNode::FoundBy::FNULL) {
        kMap->get_map()->at(temp.get_x()).at(temp.get_y())
                ->set_found_by(temp.get_found_by());
    }

    return temp;
}
