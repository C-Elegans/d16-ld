cmake_minimum_required(VERSION 3.5)
project(d16_ld)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(Glib REQUIRED)

set(SOURCE_FILES main.c link.c link.h)

add_executable(d16-ld ${SOURCE_FILES})
target_link_libraries(d16-ld ${Glib_LIBRARY})
target_include_directories(d16-ld PUBLIC
  ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}
  ${Glib_INCLUDE_DIR} ${GlibConfig_INCLUDE_DIR}
)

install(TARGETS d16-ld DESTINATION bin)
