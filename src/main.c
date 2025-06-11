/**
 * @file main.c
 * @author გიორგი მაღრაძე (magradze.giorgi@gmail.com)
 * @brief "მანუსკრიპტი" კომპილატორის მთავარი შესრულების წერტილი.
 * @version 0.5
 * @date 2024-05-21
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "codegen.h"

/**
 * @brief ბეჭდავს პროგრამის გამოყენების ინსტრუქციას.
 */
void print_usage(const char* program_name) {
    fprintf(stderr, "გამოყენება: %s <ფაილის_სახელი.მს>\n", program_name);
}

/**
 * @brief კითხულობს ფაილის სრულ შიგთავსს.
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
 * @brief აკომპილირებს და უშვებს გენერირებულ C კოდს.
 * @return int პროგრამის გამოსვლის კოდი. 0, თუ წარმატებით დასრულდა.
 */
static int compile_and_run() {
    int gcc_status = system("gcc build/output.c -o build/output_program");
    if (gcc_status != 0) {
        fprintf(stderr, "C კოდის კომპილაცია ვერ მოხერხდა.\n");
        return 1;
    }

    printf("\n--- პროგრამის შესრულების შედეგი ---\n");
    int run_status = system("./build/output_program");
    printf("----------------------------------\n");

    return run_status;
}

/**
 * @brief პროგრამის მთავარი ფუნქცია.
 */
int main(int argc, char* argv[]) {
    if (argc != 2) {
        print_usage(argv[0]);
        return 1;
    }

    char* source_code = read_file_content(argv[1]);
    if (!source_code) return 1;

    Lexer lexer;
    init_lexer(&lexer, source_code);

    Parser parser;
    init_parser(&parser, &lexer);

    AstNode* ast = parse(&parser);

    if (parser.had_error) {
        fprintf(stderr, "პარსინგის შეცდომების გამო კომპილაცია ჩაიშალა.\n");
        free_ast(ast);
        free(source_code);
        return 1;
    }

    print_ast(ast);

    FILE* outfile = fopen("build/output.c", "w");
    if (outfile == NULL) {
        fprintf(stderr, "დროებითი C ფაილის შექმნა ვერ მოხერხდა.\n");
        free_ast(ast);
        free(source_code);
        return 1;
    }

    generate_code(ast, outfile);
    fclose(outfile);

    int result = compile_and_run();

    free_ast(ast);
    free(source_code);

    return result;
}