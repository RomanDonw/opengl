@echo off

set INCLUDE=.\include
set LIB=.\lib
set SRC=.\src

echo Building main...
rem g++ -static -I%INCLUDE% -L%LIB% %SRC%\main.cpp -o .\main.exe -lglad -lglfw3 -lopengl32 -lgdi32
g++ -c -I%INCLUDE% -L%LIB% %SRC%\main.cpp -o main.o
echo Linking main...
g++ -I%INCLUDE% -L%LIB% ./main.o %LIB%\libglad.a %LIB%\libglfw3.a -lopengl32 -lgdi32 -o ./main.exe
del main.o

rem copy %LIB%\glfw3.dll .
rem copy %LIB%\glad.dll .