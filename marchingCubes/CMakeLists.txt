cmake_minimum_required( VERSION 3.10 )

project( marchingCubes CXX )

set(DRIVER_PATH "src/Main.cpp")

# Directories to include header files from
include_directories( inc )

# Gather source files into CMake variables
file( GLOB SOURCE_FILES src/*.cpp)
file( GLOB HEADER_FILES inc/*.h* )

# We remove the driver file exlicitly
list(REMOVE_ITEM SOURCE_FILES ${DRIVER_PATH})

# This enables exporting all symbols to the dll on windows
set( CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON )

# ----------------- Set up marchingCube library -------------------------

# Add library from the collected source files. The headers are given so visual studio displays them
add_library( marchingCubes SHARED ${SOURCE_FILES} ${HEADER_FILES} ) 

# Adding an executable so we can run things from C++ side as well
add_executable( driver ${DRIVER_PATH})
target_link_libraries( driver marchingCubes)

# specify the relative path the shared library object shall be installed to
if( WIN32 )
  install( TARGETS marchingCubes driver RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX} )
else( )
  install( TARGETS marchingCubes driver LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX} )
endif( )
