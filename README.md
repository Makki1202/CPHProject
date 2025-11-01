# CPHProject

For using files:
    Set "USEFILES" to true in Project.h
For using stdin:
    Set "USEFILES" to false in Project.h


To run normally, use at least C++17:
    g++ Project.cpp && ./a.out

To run with perf:
    WSL:
        /usr/lib/linux-tools/6.8.0-86-generic/perf stat -e instructions:u ./a.out
    Linux:
        perf stat -e instructions:u ./a.out
