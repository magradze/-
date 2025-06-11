/**
* @file codegen.h
 * @author გიორგი მაღრაძე (magradze.giorgi@gmail.com)
 * @brief C კოდის გენერატორის დეკლარაციები.
 * @version 0.1
 * @date 2024-05-21
 *
 * @copyright Copyright (c) 2024
 * Website: https://magradze.dev
 * GitHub: https://github.com/magradze
 */
#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"
#include <stdio.h> // FILE* ტიპისთვის

/**
 * @brief გენერირებულ C კოდს წერს მითითებულ ფაილში.
 * @param node დასამუშავებელი AST ხის ფესვი.
 * @param outfile ფაილი, რომელშიც ჩაიწერება C კოდი.
 */
void generate_code(AstNode* node, FILE* outfile);

#endif // CODEGEN_H