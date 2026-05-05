#include <pybind11/pybind11.h>

int suma(int a, int b) {
    return a + b;
}

PYBIND11_MODULE(example, m) {
    m.def("suma", &suma, "Suma dos números");
}