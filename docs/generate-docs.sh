#!/bin/bash
# Generate Doxygen documentation

set -e

echo "Generating Doxygen documentation..."

cd doxygen

rm -rf html 2>/dev/null || true

doxygen Doxyfile

echo "Documentation generated in docs/doxygen/html/"
echo "Open docs/doxygen/html/index.html to view"
