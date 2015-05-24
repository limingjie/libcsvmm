//
// csvmm.cpp
//
// May 24, 2015 - by Mingjie Li (limingjie@outlook.com)
// https://github.com/limingjie/libcsvmm
//

#include "csvmm.hpp"

csvmm::csvmm()
{

}

bool csvmm::read(std::istream &is)
{
    std::string field;
    record_t record;

    char c;
    bool reconsume = false;
    state_type state = _data_state;
    while (reconsume || is.get(c))
    {
        reconsume = false;

        switch (state)
        {
        case _data_state:
            switch (c)
            {
            case ',':
                _emit_field(field, record);
                break;
            case '\r':
                // omit
                break;
            case '\n':
                _emit_field(field, record);
                _emit_record(record);
                break;
            case '\"':
                state = _quoted_field_state;
                break;
            default:
                reconsume = true;
                state = _field_state;
                break;
            }
            break;
        case _field_state:
            switch (c)
            {
            case ',':
            case '\r':
            case '\n':
                reconsume = true;
                state = _data_state;
                break;
            default:
                field.push_back(c);
                break;
            }
            break;
        case _quoted_field_state:
            switch (c)
            {
            case '\"':
                state = _quoted_field_quote_state;
                break;
            case '\r':
                // omit
                break;
            case '\n':
                // field.push_back('\r');
                field.push_back('\n');
                break;
            default:
                field.push_back(c);
                break;
            }
            break;
        case _quoted_field_quote_state:
            switch (c)
            {
            case ',':
            case '\r':
            case '\n':
                reconsume = true;
                state = _data_state;
                break;
            case '\"':
                field.push_back('\"');
                state = _quoted_field_state;
                break;
            default:
                // parse error.
                field.push_back(c);
                state = _field_state;
                break;
            }
            break;
        default:
            // what?
            break;
        }
    }

    // EOF
    if (!field.empty() || c == ',')
    {
        _emit_field(field, record);
    }

    if (record.size() != 0)
    {
        _emit_record(record);
    }

    return true;
}

void csvmm::_emit_field(std::string &field, record_t &record)
{
    record.push_back(field);
    field.clear();
}

void csvmm::_emit_record(record_t &record)
{
    _csv.push_back(record);
    record.clear();
}

bool csvmm::_write(std::ostream &os, bool binary)
{
    if (!os.good())
    {
        return false;
    }

    for (auto it = _csv.cbegin(); it != _csv.cend(); ++it)
    {
        _write_record(os, *it, binary);
    }

    os.flush();

    return true;
}

void csvmm::_write_record(std::ostream &os, const record_t &record, bool binary)
{
    for (auto it = record.cbegin(); it != record.cend(); ++it)
    {
        if (it != record.cbegin())
        {
            os << ',';
        }

        _write_field(os, *it, binary);
    }

    if (binary)
        os << '\r';

    os << '\n';
}
void csvmm::_write_field(std::ostream &os, const std::string &field, bool binary)
{
    if (field.find_first_of(",\"\r\n") != std::string::npos)
    {
        os << '\"';

        for (auto it = field.cbegin(); it != field.cend(); ++it)
        {
            if (*it == '\"')
            {
                os << '\"';
            }
            else if (*it =='\r')
            {
                // omit '\r'
                continue;
            }
            else if (binary && *it =='\n')
            {
                os << '\r';
            }

            os << *it;
        }

        os << '\"';
    }
    else
    {
        os << field;
    }
}

bool csvmm::write(std::ostream &os)
{
    return _write(os, false);
}

bool csvmm::write(const std::string &filename)
{
    std::ofstream ofs(filename, std::ios_base::out | std::ios_base::binary);
    bool rc = _write(ofs, true);
    ofs.close();

    return rc;
}

std::string csvmm::get_field(size_t row, size_t column)
{
    if (row >= _csv.size() || column >= _csv[row].size())
    {
        return "";
    }
    else
    {
        return _csv[row][column];
    }
}
