
cmake_minimum_required(VERSION 3.13.1)

set(common_dir ${CMAKE_CURRENT_SOURCE_DIR}/../common)
set(common_src_dir ${common_dir}/src)
set(common_include_dir ${common_dir}/include)

include_directories(
	${common_include_dir}
)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(dns_resolve)

target_sources(app PRIVATE
	src/main.c
	${common_src_dir}/libc_extensions.c
)

set(gen_dir ${ZEPHYR_BINARY_DIR}/include/generated/)

include(${ZEPHYR_BASE}/samples/net/common/common.cmake)

add_subdirectory(src/net/)

