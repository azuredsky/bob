# - Find Blitz library
# Find the native Blitz includes and library
# This module defines
#  Blitz_INCLUDE_DIR, where to find tiff.h, etc.
#  Blitz_LIBRARIES, libraries to link against to use Blitz.
#  Blitz_FOUND, If false, do not try to use Blitz.
# also defined, but not for general use are
#  Blitz_LIBRARY, where to find the Blitz library.

find_path(Blitz_INCLUDE_DIR blitz/blitz.h)

find_library(Blitz_LIBRARY NAMES blitz)

# handle the QUIETLY and REQUIRED arguments and set Blitz_FOUND to TRUE if 
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Blitz DEFAULT_MSG Blitz_LIBRARY Blitz_INCLUDE_DIR)

if(BLITZ_FOUND)
  SET(Blitz_LIBRARIES ${Blitz_LIBRARY})

  # and we try to determine if the the found library supports 64-bits array
  # positions.
  include(CheckCXXSourceCompiles)
  set(CMAKE_REQUIRED_INCLUDES "${Blitz_INCLUDE_DIR}")
  CHECK_CXX_SOURCE_COMPILES("#include <blitz/blitz.h>
    int main() { blitz::sizeType s; blitz::diffType d; }" HAVE_BLITZ_SPECIAL_TYPES)
  set(CMAKE_REQUIRED_INCLUDES)

  if(HAVE_BLITZ_SPECIAL_TYPES)
    add_definitions(-DHAVE_BLITZ_SPECIAL_TYPES=1)
  endif(HAVE_BLITZ_SPECIAL_TYPES)

  if(HAVE_BLITZ_SPECIAL_TYPES)
    message(STATUS "Blitz: you can allocate arrays with more than 2G-pointees")
  else(HAVE_BLITZ_SPECIAL_TYPES)
    message(STATUS "Blitz: older version detected -- please note the 2G-pointee limit for arrays!")
  endif(HAVE_BLITZ_SPECIAL_TYPES)

  # and has blitz/tinyvec2.h and not blitz/tinyvec-et.h
  find_file(TINYVEC2_FOUND "blitz/tinyvec2.h" ${Blitz_INCLUDE_DIR})
  if(TINYVEC2_FOUND)
    add_definitions(-DHAVE_BLITZ_TINYVEC2_H=1)
  endif(TINYVEC2_FOUND)

endif(BLITZ_FOUND)

mark_as_advanced(Blitz_INCLUDE_DIR Blitz_LIBRARY)
