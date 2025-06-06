/*
 * Copyright Â© 2010 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef IR_TO_MESA_H
#define IR_TO_MESA_H

#include "../main/glheader.h"

#ifdef __cplusplus
extern "C" {
#endif

struct gl_context;
struct gl_program;
struct gl_shader;
struct gl_shader_program;
struct gl_linked_shader;
struct gl_program_parameter_list;

void _mesa_glsl_link_shader(struct gl_context *ctx, struct gl_shader_program *prog);
GLboolean _mesa_ir_link_shader(struct gl_context *ctx, struct gl_shader_program *prog);

void
_mesa_generate_parameters_list_for_uniforms(struct gl_context *ctx,
                                            struct gl_shader_program
					    *shader_program,
					    struct gl_linked_shader *sh,
					    struct gl_program_parameter_list
					    *params);
void
_mesa_associate_uniform_storage(struct gl_context *ctx,
                                struct gl_shader_program *shader_program,
                                struct gl_program *prog);

#ifdef __cplusplus
}
#endif

#endif /* IR_TO_MESA_H */
