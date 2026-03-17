#!/bin/bash
# generate-docs.sh
# One-command documentation generation with convenient entry point

set -e  # exit on any error

echo "Generating Doxygen documentation for $(basename "$PWD")..."

rm -rf docs/doxygen/html docs/doxygen/index.html 2>/dev/null || true

cd docs/doxygen

# Run Doxygen
doxygen Doxyfile

# Create convenient redirect at docs/index.html
cat > index.html << 'REDIRECT'
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="refresh" content="0; url=html/index.html">
    <title>Documentation - $(basename "$PWD")</title>
    <style>
        body { font-family: system-ui, sans-serif; text-align: center; padding: 4rem; }
        a { color: #0066cc; }
    </style>
</head>
<body>
    <h1>Documentation generated</h1>
    <p>Redirecting to the full documentation...</p>
    <p>If nothing happens, <a href="html/index.html">click here</a>.</p>
</body>
</html>
REDIRECT