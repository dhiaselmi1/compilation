#include "grammar.h"

// Initialiser la grammaire
void init_grammar(Grammar *grammar) {
    grammar->num_terminals = 0;
    grammar->num_non_terminals = 0;
    grammar->num_rules = 0;
    
    // Initialiser les tableaux FIRST et FOLLOW
    for (int i = 0; i < MAX_NON_TERMINALS; i++) {
        grammar->first_count[i] = 0;
        grammar->follow_count[i] = 0;
        
        // Initialiser la table d'analyse à -1 (erreur)
        for (int j = 0; j < MAX_TERMINALS; j++) {
            grammar->parse_table[i][j] = -1;
        }
    }
}

// Ajouter un terminal à la grammaire
void add_terminal(Grammar *grammar, char terminal) {
    // Vérifier si le terminal existe déjà
    for (int i = 0; i < grammar->num_terminals; i++) {
        if (grammar->terminals[i] == terminal) {
            return;
        }
    }
    
    grammar->terminals[grammar->num_terminals++] = terminal;
}

// Ajouter un non-terminal à la grammaire
void add_non_terminal(Grammar *grammar, char non_terminal) {
    // Vérifier si le non-terminal existe déjà
    for (int i = 0; i < grammar->num_non_terminals; i++) {
        if (grammar->non_terminals[i] == non_terminal) {
            return;
        }
    }
    
    grammar->non_terminals[grammar->num_non_terminals++] = non_terminal;
}

// Ajouter une règle à la grammaire
void add_rule(Grammar *grammar, char left, const char *right) {
    if (grammar->num_rules >= MAX_RULES) {
        printf("Erreur: Nombre maximum de règles atteint\n");
        return;
    }
    
    grammar->rules[grammar->num_rules].left = left;
    strcpy(grammar->rules[grammar->num_rules].right, right);
    grammar->num_rules++;
    
    // Ajouter automatiquement le non-terminal à gauche
    add_non_terminal(grammar, left);
    
    // Ajouter automatiquement les terminaux et non-terminaux à droite
    for (int i = 0; right[i] != '\0'; i++) {
        char symbol = right[i];
        
        // Si c'est une lettre majuscule, on considère que c'est un non-terminal
        if (symbol >= 'A' && symbol <= 'Z') {
            add_non_terminal(grammar, symbol);
        } 
        // Si c'est différent de epsilon, on considère que c'est un terminal
        else if (symbol != EPSILON_SYMBOL) {
            add_terminal(grammar, symbol);
        }
    }
}

// Définir le symbole de départ
void set_start_symbol(Grammar *grammar, char start_symbol) {
    grammar->start_symbol = start_symbol;
    
    // S'assurer que le symbole de départ est un non-terminal
    add_non_terminal(grammar, start_symbol);
}

// Obtenir l'index d'un non-terminal
int get_non_terminal_index(Grammar *grammar, char non_terminal) {
    for (int i = 0; i < grammar->num_non_terminals; i++) {
        if (grammar->non_terminals[i] == non_terminal) {
            return i;
        }
    }
    return -1;
}

// Obtenir l'index d'un terminal
int get_terminal_index(Grammar *grammar, char terminal) {
    for (int i = 0; i < grammar->num_terminals; i++) {
        if (grammar->terminals[i] == terminal) {
            return i;
        }
    }
    return -1;
}

// Vérifier si un symbole est un terminal
bool is_terminal(Grammar *grammar, char symbol) {
    return get_terminal_index(grammar, symbol) != -1;
}

// Vérifier si un symbole est un non-terminal
bool is_non_terminal(Grammar *grammar, char symbol) {
    return get_non_terminal_index(grammar, symbol) != -1;
}

// Afficher la grammaire
void print_grammar(Grammar *grammar) {
    printf("Grammaire:\n");
    printf("Terminaux: ");
    for (int i = 0; i < grammar->num_terminals; i++) {
        printf("%c ", grammar->terminals[i]);
    }
    printf("\nNon-terminaux: ");
    for (int i = 0; i < grammar->num_non_terminals; i++) {
        printf("%c ", grammar->non_terminals[i]);
    }
    printf("\nSymbole de départ: %c\n", grammar->start_symbol);
    printf("Règles:\n");
    for (int i = 0; i < grammar->num_rules; i++) {
        printf("%c -> %s\n", grammar->rules[i].left, grammar->rules[i].right);
    }
}

// a. Algorithme pour éliminer la récursivité à gauche
void eliminate_left_recursion(Grammar *grammar) {
    // Créer une copie des règles originales
    Rule original_rules[MAX_RULES];
    int original_num_rules = grammar->num_rules;
    
    for (int i = 0; i < original_num_rules; i++) {
        original_rules[i] = grammar->rules[i];
    }
    
    // Réinitialiser les règles
    grammar->num_rules = 0;
    
    // Pour chaque non-terminal Ai
    for (int i = 0; i < grammar->num_non_terminals; i++) {
        char A = grammar->non_terminals[i];
        char A_prime = A + '\''; // Créer un nouveau non-terminal A'
        bool has_left_recursion = false;
        
        // Vérifier s'il y a une récursivité à gauche pour A
        for (int j = 0; j < original_num_rules; j++) {
            if (original_rules[j].left == A && original_rules[j].right[0] == A) {
                has_left_recursion = true;
                break;
            }
        }
        
        if (has_left_recursion) {
            // Ajouter le nouveau non-terminal A'
            add_non_terminal(grammar, A_prime);
            
            // Traiter toutes les règles pour A
            for (int j = 0; j < original_num_rules; j++) {
                if (original_rules[j].left == A) {
                    if (original_rules[j].right[0] == A) {
                        // Règle avec récursivité à gauche: A -> Aα
                        // Transformer en A' -> αA'
                        char new_rule[MAX_RULE_LENGTH];
                        strcpy(new_rule, original_rules[j].right + 1); // Ignorer le premier A
                        strcat(new_rule, &A_prime);
                        add_rule(grammar, A_prime, new_rule);
                    } else {
                        // Règle sans récursivité à gauche: A -> β
                        // Transformer en A -> βA'
                        char new_rule[MAX_RULE_LENGTH];
                        strcpy(new_rule, original_rules[j].right);
                        strcat(new_rule, &A_prime);
                        add_rule(grammar, A, new_rule);
                    }
                }
            }
            
            // Ajouter A' -> ε
            char epsilon[2] = {EPSILON_SYMBOL, '\0'};
            add_rule(grammar, A_prime, epsilon);
        } else {
            // Pas de récursivité à gauche, conserver les règles originales
            for (int j = 0; j < original_num_rules; j++) {
                if (original_rules[j].left == A) {
                    add_rule(grammar, A, original_rules[j].right);
                }
            }
        }
    }
}

// Fonction auxiliaire pour calculer FIRST d'un symbole
void compute_first_of_symbol(Grammar *grammar, char symbol, char *result, int *count) {
    // Si c'est un terminal, FIRST(a) = {a}
    if (is_terminal(grammar, symbol) || symbol == EPSILON_SYMBOL) {
        result[(*count)++] = symbol;
        return;
    }
    
    // Si c'est un non-terminal
    int nt_index = get_non_terminal_index(grammar, symbol);
    
    // Si FIRST a déjà été calculé pour ce non-terminal
    if (grammar->first_count[nt_index] > 0) {
        for (int i = 0; i < grammar->first_count[nt_index]; i++) {
            // Ajouter chaque élément de FIRST(symbol) à result s'il n'y est pas déjà
            char first_symbol = grammar->first[nt_index][i];
            bool already_exists = false;
            
            for (int j = 0; j < *count; j++) {
                if (result[j] == first_symbol) {
                    already_exists = true;
                    break;
                }
            }
            
            if (!already_exists) {
                result[(*count)++] = first_symbol;
            }
        }
        return;
    }
    
    // Pour chaque règle A -> α
    for (int i = 0; i < grammar->num_rules; i++) {
        if (grammar->rules[i].left == symbol) {
            const char *right = grammar->rules[i].right;
            
            // Si A -> ε, ajouter ε à FIRST(A)
            if (right[0] == EPSILON_SYMBOL) {
                bool already_exists = false;
                for (int j = 0; j < *count; j++) {
                    if (result[j] == EPSILON_SYMBOL) {
                        already_exists = true;
                        break;
                    }
                }
                
                if (!already_exists) {
                    result[(*count)++] = EPSILON_SYMBOL;
                }
                continue;
            }
            
            // Pour chaque symbole Y1...Yk dans la partie droite
            int j = 0;
            bool all_derive_epsilon = true;
            
            while (right[j] != '\0' && all_derive_epsilon) {
                char Y = right[j];
                all_derive_epsilon = false;
                
                // Calculer FIRST(Y) récursivement
                char first_Y[MAX_FIRST_FOLLOW];
                int first_Y_count = 0;
                
                if (Y != symbol) { // Éviter la récursion infinie
                    compute_first_of_symbol(grammar, Y, first_Y, &first_Y_count);
                }
                
                // Ajouter tous les éléments de FIRST(Y) sauf ε à FIRST(A)
                for (int k = 0; k < first_Y_count; k++) {
                    if (first_Y[k] != EPSILON_SYMBOL) {
                        bool already_exists = false;
                        for (int l = 0; l < *count; l++) {
                            if (result[l] == first_Y[k]) {
                                already_exists = true;
                                break;
                            }
                        }
                        
                        if (!already_exists) {
                            result[(*count)++] = first_Y[k];
                        }
                    } else {
                        all_derive_epsilon = true;
                    }
                }
                
                j++;
            }
            
            // Si tous les symboles Y1...Yk peuvent dériver ε, ajouter ε à FIRST(A)
            if (all_derive_epsilon && right[0] != '\0') {
                bool already_exists = false;
                for (int j = 0; j < *count; j++) {
                    if (result[j] == EPSILON_SYMBOL) {
                        already_exists = true;
                        break;
                    }
                }
                
                if (!already_exists) {
                    result[(*count)++] = EPSILON_SYMBOL;
                }
            }
        }
    }
}

// b. Algorithme pour calculer les premiers (FIRST) de chaque non-terminal
void compute_first(Grammar *grammar) {
    // Initialiser les ensembles FIRST
    for (int i = 0; i < grammar->num_non_terminals; i++) {
        grammar->first_count[i] = 0;
    }
    
    // Calculer FIRST pour chaque non-terminal
    for (int i = 0; i < grammar->num_non_terminals; i++) {
        char nt = grammar->non_terminals[i];
        compute_first_of_symbol(grammar, nt, grammar->first[i], &grammar->first_count[i]);
    }
}

// Fonction auxiliaire pour calculer FIRST d'une chaîne de symboles
void compute_first_of_string(Grammar *grammar, const char *string, char *result, int *count) {
    *count = 0;
    
    if (string[0] == '\0') {
        // Chaîne vide, FIRST contient seulement ε
        result[(*count)++] = EPSILON_SYMBOL;
        return;
    }
    
    int i = 0;
    bool all_derive_epsilon = true;
    
    while (string[i] != '\0' && all_derive_epsilon) {
        char symbol = string[i];
        all_derive_epsilon = false;
        
        if (is_terminal(grammar, symbol)) {
            // Si c'est un terminal, FIRST contient seulement ce terminal
            result[(*count)++] = symbol;
            return;
        } else {
            // Si c'est un non-terminal, ajouter FIRST(symbol) sauf ε
            int nt_index = get_non_terminal_index(grammar, symbol);
            
            for (int j = 0; j < grammar->first_count[nt_index]; j++) {
                char first_symbol = grammar->first[nt_index][j];
                
                if (first_symbol != EPSILON_SYMBOL) {
                    // Vérifier si le symbole existe déjà dans result
                    bool already_exists = false;
                    for (int k = 0; k < *count; k++) {
                        if (result[k] == first_symbol) {
                            already_exists = true;
                            break;
                        }
                    }
                    
                    if (!already_exists) {
                        result[(*count)++] = first_symbol;
                    }
                } else {
                    all_derive_epsilon = true;
                }
            }
        }
        
        i++;
    }
    
    // Si tous les symboles peuvent dériver ε, ajouter ε à FIRST
    if (all_derive_epsilon) {
        result[(*count)++] = EPSILON_SYMBOL;
    }
}

// c. Algorithme pour calculer les suivants (FOLLOW) de chaque non-terminal
void compute_follow(Grammar *grammar) {
    // Initialiser les ensembles FOLLOW
    for (int i = 0; i < grammar->num_non_terminals; i++) {
        grammar->follow_count[i] = 0;
    }
    
    // Règle 1: Ajouter $ à FOLLOW(S) où S est le symbole de départ
    int start_index = get_non_terminal_index(grammar, grammar->start_symbol);
    grammar->follow[start_index][grammar->follow_count[start_index]++] = '$';
    
    // Répéter jusqu'à ce qu'aucun changement ne se produise
    bool changes;
    do {
        changes = false;
        
        // Pour chaque règle A -> αBβ
        for (int i = 0; i < grammar->num_rules; i++) {
            char A = grammar->rules[i].left;
            const char *right = grammar->rules[i].right;
            
            // Pour chaque position dans la partie droite
            for (int j = 0; right[j] != '\0'; j++) {
                char B = right[j];
                
                // Si B est un non-terminal
                if (is_non_terminal(grammar, B)) {
                    int B_index = get_non_terminal_index(grammar, B);
                    
                    // Extraire β (reste de la chaîne après B)
                    const char *beta = right + j + 1;
                    
                    // Règle 2: Si A -> αBβ, ajouter FIRST(β) - {ε} à FOLLOW(B)
                    char first_beta[MAX_FIRST_FOLLOW];
                    int first_beta_count = 0;
                    compute_first_of_string(grammar, beta, first_beta, &first_beta_count);
                    
                    bool beta_derives_epsilon = false;
                    
                    for (int k = 0; k < first_beta_count; k++) {
                        if (first_beta[k] == EPSILON_SYMBOL) {
                            beta_derives_epsilon = true;
                        } else {
                            // Ajouter first_beta[k] à FOLLOW(B) s'il n'y est pas déjà
                            bool already_exists = false;
                            for (int l = 0; l < grammar->follow_count[B_index]; l++) {
                                if (grammar->follow[B_index][l] == first_beta[k]) {
                                    already_exists = true;
                                    break;
                                }
                            }
                            
                            if (!already_exists) {
                                grammar->follow[B_index][grammar->follow_count[B_index]++] = first_beta[k];
                                changes = true;
                            }
                        }
                    }
                    
                    // Règle 3: Si A -> αBβ et β peut dériver ε, ou si A -> αB, 
                    // ajouter FOLLOW(A) à FOLLOW(B)
                    if (beta_derives_epsilon || beta[0] == '\0') {
                        int A_index = get_non_terminal_index(grammar, A);
                        
                        for (int k = 0; k < grammar->follow_count[A_index]; k++) {
                            // Ajouter follow[A_index][k] à FOLLOW(B) s'il n'y est pas déjà
                            bool already_exists = false;
                            for (int l = 0; l < grammar->follow_count[B_index]; l++) {
                                if (grammar->follow[B_index][l] == grammar->follow[A_index][k]) {
                                    already_exists = true;
                                    break;
                                }
                            }
                            
                            if (!already_exists) {
                                grammar->follow[B_index][grammar->follow_count[B_index]++] = grammar->follow[A_index][k];
                                changes = true;
                            }
                        }
                    }
                }
            }
        }
    } while (changes);
}

// d. Algorithme pour construire la table LL(1)
void construct_ll1_table(Grammar *grammar) {
    // Initialiser la table à -1 (erreur)
    for (int i = 0; i < grammar->num_non_terminals; i++) {
        for (int j = 0; j < grammar->num_terminals; j++) {
            grammar->parse_table[i][j] = -1;
        }
    }
    
    // Pour chaque règle A -> α
    for (int i = 0; i < grammar->num_rules; i++) {
        char A = grammar->rules[i].left;
        const char *alpha = grammar->rules[i].right;
        int A_index = get_non_terminal_index(grammar, A);
        
        // Calculer FIRST(α)
        char first_alpha[MAX_FIRST_FOLLOW];
        int first_alpha_count = 0;
        compute_first_of_string(grammar, alpha, first_alpha, &first_alpha_count);
        
        // Pour chaque terminal a dans FIRST(α)
        for (int j = 0; j < first_alpha_count; j++) {
            char a = first_alpha[j];
            
            if (a != EPSILON_SYMBOL) {
                int a_index = get_terminal_index(grammar, a);
                
                // Vérifier s'il y a déjà une entrée dans la table
                if (grammar->parse_table[A_index][a_index] != -1) {
                    printf("Attention: La grammaire n'est pas LL(1). Conflit pour [%c, %c]\n", A, a);
                }
                
                // Ajouter la règle à la table
                grammar->parse_table[A_index][a_index] = i;
            } else {
                // Si ε est dans FIRST(α), ajouter A -> α à M[A, b] pour chaque terminal b dans FOLLOW(A)
                for (int k = 0; k < grammar->follow_count[A_index]; k++) {
                    char b = grammar->follow[A_index][k];
                    
                    if (b != '$') {
                        int b_index = get_terminal_index(grammar, b);
                        
                        // Vérifier s'il y a déjà une entrée dans la table
                        if (grammar->parse_table[A_index][b_index] != -1) {
                            printf("Attention: La grammaire n'est pas LL(1). Conflit pour [%c, %c]\n", A, b);
                        }
                        
                        // Ajouter la règle à la table
                        grammar->parse_table[A_index][b_index] = i;
                    }
                }
            }
        }
    }
}

// e. Algorithme pour exploiter la table LL(1)
bool parse_input(Grammar *grammar, const char *input) {
    // Pile pour l'analyse
    char stack[1000];
    int top = 0;
    
    // Initialiser la pile avec le symbole de fin et le symbole de départ
    stack[top++] = '$';
    stack[top++] = grammar->start_symbol;
    
    // Position dans la chaîne d'entrée
    int pos = 0;
    
    printf("Analyse syntaxique:\n");
    printf("Pile\tEntrée\tAction\n");
    
    while (top > 0) {
        // Afficher l'état actuel
        printf("Stack: ");
        for (int i = 0; i < top; i++) {
            printf("%c", stack[i]);
        }
        printf("\tInput: %s\tAction: ", input + pos);
        
        // Symbole en haut de la pile
        char X = stack[--top];
        
        // Symbole courant dans l'entrée
        char a = input[pos];
        
        if (a == '\0') {
            a = '$'; // Fin de l'entrée
        }
        
        if (X == '$') {
            if (a == '$') {
                printf("Accepté\n");
                return true;
            } else {
                printf("Erreur: Fin de pile mais pas de fin d'entrée\n");
                return false;
            }
        } else if (is_terminal(grammar, X)) {
            if (X == a) {
                printf("Match %c\n", a);
                pos++; // Avancer dans l'entrée
            } else {
                printf("Erreur: Terminal attendu %c, trouvé %c\n", X, a);
                return false;
            }
        } else {
            // X est un non-terminal
            int X_index = get_non_terminal_index(grammar, X);
            int a_index = get_terminal_index(grammar, a);
            
            if (a_index == -1) {
                printf("Erreur: Symbole d'entrée %c non reconnu\n", a);
                return false;
            }
            
            int rule_index = grammar->parse_table[X_index][a_index];
            
            if (rule_index == -1) {
                printf("Erreur: Pas de production pour [%c, %c]\n", X, a);
                return false;
            }
            
            // Appliquer la règle
            const char *production = grammar->rules[rule_index].right;
            printf("Appliquer %c -> %s\n", X, production);
            
            // Si la production n'est pas ε, empiler les symboles en ordre inverse
            if (production[0] != EPSILON_SYMBOL) {
                int len = strlen(production);
                for (int i = len - 1; i >= 0; i--) {
                    stack[top++] = production[i];
                }
            }
        }
    }
    
    // Si on arrive ici, c'est que la pile est vide mais pas l'entrée
    if (input[pos] != '\0') {
        printf("Erreur: Pile vide mais entrée non consommée\n");
        return false;
    }
    
    return true;
}
