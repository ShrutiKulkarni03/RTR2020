gcc -fPIC -c aoMath.c
gcc -shared -o libaoMath.so aoMath.o
gcc -c main.c
gcc -o main main.o libaoMath.so
./main
