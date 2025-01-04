cmake_minimum_required(VERSION 3.13)
project(LLParseGrammar VERSION 1.0
        DESCRIPTION "Program to parse a grammer from LL(1) parsig table"
        LANGUAGES C CXX)

# set everything up for c++ 17 features
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# test that filesystem header actually is there and works
try_compile(HAS_FS "${CMAKE_BINARY_DIR}/temp"
        "${CMAKE_SOURCE_DIR}/cmake/has_stdfs.cc"
        CMAKE_FLAGS -DCMAKE_CXX_STANDARD=17 -DCMAKE_CXX_STANDARD_REQUIRED=ON )

#message("Supported features = ${CMAKE_CXX_COMPILE_FEATURES}")
if (HAS_FS)
    message(STATUS "std::filesystem supported")
else()
    message(STATUS "Using std::experimental::filesystem instead of std::filesystem")
    #set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -lstdc++fs")
    add_compile_definitions(USE_EXP_FILESYSTEM)
endif()

# Target LLParse
add_executable(LLParse 
        src/main.cpp
        src/main.h 
        src/llparsetable.cpp 
        src/llparsetable.h 
        src/parsegrammar.cpp 
        src/parsegrammar.h 
         ../LLScanner/Utilities/Utilities.cpp
         ../LLScanner/Utilities/Utilities.h
        )

target_compile_features(LLParse PUBLIC cxx_std_17)

target_include_directories(LLParse PRIVATE
        $<BUILD_INTERFACE:${LLParseGrammar_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${LLParseGrammar_SOURCE_DIR}/../LLScanner>
        )

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # using Clang
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # using GCC
    message(STATUS "Console has FS: ${HAS_FS}")
    set(CXX_FILESYSTEM_LIBRARIES "stdc++fs")
 #   set(THREADS_PREFER_PTHREAD_FLAG ON)
 #   find_package(Threads REQUIRED)
    target_link_libraries(LLParse PRIVATE ${CMAKE_DL_LIBS} ${CXX_FILESYSTEM_LIBRARIES} ${SQLITE3_LIBRARIES} LLScanner)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    # using Intel C++
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # using Visual Studio C++
 #   target_link_libraries(LLParse ${SQLITE3_LIBRARIES} NetworkServer)
endif()

add_library(LLScanner
  ../LLScanner/Scanner/scanner.cpp 
  ../LLScanner/Scanner/scanner.h 
  ../LLScanner/FileScan/FileScan.cpp 
  ../LLScanner/FileScan/FileScan.h 
  ../LLScanner/Common/io_utility.cpp 
  ../LLScanner/Common/io_utility.h
)

target_compile_features(LLScanner PUBLIC cxx_std_17)

target_include_directories(LLScanner PRIVATE
        $<BUILD_INTERFACE:${LLParseGrammar_SOURCE_DIR}/include>
        )

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # using Clang
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # using GCC
    set(CXX_FILESYSTEM_LIBRARIES "stdc++fs")
    target_link_libraries(LLScanner PRIVATE ${CMAKE_DL_LIBS} ${CXX_FILESYSTEM_LIBRARIES})
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    # using Intel C++
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # using Visual Studio C++
    #target_link_libraries(NetCS ${NetworkCS_BINARY_DIR}/lib/StorageModule.lib)
    target_link_libraries(LLScanner ws2_32)
endif()



