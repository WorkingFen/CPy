#pragma once
#include <regex>

class RegexTest {
public:
	std::smatch regex_word(const std::string& line);
	std::smatch regex_str(const std::string& line);
	std::smatch regex_con(const std::string& line);
	std::smatch regex_hex(const std::string& line);
	std::smatch regex_dbl(const std::string& line);
	std::smatch regex_oct(const std::string& line);
	std::smatch regex_dec(const std::string& line);
	std::smatch regex_literals(const std::string& line);
	std::smatch regex_line_comm(const std::string& line);
	std::smatch regex_block_comm(const std::string& line);
	std::smatch regex_graphic(const std::string& line);

};
