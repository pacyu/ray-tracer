cmake_minimum_required(VERSION 3.10)
project(rayt)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 使用 CMAKE_SOURCE_DIR 确保路径相对于项目根目录 
if(MSVC)
    # 针对 Visual Studio 等多配置生成器，取消 Debug/Release 子目录
    foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${CMAKE_SOURCE_DIR}/bin")
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${CMAKE_SOURCE_DIR}/lib")
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${CMAKE_SOURCE_DIR}/lib")
    endforeach()
else()
    # 针对 Makefile/Ninja 等单配置生成器
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")
endif()

# 依赖项
find_package(FFTW3 QUIET)
if(NOT FFTW3_FOUND)
    message(STATUS "Standard find_package(FFTW3) failed, falling back to manual search...")

    # 如果标准寻找失败（比如在 Windows 上没提供 Config 文件），手动捞出头文件和库
    find_path(FFTW3_INCLUDE_DIRS 
        NAMES fftw3.h
        PATH_SUFFIXES include
    )

    find_library(FFTW3_LIBRARIES 
        NAMES fftwf libfftw3f-3 fftw3f-3 fftw3f
        PATH_SUFFIXES lib bin
    )

    if(FFTW3_INCLUDE_DIRS AND FFTW3_LIBRARIES)
        set(FFTW3_FOUND TRUE)
    endif()
endif()
find_package(OpenMP REQUIRED)
find_package(OpenCV REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${OpenCV_INCLUDE_DIRS})

function(apply_optimizations target_name)
    if(MSVC)
        target_compile_options(${target_name} PRIVATE 
            $<$<NOT:$<CONFIG:Debug>>:/Ox>
            $<$<NOT:$<CONFIG:Debug>>:/Ot>
            $<$<NOT:$<CONFIG:Debug>>:/fp:fast>
            $<$<NOT:$<CONFIG:Debug>>:/Gy>
            $<$<NOT:$<CONFIG:Debug>>:/Gw>
            /arch:AVX2
        )
        target_link_options(${target_name} PRIVATE
            $<$<NOT:$<CONFIG:Debug>>:/LTCG>
            $<$<NOT:$<CONFIG:Debug>>:/INCREMENTAL:NO>
        )
    else()
        target_compile_options(${target_name} PRIVATE 
            $<$<NOT:$<CONFIG:Debug>>:-O3>
            $<$<NOT:$<CONFIG:Debug>>:-ffunction-sections>
            $<$<NOT:$<CONFIG:Debug>>:-fdata-sections>
            $<$<NOT:$<CONFIG:Debug>>:-ffast-math>
            -march=native
        )
    endif()
    
    # 启用 IPO
    include(CheckIPOSupported)
    check_ipo_supported(RESULT result OUTPUT error)
    if(result)
        set_property(TARGET ${target_name} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    endif()
endfunction()

add_subdirectory(src)
add_subdirectory(tests)