#pragma once
#include <regex>

class ReadingTest {
    int pos{0};
public:
    bool match_str(const std::string& line, std::string& match);
    bool match_con(const std::string& line, std::string& match);
    bool match_hex(const std::string& line, std::string& match);
    bool match_dbl(const std::string& line, std::string& match);
    bool match_oct(const std::string& line, std::string& match);
    bool match_dec(const std::string& line, std::string& match);
    bool match_literals(const std::string& line, std::string& match);
    bool match_line_comm(const std::string& line);
    bool match_block_comm(const std::string& line);
    bool match_graphic(const std::string& line, std::string& match);

    int peek_file(const std::string& line) { return (pos < line.length() ? line.at(pos) : ' '); }
    int get_file(const std::string& line) { return (pos < line.length() ? line.at(pos++) : ' '); }
    void unget_file() { pos--; }
    void seekg_file(int _pos) { pos = _pos; }
    int tellg_file() { return pos; }

    bool odd_escaping(std::string& str, size_t start_pos);
    bool isword(int _C) { return (isalnum(_C) || _C == '_'); }
};
