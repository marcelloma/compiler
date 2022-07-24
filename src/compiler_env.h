#include <libgccjit.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  gcc_jit_context *ctx;
  gcc_jit_type *bool_type;
  gcc_jit_type *int_type;
  gcc_jit_type *double_type;
  gcc_jit_type *string_type;
  gcc_jit_type *money_type;
  gcc_jit_type *money_ptr_type;
  gcc_jit_field *money_amount_field;
  gcc_jit_field *money_currency_field;
} compiler_env;

compiler_env *build_compiler_env();

void release_compiler_env(compiler_env *env);