add_executable(sandbox main.cpp)

target_compile_features(sandbox INTERFACE cxx_std_17)
target_include_directories(sandbox SYSTEM PRIVATE ${ctre_SOURCE_DIR}/include)
