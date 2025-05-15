cmake_minimum_required(VERSION 3.28)
project(AddPrefixes)

find_package(Clang
	REQUIRED
	CONFIG
)

add_library(AddPrefixes 
	MODULE
	AddPrefixes.cpp
)

target_include_directories(AddPrefixes
	PRIVATE
	${CLANG_INCLUDE_DIRS}
)
target_link_libraries(AddPrefixes
	PRIVATE
	clang
)

set_target_properties(AddPrefixes
	PROPERTIES
	COMPILE_FLAGS "-fno-rtti"
)
