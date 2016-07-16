//
//  main.cpp
//  CompilerHomework
//
//  Created by 郑铜亚 on 16/5/3.
//  Copyright © 2016年 郑铜亚. All rights reserved.
//

#include <stack>
#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>
#include "fa.hpp"

int main(int argc, const char * argv[]) {
    char scien_file[] = "science-count.txt";
//    fa nfa = *new fa();
//    nfa.init(in_file);
//    nfa.nfa_print();
//    nfa.to_dfa();
//    nfa.dfa_print();
//    string tmp;
//    while(cin >> tmp)
//        cout << tmp << " MAX:" << nfa.dfa_match(tmp) << endl;
//        cout << "hello, world" << endl;
//    char grammar_file[] = "example_grammar.txt";
//    fa*  lr_dfa = new fa();
//    lr_dfa->lr_init(grammar_file);
//    lr_dfa->build_lr_table();
//    lr_dfa->lr_dfa_print();
//    lr_dfa->lr_graph_print();
//    string temp; int status;
//    lr_dfa->lr_stack_init();
//    while(cin >> temp)
//    {
//        do {
//            status = lr_dfa->lr_dfa_match(temp);
//        } while(status == REDUCE);
//        
//        if(status == ACC)
//        {
//            printf("ACC\n"); break;
//        }
//        if(status == ERR)
//        {
//            printf("ERR\n"); break;
//        }
//    }
    //    auto, short, int, long, float, double,
    //    char, struct, union, enum, typedef, const,
    //    unsigned, signed, extern, register, static, volatile,
    //    void, if, else, switch, case, for,
    //        do, while, goto, continue, break, default,
    //            sizeof, return
//    vector<string> key_words_v = {"auto", "short", "int", "long", "float", "double",
//                                "char", "struct", "union", "enum", "typedef", "const",
//                                "unsigned", "signed", "extern", "register", "static",
//                                "volatile", "void", "if", "else", "switch", "case",
//                                "for", "do", "while", "goto", "continue", "break", "default",
//                                "sizeof", "return"};
    vector<string> key_words_v = {"int", "main", "while", "if", "else", "return", "integer", "float", "string"};
    vector<string> operators_v = {"[", "]", "->", ".", "!", "~", "++", "--", "-",
                                "*", "&", "sizeof", "*", "/", "%", "+", "-", ">>", "<<",
                                "<", "<=", ">", ">=", "==", "!=", "&", "^", "|", "&&", "||",
                                "?", ":", "=", "+=", "-=", "*=", "/=", "%=", ">>=", "<<=",
                                "&=", "^=", "|="};
    vector<string> delimiters_v = {"(", ")", ",", ";", "{", "}"};
    set<string> key_words(key_words_v.begin(), key_words_v.end());
    set<string> operators(operators_v.begin(), operators_v.end());
    set<string> delimiters(delimiters_v.begin(), delimiters_v.end());
    char id_file[] = "identifier.txt", string_file[] = "string.txt", integer_file[] = "integer.txt", float_file[] = "float.txt";
    char c_grammar_file[] = "c_simple.txt";
    char source_file[] = "example.c";
    
    fa* id_dfa = new fa(); id_dfa->init(id_file); id_dfa->to_dfa();
    fa* string_dfa = new fa();  string_dfa->init(string_file); string_dfa->to_dfa();
    fa* integer_dfa = new fa(); integer_dfa->init(integer_file); integer_dfa->to_dfa();
    fa* float_dfa = new fa(); float_dfa->init(float_file); float_dfa->to_dfa();
    fa* science_dfa = new fa(); science_dfa->init(scien_file); science_dfa->to_dfa();
    fa* c_grammar = new fa();   c_grammar->lr_init(c_grammar_file);
    c_grammar->build_lr_table();
    //freopen("lr_dfa.out", "w", stdout);
    c_grammar->lr_dfa_print();
    c_grammar->lr_graph_print();
    
    string c ; int len = 0 ; char mid_file[] = "token.txt";
    ifstream in(source_file);
    ofstream out(mid_file);
    while(in >> c)
    {
        while(c.length() > 0)
        {
            len = id_dfa->dfa_match(c);
            if (len != 0)
            {
                string tmp = c.substr(0, len);
                if(key_words.find(tmp) != key_words.end())
                    out << "KEYWORD\t" << tmp << endl;
                else
                    out << "IDENTIFIER\t" << tmp  << endl;
                c = c.substr(len, c.length() - len);
                continue;
            }
            // Not IDENTIFIER
            len = science_dfa->dfa_match(c);
            if(len > 0)
            {
                out << "CONST\t" << c.substr(0, len) << endl;
                c = c.substr(len, c.length() - len);
                continue;
            }
            len = float_dfa->dfa_match(c);
            if (len > 0)
            {
                out << "CONST\t" << c.substr(0, len) << endl;
                c = c.substr(len, c.length() - len);
                continue;
            }
            len = integer_dfa->dfa_match(c);
            if (len > 0)
            {
                string tmp = c.substr(0, len);
                out << "CONST\t" << tmp << endl;
                c = c.substr(len, c.length() - len);
                continue;
            }
            len = string_dfa->dfa_match(c);
            if (len > 0)
            {
                out << "CONST\t" << c.substr(0, len) << endl;
                c = c.substr(len, c.length() - len);
                continue;
            }
            if (operators.find(c.substr(0, 1)) != operators.end())
            {
                if (c.length() >= 2 && operators.find(c.substr(0, 2)) != operators.end())
                {
                    out << "OPERATOR\t" << c.substr(0, 2) << endl;
                    c = c.substr(2, c.length() - 2);
                    continue;
                }
                else
                {
                    out << "OPERATOR\t" << c.substr(0, 1) << endl;
                    c = c.substr(1, c.length() - 1);
                    continue;
                }
            }
            // Not IDENTIFIER or CONST or OPERATOR
            if( delimiters.find(c.substr(0, 1)) != delimiters.end())
            {
                out << "DELIMITER\t" << c.substr(0, 1) << endl;
                c = c.substr(1, c.length() - 1);
                continue;
            }
        }
    }
    out << "KEYWORD\t" << "###" << endl;
    in.close();
    out.close();
    in.open(mid_file);
    string type, temp; int status;
    c_grammar->lr_stack_init();
    while (in >> type >> temp)
    {
        if(type == "IDENTIFIER")
            temp = "id";
        if(type == "OPERATOR")
            temp = "operator";
        if(type == "CONST")
            temp = "const";
        do {
            status = c_grammar->lr_dfa_match(temp);
        } while(status == REDUCE);
        if(status == ACC)
        {
            printf("ACC\n"); break;
        }
        if(status == ERR)
        {
            printf("ERR\n"); break;
        }
    }
    return 0;
}