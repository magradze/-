/**
 * @file main.c
 * @author გიორგი მაღრაძე (magradze.giorgi@gmail.com)
 * @brief "მანუსკრიპტი" კომპილატორის მთავარი შესრულების წერტილი.
 * @version 0.3
 * @date 2024-05-21
 *
 * @copyright Copyright (c) 2024
 * Website: https://magradze.dev
 * GitHub: https://github.com/magradze
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"

/**
 * @brief ბეჭდავს პროგრამის გამოყენების ინსტრუქციას სტანდარტულ შეცდომების ნაკადში.
 * @param program_name პროგრამის სახელი (argv[0]).
 */
void print_usage(const char* program_name) {
    fprintf(stderr, "გამოყენება: %s <ფაილის_სახელი.მს>\n", program_name);
}

/**
 * @brief კითხულობს ფაილის სრულ შიგთავსს და აბრუნებს მას დინამიურად გამოყოფილ მეხსიერებაში.
 * @param filename წასაკითხი ფაილის სახელი.
 * @return char* ფაილის შიგთავსი. წარუმატებლობის შემთხვევაში აბრუნებს NULL.
 *         დაბრუნებული მეხსიერება უნდა განთავისუფლდეს free()-ით.
 */
char* read_file_content(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        perror("შეცდომა ფაილის გახსნისას");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = (char*)malloc(length + 1);
    if (!buffer) {
        fprintf(stderr, "მეხსიერების გამოყოფის შეცდომა\n");
        fclose(file);
        return NULL;
    }

    if (fread(buffer, 1, (size_t)length, file) != (size_t)length) {
        fprintf(stderr, "ფაილის წაკითხვის შეცდომა\n");
        free(buffer);
        fclose(file);
        return NULL;
    }

    buffer[length] = '\0';
    fclose(file);
    return buffer;
}

/**
 * @brief ბეჭდავს ტოკენის შესახებ დეტალურ ინფორმაციას ფორმატირებულად.
 * @param token დასაბეჭდი ტოკენი.
 */
static void print_token(Token token) {
    const char* type_str;
    switch (token.type) {
        case TOKEN_LPAREN: type_str = "LPAREN"; break;
        case TOKEN_RPAREN: type_str = "RPAREN"; break;
        case TOKEN_COMMA: type_str = "COMMA"; break;
        case TOKEN_DOT: type_str = "DOT"; break;
        case TOKEN_MINUS: type_str = "MINUS"; break;
        case TOKEN_PLUS: type_str = "PLUS"; break;
        case TOKEN_SLASH: type_str = "SLASH"; break;
        case TOKEN_STAR: type_str = "STAR"; break;
        case TOKEN_COLON: type_str = "COLON"; break;
        case TOKEN_BANG: type_str = "BANG"; break;
        case TOKEN_BANG_EQUAL: type_str = "BANG_EQUAL"; break;
        case TOKEN_EQUAL: type_str = "EQUAL"; break;
        case TOKEN_EQUAL_EQUAL: type_str = "EQUAL_EQUAL"; break;
        case TOKEN_GREATER: type_str = "GREATER"; break;
        case TOKEN_GREATER_EQUAL: type_str = "GREATER_EQUAL"; break;
        case TOKEN_LESS: type_str = "LESS"; break;
        case TOKEN_LESS_EQUAL: type_str = "LESS_EQUAL"; break;
        case TOKEN_IDENTIFIER: type_str = "IDENTIFIER"; break;
        case TOKEN_STRING: type_str = "STRING"; break;
        case TOKEN_NUMBER: type_str = "NUMBER"; break;
        case TOKEN_თუ: type_str = "KEYWORD_თუ"; break;
        case TOKEN_სხვა: type_str = "KEYWORD_სხვა"; break;
        case TOKEN_ფუნქცია: type_str = "KEYWORD_ფუნქცია"; break;
        case TOKEN_ცვლადი: type_str = "KEYWORD_ცვლადი"; break;
        case TOKEN_დაბეჭდე: type_str = "KEYWORD_დაბეჭდე"; break;
        // TODO: დაამატე სხვა საკვანძო სიტყვების ბეჭდვაც აქ...
        case TOKEN_NEWLINE: type_str = "NEWLINE"; break;
        case TOKEN_INDENT: type_str = "INDENT"; break;
        case TOKEN_DEDENT: type_str = "DEDENT"; break;
        case TOKEN_ERROR: type_str = "ERROR"; break;
        case TOKEN_EOF: type_str = "EOF"; break;
        default: type_str = "UNKNOWN"; break;
    }

    printf("Line %-4d | %-20s | '%.*s'\n",
           token.line,
           type_str,
           token.length,
           token.start);
}

/**
 * @brief პროგრამის მთავარი ფუნქცია.
 * @param argc არგუმენტების რაოდენობა.
 * @param argv არგუმენტების მასივი.
 * @return int პროგრამის გამოსვლის კოდი (0 - წარმატება, 1 - შეცდომა).
 */
int main(int argc, char* argv[]) {
    if (argc != 2) {
        print_usage(argv[0]);
        return 1;
    }

    const char* filename = argv[1];
    if (strstr(filename, ".მს") == NULL) {
        fprintf(stderr, "შეცდომა: ფაილს უნდა ჰქონდეს '.მს' გაფართოება.\n");
        return 1;
    }

    char* source_code = read_file_content(filename);
    if (!source_code) {
        return 1;
    }

    Lexer lexer;
    init_lexer(&lexer, source_code);

    printf("--- ლექსერის მიერ დაგენერირებული ტოკენები ---\n");
    while (true) {
        Token token = scan_token(&lexer);
        print_token(token);

        if (token.type == TOKEN_EOF || token.type == TOKEN_ERROR) {
            break;
        }
    }

    free(source_code);
    return 0;
}