cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

project( mammon VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS_HOME "${CMAKE_CXX_FLAGS} -O3 -march=native -m64 -ffast-math")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -march=native -m64")

message(STATUS "Build type ${CMAKE_BUILD_TYPE}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lstdc++")

set( CMAKE_EXPORT_COMPILE_COMMANDS ON ) #youcompleteme
set( CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(OpenSSL REQUIRED)
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

add_executable (beacon-chain
        common/bitlist.cpp
	ssz/hashtree.cpp
        ssz/ssz_container.cpp
	beacon-chain/main.cpp
	)


target_include_directories(beacon-chain PUBLIC
			   "${CMAKE_SOURCE_DIR}/include")


target_link_libraries(beacon-chain
	 	      crypto
                      snappy
                      yaml-cpp)

add_executable(test_ssz
                ssz/ssz_container.cpp
	 	beacon-chain/test/test_ssz.cpp
                common/bitlist.cpp
                ssz/hashtree.cpp
		)

target_include_directories(test_ssz PUBLIC
			   "${CMAKE_SOURCE_DIR}/include"
			  )

target_link_libraries(test_ssz
                      crypto
                      snappy
                      yaml-cpp)

add_executable(test_bytes
                ssz/ssz_container.cpp
                ssz/hashtree.cpp
                common/bytes_test.cpp)

target_include_directories(test_bytes PUBLIC
			   "${CMAKE_SOURCE_DIR}/include"
			  )

target_link_libraries(test_bytes
		      crypto
                      yaml-cpp)
enable_testing()

add_test(test_ssz test_ssz)
add_test(test_bytes test_bytes)
