/**
 * @file ast.h
 * @author გიორგი მაღრაძე (magradze.giorgi@gmail.com)
 * @brief აბსტრაქტული სინტაქსური ხის (AST) სტრუქტურების დეკლარაციები.
 * @version 0.5
 */
#ifndef AST_H
#define AST_H

#include "lexer.h"

/** @brief AST კვანძის შესაძლო ტიპები. */
typedef enum {
    NODE_PROGRAM,
    NODE_VAR_DECL,
    NODE_PRINT_STMT,
    NODE_BINARY_OP,
    NODE_UNARY_OP,
    NODE_VARIABLE,
    NODE_NUMBER,
} AstNodeType;

typedef struct AstNode AstNode;

/** @brief პროგრამის კვანძი, რომელიც შეიცავს განცხადებების სიას. */
typedef struct { AstNode** statements; int count; int capacity; } ProgramNode;
/** @brief ცვლადის დეკლარაციის კვანძი. */
typedef struct { Token name; AstNode* initializer; } VarDeclNode;
/** @brief "დაბეჭდე" განცხადების კვანძი. */
typedef struct { AstNode* expression; } PrintStmtNode;
/** @brief ორობითი ოპერაციის კვანძი. */
typedef struct { AstNode* left; Token operator; AstNode* right; } BinaryOpNode;
/** @brief ერთმაგი ოპერაციის კვანძი. */
typedef struct { Token operator; AstNode* right; } UnaryOpNode;
/** @brief ცვლადის გამოყენების კვანძი. */
typedef struct { Token name; } VariableNode;
/** @brief რიცხვითი ლიტერალის კვანძი. */
typedef struct { double value; } NumberNode;

/** @brief AST-ის ერთიანი კვანძის სტრუქტურა. */
struct AstNode {
    AstNodeType type;
    union {
        ProgramNode program;
        VarDeclNode var_decl;
        PrintStmtNode print_stmt;
        BinaryOpNode binary_op;
        UnaryOpNode unary_op;
        VariableNode variable;
        NumberNode number;
    } as;
};

/** @brief ქმნის ცარიელ პროგრამის კვანძს. */
AstNode* create_program_node();
/** @brief ამატებს განცხადებას პროგრამის კვანძში. */
void add_statement_to_program(AstNode* program_node, AstNode* statement);
/** @brief ქმნის ცვლადის დეკლარაციის კვანძს. */
AstNode* create_var_decl_node(Token name, AstNode* initializer);
/** @brief ქმნის "დაბეჭდე" განცხადების კვანძს. */
AstNode* create_print_stmt_node(AstNode* expression);
/** @brief ქმნის ორობითი ოპერაციის კვანძს. */
AstNode* create_binary_op_node(AstNode* left, Token op, AstNode* right);
/** @brief ქმნის ერთმაგი ოპერაციის კვანძს. */
AstNode* create_unary_op_node(Token op, AstNode* right);
/** @brief ქმნის ცვლადის გამოყენების კვანძს. */
AstNode* create_variable_node(Token name);
/** @brief ქმნის რიცხვითი ლიტერალის კვანძს. */
AstNode* create_number_node(double value);
/** @brief ბეჭდავს AST ხის სტრუქტურას კონსოლში. */
void print_ast(AstNode* node);
/** @brief ათავისუფლებს AST ხის მიერ დაკავებულ მეხსიერებას. */
void free_ast(AstNode* node);

#endif // AST_H