@echo off

set INCLUDE=.\include
set LIB=.\lib
set SRC=.\src

echo Building main...
: g++ -static -I%INCLUDE% -L%LIB% %SRC%\main.cpp -o .\main.exe -lglad -lglfw3 -lopengl32 -lgdi32
g++ -c -I%INCLUDE% -L%LIB% %SRC%\main.cpp -o main.o

if not exist .\main.o goto on_error
    echo Linking main...
    g++ -I%INCLUDE% -L%LIB% ./main.o %LIB%\libglad.a %LIB%\libglfw3.a -lopengl32 -lgdi32 -lsoft_oal -lOpenAL32 -o ./main.exe
    del main.o

    : copy %LIB%\glfw3.dll .
    : copy %LIB%\glad.dll .
    copy %LIB%\soft_oal.dll .
    copy %LIB%\OpenAL32.dll .

    set ERRORLEVEL=0
    goto end

:on_error
    set ERRORLEVEL=1

:end