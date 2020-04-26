#include "MarchingSquares.h"
#include "Node.h"
#include "Edge.h"

namespace cie {

size_t ToInt(const bool arr[], const size_t size)
{
	size_t result = 0;

	for (size_t i = 0; i < size; ++i)
		result |= static_cast<size_t>(arr[i]) << (size - i - 1);

	return result;
}
	
MarchingSquares::MarchingSquares(Function function,
								 const Limit& x_limits,
								 const Limit& y_limits,
								 const Resolution& resolution):
	function_(std::move(function)), x_limits_(x_limits),
	y_limits_(y_limits), resolution_(resolution)
{
}

EdgeVertices MarchingSquares::edge_id_to_nodes(const size_t id) const
{
	if (id >= total_edges)
		return {};

	return edge_to_vertices_[id];
}

EdgeIndexList MarchingSquares::case_to_edges(const size_t id) const
{
	if (id >= total_cases_)
		return {};

	return lookup_table_[id];
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
	
	// Go over all the cells
	for(size_t i = 0; i <= resolution_[0]; ++i)
	{
		const auto x = x_lower + i * dx;

		for(size_t j = 0; j <= resolution_[1]; ++j)
		{
			const auto y = y_lower + j * dy;

			// Calculate the node coordinates
			std::array<Node, 4> nodes = { {
					{function_, x, y + dy},			//Node 1
					{function_, x + dx, y + dy},	//Node 2
					{function_, x + dx, y},			//Node 3
					{function_, x, y},				//Node 4
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
			for(size_t k = 0; k < edges.size() / 2; ++k)
			{
				// Get the coordinates of the first edge
				auto nodes_id = edge_id_to_nodes(edges[2 * k]);
				const auto origin = Edge(nodes[nodes_id[0]], nodes[nodes_id[1]]).GetIsoCoordinates(iso_value);

				// Get the coordinates of the second edge
				nodes_id = edge_id_to_nodes(edges[2 * k + 1]);
				const auto target = Edge(nodes[nodes_id[0]], nodes[nodes_id[1]]).GetIsoCoordinates(iso_value);

				edge_list.push_back({ {{origin[0], origin[1]}, 
									   {target[0], target[1]}} });
			}
		}
	}

	return edge_list;
}
	
} // namespace cie
