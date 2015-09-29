
SET(LIB_CSVSQLDB_SOURCES
    aggregation_functions.cpp
    block.cpp
    block_iterator.cpp
    buildin_functions.cpp
    database.cpp
    execution_engine.cpp
    execution_plan.cpp
    execution_plan_creator.cpp
    file_mapping.cpp
    function_registry.cpp
    operatornode.cpp
    operatornode_factory.cpp
    sql_lexer.cpp
    sql_parser.cpp
    stack_machine.cpp
    symboltable.cpp
    table_executions.cpp
    tabledata.cpp
    typeoperations.cpp
    types.cpp
    validation_visitor.cpp
    values.cpp
    variant.cpp

    aggregation_functions.h
    block.h
    block_iterator.h
    buildin_functions.h
    database.h
    execution_engine.h
    execution_plan.h
    execution_plan_creator.h
    file_mapping.h
    function_registry.h
    operatornode.h
    operatornode_factory.h
    sql_ast.h
    sql_astdump.h
    sql_astexpressionvisitor.h
    sql_lexer.h
    sql_parser.h
    stack_machine.h
    symboltable.h
    table_executions.h
    tabledata.h
    typeoperations.h
    types.h
    validation_visitor.h
    values.h
    variant.h
    visitor.h
    inc.h
)

SET(LIB_CSVSQLDB_BASE_SOURCES
    base/application.cpp
    base/configuration.cpp
    base/csv_parser.cpp
    base/date.cpp
    base/default_configuration.cpp
    base/duration.cpp
    base/exception.cpp
    base/float_helper.cpp
    base/global_configuration.cpp
    base/json_object.cpp
    base/json_parser.cpp
    base/lexer.cpp
    base/log_devices.cpp
    base/logging.cpp
    base/lua_configuration.cpp
    base/string_helper.cpp
    base/thread_helper.cpp
    base/thread_pool.cpp
    base/time.cpp
    base/timestamp.cpp
    base/time_helper.cpp
    base/time_measurement.cpp
    base/types.cpp
    base/regexp.cpp
)

SET(LIB_CSVSQLDB_BASE_HEADERS
    base/any.h
    base/application.h
    base/configuration.h
    base/csv_parser.h
    base/date.h
    base/default_configuration.h
    base/duration.h
    base/exception.h
    base/float_helper.h
    base/function_traits.h
    base/glob.h
    base/global_configuration.h
    base/hash_helper.h
    base/json_object.h
    base/json_parser.h
    base/lexer.h
    base/log_devices.h
    base/logging.h
    base/lua_configuration.h
    base/lua_engine.h
    base/signalhandler.h
    base/string_helper.h
    base/thread_helper.h
    base/thread_pool.h
    base/time.h
    base/timestamp.h
    base/time_helper.h
    base/time_measurement.h
    base/tribool.h
    base/types.h
    base/regexp.h
)

SET(LIB_CSVSQLDB_COMPAT_HEADERS
    compat/get_time.h
    compat/put_time.h
)

IF(NOT APPLE AND UNIX)
    SET(LIB_CSVSQLDB_BASE_SOURCES ${LIB_CSVSQLDB_BASE_SOURCES}
        base/detail/posix/glob.cpp
        base/detail/posix/signalhandler.cpp
    )
ELSEIF(APPLE)
    SET(LIB_CSVSQLDB_BASE_SOURCES ${LIB_CSVSQLDB_BASE_SOURCES}
        base/detail/posix/glob.cpp
        base/detail/posix/signalhandler.cpp
    )
ELSEIF(WIN32)
    SET(LIB_CSVSQLDB_BASE_SOURCES ${LIB_CSVSQLDB_BASE_SOURCES}
        base/detail/windows/glob.cpp
        base/detail/windows/signalhandler.cpp)
ENDIF()

SOURCE_GROUP("Source Files\\base" FILES ${LIB_CSVSQLDB_BASE_SOURCES})
SOURCE_GROUP("Header Files\\base" FILES ${LIB_CSVSQLDB_BASE_HEADERS})
SOURCE_GROUP("Header Files\\compat" FILES ${LIB_CSVSQLDB_COMPAT_HEADERS})

CONFIGURE_FILE(version.h.in ${CMAKE_CURRENT_BINARY_DIR}/version.h)
SET(LIB_CSVSQLDB_SOURCES 
    ${LIB_CSVSQLDB_SOURCES}
    ${LIB_CSVSQLDB_BASE_SOURCES}
    ${LIB_CSVSQLDB_BASE_HEADERS}
    ${LIB_CSVSQLDB_COMPAT_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/version.h
)

ADD_LIBRARY(csvsqldb SHARED ${LIB_CSVSQLDB_SOURCES})
SET_TARGET_PROPERTIES(csvsqldb PROPERTIES DEFINE_SYMBOL LIBCSVSQLDB VERSION ${CSVSQLDB_VERSION} SOVERSION ${CSVSQLDB_VERSION_SHORT})
IF(NOT WIN32)
    TARGET_LINK_LIBRARIES(csvsqldb ${CSVSQLDB_PROJECT_LIBS} ${CSVSQLDB_PLATFORM_LIBS} ${LUA_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${Boost_REGEX_LIBRARY} ${Readline_LIBRARY})
ELSE()
    TARGET_LINK_LIBRARIES(csvsqldb ${CSVSQLDB_PROJECT_LIBS} ${CSVSQLDB_PLATFORM_LIBS} ${LUA_LIBRARY})
ENDIF()
INSTALL(TARGETS csvsqldb LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
