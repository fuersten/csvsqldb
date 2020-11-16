
set(CSVSQLDB_SOURCES
)

set(LINENOISE_SOURCES
    ${CMAKE_SOURCE_DIR}/linenoise-ng/linenoise.h
    ${CMAKE_SOURCE_DIR}/linenoise-ng/linenoise.cpp
    ${CMAKE_SOURCE_DIR}/linenoise-ng/ConvertUTF.h
    ${CMAKE_SOURCE_DIR}/linenoise-ng/ConvertUTF.cpp
    ${CMAKE_SOURCE_DIR}/linenoise-ng/ConvertUTF.cpp
    ${CMAKE_SOURCE_DIR}/linenoise-ng/wcwidth.cpp
)

if(NOT WIN32)
# loosen strict warning handling for external dependencies
set_property(
    SOURCE ${CMAKE_SOURCE_DIR}/linenoise-ng/ConvertUTF.cpp
           ${CMAKE_SOURCE_DIR}/linenoise-ng/linenoise.cpp
           ${CMAKE_SOURCE_DIR}/linenoise-ng/wcwidth.cpp
    APPEND_STRING
    PROPERTY COMPILE_FLAGS
    "-Wno-all -Wno-error -Wno-conversion -Wno-missing-prototypes -Wno-pedantic"
)
endif()

add_executable(csvsqldb-bin 
    main.cpp

    console.cpp
    console.h

    ${LINENOISE_SOURCES}
)

target_link_libraries(csvsqldb-bin ${CSVSQLDB_PROJECT_LIBS} ${CSVSQLDB_PLATFORM_LIBS} csvsqldb CLI11::CLI11)

set_target_properties(csvsqldb-bin PROPERTIES OUTPUT_NAME csvsqldb)

