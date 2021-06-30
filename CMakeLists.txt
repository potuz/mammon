cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)

project( mammon VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set( CMAKE_EXPORT_COMPILE_COMMANDS ON ) #youcompleteme
set(CMAKE_INCLUDE_CURRENT_DIR ON)

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
	configure_file(config/minimal.hpp include/config.hpp COPYONLY)
else()
	message(STATUS "Running mainnet config")
	configure_file(config/mainnet.hpp include/config.hpp COPYONLY)
endif()

add_executable (beacon-chain
        common/bitlist.cpp
        ssz/ssz_container.cpp
        beacon-chain/beacon_block.cpp
	beacon-chain/main.cpp
	)


target_include_directories(beacon-chain PUBLIC
			   "${CMAKE_SOURCE_DIR}/include"
			   "${CMAKE_BINARY_DIR}/include"
			  )


target_link_libraries(beacon-chain
                      snappy
                      yaml-cpp)

add_executable(test_ssz
                common/bitlist.cpp
                beacon-chain/beacon_block.cpp
                ssz/ssz_container.cpp
	 	beacon-chain/test/test_ssz.cpp
		)

target_include_directories(test_ssz PUBLIC
			   "${CMAKE_SOURCE_DIR}/include"
			   "${CMAKE_BINARY_DIR}/include"
			  )

target_link_libraries(test_ssz
                      snappy
                      yaml-cpp)

add_executable(test_bytes
                ssz/ssz_container.cpp
                common/bytes_test.cpp)

target_include_directories(test_bytes PUBLIC
			   "${CMAKE_SOURCE_DIR}/include"
			   "${CMAKE_BINARY_DIR}/include"
			  )

target_link_libraries(test_bytes
                      yaml-cpp)
enable_testing()

add_test(test_ssz test_ssz)
add_test(test_bytes test_bytes)
