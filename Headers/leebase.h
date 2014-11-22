#ifndef _LEEBASE_H_
#define _LEEBASE_H_

#pragma once

#include <queue>
#include <deque>
#include "map.h"
#include "node.h"

using namespace Utilities;

namespace Lee {
    class LeeBase {
    public:
        LeeBase();

        LeeBase(Map *);

        ~LeeBase();

        LeeBase &set_map(Map *);

        virtual void start() =0;
        virtual void start(Connection) =0;

        // Make sure child classes can get access to these!
    protected:
        Point kSink;
        Point kSource;
        Map *kMap;
        deque <Node> kWaveFront;
        vector <Node> kTraceBack;

        int calculate_manhattan_distance(int, int);

        int calculate_manhattan_distance(Node, Node);

        int calculate_lees_distance(Node);

        int caclulate_euclidean_distance(int, int);

        double calculate_euclidean_distance(Node, Node);

        bool is_sink(Node);

        bool is_sink(int, int);

        bool is_source(Node);

        bool is_source(int, int);

        bool is_adjacent(Node, Node);

        bool is_placeable(int x, int y);

        bool is_placeable(Node);

        bool is_adjacent_to_source(Node);

        bool is_in_vector(Node);

        bool is_in_bounds(int, int);

        bool is_same_coordinate(Node, Node);
    };
}
#endif //_LEEBASE_H_