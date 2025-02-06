file(GLOB_RECURSE CORE_SRCS CONFIGURE_DEPENDS *.cpp)

add_library(core SHARED ${CORE_SRCS})
target_include_directories(core PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_options(core PRIVATE ${CFLAGS_COMMON} ${CFLAGS_BUILD} -fopenmp)
target_link_libraries(core PUBLIC m fftw3f fftw3f_omp gomp)

install(
        TARGETS core
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

