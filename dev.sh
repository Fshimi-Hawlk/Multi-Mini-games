# dev.nu - Environnement de Debug Haute Intensité
let session = "multi-mini-games-debug"
let debug_mode = "MODE=clang-debug"

# 1. Nettoyage et Rebuild (Pour garantir l'absence de résidus)
print "🔨 Compilation en mode Haute Sécurité (Clang Sanitizers)..."
make rebuild $debug_mode

# 2. Initialisation de la session TMUX
tmux new-session -d -s $session

# Volet Gauche : Serveur avec Logs de Debug
tmux send-keys -t $session $"make run-server ($debug_mode)" C-m

# Split Droit (Haut) : Client 1
tmux split-window -h -t $session
tmux send-keys -t $session $"make run-client ($debug_mode)" C-m

# Split Droit (Bas) : Client 2
tmux split-window -v -t $session
tmux send-keys -t $session $"make run-client ($debug_mode)" C-m

# Focus sur le serveur pour voir les premières connexions
tmux select-pane -t 0
tmux attach-session -t $session
