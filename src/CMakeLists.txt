# 搜集所有源文件
file(GLOB_RECURSE TRACER_SOURCES 
    "tracer/*.cpp"
)

# 构建核心静态库
add_library(tracer_lib STATIC ${TRACER_SOURCES})

target_include_directories(tracer_lib PUBLIC 
    "${CMAKE_SOURCE_DIR}/include"
    "${FFTW3_INCLUDE_DIRS}"
    "${OpenCV_INCLUDE_DIRS}"
)

target_link_libraries(tracer_lib 
    PUBLIC 
    ${OpenCV_LIBS} 
    ${FFTW3_LIBRARIES}
    OpenMP::OpenMP_CXX
)

apply_optimizations(tracer_lib)

# 构建主程序
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE tracer_lib)
apply_optimizations(${PROJECT_NAME})