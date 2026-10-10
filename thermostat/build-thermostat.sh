#!/usr/bin/env bash
set -euo pipefail

# Build the application without running bitbake. For a target-compatible
# cross build, pass the Yocto SDK environment-setup script as the first
# argument. With no argument this builds natively (for example on the Pi).

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SOURCE_DIR="${ROOT_DIR}"
BUILD_DIR="${ROOT_DIR}/build-standalone"
OUTPUT_DIR="${ROOT_DIR}/dist"
SDK_ENV="${1:-}"

if [[ -n "${SDK_ENV}" ]]; then
  if [[ ! -f "${SDK_ENV}" ]]; then
    echo "SDK environment script not found: ${SDK_ENV}" >&2
    exit 1
  fi
  # shellcheck disable=SC1090
  source "${SDK_ENV}"
fi

cmake -S "${SOURCE_DIR}" -B "${BUILD_DIR}" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=/usr
cmake --build "${BUILD_DIR}" --parallel "$(nproc)"

rm -rf "${OUTPUT_DIR}"
cmake --install "${BUILD_DIR}" --prefix "${OUTPUT_DIR}/rootfs"

tar -C "${OUTPUT_DIR}/rootfs" -czf "${OUTPUT_DIR}/thermostat.tar.gz" .
echo "Created ${OUTPUT_DIR}/thermostat.tar.gz"
echo "Install on the target with: tar -xzf thermostat.tar.gz -C /"