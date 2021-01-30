// Internal Includes
#include "MarchingSquares.h"

// Pybind includes
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>

// Standard includes
#include <memory>
#include <tuple>


namespace py = pybind11;

using namespace marching_squares;

PYBIND11_MODULE(pymarchingCubes, m )
{
    m.doc( ) = "Module that implements marching cubes and marching squares";

	m.def("compute_faster_wrapper", [](const marching_squares::MarchingSquares& obj, const double iso_value) ->std::tuple<py::array, py::array>
	{
		auto result = obj.compute_faster(iso_value);

		// We grab the arrays from the tuple
		auto vertices = new VerticesList{ std::move( std::get<0>(result) ) };
		auto indices = new IndicesList{ std::move(std::get<1>(result)) };

		// Create py::capsules to keep the object alive in python
		const auto v_capsule = py::capsule(vertices, [](void* vertices) { delete reinterpret_cast<VerticesList*>(vertices); });
		const auto i_capsule = py::capsule(indices, [](void* indices) { delete reinterpret_cast<IndicesList*>(indices); });

		// Return it as a tuple
		return { py::array(vertices->size(), vertices->data(), v_capsule),
		         py::array(indices->size(), indices->data(), i_capsule) };

	}, "Compute result and transfer ownership to python without copying (when vectors are big, copying is expensive)")
	;

	py::class_<MarchingSquares, std::shared_ptr<MarchingSquares>>(m, "MarchingSquares")
	.def(py::init<Function, const Limits&, const Limits&, const Resolution&> ())
	.def("compute", &MarchingSquares::compute)
	.def("compute_faster", &MarchingSquares::compute_faster, py::call_guard<py::gil_scoped_release>())
	;
}
