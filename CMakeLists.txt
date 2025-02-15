cmake_minimum_required(VERSION 3.10)
project(tumble C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

add_compile_options(-Wall -Wextra -Wshadow)

file(GLOB TUMBLE_SOURCES CONFIGURE_DEPENDS src/*.c)
list(FILTER TUMBLE_SOURCES EXCLUDE REGEX "src/cli\\.c$")

add_library(tumble STATIC ${TUMBLE_SOURCES})
target_include_directories(tumble PUBLIC include PRIVATE src)
target_link_libraries(tumble m)

add_executable(tumble_cli src/cli.c)
set_target_properties(tumble_cli PROPERTIES OUTPUT_NAME tumble)
target_link_libraries(tumble_cli tumble)

enable_testing()
file(GLOB TUMBLE_TESTS CONFIGURE_DEPENDS tests/test_*.c)
foreach(test_src ${TUMBLE_TESTS})
  get_filename_component(test_name ${test_src} NAME_WE)
  add_executable(${test_name} ${test_src})
  target_link_libraries(${test_name} tumble)
  add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()
