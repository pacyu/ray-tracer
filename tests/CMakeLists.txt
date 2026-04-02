# 定义所有测试
set(TEST_NAMES test_parser test_tracer test_ocean test_dynamic_ocean test_ripple test_cloth)

foreach(t_name ${TEST_NAMES})
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${t_name}.cpp")
        add_executable(${t_name} "${t_name}.cpp")
        target_link_libraries(${t_name} PRIVATE tracer_lib)
        
        apply_optimizations(${t_name})
    endif()
endforeach()