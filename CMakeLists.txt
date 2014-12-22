CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
project(gemplates)

include(CTest)

OPTION(GEMPLATES_BUILD_TESTS "Build gemplates tests" ON)
OPTION(GEMPLATES_BUILD_EXAMPLES "Build gemplates examples" OFF)

#add_subdirectory(src)

# Make QtCreator see the include files
FILE(GLOB INCLUDES include/* include/**/*)
add_custom_target(DummyProject SOURCES ${INCLUDES})

if(GEMPLATES_BUILD_TESTS)
  add_subdirectory(test)
endif(GEMPLATES_BUILD_TESTS)

if(GEMPLATES_BUILD_EXAMPLES)
  add_subdirectory(example)
endif(GEMPLATES_BUILD_EXAMPLES)
