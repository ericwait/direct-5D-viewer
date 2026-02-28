cmake_minimum_required(VERSION 3.22)

find_package(pybind11 REQUIRED)

set(D3dPy_SOURCES
    PyBind_D3d.cpp
    Threads_Py.cpp
)

pybind11_add_module(d3d_viewer_ext ${D3dPy_SOURCES})

target_include_directories(d3d_viewer_ext PRIVATE 
    ${D3d_DIR}
    ${Global_DIR}
    ${Messages_DIR}
)

target_link_libraries(d3d_viewer_ext PRIVATE D3dLib)
