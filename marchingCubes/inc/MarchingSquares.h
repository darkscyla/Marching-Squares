#pragma once

// Standard includes
#include <vector>
#include <array>
#include <functional>
#include <tuple>


namespace cie {

using Point2D = std::array<double, 2>;

using EdgeVertices = std::array<size_t, 2>;
using EdgeIndexList = std::vector<size_t>;

using EdgeList = std::vector<std::array<std::array<double, 2>, 2>>;
    
using Function = std::function<double(double, double)>;
using Limits = std::array<double, 2>;
using Resolution = std::array<size_t, 2>;

using VerticesList = std::vector<Point2D>;
using IndicesList = std::vector<EdgeVertices>;

class MarchingSquares
{
/*
 * The standard approach in literature is to construct and use
 * lookup tables for all the possible cases. There are certain
 * ambiguous cases as well which will be dealt explicitly
 *
 * The numbering of the square is as follows:
 *
 *        Node: 3     Edge: 2     Node: 2
 *            o-----------------------o
 *            |                       |
 *            |                       |
 *            |                       |
 *  Edge: 3   |                       |   Edge: 1
 *            |                       |
 *            |                       |
 *            |                       |
 *            |                       |
 *            o-----------------------o
 *        Node: 0     Edge: 0     Node: 1
 *
 * We will represent the node by 1 if it is positive
 * Otherwise, we assign it the value 0
 *
 * All possible states of the nodes ( 2 ^ 4 = 16) can
 * be represented by a 4-bit id. This id, we will use
 * the following arbitrary order:
 *
 *      (Node0, Node1, Node2, Node3)
 *
 * So, for example, if node 0 is the only positive node,
 * we have,
 *
 *      id: 1000
 *
 * This binary converted to int corresponds to the lookup key
 * 
 */

private:
    // Maps edge to node ids
    const std::vector<EdgeVertices> edge_to_vertices_ = {
        {0, 1},         // EdgeVertices 0
        {1, 2},         // EdgeVertices 1
        {2, 3},         // EdgeVertices 2
        {3, 0}          // EdgeVertices 3
    };
    
    // Maps the case to intersected edges
    const std::vector<EdgeIndexList> lookup_table_ = {
        {},             // Case: 0,     0000
        {2, 3},         // Case: 1      0001
        {1, 2},         // Case: 2      0010
        {1, 3},         // Case: 3      0011
        {0, 1},         // Case: 4      0100
        {0, 1, 2, 3},   // Case: 5      0101 (Ambiguous case)
        {0, 2},         // Case: 6      0110
        {0, 3},         // Case: 7      0111
        {0, 3},         // Case: 8      1000
        {0, 2},         // Case: 9      1001
        {0, 1, 2, 3},   // Case: 10     1010 (Ambiguous case)
        {0, 1},         // Case: 11     1011
        {1, 3},         // Case: 12     1100
        {1, 2},         // Case: 13     1101
        {2, 3},         // Case: 14     1110
        {}              // Case: 15     1111
    };

    const size_t total_edges_ = edge_to_vertices_.size();
    const size_t total_cases_ = lookup_table_.size();

    const Function function_;
    const Limits x_limits_;
    const Limits y_limits_;
    const Resolution resolution_;

    const double dx_, dy_;
    mutable double cur_x_, cur_y_;
    const bool x_major_;

    size_t nx1_, nx2_;

    // Preallocate memory for maximum possible vertices m(n+1) + n(m+1)
    mutable size_t vertex_count_ = 0;
    mutable VerticesList vertices_ = {};

    static Resolution verify_resolution(const Resolution& resolution);
    EdgeVertices edge_id_to_nodes(size_t id) const;
    EdgeIndexList case_to_edges(size_t id) const;

    void increment_minor_axis() const;
    void reset_minor_axis() const;
    void increment_major_axis() const;
    void reset_major_axis() const;
    inline void check_vertical_edge(std::vector<double>& arr, const size_t index, const double iso_value, Point2D& last_point, std::vector<size_t>& index_map) const;
    Point2D get_previous_horizontal_point() const;
    inline void check_horizontal_edge(const Point2D&& values, double iso_value, Point2D&& last_point, size_t& index) const;
    void reset() const;

public:
    MarchingSquares(Function function,
                    const Limits& x_limits,
                    const Limits& y_limits,
                    const Resolution& resolution);
    
    EdgeList compute(double iso_value) const;

    std::tuple<VerticesList, IndicesList> compute2(double iso_value) const;
};

} //namespace cie
