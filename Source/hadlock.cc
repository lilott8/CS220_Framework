#include <deque>
#include <queue>
#include <leebase.h>
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
    //LeeBase::start();
    printf("Starting Hadlock's\n");

    kWaveFrontPQ.push(kSource);

    printf("Source coords: %d, %d\n", kSource.x, kSource.y);

    solve_recursive(0);
}

int Hadlock::solve_recursive(int iteration) {

    // Base case 1: Not finding a solution
    printf("size of queue: %lu\n", kWaveFrontPQ.size());
    if (kWaveFrontPQ.size() < 1) {
        printf("We have nothing in our queue\n");
        printf("=====================\n\n");
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
        kMap->get_map()->at(curr.get_x()).at(curr.get_y())->set_type(LeeNode::NodeType::TRACEBACK);
        printf("We found the sink!\n");
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
    }
    printf("******************************\n");
    kMap->print_map();
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

vector<LeeNode> Hadlock::get_adjacent_coordinates(LeeNode c) {
    vector<LeeNode> results;
    LeeNode temp;

    // We use this quite a bit, so we will save it
    double source_distance = calculate_euclidean_distance(c, kSink);

    // (x, y+1)
    if (is_placeable_router(c.get_x(), c.get_y() + 1)) {
        temp.set_x_coord(c.get_x());
        temp.set_y_coord(c.get_y() + 1);
        if (!is_in_vector(temp)) {
            temp.dist = LeeBase::calculate_manhattan_distance(temp, kSource);
            if (calculate_euclidean_distance(temp, kSink) <=
                    source_distance) {
                printf("Temp: %d, %d dist: %d is closer to sink than our current!\n", temp.get_x(), temp.get_y(), temp.dist);
                temp.get_hadlock() = c.get_hadlock();
            } else {
                printf("Temp is not closer to sink\n");
                temp.get_hadlock() = c.get_hadlock() + 1;
            }
            //kMap->get_map()->at(temp.get_x()).at(temp.get_y()) = temp.get_hadlock();
            results.push_back(temp);
            printf("Adding (x,y+1): (%d, %d)\n", temp.get_x(), temp.get_y());
        }
    }
    // (x, y-1)
    if (is_placeable_router(c.get_x(), c.get_y() - 1)) {
        temp.set_x_coord(c.get_x());
        temp.set_y_coord(c.get_y() - 1);
        if (!is_in_vector(temp)) {
            temp.dist = calculate_manhattan_distance(temp, kSource);
            if (calculate_euclidean_distance(temp, kSink) <=
                    source_distance) {
                printf("Temp: %d, %d dist: %d is closer to sink than our current!\n", temp.get_x(), temp.get_y(), temp.dist);
                temp.get_hadlock() = c.get_hadlock();
            } else {
                printf("Temp is not closer to sink\n");
                temp.get_hadlock() = c.get_hadlock() + 1;
            }
            //kMap->get_map()->at(temp.get_x()).at(temp.get_y()) = temp.get_hadlock();
            results.push_back(temp);
            printf("Adding (x,y-1): (%d, %d)\n", temp.get_x(), temp.get_y());
        }
    }
    // (x+1, y)
    if (is_placeable_router(c.get_x() + 1, c.get_y())) {
        temp.set_x_coord(c.get_x() + 1);
        temp.set_y_coord(c.get_y());
        if (!is_in_vector(temp)) {
            temp.dist = calculate_manhattan_distance(temp, kSource);
            if (calculate_euclidean_distance(temp, kSink) <=
                    source_distance) {
                printf("Temp: %d, %d dist: %d is closer to sink than our current!\n", temp.get_x(), temp.get_y(), temp.dist);
                temp.get_hadlock() = c.get_hadlock();
            } else {
                printf("Temp is not closer to sink\n");
                temp.get_hadlock() = c.get_hadlock() + 1;
            }
            //kMap->get_map()->at(temp.get_x()).at(temp.get_y()) = temp.get_hadlock();
            results.push_back(temp);
            printf("Adding (x+1,y): (%d, %d)\n", temp.get_x(), temp.get_y());
        }
    }
    // (x-1, y)
    if (is_placeable_router(c.get_x() - 1, c.get_y())) {
        temp.set_x_coord(c.get_x() - 1);
        temp.set_y_coord(c.get_y());
        if (!is_in_vector(temp)) {
            temp.dist = calculate_manhattan_distance(temp, kSource);
            if (calculate_euclidean_distance(temp, kSink) <=
                    source_distance) {
                printf("Temp: %d, %d dist: %d is closer to sink than our current!\n", temp.get_x(), temp.get_y(), temp.dist);
                temp.get_hadlock() = c.get_hadlock();
            } else {
                printf("Temp is not closer to sink\n");
                temp.get_hadlock() = c.get_hadlock() + 1;
            }
            //kMap->get_map()->at(temp.get_x()).at(temp.get_y()) = temp.get_hadlock();
            results.push_back(temp);
            printf("Adding (x-1,y): (%d, %d)\n", temp.get_x(), temp.get_y());
        }
    }
    return results;
}


bool Hadlock::is_in_vector(LeeNode c) {
    return false;
}