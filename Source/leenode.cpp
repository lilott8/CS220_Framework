#include <node.h>
#include <string>
#include <leenode.h>
#include <claim.h>

using namespace Utilities;
using namespace std;
using namespace Lee;

LeeNode::LeeNode(int x, int y, int cost) {
    Node::Node(x, y, cost);
    LeeNode::type = LeeNode::NONE;
    LeeNode::detour = 0;
    LeeNode::wave = 0;
    LeeNode::output = 0;
}

LeeNode::LeeNode(Point x) {
    Node::Node(x.x, x.y);
    LeeNode::type = LeeNode::NONE;
    LeeNode::detour = 0;
    LeeNode::wave = 0;
    LeeNode::output = 0;
}

LeeNode::~LeeNode() {
    //claim("LeeNode: We aren't removing nodes in our map!", kWarning);
    //Utilities::Node::~Node();
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
        case TRACEBACK:
            s = "r";
            break;
    }
    return s;
}

int LeeNode::get_detour() {
    return detour;
}

int LeeNode::get_wave() {
    return wave;
}

int LeeNode::get_output() {
    return output;
}

void LeeNode::set_detour(int d) {
    detour = d;
}
void LeeNode::set_wave(int w) {
    wave = w;
}
void LeeNode::set_output(int o) {
    output = o;
}

string LeeNode::to_string() {
    string output = "Coords: (" + std::to_string(this->get_x()) + ", "
            + std::to_string(this->get_y()) + ")\tCost: " + std::to_string(this->get_cost())
            + "\tType: " + convert_type_to_string(this->get_type())
    + "\tDetour: " + std::to_string(this->get_detour()) + "\tWave: " + std::to_string(this->get_wave())
    + "\tOutput: " + std::to_string(this->get_output());

    return output;
}
