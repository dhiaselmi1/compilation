#include "slr_parser.h"

int main() {
    SLRParser parser;
    slr_init_parser(&parser);
    
    // Exemple de grammaire pour l'analyseur SLR
    // E -> E+T | T
    // T -> T*F | F
    // F -> (E) | id
    
    printf("=== Exemple de grammaire pour l'analyseur SLR ===\n");
    slr_set_start_symbol(&parser, 'E');
    
    slr_add_rule(&parser, 'E', "E+T");
    slr_add_rule(&parser, 'E', "T");
    slr_add_rule(&parser, 'T', "T*F");
    slr_add_rule(&parser, 'T', "F");
    slr_add_rule(&parser, 'F', "(E)");
    slr_add_rule(&parser, 'F', "i");  // 'i' représente 'id'
    
    slr_print_grammar(&parser);
    
    printf("\n=== Construction de l'automate LR(0) ===\n");
    slr_construct_automaton(&parser);
    slr_print_automaton(&parser);
    
    printf("\n=== Calcul des ensembles FIRST ===\n");
    slr_compute_first(&parser);
    
    for (int i = 0; i < parser.num_non_terminals; i++) {
        printf("FIRST(%c) = { ", parser.non_terminals[i]);
        for (int j = 0; j < parser.first_count[i]; j++) {
            printf("%c ", parser.first[i][j]);
        }
        printf("}\n");
    }
    
    printf("\n=== Calcul des ensembles FOLLOW ===\n");
    slr_compute_follow(&parser);
    
    for (int i = 0; i < parser.num_non_terminals; i++) {
        printf("FOLLOW(%c) = { ", parser.non_terminals[i]);
        for (int j = 0; j < parser.follow_count[i]; j++) {
            printf("%c ", parser.follow[i][j]);
        }
        printf("}\n");
    }
    
    printf("\n=== Construction de la table SLR ===\n");
    slr_construct_slr_table(&parser);
    slr_print_slr_table(&parser);
    
    printf("\n=== Analyse d'une chaîne d'entrée ===\n");
    const char *input = "i+i*i";
    printf("Entrée: %s\n", input);
    
    bool accepted = slr_parse_input(&parser, input);
    printf("Résultat: %s\n", accepted ? "Accepté" : "Rejeté");
    
    // Exemple avec une grammaire plus simple pour tester
    printf("\n\n=== Exemple avec une grammaire plus simple ===\n");
    SLRParser simple_parser;
    slr_init_parser(&simple_parser);
    
    // S -> AB
    // A -> aA | ε
    // B -> bB | ε
    
    slr_set_start_symbol(&simple_parser, 'S');
    
    slr_add_rule(&simple_parser, 'S', "AB");
    slr_add_rule(&simple_parser, 'A', "aA");
    slr_add_rule(&simple_parser, 'A', "$");  // ε est représenté par $
    slr_add_rule(&simple_parser, 'B', "bB");
    slr_add_rule(&simple_parser, 'B', "$");  // ε est représenté par $
    
    slr_print_grammar(&simple_parser);
    
    printf("\n=== Construction de l'automate LR(0) ===\n");
    slr_construct_automaton(&simple_parser);
    slr_print_automaton(&simple_parser);
    
    printf("\n=== Calcul des ensembles FIRST ===\n");
    slr_compute_first(&simple_parser);
    
    for (int i = 0; i < simple_parser.num_non_terminals; i++) {
        printf("FIRST(%c) = { ", simple_parser.non_terminals[i]);
        for (int j = 0; j < simple_parser.first_count[i]; j++) {
            printf("%c ", simple_parser.first[i][j]);
        }
        printf("}\n");
    }
    
    printf("\n=== Calcul des ensembles FOLLOW ===\n");
    slr_compute_follow(&simple_parser);
    
    for (int i = 0; i < simple_parser.num_non_terminals; i++) {
        printf("FOLLOW(%c) = { ", simple_parser.non_terminals[i]);
        for (int j = 0; j < simple_parser.follow_count[i]; j++) {
            printf("%c ", simple_parser.follow[i][j]);
        }
        printf("}\n");
    }
    
    printf("\n=== Construction de la table SLR ===\n");
    slr_construct_slr_table(&simple_parser);
    slr_print_slr_table(&simple_parser);
    
    printf("\n=== Analyse d'une chaîne d'entrée ===\n");
    const char *simple_input = "aabb";
    printf("Entrée: %s\n", simple_input);
    
    bool simple_accepted = slr_parse_input(&simple_parser, simple_input);
    printf("Résultat: %s\n", simple_accepted ? "Accepté" : "Rejeté");
    
    return 0;
}
