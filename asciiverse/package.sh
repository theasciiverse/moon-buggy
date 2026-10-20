#!/bin/sh
# Packs dist/moon-buggy-<target>.tar.gz (.zip for Windows) for av: the
# binary at the top, with COPYING, README and PATCHES.md.
#
#   asciiverse/package.sh x86_64-unknown-linux-musl
set -eu
cd "$(dirname "$0")/.."
target="$1"
case "$target" in *windows*) exe=.exe ;; *) exe= ;; esac
stage=stage/moon-buggy
rm -rf "$stage" && mkdir -p "$stage" dist
cp "moon-buggy$exe" COPYING README PATCHES.md "$stage/"
if [ -n "$exe" ]; then
  (cd "$stage" && zip -qr "../../dist/moon-buggy-$target.zip" .)
else
  tar -czf "dist/moon-buggy-$target.tar.gz" -C "$stage" .
fi
ls dist
