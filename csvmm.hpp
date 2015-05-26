//
// csvmm.hpp
//
// May 24, 2015 - by Mingjie Li (limingjie@outlook.com)
// https://github.com/limingjie/libcsvmm
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

class csvmm
{
private:
    typedef std::vector<std::string> record_t;
    typedef std::vector<record_t>    csv_t;

    enum state_type
    {
        _data_state,
        _data_cr_state,
        _field_state,
        _quoted_field_state,
        _quoted_field_quote_state,
        _quoted_field_cr_state
    };

    csv_t _csv;

    void _emit_field(std::string &field, record_t &record);
    void _emit_record(record_t &record);

    void _read(std::istream &is);

    // set binary to true will insert CR before LF,
    // it should only be set, if the ostream is in binary mode.
    void _write(std::ostream &os, bool binary);
    void _write_record(std::ostream &os, const record_t &record, bool binary);
    void _write_field(std::ostream &os, const std::string &field, bool binary);

public:
    csvmm();

    // to invoke delete from base class.
    virtual ~csvmm() {}

    bool read(std::istream &is);

    // read csv file.
    bool read(const std::string &filename);

    // non-binary output.
    bool write(std::ostream &os);

    // write csv file, make sure newline as CRLF.
    bool write(const std::string &filename);

    // insert a record before nth record.
    bool insert(size_t row);

    // insert a field before mth field of nth record.
    bool insert(size_t row, size_t column, const std::string &field);

    // erase record.
    bool erase(size_t row);

    // erase field.
    bool erase(size_t row, size_t column);

    // clear all data
    void clear();

    // return number of records.
    size_t size();

    // return number of fields of nth record.
    size_t size(size_t row);

    // serialize csv to string.
    std::string to_string();

    // serialize nth record to string.
    std::string to_string(size_t row);

    // serialize mth field of nth record to string.
    std::string to_string(size_t row, size_t column, bool raw = false);
};


