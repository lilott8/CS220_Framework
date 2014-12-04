#include <node.h>
#include <leenode.h>
#include <claim.h>

using namespace Utilities;
using namespace std;
using namespace Lee;

LeeNode::LeeNode() {
}

LeeNode::LeeNode(int x, int y, int cost) {
    Node::set_x_coord(x);
    Node::set_y_coord(y);
    Node::set_cost(cost);
    this->kType = LeeNode::NONE;
    this->kLeeWave = 0;
    this->kLee2BitWave = 0;
    this->kLee3BitWave = 0;
    this->kHadlock = 0;
    this->kKorn = 0.0;
    this->kRuben = 0.0;
    this->kOutput = 0;
    this->kFoundBy = LeeNode::FNULL;
}

LeeNode::LeeNode(Point p, int cost) {
    Node::set_coord(p);
    Node::set_cost(cost);
    this->kType = LeeNode::NONE;
    this->kLeeWave = 0;
    this->kLee2BitWave = 0;
    this->kLee3BitWave = 0;
    this->kHadlock = 0;
    this->kKorn = 0.0;
    this->kRuben = 0.0;
    this->kOutput = 0;
    this->kFoundBy = LeeNode::FNULL;
}

LeeNode::~LeeNode() {
    //TODO: Figure out why this errors out!
    //claim("LeeNode: We aren't removing nodes in our map!", kWarning);
    //Utilities::Node::~Node();
}

void LeeNode::set_type(NodeType t) {
    this->kType = t;
}

void LeeNode::set_lee3bitwave(int x) {
    this->kLee3BitWave = x;
}

void LeeNode::set_leewave(int x) {
    this->kLeeWave = x;
}

void LeeNode::set_lee2bitwave(int x) {
    this->kLee2BitWave = x;
}

void LeeNode::set_korn(double x) {
    this->kKorn = x;
}

void LeeNode::set_hadlock(int x) {
    this->kHadlock = x;
}

void LeeNode::set_ruben(double x) {
    this->kRuben = x;
}

void LeeNode::set_output(int o) {
    kOutput = o;
}

void LeeNode::set_found_by(FoundBy f) {
    kFoundBy = f;
}

int LeeNode::get_output() {
    return kOutput;
}

LeeNode::NodeType LeeNode::get_type() {
    return this->kType;
}

int LeeNode::get_leewave() {
    return this->kLeeWave;
}

int LeeNode::get_lee2bitwave() {
    return this->kLee2BitWave;
}
int LeeNode::get_lee3bitwave() {
    return this->kLee3BitWave;
}
int LeeNode::get_hadlock() {
    return this->kHadlock;
}
double LeeNode::get_ruben() {
    return this->kRuben;
}
double LeeNode::get_korn() {
    return this->kKorn;
}

LeeNode::FoundBy LeeNode::get_found_by() {
    return this->kFoundBy;
}

bool LeeNode::is_placeable() {
    if(this->kOutput > 0) return false;
    if(this->get_cost() > 0) return false;
    if(this->kLeeWave > 0) return false;
    if(this->kRuben > 0) return false;
    if(this->kKorn > 0) return false;
    if(this->kLee2BitWave > 0) return false;
    if(this->kLee3BitWave > 0) return false;
    if(this->kHadlock > 0) return false;

    return true;
}

string LeeNode::to_string() {
    string output = "Coords: (" + std::to_string(this->get_x()) + ", "
            + std::to_string(this->get_y()) + ")\tCost: " + std::to_string(this->get_cost())
            + "\tType: " + convert_type_to_string(this->kType)
            + "\tLees Value: " + std::to_string(this->kLeeWave)
            + "\tLees2Bit Value: " + std::to_string(this->kLee2BitWave)
            + "\tLees3Bit Value: " + std::to_string(this->kLee3BitWave)
            + "\tRubens Value: " + std::to_string(this->kRuben)
            + "\tKorns Value: " + std::to_string(this->kKorn)
            + "\tHadlocks Value: " + std::to_string(this->kHadlock)
            + "\tFound by: " + convert_found_by_to_string(this->kFoundBy)
    + "\tOutput: " + std::to_string(this->get_output());

    return output;
}

string LeeNode::coords_to_string() {
    return "(" + std::to_string(this->get_x()) + ", "
    + std::to_string(this->get_y()) + ")";
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

string LeeNode::convert_found_by_to_string(FoundBy t) {
    string s = "";
    switch (t) {
        default:
        case FNULL:
            s = "null";
            break;
        case FSINK:
            s = "sink";
            break;
        case FSOURCE:
            s = "source";
            break;
    }
    return s;
}