#include "slr_parser.h"

// Initialiser l'analyseur SLR
void slr_init_parser(SLRParser *parser) {
    parser->num_terminals = 0;
    parser->num_non_terminals = 0;
    parser->num_rules = 0;
    parser->num_states = 0;
    
    // Initialiser les tableaux FIRST et FOLLOW
    for (int i = 0; i < MAX_NON_TERMINALS; i++) {
        parser->first_count[i] = 0;
        parser->follow_count[i] = 0;
    }
    
    // Initialiser la table GOTO à -1 (pas de transition)
    for (int i = 0; i < MAX_STATES; i++) {
        for (int j = 0; j < MAX_SYMBOLS; j++) {
            parser->goto_table[i][j] = -1;
        }
    }
    
    // Initialiser la table ACTION à ERROR
    for (int i = 0; i < MAX_STATES; i++) {
        for (int j = 0; j < MAX_TERMINALS + 1; j++) {
            parser->action_table[i][j].type = ACTION_ERROR;
            parser->action_table[i][j].value = -1;
        }
    }
}

// Ajouter un terminal à l'analyseur
void slr_add_terminal(SLRParser *parser, char terminal) {
    // Vérifier si le terminal existe déjà
    for (int i = 0; i < parser->num_terminals; i++) {
        if (parser->terminals[i] == terminal) {
            return;
        }
    }
    
    parser->terminals[parser->num_terminals++] = terminal;
}

// Ajouter un non-terminal à l'analyseur
void slr_add_non_terminal(SLRParser *parser, char non_terminal) {
    // Vérifier si le non-terminal existe déjà
    for (int i = 0; i < parser->num_non_terminals; i++) {
        if (parser->non_terminals[i] == non_terminal) {
            return;
        }
    }
    
    parser->non_terminals[parser->num_non_terminals++] = non_terminal;
}

// Ajouter une règle à l'analyseur
void slr_add_rule(SLRParser *parser, char left, const char *right) {
    if (parser->num_rules >= MAX_RULES) {
        printf("Erreur: Nombre maximum de règles atteint\n");
        return;
    }
    
    parser->rules[parser->num_rules].left = left;
    strcpy(parser->rules[parser->num_rules].right, right);
    parser->rules[parser->num_rules].id = parser->num_rules;
    parser->num_rules++;
    
    // Ajouter automatiquement le non-terminal à gauche
    slr_add_non_terminal(parser, left);
    
    // Ajouter automatiquement les terminaux et non-terminaux à droite
    for (int i = 0; right[i] != '\0'; i++) {
        char symbol = right[i];
        
        // Si c'est une lettre majuscule, on considère que c'est un non-terminal
        if (symbol >= 'A' && symbol <= 'Z') {
            slr_add_non_terminal(parser, symbol);
        } 
        // Si c'est différent de epsilon, on considère que c'est un terminal
        else if (symbol != EPSILON_SYMBOL) {
            slr_add_terminal(parser, symbol);
        }
    }
}

// Définir le symbole de départ
void slr_set_start_symbol(SLRParser *parser, char start_symbol) {
    parser->start_symbol = start_symbol;
    
    // S'assurer que le symbole de départ est un non-terminal
    slr_add_non_terminal(parser, start_symbol);
    
    // Ajouter une règle spéciale S' -> S pour faciliter l'analyse SLR
    char augmented_start = 'Z'; // On utilise Z comme symbole de départ augmenté
    char start_rule[3] = {start_symbol, '\0'};
    
    // Ajouter le nouveau symbole de départ
    slr_add_non_terminal(parser, augmented_start);
    
    // Ajouter la règle S' -> S
    slr_add_rule(parser, augmented_start, start_rule);
    
    // Mettre à jour le symbole de départ
    parser->start_symbol = augmented_start;
}

// Obtenir l'index d'un non-terminal
int slr_get_non_terminal_index(SLRParser *parser, char non_terminal) {
    for (int i = 0; i < parser->num_non_terminals; i++) {
        if (parser->non_terminals[i] == non_terminal) {
            return i;
        }
    }
    return -1;
}

// Obtenir l'index d'un terminal
int slr_get_terminal_index(SLRParser *parser, char terminal) {
    for (int i = 0; i < parser->num_terminals; i++) {
        if (parser->terminals[i] == terminal) {
            return i;
        }
    }
    return -1;
}

// Obtenir l'index d'un symbole (terminal ou non-terminal)
int slr_get_symbol_index(SLRParser *parser, char symbol) {
    if (slr_is_terminal(parser, symbol)) {
        return slr_get_terminal_index(parser, symbol);
    } else if (slr_is_non_terminal(parser, symbol)) {
        return parser->num_terminals + slr_get_non_terminal_index(parser, symbol);
    }
    return -1;
}

// Vérifier si un symbole est un terminal
bool slr_is_terminal(SLRParser *parser, char symbol) {
    return slr_get_terminal_index(parser, symbol) != -1;
}

// Vérifier si un symbole est un non-terminal
bool slr_is_non_terminal(SLRParser *parser, char symbol) {
    return slr_get_non_terminal_index(parser, symbol) != -1;
}

// Afficher la grammaire
void slr_print_grammar(SLRParser *parser) {
    printf("Grammaire SLR:\n");
    printf("Terminaux: ");
    for (int i = 0; i < parser->num_terminals; i++) {
        printf("%c ", parser->terminals[i]);
    }
    printf("\nNon-terminaux: ");
    for (int i = 0; i < parser->num_non_terminals; i++) {
        printf("%c ", parser->non_terminals[i]);
    }
    printf("\nSymbole de départ: %c\n", parser->start_symbol);
    printf("Règles:\n");
    for (int i = 0; i < parser->num_rules; i++) {
        printf("%d: %c -> %s\n", i, parser->rules[i].left, parser->rules[i].right);
    }
}

// Fonction pour vérifier si deux items LR(0) sont égaux
bool slr_items_equal(LRItem item1, LRItem item2) {
    return (item1.rule_index == item2.rule_index && item1.dot_position == item2.dot_position);
}

// Fonction pour vérifier si un item existe déjà dans un état
bool slr_item_exists(LRState *state, LRItem item) {
    for (int i = 0; i < state->num_items; i++) {
        if (slr_items_equal(state->items[i], item)) {
            return true;
        }
    }
    return false;
}

// Fonction pour ajouter un item à un état
void slr_add_item(LRState *state, LRItem item) {
    if (!slr_item_exists(state, item)) {
        state->items[state->num_items++] = item;
    }
}

// Fonction pour vérifier si deux états sont égaux
bool slr_states_equal(LRState state1, LRState state2) {
    if (state1.num_items != state2.num_items) {
        return false;
    }
    
    for (int i = 0; i < state1.num_items; i++) {
        bool found = false;
        for (int j = 0; j < state2.num_items; j++) {
            if (slr_items_equal(state1.items[i], state2.items[j])) {
                found = true;
                break;
            }
        }
        if (!found) {
            return false;
        }
    }
    
    return true;
}

// Fonction pour vérifier si un état existe déjà dans l'automate
int slr_state_exists(SLRParser *parser, LRState state) {
    for (int i = 0; i < parser->num_states; i++) {
        if (slr_states_equal(parser->states[i], state)) {
            return i;
        }
    }
    return -1;
}

// Fonction pour calculer la fermeture d'un ensemble d'items LR(0)
void slr_closure(SLRParser *parser, LRState *state) {
    bool changes;
    
    do {
        changes = false;
        
        // Pour chaque item [A -> α.Bβ] dans l'état
        for (int i = 0; i < state->num_items; i++) {
            LRItem item = state->items[i];
            SLRRule rule = parser->rules[item.rule_index];
            
            // Si le point est devant un non-terminal B
            if (item.dot_position < strlen(rule.right)) {
                char B = rule.right[item.dot_position];
                
                if (slr_is_non_terminal(parser, B)) {
                    // Pour chaque règle B -> γ
                    for (int j = 0; j < parser->num_rules; j++) {
                        if (parser->rules[j].left == B) {
                            // Ajouter l'item [B -> .γ]
                            LRItem new_item = {j, 0};
                            
                            if (!slr_item_exists(state, new_item)) {
                                slr_add_item(state, new_item);
                                changes = true;
                            }
                        }
                    }
                }
            }
        }
    } while (changes);
}

// Fonction pour calculer l'état suivant (GOTO)
void slr_goto(SLRParser *parser, LRState *state, char symbol, LRState *next_state) {
    next_state->num_items = 0;
    
    // Pour chaque item [A -> α.Xβ] dans l'état
    for (int i = 0; i < state->num_items; i++) {
        LRItem item = state->items[i];
        SLRRule rule = parser->rules[item.rule_index];
        
        // Si le point est devant le symbole X
        if (item.dot_position < strlen(rule.right) && rule.right[item.dot_position] == symbol) {
            // Ajouter l'item [A -> αX.β]
            LRItem new_item = {item.rule_index, item.dot_position + 1};
            slr_add_item(next_state, new_item);
        }
    }
    
    // Calculer la fermeture du nouvel état
    slr_closure(parser, next_state);
}

// a. Algorithme pour construire l'automate LR(0)
void slr_construct_automaton(SLRParser *parser) {
    // Initialiser l'état initial avec l'item [S' -> .S]
    LRState initial_state = {0};
    initial_state.num_items = 0;
    
    // Ajouter l'item [S' -> .S] à l'état initial
    LRItem initial_item = {0, 0}; // La règle 0 est S' -> S
    slr_add_item(&initial_state, initial_item);
    
    // Calculer la fermeture de l'état initial
    slr_closure(parser, &initial_state);
    
    // Ajouter l'état initial à l'automate
    initial_state.state_id = 0;
    parser->states[0] = initial_state;
    parser->num_states = 1;
    
    // Tableau pour stocker les états à traiter
    int states_to_process[MAX_STATES];
    int num_states_to_process = 1;
    states_to_process[0] = 0;
    
    // Tant qu'il y a des états à traiter
    while (num_states_to_process > 0) {
        // Prendre le prochain état à traiter
        int current_state_id = states_to_process[0];
        
        // Décaler les états restants
        for (int i = 0; i < num_states_to_process - 1; i++) {
            states_to_process[i] = states_to_process[i + 1];
        }
        num_states_to_process--;
        
        // Pour chaque symbole possible
        for (int i = 0; i < parser->num_terminals; i++) {
            char symbol = parser->terminals[i];
            
            // Calculer l'état suivant
            LRState next_state = {0};
            slr_goto(parser, &parser->states[current_state_id], symbol, &next_state);
            
            // Si l'état suivant n'est pas vide
            if (next_state.num_items > 0) {
                // Vérifier si cet état existe déjà
                int existing_state_id = slr_state_exists(parser, next_state);
                
                if (existing_state_id == -1) {
                    // Nouvel état
                    next_state.state_id = parser->num_states;
                    parser->states[parser->num_states] = next_state;
                    
                    // Ajouter la transition
                    parser->goto_table[current_state_id][slr_get_symbol_index(parser, symbol)] = parser->num_states;
                    
                    // Ajouter le nouvel état à la liste des états à traiter
                    states_to_process[num_states_to_process++] = parser->num_states;
                    
                    parser->num_states++;
                } else {
                    // État existant
                    parser->goto_table[current_state_id][slr_get_symbol_index(parser, symbol)] = existing_state_id;
                }
            }
        }
        
        // Pour chaque non-terminal
        for (int i = 0; i < parser->num_non_terminals; i++) {
            char symbol = parser->non_terminals[i];
            
            // Calculer l'état suivant
            LRState next_state = {0};
            slr_goto(parser, &parser->states[current_state_id], symbol, &next_state);
            
            // Si l'état suivant n'est pas vide
            if (next_state.num_items > 0) {
                // Vérifier si cet état existe déjà
                int existing_state_id = slr_state_exists(parser, next_state);
                
                if (existing_state_id == -1) {
                    // Nouvel état
                    next_state.state_id = parser->num_states;
                    parser->states[parser->num_states] = next_state;
                    
                    // Ajouter la transition
                    parser->goto_table[current_state_id][slr_get_symbol_index(parser, symbol)] = parser->num_states;
                    
                    // Ajouter le nouvel état à la liste des états à traiter
                    states_to_process[num_states_to_process++] = parser->num_states;
                    
                    parser->num_states++;
                } else {
                    // État existant
                    parser->goto_table[current_state_id][slr_get_symbol_index(parser, symbol)] = existing_state_id;
                }
            }
        }
    }
}

// Fonction auxiliaire pour calculer FIRST d'un symbole
void slr_compute_first_of_symbol(SLRParser *parser, char symbol, char *result, int *count) {
    // Si c'est un terminal, FIRST(a) = {a}
    if (slr_is_terminal(parser, symbol) || symbol == EPSILON_SYMBOL) {
        result[(*count)++] = symbol;
        return;
    }
    
    // Si c'est un non-terminal
    int nt_index = slr_get_non_terminal_index(parser, symbol);
    
    // Si FIRST a déjà été calculé pour ce non-terminal
    if (parser->first_count[nt_index] > 0) {
        for (int i = 0; i < parser->first_count[nt_index]; i++) {
            // Ajouter chaque élément de FIRST(symbol) à result s'il n'y est pas déjà
            char first_symbol = parser->first[nt_index][i];
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
    for (int i = 0; i < parser->num_rules; i++) {
        if (parser->rules[i].left == symbol) {
            const char *right = parser->rules[i].right;
            
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
                    slr_compute_first_of_symbol(parser, Y, first_Y, &first_Y_count);
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
void slr_compute_first(SLRParser *parser) {
    // Initialiser les ensembles FIRST
    for (int i = 0; i < parser->num_non_terminals; i++) {
        parser->first_count[i] = 0;
    }
    
    // Calculer FIRST pour chaque non-terminal
    for (int i = 0; i < parser->num_non_terminals; i++) {
        char nt = parser->non_terminals[i];
        slr_compute_first_of_symbol(parser, nt, parser->first[i], &parser->first_count[i]);
    }
}

// Fonction auxiliaire pour calculer FIRST d'une chaîne de symboles
void slr_compute_first_of_string(SLRParser *parser, const char *string, char *result, int *count) {
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
        
        if (slr_is_terminal(parser, symbol)) {
            // Si c'est un terminal, FIRST contient seulement ce terminal
            result[(*count)++] = symbol;
            return;
        } else {
            // Si c'est un non-terminal, ajouter FIRST(symbol) sauf ε
            int nt_index = slr_get_non_terminal_index(parser, symbol);
            
            for (int j = 0; j < parser->first_count[nt_index]; j++) {
                char first_symbol = parser->first[nt_index][j];
                
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
void slr_compute_follow(SLRParser *parser) {
    // Initialiser les ensembles FOLLOW
    for (int i = 0; i < parser->num_non_terminals; i++) {
        parser->follow_count[i] = 0;
    }
    
    // Règle 1: Ajouter $ à FOLLOW(S) où S est le symbole de départ
    int start_index = slr_get_non_terminal_index(parser, parser->start_symbol);
    parser->follow[start_index][parser->follow_count[start_index]++] = END_MARKER;
    
    // Répéter jusqu'à ce qu'aucun changement ne se produise
    bool changes;
    do {
        changes = false;
        
        // Pour chaque règle A -> αBβ
        for (int i = 0; i < parser->num_rules; i++) {
            char A = parser->rules[i].left;
            const char *right = parser->rules[i].right;
            
            // Pour chaque position dans la partie droite
            for (int j = 0; right[j] != '\0'; j++) {
                char B = right[j];
                
                // Si B est un non-terminal
                if (slr_is_non_terminal(parser, B)) {
                    int B_index = slr_get_non_terminal_index(parser, B);
                    
                    // Extraire β (reste de la chaîne après B)
                    const char *beta = right + j + 1;
                    
                    // Règle 2: Si A -> αBβ, ajouter FIRST(β) - {ε} à FOLLOW(B)
                    char first_beta[MAX_FIRST_FOLLOW];
                    int first_beta_count = 0;
                    slr_compute_first_of_string(parser, beta, first_beta, &first_beta_count);
                    
                    bool beta_derives_epsilon = false;
                    
                    for (int k = 0; k < first_beta_count; k++) {
                        if (first_beta[k] == EPSILON_SYMBOL) {
                            beta_derives_epsilon = true;
                        } else {
                            // Ajouter first_beta[k] à FOLLOW(B) s'il n'y est pas déjà
                            bool already_exists = false;
                            for (int l = 0; l < parser->follow_count[B_index]; l++) {
                                if (parser->follow[B_index][l] == first_beta[k]) {
                                    already_exists = true;
                                    break;
                                }
                            }
                            
                            if (!already_exists) {
                                parser->follow[B_index][parser->follow_count[B_index]++] = first_beta[k];
                                changes = true;
                            }
                        }
                    }
                    
                    // Règle 3: Si A -> αBβ et β peut dériver ε, ou si A -> αB, 
                    // ajouter FOLLOW(A) à FOLLOW(B)
                    if (beta_derives_epsilon || beta[0] == '\0') {
                        int A_index = slr_get_non_terminal_index(parser, A);
                        
                        for (int k = 0; k < parser->follow_count[A_index]; k++) {
                            // Ajouter follow[A_index][k] à FOLLOW(B) s'il n'y est pas déjà
                            bool already_exists = false;
                            for (int l = 0; l < parser->follow_count[B_index]; l++) {
                                if (parser->follow[B_index][l] == parser->follow[A_index][k]) {
                                    already_exists = true;
                                    break;
                                }
                            }
                            
                            if (!already_exists) {
                                parser->follow[B_index][parser->follow_count[B_index]++] = parser->follow[A_index][k];
                                changes = true;
                            }
                        }
                    }
                }
            }
        }
    } while (changes);
}
