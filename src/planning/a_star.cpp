//
// Created by quan
// 2020.3.15
//
#include "a_star.h"

class DjikstraPlanner {
    /**
     * Robotics Djikstra Planner
     * @calcPath - calculating the final path and draw it on Obstacle map
     * @AstarPlanning - plan the path on the given obstacle map
    */
 private:
    vector<vector<float> > path_cost;
    vector<vector<int> > traversed;
    vector<Node> motions = { Node(1, 0, 1.0), Node(0, 1, 1.0), Node(0, -1, 1.0), Node(-1, 0, 1.0), 
                            Node(1, 1, sqrt(2)), Node(1, -1, sqrt(2)), Node(-1, -1, sqrt(2)), Node(-1, 1, sqrt(2))};
 public:
    void calcPath(Node *end, GlobalObstacleMap om)
    {
        /**
         * @brief - calculate the final path finded by Djikstra
         * @param goal - goal place; m - obstacle map
        */
        while (end != NULL)
        {
            om.annoteCell(end->x, end->y, PATH);
            end = end->pre;
        }
    }

    void DjikstraPlanning(GlobalObstacleMap m) {
        /**
         * @brief - Djikstra planning algorithm
         * @param m - obstacle map
        */
        traversed = vector<vector<int>>(m.map_size_x, vector<int>(m.map_size_y, 0));
        path_cost = vector<vector<float>>(m.map_size_x, vector<float>(m.map_size_y, 0.0));

        auto cmp = [](const Node *a, const Node *b) { return a->cost > b->cost; };
        priority_queue<Node*, vector<Node*>, decltype(cmp)> nodes(cmp);

        Node *start, *final;
        bool ending_flag = false;
        start = new Node(m.start_x, m.start_y, 0.0);
        nodes.push(start);
        path_cost[m.start_x][m.start_y] = 0.0;
        while (!nodes.empty()) {
            Node *current = nodes.top();
            nodes.pop();
            if (traversed[current->x][current->y]) continue;
            traversed[current->x][current->y] = 1;
            if (DEBUG) cout << current->x << ' ' << current->y << endl;

            m.annoteCell(current->x, current->y, ROBOT);
            m.render(0);
            
            // For each motion, update djikstra
            for (auto motion : motions) {
                int next_x = current->x + motion.x, next_y = current->y + motion.y;
                float current_path_cost = path_cost[current->x][current->y];

                Node *next;
                if (m.checkCell(next_x, next_y) == GOAL) {
                    next = new Node(next_x, next_y, 0.0, current);
                    final = next;
                    ending_flag = true;
                    cout << "Path Find!" << endl;
                    break;
                }
                if (traversed[next_x][next_y] || m.checkCell(next_x, next_y) == OBSTACLE) continue;

                float next_cost = current_path_cost + motion.cost;
                next = new Node(next_x, next_y, next_cost, current);
                path_cost[next_x][next_y] = current_path_cost + motion.cost;
                nodes.push(next);
                if (DEBUG) cout << next->cost << endl;
            }

            if (ending_flag) break;
        }

        calcPath(final, m);
        m.render(0);
    }
};

class AstarPlanner {
    /**
     * Robotics A* Planner
     * @calcPath - calculating the final path and draw it on Obstacle map
     * @AstarPlanning - plan the path on the given obstacle map
    */
 private:
    vector<vector<int> > close_list;
    vector<vector<float> > path_cost;
    vector<Node> motions = { Node(1, 0, 1.0), Node(0, 1, 1.0), Node(0, -1, 1.0), Node(-1, 0, 1.0), 
                            Node(1, 1, sqrt(2)), Node(1, -1, sqrt(2)), Node(-1, -1, sqrt(2)), Node(-1, 1, sqrt(2))};

 public:
    void calcPath(Node *end, GlobalObstacleMap om)
    {
        /**
         * @brief - calculate the final path finded by A*
         * @param goal - goal place; m - obstacle map
        */
        while (end != NULL)
        {
            om.annoteCell(end->x, end->y, PATH);
            end = end->pre;
        }
    }

    void AstarPlanning(GlobalObstacleMap om)
    {
        /**
         * @brief - A* planning algorithm
         * @param m - obstacle map
        */
        // Initialize open & close lists
        for (int i = 0;i < om.map_size_x;i++) {
            vector<int> tmp;
            vector<float> tmp_c;
            for (int j = 0;j < om.map_size_y;j++) {
                tmp.push_back(0);
                tmp_c.push_back(2147483647);
            }
            path_cost.push_back(tmp_c);
            close_list.push_back(tmp);
        }

        auto cmp = [](const Node *a, const Node *b) { return a->cost > b->cost; };
        priority_queue<Node*, vector<Node*>, decltype(cmp)> open(cmp);

        Node *start;
        start = new Node(om.start_x, om.start_y, 0.0);
        path_cost[om.start_x][om.start_y] = 0.0;
        open.push(start);

        bool ending_flag = false;
        Node *final;
        while (!open.empty())
        {
            // Smallest element in open list, put it into close list
            Node *current = open.top();
            open.pop();
            if (close_list[current->x][current->y]) continue;
            if (DEBUG) cout << current->cost << endl;
            close_list[current->x][current->y] = 1;

            om.annoteCell(current->x, current->y, ROBOT);
            om.render(0);

            // For every motion, get its next Node
            if (DEBUG) cout << "-----------next----------" << endl;
            for (auto motion : motions)
            {
                int next_x = current->x + motion.x, next_y = current->y + motion.y;
                float current_path_cost = path_cost[current->x][current->y];

                Node *next;
                if (om.checkCell(next_x, next_y) == GOAL) {
                    next = new Node(next_x, next_y, 0.0, current);
                    final = next;
                    ending_flag = true;
                    cout << "Path Find!" << endl;
                    break;
                }
                if (close_list[next_x][next_y] || om.checkCell(next_x, next_y) == OBSTACLE) continue;

                float next_cost = current_path_cost + om.heuristic(next_x, next_y) + motion.cost;
                next = new Node(next_x, next_y, next_cost, current);
                path_cost[next_x][next_y] = min(path_cost[next_x][next_y], current_path_cost + motion.cost);
                open.push(next);
                if (DEBUG) cout << next->cost << endl;
            }
            if (DEBUG) cout << "-----------next----------" << endl;

            if (ending_flag) break;
        }

        calcPath(final, om);
        om.render(0);
    }
};

int main()
{
    vector<int> o_x, o_y;
    for (int i = 0;i < 50;i++) {
        o_x.push_back(0);
        o_y.push_back(i);
    }
    for (int i = 0;i < 50;i++) {
        o_x.push_back(i);
        o_y.push_back(0);
    }
    for (int i = 0;i < 50;i++) {
        o_x.push_back(i);
        o_y.push_back(49);
    }
    for (int i = 0;i < 50;i++) {
        o_x.push_back(49);
        o_y.push_back(i);
    }
    for (int i = 0;i < 26;i++) {
        o_x.push_back(i);
        o_y.push_back(15);
    }
    for (int i = 0;i < 26;i++) {
        o_x.push_back(50 - i);
        o_y.push_back(35);
    }
    GlobalObstacleMap m(50, 50, 5, 5, 45, 45, o_x, o_y, "A*");
    AstarPlanner planner;
    planner.AstarPlanning(m);
    // DjikstraPlanner planner;
    // planner.DjikstraPlanning(m);
    m.render(0);
    cv::resize(m.background, m.background, cv::Size(200, 200));
    cv::imwrite("../results/planning/astar.png", m.background);
}