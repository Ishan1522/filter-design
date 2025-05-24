# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "C:/Users/ishan/filter-design-1/build/_deps/wpilib-src")
  file(MAKE_DIRECTORY "C:/Users/ishan/filter-design-1/build/_deps/wpilib-src")
endif()
file(MAKE_DIRECTORY
  "C:/Users/ishan/filter-design-1/build/_deps/wpilib-build"
  "C:/Users/ishan/filter-design-1/build/_deps/wpilib-subbuild/wpilib-populate-prefix"
  "C:/Users/ishan/filter-design-1/build/_deps/wpilib-subbuild/wpilib-populate-prefix/tmp"
  "C:/Users/ishan/filter-design-1/build/_deps/wpilib-subbuild/wpilib-populate-prefix/src/wpilib-populate-stamp"
  "C:/Users/ishan/filter-design-1/build/_deps/wpilib-subbuild/wpilib-populate-prefix/src"
  "C:/Users/ishan/filter-design-1/build/_deps/wpilib-subbuild/wpilib-populate-prefix/src/wpilib-populate-stamp"
)

set(configSubDirs Debug)
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/ishan/filter-design-1/build/_deps/wpilib-subbuild/wpilib-populate-prefix/src/wpilib-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/ishan/filter-design-1/build/_deps/wpilib-subbuild/wpilib-populate-prefix/src/wpilib-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
