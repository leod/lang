rm -f a.out
find -name '*.cpp' | xargs gcc -Icompiler -lstdc++ -Wall -g -pedantic -Werror -Wextra -Wformat -Wconversion
