SET(CSVSQLDB_TEST_SOURCES
    main.cpp

    test.cpp
    test.h

    aggregation_test.cpp
    any_test.cpp
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
    row_processing_test.cpp
    sort_operation_test.cpp
    subquery_test.cpp
    sql_parser_test.cpp
    stackmachine_test.cpp
    stringutil_test.cpp
    symboltable_test.cpp
    tabledata_test.cpp
    threadpool_test.cpp
    threadutil_test.cpp
    time_test.cpp
    time_helper_test.cpp
    timestamp_test.cpp
    tribool_test.cpp
    typeoperations_test.cpp
    types_test.cpp
    union_test.cpp
    values_test.cpp
    variant_test.cpp
    regexp_test.cpp
)

IF(WIN32)
    INCLUDE_DIRECTORIES(detail/windows)
ELSE()
    INCLUDE_DIRECTORIES(detail/posix)
    SET(CSVSQLDB_TEST_SOURCES  ${CSVSQLDB_TEST_SOURCES} signalhandler_test.cpp glob_test.cpp)
ENDIF()

CONFIGURE_FILE(test_util.h.in ${CMAKE_CURRENT_BINARY_DIR}/test_util.h)
SET(CSVSQLDB_TEST_SOURCES ${CSVSQLDB_TEST_SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/test_util.h)

ADD_EXECUTABLE(csvsqldbtest ${CSVSQLDB_TEST_SOURCES})

IF(CMAKE_BUILD_TYPE MATCHES "Debug")
	TARGET_LINK_LIBRARIES(csvsqldbtest ${CSVSQLDB_PROJECT_LIBS} ${CSVSQLDB_PLATFORM_LIBS} csvsqldb)
ELSE()
	TARGET_LINK_LIBRARIES(csvsqldbtest ${CSVSQLDB_PROJECT_LIBS} ${CSVSQLDB_PLATFORM_LIBS} csvsqldb)
ENDIF()
