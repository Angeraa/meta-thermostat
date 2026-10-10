#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ARCHIVE="${ROOT_DIR}/dist/thermostat.tar.gz"
TARGET="${1:?Usage: $0 user@target}"

if [[ ! -f "${ARCHIVE}" ]]; then
  echo "Build the application first: ${ROOT_DIR}/build-thermostat.sh" >&2
  exit 1
fi

scp "${ARCHIVE}" "${TARGET}:/tmp/thermostat.tar.gz"
ssh "${TARGET}" 'sudo tar -xzf /tmp/thermostat.tar.gz -C / && sudo systemctl daemon-reload && sudo systemctl restart thermostat.service'