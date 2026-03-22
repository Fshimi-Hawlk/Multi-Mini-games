@page changelog Changer de journal

@ref index "Back to Home"

** Dernière vérification par rapport à la structure du projet :** 16 mars 2026

Cette page liste tous les changements notables au Multi Mini-Games monorepo.

Le format suit les conventions [Gardez un changelog](https://keepachangelog.com/en/1.1.0/).
Nous utilisons la version sémantique là où elle a du sens, ou la version du calendrier pour les premiers stades de développement.

### Comment nous maintenons ce changementlog

- Seuls des changements significatifs sont enregistrés ici (nouvelle intégration de jeu, code partagé majeur, grands refacteurs, mises à jour importantes de documentation, etc.)
- Tout va sous **[Unreleased]** jusqu'à ce que l'équipe décide qu'un tag de version est prêt (exemple: 0.10.0 quand lobby + plusieurs jeux sont jouables ensemble)

### [Non publié]

#### Ajouté
- Fondation Monorepo : dossiers partagés (`firstparty/`,`thirdparty/`,`assets/`)
- Fichiers de documentation de base dans root: README.md, CONTRIBUTING.md, TODO.md, CodeStyleAndConventions.md
- Dossier modèle`sub-project-example/`pour guider la nouvelle structure du jeu
- Bases légales : fichiers LICENSE et AVIS (crédits tiers)
- Root Makefile avec des compilations statiques paresseuses, copie de l'en-tête de l'API`firstparty/APIs/`, et liaison exécutable de lobby progressif
- Système central de configuration:`gameConfig.h`définition`GameConfig_St`,`AudioConfig_St`,`VideoConfig_St`+ macros par défaut
- Premier jeu complet API:`solitaireAPI.h`(modèle mini-jeu standard, mis en œuvre par Maxime CHAUVEAU)
- En-tête initial de l'API Tetris :`tetrisAPI.h`(pour la mise en œuvre prochaine)

#### Changement
- Amélioration du README racine : explication du projet plus claire + commandes Git conviviales pour les débutants
- Simplifier et adoucir CONTRIBUTING.md: étapes de travail plus faciles, exemples pratiques Git, ton moins strict sur le style
- Ce changement lui-même : ajout de règles de mise à jour plus claires et reflet d'améliorations récentes du doc
- Rendre le libellé de TODO.md plus direct tout en préservant la liste complète des tâches
- Mise à jour du codeStyleAndConventions.md: maintenant présenté comme un style recommandé (non obligatoire), avec invitation à discuter de règles incommodes
- Gestion étendue des erreurs : nouveaux codes`ERROR_INVALID_CONFIG`,`ERROR_AUDIO_LOAD`,`ERROR_WINDOW_INIT`,`ERROR_ASSET_LOAD`ajouté à`generalAPI.h`
- Modèle standard de mini-jeu API: utilisation cohérente de`GameConfig_St*`, ajouté`isRunning()`helper, nom unifié à travers les jeux

#### Correction
- (pas encore enregistré)

#### Supprimé
- (pas encore enregistré)

**Créé :** 15 janvier 2025
**Dernière mise à jour :** 16 mars 2026
**Auteur:** [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)

@ref index "Back to Home"