#ifndef _LEEBASE_H_
#define _LEEBASE_H_

#pragma once

#include <queue>
#include <deque>
#include "map.h"
#include "LeeNode.h"

using namespace Utilities;

namespace Lee {
    class LeeBase {
    public:
        LeeBase();

        LeeBase(Map *);

        ~LeeBase();

        LeeBase &set_map(Map *);

        // Make sure child classes can get access to these!
        virtual void start(Route);

        virtual void start();

        bool is_valid();

        string get_path_back();

        void enable_intersections();

    protected:
        // appease the compiler...
        LeeNode kSink = LeeNode(-1, -1);
        LeeNode kSource = LeeNode(-1, -1);
        Map *kMap;
        deque <LeeNode> kWaveFront;
        vector<Path*> kPathBack;
        vector <LeeNode> kTraceBack;
        bool intersection_enabled;
        bool valid_placement;

        int calculate_manhattan_distance(int, int);

        int calculate_manhattan_distance(LeeNode, LeeNode);

        int calculate_lees_distance(LeeNode);

        int caclulate_euclidean_distance(int, int);

        double calculate_euclidean_distance(LeeNode, LeeNode);

        bool is_sink(LeeNode);

        bool is_sink(int, int);

        bool is_source(LeeNode);

        bool is_source(int, int);

        bool is_adjacent(LeeNode, LeeNode);

        bool is_placeable_router(int, int);

        bool is_placeable_router(LeeNode);

        bool is_placeable_no_intersection(int, int);

        bool is_placeable_no_intersection(LeeNode);

        bool is_placeable_intersection(LeeNode);

        bool is_placeable_intersection(int, int);

        bool is_adjacent_to_source(LeeNode);

        bool is_in_vector(LeeNode);

        bool is_in_bounds(int, int);

        bool is_same_coordinate(LeeNode, LeeNode);

        bool is_valid_placement(LeeNode);
    };
}
#endif //_LEEBASE_H_