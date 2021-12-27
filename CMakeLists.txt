cmake_minimum_required (VERSION 3.5)
project (flowfree-SAT VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(lib/minisat)

find_package(Boost 1.33.1)
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

add_executable(flowfree-cli
    main.cpp
    BoolExpr.cpp
    Solver.cpp
    # Headers for IDEs
    BoolExpr.hpp
    Solver.hpp
)

target_link_libraries(flowfree-cli MiniSat::libminisat)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT flowfree-cli)
