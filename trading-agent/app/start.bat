@echo off
setlocal
cd /d "%~dp0"
title Concept Desk (paper trading)
where py >nul 2>nul && (set "PY=py -3") || (set "PY=python")
%PY% -c "import sys; sys.exit(0 if sys.version_info >= (3, 10) else 1)" 2>nul
if errorlevel 1 (
  echo Python 3.10 or newer is needed.
  echo Install it from https://www.python.org/downloads/ and tick "Add python.exe to PATH", then run this again.
  pause
  exit /b 1
)
if not exist ".venv\Scripts\python.exe" (
  echo First start: setting up a private Python environment...
  %PY% -m venv .venv
  if errorlevel 1 ( pause & exit /b 1 )
)
".venv\Scripts\python.exe" -m pip install --quiet --disable-pip-version-check -r requirements.txt
if errorlevel 1 (
  echo Could not install the requirements. Check your internet connection and try again.
  pause
  exit /b 1
)
cd ..
echo Starting Concept Desk. Your browser will open http://localhost:8765
echo Keep this window open while you trade. Close it to stop the app.
"app\.venv\Scripts\python.exe" -m app.server %*
pause
