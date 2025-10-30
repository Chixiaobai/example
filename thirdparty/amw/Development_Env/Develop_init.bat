if "%GENERIC_DEVELOPMENT_ENVIRONMENT_ROOT%"=="" (
    echo Error: you should install sdk development environment first. 
    pause
    exit /B 
)

if "%1" == "" Goto install
if "%1" == "--install" Goto install
if "%1" == "--uninstall" Goto uninstall
echo Error: invalid parameters
Goto :finish

:uninstall
echo.&echo ����ж��Amw����������
wmic ENVIRONMENT where "name='AMW_DEVELOPMENT_ENVIRONMENT_ROOT'" delete
echo.&echo Amw��������ж����ɣ�
Goto :finish

:install
echo.&echo ���ڰ�װAmw����������
set AmwDevelopmentRoot=%~dp0
wmic ENVIRONMENT where "name='AMW_DEVELOPMENT_ENVIRONMENT_ROOT'" delete
wmic ENVIRONMENT create name="AMW_DEVELOPMENT_ENVIRONMENT_ROOT",username="%ComputerName%\\%UserName%",VariableValue="%AmwDevelopmentRoot:~0,-1%"
echo.&echo Amw����������װ��ɣ�
Goto :finish

:finish
