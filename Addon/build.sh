#!/bin/sh
# Baut das Heizkalender-Addon-Paket (heizkalender-addon-<version>.tar.gz)
# Aufruf: ./Addon/build_addon.sh  (vom Projektverzeichnis aus)
# Ablage: Addon/build/heizkalender-addon-<version>.tar.gz

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
VERSION="$(cat "${SCRIPT_DIR}/src/VERSION")"
OUT="${SCRIPT_DIR}/build/heizkalender-addon-${VERSION}.tar.gz"

mkdir -p "${SCRIPT_DIR}/build"
cd "${SCRIPT_DIR}/src"

tar -czf "${OUT}" \
  --owner=0 --group=0 \
  --exclude='.DS_Store' \
  VERSION \
  update_script \
  heizkalender/www/ \
  rc.d/

echo "Gebaut: ${OUT} ($(du -sh "${OUT}" | cut -f1))"
