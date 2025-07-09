#!/bin/bash

# Test: 1000 de cada receta
# Hamburguesas: 1000, Tacos: 1000, Sandwiches: 1000, Quesadillas: 1000

echo "=== TEST: 1000 de cada receta ==="
echo "Ejecutando:./kitchen 1000 1000 1000 1000"
echo "Total: 4000 platos"
echo ""

echo "⚠️  ATENCIÓN: Este test preparará 4000 platos y puede tomar mucho tiempo"
echo "Presiona Ctrl+C para cancelar en cualquier momento"
echo ""

./kitchen 1000 1000 1000 1000 