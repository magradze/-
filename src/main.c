/**
 * @file main.c
 * @author გიორგი მაღრაძე (magradze.giorgi@gmail.com)
 * @brief "მანუსკრიპტი" კომპილატორის მთავარი შესრულების წერტილი.
 * @version 0.1
 * @date 2024-05-21
 *
 * @copyright Copyright (c) 2024
 * Website: https://magradze.dev
 * GitHub: https://github.com/magradze
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief ბეჭდავს პროგრამის გამოყენების ინსტრუქციას.
 *
 * @param program_name პროგრამის სახელი (argv[0]).
 */
void print_usage(const char* program_name) {
    fprintf(stderr, "გამოყენება: %s <ფაილის_სახელი.მს>\n", program_name);
}

/**
 * @brief კითხულობს ფაილის სრულ შიგთავსს და აბრუნებს მას სტრიქონის სახით.
 *
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
 * @brief პროგრამის მთავარი ფუნქცია.
 *
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

    printf("--- ფაილის შიგთავსი ---\n%s\n----------------------\n", source_code);

    // TODO:
    // 1. გადავეცით source_code ლექსერს ტოკენების მისაღებად.
    // 2. ტოკენები გადავეცით პარსერს AST-ის ასაგებად.
    // 3. AST გადავეცით კოდის გენერატორს C კოდის დასაწერად.
    // 4. დავაკომპილიროთ და გავუშვათ გენერირებული C კოდი.

    free(source_code);

    return 0;
}