/**
* @file parser.h
 * @author გიორგი მაღრაძე (magradze.giorgi@gmail.com)
 * @brief სინტაქსური ანალიზატორის (პარსერის) დეკლარაციები.
 * @version 0.1
 * @date 2024-05-21
 *
 * @copyright Copyright (c) 2024
 * Website: https://magradze.dev
 * GitHub: https://github.com/magradze
 */
#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"

/**
 * @brief პარსერის სტრუქტურა, რომელიც ინახავს პარსინგის მდგომარეობას.
 */
typedef struct {
    Lexer* lexer;          // ლექსერის მისამართი, საიდანაც ვიღებთ ტოკენებს
    Token current;         // მიმდინარე ტოკენი
    Token previous;        // წინა ტოკენი
    bool had_error;        // დროშა, რომელიც აღნიშნავს, მოხდა თუ არა სინტაქსური შეცდომა
    bool panic_mode;       // პანიკის რეჟიმი შეცდომების შემდეგ აღდგენისთვის
} Parser;

/**
 * @brief პარსერის ინიციალიზაცია.
 * @param parser პარსერის ობიექტის მისამართი.
 * @param lexer ინიციალიზებული ლექსერის მისამართი.
 */
void init_parser(Parser* parser, Lexer* lexer);

/**
 * @brief არჩევს ტოკენების ნაკადს და აბრუნებს აგებულ AST-ს.
 * @param parser პარსერის ობიექტი.
 * @return AstNode* აგებული AST-ის ფესვი. შეცდომის შემთხვევაში NULL.
 */
AstNode* parse(Parser* parser);

#endif // PARSER_H