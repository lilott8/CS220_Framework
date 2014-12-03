#ifndef _RUBEN_H_
#define _RUBEN_H_

#pragma once

#include "map.h"
#include "comparators.h"
#include <queue>

namespace Lee {

    class Ruben : public LeeBase {
    public:
        Ruben();
        Ruben(Map*, double=1.0);
        ~Ruben();
        void start(Route);
    private:
        priority_queue <LeeNode, vector<LeeNode>, CompareNodesRuben> kWaveFrontPQ;
        int solve_recursive(int);

        int solve_recursive_bi_directional(int);
        vector<LeeNode> get_adjacent_coordinates(LeeNode, int);
        LeeNode calculate_metric(LeeNode, int);
    };
}
#endif //_RUBEN_H_