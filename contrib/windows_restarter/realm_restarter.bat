@ECHO OFF
@title OregonRealm
CLS
ECHO Initializing Realm (Logon-Server)...
:1
start "Oregon Realm" /B /MIN /WAIT oregon-realm.exe -c oregonrealm.conf
if %errorlevel% == 0 goto end
goto 1
:end