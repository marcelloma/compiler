#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libgccjit.h>
#include <json-c/json.h>
#include "../src/compiler_env.h"

#define CALCULATED_FIELDS_KEY "calculated_fields"

#define ID_KEY "id"
#define AST_KEY "ast"

gcc_jit_rvalue *comp_un_op_ast(const char *token, json_object *ast, compiler_env *c_env);
gcc_jit_rvalue *comp_bin_op_ast(const char *token, json_object *lhs_ast, json_object *rhs_ast, compiler_env *c_env);
gcc_jit_rvalue *comp_ast(json_object *ast, compiler_env *c_env);

void comp_calculated_fields(json_object *structure, compiler_env *c_env);