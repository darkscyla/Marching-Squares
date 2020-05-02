#include "MarchingSquares.h"
#include "Node.h"
#include "Edge.h"

#include <cassert>
#include <iostream>


namespace cie {

/**
 * Converts the binary array to a single int
 *
 * @param arr Array containing the bool values
 * @param size The size if the array (see array decay)
 *
 * @return Integer value of the given binary number, or 0 if empty
 */
size_t ToInt(const bool arr[], const size_t size)
{
    size_t result = 0;

    for (size_t i = 0; i < size; ++i)
        result |= static_cast<size_t>(arr[i]) << (size - i - 1);

    return result;
}

inline Point2D LinearInterpolate(const Point2D& origin,
                                 const Point2D&& target,
                                 const Point2D& values,
                                 const double iso_value)
{
    const auto t = (iso_value - values[0]) / (values[1] - values[0]);

    return {
        (1 - t) * origin[0] + t * target[0],
        (1 - t) * origin[1] + t * target[1]
    };
}

/**
 * Constructor of the marching squares class
 *
 * @param function The function for which marching squares needs to be generated
 * @param x_limits The lower and upper bounds in the x direction
 * @param y_limits The lower and upper bounds in the y direction
 * @param resolution The number of cells in the x and y direction
 */
MarchingSquares::MarchingSquares(Function function,
                                 const Limits& x_limits,
                                 const Limits& y_limits,
                                 const Resolution& resolution):
    function_(std::move(function)),
    x_limits_(x_limits),
    y_limits_(y_limits),
    resolution_(verify_resolution(resolution)),
    dx_((x_limits[1] - x_limits[0]) / resolution_[0]),
    dy_((y_limits[1] - y_limits[0]) / resolution_[1]),
    cur_x_(x_limits[0]),
    cur_y_(y_limits[0]),
    x_major_(resolution[0] > resolution[1])
{
    nx1_ = resolution[!x_major_];
    nx2_ = resolution[x_major_];
}

Resolution MarchingSquares::verify_resolution(const Resolution& resolution)
{
    assert(("The number of cells in x directions must be greater than 0", resolution[0] > 0));
    assert(("The number of cells in y directions must be greater than 0", resolution[1] > 0));

    return resolution;
}

EdgeVertices MarchingSquares::edge_id_to_nodes(const size_t id) const
{
    if (id >= total_edges_)
        return {};

    return edge_to_vertices_[id];
}

EdgeIndexList MarchingSquares::case_to_edges(const size_t id) const
{
    if (id >= total_cases_)
        return {};

    return lookup_table_[id];
}

void MarchingSquares::increment_minor_axis() const
{
    if (x_major_)
        cur_y_ += dy_;
    else
        cur_x_ += dx_;
}

void MarchingSquares::reset_minor_axis() const
{
    if (x_major_)
        cur_y_ = y_limits_[0];
    else
        cur_x_ = x_limits_[0];
}

void MarchingSquares::increment_major_axis() const
{
    if (x_major_)
        cur_x_ += dx_;
    else
        cur_y_ += dy_;
}

void MarchingSquares::reset_major_axis() const
{
    if (x_major_)
        cur_x_ = x_limits_[0];
    else
        cur_y_ = y_limits_[0];
}

EdgeList MarchingSquares::compute(double iso_value) const
{
    EdgeList edge_list;

    // Extract the entries for better readability
    const auto x_lower = x_limits_[0];
    const auto x_upper = x_limits_[1];
    const auto y_lower = y_limits_[0];
    const auto y_upper = y_limits_[1];

    const auto dx = (x_upper - x_lower) / resolution_[0];
    const auto dy = (y_upper - y_lower) / resolution_[1];

    auto func_values = std::vector<std::vector<double>>(resolution_[0] + 1,
                                     std::vector<double>(resolution_[1] + 1));

    // Fill in the function values
    for (size_t i = 0; i <= resolution_[0]; ++i)
    {
        const auto x = x_lower + i * dx;

        for (size_t j = 0; j <= resolution_[1]; ++j)
        {
            const auto y = y_lower + j * dy;
            func_values[i][j] = function_(x, y);
        }
    }
    
    // Go over all the cells
    for(size_t i = 0; i < resolution_[0]; ++i)
    {
        const auto x = x_lower + i * dx;

        for(size_t j = 0; j < resolution_[1]; ++j)
        {
            const auto y = y_lower + j * dy;

            // Calculate the node coordinates
            std::array<Node, 4> nodes = { {
                    {func_values[i][j + 1], x, y + dy},             //Node 1
                    {func_values[i + 1][j + 1], x + dx, y + dy},    //Node 2
                    {func_values[i + 1][j], x + dx, y},             //Node 3
                    {func_values[i][j], x, y},                      //Node 4
                } };

            // Compute the function signs based on iso value
            bool func_signs[4];
            
            for(size_t k = 0; k < 4; ++k)
            {
                func_signs[k] = (iso_value - nodes[k].Value()) > 0;
            }

            // Compute the corresponding key from the function signs
            const auto key = ToInt(func_signs, 4);

            // Get the edges corresponding to the key
            const auto edges = case_to_edges(key);
            
            // Compute and add the edge after interpolation
            if (edges.size() == 2)
            {
                // Get the coordinates of the first edge
                auto nodes_id = edge_id_to_nodes(edges[0]);
                const auto origin = Edge(&nodes[nodes_id[0]], &nodes[nodes_id[1]]).GetIsoCoordinates(iso_value);

                // Get the coordinates of the second edge
                nodes_id = edge_id_to_nodes(edges[1]);
                const auto target = Edge(&nodes[nodes_id[0]], &nodes[nodes_id[1]]).GetIsoCoordinates(iso_value);

                edge_list.push_back({ {{origin[0], origin[1]}, 
                                       {target[0], target[1]}} });
            }
            else if(edges.size() == 4)
            {
                auto nodes_id = edge_id_to_nodes(edges[0]);
                auto point1 = Edge(&nodes[nodes_id[0]], &nodes[nodes_id[1]]).GetIsoCoordinates(iso_value);

                nodes_id = edge_id_to_nodes(edges[1]);
                const auto point2 = Edge(&nodes[nodes_id[0]], &nodes[nodes_id[1]]).GetIsoCoordinates(iso_value);

                nodes_id = edge_id_to_nodes(edges[2]);
                auto point3 = Edge(&nodes[nodes_id[0]], &nodes[nodes_id[1]]).GetIsoCoordinates(iso_value);

                nodes_id = edge_id_to_nodes(edges[3]);
                const auto point4 = Edge(&nodes[nodes_id[0]], &nodes[nodes_id[1]]).GetIsoCoordinates(iso_value);

                // Sort based on the x-component of the array
                if (point1[0] > point3[0])
                    std::swap(point1, point3);

                edge_list.push_back({ {{point1[0], point1[1]},
                                       {point4[0], point4[1]}} });

                edge_list.push_back({ {{point2[0], point2[1]},
                                       {point3[0], point3[1]}} });
            }
        }
    }

    return edge_list;
}

void MarchingSquares::check_vertical_edge(std::vector<double>& arr, const size_t index, const double iso_value, Point2D& last_point, std::vector<size_t>& index_map) const
{
    arr[index + 1] = function_(cur_x_, cur_y_);

    // If function value signs are opposite
    if ((arr[index] - iso_value) *
        (arr[index + 1] - iso_value) <= 0)
    {
        vertices_.emplace_back(LinearInterpolate(last_point,
            { cur_x_, cur_y_ },
            { arr[index], arr[index + 1] },
            iso_value));

        index_map[index] = vertex_count_;

        ++vertex_count_;
    }
    last_point = { cur_x_, cur_y_ };
}

void MarchingSquares::check_horizontal_edge(const Point2D&& values, double iso_value, Point2D&& last_point, size_t& index) const
{
    if ((values[0] - iso_value) *
        (values[1] - iso_value) <= 0)
    {
        vertices_.emplace_back(LinearInterpolate(last_point,
            { cur_x_, cur_y_ },
            values,
            iso_value));
        index = vertex_count_;
        ++vertex_count_;
    }
}

void MarchingSquares::reset() const
{
    reset_major_axis();
    reset_minor_axis();
}

Point2D MarchingSquares::get_previous_horizontal_point() const
{
    if (x_major_)
        return Point2D{ cur_x_ - dx_, cur_y_ };

    return Point2D{ cur_x_, cur_y_ - dy_};
}

// In order to store minimum number of calculations, we play smart
// We will return a vertex array object and an element index buffer
std::tuple<VerticesList, IndicesList> MarchingSquares::compute2(const double iso_value) const
{
    size_t edge_counter = 0;
    // Preallocate memory for maximum possible vertices m(n+1) + n(m+1)
    vertices_.reserve(nx1_ * (nx2_ + 1) + nx2_ * (nx1_ + 1));
    std::vector<std::array<size_t, 2>> indices;
    indices.reserve(2 * nx1_ * nx2_);

    // Pre-compute the function values at first column of the minor axis
    std::vector<double> last_col_func(nx2_ + 1);
    std::vector<double> cur_col_func(nx2_ + 1);

    // Map to store the vertices index
    std::vector<size_t> last_index_map(nx2_);
    std::vector<size_t> cur_index_map(nx2_);

    size_t top_index, bottom_index;
    bool pattern[4];
    size_t assembled_point_indexes[4];

    // Compute first point of the zeroth column
    last_col_func[0] = function_(cur_x_, cur_y_);
    Point2D last_point = { cur_x_, cur_y_ };
    increment_minor_axis();

    // Calculate rest of the entries of the zeroth column
    for (size_t j = 0; j < nx2_; ++j)
    {
        check_vertical_edge(last_col_func, j, iso_value, last_point, last_index_map);
        increment_minor_axis();
    }
    reset_minor_axis();
    increment_major_axis();

    // Go over all the cells, iterating primarily over minor axis (smaller boundary, less storage)
    for (size_t i = 0; i < nx1_; ++i)
    {
        // Compute the first entry of the current column
        cur_col_func[0] = function_(cur_x_, cur_y_);
        last_point = { cur_x_, cur_y_ };

        // Check the first horizontal edge
        check_horizontal_edge(
            { last_col_func[0], cur_col_func[0] },
            iso_value,
            get_previous_horizontal_point(),
            bottom_index);

        increment_minor_axis();

        for (size_t j = 0; j < nx2_; ++j)
        {
            check_vertical_edge(cur_col_func, j, iso_value, last_point, cur_index_map);

            // Now we compute the coordinates of the new vertex on the horizontal edge
            check_horizontal_edge(
                { last_col_func[j + 1], cur_col_func[ j + 1] },
                iso_value,
                get_previous_horizontal_point(),
                top_index);

            assembled_point_indexes[0] = bottom_index;
            assembled_point_indexes[1] = cur_index_map[j];
            assembled_point_indexes[2] = top_index;
            assembled_point_indexes[3] = last_index_map[j];

            // Check the cells case
            pattern[0] = last_col_func[j] - iso_value > 0;
            pattern[1] = cur_col_func[j] - iso_value > 0;
            pattern[2] = cur_col_func[j + 1] - iso_value > 0;
            pattern[3] = last_col_func[j + 1] - iso_value > 0;

            const auto key = ToInt(pattern, 4);
            const auto intersected_edges = case_to_edges(key);

            if (intersected_edges.size() == 2)
            {
                indices.emplace_back(std::array<size_t, 2> {
                    assembled_point_indexes[intersected_edges[0]],
                        assembled_point_indexes[intersected_edges[1]]
                });

                edge_counter += 1;
            }
            // Ambiguous cases
            else if (intersected_edges.size() == 4)
            {
                // We sort by the x component
                if (vertices_[assembled_point_indexes[0]][0] > 
                    vertices_[assembled_point_indexes[2]][0])
                {
                    indices.emplace_back(std::array<size_t, 2> {
                        assembled_point_indexes[intersected_edges[0]],
                            assembled_point_indexes[intersected_edges[1]]
                    });

                    indices.emplace_back(std::array<size_t, 2> {
                        assembled_point_indexes[intersected_edges[2]],
                            assembled_point_indexes[intersected_edges[3]]
                    });
                }
                else
                {
                    indices.emplace_back(std::array<size_t, 2> {
                        assembled_point_indexes[intersected_edges[0]],
                            assembled_point_indexes[intersected_edges[3]]
                    });

                    indices.emplace_back(std::array<size_t, 2> {
                        assembled_point_indexes[intersected_edges[1]],
                            assembled_point_indexes[intersected_edges[2]]
                    });
                }
                edge_counter += 2;
            }

            bottom_index = top_index;
            increment_minor_axis();
        }
        last_index_map.swap(cur_index_map);
        last_col_func.swap(cur_col_func);

        reset_minor_axis();
        increment_major_axis();
    }
    reset();

 /*   std::cout << "Vertex count var: " << vertex_count_ << std::endl;
    std::cout << "Vertex count before: " << vertices_.size() << std::endl;
    vertices_.resize(vertex_count_);
    vertices_.shrink_to_fit();
    std::cout << "Vertex count after: " << vertices_.size() << std::endl;
    indices.resize(edge_counter);
    indices.shrink_to_fit();*/

    vertex_count_ = 0;
    // vertices_ will implicitly be reset due to std::move
    return { std::move(vertices_), std::move(indices) };
}
} // namespace cie
