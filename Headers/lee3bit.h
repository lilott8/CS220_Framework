#ifndef _LEE_3_BIT_H_
#define _LEE_3_BIT_H_

#pragma once

#include "map.h"

using namespace Utilities;

namespace Lee {

    class Lee3Bit : public LeeBase {
    public:
        Lee3Bit();
        Lee3Bit(Map*);
        ~Lee3Bit();
        void start(Route);
    private:
        int solve_recursive(int);

        int solve_recursive_bi_directional(int);
        vector<LeeNode> get_adjacent_coordinates(LeeNode, int);
        LeeNode calculate_metric(LeeNode, int);
    };
}
#endif //_LEE_3_BIT_H_