#include "Source.h"

Lexer::Source::Source(const std::string fileName) : is_beginning(true), new_line(false), after_comment(false) {
	file.open(fileName, std::ios::in);

	if(file.fail()) {
		std::cerr << "Couldn't open the file" << std::endl;
		exit(1);
	}
}

Lexer::Source::~Source() {
	file.close();
}

// Move to the previous char; operating on one line
int Lexer::Source::prev_char() {
	if (current_position.cn == 0) {
		return -1;
	}
	current_position.cn--;
	current_position.sn--;
	return 1;
}

// Move to next char
int Lexer::Source::next_char() {
	int result = 1;

	if(line.length() <= current_position.cn) {
		result = next_line();
		if(result == EOF) return EOF;
	}
	while(line.empty()) result = next_line();
	if(result > 0) {
		if(line[current_position.cn] == '\t') current_position.sn += 8 - (current_position.sn % 8);
		else current_position.sn++;
		return line[current_position.cn++];
	}
	else return EOF;
}

int Lexer::Source::next_line() {
	// Error while reading from file
	if(file.fail()) {
		std::cerr << "IO error" << std::endl;
		exit(1);
	}
	// check for the EOF
	else if(file.eof())	return EOF;

	std::getline(file, line);

	if(is_beginning) is_beginning = false;
	else {
		current_position.rn++;
		if(after_comment) after_comment = false;
		else new_line = true;
	}
	current_position.cn = 0;
	current_position.sn = 0;

	return current_position.rn;
}