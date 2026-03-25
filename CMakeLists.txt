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
find_package(OpenMP REQUIRED)
find_package(OpenCV REQUIRED)

include_directories(include)

# 构建静态库
file(GLOB_RECURSE SOURCES "src/parser/*.cpp" "src/core/*.cpp")
add_library(tracer_lib STATIC ${SOURCES})
target_link_libraries(tracer_lib PUBLIC OpenMP::OpenMP_CXX PRIVATE ${OpenCV_LIBS})

# 构建主可执行文件
add_executable(${PROJECT_NAME} src/main.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE tracer_lib ${OpenCV_LIBS})

# 构建解析器测试可执行文件
add_executable(test_parser tests/test_parser.cpp)
target_include_directories(test_parser PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(test_parser PRIVATE tracer_lib)

# 构建渲染器测试可执行文件
add_executable(test_tracer tests/test_tracer.cpp)
target_include_directories(test_tracer PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(test_tracer PRIVATE tracer_lib ${OpenCV_LIBS})

set(ALL_TARGETS ${PROJECT_NAME} tracer_lib test_parser test_tracer)
foreach(target ${ALL_TARGETS})
    if(MSVC)
        # 使用生成器表达式：如果【不是】Debug 模式，才应用优化选项
        target_compile_options(${target} PRIVATE 
            $<$<NOT:$<CONFIG:Debug>>:/Ox>
            $<$<NOT:$<CONFIG:Debug>>:/Ot>
            $<$<NOT:$<CONFIG:Debug>>:/fp:fast>
            /arch:AVX2 # AVX2 指令集可以在 Debug 下共存，保持不变
        )
    else()
        # 同样，类 Unix 平台下，Debug 模式也建议不加 -O3
        target_compile_options(${target} PRIVATE 
            $<$<NOT:$<CONFIG:Debug>>:-O3>
            $<$<NOT:$<CONFIG:Debug>>:-ffast-math>
            -march=native
        )
    endif()
endforeach()

# 启用 IPO（Interprocedural Optimization）以提高性能
include(CheckIPOSupported)
check_ipo_supported(RESULT result OUTPUT error)
if(result)
    set_target_properties(${PROJECT_NAME} PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()
