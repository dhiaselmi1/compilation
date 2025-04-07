#ifndef SLR_PARSER_H
#define SLR_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_SYMBOLS 100
#define MAX_RULES 100
#define MAX_RULE_LENGTH 50
#define MAX_TERMINALS 50
#define MAX_NON_TERMINALS 50
#define MAX_STATES 200
#define MAX_ITEMS 500
#define MAX_FIRST_FOLLOW 50
#define EPSILON_SYMBOL '$'
#define END_MARKER '#'

// Types d'actions dans la table SLR
typedef enum {
    ACTION_SHIFT,
    ACTION_REDUCE,
    ACTION_ACCEPT,
    ACTION_ERROR
} ActionType;

// Structure pour une règle de production
typedef struct {
    char left; // Non-terminal à gauche de la production
    char right[MAX_RULE_LENGTH]; // Partie droite de la production
    int id; // Identifiant unique de la règle
} SLRRule;

// Structure pour un item LR(0)
typedef struct {
    int rule_index; // Index de la règle
    int dot_position; // Position du point dans la partie droite
} LRItem;

// Structure pour un état de l'automate LR(0)
typedef struct {
    LRItem items[MAX_ITEMS];
    int num_items;
    int state_id;
} LRState;

// Structure pour une action dans la table SLR
typedef struct {
    ActionType type;
    int value; // État pour shift, règle pour reduce
} SLRAction;

// Structure pour la grammaire et l'analyseur SLR
typedef struct {
    // Grammaire
    char terminals[MAX_TERMINALS];
    int num_terminals;
    
    char non_terminals[MAX_NON_TERMINALS];
    int num_non_terminals;
    
    SLRRule rules[MAX_RULES];
    int num_rules;
    
    char start_symbol;
    
    // Ensembles FIRST et FOLLOW
    char first[MAX_NON_TERMINALS][MAX_FIRST_FOLLOW];
    int first_count[MAX_NON_TERMINALS];
    
    char follow[MAX_NON_TERMINALS][MAX_FIRST_FOLLOW];
    int follow_count[MAX_NON_TERMINALS];
    
    // Automate LR(0)
    LRState states[MAX_STATES];
    int num_states;
    
    // Transitions entre états
    int goto_table[MAX_STATES][MAX_SYMBOLS];
    
    // Table d'analyse SLR
    SLRAction action_table[MAX_STATES][MAX_TERMINALS + 1]; // +1 pour le marqueur de fin
} SLRParser;

// Fonctions de manipulation de la grammaire
void slr_init_parser(SLRParser *parser);
void slr_add_terminal(SLRParser *parser, char terminal);
void slr_add_non_terminal(SLRParser *parser, char non_terminal);
void slr_add_rule(SLRParser *parser, char left, const char *right);
void slr_set_start_symbol(SLRParser *parser, char start_symbol);
int slr_get_non_terminal_index(SLRParser *parser, char non_terminal);
int slr_get_terminal_index(SLRParser *parser, char terminal);
int slr_get_symbol_index(SLRParser *parser, char symbol);
bool slr_is_terminal(SLRParser *parser, char symbol);
bool slr_is_non_terminal(SLRParser *parser, char symbol);
void slr_print_grammar(SLRParser *parser);

// Algorithmes pour l'analyseur SLR
void slr_construct_automaton(SLRParser *parser);
void slr_compute_first(SLRParser *parser);
void slr_compute_follow(SLRParser *parser);
void slr_construct_slr_table(SLRParser *parser);
bool slr_parse_input(SLRParser *parser, const char *input);

// Fonctions auxiliaires
void slr_print_item(SLRParser *parser, LRItem item);
void slr_print_state(SLRParser *parser, LRState state);
void slr_print_automaton(SLRParser *parser);
void slr_print_slr_table(SLRParser *parser);

#endif /* SLR_PARSER_H */
