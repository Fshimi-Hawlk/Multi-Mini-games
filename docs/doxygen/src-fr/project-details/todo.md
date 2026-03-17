@page todo TODO

@ref index "Retour à la maison"

** Dernière vérification par rapport à la structure du projet :** 16 mars 2026

Cette page énumère les principales tâches ouvertes pour l'ensemble du monorepo.
La plupart d'entre eux ont une incidence sur le lobby, le code partagé ou la collection globale de mini-jeux.

Les tâches sont approximativement triées de "faire cela bientôt" à "serait cool plus tard".
Nous mettons à jour ce fichier au fur et à mesure - n'hésitez pas à ajouter des éléments ou à modifier les priorités après la discussion en groupe.

## Priorité élevée - faites-les bientôt

- [x] Créer un Makefile global au niveau racine
- C'est fait. Il gère maintenant les compilations paresseuses de bibliothèque, copie les en-têtes d'API à la première partie/API/, prend en charge les compilations incrémentales, force-reconstructions, modes de débogage, désinfectants, etc.

- [ ] Terminer correctement la configuration du dossier root `docs/`
-> Déjà créé le dossier et déplacé plusieurs guides là (`API Conversion.md`, `CodeStyleAndConventions.md`, `makefile.md`, etc.) pour garder racine propre.
- Doxyfile principal est dans "docs/" et documents lobby + code partagé
- Suivant: étendre pour inclure la documentation pour chaque module de jeu une fois de plus sont intégrés

## Après le premier jeu est complètement fusionné en main

- [x] Écrire des règles claires pour transformer un jeu autonome en API compatible avec le lobby
- C'est fait. Voir @ref api conversion "API Conversion & Intégration"

- [ ] Décider du système d'enregistrement / progression / paramètres
-> Options jusqu'ici : fichiers texte simples dans `assets/<game-name>/saves/`, simple JSON, ou quelque chose de personnalisé?
-> Plan: ajouter des helpers de chargement/sauvetage partagés dans `firstpart/` afin que chaque jeu puisse utiliser le même code

- [ ] Définir le format local du classement
-> Gardez-le simple pour l'instant (fichiers locaux seulement, pas en ligne).
-> Peut-être JSON ou binaire dans `actifs/leaderboards/`?

## Joli à avoir / à plus long terme

- [x] Ajouter une feuille de route visible
-> C'est fait. Voir le diagramme de Gantt sur les projets GitHub (https://github.com/users/Fshimi-Hawlk/projects/1)
-> En outre, se référer à la feuille de route @ref "Plan de route"

- [ ] Ajouter les paramètres audio de base (sliders de volume, toggles muets) dans le lobby
- [ ] Ajouter des déverrouillages / cosmétiques persistants à travers les jeux
- [ ] Expérimentez avec un multijoueur local très basique (écran ou siège chauffant)
- [ ] Ajouter une récupération d'erreur simple dans le lobby (recharger le jeu sur le crash au lieu de la sortie)

Les grands changements devraient être discutés dans le groupe chat d'abord afin que tout le monde est d'accord.

**Créé:** 2 mars 2025
**Dernière mise à jour :** 16 mars 2026
**Auteur:** [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)

@ref index "Retour à la maison"