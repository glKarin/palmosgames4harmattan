#!/bin/sh

export PREENV_ROOT=$(dirname $(readlink -f "$0"))
export PREENV_APPID="$1"
. "$PREENV_ROOT/env.sh"

cd "$(dirname "$2")"
exec "$2"

