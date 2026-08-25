function(add_qt_test name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} PRIVATE artillery_core artillery_coreplugin Qt6::Test Qt6::Quick)
    add_test(NAME ${name} COMMAND ${name})
    set_tests_properties(${name} PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
endfunction()

add_qt_test(test_gameengine)
