
namespace Lee {
    class LeeNode : public Utilities::Node {

        class Edge;

    private:
        int wave;
        int detour;
        int dist;

    public:
        LeeNode();

        int get_x();
        int get_y();
        unsigned connections_size();
        bool connections_empty();
        bool connections_contains(Edge* edge);
        Edge* connections_at(unsigned index);
        int get_cost();

        /* Mutators */
        void set_coord(int x, int y);
        void set_x_coord(int x);
        void set_y_coord(int y);
        void set_coord(Point coord);
        void set_connections(vector<Edge*> connections);
        void add_connection(Edge* connection);
        void set_cost(int cost);
        void remove_connection(Edge* connection);
    };
}