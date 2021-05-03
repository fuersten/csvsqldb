FetchContent_Declare(
  reflex
  URL https://github.com/Genivia/RE-flex/archive/refs/tags/v3.0.3.tar.gz
)

FetchContent_GetProperties(reflex)
if(NOT reflex_POPULATED)
  FetchContent_Populate(reflex)
endif()

