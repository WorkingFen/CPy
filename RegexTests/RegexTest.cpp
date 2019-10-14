#include "pch.h"
#include "RegexTest.h"

std::smatch RegexTest::regex_word(const std::string& line) {
	std::smatch sm;
	std::regex word("(.*?)(\\s|$)");

	std::regex_search(line, sm, word);
	return sm;
}

std::smatch RegexTest::regex_str(const std::string& line) {
	std::smatch sm;
	std::regex str("(L?\".*\")(.*)");

	std::regex_search(line, sm, str);
	return sm;
}

std::smatch RegexTest::regex_con(const std::string& line) {
	std::smatch sm;
	std::regex con("(L?'(.*|\\n)')(.*)");

	std::regex_search(line, sm, con);
	return sm;
}

std::smatch RegexTest::regex_hex(const std::string& line) {
	std::smatch sm;
	std::regex hex("(0[xX][[:xdigit:]]+)(.*)");

	std::regex_search(line, sm, hex);
	return sm;
}

std::smatch RegexTest::regex_dbl(const std::string& line) {
	std::smatch sm;
	std::regex dbl("(\\d+\\.\\d*|\\d*\\.\\d+)(.*)");

	std::regex_search(line, sm, dbl);
	return sm;
}

std::smatch RegexTest::regex_oct(const std::string& line) {
	std::smatch sm;
	std::regex oct("(0[0-7]+)(.*)");

	std::regex_search(line, sm, oct);
	return sm;
}

std::smatch RegexTest::regex_dec(const std::string& line) {
	std::smatch sm;
	std::regex dec("(0|\\d+)(.*)");

	std::regex_search(line, sm, dec);
	return sm;
}

std::smatch RegexTest::regex_literals(const std::string& line) {
	std::smatch sm;
	std::regex literals("([^\\W\\d]\\w*)(.*)");

	std::regex_search(line, sm, literals);
	return sm;
}

std::smatch RegexTest::regex_line_comm(const std::string& line) {
	std::smatch sm;
	std::regex line_comm("\\/\\/.*");

	std::regex_search(line, sm, line_comm);
	return sm;
}

std::smatch RegexTest::regex_block_comm(const std::string& line) {
	std::smatch sm;
	std::smatch wm;
	std::regex end_block("(\\/\\*.*\\*\\/)(.*)");

	std::regex_search(line, wm, end_block);
	return wm;
}

std::smatch RegexTest::regex_graphic(const std::string& line) {
	std::smatch sm;
	std::regex graphic("([()[\\]{},;.:?]|[-+&|]{2}|[-+=<>!%*\\/]=?)(.*)");

	std::regex_search(line, sm, graphic);
	return sm;
}
