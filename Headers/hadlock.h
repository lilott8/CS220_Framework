#ifndef _HADLOCK_H_
#define _HADLOCK_H_

#pragma once

#include "map.h"
#include "comparators.h"
#include <queue>

namespace Lee {
    class Hadlock : public LeeBase {
    public:
        Hadlock();
        Hadlock(Map *);
        ~Hadlock();
        void start(Route);
    private:
        priority_queue<LeeNode, vector<LeeNode>, CompareNodesHadlock> kWaveFrontSourcePQ;
        priority_queue<LeeNode, vector<LeeNode>, CompareNodesHadlock> kWaveFrontSinkPQ;
        LeeNode::FoundBy kFoundByFlag;
        int solve_recursive(int);

        int solve_recursive_bi_directional(int);

        vector<LeeNode> get_adjacent_coordinates(LeeNode, LeeNode::FoundBy);
        bool is_in_vector(LeeNode);

        LeeNode calculate_metric(LeeNode, LeeNode, LeeNode::FoundBy);
    };
}

#endif //_HADLOCK_H_