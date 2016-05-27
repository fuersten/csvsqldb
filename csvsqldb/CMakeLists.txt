
SET(CSVSQLDB_SOURCES
    main.cpp
    console.cpp
    console.h
)

SET(LINENOISE_SOURCES
    ${CMAKE_SOURCE_DIR}/linenoise-ng/linenoise.h
    ${CMAKE_SOURCE_DIR}/linenoise-ng/linenoise.cpp
    ${CMAKE_SOURCE_DIR}/linenoise-ng/ConvertUTF.h
    ${CMAKE_SOURCE_DIR}/linenoise-ng/ConvertUTF.cpp
    ${CMAKE_SOURCE_DIR}/linenoise-ng/ConvertUTF.cpp
    ${CMAKE_SOURCE_DIR}/linenoise-ng/wcwidth.cpp
)

IF(NOT WIN32)
# loosen strict warning handling for external dependencies
set_property(
    SOURCE ${CMAKE_SOURCE_DIR}/linenoise-ng/ConvertUTF.cpp
           ${CMAKE_SOURCE_DIR}/linenoise-ng/linenoise.cpp
           ${CMAKE_SOURCE_DIR}/linenoise-ng/wcwidth.cpp
    APPEND_STRING
    PROPERTY COMPILE_FLAGS
    "-Wno-all -Wno-error -Wno-conversion -Wno-missing-prototypes -Wno-pedantic"
)
ENDIF()

ADD_EXECUTABLE(csvsqldb-bin ${CSVSQLDB_SOURCES} ${LINENOISE_SOURCES})

IF(NOT WIN32)
    TARGET_LINK_LIBRARIES(csvsqldb-bin ${CSVSQLDB_PROJECT_LIBS} ${CSVSQLDB_PLATFORM_LIBS} ${Boost_PROGRAM_OPTIONS_LIBRARY} csvsqldb)
ELSE()
    TARGET_LINK_LIBRARIES(csvsqldb-bin ${CSVSQLDB_PROJECT_LIBS} ${CSVSQLDB_PLATFORM_LIBS} csvsqldb)
ENDIF()

SET_TARGET_PROPERTIES(csvsqldb-bin PROPERTIES OUTPUT_NAME csvsqldb)

