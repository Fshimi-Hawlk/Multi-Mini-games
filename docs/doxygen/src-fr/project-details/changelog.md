@page changelog Changer de journal

@ref index "Retour à la maison"

** Dernière vérification par rapport à la structure du projet :** 16 mars 2026

Cette page liste tous les changements notables au Multi Mini-Games monorepo.

Le format suit [Garder un changementlog](https://keepachangelog.com/fr/1.1.0/) conventions.
Nous utilisons la version sémantique là où elle a du sens, ou la version du calendrier pour les premiers stades de développement.

### Comment nous maintenons ce changementlog

- Seuls des changements significatifs sont enregistrés ici (nouvelle intégration de jeu, code partagé majeur, grands refacteurs, mises à jour importantes de documentation, etc.)
- Tout va sous **[Unreleased]** jusqu'à ce que l'équipe décide qu'un tag de version est prêt (exemple: 0.10.0 quand lobby + plusieurs jeux sont jouables ensemble)

### [Non publié]

#### Ajouté
- Fondation Monorepo : dossiers partagés (`première partie/`, `tiers/`, `actifs/`)
- Fichiers de documentation de base dans root: README.md, CONTRIBUTING.md, TODO.md, CodeStyleAndConventions.md
- Dossier modèle `sous-projet-exemple/` pour guider la nouvelle structure de jeu
- Bases légales : fichiers LICENSE et AVIS (crédits tiers)
- Root Makefile avec des compilations statiques paresseuses, copie de l'en-tête de l'API vers `firstparty/API/`, et liaison exécutable de lobby incrémental
- Système de configuration central : `gameConfig.h` définissant `GameConfig St`, `AudioConfig St`, `VideoConfig St` + macros par défaut
- Premier jeu complet API: `solitaireAPI.h` (modèle mini-jeu standard, implémenté par Maxime CHAUVEAU)
- En-tête initial de l'API Tetris: `tetrisAPI.h` (arrêt pour la mise en œuvre prochaine)

#### Changement
- Amélioration du README racine : explication du projet plus claire + commandes Git conviviales pour les débutants
- Simplifier et adoucir CONTRIBUTING.md: étapes de travail plus faciles, exemples pratiques Git, ton moins strict sur le style
- Ce changement lui-même : ajout de règles de mise à jour plus claires et reflet d'améliorations récentes du doc
- Rendre le libellé de TODO.md plus direct tout en préservant la liste complète des tâches
- Mise à jour du codeStyleAndConventions.md: maintenant présenté comme un style recommandé (non obligatoire), avec invitation à discuter de règles incommodes
- Gestion étendue des erreurs : nouveaux codes `ERROR INVALID CONFIG`, `ERROR AUDIO LOAD`, `ERROR WINDOW INIT`, `ERROR ASSET LOAD` ajoutés à `generalAPI.h`
- Modèle d'API mini-jeu normalisé : utilisation cohérente de `GameConfig St*`, ajout de `isRunning()` helper, nommage unifié entre les jeux

#### Correction
- (pas encore enregistré)

#### Supprimé
- (pas encore enregistré)

**Créé :** 15 janvier 2025
**Dernière mise à jour :** 16 mars 2026
**Auteur:** [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)

@ref index "Retour à la maison"