
set(LIB_CSVSQLDB_SOURCES
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
    operatornode_factory.cpp
    sql_lexer.cpp
    sql_parser.cpp
    stack_machine.cpp
    symboltable.cpp
    table_executions.cpp
    tabledata.cpp
    tokens.cpp
    typeoperations.cpp
    types.cpp
    validation_visitor.cpp
    values.cpp
    variant.cpp

    lex.yy.cpp
    lex.yy.h

    operatornodes/aggregation_operatornode.cpp
    operatornodes/aggregation_operatornode.h
    operatornodes/cross_join_operatornode.cpp
    operatornodes/cross_join_operatornode.h
    operatornodes/extended_projection_operatornode.cpp
    operatornodes/extended_projection_operatornode.h
    operatornodes/grouping_operatornode.cpp
    operatornodes/grouping_operatornode.h
    operatornodes/inner_hash_join_operatornode.cpp
    operatornodes/inner_hash_join_operatornode.h
    operatornodes/inner_join_operatornode.cpp
    operatornodes/inner_join_operatornode.h
    operatornodes/limit_operatornode.cpp
    operatornodes/limit_operatornode.h
    operatornodes/output_row_operatornode.cpp
    operatornodes/output_row_operatornode.h
    operatornodes/scan_operatornode.cpp
    operatornodes/scan_operatornode.h
    operatornodes/select_operatornode.cpp
    operatornodes/select_operatornode.h
    operatornodes/sort_operatornode.cpp
    operatornodes/sort_operatornode.h
    operatornodes/systemtable_scan_operatornode.cpp
    operatornodes/systemtable_scan_operatornode.h
    operatornodes/table_scan_operatornode.cpp
    operatornodes/table_scan_operatornode.h
    operatornodes/union_operatornode.cpp
    operatornodes/union_operatornode.h

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
    tokens.h
    typeoperations.h
    types.h
    validation_visitor.h
    values.h
    variant.h
    visitor.h
    inc.h
)

set(LIB_CSVSQLDB_BASE_SOURCES
    base/application.cpp
    base/configuration.cpp
    base/csv_parser.cpp
    base/csv_string_parser.cpp
    base/date.cpp
    base/default_configuration.cpp
    base/duration.cpp
    base/exception.cpp
    base/global_configuration.cpp
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

set(LIB_CSVSQLDB_BASE_HEADERS
    base/application.h
    base/configuration.h
    base/csv_parser.h
    base/csv_string_parser.h
    base/date.h
    base/default_configuration.h
    base/duration.h
    base/exception.h
    base/float_helper.h
    base/function_traits.h
    base/glob.h
    base/global_configuration.h
    base/hash_helper.h
    base/json.h
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
    base/detail/types_detail.h
)

set(LIB_CSVSQLDB_COMPAT_HEADERS
    compat/get_time.h
    compat/put_time.h
    compat/time_api.h
)

if(NOT APPLE AND UNIX)
    set(LIB_CSVSQLDB_BASE_SOURCES ${LIB_CSVSQLDB_BASE_SOURCES}
        base/detail/posix/glob.cpp
        base/detail/posix/signalhandler.cpp
    )
elseif(APPLE)
    set(LIB_CSVSQLDB_BASE_SOURCES ${LIB_CSVSQLDB_BASE_SOURCES}
        base/detail/posix/glob.cpp
        base/detail/posix/signalhandler.cpp
    )
elseif(WIN32)
    set(LIB_CSVSQLDB_BASE_SOURCES ${LIB_CSVSQLDB_BASE_SOURCES}
        base/detail/windows/glob.cpp
        base/detail/windows/signalhandler.cpp)
endif()

source_group("Source Files\\base" FILES ${LIB_CSVSQLDB_BASE_SOURCES})
source_group("Header Files\\base" FILES ${LIB_CSVSQLDB_BASE_HEADERS})
source_group("Header Files\\compat" FILES ${LIB_CSVSQLDB_COMPAT_HEADERS})

configure_file(version.h.in ${CMAKE_CURRENT_BINARY_DIR}/version.h)
set(LIB_CSVSQLDB_SOURCES 
    ${LIB_CSVSQLDB_SOURCES}
    ${LIB_CSVSQLDB_BASE_SOURCES}
    ${LIB_CSVSQLDB_BASE_HEADERS}
    ${LIB_CSVSQLDB_COMPAT_HEADERS}
    ${CMAKE_CURRENT_BINARY_DIR}/version.h
)

add_library(libcsvsqldb SHARED ${LIB_CSVSQLDB_SOURCES})

target_include_directories(libcsvsqldb PRIVATE ${CMAKE_SOURCE_DIR})
target_include_directories(libcsvsqldb PRIVATE ${CMAKE_BINARY_DIR})
target_include_directories(libcsvsqldb PRIVATE ${CMAKE_SOURCE_DIR}/csvsqldb)
target_include_directories(libcsvsqldb PRIVATE ${CMAKE_SOURCE_DIR}/thirdparty/json/single_include/)
target_include_directories(libcsvsqldb PRIVATE SYSTEM ${lua_SOURCE_DIR})
target_include_directories(libcsvsqldb SYSTEM PRIVATE ${ctre_SOURCE_DIR}/include)
target_include_directories(libcsvsqldb PRIVATE SYSTEM /usr/local/Cellar/re-flex/3.0.2/include)

target_link_directories(libcsvsqldb PUBLIC /usr/local/Cellar/re-flex/3.0.2/lib)

set_target_properties(libcsvsqldb PROPERTIES DEFINE_SYMBOL LIBCSVSQLDB VERSION ${CSVSQLDB_VERSION} SOVERSION ${CSVSQLDB_VERSION_SHORT})
target_link_libraries(libcsvsqldb PUBLIC ${CSVSQLDB_PLATFORM_LIBS} lua reflex)

if(WIN32)
  set(CSVSQLDB_COMPILE_OPTIONS "${CSVSQLDB_COMPILE_OPTIONS} -DLIBCSVSQLDB")
endif()

if(COVERAGE)
  target_link_libraries(libcsvsqldb PUBLIC --coverage)
  target_compile_options(libcsvsqldb PUBLIC --coverage -g)
else()
  target_compile_options(libcsvsqldb PRIVATE ${CSVSQLDB_COMPILE_OPTIONS})
endif()

if(NOT WIN32)
  set_target_properties(libcsvsqldb PROPERTIES OUTPUT_NAME csvsqldb)
endif()

install(TARGETS libcsvsqldb LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
