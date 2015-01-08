CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
project(gemplates-tests)

include_directories(
  ../include
)

list(APPEND CMAKE_CXX_FLAGS "-Wall -Wextra --std=c++11")

add_executable(eventful-test-simple eventful-simple.cpp)
add_test(eventful-test-simple eventful-test-simple)

add_executable(eventful-test-queued eventful-queued.cpp)
add_test(eventful-test-queued eventful-test-queued)

add_executable(eventful-test-signaling eventful-signaling.cpp)
add_test(eventful-test-signaling eventful-test-signaling)

add_executable(scenery-test-simple scenery-simple.cpp)
add_test(scenery-test-simple scenery-test-simple)

add_executable(cabinet-test-simple cabinet-simple.cpp)
add_test(cabinet-test-simple cabinet-test-simple)

add_executable(nodedon-test-simple nodedon-simple.cpp)
add_test(nodedon-test-simple nodedon-test-simple)

add_executable(unrolled_list-test-simple unrolled_list-simple.cpp)
add_test(unrolled_list-test-simple unrolled_list-test-simple)

add_executable(componentstorage-test-simple componentstorage-simple.cpp)
add_test(componentstorage-test-simple componentstorage-test-simple)
