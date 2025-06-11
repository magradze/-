/**
 * @file lexer.c
 * @author გიორგი მაღრაძე (magradze.giorgi@gmail.com)
 * @brief ლექსიკური ანალიზატორის (ლექსერის) იმპლემენტაცია.
 * @version 0.5
 * @date 2024-05-21
 *
 * @copyright Copyright (c) 2024
 * Website: https://magradze.dev
 * GitHub: https://github.com/magradze
 */

#include <stdio.h>
#include <string.h>
#include "lexer.h"

// --- დამხმარე ფუნქციები ---

static bool is_at_end(Lexer* lexer) {
    return *lexer->current == '\0';
}

static Token make_token(Lexer* lexer, TokenType type) {
    Token token;
    token.type = type;
    token.start = lexer->start;
    token.length = (int)(lexer->current - lexer->start);
    token.line = lexer->line;
    return token;
}

static Token make_virtual_token(Lexer* lexer, TokenType type) {
    Token token;
    token.type = type;
    token.start = lexer->current;
    token.length = 0;
    token.line = lexer->line;
    return token;
}

static Token error_token(Lexer* lexer, const char* message) {
    Token token;
    token.type = TOKEN_ERROR;
    token.start = message;
    token.length = (int)strlen(message);
    token.line = lexer->line;
    return token;
}

static char advance(Lexer* lexer) {
    lexer->current++;
    return lexer->current[-1];
}

static bool match(Lexer* lexer, char expected) {
    if (is_at_end(lexer)) return false;
    if (*lexer->current != expected) return false;
    lexer->current++;
    return true;
}

static char peek(Lexer* lexer) {
    return *lexer->current;
}

static void skip_whitespace(Lexer* lexer) {
    while (true) {
        char c = peek(lexer);
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance(lexer);
                break;
            default:
                return;
        }
    }
}

static bool is_digit(char c) {
    return c >= '0' && c <= '9';
}

static bool is_alpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static Token number(Lexer* lexer) {
    while (is_digit(peek(lexer))) advance(lexer);
    if (peek(lexer) == '.' && is_digit(lexer->current[1])) {
        advance(lexer);
        while (is_digit(peek(lexer))) advance(lexer);
    }
    return make_token(lexer, TOKEN_NUMBER);
}

static Token string(Lexer* lexer) {
    while (peek(lexer) != '"' && !is_at_end(lexer)) {
        if (peek(lexer) == '\n') lexer->line++;
        advance(lexer);
    }

    if (is_at_end(lexer)) return error_token(lexer, "დაუსრულებელი სტრიქონი.");

    advance(lexer);
    return make_token(lexer, TOKEN_STRING);
}

static TokenType identifier_type(Lexer* lexer) {
    const char* str = lexer->start;
    int len = lexer->current - lexer->start;

    if (len == 6 && strncmp(str, "თუ", 6) == 0) return TOKEN_თუ;
    if (len == 12 && strncmp(str, "სხვა", 12) == 0) return TOKEN_სხვა;
    if (len == 15 && strncmp(str, "ფუნქცია", 15) == 0) return TOKEN_ფუნქცია;
    if (len == 15 && strncmp(str, "ცვლადი", 15) == 0) return TOKEN_ცვლადი;
    if (len == 18 && strncmp(str, "დაბეჭდე", 18) == 0) return TOKEN_დაბეჭდე;

    return TOKEN_IDENTIFIER;
}

static Token identifier(Lexer* lexer) {
    while (is_alpha(peek(lexer)) || is_digit(peek(lexer)) || (unsigned char)peek(lexer) > 127) {
        advance(lexer);
    }
    return make_token(lexer, identifier_type(lexer));
}

// --- მთავარი ფუნქციები ---

void init_lexer(Lexer* lexer, const char* source) {
    lexer->start = source;
    lexer->current = source;
    lexer->line = 1;
    lexer->at_start_of_line = true;
    lexer->indent_level_count = 1;
    lexer->indent_stack[0] = 0;
    lexer->dedent_to_emit = 0;
}

Token scan_token(Lexer* lexer) {
    while (true) {
        if (lexer->dedent_to_emit > 0) {
            lexer->dedent_to_emit--;
            return make_virtual_token(lexer, TOKEN_DEDENT);
        }

        if (lexer->at_start_of_line) {
            // თუ ხაზი კომენტარს შეიცავს ან ცარიელია, სრულად გამოვტოვოთ
            if (peek(lexer) == '#' || peek(lexer) == '\n' || peek(lexer) == '\r') {
                while (!is_at_end(lexer) && peek(lexer) != '\n') {
                    advance(lexer);
                }
                // გამოვტოვოთ თვითონ '\n' სიმბოლოც, რომ შემდეგ ხაზზე გადავიდეთ
                if (!is_at_end(lexer)) {
                    advance(lexer);
                    lexer->line++;
                }
                continue; // დავბრუნდეთ ციკლის დასაწყისში შემდეგი ხაზისთვის
            }

            int current_indent = 0;
            while (peek(lexer) == ' ') {
                current_indent++;
                advance(lexer);
            }

            lexer->at_start_of_line = false;
            int last_indent = lexer->indent_stack[lexer->indent_level_count - 1];

            if (current_indent > last_indent) {
                if (lexer->indent_level_count >= MAX_INDENT_LEVELS) {
                    return error_token(lexer, "შეწევის დონე ძალიან ღრმაა.");
                }
                lexer->indent_stack[lexer->indent_level_count++] = current_indent;
                return make_virtual_token(lexer, TOKEN_INDENT);
            } else if (current_indent < last_indent) {
                while (lexer->indent_level_count > 1 && lexer->indent_stack[lexer->indent_level_count - 1] > current_indent) {
                    lexer->indent_level_count--;
                    lexer->dedent_to_emit++;
                }
                if (lexer->indent_stack[lexer->indent_level_count - 1] != current_indent) {
                    return error_token(lexer, "არათანმიმდევრული გამოწევა (dedent).");
                }
                lexer->dedent_to_emit--;
                return make_virtual_token(lexer, TOKEN_DEDENT);
            }
        }
        break; // გამოვიდეთ ციკლიდან, როცა რეალური ტოკენის წაკითხვა გვინდა
    }

    skip_whitespace(lexer);
    lexer->start = lexer->current;

    if (is_at_end(lexer)) {
        if (lexer->indent_level_count > 1) {
            lexer->indent_level_count--;
            return make_virtual_token(lexer, TOKEN_DEDENT);
        }
        return make_virtual_token(lexer, TOKEN_EOF);
    }

    char c = advance(lexer);

    if (is_alpha(c) || (unsigned char)c > 127) return identifier(lexer);
    if (is_digit(c)) return number(lexer);

    switch (c) {
        case '(': return make_token(lexer, TOKEN_LPAREN);
        case ')': return make_token(lexer, TOKEN_RPAREN);
        case ':': return make_token(lexer, TOKEN_COLON);
        case ',': return make_token(lexer, TOKEN_COMMA);
        case '.': return make_token(lexer, TOKEN_DOT);
        case '-': return make_token(lexer, TOKEN_MINUS);
        case '+': return make_token(lexer, TOKEN_PLUS);
        case '/': return make_token(lexer, TOKEN_SLASH);
        case '*': return make_token(lexer, TOKEN_STAR);
        case '!': return make_token(lexer, match(lexer, '=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
        case '=': return make_token(lexer, match(lexer, '=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
        case '<': return make_token(lexer, match(lexer, '=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
        case '>': return make_token(lexer, match(lexer, '=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
        case '"': return string(lexer);
        case '\n':
            lexer->at_start_of_line = true;
            lexer->line++;
            return make_token(lexer, TOKEN_NEWLINE);
    }

    return error_token(lexer, "გაუთვალისწინებელი სიმბოლო.");
}