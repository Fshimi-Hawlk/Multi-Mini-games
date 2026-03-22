@page roadmap Feuille de route du projet

@ref index "Back to Home"

** Dernière vérification par rapport à la structure du projet :** 16 mars 2026

Cette page décrit où nous en sommes en ce moment et la voie difficile sur laquelle nous nous sommes mis d'accord pour garder les choses gérables.

## Situation actuelle (mars 2026)

- Oui`main`-> squelette précoce + quelques docs, mais complètement dépassé maintenant. On a cessé de le toucher il y a des mois.
- Directions du développement actif :
- Branches de jeu individuelles (par exemple.`tetris`,`snake-classic`,`block-blast`,`memory`, etc.) -> où les jeux mono-joueur sont construits et polis autonome
- Oui`docs-setup`-> documentation partagée, règles de style de code, guides de configuration, petits refacteurs qui touchent plusieurs parties
- Oui`sub-games-integration-test`(on appelle ça`sgit`) -> la branche d'intégration principale actuelle. Nous fusionnons des jeux en un seul joueur pour les brancher dans le hall, les tester ensemble, corriger les bugs du hall, ajouter des fonctionnalités partagées

En ce moment: plusieurs jeux fonctionnent autonome, un couple sont déjà jouables dans le hall via`sgit`. Le lobby lui-même a des mouvements basiques, des déclencheurs de collision et de l'interface utilisateur, mais il est encore rugueux autour des bords.

## Structure à long terme prévue

Pour éviter les grands conflits lorsque nous commençons à ajouter des multijoueurs (code réseau, problèmes de synchronisation, logique de jeu différente), nous avons convenu de cette approche progressive:

1. ** Phase monojoueur** (en cours / quelques semaines-mois)
- Finir et polir les versions monojoueur sur leurs propres branches
- Intégrez-les un par un dans le hall`sgit`
- Une fois que nous avons 4-5+ jeux bien travailler ensemble -> peut-être renommer`sgit`à quelque chose de plus clair comme`singleplayer-games-integration-test`

2. **La phase multijoueur** (prochaine étape majeure après un seul joueur se sent solide)
- Créer une nouvelle branche :`multiplayer-games-integration-test`(courte :`mgit`)
- Prototyper des versions multijoueurs (réseaux, synchronisation en temps réel ou en temps réel, différences d'interface utilisateur)
- Crocheter les versions MP dans le hall séparément -> garder le code à un seul joueur intact

3. **Combine la phase** (lorsque SP et MP sont prêts)
- Créer`combine-games-integration-test`(courte :`cgit`)
- Fusionner des branches monojoueur et multijoueur
- Résoudre les conflits (modes locaux vs en réseau, toggles d'interface utilisateur, problèmes de performance, etc.)
- Testez le hall complet avec un mélange de jeux SP + MP

4. **Stable libération au principal**
- Quand`cgit`est jouable et raisonnablement stable -> tout fusionner à`main`
- Étiqueter une version (par exemple v0.1-simplejoueur ou v1.0 si ambitieux)
- Peut-être créer la version GitHub avec builds, screenshots, brève description

C'est notre plan pour garder les branches testables et réduire l'enfer de fusion.
Dans la pratique:
- Si quelque chose est petit et sans conflit, nous pourrions sauter les étapes et fusionner plus tôt
- Si le multijoueur prend plus de temps que prévu, nous pouvons expédier une version`main`premier
- Tout est flexible — nous discuterons en groupe et nous ajusterons chaque fois que nécessaire

## Liste rapide de tâches / idées

- Obtenez au moins 4-5 jeux monojoueurs entièrement intégrés et jouables dans`sgit`
- Améliore le lobby : sélection plus agréable de jeux (signaux ou portes au lieu de simples boîtes à frapper), affichage des scores de base, pause/résume, persistance de la peau
- Démarrer le prototypage multijoueur : choisissez 1–2 jeux simples (par exemple en mode tour-basé comme mémoire ou block-blast) et expérimentez le réseautage sur une branche de fonctionnalités
- Améliorations partagées : meilleure gestion audio, configuration d'entrée unifiée, meilleure enregistrement/récupération des erreurs
- Documentation : ajouter des captures d'écran ou des GIF du lobby + jeux en cours, terminer les guides de configuration

Si cela semble intéressant, n'hésitez pas à sauter! Ouvrir les questions, commencer les projets de relations publiques, poser des questions dans le chat. On s'attend à des erreurs, on apprend tous.

**Créé :** 16 mars 2026
**Dernière mise à jour :** 16 mars 2026
**Auteur:** [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)

@ref index "Back to Home"