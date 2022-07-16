#include <libgccjit.h>
#include <json-c/json.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef int (*entry_fn) ();

gcc_jit_rvalue *compile_un_op_ast(gcc_jit_context *ctx, const char *token, json_object *ast);
gcc_jit_rvalue *compile_bin_op_ast(gcc_jit_context *ctx, const char *token, json_object *lhs_ast, json_object *rhs_ast);
gcc_jit_rvalue *compile_ast(gcc_jit_context *ctx, json_object *ast);

gcc_jit_rvalue *compile_un_op_ast(gcc_jit_context *ctx, const char *token, json_object *ast) {
  gcc_jit_type *jit_int_type = gcc_jit_context_get_type(ctx, GCC_JIT_TYPE_INT);
  gcc_jit_rvalue *rvalue = compile_ast(ctx, ast);

  if (strcmp(token, "-") == 0) {
    return gcc_jit_context_new_unary_op(ctx, NULL, GCC_JIT_UNARY_OP_MINUS, jit_int_type, rvalue);
  } else {
    fprintf(stderr, "unsupported unary operator %s", token);
    exit(1);
  }
}

gcc_jit_rvalue *compile_bin_op_ast(gcc_jit_context *ctx, const char *token, json_object *lhs_ast, json_object *rhs_ast) {
  gcc_jit_type *jit_int_type = gcc_jit_context_get_type(ctx, GCC_JIT_TYPE_INT);
  gcc_jit_rvalue *lhs_rvalue = compile_ast(ctx, lhs_ast);
  gcc_jit_rvalue *rhs_rvalue = compile_ast(ctx, rhs_ast);

  if (strcmp(token, "+") == 0) {
    return gcc_jit_context_new_binary_op(ctx, NULL, GCC_JIT_BINARY_OP_PLUS, jit_int_type, lhs_rvalue, rhs_rvalue);
  } else if (strcmp(token, "-") == 0) {
    return gcc_jit_context_new_binary_op(ctx, NULL, GCC_JIT_BINARY_OP_MINUS, jit_int_type, lhs_rvalue, rhs_rvalue);
  } else if (strcmp(token, "*") == 0) {
    return gcc_jit_context_new_binary_op(ctx, NULL, GCC_JIT_BINARY_OP_MULT, jit_int_type, lhs_rvalue, rhs_rvalue);
  } else if (strcmp(token, "/") == 0) {
    return gcc_jit_context_new_binary_op(ctx, NULL, GCC_JIT_BINARY_OP_DIVIDE, jit_int_type, lhs_rvalue, rhs_rvalue);
  } else {
    fprintf(stderr, "unsupported binary operator %s", token);
    exit(1);
  }
}

gcc_jit_rvalue *compile_ast(gcc_jit_context *ctx, json_object *ast) {
  json_type json_obj_type = json_object_get_type(ast);

  switch (json_obj_type) {
    case json_type_int: {
      gcc_jit_type *jit_int_type = gcc_jit_context_get_type(ctx, GCC_JIT_TYPE_INT);
      int32_t value = json_object_get_int(ast);
      return gcc_jit_context_new_rvalue_from_int(ctx, jit_int_type, value);
    }

    case json_type_array: {
      size_t ast_size = json_object_array_length(ast) - 2;
      json_object *token = json_object_array_get_idx(ast, 0);
      const char *token_str = json_object_get_string(token);

      switch (ast_size) {
        case 1: {
          json_object *arg_ast = json_object_array_get_idx(ast, 1);
          return compile_un_op_ast(ctx, token_str, arg_ast);
        }
        case 2: {
          json_object *lhs_ast = json_object_array_get_idx(ast, 1);
          json_object *rhs_ast = json_object_array_get_idx(ast, 2);
          return compile_bin_op_ast(ctx, token_str, lhs_ast, rhs_ast);
        }
        default: {
          fprintf(stderr, "unsupported ast");
          exit(1);
        }
      }
    }
  }

  return 0;
}

void compile_program(gcc_jit_context *ctx, json_object *root_ast) {
  gcc_jit_type *jit_int_type = gcc_jit_context_get_type(ctx, GCC_JIT_TYPE_INT);

  gcc_jit_function *func =
    gcc_jit_context_new_function(
      ctx, NULL, GCC_JIT_FUNCTION_EXPORTED, jit_int_type, "entry", 0, NULL, 0
    );

  gcc_jit_block *block = gcc_jit_function_new_block(func, NULL);

  gcc_jit_block_end_with_return(block, NULL, compile_ast(ctx, root_ast));
}

int main(int argc, char **argv) {
  gcc_jit_context *ctx = gcc_jit_context_acquire();

  // (((10 - 5) * 2) + (-4)) / 2
  json_object *root_ast = json_tokener_parse(
    "[\"/\", [\"+\", [\"*\", [\"-\", 10, 5, {}], 2, {}], [\"-\", 3, {}], {}], 2, {}]"
  );

  compile_program(ctx, root_ast);

  gcc_jit_result *jit_result = gcc_jit_context_compile(ctx);
  if (!jit_result) {
    fprintf(stderr, "NULL result");
    exit(1);
  }

  entry_fn entry = (entry_fn)gcc_jit_result_get_code(jit_result, "entry");

  printf("%d\n", entry());

  gcc_jit_context_release(ctx);

  return 0;
}

