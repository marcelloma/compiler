#include <libgccjit.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  gcc_jit_context *ctx;
  gcc_jit_type *size_t_type;
  gcc_jit_type *void_type;
  gcc_jit_type *void_ptr_type;
  gcc_jit_type *bool_type;
  gcc_jit_type *int_type;
  gcc_jit_type *double_type;
  gcc_jit_type *string_type;
  gcc_jit_type *money_type;
  gcc_jit_type *money_ptr_type;
  gcc_jit_field *money_amount_field;
  gcc_jit_field *money_currency_field;
  gcc_jit_function *printf;
  gcc_jit_function *malloc;
  gcc_jit_function *free;
  gcc_jit_function *mpd_new;
  gcc_jit_function *mpd_set_string;
  gcc_jit_function *dec_from_str;
  gcc_jit_function *dec_minus;
  gcc_jit_function *get_mpd_ctx;
  gcc_jit_param *r_env;
  gcc_jit_rvalue *mpd_ctx;
  gcc_jit_block *block;
  gcc_jit_function *func;
} compiler_env;

compiler_env *build_compiler_env();
void add_printf_function(compiler_env *env);
void add_malloc_function(compiler_env *env);
void add_free_function(compiler_env *env);
void add_mpd_new_function(compiler_env *env);
void add_mpd_set_string_function(compiler_env *env);
void add_get_mpd_ctx_function(compiler_env *env);
void add_dec_from_str(compiler_env *env);
void add_dec_minus(compiler_env *env);
void release_compiler_env(compiler_env *env);