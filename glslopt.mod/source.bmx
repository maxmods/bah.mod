' Copyright (c) 2014 Bruce A Henderson
' 
' Permission is hereby granted, free of charge, to any person obtaining a copy
' of this software and associated documentation files (the "Software"), to deal
' in the Software without restriction, including without limitation the rights
' to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
' copies of the Software, and to permit persons to whom the Software is
' furnished to do so, subject to the following conditions:
' 
' The above copyright notice and this permission notice shall be included in
' all copies or substantial portions of the Software.
' 
' THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
' IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
' FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
' AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
' LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
' OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
' THE SOFTWARE.
' 
SuperStrict

Import BRL.Blitz

Import "glslopt/include/*.h"
Import "glslopt/src/mesa/*.h"
Import "glslopt/src/glsl/*.h"

Import "glslopt/src/glsl/ast_expr.cpp"
Import "glslopt/src/glsl/ast_function.cpp"
Import "glslopt/src/glsl/ast_to_hir.cpp"
Import "glslopt/src/glsl/lower_named_interface_blocks.cpp"
Import "glslopt/src/glsl/ast_type.cpp"
Import "glslopt/src/glsl/glsl_lexer.cpp"
Import "glslopt/src/glsl/ir_stats.cpp"
Import "glslopt/src/glsl/glsl_optimizer.cpp"
Import "glslopt/src/glsl/glsl_parser_extras.cpp"
Import "glslopt/src/glsl/glsl_parser.cpp"
Import "glslopt/src/glsl/glsl_types.cpp"
Import "glslopt/src/glsl/hir_field_selection.cpp"
Import "glslopt/src/glsl/ir_basic_block.cpp"
Import "glslopt/src/glsl/ir_clone.cpp"
Import "glslopt/src/glsl/ir_constant_expression.cpp"
Import "glslopt/src/glsl/ir_expression_flattening.cpp"
Import "glslopt/src/glsl/ir_function_can_inline.cpp"
Import "glslopt/src/glsl/ir_function.cpp"
Import "glslopt/src/glsl/ir_hierarchical_visitor.cpp"
Import "glslopt/src/glsl/ir_hv_accept.cpp"
Import "glslopt/src/glsl/ir_import_prototypes.cpp"
Import "glslopt/src/glsl/ir_print_glsl_visitor.cpp"
Import "glslopt/src/glsl/ir_print_visitor.cpp"
Import "glslopt/src/glsl/lower_vector_insert.cpp"
Import "glslopt/src/glsl/ir_unused_structs.cpp"
Import "glslopt/src/glsl/ir_validate.cpp"
Import "glslopt/src/glsl/opt_dead_builtin_varyings.cpp"
Import "glslopt/src/glsl/ir_variable_refcount.cpp"
Import "glslopt/src/glsl/ir.cpp"
Import "glslopt/src/glsl/link_functions.cpp"
Import "glslopt/src/glsl/linker.cpp"
Import "glslopt/src/glsl/s_expression.cpp"
Import "glslopt/src/glsl/ir_rvalue_visitor.cpp"
Import "glslopt/src/glsl/ir_equals.cpp"
Import "glslopt/src/glsl/glsl_symbol_table.cpp"
Import "glslopt/src/glsl/loop_analysis.cpp"
Import "glslopt/src/glsl/loop_controls.cpp"
Import "glslopt/src/glsl/loop_unroll.cpp"
Import "glslopt/src/glsl/opt_vectorize.cpp"
Import "glslopt/src/glsl/lower_noise.cpp"
Import "glslopt/src/glsl/lower_variable_index_to_cond_assign.cpp"
Import "glslopt/src/glsl/opt_redundant_jumps.cpp"
Import "glslopt/src/glsl/lower_if_to_cond_assign.cpp"
Import "glslopt/src/glsl/lower_jumps.cpp"
Import "glslopt/src/glsl/lower_mat_op_to_vec.cpp"
Import "glslopt/src/glsl/lower_vec_index_to_cond_assign.cpp"
Import "glslopt/src/glsl/lower_vec_index_to_swizzle.cpp"
Import "glslopt/src/glsl/opt_algebraic.cpp"
Import "glslopt/src/glsl/opt_constant_folding.cpp"
Import "glslopt/src/glsl/opt_constant_propagation.cpp"
Import "glslopt/src/glsl/opt_constant_variable.cpp"
Import "glslopt/src/glsl/opt_copy_propagation.cpp"
Import "glslopt/src/glsl/opt_dead_code_local.cpp"
Import "glslopt/src/glsl/link_atomics.cpp"
Import "glslopt/src/glsl/opt_dead_code.cpp"
Import "glslopt/src/glsl/opt_dead_functions.cpp"
Import "glslopt/src/glsl/opt_function_inlining.cpp"
Import "glslopt/src/glsl/opt_if_simplification.cpp"
Import "glslopt/src/glsl/opt_noop_swizzle.cpp"
Import "glslopt/src/glsl/opt_structure_splitting.cpp"
Import "glslopt/src/glsl/opt_swizzle_swizzle.cpp"
Import "glslopt/src/glsl/opt_tree_grafting.cpp"
Import "glslopt/src/glsl/builtin_functions.cpp"
Import "glslopt/src/glsl/lower_discard.cpp"
Import "glslopt/src/glsl/lower_instructions.cpp"
Import "glslopt/src/glsl/lower_vector.cpp"
Import "glslopt/src/glsl/builtin_types.cpp"
Import "glslopt/src/glsl/opt_copy_propagation_elements.cpp"
Import "glslopt/src/glsl/opt_cse.cpp"
Import "glslopt/src/glsl/ir_function_detect_recursion.cpp"
Import "glslopt/src/glsl/standalone_scaffolding.cpp"
Import "glslopt/src/glsl/opt_flip_matrices.cpp"
Import "glslopt/src/glsl/link_interface_blocks.cpp"
Import "glslopt/src/glsl/builtin_variables.cpp"
Import "glslopt/src/glsl/link_uniforms.cpp"
Import "glslopt/src/glsl/link_uniform_initializers.cpp"
Import "glslopt/src/glsl/lower_clip_distance.cpp"
Import "glslopt/src/glsl/lower_discard_flow.cpp"
Import "glslopt/src/glsl/lower_output_reads.cpp"
Import "glslopt/src/glsl/lower_ubo_reference.cpp"
Import "glslopt/src/glsl/opt_array_splitting.cpp"
Import "glslopt/src/glsl/ir_builder.cpp"
Import "glslopt/src/glsl/ast_array_index.cpp"
Import "glslopt/src/glsl/link_uniform_block_active_visitor.cpp"
Import "glslopt/src/glsl/link_uniform_blocks.cpp"
Import "glslopt/src/glsl/link_varyings.cpp"
Import "glslopt/src/glsl/lower_packed_varyings.cpp"
Import "glslopt/src/glsl/lower_packing_builtins.cpp"
Import "glslopt/src/glsl/opt_flatten_nested_if_blocks.cpp"
Import "glslopt/src/glsl/strtod.c"
Import "glslopt/src/glsl/ralloc.c"

Import "glslopt/src/glsl/glcpp/glcpp-lex.c"
Import "glslopt/src/glsl/glcpp/glcpp-parse.c"
Import "glslopt/src/glsl/glcpp/pp.c"

Import "glslopt/src/mesa/main/hash_table.c"
Import "glslopt/src/mesa/main/imports.c"
Import "glslopt/src/mesa/program/prog_hash_table.c"
Import "glslopt/src/mesa/program/symbol_table.c"

Import "glue.cpp"
