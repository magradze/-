/**
 * @file codegen.c
 * @author გიორგი მაღრაძე (magradze.giorgi@gmail.com)
 * @brief C კოდის გენერატორის იმპლემენტაცია.
 * @version 0.2
 */
#include "codegen.h"

static void generate_expression(AstNode* node, FILE* outfile);

/**
 * @brief გენერირებს კოდს ერთი განცხადებისთვის.
 */
static void generate_statement(AstNode* node, FILE* outfile) {
    if (node == NULL) return;
    switch (node->type) {
        case NODE_VAR_DECL:
            fprintf(outfile, "double %.*s = ", node->as.var_decl.name.length, node->as.var_decl.name.start);
            generate_expression(node->as.var_decl.initializer, outfile);
            fprintf(outfile, ";\n");
            break;
        case NODE_PRINT_STMT:
            fprintf(outfile, "printf(\"%%f\\n\", ");
            generate_expression(node->as.print_stmt.expression, outfile);
            fprintf(outfile, ");\n");
            break;
        default:
            fprintf(stderr, "CodeGen Error: Unknown statement type for generation.\n");
            break;
    }
}

/**
 * @brief გენერირებს კოდს გამოსახულებისთვის.
 */
static void generate_expression(AstNode* node, FILE* outfile) {
    if (node == NULL) return;
    switch (node->type) {
        case NODE_NUMBER:
            fprintf(outfile, "%f", node->as.number.value);
            break;
        case NODE_VARIABLE:
            fprintf(outfile, "%.*s", node->as.variable.name.length, node->as.variable.name.start);
            break;
        case NODE_UNARY_OP:
            fprintf(outfile, "(%.*s", node->as.unary_op.operator.length, node->as.unary_op.operator.start);
            generate_expression(node->as.unary_op.right, outfile);
            fprintf(outfile, ")");
            break;
        case NODE_BINARY_OP:
            fprintf(outfile, "(");
            generate_expression(node->as.binary_op.left, outfile);
            fprintf(outfile, " %.*s ", node->as.binary_op.operator.length, node->as.binary_op.operator.start);
            generate_expression(node->as.binary_op.right, outfile);
            fprintf(outfile, ")");
            break;
        default:
            fprintf(stderr, "CodeGen Error: Node is not a valid expression.\n");
            break;
    }
}

void generate_code(AstNode* node, FILE* outfile) {
    fprintf(outfile, "#include <stdio.h>\n\n");
    fprintf(outfile, "int main() {\n");

    if (node != NULL && node->type == NODE_PROGRAM) {
        ProgramNode* prog = &node->as.program;
        for (int i = 0; i < prog->count; i++) {
            fprintf(outfile, "    ");
            generate_statement(prog->statements[i], outfile);
        }
    }

    fprintf(outfile, "    return 0;\n");
    fprintf(outfile, "}\n");
}