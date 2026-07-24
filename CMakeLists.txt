cmake_minimum_required(VERSION 3.21)
project(energy_prices, LANGUAGES CXX)
set(CMAKE_CXX_COMPILER /usr/bin/g++)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)

add_library(energyPriceCore
	src/tgeParser.cc
)

target_include_directories(energyPriceCore
	PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_compile_options(energyPriceCore
	PRIVATE
		-Wall
		-Wextra
		-Wpedantic
		-Werror
)

add_executable(energyPrices
	app/main.cc
)

target_link_libraries(energyPrices
	PRIVATE
		energyPriceCore
)

target_compile_options(energyPrices
	PRIVATE
		-Wall
		-Wextra
		-Wpedantic
		-Werror
)


enable_testing()
find_package(GTest REQUIRED)

add_executable(unitTest
	tests/tgeParserTest.cc
)

target_link_libraries(unitTest
	PRIVATE
		energyPriceCore
		GTest::gmock_main
)

target_compile_options(unitTest
	PRIVATE
		-Wall
		-Wextra
		-Wpedantic
		-Werror
)

include(GoogleTest)
gtest_discover_tests(unitTest)
option(ENABLE_ASAN "Enable AddressSanitizer" ON)



add_custom_target(
	symlink-compile-commands ALL
	COMMAND
		"${CMAKE_COMMAND}" -E rm -f
		"${CMAKE_SOURCE_DIR}/compile_commands.json"
	COMMAND
		"${CMAKE_COMMAND}" -E create_symlink
		"${CMAKE_BINARY_DIR}/compile_commands.json"
		"${CMAKE_SOURCE_DIR}/compile_commands.json"
)
