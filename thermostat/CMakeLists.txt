cmake_minimum_required(VERSION 3.28.3)

set(THERMOSTAT_YOCTO_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../yocto/build" CACHE PATH
    "Yocto build directory used for the target compiler and sysroot")

if(NOT DEFINED CMAKE_C_COMPILER AND NOT DEFINED ENV{CC})
  file(GLOB YOCTO_THERMOSTAT_WORK_DIRS
    "${THERMOSTAT_YOCTO_BUILD_DIR}/tmp/work/*/thermostat/*")
  list(SORT YOCTO_THERMOSTAT_WORK_DIRS ORDER DESCENDING)
  if(YOCTO_THERMOSTAT_WORK_DIRS)
    list(GET YOCTO_THERMOSTAT_WORK_DIRS 0 YOCTO_THERMOSTAT_WORK_DIR)
    set(YOCTO_NATIVE_SYSROOT "${YOCTO_THERMOSTAT_WORK_DIR}/recipe-sysroot-native")
    set(YOCTO_TARGET_SYSROOT "${YOCTO_THERMOSTAT_WORK_DIR}/recipe-sysroot")
    file(GLOB YOCTO_C_COMPILERS
      "${YOCTO_NATIVE_SYSROOT}/usr/bin/*/*-gcc")
    file(GLOB YOCTO_CXX_COMPILERS
      "${YOCTO_NATIVE_SYSROOT}/usr/bin/*/*-g++")
    if(YOCTO_C_COMPILERS AND YOCTO_CXX_COMPILERS)
      list(GET YOCTO_C_COMPILERS 0 YOCTO_C_COMPILER)
      list(GET YOCTO_CXX_COMPILERS 0 YOCTO_CXX_COMPILER)
      set(CMAKE_C_COMPILER "${YOCTO_C_COMPILER}" CACHE FILEPATH
          "Yocto C compiler" FORCE)
      set(CMAKE_CXX_COMPILER "${YOCTO_CXX_COMPILER}" CACHE FILEPATH
          "Yocto C++ compiler" FORCE)
      set(CMAKE_SYSROOT "${YOCTO_TARGET_SYSROOT}" CACHE PATH
          "Yocto target sysroot" FORCE)
      set(CMAKE_FIND_ROOT_PATH "${YOCTO_TARGET_SYSROOT}" CACHE PATH
          "Yocto target package root" FORCE)
      set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER CACHE STRING "" FORCE)
      set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY CACHE STRING "" FORCE)
      set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY CACHE STRING "" FORCE)
      set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY CACHE STRING "" FORCE)
    endif()
  endif()
endif()

project(Thermostat)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

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
list(APPEND SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/src/modules/ui/fonts/montserrat_240.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/lib/bsec/bsec_iaq.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/lib/bsec/bsec_iaq.h"
)

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

target_compile_definitions(${PROJECT_NAME} PRIVATE LV_CONF_INCLUDE_SIMPLE=1)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
install(FILES thermostat.service
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/systemd/system)
