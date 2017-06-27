#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <celllists/cell.h>

namespace py = pybind11;

typedef celllists::Cell Cell;

PYBIND11_MODULE(pycellist, m) {
    m.doc() = "Python wrapper for the CellLists library";
    py::class_<Cell>(m, "Cell", py::buffer_protocol())
        .def("__init__", [](Cell &c, py::array_t<double, py::array::c_style | py::array::forcecast> ar, int n) {
                py::buffer_info info = ar.request();
                if (info.ndim != 2)
                    throw std::runtime_error("Incompatible buffer dimension. Expected 2, got " + std::to_string(info.ndim));
                if (info.shape != std::vector<py::ssize_t> {3, n})
                    throw std::runtime_error("Unexpected vector size. Should be 3, " 
                                                + std::to_string(n) + ". Got " 
                                                + std::to_string(info.shape[0]) + ", " 
                                                + std::to_string(info.shape[1]));
                    
                new (&c) Cell(static_cast<double *>(info.ptr), n);
            })
        .def(py::init<>())
        .def("create_subcell", &Cell::create_subcell)
        .def("create_reciprocal", &Cell::create_reciprocal)
        .def("nvec", &Cell::nvec)
        .def("vecs", [] (Cell &c) {
            const double* data = c.vecs();
            py::array_t<double> a({3,3},data);
            return a;
        })
        .def("gvecs", [] (Cell &c) {
            const double* data = c.gvecs();
            py::array_t<double> a({3,3},data);
            return a;
        })
        .def("volume", &Cell::volume)
        .def("gvolume", &Cell::gvolume)
        .def("lengths", [] (Cell &c) {
            const double* data = c.lengths();
            py::array_t<double> a({3},data);
            return a;
        })
        .def("glengths", [] (Cell &c) {
            const double* data = c.glengths();
            py::array_t<double> a({3},data);
            return a;
        })
        .def("spacings", [] (Cell &c) {
            const double* data = c.spacings();
            py::array_t<double> a({3},data);
            return a;
        })
        .def("gspacings", [] (Cell &c) {
            const double* data = c.gspacings();
            py::array_t<double> a({3},data);
            return a;
        })
        .def("cubic", &Cell::cubic)
        .def("cuboid", &Cell::cuboid)
        .def("to_frac", [] (Cell &c, py::array_t<double, py::array::c_style | py::array::forcecast> cart) {
            py::buffer_info c_info = cart.request();
            if (c_info.ndim != 1)
                throw std::runtime_error("Incompatible buffer dimension. Expected 1, got " + std::to_string(c_info.ndim));
            if (c_info.shape[0] != 3)
                throw std::runtime_error("Incompatible buffer shape. Expected 3, got " + std::to_string(c_info.shape[0]));
            py::array_t<double> frac({3,3});
            py::buffer_info f_info = frac.request();
            c.to_frac(static_cast<double *>(c_info.ptr), 
                      static_cast<double *>(f_info.ptr));
            return frac;
            }, py::arg("cart").none(false))
        .def("to_cart", [] (Cell &c, py::array_t<double, py::array::c_style | py::array::forcecast> frac) {
            py::buffer_info f_info = frac.request();
            if (f_info.ndim != 1)
                throw std::runtime_error("Incompatible buffer dimension. Expected 1, got " + std::to_string(f_info.ndim));
            if (f_info.shape[0] != 3)
                throw std::runtime_error("Incompatible buffer shape. Expected 3, got " + std::to_string(f_info.shape[0]));
            py::array_t<double> cart({3,3});
            py::buffer_info c_info = cart.request();
            c.to_frac(static_cast<double *>(f_info.ptr), 
                      static_cast<double *>(c_info.ptr));
            return frac;
        }, py::arg("frac").none(false))
        .def("iwrap_mic", &Cell::iwrap_mic)
        .def("iwrap_box", &Cell::iwrap_box)
        .def("ranges_cutoff", [] (Cell &c, py::array_t<double, py::array::c_style | py::array::forcecast> center, double cutoff) {
            py::buffer_info info = center.request();
            if (info.ndim != 1)
                throw std::runtime_error("Incompatible buffer dimension. Expected 1, got " + std::to_string(info.ndim));
            if (info.shape[0] != 3)
                throw std::runtime_error("Incompatible buffer shape. Expected 3, got " + std::to_string(info.shape[0]));
            
            py::array_t<py::ssize_t> ranges_begin({3}); //broken here
            auto rb = ranges_begin.unchecked<1>();
            py::buffer_info rb_info = ranges_begin.request();
            for (ssize_t i = 0; i < rb.shape(0); i++)
                rb(i) = 0;
            
            py::array_t<py::ssize_t> ranges_end({3}); //broken here
            auto re = ranges_end.unchecked<1>();
            py::buffer_info re_info = ranges_end.request();
            for (ssize_t i = 0; i < re.shape(0); i++)
                re(i) = 0;
            
            c.ranges_cutoff(static_cast<double *>(info.ptr), 
                            cutoff,
                            static_cast<int *>(rb_info.ptr),
                            static_cast<int *>(re_info.ptr));
            return std::make_tuple(ranges_begin, ranges_end);
        }); 
}
