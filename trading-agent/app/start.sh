#!/usr/bin/env sh
# Mac / Linux start script (Windows: double-click start.bat).
set -e
cd "$(dirname "$0")"
[ -x .venv/bin/python ] || python3 -m venv .venv
.venv/bin/python -m pip install --quiet --disable-pip-version-check -r requirements.txt
cd ..
exec app/.venv/bin/python -m app.server "$@"
