cmake_minimum_required(VERSION 3.28.3)

project(Thermostat)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(GNUInstallDirs)

# BSEC is distributed as a pre-built library and is not available through the
# normal package search path. Keep the default useful for the Yocto recipe,
# while allowing a standalone build to point at another copy of the library.
set(BSEC_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/lib/bsec/libalgobsec.a" CACHE FILEPATH
    "Path to the Bosch BSEC static library")
if(NOT EXISTS "${BSEC_LIBRARY}")
  message(FATAL_ERROR "BSEC library not found: ${BSEC_LIBRARY}. Set -DBSEC_LIBRARY=/path/to/libalgobsec.a")
endif()

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})

find_package(PahoMqttCpp REQUIRED)

add_library(bme68x lib/bme68x/bme68x.c)

find_package(Threads REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE 
  bme68x
  "${BSEC_LIBRARY}"
  PahoMqttCpp::paho-mqttpp3
  gpiodcxx
  lvgl
  drm
  Threads::Threads
)

target_include_directories(${PROJECT_NAME} PRIVATE 
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/bme68x
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/bsec
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    LV_CONF_INCLUDE_SIMPLE="${CMAKE_CURRENT_SOURCE_DIR}/include/lv_conf.h"
)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
install(FILES thermostat.service
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/systemd/system)
