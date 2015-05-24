//
// Demo to libcsvmm
//
// May 22, 2015 - by Mingjie Li (limingjie@outlook.com)
// https://github.com/limingjie/libcsvmm
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "csvmm.hpp"

void usage()
{
    std::cerr
        << "Usage: demo\n"
        << "   or: demo FILE\n"
        << "   or: demo -h\n"
        << "Parse CSV."
        << std::endl;
}

void read_std_input()
{
    csvmm csv;

    if (csv.read(std::cin))
    {
        csv.write(std::cout);
        csv.write("binary.csv");
    }
}

void read_file(const std::string &filename)
{
    csvmm csv;

    if (csv.read(filename))
    {
        csv.write(std::cout);
        csv.write("binary.csv");
    }
    else
    {
        std::cerr << "Error: fail to open file " << filename << std::endl;
    }
}

int main(int argc, char *argv[])
{
    switch (argc)
    {
    case 1:
        read_std_input();
        break;
    case 2:
        if (argv[1][0] == '-')
        {
            usage();
        }
        else
        {
            read_file(argv[1]);
        }
        break;
    default:
        usage();
    }

    return 0;
}
