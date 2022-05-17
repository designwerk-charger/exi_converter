#include "library.h"
#include <pybind11/pybind11.h>

#include <iostream>

static std::string getStringCanIf() {
    return "blabla";
}

// the module name "mdccanif" must be the same as the one in the cmake file
PYBIND11_MODULE(exi_converter, m) {
    m.doc() = R"pbdoc(
        Designwerk EXI Converter Library
        -----------------------

        Converts JSON to EXI and backword acording to xsd specivifaction for given namespace
    )pbdoc";

    // VERSION_INFO provided by setup.py installation script
#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
    m.def("getStringCanIf", &getStringCanIf);

}

void hello() {
    std::cout << "Hello, World!" << std::endl;
}
