@echo off

set INCLUDE=.\include
set LIB=.\lib
set SRC=.\src

echo Building GLAD...
rem gcc -shared -I%INCLUDE% %SRC%\lib\glad.c -o %LIB%\glad.dll -Wl,--out-implib,%LIB%\libglad.dll.a
gcc -c -I%INCLUDE% %SRC%\lib\glad.c -o %LIB%\glad.o
ar rcs %LIB%\libglad.a %LIB%\glad.o
del %LIB%\glad.o