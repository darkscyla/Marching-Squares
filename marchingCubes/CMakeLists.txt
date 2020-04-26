cmake_minimum_required( VERSION 3.10 )

project( marchingCubes CXX )

# Directories to include header files from
include_directories( inc )

# Gather source files into CMake variables
file( GLOB SOURCE_FILES src/*.cpp)
file( GLOB HEADER_FILES inc/*.h* )

# This enables exporting all symbols to the dll on windows
set( CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON )

# ----------------- Set up marchingCube library -------------------------

# Add library from the collected source files. The headers are given so visual studio displays them
add_library( marchingCubes SHARED ${SOURCE_FILES} ${HEADER_FILES} ) 

# specify the relative path the shared library object shall be installed to
if( WIN32 )
  install( TARGETS marchingCubes RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX} )
else( )
  install( TARGETS marchingCubes LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX} )
endif( )
