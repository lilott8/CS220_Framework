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
            BLOCKED, SINK, SOURCE, NONE, TRACEBACK
        };

        LeeNode();
        LeeNode(int, int, int = 0);
        LeeNode(Point, int = 0);

        ~LeeNode();

        void set_type(LeeNode::NodeType);

        NodeType get_type();


        static string convert_type_to_string(LeeNode::NodeType);
        string to_string();
        string coords_to_string();

        void set_output(int);
        void set_wave(int);
        void set_detour(int);

        int get_output();
        int get_wave();
        int get_detour();

        bool is_placeable();

    private:
        // stuff for private
        NodeType type;
        int output;
        int wave;
        int detour;
    };
}

#endif //_LEENODE_H_