#ifndef _COMPARATORS_H_
#define _COMPARATORS_H_

#pragma once

class CompareNodesRuben {
public:
    bool operator()(LeeNode &a, LeeNode &b) {
        // do the checking here!
        return a.get_ruben() >= b.get_ruben();
    }
};

class CompareNodesHadlock {
public:
    bool operator()(LeeNode &a, LeeNode &b) {
        // do the checking here!
        return a.get_hadlock()  >= b.get_hadlock();
        //return true;
    }
};

#endif //_COMPARATORS_H_
