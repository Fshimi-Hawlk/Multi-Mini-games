#!/bin/bash

# test-multi.sh - Lance un serveur et plusieurs instances du jeu pour tester le multijoueur.

NUM_CLIENTS=${1:-2}
MODE=${2:-release}

echo "--- Préparation du test multi-joueurs ($NUM_CLIENTS clients) ---"

# 1. Tuer les instances précédentes
pkill -9 main 2>/dev/null
pkill -9 server 2>/dev/null

# 2. Recompiler
echo "🔨 Compilation..."
make all MODE=$MODE
if [ $? -ne 0 ]; then
    echo "❌ Échec de la compilation."
    exit 1
fi

# 3. Lancer le serveur
echo "🌐 Lancement du serveur..."
./build/bin/server > logs/server_test.log 2>&1 &
SERVER_PID=$!
sleep 1

# 4. Lancer les clients
for i in $(seq 1 $NUM_CLIENTS); do
    echo "🎮 Lancement du Client $i..."
    ./build/bin/main > "logs/client_${i}_test.log" 2>&1 &
done

echo "✅ Tout est lancé. Utilisez 'pkill main' pour fermer les clients."
echo "Logs disponibles dans le dossier logs/."

# Garder le script actif pour pouvoir tout tuer à la fin (optionnel)
# wait
