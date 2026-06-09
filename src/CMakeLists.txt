# 搜集所有源文件
file(GLOB_RECURSE TRACER_SOURCES 
    "tracer/*.cpp"
)

# 构建核心静态库
add_library(open_rayt STATIC ${TRACER_SOURCES})

target_include_directories(open_rayt PUBLIC 
    "${CMAKE_SOURCE_DIR}/include"
    "${FFTW3_INCLUDE_DIRS}"
    "${OpenCV_INCLUDE_DIRS}"
)

if(FFTW3f_FOUND)
    target_link_libraries(open_rayt 
        PUBLIC
        ${OpenCV_LIBS}
        FFTW3::fftw3f
        OpenMP::OpenMP_CXX
    )
else()
    target_link_libraries(open_rayt 
        PUBLIC
        ${OpenCV_LIBS}
        ${FFTW3f_LIBRARIES}
        OpenMP::OpenMP_CXX
    )
endif()
apply_optimizations(open_rayt)

# 构建主程序
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE open_rayt)
apply_optimizations(${PROJECT_NAME})