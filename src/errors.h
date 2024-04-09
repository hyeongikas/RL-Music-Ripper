#ifndef ERRORS_H
#define ERRORS_H

#include <iostream>
#include <string>

class Parse_error {
public:
    virtual void print_self(std::ostream& os) const {
        os << "unspecified.";
    }

    friend std::ostream& operator<<(std::ostream& os, const Parse_error& pe) {
        os << "Parse error: ";
        pe.print_self(os);
        return os;
    }

    virtual ~Parse_error() {}
};

class Argument_error : public Parse_error {
    std::string errmsg;

public:
    Argument_error(const char* str) : errmsg(str) {}

    void print_self(std::ostream& os) const override {
        os << "Argument error: " << errmsg;
    }
};

class File_open_error : public Parse_error {
    std::string filename;

public:
    File_open_error(const std::string& name) : filename(name) {}

    void print_self(std::ostream& os) const override {
        os << "Error opening " << filename;
    }
};

class Parse_error_str : public Parse_error {
    std::string str;

public:
    explicit Parse_error_str(std::string s) : str(std::move(s)) {}

    void print_self(std::ostream& os) const override {
        os << str;
    }
};

class Size_mismatch : public Parse_error {
    unsigned long real_size;
    unsigned long read_size;

public:
    Size_mismatch(unsigned long real_s, unsigned long read_s)
        : real_size(real_s), read_size(read_s) {}

    void print_self(std::ostream& os) const override {
        os << "expected " << real_size << " bits, read " << read_size;
    }
};

class Invalid_id : public Parse_error {
    int id;

public:
    explicit Invalid_id(int i) : id(i) {}

    void print_self(std::ostream& os) const override {
        os << "invalid codebook id " << id << ", try --inline-codebooks";
    }
};

#endif // ERRORS_H
