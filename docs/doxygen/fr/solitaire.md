/**
    @page solitaire_fr Solitaire (Klondike)

    @tableofcontents

    @section sol_overview Vue d'ensemble

    **Solitaire** est une implémentation complète du jeu de cartes Klondike.

    @subsection sol_language Langue
    - @ref solitaire_fr "Français"
    - @ref solitaire "English"

    @subsection sol_rules Règles du jeu

    - <b>Objectif</b>: Déplacer toutes les cartes vers les 4 fondations (As → Roi), triées par couleur
    - <b>Tableau</b>: Empiler les cartes par ordre décroissant en alternant les couleurs
    - <b>Pioche</b>: Tirer des cartes de la pioche vers la défausse

    @subsection sol_controls Contrôles

    - <b>Clic gauche</b>: Déplacer les cartes entre les piles
    - <b>Cliquer sur la pioche</b>: Tirer une carte (ou recycler la défausse quand vide)
    - <b>Touche N</b>: Nouvelle partie

    @subsection sol_features Fonctionnalités

    - Déplacement par glisser-déposer de plusieurs cartes
    - Système de score (+10 fondation, +5 tableau)
    - Minuteur de partie
    - Détection de victoire avec écran de félicitations
    - Détection de défaite toutes les 2 minutes

    @section sol_arch Architecture

    @subsection sol_core Logique du jeu (src/core/game.c)

    - @c solitaire_init(): Initialiser l'état du jeu
    - @c solitaire_dealCards(): Mélanger et distribuer les cartes
    - @c solitaire_update(): Mise à jour de la boucle de jeu
    - @c solitaire_checkWin(): Vérifier la condition de victoire
    - @c solitaire_checkLose(): Vérifier la condition de défaite

    @subsection sol_rendering Rendu graphique (src/ui/renderer.c)

    - @c RenderGame(): Fonction principale de rendu
    - @c RenderStock(), RenderWaste(), RenderFoundation(), RenderTableauPile()
    - @c LoadAssets(): Charger les textures des cartes

    @subsection sol_structures Structures de données

    - @c Card_St: Carte individuelle (couleur, valeur, face visible/cachée)
    - @c Pile_St: Pile de cartes (tableau, nombre, position)
    - @c SolitaireGameState: État complet du jeu
    - @c DragState_St: État de l'opération de déplacement

    @section sol_author Auteur

    Maxime CHAUVEAU - Mars 2026
*/
