#include <node.h>
#include <string>
#include <leenode.h>

using namespace Utilities;
using namespace std;
using namespace Lee;

LeeNode::LeeNode(int x, int y, int cost) {
    Node::Node(x, y, cost);
    LeeNode::type = LeeNode::NONE;
}

LeeNode::~LeeNode() {
    Utilities::Node::~Node();
}

void LeeNode::set_type(NodeType t) {
    this->type = t;
}

LeeNode::NodeType LeeNode::get_type() {
    return this->type;
}

string LeeNode::convert_type_to_string(NodeType t) {
    string s = "";
    switch (t) {
        case BLOCKED:
            s = "x";
            break;
        case NONE:
        default:
            s = "0";
            break;
        case SINK:
            s = "t";
            break;
        case SOURCE:
            s = "s";
            break;
    }
    return s;
}