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
        << "   or: demo -s STRING\n"
        << "   or: demo -h\n"
        << "Parse CSV."
        << std::endl;
}

void parse_csv(std::istream &is)
{
    csvmm csv;
    csv.read(is);
    csv.write(std::cout);
    csv.write("binary.csv");
}

void read_std_input()
{
    std::cerr << "Info: read_std_input" << std::endl;

    parse_csv(std::cin);
}

void read_file(const std::string &filename)
{
    std::ifstream ifs(filename);

    if (ifs)
    {
        std::cerr << "Info: read_file" << std::endl;

        parse_csv(ifs);
    }
    else
    {
        std::cerr << "Error: fail to open file " << filename << std::endl;
    }

    ifs.close();
}

void read_string(const std::string &data)
{
    std::cerr << "Info: read_string" << std::endl;

    std::stringstream ss(data);
    parse_csv(ss);
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
    case 3:
        if (std::string(argv[1]) == "-s")
        {
            read_string(argv[2]);
        }
        else
        {
            usage();
        }
        break;
    default:
        usage();
    }

    return 0;
}
