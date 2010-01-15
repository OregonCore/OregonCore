@ECHO OFF
@title OregonRealm
CLS
ECHO Initializing Realm (Logon-Server)...
:1
start "Oregon Realm" /B /MIN /WAIT OregonRealm.exe -c OregonRealm.conf
if %errorlevel% == 0 goto end
goto 1
:end