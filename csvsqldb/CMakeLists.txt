
set(LINENOISE_SOURCES
    ${CMAKE_SOURCE_DIR}/thirdparty/linenoise-ng/include/linenoise.h
    ${CMAKE_SOURCE_DIR}/thirdparty/linenoise-ng/src/linenoise.cpp
    ${CMAKE_SOURCE_DIR}/thirdparty/linenoise-ng/src/ConvertUTF.h
    ${CMAKE_SOURCE_DIR}/thirdparty/linenoise-ng/src/ConvertUTF.cpp
    ${CMAKE_SOURCE_DIR}/thirdparty/linenoise-ng/src/ConvertUTF.cpp
    ${CMAKE_SOURCE_DIR}/thirdparty/linenoise-ng/src/wcwidth.cpp
)

if(NOT WIN32)
# loosen strict warning handling for external dependencies
set_property(
    SOURCE ${CMAKE_SOURCE_DIR}/thirdparty/linenoise-ng/src/ConvertUTF.cpp
           ${CMAKE_SOURCE_DIR}/thirdparty/linenoise-ng/src/linenoise.cpp
           ${CMAKE_SOURCE_DIR}/thirdparty/linenoise-ng/src/wcwidth.cpp
    APPEND_STRING
    PROPERTY COMPILE_FLAGS
    "-Wno-all -Wno-error -Wno-conversion -Wno-pedantic"
)
endif()

source_group("Source Files\\linenoise" FILES ${LINENOISE_SOURCES})

add_executable(csvsqldb-bin 
    main.cpp

    console.cpp
    console.h

    ${LINENOISE_SOURCES}
)

target_include_directories(csvsqldb-bin PRIVATE ${CMAKE_SOURCE_DIR}/thirdparty/linenoise-ng/include/)
target_link_libraries(csvsqldb-bin ${CSVSQLDB_PROJECT_LIBS} ${CSVSQLDB_PLATFORM_LIBS} csvsqldb CLI11::CLI11)

set_target_properties(csvsqldb-bin PROPERTIES OUTPUT_NAME csvsqldb)

