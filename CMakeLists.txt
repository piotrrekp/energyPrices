cmake_minimum_required(VERSION 3.21)
project(energy_prices, LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)

add_library(energyPriceCore
	src/htmlTableExtractor.cc
	src/httpClient.cc
	src/priceService.cc
	src/stringUtils.cc
	src/tgeParser.cc
	src/tgeUrlBuilder.cc
	src/tgePriceProvider.cc
)

target_include_directories(energyPriceCore
	PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/src"
)

find_path(ASIO_INCLUDE_DIR asio.hpp)

if(NOT ASIO_INCLUDE_DIR)
    message(FATAL_ERROR
        "Asio not found. Install it with: sudo apt install libasio-dev"
    )
endif()

include(FetchContent)

FetchContent_Declare(
    Crow
    GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
    GIT_TAG v1.2.1
)

FetchContent_MakeAvailable(Crow)

find_package(LibXml2 REQUIRED)
find_package(CURL REQUIRED)

target_link_libraries(energyPriceCore
	PRIVATE
	LibXml2::LibXml2
	CURL::libcurl
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

add_executable(energyPricesServer
	app/server.cc
	src/energyPricesServer.cc
	src/priceService.cc
)

target_link_libraries(energyPricesServer
	PRIVATE
		energyPriceCore
		Crow::Crow
)

target_compile_options(energyPricesServer
	PRIVATE
		-Wall
		-Wextra
		-Wpedantic
		-Werror
)

enable_testing()
find_package(GTest REQUIRED)

add_executable(unitTest
	src/energyPricesServer.cc
	tests/unit/tgeParserTest.cc
	tests/unit/htmlTableExtractorTest.cc
	tests/unit/httpClientTests.cc
	tests/unit/tgeUrlBuilderTests.cc
	tests/unit/stringUtilsTests.cc
	tests/unit/priceServiceTests.cc
	tests/unit/energyPriceServerTests.cc
)

target_link_libraries(unitTest
	PRIVATE
		energyPriceCore
		GTest::gmock_main
		LibXml2::LibXml2
		Crow::Crow
)

target_compile_definitions(unitTest
	PRIVATE
		TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/tests/data"
)

target_compile_options(unitTest
	PRIVATE
		-Wall
		-Wextra
		-Wpedantic
		-Werror
)

add_executable(integrationTest
	tests/integration/tgeGoldenCaseTest.cc
)

target_link_libraries(integrationTest
	PRIVATE
		energyPriceCore
		GTest::gmock_main
)

target_compile_definitions(integrationTest
	PRIVATE
		TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/tests/data"
)

target_compile_options(integrationTest
	PRIVATE
		-Wall
		-Wextra
		-Wpedantic
		-Werror
)

add_executable(liveTest
	tests/integration/tgeLiveTest.cc
)

target_link_libraries(liveTest
	PRIVATE
		energyPriceCore
		GTest::gmock_main
)

target_compile_options(liveTest
	PRIVATE
		-Wall
		-Wextra
		-Wpedantic
		-Werror
)

include(GoogleTest)
gtest_discover_tests(unitTest PROPERTIES LABELS unit)
gtest_discover_tests(integrationTest PROPERTIES LABELS integration)
gtest_discover_tests(liveTest PROPERTIES LABELS live)

option(ENABLE_ASAN "Enable AddressSanitizer" ON)

if(ENABLE_ASAN)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(energyPriceCore PRIVATE
                -fsanitize=address
                -fno-omit-frame-pointer
        )

        target_compile_options(unitTest PRIVATE
                -fsanitize=address
                -fno-omit-frame-pointer
        )
        target_compile_options(integrationTest PRIVATE
                -fsanitize=address
                -fno-omit-frame-pointer
        )

        target_compile_options(liveTest PRIVATE
                -fsanitize=address
                -fno-omit-frame-pointer
        )
	target_compile_options(energyPrices PRIVATE -fsanitize=address -g)
        target_link_options(integrationTest PRIVATE -fsanitize=address)
	target_link_options(energyPrices PRIVATE -fsanitize=address)
	target_link_options(energyPricesServer PRIVATE -fsanitize=address)
	target_link_options(liveTest PRIVATE -fsanitize=address)
        target_link_options(unitTest PRIVATE -fsanitize=address)
    else()
        message(WARNING "AddressSanitizer is not configured for this compiler")
    endif()
endif()



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
