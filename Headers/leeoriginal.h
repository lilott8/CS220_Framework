#ifndef _LEEORIGINAL_H_
#define _LEEORIGINAL_H_

#pragma once

#include "map.h"

namespace Lee {

    class LeeOriginal : public LeeBase {
    public:
        LeeOriginal();
        LeeOriginal(Map*);
        ~LeeOriginal();
        void start(Route);
    private:
        int solve_recursive(int);
        int solve_recursive_bi_directional(int);

        vector<LeeNode> get_adjacent_coordinates(LeeNode, int, LeeNode::FoundBy);

        LeeNode calculate_metric(LeeNode, int, LeeNode::FoundBy);
    };
}
#endif //_LEEORIGINAL_H_