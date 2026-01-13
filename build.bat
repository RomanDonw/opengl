@echo off

set INCLUDE=.\include
set LIB=.\lib
set SRC=.\src

echo Building GLAD...
gcc -shared -I%INCLUDE% %SRC%\lib\glad.c -o %LIB%\glad.dll -Wl,--out-implib,%LIB%\libglad.dll.a

echo Building main...
g++ -I%INCLUDE% -L%LIB% %SRC%\main.cpp -o .\main.exe -lglad -lglfw3 -lopengl32 -lgdi32

copy %LIB%\glfw3.dll .
copy %LIB%\glad.dll .