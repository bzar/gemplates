CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
project(gemplates-example-sdl2-danmaku)

list(APPEND CMAKE_CXX_FLAGS "-Wall -Wextra --std=c++11")

find_package(PkgConfig REQUIRED)
pkg_check_modules(sdl2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
PKG_SEARCH_MODULE(SDL2TTF REQUIRED SDL2_ttf>=2.0.0)
find_package(Boost COMPONENTS container REQUIRED)
include_directories(
  ${sdl2_INCLUDE_DIRS}
  ${SDL2IMAGE_INCLUDE_DIRS}
  ${SDL2TTF_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIR}
)

add_executable(sdl2-danmaku engine.cpp main.cpp bullets.cpp)
target_link_libraries(sdl2-danmaku ${sdl2_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${SDL2TTF_LIBRARIES} ${Boost_LIBRARIES})


file(COPY img DESTINATION .)
file(COPY ttf DESTINATION .)
