cmake_minimum_required( VERSION 3.10 )

set( PROJECT_NAME "marchingCubes" )
set( PYBIND_PATH "external/pybind11" )
set( CMAKE_CXX_STANDARD 14 )

project( ${PROJECT_NAME} CXX )

# Enable maximum warnings
if( CMAKE_COMPILER_IS_GNUCXX )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wall -Werror -fPIC" )
endif( CMAKE_COMPILER_IS_GNUCXX )

# Set the install path
if ( CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT )
    set ( CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/install" CACHE PATH "default install path" FORCE )
endif( )

SET( CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}" )

# Set up the main project
add_subdirectory( ${PROJECT_NAME} )

# Setup the python bindings
set( PYBIND11_CPP_STANDARD -std=c++14 )

# Add pybind11 project
add_subdirectory( ${PYBIND_PATH} )

# Add bindings
add_subdirectory( python )
