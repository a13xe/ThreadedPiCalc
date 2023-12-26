@echo off
echo Compiling threaded_pi_calc_win32api.cpp...
g++ -static -o threaded_pi_calc_win32api threaded_pi_calc_win32api.cpp
if %ERRORLEVEL% neq 0 goto error

echo Running threaded_pi_calc_win32api.exe...
threaded_pi_calc_win32api.exe
if %ERRORLEVEL% neq 0 goto error

echo.
echo Compiling threaded_pi_calc_openmp.cpp...
g++ -fopenmp -static -o threaded_pi_calc_openmp threaded_pi_calc_openmp.cpp
if %ERRORLEVEL% neq 0 goto error

echo Running threaded_pi_calc_openmp.exe...
threaded_pi_calc_openmp.exe
if %ERRORLEVEL% neq 0 goto error

echo.
echo Both programs executed successfully.
pause
exit

:error
echo An error occurred during execution.
pause
