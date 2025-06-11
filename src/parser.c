/**
 * @file parser.c
 * @author გიორგი მაღრაძე (magradze.giorgi@gmail.com)
 * @brief სინტაქსური ანალიზატორის (პარსერის) იმპლემენტაცია.
 * @version 0.5
 */
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>

// --- წინასწარი დეკლარაციები ---
static AstNode* expression(Parser* parser);
static AstNode* declaration(Parser* parser);

/** @brief ბეჭდავს შეცდომის შეტყობინებას. */
static void error_at(Parser* parser, Token* token, const char* message) { if (parser->panic_mode) return; parser->panic_mode = true; fprintf(stderr, "[Line %d] შეცდომა", token->line); if (token->type == TOKEN_EOF) { fprintf(stderr, " ფაილის ბოლოს"); } else if (token->type != TOKEN_ERROR) { fprintf(stderr, " '%.*s'-თან", token->length, token->start); } fprintf(stderr, ": %s\n", message); parser->had_error = true; }
/** @brief იღებს შემდეგ ტოკენს ლექსერიდან. */
static void advance(Parser* parser) { parser->previous = parser->current; for (;;) { parser->current = scan_token(parser->lexer); if (parser->current.type != TOKEN_ERROR) break; error_at(parser, &parser->current, parser->current.start); } }
/** @brief ამოწმებს მიმდინარე ტოკენის ტიპს. */
static bool check(Parser* parser, TokenType type) { return parser->current.type == type; }
/** @brief ამოწმებს მიმდინარე ტოკენს და გადადის წინ, თუ ემთხვევა. */
static bool match(Parser* parser, TokenType type) { if (!check(parser, type)) return false; advance(parser); return true; }
/** @brief ამოწმებს და მოიხმარს ტოკენს, წინააღმდეგ შემთხვევაში აგენერირებს შეცდომას. */
static void consume(Parser* parser, TokenType type, const char* message) { if (check(parser, type)) { advance(parser); return; } error_at(parser, &parser->current, message); }

// --- გამოსახულებების პარსინგი ---

/** @brief არჩევს პირველად გამოსახულებებს. */
static AstNode* primary(Parser* parser) {
    if (match(parser, TOKEN_NUMBER)) return create_number_node(strtod(parser->previous.start, NULL));
    if (match(parser, TOKEN_IDENTIFIER)) return create_variable_node(parser->previous);
    if (match(parser, TOKEN_LPAREN)) {
        AstNode* expr = expression(parser);
        consume(parser, TOKEN_RPAREN, "მოსალოდნელი იყო ')' გამოსახულების შემდეგ.");
        return expr;
    }
    error_at(parser, &parser->current, "მოსალოდნელი იყო გამოსახულება.");
    return NULL;
}

/** @brief არჩევს ერთმაგ ოპერაციებს. */
static AstNode* unary(Parser* parser) {
    if (match(parser, TOKEN_MINUS)) {
        Token op = parser->previous;
        AstNode* right = unary(parser);
        return create_unary_op_node(op, right);
    }
    return primary(parser);
}

/** @brief არჩევს გამრავლებას და გაყოფას. (შესწორებული ლოგიკა) */
static AstNode* factor(Parser* parser) {
    AstNode* node = unary(parser);
    while (check(parser, TOKEN_SLASH) || check(parser, TOKEN_STAR)) {
        Token op = parser->current; // ვიმახსოვრებთ ოპერატორს
        advance(parser); // გადავდივართ წინ
        AstNode* right = unary(parser);
        node = create_binary_op_node(node, op, right);
    }
    return node;
}

/** @brief არჩევს მიმატებას და გამოკლებას. (შესწორებული ლოგიკა) */
static AstNode* term(Parser* parser) {
    AstNode* node = factor(parser);
    while (check(parser, TOKEN_MINUS) || check(parser, TOKEN_PLUS)) {
        Token op = parser->current; // ვიმახსოვრებთ ოპერატორს
        advance(parser); // გადავდივართ წინ
        AstNode* right = factor(parser);
        node = create_binary_op_node(node, op, right);
    }
    return node;
}

/** @brief არჩევს გამოსახულებას. */
static AstNode* expression(Parser* parser) {
    return term(parser);
}

// --- განცხადებების და დეკლარაციების პარსინგი ---

/** @brief არჩევს "დაბეჭდე" განცხადებას. */
static AstNode* print_statement(Parser* parser) {
    AstNode* value = expression(parser);
    consume(parser, TOKEN_NEWLINE, "მოსალოდნელი იყო ახალი ხაზი 'დაბეჭდე'-ს შემდეგ.");
    return create_print_stmt_node(value);
}

/** @brief არჩევს ცვლადის დეკლარაციას. */
static AstNode* var_declaration(Parser* parser) {
    consume(parser, TOKEN_IDENTIFIER, "მოსალოდნელი იყო ცვლადის სახელი.");
    Token name = parser->previous;
    consume(parser, TOKEN_EQUAL, "მოსალოდნელი იყო '=' ცვლადის დეკლარაციაში.");
    AstNode* initializer = expression(parser);
    consume(parser, TOKEN_NEWLINE, "მოსალოდნელი იყო ახალი ხაზი დეკლარაციის შემდეგ.");
    return create_var_decl_node(name, initializer);
}

/** @brief არჩევს განცხადებას. */
static AstNode* statement(Parser* parser) {
    if (match(parser, TOKEN_დაბეჭდე)) {
        return print_statement(parser);
    }
    error_at(parser, &parser->current, "უცნობი განცხადება.");
    return NULL;
}

/** @brief არჩევს დეკლარაციას (უმაღლესი დონის კონსტრუქცია). */
static AstNode* declaration(Parser* parser) {
    if (match(parser, TOKEN_ცვლადი)) {
        return var_declaration(parser);
    }
    return statement(parser);
}

// --- მთავარი ფუნქციები ---

/** @brief პარსერის ინიციალიზაცია. */
void init_parser(Parser* parser, Lexer* lexer) {
    parser->lexer = lexer;
    parser->had_error = false;
    parser->panic_mode = false;
    advance(parser);
}

/** @brief არჩევს პროგრამას, რომელიც შედგება მრავალი დეკლარაციისგან. */
AstNode* parse(Parser* parser) {
    AstNode* program = create_program_node();
    while (!check(parser, TOKEN_EOF)) {
        AstNode* decl = declaration(parser);
        if (decl) {
            add_statement_to_program(program, decl);
        }
        if (parser->panic_mode) {
             while(!check(parser, TOKEN_EOF) && parser->previous.type != TOKEN_NEWLINE) {
                 advance(parser);
             }
             parser->panic_mode = false;
        }
    }
    return program;
}