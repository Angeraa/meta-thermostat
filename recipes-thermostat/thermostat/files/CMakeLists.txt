cmake_minimum_required(VERSION 3.28.3)

project(Thermostat)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})

add_library(bme68x lib/bme68x/bme68x.c)
target_link_libraries(${PROJECT_NAME} PRIVATE bme68x)

target_include_directories(${PROJECT_NAME} PRIVATE 
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)