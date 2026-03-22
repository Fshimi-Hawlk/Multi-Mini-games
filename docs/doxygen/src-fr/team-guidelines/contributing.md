@page contributing Lignes directrices sur les contributions

@ref index "Back to Home"

** Dernière vérification par rapport à la structure du projet :** 16 mars 2026

Nous sommes une petite équipe de quatre étudiants apprenant la collaboration Git au fur et à mesure.
Cette page décrit notre workflow actuel pour les commits, les branches, les requêtes de tirage et les avis (en date de mars 2026).
Même si le projet est encore WIP, nous avons déjà défini des règles de base pour garder les choses organisées et traçables.

Le principe le plus important reste le même : communiquer ouvertement dans le chat de groupe quand quelque chose n'est pas clair.

## Branches actives actuelles et aperçu du déroulement du travail

`main`est actuellement interrompu et la plupart du temps dépassé.
Le développement actif se produit sur ces branches :

- Branches de jeu individuelles (par exemple.`tetris`,`snake-classic`,`block-blast`, etc.) -> développement de jeu monojoueur et poli
- Oui`docs-setup`-> documentation, style de code, conventions, guides de configuration
- Oui`sub-games-integration-test`(on l'appelle habituellement`sgit`) -> améliorations de lobby, intégration de jeu, corrections de bugs partagées

Flux typique pour un membre de l'équipe:
1. Tirez le dernier de la branche sur laquelle vous travaillez
2. Travaillez sur votre fonction / jeu / réparation
3. S ' engager avec des messages clairs (voir ci-dessous)
4. Poussez votre branche
5. Ouvrir une demande de tirage (généralement`sgit`pour les intégrations ou la branche partagée concernée)
6. Demander des avis dans le chat de groupe
7. Fix feedback -> une fois approuvé et testé dans le hall -> fusion

Une fois les jeux sont solides en un seul joueur sur`sgit`, nous fusionnerons plus tard tout en`main`(y compris les futures parties multijoueurs).

Pour les étapes prévues, état actuel et vision à long terme, voir @ref roadmap "Roadmap"

## Envoyer des messages

Nous suivons un style simplifié **Commits conventionnels**.
Il aide tout le monde à comprendre les changements rapidement lors des évaluations et dans git log.

Préfixes communs que nous utilisons le plus souvent:

- Oui`feat:`nouvelle fonctionnalité / changement visible
- Oui`fix:`correction du bug
- Oui`docs:`documentation, commentaires, guides
- Oui`style:`formatage, espace blanc, nommage (pas de changement logique)
- Oui`refactor:`nettoyage du code / meilleure structure (aucun changement de comportement)
- Oui`test:`essais ajoutés ou fixés
- Oui`chore:`entretien, outillage, .gitignore, petits nettoyages
- Oui`build:`Makefile, construire des drapeaux, des options de compilateur

Nous utilisons aussi parfois:
- Oui`fetch`/`merge`-> apporter des changements d'une autre branche
- Oui`undo`-> supprimer clairement quelque chose ajouté plus tôt

Exemples de nos récents commits:
- Oui`feat: added hold piece mechanic in Tetris`
- Oui`fix: resolved player getting stuck on lobby platforms`
- Oui`docs: updated API conversion guide with new error codes`
- Oui`refactor: moved player globals into BallSystem_St`
- Oui`undo: removed temporary pause menu draft`

Pour de très petits changements (typo, espace de traînage) nous faisons soit une petite`style:`/`chore:`commit ou le replie dans le prochain commit réel — les deux sont bien.

Facultatif mais recommandé : ajouter un résumé en une seule ligne** en haut des messages de commit plus longs.
Il fait`git log --oneline`et GitHub commit listes beaucoup plus facile à scanner.

Exemple de bon message de commit récent:

```text
Docs update / less globals / temporary pause menu removal / fixes / new font
- docs: Updated creation date and last updated dates
- chore: Removed unused windowRect and appFont from globals
- refactor: Moved game globals to BallSystem_St fields
- undo: Removed pause menu draft - will reimplement later in lobby context
- fix: Fixed font size warning display
- fix: Can now place daub immediately when ball text appears
- chore: Switched font to NotoSansMono-Bold.ttf
```

Nous ne sommes pas stricts sur les préfixes — la clarté et l'histoire utile comptent plus que la catégorisation parfaite.
Si vous n'êtes pas sûr, choisissez le plus proche ou demandez en chat.

## Nom de la succursale pour les demandes de tirage

Nous utilisons ce modèle pour les branches PR:

`<target-branch>-PR<day-sequence-number>-DD/MM`

Exemples:
- Oui`tetris-PR1-16/03`
- Oui`sgit-PR3-16/03`
- Oui`docs-setup-PR2-17/03`

Cela rend évident ce que la branche cible, quand elle a été créée, et son ordre ce jour-là.

## Demandes de tirage

Les PR sont notre principale façon d'examiner, de discuter et d'intégrer les changements en toute sécurité.

### Avant d'ouvrir un PR
- Essais:`make MODE=clang-debug run-tests`(ou au moins`make tests`)
- Assurez-vous qu'il se construit sans avertissements/erreurs

### Création de la branche PR (deux cas courants)

**Case 1: Aide sur une branche de jeu/caractère** (p. ex. amélioration`tetris`)

```bash
git fetch origin
git checkout tetris
git pull origin tetris
git checkout -b tetris-PR1-16/03
# work -> commit -> ...
git push origin tetris-PR1-16/03
```

**Case 2: Intégration dans une branche partagée** (p. ex.`sgit`,`docs-setup`)

```bash
git fetch origin
git checkout sub-games-integration-test
git pull origin sub-games-integration-test
git checkout -b sgit-PR1-16/03
# work -> commit -> ...
git push origin sgit-PR1-16/03
```

### Ouverture et révision
- Titre: court et clair (par exemple "Collision sur les plates-formes de hall")
- Description: expliquer quoi + pourquoi, lier les documents/questions connexes, le cas échéant
- Ajouter les évaluateurs :
- Branches de jeu -> principalement le propriétaire de la branche
- Directions partagées -> tous les autres membres de l'équipe
- Chat de groupe Ping: "Pr #X ouvert pour examen"

Nous ouvrons souvent comme projet de RP tôt pour obtenir des commentaires avant qu'il soit terminé.

### Règles de fusion (convention d'équipe)
- Branches de jeu/de caractéristiques -> doit au moins être approuvé par le propriétaire de la branche
-> seul le propriétaire (ou le délégué) de la succursale doit fusionner
- Les branches partagées (`sgit`,`docs-setup`,`main`, etc.) -> doit être approuvé par tous les autres membres de l'équipe
-> seul l'auteur du PR fusionne une fois que les approbations sont en (sauf indication contraire)
- Utiliser **"Créer un commit de fusion"** (par défaut) -> conserve la totalité de l'auteur et l'historique granulaire
- Ne pas squash ou rebase à moins que l'équipe accepte explicitement pour ce PR

### Après fusion
- Supprimer la branche PR de GitHub (conserve l'onglet Branches propres)
- Tirer le dernier sur la branche cible
- L'historique reste entièrement traçable via le commit de fusion

## Style de code

Voir @ref code_style_and_conventions "Code Style & Conventions" pour le nom, le formatage, la mise en page des fichiers, etc.

Il est principalement basé sur la façon dont le projet a commencé — objectif est la cohérence de base, pas l'uniformité parfaite.
Si quelque chose ne vous convient pas ou n'arrive pas, faites-le monter en chat ou ouvrez un PR pour améliorer le guide de style.

## Commentaires et questions

- Ping le chat de groupe lors de l'ouverture d'un PR ou lorsque vous avez besoin d'entrée
- Demander "ça va ?" ou "que pensez-vous de cette approche?" est tout à fait normal
- Fusionner les conflits — il suffit de demander de l'aide, aucun problème

Merci de travailler ensemble — prenez votre temps, posez des questions, profitez du processus!

**Créé :** 15 janvier 2026
**Dernière mise à jour :** 16 mars 2026
**Auteur:** [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)

@ref index "Back to Home"