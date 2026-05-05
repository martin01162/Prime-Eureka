#include <pybind11/embed.h>
#include <Python.h>
#include <iostream>

namespace py = pybind11;

int main() {

    Py_SetPythonHome(L"C:/Users/marti/AppData/Local/Python/pythoncore-3.14-64");

    py::scoped_interpreter guard{};  // ← inicia Python

    py::module script = py::module::import("script");

    script.attr("saludar")();

    int resultado = script.attr("suma")(3, 4).cast<int>();

    std::cout << "Resultado: " << resultado << std::endl;

    system("pause");
}