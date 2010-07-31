rm -f a.out
find -name '*.cpp' | xargs gcc -Icompiler -lstdc++ -Wall -g -pedantic -Wextra -Wformat -Wconversion -std=c++0x -Wfatal-errors || exit $?
