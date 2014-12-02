#ifndef _LEEORIGINAL_H_
#define _LEEORIGINAL_H_

#pragma once

#include <map.h>

namespace Lee {

    class LeeOriginal : public LeeBase {
    public:
        LeeOriginal();
        LeeOriginal(Map*);
        ~LeeOriginal();
        void start(Route);
    private:
        int solve_recursive(int);
        vector<LeeNode> get_adjacent_coordinates(LeeNode, int);
        LeeNode calculate_metric(LeeNode, int);
    };
}
#endif //_LEEORIGINAL_H_