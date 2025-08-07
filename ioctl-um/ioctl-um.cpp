#include "windows.h"
#include "iostream"

#include "io.hpp"

int main()
{
    io->create_handle();

    io->attach_vm(L"notepad.exe");
    while ( 1 );

    std::cin.get();
}