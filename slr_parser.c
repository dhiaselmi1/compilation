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
