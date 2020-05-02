// Internal Includes
#include "MarchingSquares.h"

// Pybind includes
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include <pybind11/functional.h>

// Standard includes
#include <memory>

namespace py = pybind11;

using namespace cie;

PYBIND11_MODULE(pymarchingCubes, m )
{
    m.doc( ) = "Module that implements marching cubes and marching squares";
	
	m.def("edges", []()
	{
		using EdgeList = std::vector<std::array<double, 2>>;

		// A non-copy version
		auto v = new EdgeList{ {1,2}, {3,4} };
		const auto capsule = py::capsule(v, [](void* v) { delete reinterpret_cast<EdgeList*>(v); });
		return py::array(v->size(), v->data(), capsule);
	})
	;

	py::class_<MarchingSquares, std::shared_ptr<MarchingSquares>>(m, "MarchingSquares")
	.def(py::init<Function, const Limits&, const Limits&, const Resolution&> ())
	.def("compute", &MarchingSquares::compute)
	.def("compute_faster", &MarchingSquares::compute2, py::call_guard<py::gil_scoped_release>())
	;
}
