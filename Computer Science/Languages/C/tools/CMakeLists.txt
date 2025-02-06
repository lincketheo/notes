cmake_minimum_required(VERSION 3.10)
project(Emprobe CXX)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CFLAGS_COMMON -march=native)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CFLAGS_BUILD -g -pg)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
  add_definitions(-DNDEBUG=1)
  add_definitions(-DNTEST=1)
  set(CFLAGS_BUILD -O2)
else()
  message(FATAL_ERROR "Unknown build type: ${CMAKE_BUILD_TYPE}")
endif()

file(GLOB_RECURSE CORE_SRCS "${CMAKE_SOURCE_DIR}/src/core/*.cpp")
file(GLOB_RECURSE EMPROBE_SRCS "${CMAKE_SOURCE_DIR}/src/emprobe/*.cpp")

add_library(core SHARED ${CORE_SRCS})
target_include_directories(core PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_options(core PRIVATE ${CFLAGS_COMMON} ${CFLAGS_BUILD} -fopenmp)
target_link_libraries(core PUBLIC m fftw3f fftw3f_omp gomp)

add_library(emprobe SHARED ${EMPROBE_SRCS})
target_include_directories(emprobe PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_options(emprobe PRIVATE ${CFLAGS_COMMON} ${CFLAGS_BUILD} -fopenmp)
target_link_libraries(emprobe PUBLIC m fftw3f fftw3f_omp gomp core)

install(
  TARGETS emprobe 
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

install(
  TARGETS core
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

install(
  DIRECTORY ${CMAKE_SOURCE_DIR}/include/
  DESTINATION include
  FILES_MATCHING
  PATTERN "*.h")

file(GLOB_RECURSE APPS "${CMAKE_SOURCE_DIR}/src/apps/*.cpp")
foreach(app_src ${APPS})
  get_filename_component(app_name ${app_src} NAME_WE)
  get_filename_component(app_path ${app_src} DIRECTORY)

  if("${app_path}" STREQUAL "${CMAKE_SOURCE_DIR}/src/apps" AND "${app_name}" STREQUAL "test")
    add_executable(${app_name} ${app_src})

    # Link with core
    target_link_libraries(${app_name} PRIVATE core)
    target_link_options(test PRIVATE -Wl,--whole-archive $<TARGET_FILE:core> -Wl,--no-whole-archive)

    # Link with emprobe
    target_link_libraries(${app_name} PRIVATE emprobe)
    target_link_options(test PRIVATE -Wl,--whole-archive $<TARGET_FILE:emprobe> -Wl,--no-whole-archive)

    # Add compile options
    target_compile_options(${app_name} PRIVATE ${CFLAGS_COMMON} ${CFLAGS_BUILD} -fopenmp)

    install(TARGETS ${app_name} DESTINATION bin)
  else()
    add_executable(${app_name} ${app_src})
    target_link_libraries(${app_name} PRIVATE core)
    target_link_libraries(${app_name} PRIVATE emprobe)
    target_compile_options(${app_name} PRIVATE ${CFLAGS_COMMON} ${CFLAGS_BUILD} -fopenmp)

    install(TARGETS ${app_name} DESTINATION bin)
  endif()
endforeach()

include(CPack)

set(CPACK_PACKAGE_NAME "Emprobe")
set(CPACK_PACKAGE_VERSION "0.0.3")
set(CPACK_GENERATOR "TGZ") 
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Shell utitlities for cortez processor!")
set(CPACK_PACKAGE_MAINTAINER "Theo Lincke <tjl@rincon.com>")
set(CPACK_PACKAGE_CONTACT "tjl@rincon.com")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
set(CPACK_SOURCE_GENERATOR "TGZ")
