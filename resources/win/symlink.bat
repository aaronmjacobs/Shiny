@echo off

:: If the symlink already exists, do nothing
if exist "%2" exit /B

:: Execute mklink as admin (will generate UAC prompt)
echo Set UAC = CreateObject^("Shell.Application"^) > "%temp%\getadmin.vbs"
echo UAC.ShellExecute "cmd.exe", "/c mklink /D ""%2"" ""%1""", "", "runas", 1 >> "%temp%\getadmin.vbs"

"%temp%\getadmin.vbs"
del "%temp%\getadmin.vbs"
exit /B
