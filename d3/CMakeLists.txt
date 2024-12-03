cmake_minimum_required(VERSION 3.20)

project(day3)

include(FetchContent)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(PCRE2_USE_STATIC_LIBS ON)
set(PCRE2_BUILD_TESTS OFF)
set(PCRE2_BUILD_PCRE2GREP OFF)
include(FetchContent)
FetchContent_Declare(pcre2
		URL      https://github.com/PCRE2Project/pcre2/releases/download/pcre2-10.42/pcre2-10.42.tar.gz
		URL_HASH SHA256=c33b418e3b936ee3153de2c61cc638e7e4fe3156022a5c77d0711bcbb9d64f1f
		)
FetchContent_MakeAvailable(pcre2)

if(NOT pcre2_POPULATED)
	FetchContent_Populate(pcre2)
	add_subdirectory(${pcre2_SOURCE_DIR} ${pcre2_BUILD_DIR})
endif()

add_executable(${PROJECT_NAME} main.c)

target_link_libraries(${PROJECT_NAME} PRIVATE pcre2-8-static)