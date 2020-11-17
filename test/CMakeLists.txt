configure_file(test_util.h.in ${CMAKE_CURRENT_BINARY_DIR}/test_util.h)
set(CSVSQLDB_TEST_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/test_util.h)

if(NOT WIN32)
    set(CSVSQLDB_TEST_SOURCES ${CSVSQLDB_TEST_SOURCES} signalhandler_test.cpp glob_test.cpp)
endif()

add_executable(csvsqldbtest 
    main.cpp

    aggregation_test.cpp
    application_test.cpp
    block_test.cpp
    blockmanager_test.cpp
    buildin_functions_test.cpp
    configuration_test.cpp
    csv_parser_test.cpp
    data_framework_test.cpp
    data_test_framework.h
    date_test.cpp
    duration_test.cpp
    exception_test.cpp
    execution_plan_test.cpp
    file_mapping_test.cpp
    groupby_test.cpp
    join_test.cpp
    json_test.cpp
    lexer_test.cpp
    limit_test.cpp
    logging_test.cpp
    luaengine_test.cpp
    null_operation_test.cpp
    regexp_test.cpp
    row_processing_test.cpp
    sort_operation_test.cpp
    sql_parser_test.cpp
    stackmachine_test.cpp
    stringutil_test.cpp
    subquery_test.cpp
    symboltable_test.cpp
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
    values_test.cpp
    variant_test.cpp

    ${CSVSQLDB_TEST_SOURCES}
)

if(WIN32)
    target_include_directories(csvsqldbtest PRIVATE detail/windows)
else()
    target_include_directories(csvsqldbtest PRIVATE detail/posix)
endif()

target_include_directories(csvsqldbtest PRIVATE ${PROJECT_SOURCE_DIR}/thirdparty/Catch2/single_include)
target_link_libraries(csvsqldbtest ${CSVSQLDB_PROJECT_LIBS} ${CSVSQLDB_PLATFORM_LIBS} csvsqldb)

include(ParseAndAddCatchTests)
ParseAndAddCatchTests(csvsqldbtest)
