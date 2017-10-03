cmake_minimum_required(VERSION 3.2)
project(avakar.blob)

include(deps.cmake)

add_library(avakar.blob STATIC
	include/avakar/blob.h
	include/avakar/rope.h
	include/avakar/token.h
	src/blob.cpp
	src/node.cpp
	src/node.h
	src/rope.cpp
	src/storage.h
	src/token.cpp
	)

target_include_directories(avakar.blob PUBLIC include)
target_link_libraries(avakar.blob string_view)

add_executable(avakar.blob.test EXCLUDE_FROM_ALL
	test/test.cpp
	)
target_link_libraries(avakar.blob.test avakar.blob)
