#ifndef _LEE_2_BIT_H_
#define _LEE_2_BIT_H_

#pragma once

#include "map.h"

using namespace Utilities;

namespace Lee {

    class Lee2Bit : public LeeBase {
    public:
        Lee2Bit();
        Lee2Bit(Map*);
        ~Lee2Bit();
        void start(Route);
    private:
        int solve_recursive(int);

        int solve_recursive_bi_directional(int);
        vector<LeeNode> get_adjacent_coordinates(LeeNode, int, LeeNode::FoundBy);
        LeeNode calculate_metric(LeeNode, int, LeeDirection);
    };
}
#endif //_LEE_2_BIT_H_

