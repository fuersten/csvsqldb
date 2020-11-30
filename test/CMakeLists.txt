configure_file(test_util.h.in ${CMAKE_CURRENT_BINARY_DIR}/test_util.h)

if(NOT WIN32)
    set(CSVSQLDB_TEST_SOURCES signalhandler_test.cpp glob_test.cpp)
endif()

add_executable(csvsqldbtest 
    main.cpp

    aggregation_functions_test.cpp
    aggregation_test.cpp
    application_test.cpp
    block_iterator_test.cpp
    block_provider_test.cpp
    block_test.cpp
    block_manager_test.cpp
    buildin_functions_test.cpp
    configuration_test.cpp
    csv_parser_test.cpp
    csv_string_parser_test.cpp
    data_framework_test.cpp
    database_system_test.cpp
    database_test.cpp
    date_test.cpp
    duration_test.cpp
    exception_test.cpp
    execution_engine_test.cpp
    execution_plan_creator_test.cpp
    execution_plan_test.cpp
    file_mapping_test.cpp
    groupby_test.cpp
    join_test.cpp
    lexer_test.cpp
    limit_test.cpp
    logging_test.cpp
    luaengine_test.cpp
    null_operation_test.cpp
    operatornode_factory_test.cpp
    regexp_test.cpp
    row_processing_test.cpp
    sort_operation_test.cpp
    sql_lexer_test.cpp
    sql_parser_test.cpp
    stackmachine_test.cpp
    stringutil_test.cpp
    subquery_test.cpp
    symboltable_test.cpp
    table_executions_test.cpp
    tabledata_test.cpp
    threadpool_test.cpp
    threadutil_test.cpp
    time_helper_test.cpp
    time_test.cpp
    timestamp_test.cpp
    tribool_test.cpp
    typeoperations_test.cpp
    types_test.cpp
    union_test.cpp
    valint_test.cpp
    utils_test.cpp
    values_test.cpp
    variant_test.cpp

    ${CSVSQLDB_TEST_SOURCES}

    data_test_framework.h
    temporary_directory.cpp
    temporary_directory.h
    test_helper.h
    ${CMAKE_CURRENT_BINARY_DIR}/test_util.h
)

if(WIN32)
    target_include_directories(csvsqldbtest PRIVATE detail/windows)
else()
    target_include_directories(csvsqldbtest PRIVATE detail/posix)
endif()

target_include_directories(csvsqldbtest PRIVATE ${PROJECT_SOURCE_DIR}/thirdparty/Catch2/single_include)
target_include_directories(csvsqldbtest PRIVATE ${CMAKE_SOURCE_DIR})
target_include_directories(csvsqldbtest PRIVATE ${CMAKE_BINARY_DIR})
target_include_directories(csvsqldbtest PRIVATE SYSTEM ${LUA_INCLUDE_DIR})
target_link_libraries(csvsqldbtest PRIVATE ${CSVSQLDB_PLATFORM_LIBS} csvsqldb ${LUA_LIBRARY})

if(COVERAGE)
  target_link_libraries(csvsqldbtest PUBLIC --coverage)
  target_compile_options(csvsqldbtest PUBLIC --coverage -g)
else()
  target_compile_options(csvsqldbtest PRIVATE ${CSVSQLDB_COMPILE_OPTIONS})
endif()

include(ParseAndAddCatchTests)
ParseAndAddCatchTests(csvsqldbtest)
