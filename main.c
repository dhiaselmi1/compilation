#include "grammar.h"

int main() {
    Grammar grammar;
    init_grammar(&grammar);
    
    // Exemple de grammaire avec récursivité à gauche
    // E -> E+T | T
    // T -> T*F | F
    // F -> (E) | id
    
    printf("=== Exemple de grammaire avec récursivité à gauche ===\n");
    set_start_symbol(&grammar, 'E');
    
    add_rule(&grammar, 'E', "E+T");
    add_rule(&grammar, 'E', "T");
    add_rule(&grammar, 'T', "T*F");
    add_rule(&grammar, 'T', "F");
    add_rule(&grammar, 'F', "(E)");
    add_rule(&grammar, 'F', "i");  // 'i' représente 'id'
    
    print_grammar(&grammar);
    
    printf("\n=== Élimination de la récursivité à gauche ===\n");
    eliminate_left_recursion(&grammar);
    print_grammar(&grammar);
    
    printf("\n=== Calcul des ensembles FIRST ===\n");
    compute_first(&grammar);
    
    for (int i = 0; i < grammar.num_non_terminals; i++) {
        printf("FIRST(%c) = { ", grammar.non_terminals[i]);
        for (int j = 0; j < grammar.first_count[i]; j++) {
            printf("%c ", grammar.first[i][j]);
        }
        printf("}\n");
    }
    
    printf("\n=== Calcul des ensembles FOLLOW ===\n");
    compute_follow(&grammar);
    
    for (int i = 0; i < grammar.num_non_terminals; i++) {
        printf("FOLLOW(%c) = { ", grammar.non_terminals[i]);
        for (int j = 0; j < grammar.follow_count[i]; j++) {
            printf("%c ", grammar.follow[i][j]);
        }
        printf("}\n");
    }
    
    printf("\n=== Construction de la table LL(1) ===\n");
    construct_ll1_table(&grammar);
    
    printf("Table LL(1):\n");
    printf("    ");
    for (int j = 0; j < grammar.num_terminals; j++) {
        printf("%c  ", grammar.terminals[j]);
    }
    printf("\n");
    
    for (int i = 0; i < grammar.num_non_terminals; i++) {
        printf("%c   ", grammar.non_terminals[i]);
        for (int j = 0; j < grammar.num_terminals; j++) {
            int rule_index = grammar.parse_table[i][j];
            if (rule_index != -1) {
                printf("%c->%s ", grammar.rules[rule_index].left, grammar.rules[rule_index].right);
            } else {
                printf("-  ");
            }
        }
        printf("\n");
    }
    
    printf("\n=== Analyse d'une chaîne d'entrée ===\n");
    const char *input = "i+i*i";
    printf("Entrée: %s\n", input);
    
    bool accepted = parse_input(&grammar, input);
    printf("Résultat: %s\n", accepted ? "Accepté" : "Rejeté");
    
    // Exemple avec une grammaire plus simple pour tester
    printf("\n\n=== Exemple avec une grammaire plus simple ===\n");
    Grammar simple_grammar;
    init_grammar(&simple_grammar);
    
    // S -> AB
    // A -> aA | ε
    // B -> bB | ε
    
    set_start_symbol(&simple_grammar, 'S');
    
    add_rule(&simple_grammar, 'S', "AB");
    add_rule(&simple_grammar, 'A', "aA");
    add_rule(&simple_grammar, 'A', "$");  // ε est représenté par $
    add_rule(&simple_grammar, 'B', "bB");
    add_rule(&simple_grammar, 'B', "$");  // ε est représenté par $
    
    print_grammar(&simple_grammar);
    
    printf("\n=== Calcul des ensembles FIRST ===\n");
    compute_first(&simple_grammar);
    
    for (int i = 0; i < simple_grammar.num_non_terminals; i++) {
        printf("FIRST(%c) = { ", simple_grammar.non_terminals[i]);
        for (int j = 0; j < simple_grammar.first_count[i]; j++) {
            printf("%c ", simple_grammar.first[i][j]);
        }
        printf("}\n");
    }
    
    printf("\n=== Calcul des ensembles FOLLOW ===\n");
    compute_follow(&simple_grammar);
    
    for (int i = 0; i < simple_grammar.num_non_terminals; i++) {
        printf("FOLLOW(%c) = { ", simple_grammar.non_terminals[i]);
        for (int j = 0; j < simple_grammar.follow_count[i]; j++) {
            printf("%c ", simple_grammar.follow[i][j]);
        }
        printf("}\n");
    }
    
    printf("\n=== Construction de la table LL(1) ===\n");
    construct_ll1_table(&simple_grammar);
    
    printf("Table LL(1):\n");
    printf("    ");
    for (int j = 0; j < simple_grammar.num_terminals; j++) {
        printf("%c  ", simple_grammar.terminals[j]);
    }
    printf("\n");
    
    for (int i = 0; i < simple_grammar.num_non_terminals; i++) {
        printf("%c   ", simple_grammar.non_terminals[i]);
        for (int j = 0; j < simple_grammar.num_terminals; j++) {
            int rule_index = simple_grammar.parse_table[i][j];
            if (rule_index != -1) {
                printf("%c->%s ", simple_grammar.rules[rule_index].left, simple_grammar.rules[rule_index].right);
            } else {
                printf("-  ");
            }
        }
        printf("\n");
    }
    
    printf("\n=== Analyse d'une chaîne d'entrée ===\n");
    const char *simple_input = "aabb";
    printf("Entrée: %s\n", simple_input);
    
    bool simple_accepted = parse_input(&simple_grammar, simple_input);
    printf("Résultat: %s\n", simple_accepted ? "Accepté" : "Rejeté");
    
    return 0;
}
