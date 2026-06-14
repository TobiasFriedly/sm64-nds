#!/bin/sh
set -e

REGION="${1:-us}"
case "$REGION" in
  us|eu|jp|sh|cn) ;;
  *)
    echo "Unknown region: $REGION"
    echo "Use one of: us eu jp sh cn   (default: us)"
    exit 1
    ;;
esac

if [ ! -f "baserom.$REGION.z64" ]; then
  echo "Missing baserom.$REGION.z64"
  echo "Put your own Super Mario 64 ($REGION) ROM dump in this folder,"
  echo "renamed to baserom.$REGION.z64, then run this again."
  exit 1
fi

JOBS=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

docker build -t sm64ds .
docker run --rm --mount type=bind,source="$(pwd)",destination=/sm64 sm64ds make VERSION="$REGION" -j"$JOBS"

echo "Done. ROM: build/${REGION}_nds/sm64.${REGION}.nds"
