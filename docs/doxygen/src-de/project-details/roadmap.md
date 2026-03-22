@page roadmap Projekt Roadmap

@ref index "Back to Home"

**Letzte Überprüfung gegen Projektstruktur:* 16. März 2026

Diese Seite zeigt, wo wir jetzt sind und der grobe Pfad, den wir vereinbart haben, um die Dinge zu verwalten.

## Aktueller Status (März 2026)

- Ja`main`-> frühes Skelett + einige Punkte, aber jetzt vollständig veraltet. Wir haben es vor Monaten nicht berührt.
- Aktive Entwicklungszweige:
- Individuelle Spielzweige (z.`tetris`,`snake-classic`,`block-blast`,`memory`, etc.) -> wo einzelspieler-spiele gebaut und poliert standalone
- Ja`docs-setup`-> geteilte dokumentation, code-stil regeln, setup guides, kleine refactors, die mehrere teile berühren
- Ja`sub-games-integration-test`(wir nennen es)`sgit`) -> den aktuellen Hauptintegrationszweig. Wir verschmelzen fertige Single-Player-Spiele hier, um sie in die Lobby zu haken, sie gemeinsam testen, Lobby-Bugs beheben, gemeinsame Funktionen hinzufügen

Gerade jetzt: mehrere Spiele arbeiten standalone, ein Paar sind bereits in der Lobby über`sgit`. Die Lobby selbst hat grundlegende Bewegung, Kollisionsauslöser und UI, aber es ist immer noch rau um die Kanten.

## Geplante langfristige Struktur

Um große Konflikte zu vermeiden, wenn wir Multiplayer (Netzwerkcode, Sync-Probleme, verschiedene Spiellogik) hinzufügen, haben wir uns auf diesen phased-Ansatz geeinigt:

1. **Einspielerphase** (aktuelle / nächste Wochenmonate)
- Fertigen und Polieren Einzelspieler-Versionen auf eigenen Zweigen
- Integrieren Sie sie von einem in die Lobby durch`sgit`
- Sobald wir 4–5+ Spiele gut zusammen arbeiten -> vielleicht umbenennen`sgit`etwas klarer wie`singleplayer-games-integration-test`

2. **Multiplayer Phase** (nächste Hauptschritt nach Single-Player fühlt sich solide)
- Erstellen Sie einen neuen Zweig:`multiplayer-games-integration-test`(kurz:`mgit`)
- Prototype Multiplayer-Versionen dort (Netzwerk-, Dreh- oder Echtzeit-Sync, UI-Differenzen)
- Hook MP Versionen in die Lobby getrennt -> Einzelspielercode unberührt halten

3. ** Kombinieren Sie Phase** (wenn beide SP und MP bereit aussehen)
- Erstellen`combine-games-integration-test`(kurz:`cgit`)
- Sammeln Sie Einzelspieler und Multiplayer-Zweigstellen dort
- Konflikte lösen (lokale vs vernetzte Modi, UI-Folge, Leistungsprobleme usw.)
- Testen Sie die komplette Lobby mit einer Mischung von SP + MP-Spielen

4. **Tabellenfreigabe an Haupt**
- Wann`cgit`ist spielbar und stabil -> alles zusammenführen`main`
- Tag einer Version (z.B. v0.1-singleplayer oder v1.0 wenn ambitioniert)
- Vielleicht erstellen GitHub Release mit Builds, Screenshots, kurze Beschreibung

Dies ist unser Plan, Äste testbar zu halten und die Verschmelzung Hölle zu reduzieren.
In der Praxis
- Wenn etwas klein und konfliktfrei ist, könnten wir Schritte überspringen und früher verschmelzen
- Wenn Multiplayer länger dauert als erwartet, können wir eine Einzelspieler-nur-Version zu versenden`main`erste
- Alles ist flexibel - wir werden im Gruppenchat diskutieren und sich anpassen, wenn nötig

## Schnelle To-Do / Ideenliste

- Erhalten Sie mindestens 4–5 Einzelspieler-Spiele vollständig integriert und spielbar in`sgit`
- Verbesserung der Lobby: schönere Spielauswahl (kann Schilder oder Türen statt nur hitboxes), Basispunkteanzeige, Pause/Resume, Hautbeharrlichkeit
- Starten Sie Multiplayer-Prototyping: wählen Sie 1–2 einfache Spiele (z.B. drehbasiert wie Speicher oder Block-Blast) und experimentieren Sie mit Netzwerken auf einem Feature-Zweig
- Geteilte Verbesserungen: bessere Audio-Handhabung, einheitliche Eingabe-Konfiguration, verbesserte Fehlersuche/Recovery
- Dokumentation: fügen Sie Screenshots oder GIFs der Lobby + laufende Spiele, Finish Setup Anleitungen

Wenn eine dieser Klänge interessant ist — fühlen Sie sich frei, einspringen! Öffnen Sie Probleme, starten Sie Draft PRs, stellen Sie Fragen im Chat. Fehler werden erwartet, wir lernen alle.

**Erstellt:* 16. März 2026
**Letzte Aktualisierung:* 16. März 2026
**Autor:** [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)

@ref index "Back to Home"