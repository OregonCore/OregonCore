@ECHO OFF
@title OregonCore
CLS
ECHO Initializing Core (World-Server)...
:1
start "Oregon Core" /B /MIN /WAIT OregonCore.exe -c OregonCore.conf
if %errorlevel% == 0 goto end
goto 1
:end