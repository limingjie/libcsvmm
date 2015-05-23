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
        return false;

    csv_t::const_iterator r_it;
    record_t::const_iterator f_it;
    std::string::const_iterator s_it;
    std::string field;

    // loop records
    for (r_it = _csv.cbegin(); r_it != _csv.cend(); ++r_it)
    {
        // loop fields
        for (f_it = (*r_it).cbegin(); f_it != (*r_it).cend(); ++f_it)
        {
            // if field contains double quote, quote it and escape " with "";
            if ((*f_it).find('\"') != std::string::npos)
            {
                field = '\"';

                for (s_it = (*f_it).cbegin(); s_it != (*f_it).cend(); ++s_it)
                {
                    if ((*s_it) == '\"')
                        field.push_back('\"');

                    field.push_back(*s_it);
                }

                field.push_back('\"');
            }
            // if field contains comma or newline, quote it.
            else if ((*f_it).find_first_of(",\n") != std::string::npos)
            {
                field = '\"' + *f_it + '\"';
            }
            else
            {
                field = *f_it;
            }

            // output comma before all fields except the first one.
            if (f_it != (*r_it).cbegin())
                os << ',';

            // output newline as CRLF in binary mode.
            if (binary && field.find('\n') != std::string::npos)
            {
                for (s_it = field.cbegin(); s_it != field.cend(); ++s_it)
                {
                    if ((*s_it) == '\n')
                        os << '\r';

                    os << *s_it;
                }
            }
            else
            {
                os << field;
            }
        }

        if (binary)
            os << '\r';

        os << '\n';
    }

    os.flush();

    return true;
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
