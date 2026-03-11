/**
    @page solitaire Solitaire (Klondike)

    @tableofcontents

    @section sol_overview Overview

    **Solitaire** is a fully-featured Klondike Solitaire card game implementation.

    @subsection sol_language Language
    - @ref solitaire_fr "Français"
    - @ref solitaire "English"

    @subsection sol_rules Game Rules

    - <b>Objective</b>: Move all cards to the four foundation piles, sorted by suit from Ace to King
    - <b>Tableau</b>: Build down in alternating colors (red on black, black on red)
    - <b>Stock</b>: Draw cards from the stock pile to the waste pile

    @subsection sol_controls Controls

    - <b>Left Click</b>: Drag cards between piles
    - <b>Click Stock</b>: Draw a card (or recycle waste pile when empty)
    - <b>N Key</b>: Start a new game

    @subsection sol_features Features

    - Drag and drop multiple cards at once
    - Score tracking (+10 for foundation moves, +5 for tableau moves)
    - Game timer
    - Win detection with congratulations screen
    - Lose detection every 2 minutes

    @section sol_arch Architecture

    @subsection sol_core Core Game Logic (src/core/game.c)

    - @c solitaire_init(): Initialize game state
    - @c solitaire_dealCards(): Shuffle and deal cards
    - @c solitaire_update(): Game loop update (input, drag & drop)
    - @c solitaire_checkWin(): Check victory condition
    - @c solitaire_checkLose(): Check defeat condition

    @subsection sol_rendering Rendering (src/ui/renderer.c)

    - @c RenderGame(): Main render function
    - @c RenderStock(), RenderWaste(), RenderFoundation(), RenderTableauPile()
    - @c LoadAssets(): Load card textures

    @subsection sol_structures Data Structures

    - @c Card_St: Individual card (suit, rank, face-up/down)
    - @c Pile_St: Card pile (array of cards, count, position)
    - @c SolitaireGameState: Complete game state
    - @c DragState_St: Current drag operation state

    @section sol_author Author

    Maxime CHAUVEAU - March 2026
*/
