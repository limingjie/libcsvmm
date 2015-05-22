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

enum state_type
{
    state_data,
    state_field,
    state_quoted_field,
    state_quoted_field_quote
};

void emit_field(std::string &field)
{
    std::cout << "Field : [[" << field << "]]\n";
    field.clear();
}

void emit_record()
{
    std::cout << "--\n";
}

void parse_csv(std::istream &is)
{
    char c;
    bool reconsume = false;

    std::string field;

    state_type state = state_data;
    while (reconsume || is.get(c))
    {
        reconsume = false;

        switch (state)
        {
        case state_data:
            switch (c)
            {
            case ',':
                emit_field(field);
                break;
            case '\r':
                // omit
                break;
            case '\n':
                emit_field(field);
                emit_record();
                break;
            case '\"':
                state = state_quoted_field;
                break;
            default:
                reconsume = true;
                state = state_field;
                break;
            }
            break;
        case state_field:
            switch (c)
            {
            case ',':
            case '\r':
            case '\n':
                reconsume = true;
                state = state_data;
                break;
            default:
                field.push_back(c);
                break;
            }
            break;
        case state_quoted_field:
            switch (c)
            {
            case '\"':
                state = state_quoted_field_quote;
                break;
            case '\r':
                // omit
                break;
            case '\n':
                field.push_back('\r');
                field.push_back('\n');
                break;
            default:
                field.push_back(c);
                break;
            }
            break;
        case state_quoted_field_quote:
            switch (c)
            {
            case ',':
            case '\r':
            case '\n':
                reconsume = true;
                state = state_data;
                break;
            case '\"':
                field.push_back('\"');
                state = state_quoted_field;
                break;
            default:
                // parse error.
                field.push_back(c);
                state = state_field;
                break;
            }
            break;
        default:
            std::cerr << "what?\n";
        }
    }

    // EOF
    emit_field(field);
    emit_record();

    std::cout.flush();
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
