cmake_minimum_required(VERSION 3.4...3.18)
set(CMAKE_CXX_STANDARD 17)
project(cellworld2)

####
#### DEPENDENCIES
####

find_package (Dependencies QUIET)

if (NOT ${Dependencies_FOUND})
    if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/DependenciesConfig.cmake)
        file(DOWNLOAD https://raw.githubusercontent.com/germanespinosa/dependencies/main/DependenciesConfig.cmake ${CMAKE_CURRENT_BINARY_DIR}/DependenciesConfig.cmake)
    endif()
    set(Dependencies_DIR "${CMAKE_CURRENT_BINARY_DIR}")
    find_package (Dependencies REQUIRED)
endif()

####
#### END DEPENDENCIES
####

install_git_dependency( Cellworld
        https://github.com/germanespinosa/cellworld
        INCLUDE_DIRECTORIES include )

install_git_dependency( Date
        https://github.com/HowardHinnant/date
        INCLUDE_DIRECTORIES include )

install_git_dependency( Json-cpp
        https://github.com/germanespinosa/json-cpp
        INCLUDE_DIRECTORIES include )



####
#### MAIN LIBRARY SETUP
####

install_git_dependency( Pybind11
        https://github.com/pybind/pybind11
        ADD_SUBDIRECTORY
        INCLUDE_DIRECTORIES include )

install_git_dependency( Catch
        https://github.com/germanespinosa/catch
        ADD_SUBDIRECTORY)

include_directories(include)

set(json_cpp_files
        ${Json-cpp_FOLDER}/src/json_builder.cpp
        ${Json-cpp_FOLDER}/src/json_date.cpp
        ${Json-cpp_FOLDER}/src/json_base.cpp
        ${Json-cpp_FOLDER}/src/json_object.cpp
        ${Json-cpp_FOLDER}/src/json_util.cpp
        )

set(cellworld_files
        ${Cellworld_FOLDER}/src/agent.cpp
        ${Cellworld_FOLDER}/src/cell_group.cpp
        ${Cellworld_FOLDER}/src/connection.cpp
        ${Cellworld_FOLDER}/src/coordinates.cpp
        ${Cellworld_FOLDER}/src/location.cpp
        ${Cellworld_FOLDER}/src/geometry.cpp
        ${Cellworld_FOLDER}/src/transformation.cpp
        ${Cellworld_FOLDER}/src/core.cpp
        ${Cellworld_FOLDER}/src/cell.cpp
        ${Cellworld_FOLDER}/src/shape.cpp
        ${Cellworld_FOLDER}/src/model.cpp
        ${Cellworld_FOLDER}/src/chance.cpp
        ${Cellworld_FOLDER}/src/visibility.cpp
        ${Cellworld_FOLDER}/src/world.cpp
        ${Cellworld_FOLDER}/src/graph.cpp
        ${Cellworld_FOLDER}/src/paths.cpp
        ${Cellworld_FOLDER}/src/state.cpp
        ${Cellworld_FOLDER}/src/map.cpp
        ${Cellworld_FOLDER}/src/capture.cpp
        ${Cellworld_FOLDER}/src/peeking.cpp
        )

pybind11_add_module(cellworld2_core src/cellworld_python.cpp ${cellworld_files} ${json_cpp_files})

target_compile_definitions(cellworld2_core
        PRIVATE VERSION_INFO=${EXAMPLE_VERSION_INFO})
