import os

def fix_king_client():
    with open("king-for-four/src/core/king_client_module.c", "r") as f:
        content = f.read()
    
    content = content.replace(
        "if (local_state.discard_pile.head == NULL) {\n                push_card(&local_state.discard_pile, sync.top_card);\n            } else {\n                local_state.discard_pile.head->card = sync.top_card;\n            }",
        "if (local_state.discard_pile.size == 0) {\n                push_card(&local_state.discard_pile, sync.top_card);\n            } else {\n                local_state.discard_pile.cards[local_state.discard_pile.size - 1] = sync.top_card;\n            }"
    )

    content = content.replace(
        "Node* curr = local_state.players[0].hand.head;\n                for (int i = 0; i < clickedHandIndex; i++) curr = curr->next;\n                \n                if (curr->card.color == CARD_BLACK) {",
        "if (local_state.players[0].hand.cards[clickedHandIndex].color == CARD_BLACK) {"
    )

    with open("king-for-four/src/core/king_client_module.c", "w") as f:
        f.write(content)

def fix_king_module():
    with open("king-for-four/src/core/king_module.c", "r") as f:
        content = f.read()

    content = content.replace(
        "if (g->discard_pile.head != NULL) {\n        top_card = g->discard_pile.head->card;\n    }",
        "if (g->discard_pile.size > 0) {\n        top_card = g->discard_pile.cards[g->discard_pile.size - 1];\n    }"
    )
    content = content.replace(
        "Node* curr = g->players[i].hand.head;\n        for (int j = 0; j < hand_count; j++) {\n            cards[j] = curr->card;\n            curr = curr->next;\n        }",
        "for (int j = 0; j < hand_count; j++) {\n            cards[j] = g->players[i].hand.cards[j];\n        }"
    )
    content = content.replace(
        "Node* curr = g->players[internal_id].hand.head;\n            for(int i=0; i < p.card_index && curr; i++) curr = curr->next;\n            \n            if (curr && try_play_card(g, internal_id, p.card_index)) {\n                Card played = g->discard_pile.head->card;",
        "if (p.card_index >= 0 && p.card_index < g->players[internal_id].hand.size && try_play_card(g, internal_id, p.card_index)) {\n                Card played = g->discard_pile.cards[g->discard_pile.size - 1];"
    )
    content = content.replace(
        "Node* curr = g->players[cp].hand.head;\n                for(int i=0; i<card_idx; i++) curr = curr->next;\n                \n                Card toPlay = curr->card;\n                if (try_play_card(g, cp, card_idx)) {",
        "if (card_idx < g->players[cp].hand.size) {\n                Card toPlay = g->players[cp].hand.cards[card_idx];\n                if (try_play_card(g, cp, card_idx)) {"
    )
    with open("king-for-four/src/core/king_module.c", "w") as f:
        f.write(content)

def fix_renderer():
    with open("king-for-four/src/ui/renderer.c", "r") as f:
        content = f.read()

    content = content.replace("g->discard_pile.head != NULL", "g->discard_pile.size > 0")
    content = content.replace("g->discard_pile.head->card", "g->discard_pile.cards[g->discard_pile.size - 1]")
    
    content = content.replace("p->hand.head == NULL", "p->hand.size == 0")
    
    content = content.replace(
        "Node* current = p->hand.head; \n    int i = 0;\n\n    while (current != NULL) {\n        Rectangle dest = { (float)startX + (i * padding), (float)startY, cardW, cardH };\n\n        // Animation de survol\n        if (i == hoveredIndex) {\n            dest.y -= 40;\n            // Hover glow\n            DrawRectangleRounded((Rectangle){dest.x - 5, dest.y - 5, dest.width + 10, dest.height + 10}, 0.1f, 10, Fade(GOLD, 0.5f));\n        }\n\n        // Card shadow\n        DrawRectangleRounded((Rectangle){dest.x + 3, dest.y + 3, dest.width, dest.height}, 0.1f, 10, Fade(BLACK, 0.3f));\n\n        Rectangle source = GetCardSourceRec(current->card, assets.cardSheet);\n        DrawTexturePro(assets.cardSheet, source, dest, (Vector2){0,0}, 0.0f, WHITE);\n\n        // Help text\n        if (current->card.value >= SKIP && i == hoveredIndex) {\n            const char* text = \"\";\n            switch(current->card.value) {",
        "for (int i = 0; i < p->hand.size; i++) {\n        Card current = p->hand.cards[i];\n        Rectangle dest = { (float)startX + (i * padding), (float)startY, cardW, cardH };\n\n        // Animation de survol\n        if (i == hoveredIndex) {\n            dest.y -= 40;\n            // Hover glow\n            DrawRectangleRounded((Rectangle){dest.x - 5, dest.y - 5, dest.width + 10, dest.height + 10}, 0.1f, 10, Fade(GOLD, 0.5f));\n        }\n\n        // Card shadow\n        DrawRectangleRounded((Rectangle){dest.x + 3, dest.y + 3, dest.width, dest.height}, 0.1f, 10, Fade(BLACK, 0.3f));\n\n        Rectangle source = GetCardSourceRec(current, assets.cardSheet);\n        DrawTexturePro(assets.cardSheet, source, dest, (Vector2){0,0}, 0.0f, WHITE);\n\n        // Help text\n        if (current.value >= SKIP && i == hoveredIndex) {\n            const char* text = \"\";\n            switch(current.value) {"
    )

    content = content.replace("current = current->next;\n        i++;", "")

    content = content.replace(
        "Node* current = p->hand.head;\n    int hoveredIndex = -1;\n    for (int i = 0; current; i++, current = current->next) {\n        Rectangle hitBox = { (float)startX + (i * padding), (float)startY - (hoveredIndex == i ? 40 : 0), (float)padding, (float)cardH };\n        // Last card has full width hitBox\n        if (current->next == NULL) hitBox.width = cardW;",
        "int hoveredIndex = -1;\n    for (int i = 0; i < p->hand.size; i++) {\n        Rectangle hitBox = { (float)startX + (i * padding), (float)startY - (hoveredIndex == i ? 40 : 0), (float)padding, (float)cardH };\n        // Last card has full width hitBox\n        if (i == p->hand.size - 1) hitBox.width = cardW;"
    )

    with open("king-for-four/src/ui/renderer.c", "w") as f:
        f.write(content)

fix_king_client()
fix_king_module()
fix_renderer()
