// d. Algorithme pour construire la table SLR
void slr_construct_slr_table(SLRParser *parser) {
    // Ajouter le marqueur de fin aux terminaux s'il n'y est pas déjà
    bool end_marker_exists = false;
    for (int i = 0; i < parser->num_terminals; i++) {
        if (parser->terminals[i] == END_MARKER) {
            end_marker_exists = true;
            break;
        }
    }
    
    if (!end_marker_exists) {
        slr_add_terminal(parser, END_MARKER);
    }
    
    // Pour chaque état de l'automate
    for (int i = 0; i < parser->num_states; i++) {
        LRState state = parser->states[i];
        
        // Pour chaque item dans l'état
        for (int j = 0; j < state.num_items; j++) {
            LRItem item = state.items[j];
            SLRRule rule = parser->rules[item.rule_index];
            
            // Si le point est à la fin de la règle [A -> α.]
            if (item.dot_position == strlen(rule.right)) {
                // Si c'est la règle augmentée [S' -> S.]
                if (rule.left == parser->start_symbol && rule.right[0] == parser->non_terminals[0]) {
                    // Action ACCEPT pour le marqueur de fin
                    int end_marker_index = slr_get_terminal_index(parser, END_MARKER);
                    parser->action_table[i][end_marker_index].type = ACTION_ACCEPT;
                } else {
                    // Pour chaque terminal a dans FOLLOW(A)
                    int A_index = slr_get_non_terminal_index(parser, rule.left);
                    
                    for (int k = 0; k < parser->follow_count[A_index]; k++) {
                        char a = parser->follow[A_index][k];
                        
                        if (a == END_MARKER) {
                            a = '#'; // Utiliser le caractère # pour le marqueur de fin
                        }
                        
                        int a_index = slr_get_terminal_index(parser, a);
                        
                        // Action REDUCE A -> α
                        if (parser->action_table[i][a_index].type != ACTION_ERROR) {
                            printf("Conflit Reduce/Reduce ou Shift/Reduce dans l'état %d pour le terminal %c\n", i, a);
                        }
                        
                        parser->action_table[i][a_index].type = ACTION_REDUCE;
                        parser->action_table[i][a_index].value = item.rule_index;
                    }
                }
            } else {
                // Si le point est devant un terminal [A -> α.aβ]
                char next_symbol = rule.right[item.dot_position];
                
                if (slr_is_terminal(parser, next_symbol)) {
                    int next_symbol_index = slr_get_terminal_index(parser, next_symbol);
                    int next_state = parser->goto_table[i][slr_get_symbol_index(parser, next_symbol)];
                    
                    if (next_state != -1) {
                        // Action SHIFT
                        if (parser->action_table[i][next_symbol_index].type != ACTION_ERROR) {
                            printf("Conflit Shift/Reduce dans l'état %d pour le terminal %c\n", i, next_symbol);
                        }
                        
                        parser->action_table[i][next_symbol_index].type = ACTION_SHIFT;
                        parser->action_table[i][next_symbol_index].value = next_state;
                    }
                }
            }
        }
        
        // Pour chaque non-terminal A
        for (int j = 0; j < parser->num_non_terminals; j++) {
            char A = parser->non_terminals[j];
            int A_index = slr_get_symbol_index(parser, A);
            
            // GOTO[i, A] = j
            int next_state = parser->goto_table[i][A_index];
            
            if (next_state != -1) {
                parser->goto_table[i][A_index] = next_state;
            }
        }
    }
}

// e. Algorithme pour exploiter la table SLR
bool slr_parse_input(SLRParser *parser, const char *input) {
    // Pile pour l'analyse
    int state_stack[1000];
    int top = 0;
    
    // Initialiser la pile avec l'état 0
    state_stack[top] = 0;
    
    // Position dans la chaîne d'entrée
    int pos = 0;
    
    printf("Analyse syntaxique SLR:\n");
    printf("Pile\tEntrée\tAction\n");
    
    while (1) {
        // Afficher l'état actuel
        printf("Pile: ");
        for (int i = 0; i <= top; i++) {
            printf("%d ", state_stack[i]);
        }
        
        // Symbole courant dans l'entrée
        char a = input[pos];
        
        if (a == '\0') {
            a = END_MARKER; // Fin de l'entrée
        }
        
        printf("\tEntrée: %s\tAction: ", input + pos);
        
        // Obtenir l'action pour l'état courant et le symbole courant
        int current_state = state_stack[top];
        int a_index = slr_get_terminal_index(parser, a);
        
        if (a_index == -1) {
            printf("Erreur: Symbole d'entrée %c non reconnu\n", a);
            return false;
        }
        
        SLRAction action = parser->action_table[current_state][a_index];
        
        switch (action.type) {
            case ACTION_SHIFT:
                printf("Shift %d\n", action.value);
                
                // Empiler le nouvel état
                state_stack[++top] = action.value;
                
                // Avancer dans l'entrée
                pos++;
                break;
                
            case ACTION_REDUCE:
                {
                    SLRRule rule = parser->rules[action.value];
                    printf("Reduce %c -> %s\n", rule.left, rule.right);
                    
                    // Dépiler |β| états
                    int rule_length = strlen(rule.right);
                    if (rule.right[0] != EPSILON_SYMBOL) {
                        top -= rule_length;
                    }
                    
                    // Obtenir l'état s = pile[top]
                    int s = state_stack[top];
                    
                    // Empiler GOTO[s, A]
                    int A_index = slr_get_symbol_index(parser, rule.left);
                    int goto_state = parser->goto_table[s][A_index];
                    
                    if (goto_state == -1) {
                        printf("Erreur: Pas de transition GOTO[%d, %c]\n", s, rule.left);
                        return false;
                    }
                    
                    state_stack[++top] = goto_state;
                }
                break;
                
            case ACTION_ACCEPT:
                printf("Accept\n");
                return true;
                
            case ACTION_ERROR:
                printf("Erreur: Pas d'action définie pour l'état %d et le symbole %c\n", current_state, a);
                return false;
        }
    }
    
    return false;
}

// Fonction pour afficher un item LR(0)
void slr_print_item(SLRParser *parser, LRItem item) {
    SLRRule rule = parser->rules[item.rule_index];
    
    printf("%c -> ", rule.left);
    
    for (int i = 0; i < strlen(rule.right); i++) {
        if (i == item.dot_position) {
            printf(". ");
        }
        printf("%c ", rule.right[i]);
    }
    
    if (item.dot_position == strlen(rule.right)) {
        printf(". ");
    }
}

// Fonction pour afficher un état
void slr_print_state(SLRParser *parser, LRState state) {
    printf("État %d:\n", state.state_id);
    
    for (int i = 0; i < state.num_items; i++) {
        printf("  ");
        slr_print_item(parser, state.items[i]);
        printf("\n");
    }
}

// Fonction pour afficher l'automate
void slr_print_automaton(SLRParser *parser) {
    printf("Automate LR(0):\n");
    
    for (int i = 0; i < parser->num_states; i++) {
        slr_print_state(parser, parser->states[i]);
        
        // Afficher les transitions
        for (int j = 0; j < parser->num_terminals + parser->num_non_terminals; j++) {
            int next_state = parser->goto_table[i][j];
            
            if (next_state != -1) {
                char symbol;
                
                if (j < parser->num_terminals) {
                    symbol = parser->terminals[j];
                } else {
                    symbol = parser->non_terminals[j - parser->num_terminals];
                }
                
                printf("  GOTO(%d, %c) = %d\n", i, symbol, next_state);
            }
        }
        
        printf("\n");
    }
}

// Fonction pour afficher la table SLR
void slr_print_slr_table(SLRParser *parser) {
    printf("Table SLR:\n");
    
    // En-tête
    printf("État\t");
    
    // Terminaux (ACTION)
    for (int i = 0; i < parser->num_terminals; i++) {
        printf("%c\t", parser->terminals[i]);
    }
    
    // Non-terminaux (GOTO)
    for (int i = 0; i < parser->num_non_terminals; i++) {
        printf("%c\t", parser->non_terminals[i]);
    }
    
    printf("\n");
    
    // Pour chaque état
    for (int i = 0; i < parser->num_states; i++) {
        printf("%d\t", i);
        
        // ACTION pour chaque terminal
        for (int j = 0; j < parser->num_terminals; j++) {
            SLRAction action = parser->action_table[i][j];
            
            switch (action.type) {
                case ACTION_SHIFT:
                    printf("s%d\t", action.value);
                    break;
                    
                case ACTION_REDUCE:
                    printf("r%d\t", action.value);
                    break;
                    
                case ACTION_ACCEPT:
                    printf("acc\t");
                    break;
                    
                case ACTION_ERROR:
                    printf("-\t");
                    break;
            }
        }
        
        // GOTO pour chaque non-terminal
        for (int j = 0; j < parser->num_non_terminals; j++) {
            int goto_state = parser->goto_table[i][parser->num_terminals + j];
            
            if (goto_state != -1) {
                printf("%d\t", goto_state);
            } else {
                printf("-\t");
            }
        }
        
        printf("\n");
    }
}
