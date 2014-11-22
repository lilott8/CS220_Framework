#ifndef _LEENODE_H_
#define _LEENODE_H_

#pragma once

#include "node.h"
#include <string>

using namespace Utilities;
using namespace std;

namespace Lee {
    class LeeNode : public Node {
    public:
        enum NodeType {
            BLOCKED, SINK, SOURCE, NONE
        };

        LeeNode(int, int, int = 0);

        ~LeeNode();

        void set_type(LeeNode::NodeType);

        NodeType get_type();

        static string convert_type_to_string(LeeNode::NodeType);

    private:
        // stuff for private
        NodeType type;
    };
}

#endif //_LEENODE_H_