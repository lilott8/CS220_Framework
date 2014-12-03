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
        enum FoundBy {
            FSINK, FSOURCE, FNULL
        };

        LeeNode();
        LeeNode(int, int, int = 0);
        LeeNode(Point, int = 0);

        ~LeeNode();

        void set_type(LeeNode::NodeType);

        NodeType get_type();


        static string convert_type_to_string(LeeNode::NodeType);

        static string convert_found_by_to_string(LeeNode::FoundBy);
        string to_string();
        string coords_to_string();

        void set_output(int);
        void set_leewave(int);
        void set_lee2bitwave(int);
        void set_lee3bitwave(int);
        void set_ruben(double);
        void set_korn(double);
        void set_hadlock(int);

        void set_found_by(FoundBy);

        int get_output();
        int get_leewave();
        int get_lee2bitwave();
        int get_lee3bitwave();
        double get_ruben();
        double get_korn();
        int get_hadlock();

        FoundBy get_found_by();

        bool is_placeable();

    private:
        // stuff for private
        // What is the purpose of this node
        NodeType kType;
        // what is the textual representation of this node
        int kOutput;
        int kLeeWave;
        int kLee2BitWave;
        int kLee3BitWave;
        double kRuben;
        double kKorn;
        int kHadlock;
        FoundBy kFoundBy;
    };
}

#endif //_LEENODE_H_