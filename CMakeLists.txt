cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

project( mammon VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -m64 -ffast-math")
set(CMAKE_CXX_FLAGS_HOME "${CMAKE_CXX_FLAGS} -O3 -g -march=native -m64 -ffast-math")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -march=native -g -m64 -ffast-math")


find_program(NASM_EXECUTABLE NAMES yasm REQUIRED)
enable_language(ASM_NASM)

set(CMAKE_ASM_NASM_COMPILER "${NASM_EXECUTABLE}")

if(${CMAKE_BUILD_TYPE} STREQUAL "HOME" OR ${CMAKE_BUILD_TYPE} STREQUAL "DEBUG")
    set(CMAKE_ASM_NASM_FLAGS "${ASM_NASM_FLAGS} -X gnu -g dwarf2")
endif()

message(STATUS "Build type ${CMAKE_BUILD_TYPE}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lstdc++")

set( CMAKE_EXPORT_COMPILE_COMMANDS ON ) #youcompleteme
set( CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Snappy REQUIRED)
if(Snappy_FOUND)
	message(STATUS "Found Snappy")
endif()


find_package(yaml-cpp REQUIRED)
if(yaml-cpp_FOUND)
	message(STATUS "Found Yaml-cpp")
endif()

option(MINIMAL "Enable minimal network config")
if(MINIMAL)
	message(STATUS "Running minimal config")
	configure_file(config/minimal.hpp.in "${CMAKE_SOURCE_DIR}/include/config.hpp")
else()
	message(STATUS "Running mainnet config")
	configure_file(config/mainnet.hpp.in "${CMAKE_SOURCE_DIR}/include/config.hpp")
endif()

option(CPPCHECK "Analyze the code with cppcheck")
if(CPPCHECK)
	message(STATUS "Building with cppcheck")
	set(CMAKE_CXX_CPPCHECK "cppcheck")
	list(
		APPEND CMAKE_CXX_CPPCHECK
		"--enable=all"
		"--suppress=duplicateBranch"
		"--inline-suppr"
		"--inconclusive"
	)
endif()

option(TIDY "Analyze the code with clang-tidy")
if(TIDY)
	message(STATUS "Building with clang-tidy")
	set(CMAKE_CXX_CLANG_TIDY "clang-tidy")
endif()

add_library( ssz OBJECT 
            common/bitlist.cpp
            ssz/hasher.cpp
            ssz/hashtree.cpp
            ssz/sha256_shani.asm
            ssz/sha256_avx_one_block.asm
            ssz/sha256_avx.asm
            ssz/sha256_avx2.asm
            ssz/ssz_container.cpp
            beacon-chain/validator.cpp
          )
target_include_directories(ssz PUBLIC "${CMAKE_SOURCE_DIR}/include")
target_link_libraries( ssz snappy yaml-cpp )

add_executable ( beacon-chain $<TARGET_OBJECTS:ssz> beacon-chain/main.cpp )
target_include_directories(beacon-chain PUBLIC "${CMAKE_SOURCE_DIR}/include")
target_link_libraries( beacon-chain snappy yaml-cpp )

add_executable( test_ssz $<TARGET_OBJECTS:ssz>  beacon-chain/test/test_ssz.cpp )
target_include_directories(test_ssz PUBLIC "${CMAKE_SOURCE_DIR}/include" )
target_link_libraries(test_ssz snappy yaml-cpp)

add_executable( test_bytes $<TARGET_OBJECTS:ssz> common/bytes_test.cpp )
target_include_directories( test_bytes PUBLIC "${CMAKE_SOURCE_DIR}/include" )
target_link_libraries(test_bytes yaml-cpp)

add_executable(test_sha256
               ssz/hasher.cpp
               ssz/sha256_avx_one_block.asm
               ssz/sha256_avx.asm
               ssz/sha256_avx2.asm
               ssz/sha256_shani.asm
               ssz/test_sha256.cpp
              )
target_compile_definitions(test_sha256 PUBLIC CUSTOM_HASHER)

set(CMAKE_HASHER_IMPL_LIST "SSE;AVX;AVX2;SHA")
foreach(CMAKE_HASHER_IMPL ${CMAKE_HASHER_IMPL_LIST})
  configure_file(bench/bench_sha256.cpp.in "bench/bench_sha256_${CMAKE_HASHER_IMPL}.cpp")
  add_executable(bench_sha256_${CMAKE_HASHER_IMPL}
                  ssz/ssz_container.cpp
                  common/bitlist.cpp
                  beacon-chain/validator.cpp
                  ssz/sha256_shani.asm
                  ssz/sha256_avx.asm
                  ssz/sha256_avx_one_block.asm
                  ssz/sha256_avx2.asm
                  ssz/hasher.cpp
                  ssz/hashtree.cpp
                  bench/bench_sha256_impl.cpp
                  bench/bench_sha256_${CMAKE_HASHER_IMPL}.cpp
                  )

  target_include_directories( bench_sha256_${CMAKE_HASHER_IMPL} PUBLIC "${CMAKE_SOURCE_DIR}/include" )
  target_link_libraries( bench_sha256_${CMAKE_HASHER_IMPL} yaml-cpp )
  target_compile_definitions(bench_sha256_${CMAKE_HASHER_IMPL} PUBLIC CUSTOM_HASHER)
endforeach(CMAKE_HASHER_IMPL)

enable_testing()
add_test(test_ssz test_ssz)
add_test(test_bytes test_bytes)
add_test(test_sha256 test_sha256)
