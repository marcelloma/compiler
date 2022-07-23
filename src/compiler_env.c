#include "../src/compiler_env.h"

compiler_env *build_compiler_env()
{
  compiler_env *env = malloc(sizeof(compiler_env));
  if (env == NULL)
    exit(1);

  env->ctx = gcc_jit_context_acquire();
  env->bool_type = gcc_jit_context_get_type(env->ctx, GCC_JIT_TYPE_BOOL);
  env->int_type = gcc_jit_context_get_type(env->ctx, GCC_JIT_TYPE_INT);
  env->double_type = gcc_jit_context_get_type(env->ctx, GCC_JIT_TYPE_DOUBLE);
  env->string_type = gcc_jit_context_get_type(env->ctx, GCC_JIT_TYPE_CONST_CHAR_PTR);  

  env->money_amount_field = gcc_jit_context_new_field(env->ctx, NULL, env->double_type, "amount");
  env->money_currency_field = gcc_jit_context_new_field(env->ctx, NULL, env->string_type, "currency");
  gcc_jit_field *money_fields[2] = {env->money_amount_field, env->money_currency_field};
  gcc_jit_struct *money = gcc_jit_context_new_struct_type(env->ctx, NULL, "money", 2, money_fields);
  
  env->money_type = gcc_jit_struct_as_type(money);
  env->money_ptr_type = gcc_jit_type_get_pointer(env->money_type);

  return env;
}

void release_compiler_env(compiler_env *env)
{
  gcc_jit_context_release(env->ctx);

  free(env);
}