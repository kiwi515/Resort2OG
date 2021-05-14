#include <iostream>
#include "convert.h"
#include "types.h"
#include "core/endian.h"

void showUsage();

int main(int argc, char* argv[])
{
    // Check arg count
    if (argc != 3)
    {
        showUsage();
        return 0;
    }

    if (_strcmpi(argv[1], "KCL") == 0)
    {
        convertFile(argv[2], EFileType::KCL);
    }
    else if (_strcmpi(argv[1], "PMP") == 0)
    {
        convertFile(argv[2], EFileType::PMP);
    }
    else
    {
        showUsage();
    }

    return 0;
}

void showUsage()
{
    std::cout << "Invalid input!" << std::endl;
    std::cout << "Valid usage:" << std::endl << "Resort2OG.exe kcl/pmp myFile.kcl/myFile.pmp" << std::endl;
}
