@page changelog Änderungen

@ref index "Back to Home"

**Letzte Überprüfung gegen Projektstruktur:* 16. März 2026

Diese Seite listet alle bemerkenswerten Änderungen an der Multi Mini-Games monorepo.

Das Format folgt [Changelog weiterführen](https://keepachangelog.com/en/1.1.0/) Konventionen.
Wir verwenden Semantic Versioning, wo es Sinn macht, oder Kalender Versioning für frühe Entwicklungsstufen.

### Wie wir dieses Changelog pflegen

- Hier werden nur signifikante Änderungen erfasst (neue Spielintegration, großer gemeinsamer Code, große Refactors, wichtige Dokumentationsaktualisierungen usw.)
- Alles geht unter **[Unreleased]**, bis das Team entscheidet, dass ein Versions-Tag bereit ist (Beispiel: 0.1.0, wenn Lobby + mehrere Spiele zusammen spielbar sind)

### (Unveröffentlicht)

#### Zusätzlich
- Monorepo-Stiftung: geteilte Ordner (`firstparty/`,`thirdparty/`,`assets/`)
- Kerndokumentationsdateien in root: README.md, CONTRIBUTING.md, TODO.md, CodeStyleAndConventions.md
- Vorlagenordner`sub-project-example/`um neue spielstruktur zu führen
- Rechtliche Grundlagen: LICENSE und NOTICE-Dateien (Drittmittelkredite)
- Root Makefile mit faulen Statik-lib baut, API-Header Kopieren auf`firstparty/APIs/`, und inkrementelle lobby ausführbare verknüpfung
- Zentrales Konfigurationssystem:`gameConfig.h`bestimmung`GameConfig_St`,`AudioConfig_St`,`VideoConfig_St`+ standardmakros
- Erste volles Spiel API:`solitaireAPI.h`(Standard-Minispielmuster, implementiert von Maxime CHAUVEAU)
- Initial Tetris API Header:`tetrisAPI.h`(vorschlag für eine kommende umsetzung)

#### Geändert
- Verbesserte Root README: deutlichere Projekterklärung + Anfängerfreundliche Git-Befehle
- Vereinfachte und erweichte CONTRIBUTING.md: einfachere Workflow-Schritte, praktische Git-Beispiele, weniger strenger Ton auf Stil
- Dieses Änderungsprotokoll selbst: ergänzte klarere Aktualisierungsregeln und widerspiegelte die jüngsten Verbesserungen
- Hergestellt TODO.md Formulierung direkter, während die volle Aufgabenliste
- Aktualisierte CodeStyleAndConventions.md: jetzt als empfohlener Stil präsentiert (nicht zwingend), mit Einladung zu diskutieren unannehmliche Regeln
- Erweiterte Fehlerbehandlung: neue Codes`ERROR_INVALID_CONFIG`,`ERROR_AUDIO_LOAD`,`ERROR_WINDOW_INIT`,`ERROR_ASSET_LOAD`zugefügt`generalAPI.h`
- Standardisiertes Mini-Spiel-API-Muster: konsequente Verwendung`GameConfig_St*`, hinzugefügt`isRunning()`helper, einheitliche namensgebung über spiele

#### Behoben
- (noch nicht aufgezeichnet)

#### Entfernt
- (noch nicht aufgezeichnet)

**Erstellt:* 15. Januar 2025
**Letzte Aktualisierung:* 16. März 2026
**Autor:** [Fshimi Hawlk](https://github.com/Fshimi-Hawlk)

@ref index "Back to Home"