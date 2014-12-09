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

        virtual ~LeeBase();

        LeeBase &set_map(Map *);

        // Make sure child classes can get access to these!
        virtual void start(Route);
        virtual void start();
        bool is_valid();
        Path* get_path_back(int x);
        void enable_intersections();

        void enable_bi_direction();
        void set_korn(double);
        double get_korn();

        int calculate_manhattan_distance(int, int);
        int calculate_manhattan_distance(LeeNode, LeeNode);
        int calculate_lees_distance(LeeNode);
        int caclulate_euclidean_distance(int, int);
        double calculate_euclidean_distance(LeeNode, LeeNode);
        bool is_adjacent(LeeNode, LeeNode);
        static bool is_same_coordinate(LeeNode, LeeNode);

    protected:
        // appease the compiler...
        LeeNode kSink = LeeNode(-1, -1);
        LeeNode kSource = LeeNode(-1, -1);
        Map *kMap;
        deque<LeeNode> kWaveFrontSource;
        deque<LeeNode> kWaveFrontSink;
        vector<Path*> kPathBack;
        vector<LeeNode> kTraceBackSink;
        vector<LeeNode> kTraceBackSource;
        // Everything will use the kPathBack, this is just a holder for the BD
        vector<Path*> kPathBackBiDirectional;
        bool kIntersectionEnabled = false;
        bool kBiDirectionEnabled = false;
        bool kValidPlacement;
        double kKornModifier = 1;

        void clear_queues();
        bool is_sink(LeeNode);
        bool is_sink(int, int);
        bool is_source(LeeNode);
        bool is_source(int, int);

        bool is_placeable_router(int, int, LeeNode::FoundBy);
        bool is_placeable_router(int, int);
        bool is_placeable_router(LeeNode);
        bool is_placeable_router(LeeNode, LeeNode::FoundBy);

        bool is_placeable_no_intersection(int, int);
        bool is_placeable_no_intersection(LeeNode);

        bool is_placeable_intersection(LeeNode);
        bool is_placeable_intersection(int, int);

        bool is_placeable_intersection_bd(int, int, LeeNode::FoundBy);
        bool is_placeable_no_intersection_bd(int, int, LeeNode::FoundBy);

        bool is_adjacent_to_source(LeeNode);
        bool is_in_vector(LeeNode);
        bool is_in_bounds(int, int);
        bool is_valid_placement(LeeNode);
        bool is_in_sink_vector(LeeNode);
        bool is_in_source_vector(LeeNode);

        bool is_in_vector(LeeNode, LeeNode::FoundBy);
    };
}
#endif //_LEEBASE_H_