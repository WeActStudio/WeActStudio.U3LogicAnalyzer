#!/bin/sh
# clean_pycache.sh

# 脚本所在目录
startDir=$(cd "$(dirname "$0")" && pwd)
cd "$startDir" || exit 1

# 删除所有 __pycache__ 目录
find "$startDir" -type d -name "__pycache__" -print -exec rm -rf {} \;

# 删除所有 *.pyc 文件
find "$startDir" -type f -name "*.pyc" -print -delete

echo "Cleanup completed."
