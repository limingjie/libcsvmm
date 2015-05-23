//
// csvmm.hpp
//
// May 24, 2015 - by Mingjie Li (limingjie@outlook.com)
// https://github.com/limingjie/libcsvmm
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

class csvmm
{
private:
    using record_t = std::vector<std::string>;
    using csv_t    = std::vector<record_t>;

    enum state_type
    {
        _data_state,
        _field_state,
        _quoted_field_state,
        _quoted_field_quote_state
    };

    csv_t _csv;

    void _emit_field(std::string &field, record_t &record);
    void _emit_record(record_t &record);

    // set binary to true will insert CR before LF,
    // it should only be set, if the ostream is in binary mode.
    bool _write(std::ostream &os, bool binary);

public:
    csvmm();

    // to invoke delete from base class.
    virtual ~csvmm() {}

    bool read(std::istream &is);

    // non-binary output, newline depends on system.
    bool write(std::ostream &os);

    // binary output, make sure newline as CRLF.
    bool write(const std::string &filename);

    // TODO: counts
    std::string get_field(size_t row, size_t column);
};
