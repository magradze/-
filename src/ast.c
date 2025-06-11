/**
 * @file ast.c
 * @author გიორგი მაღრაძე (magradze.giorgi@gmail.com)
 * @brief აბსტრაქტული სინტაქსური ხის (AST) ფუნქციების იმპლემენტაცია.
 * @version 0.5
 */
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

/** @brief დამხმარე ფუნქცია ახალი AST კვანძის შესაქმნელად. */
static AstNode* allocate_node(AstNodeType type) {
    AstNode* node = (AstNode*)malloc(sizeof(AstNode));
    if (!node) {
        fprintf(stderr, "FATAL: Memory allocation failed for AST node.\n");
        exit(1);
    }
    node->type = type;
    return node;
}

AstNode* create_program_node() {
    AstNode* node = allocate_node(NODE_PROGRAM);
    node->as.program.count = 0;
    node->as.program.capacity = 8;
    node->as.program.statements = malloc(sizeof(AstNode*) * node->as.program.capacity);
    if (!node->as.program.statements) {
        fprintf(stderr, "FATAL: Memory allocation failed for program statements.\n");
        exit(1);
    }
    return node;
}

void add_statement_to_program(AstNode* program_node, AstNode* statement) {
    if (program_node->type != NODE_PROGRAM) return;
    ProgramNode* prog = &program_node->as.program;
    if (prog->count >= prog->capacity) {
        prog->capacity *= 2;
        prog->statements = realloc(prog->statements, sizeof(AstNode*) * prog->capacity);
        if (!prog->statements) {
            fprintf(stderr, "FATAL: Memory reallocation failed for program statements.\n");
            exit(1);
        }
    }
    prog->statements[prog->count++] = statement;
}

AstNode* create_binary_op_node(AstNode* left, Token op, AstNode* right) {
    AstNode* node = allocate_node(NODE_BINARY_OP);
    node->as.binary_op.left = left;
    node->as.binary_op.operator = op;
    node->as.binary_op.right = right;
    return node;
}

AstNode* create_number_node(double value) {
    AstNode* node = allocate_node(NODE_NUMBER);
    node->as.number.value = value;
    return node;
}

AstNode* create_unary_op_node(Token op, AstNode* right) {
    AstNode* node = allocate_node(NODE_UNARY_OP);
    node->as.unary_op.operator = op;
    node->as.unary_op.right = right;
    return node;
}

AstNode* create_print_stmt_node(AstNode* expression) {
    AstNode* node = allocate_node(NODE_PRINT_STMT);
    node->as.print_stmt.expression = expression;
    return node;
}

AstNode* create_var_decl_node(Token name, AstNode* initializer) {
    AstNode* node = allocate_node(NODE_VAR_DECL);
    node->as.var_decl.name = name;
    node->as.var_decl.initializer = initializer;
    return node;
}

AstNode* create_variable_node(Token name) {
    AstNode* node = allocate_node(NODE_VARIABLE);
    node->as.variable.name = name;
    return node;
}

void free_ast(AstNode* node) {
    if (!node) return;
    switch (node->type) {
        case NODE_PROGRAM:
            for (int i = 0; i < node->as.program.count; i++) free_ast(node->as.program.statements[i]);
            free(node->as.program.statements);
            break;
        case NODE_VAR_DECL: free_ast(node->as.var_decl.initializer); break;
        case NODE_PRINT_STMT: free_ast(node->as.print_stmt.expression); break;
        case NODE_BINARY_OP: free_ast(node->as.binary_op.left); free_ast(node->as.binary_op.right); break;
        case NODE_UNARY_OP: free_ast(node->as.unary_op.right); break;
        case NODE_VARIABLE: case NODE_NUMBER: break;
    }
    free(node);
}

/** @brief რეკურსიულად ბეჭდავს AST ხის თითოეულ კვანძს შეწევებით. */
static void print_ast_recursive(AstNode* node, int indent) {
    if (node == NULL) return;
    for (int i = 0; i < indent; i++) printf("  ");

    switch (node->type) {
        case NODE_PROGRAM:
            printf("Program\n");
            for (int i = 0; i < node->as.program.count; i++) {
                print_ast_recursive(node->as.program.statements[i], indent + 1);
            }
            break;
        case NODE_VAR_DECL:
            printf("VarDecl(%.*s)\n", node->as.var_decl.name.length, node->as.var_decl.name.start);
            print_ast_recursive(node->as.var_decl.initializer, indent + 1);
            break;
        case NODE_PRINT_STMT:
            printf("PrintStmt\n");
            print_ast_recursive(node->as.print_stmt.expression, indent + 1);
            break;
        case NODE_BINARY_OP:
            printf("BinaryOp(%.*s)\n", node->as.binary_op.operator.length, node->as.binary_op.operator.start);
            print_ast_recursive(node->as.binary_op.left, indent + 1);
            print_ast_recursive(node->as.binary_op.right, indent + 1);
            break;
        case NODE_UNARY_OP:
            printf("UnaryOp(%.*s)\n", node->as.unary_op.operator.length, node->as.unary_op.operator.start);
            print_ast_recursive(node->as.unary_op.right, indent + 1);
            break;
        case NODE_VARIABLE:
            printf("Variable(%.*s)\n", node->as.variable.name.length, node->as.variable.name.start);
            break;
        case NODE_NUMBER:
            printf("Number(%.2f)\n", node->as.number.value);
            break;
        default:
            printf("Unknown Node\n");
            break;
    }
}

void print_ast(AstNode* node) {
    printf("--- Abstract Syntax Tree ---\n");
    print_ast_recursive(node, 0);
    printf("--------------------------\n");
}