# CMakeList.txt : CMake project for libpqxx, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.10)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("ClientDB")

# Add source to this project's executable.
add_executable (ClientDB "main.cpp" "main.h")

set(PostgreSQL_ROOT "C:/Program Files/PostgreSQL/18")
find_package(PostgreSQL REQUIRED)

add_subdirectory("C:/Users/Administrator/Desktop/cpp practice/libpqxx-8.0.2" libpqxx-build)

target_compile_features(ClientDB PRIVATE cxx_std_20)

target_link_libraries(ClientDB pqxx)


# TODO: Add tests and install targets if needed.
