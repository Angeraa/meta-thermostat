#!/usr/bin/env bash
set -euo pipefail

# Build only the application recipe, using the image's Yocto configuration.
# An optional SDK environment script selects a standalone CMake build instead.

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SOURCE_DIR="${ROOT_DIR}"
BUILD_DIR="${ROOT_DIR}/yocto-build"
OUTPUT_DIR="${BUILD_DIR}/package"
YOCTO_BUILD_DIR="${YOCTO_BUILD_DIR:-${ROOT_DIR}/../yocto/build}"
YOCTO_SETUP="${YOCTO_SETUP:-${ROOT_DIR}/../yocto/sources/poky/oe-init-build-env}"
SDK_ENV="${1:-}"

if (( $# > 1 )); then
  echo "Usage: $0 [SDK environment-setup script]" >&2
  exit 1
fi

if [[ -n "${SDK_ENV}" ]]; then
  if [[ ! -f "${SDK_ENV}" ]]; then
    echo "SDK environment script not found: ${SDK_ENV}" >&2
    exit 1
  fi
  set +u
  # shellcheck disable=SC1090
  source "${SDK_ENV}"
  set -euo pipefail

  cmake -S "${SOURCE_DIR}" -B "${ROOT_DIR}/build-sdk" \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    -DCMAKE_INSTALL_PREFIX=/usr
  cmake --build "${ROOT_DIR}/build-sdk" --target Thermostat --parallel "$(nproc)"
  mkdir -p "${BUILD_DIR}"
  install -m 0755 "${ROOT_DIR}/build-sdk/Thermostat" "${BUILD_DIR}/Thermostat"
else
  if [[ ! -f "${YOCTO_SETUP}" || ! -f "${YOCTO_BUILD_DIR}/conf/local.conf" ]]; then
    echo "Yocto setup or configured build directory missing. Set YOCTO_SETUP and YOCTO_BUILD_DIR, or pass an SDK environment script." >&2
    exit 1
  fi
  YOCTO_BUILD_DIR="$(cd "${YOCTO_BUILD_DIR}" && pwd)"
  set +u
  # shellcheck disable=SC1090
  source "${YOCTO_SETUP}" "${YOCTO_BUILD_DIR}"
  set -euo pipefail
  bitbake -c install thermostat
  INSTALL_DIR="$(bitbake -e thermostat | sed -n 's/^D="\(.*\)"$/\1/p')"
  if [[ -z "${INSTALL_DIR}" || ! -f "${INSTALL_DIR}/usr/bin/Thermostat" ]]; then
    echo "Thermostat binary missing from the recipe install directory: ${INSTALL_DIR}" >&2
    exit 1
  fi
  mkdir -p "${BUILD_DIR}"
  install -m 0755 "${INSTALL_DIR}/usr/bin/Thermostat" "${BUILD_DIR}/Thermostat"
fi

rm -rf "${OUTPUT_DIR}"
mkdir -p "${OUTPUT_DIR}/rootfs"
if [[ -n "${SDK_ENV}" ]]; then
  install -D -m 0755 "${BUILD_DIR}/Thermostat" "${OUTPUT_DIR}/rootfs/usr/bin/Thermostat"
else
  cp -a "${INSTALL_DIR}/." "${OUTPUT_DIR}/rootfs/"
fi

tar -C "${OUTPUT_DIR}/rootfs" -czf "${BUILD_DIR}/thermostat.tar.gz" .
echo "Binary: ${BUILD_DIR}/Thermostat"
echo "Created ${BUILD_DIR}/thermostat.tar.gz"
echo "On the target: systemctl stop thermostat; tar -xzf thermostat.tar.gz -C /; systemctl daemon-reload; systemctl start thermostat"