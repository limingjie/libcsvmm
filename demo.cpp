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
        for (size_t record = 0; record < csv.size(); ++record)
        {
            for (size_t field = 0; field < csv.size(record); ++field)
            {
                std::cerr << "field [" << record << ", " << field << "] = "
                    << csv.to_string(record, field, true) << std::endl;
            }
        }
    }
}

void read_file(const std::string &filename)
{
    csvmm csv;

    if (csv.read(filename))
    {
        csv.write(std::cout);
        csv.erase(3);
        csv.erase(2, 2);
        csv.insert(3);
        csv.insert(3, 0, "0000000");
        csv.insert(3, 1, "1111111");
        csv.insert(3, 2, " \"hello\r\nworld\"   ");
        csv.insert(8);
        csv.insert(8, 0, "the last record");
        csv.insert(8, 0, "");
        csv.insert(8, 0, "");
        csv.write(std::cout);
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
