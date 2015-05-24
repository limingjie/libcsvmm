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

void csvmm::_read(std::istream &is)
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

void csvmm::_write(std::ostream &os, bool binary)
{
    for (auto it = _csv.cbegin(); it != _csv.cend(); ++it)
    {
        _write_record(os, *it, binary);
    }

    os.flush();
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

bool csvmm::read(std::istream &is)
{
    if (is)
    {
        _read(is);
        return true;
    }

    return false;
}

bool csvmm::read(const std::string &filename)
{
    std::ifstream ifs(filename);

    if (ifs)
    {
        _read(ifs);
        ifs.close();

        return true;
    }

    return false;
}

bool csvmm::write(std::ostream &os)
{
    if (os)
    {
        _write(os, false);
        return true;
    }

    return false;
}

bool csvmm::write(const std::string &filename)
{
    std::ofstream ofs(filename, std::ios_base::out | std::ios_base::binary);

    if (ofs)
    {
        _write(ofs, true);
        ofs.close();

        return true;
    }

    return false;
}

size_t csvmm::size()
{
    return _csv.size();
}

size_t csvmm::size(size_t row)
{
    if (row < _csv.size())
    {
        return _csv[row].size();
    }

    return 0;
}

std::string csvmm::to_string()
{
    std::ostringstream oss;

    _write(oss, false);

    return oss.str();
}

std::string csvmm::to_string(size_t row)
{
    std::ostringstream oss;

    if (row < _csv.size())
    {
        _write_record(oss, _csv[row], false);
    }

    return oss.str();
}

std::string csvmm::to_string(size_t row, size_t column)
{
    std::ostringstream oss;

    if (row < _csv.size() && column < _csv[row].size())
    {
        _write_field(oss, _csv[row][column], false);
    }

    return oss.str();
}
