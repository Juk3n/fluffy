cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
project(fluffy LANGUAGES CXX VERSION 1.0.0)

include(FetchContent)

FetchContent_Declare(googletest
  GIT_REPOSITORY https://github.com/google/googletest
  GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

#find_program(CLANG_TIDY NAMES "clang-tidy" REQUIRED)
#set(CLANG_TIDY_COMMAND "${CLANG_TIDY}" "-checks=-*,modernize-*")

find_package(SQLite3 REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/include)
enable_testing()
add_executable(fluffy src/main.cpp src/fluffyApplication.cpp src/gameRepository.cpp)

add_executable(
  tests
  tests/gameTest.cpp
)
target_link_libraries(
  tests
  GTest::gtest_main
)

target_link_libraries(fluffy
  PRIVATE SQLite::SQLite3
)
#set_target_properties(fluffy PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")

if (EMSCRIPTEN) 
  string(APPEND CMAKE_CXX_FLAGS " -s USE_PTHREADS") 
  string(APPEND CMAKE_EXE_LINKER_FLAGS " -s ASYNCIFY") 
  string(APPEND CMAKE_EXE_LINKER_FLAGS " -s PROXY_TO_PTHREAD") 

  foreach(file "index.html" "run_webassembly.py")
    configure_file("src/${file}" ${file})
  endforeach(file)
endif() 

include(GoogleTest)
gtest_discover_tests(tests)
