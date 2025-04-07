#ifndef GRAMMAR_H
#define GRAMMAR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_SYMBOLS 100
#define MAX_RULES 100
#define MAX_RULE_LENGTH 50
#define MAX_TERMINALS 50
#define MAX_NON_TERMINALS 50
#define MAX_FIRST_FOLLOW 50
#define EPSILON_SYMBOL '$'

typedef struct {
    char left; // Non-terminal à gauche de la production
    char right[MAX_RULE_LENGTH]; // Partie droite de la production
} Rule;

typedef struct {
    char terminals[MAX_TERMINALS];
    int num_terminals;
    
    char non_terminals[MAX_NON_TERMINALS];
    int num_non_terminals;
    
    Rule rules[MAX_RULES];
    int num_rules;
    
    char start_symbol;
    
    // Ensembles FIRST et FOLLOW pour chaque non-terminal
    char first[MAX_NON_TERMINALS][MAX_FIRST_FOLLOW];
    int first_count[MAX_NON_TERMINALS];
    
    char follow[MAX_NON_TERMINALS][MAX_FIRST_FOLLOW];
    int follow_count[MAX_NON_TERMINALS];
    
    // Table LL(1)
    int parse_table[MAX_NON_TERMINALS][MAX_TERMINALS];
} Grammar;

// Fonctions de manipulation de la grammaire
void init_grammar(Grammar *grammar);
void add_terminal(Grammar *grammar, char terminal);
void add_non_terminal(Grammar *grammar, char non_terminal);
void add_rule(Grammar *grammar, char left, const char *right);
void set_start_symbol(Grammar *grammar, char start_symbol);
int get_non_terminal_index(Grammar *grammar, char non_terminal);
int get_terminal_index(Grammar *grammar, char terminal);
bool is_terminal(Grammar *grammar, char symbol);
bool is_non_terminal(Grammar *grammar, char symbol);
void print_grammar(Grammar *grammar);

// Algorithmes pour l'analyseur LL(1)
void eliminate_left_recursion(Grammar *grammar);
void compute_first(Grammar *grammar);
void compute_follow(Grammar *grammar);
void construct_ll1_table(Grammar *grammar);
bool parse_input(Grammar *grammar, const char *input);

#endif /* GRAMMAR_H */
