FetchContent_Declare(
  ctre
  GIT_REPOSITORY https://github.com/hanickadot/compile-time-regular-expressions.git
)

FetchContent_GetProperties(ctre)
if(NOT ctre_POPULATED)
  FetchContent_Populate(ctre)
endif()

