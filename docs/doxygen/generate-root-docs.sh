#!/bin/bash
# generate-root-docs.sh
# Generates English documentation. Other languages via index.php + LibreTranslate.

set -e

echo "Generating documentation for $(basename "$PWD")..."

rm -rf docs/doxygen/html 2>/dev/null || true

cd docs/doxygen
doxygen Doxyfile

echo "Done. Open docs/doxygen/index.php in XAMPP."
