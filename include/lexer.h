/**
 * @file lexer.h
 * @author გიორგი მაღრაძე (magradze.giorgi@gmail.com)
 * @brief ლექსიკური ანალიზატორის (ლექსერის) დეკლარაციები.
 * @version 0.2
 * @date 2024-05-21
 *
 * @copyright Copyright (c) 2024
 * Website: https://magradze.dev
 * GitHub: https://github.com/magradze
 */
#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>
#include <stdbool.h>

/**
 * @brief "მანუსკრიპტი" ენის ყველა შესაძლო ტოკენის ტიპი.
 */
typedef enum {
    // ერთსიმბოლოიანი ტოკენები
    TOKEN_LPAREN, TOKEN_RPAREN, TOKEN_COMMA, TOKEN_DOT, TOKEN_MINUS,
    TOKEN_PLUS, TOKEN_SLASH, TOKEN_STAR, TOKEN_COLON,

    // ერთ ან ორსიმბოლოიანი ტოკენები
    TOKEN_BANG, TOKEN_BANG_EQUAL, TOKEN_EQUAL, TOKEN_EQUAL_EQUAL,
    TOKEN_GREATER, TOKEN_GREATER_EQUAL, TOKEN_LESS, TOKEN_LESS_EQUAL,

    // ლიტერალები
    TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_NUMBER,

    // საკვანძო სიტყვები (Keywords)
    TOKEN_თუ, TOKEN_სხვა, TOKEN_სხვა_თუ, TOKEN_და, TOKEN_ან, TOKEN_არ,
    TOKEN_ჭეშმარიტი, TOKEN_მცდარი, TOKEN_არარა, TOKEN_ფუნქცია, TOKEN_დაბრუნე,
    TOKEN_სანამ, TOKEN_ყოველი, TOKEN_ში, TOKEN_ცვლადი, TOKEN_დაბეჭდე,

    // Python-ის სტილის სინტაქსისთვის
    TOKEN_NEWLINE,    // ახალი ხაზის სიმბოლო (\n)
    TOKEN_INDENT,     // ბლოკის დასაწყისი (შეწევა)
    TOKEN_DEDENT,     // ბლოკის დასასრული (გამოწევა)

    // სამართავი ტოკენები
    TOKEN_ERROR,      // შეცდომის ტოკენი
    TOKEN_EOF         // ფაილის დასასრული (End Of File)
} TokenType;

/**
 * @brief ტოკენის სტრუქტურა.
 */
typedef struct {
    TokenType type;
    const char* start;
    int length;
    int line;
} Token;

// მაქსიმალური ბუდობრიობის (nesting) დონე
#define MAX_INDENT_LEVELS 16

/**
 * @brief ლექსერის სტრუქტურა.
 */
typedef struct {
    const char* start;
    const char* current;
    int line;
    bool at_start_of_line; // დროშა, რომელიც აღნიშნავს, ვართ თუ არა ხაზის დასაწყისში
    int indent_stack[MAX_INDENT_LEVELS]; // შეწევის დონეების დასტა (stack)
    int indent_level_count;  // ელემენტების რაოდენობა დასტაში
    int dedent_to_emit;      // დასაგენერირებელი DEDENT ტოკენების რაოდენობა
} Lexer;

/**
 * @brief ლექსერის ინიციალიზაცია.
 * @param lexer ლექსერის ობიექტის მისამართი.
 * @param source დასამუშავებელი საწყისი კოდი.
 */
void init_lexer(Lexer* lexer, const char* source);

/**
 * @brief სკანირებს და აბრუნებს შემდეგ ტოკენს.
 * @param lexer ლექსერის ობიექტის მისამართი.
 * @return Token შემდეგი ტოკენი კოდში.
 */
Token scan_token(Lexer* lexer);

#endif // LEXER_H