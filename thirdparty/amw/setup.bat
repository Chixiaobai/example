@echo off&pushd %~dp0 &title Amw���������
:menu
echo.&echo �������л���ֻ�ܶ�ѡһ
echo.&echo 1����װ64λAmw�����(Visual Studio 2017)
echo.&echo 2����װ32λAmw�����(Visual Studio 2017)
echo.&echo 3��ж�ص�ǰAmw�����
echo.&echo 4���˳�
echo.&echo.

set /p a=�������ֻس���
IF NOT "%a%"=="" SET a=%a:~0,1%

if "%a%"=="1" Goto install_v141_x64
if "%a%"=="2" Goto install_v141_x86
if "%a%"=="3" Goto uninstall
if "%a%"=="4" Goto finish
echo.&echo ������Ч�����������룡 
PAUSE >NUL && CLS && GOTO Menu

:uninstall
call "%~dp0/Runtime_Env/Runtime_init.bat" --uninstall
pause
call "%~dp0/Development_Env/Develop_init.bat" --uninstall
cls && echo.&echo Amw�����ж����ɣ���������أ�&&pause >nul && cls && goto menu

:install_v141_x64
call "%~dp0/Development_Env/Develop_init.bat" --install x86_64 v141 x64
pause
call "%~dp0/Runtime_Env/Runtime_init.bat" --install x86_64 v141 x64
cls && echo.&echo Amw�������װ��ɣ���������أ�&&pause >nul && cls && goto menu

:install_v141_x86
call "%~dp0/Development_Env/Develop_init.bat" --install x86_64 v141 x86
pause
call "%~dp0/Runtime_Env/Runtime_init.bat" --install x86_64 v141 x86
cls && echo.&echo Amw�������װ��ɣ���������أ�&&pause >nul && cls && goto menu

:finish
echo.

