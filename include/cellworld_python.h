#pragma once
#include <json_cpp.h>
#include <pybind11/pybind11.h>
#include <pybind11/operators.h>

template<typename T>
pybind11::class_<T> json_object_binding(pybind11::handle module, const char *class_name){
    static_assert(std::is_base_of<json_cpp::Json_base, T>::value, "Must inherit from Json_base");
    return pybind11::class_<T>(module, class_name)
            .def(pybind11::init<>())
            .def("load", &json_cpp::Json_base::load)
            .def("save", &json_cpp::Json_base::save)
            .def("__str__", &json_cpp::Json_base::to_json)
            .def("__repr__", &json_cpp::Json_base::to_json)
            .def("to_json", &json_cpp::Json_base::to_json)
            .def("from_json", &json_cpp::Json_base::from_json)
            ;
}


template<typename T>
pybind11::class_<T> json_vector_binding(pybind11::handle module, const char *class_name){

    return json_object_binding<json_cpp::Json_vector<T>>(module,class_name)
            .def(pybind11::init<size_t>())
            .def(pybind11::init<size_t, T>())
            .def("__getitem__", +[](const json_cpp::Json_vector<T>& m, const int c){return m[c];})
            .def("__setitem__", +[](json_cpp::Json_vector<T>& m, const int c, T v){m[c] = v;})
            .def("__len__", &json_cpp::Json_vector<T>::size)
            .def("__contains__", [](const json_cpp::Json_vector<T> &s, T v) { return s.contains(v); })
            .def("__reversed__", [](const json_cpp::Json_vector<T> &s) { return s.reversed(); })
            .def("__iter__", [](const json_cpp::Json_vector<T> &s) { return pybind11::make_iterator(s.begin(), s.end()); }, pybind11::keep_alive<0, 1>() /* Essential: keep object alive while iterator exists */)
            ;
}

