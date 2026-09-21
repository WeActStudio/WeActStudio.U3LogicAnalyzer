#!/bin/sh
# clean_pycache.sh

# Directory that holds this script
startDir=$(cd "$(dirname "$0")" && pwd)
cd "$startDir" || exit 1

# Remove all __pycache__ directories
find "$startDir" -type d -name "__pycache__" -print -exec rm -rf {} \;

# Remove all *.pyc files
find "$startDir" -type f -name "*.pyc" -print -delete

echo "Cleanup completed."
