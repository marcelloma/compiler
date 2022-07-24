#include "../src/compiler.h"

gcc_jit_rvalue *comp_un_op_ast(const char *token, json_object *ast, compiler_env *c_env) {
  if (strcmp(token, "-") == 0) {
    gcc_jit_rvalue *dec = comp_ast(ast, c_env);
    gcc_jit_rvalue *args[2] = {dec, gcc_jit_param_as_rvalue(c_env->r_env)};
    return gcc_jit_context_new_call(c_env->ctx, NULL, c_env->dec_minus, 2, args);
  } else if (strcmp(token, "number") == 0) {
    gcc_jit_rvalue *dec_str = gcc_jit_context_new_string_literal(c_env->ctx, json_object_get_string(ast));
    gcc_jit_rvalue *args[2] = {dec_str, gcc_jit_param_as_rvalue(c_env->r_env)};
    return gcc_jit_context_new_call(c_env->ctx, NULL, c_env->dec_from_str, 2, args);
  } else {
    fprintf(stderr, "unsupported unary operator %s\n", token);
    exit(1);
  }
}

gcc_jit_rvalue *comp_bin_op_ast(const char *token, json_object *lhs_ast, json_object *rhs_ast, compiler_env *c_env) {
  gcc_jit_rvalue *lhs_rvalue = comp_ast(lhs_ast, c_env);
  gcc_jit_rvalue *rhs_rvalue = comp_ast(rhs_ast, c_env);

  if (strcmp(token, "+") == 0) {
    return gcc_jit_context_new_binary_op(c_env->ctx, NULL, GCC_JIT_BINARY_OP_PLUS, c_env->double_type, lhs_rvalue,
                                         rhs_rvalue);
  } else if (strcmp(token, "-") == 0) {
    return gcc_jit_context_new_binary_op(c_env->ctx, NULL, GCC_JIT_BINARY_OP_MINUS, c_env->double_type, lhs_rvalue,
                                         rhs_rvalue);
  } else if (strcmp(token, "*") == 0) {
    return gcc_jit_context_new_binary_op(c_env->ctx, NULL, GCC_JIT_BINARY_OP_MULT, c_env->double_type, lhs_rvalue,
                                         rhs_rvalue);
  } else if (strcmp(token, "/") == 0) {
    return gcc_jit_context_new_binary_op(c_env->ctx, NULL, GCC_JIT_BINARY_OP_DIVIDE, c_env->double_type, lhs_rvalue,
                                         rhs_rvalue);
  } else {
    fprintf(stderr, "error: unsupported binary operator %s\n", token);
    exit(1);
  }
}

gcc_jit_rvalue *comp_ast(json_object *ast, compiler_env *c_env) {
  json_type json_obj_type = json_object_get_type(ast);

  switch (json_obj_type) {
    case json_type_double: {
      double value = json_object_get_double(ast);
      return gcc_jit_context_new_rvalue_from_double(c_env->ctx, c_env->double_type, value);
    }

    case json_type_array: {
      size_t ast_size = json_object_array_length(ast) - 1;
      json_object *token = json_object_array_get_idx(ast, 0);
      const char *token_str = json_object_get_string(token);

      switch (ast_size) {
        case 1: {
          json_object *arg_ast = json_object_array_get_idx(ast, 1);
          return comp_un_op_ast(token_str, arg_ast, c_env);
        }
        case 2: {
          json_object *lhs_ast = json_object_array_get_idx(ast, 1);
          json_object *rhs_ast = json_object_array_get_idx(ast, 2);
          return comp_bin_op_ast(token_str, lhs_ast, rhs_ast, c_env);
        }
        default: {
          fprintf(stderr, "error: unsupported ast\n");
          exit(1);
        }
      }
    }
    default: {
      fprintf(stderr, "error: unexpected json_obj_type %u\n", json_obj_type);
      exit(1);
    }
  }

  return 0;
}

void comp_calculated_field(json_object *j_calculated_field, compiler_env *c_env) {
  json_object *j_id = json_object_object_get(j_calculated_field, ID_KEY);
  json_object *j_ast = json_object_object_get(j_calculated_field, AST_KEY);
  int32_t id = json_object_get_int(j_id);

  // Q: Is there a better way to generate this name? I think I'm supposed to free this,
  // at least according to asprintf's docs however the compiler won't let me free it.
  char *tmp_name[50];
  asprintf(tmp_name, "calculated_field_%d", id);
  const char *name = tmp_name[0];

  c_env->r_env = gcc_jit_context_new_param(c_env->ctx, NULL, c_env->void_ptr_type, "r_env");
  c_env->func = gcc_jit_context_new_function(c_env->ctx, NULL, GCC_JIT_FUNCTION_EXPORTED, c_env->void_ptr_type, name, 1,
                                             &c_env->r_env, 0);

  c_env->block = gcc_jit_function_new_block(c_env->func, NULL);

  gcc_jit_rvalue *r_env_rvalue = gcc_jit_param_as_rvalue(c_env->r_env);

  c_env->mpd_ctx = gcc_jit_context_new_call(c_env->ctx, NULL, c_env->get_mpd_ctx, 1, &r_env_rvalue);

  gcc_jit_block_end_with_return(c_env->block, NULL, comp_ast(j_ast, c_env));
}

void comp_calculated_fields(json_object *j_structure, compiler_env *c_env) {
  json_object *j_calculated_fields = json_object_object_get(j_structure, CALCULATED_FIELDS_KEY);
  if (json_object_get_type(j_calculated_fields) != json_type_array) {
    printf("calculated fields key is not a array\n");
    exit(1);
  }

  for (size_t i = 0; i < json_object_array_length(j_calculated_fields); i++) {
    json_object *j_calculated_field = json_object_array_get_idx(j_calculated_fields, i);
    comp_calculated_field(j_calculated_field, c_env);
  }
}
