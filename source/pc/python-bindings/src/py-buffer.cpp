#include "py-buffer.hpp"

#include "wideusb/buffer.hpp"

namespace py = pybind11;

void add_buffer(pybind11::module& m)
{
    py::class_<Buffer, PBuffer>(m, "Buffer")
        .def(py::init([](size_t size){ return Buffer::create(size); }))
        ;

}
