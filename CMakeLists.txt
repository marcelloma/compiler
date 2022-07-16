cmake_minimum_required(VERSION 3.23)

# set(CMAKE_VERBOSE_MAKEFILE ON)

project(calculator)
add_executable(calculator calculator.c)

if(APPLE)
  execute_process(COMMAND brew --prefix libgccjit OUTPUT_VARIABLE LIBGCCJIT_DIR)
  execute_process(COMMAND brew --prefix json-c OUTPUT_VARIABLE JSON_C_DIR)
  string(STRIP ${LIBGCCJIT_DIR} LIBGCCJIT_DIR)
  string(STRIP ${JSON_C_DIR} JSON_C_DIR)

  target_include_directories(calculator PRIVATE ${LIBGCCJIT_DIR}/include)
  target_include_directories(calculator PRIVATE ${JSON_C_DIR}/include)
  target_link_directories(calculator PRIVATE ${LIBGCCJIT_DIR}/lib/gcc/11)
  target_link_directories(calculator PRIVATE ${JSON_C_DIR}/lib)
endif()

target_link_libraries(calculator gccjit json-c)

