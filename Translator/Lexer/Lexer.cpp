#include "Lexer.h"
#include <cctype>
#include <stdexcept>
#include <iostream>

Lexer::Lexer::Lexer(Source* source_) : source(source_) {
	source->next_char();
	map.insert(std::make_pair("++", Type::INC_OP));
	map.insert(std::make_pair("--", Type::DEC_OP));
	map.insert(std::make_pair("<=", Type::LE_OP));
	map.insert(std::make_pair(">=", Type::GE_OP));
	map.insert(std::make_pair("==", Type::EQ_OP));
	map.insert(std::make_pair("!=", Type::NE_OP));
	map.insert(std::make_pair("&&", Type::AND_OP));
	map.insert(std::make_pair("||", Type::OR_OP));
	map.insert(std::make_pair("*=", Type::MUL_ASSIGN));
	map.insert(std::make_pair("/=", Type::DIV_ASSIGN));
	map.insert(std::make_pair("%=", Type::MOD_ASSIGN));
	map.insert(std::make_pair("+=", Type::ADD_ASSIGN));
	map.insert(std::make_pair("-=", Type::SUB_ASSIGN));
	map.insert(std::make_pair("typedef", Type::TYPEDEF));
	map.insert(std::make_pair("char", Type::CHAR));
	map.insert(std::make_pair("short", Type::SHORT));
	map.insert(std::make_pair("int", Type::INT));
	map.insert(std::make_pair("long", Type::LONG));
	map.insert(std::make_pair("signed", Type::SIGNED));
	map.insert(std::make_pair("unsigned", Type::UNSIGNED));
	map.insert(std::make_pair("float", Type::FLOAT));
	map.insert(std::make_pair("double", Type::DOUBLE));
	map.insert(std::make_pair("void", Type::VOID));
	map.insert(std::make_pair("struct", Type::STRUCT));
	map.insert(std::make_pair("enum", Type::ENUM));
	map.insert(std::make_pair("case", Type::CASE));
	map.insert(std::make_pair("default", Type::DEFAULT));
	map.insert(std::make_pair("if", Type::IF));
	map.insert(std::make_pair("else", Type::ELSE));
	map.insert(std::make_pair("switch", Type::SWITCH));
	map.insert(std::make_pair("while", Type::WHILE));
	map.insert(std::make_pair("do", Type::DO));
	map.insert(std::make_pair("for", Type::FOR));
	map.insert(std::make_pair("break", Type::BREAK));
	map.insert(std::make_pair("return", Type::RETURN));
}

Lexer::Token Lexer::Lexer::get_token() {
	return token;
}

void Lexer::Lexer::get_next_token() {
	std::regex l("[[:alpha:]_]");
	std::regex d("[[:digit:]]");
	std::regex g("[[:graph:]]");

	ignore_whitespaces();

	std::string tmp = "";
	tmp += current_char;

	if(std::regex_match(tmp, l)) {																	// Get string or constant char that starts with L"", L'' or get Identifier
		if(current_char == 'L') {
			token = get_string_token();
		}
		else token = get_chars_token();		
	}
	else if(current_char == '\'' || current_char == '"') {											// Get string or constant char
		std::string x = "";
		token = get_string_token(x, current_char);
	}
	else if(current_char == '0') token = get_number_token();										// Get octal and hexal numbers OR zero
	else if(std::regex_match(tmp, d)) token = get_digit_token();									// Get digits
	else if(current_char == EOF) token.set_type(Type::eof);											// Get EOF
	else if(std::regex_match(tmp, g)) {
		if(current_char == '/') {
			char tmp;
			if((tmp = source->next_char()) == '/') token = comment(1);
			else if(tmp == '*') token = comment(0);
			else {
				source->prev_char();
				token = get_other_token();															// Get other tokens
			}
		}
		else
			token = get_other_token();																// Get other tokens
	}
	else token.set_type(Type::none);																// Unknown token
}

void Lexer::Lexer::ignore_whitespaces() {
	do{
		current_char = source->next_char();
#ifdef _DEBUG
		std::cout << (int)current_char << " '" << current_char << "' " << "symbol number: " << source->get_pos().sn << std::endl;
#endif
	}while(isspace(current_char));
}

// Type 0 is block "/*", type 1 is line "//"
Lexer::Token Lexer::Lexer::comment(bool type) {
	Token tmp;
	std::string x = "";
	if(type) source->next_line();
	else {
		char c, c1;
		do {
			while((c = source->next_char()) != '*' && c != 0);
		}while(((c1 = source->next_char()) != '/' && c != 0));
	}
	source->set_new_line(false);
	source->set_after_comment(true);
	tmp.set_chars(x);
	tmp.set_type(Type::comment);
	return tmp;
}

Lexer::Token Lexer::Lexer::get_chars_token() {
	std::regex d("[[:digit:]]");
	std::regex l("[[:alpha:]_]");

	Token tmp;
	std::string x = "";
	std::string text = "";

	do{
#ifdef _DEBUG
		std::cout << current_char;
#endif
		x += current_char;
		current_char = source->next_char();
		text = current_char;
	}while(std::regex_match(text, l) || std::regex_match(text, d));

	if(current_char != EOF) source->prev_char();

	tmp.set_chars(x);

	auto pair = map.find(x);
	if(pair != map.end()) {
		tmp.set_type(pair->second);
		return tmp;
	}

	tmp.set_type(Type::IDENTIFIER);
	return tmp;
}

Lexer::Token Lexer::Lexer::get_string_token() {
	std::string x = "";
	x += current_char;
	current_char = source->next_char();
	return get_string_token(x, current_char);
}

Lexer::Token Lexer::Lexer::get_string_token(std::string z, char curr) {
	std::regex p("[[:print:]]");

	Token tmp;
	std::string x = z;
	std::string text = "";

#ifdef _DEBUG
	std::cout << current_char;
#endif

	x += current_char;
	current_char = source->next_char();
	text = current_char;

	do{
#ifdef _DEBUG
		std::cout << current_char;
#endif
		if(current_char == '\\') {
			x += current_char;
			current_char = source->next_char();
		}
		else if(current_char == curr) {
			x += current_char;
			current_char = source->next_char();
			break;
		}
		x += current_char;
		current_char = source->next_char();
		text = current_char;
	}while(std::regex_match(text, p));

	if(current_char != EOF) source->prev_char();

	tmp.set_chars(x);

	if(curr == '\'') tmp.set_type(Type::CONSTANT);
	else if(curr == '"') tmp.set_type(Type::STRING_LITERAL);
	return tmp;
}

Lexer::Token Lexer::Lexer::get_number_token() {
	std::regex o("[0-7]");
	std::regex h("[[:xdigit:]]");
	std::regex xs("[xX]");

	Token tmp;
	std::string x = "";
	std::string text = "";

	x += current_char;
	current_char = source->next_char();
	text = current_char;

	if(std::regex_match(text, xs)) {
		text = current_char;
		do {
#ifdef _DEBUG
			std::cout << current_char;
#endif
			x += current_char;
			if(!std::regex_match(text, h) && current_char != ';') {
				tmp.set_type(Type::error);
				return tmp;
			}
			current_char = source->next_char();
			text = current_char;
		} while(std::regex_match(text, h));
	}
	else if(std::regex_match(text, o)) {
		text = current_char;
		do {
#ifdef _DEBUG
			std::cout << current_char;
#endif
			x += current_char;
			if(!std::regex_match(text, o) && current_char != ';') {
				tmp.set_type(Type::error);
				return tmp;
			}
			current_char = source->next_char();
			text = current_char;
		} while(std::regex_match(text, o));
	}

	if(current_char != EOF) source->prev_char();

	tmp.set_chars(x);
	tmp.set_type(Type::CONSTANT);
	return tmp;
}

Lexer::Token Lexer::Lexer::get_digit_token() {
	std::regex d("[[:digit:]]");

	Token tmp;
	std::string x = ""; 
	std::string text = "";
	text = current_char;

	do{
#ifdef _DEBUG
		std::cout << current_char;
#endif
		x += current_char;
		if(!std::regex_match(text, d)) {
			tmp.set_type(Type::error);
			return tmp;
		}
		current_char = source->next_char();
		text = current_char;
	}while(std::regex_match(text, d));

	if(current_char != EOF) source->prev_char();

	tmp.set_chars(x);

	tmp.set_type(Type::CONSTANT);
	return tmp;
}

Lexer::Token Lexer::Lexer::get_other_token() {
	std::regex p("[!%&*+-/<=>|]");

	Token tmp;
	std::string x = "";
	std::string text = "";

	text = current_char;

	if(std::regex_match(text, p)) {
		do {
#ifdef _DEBUG
			std::cout << current_char;
#endif
			if(current_char == '\\') current_char = source->next_char();
			x += current_char;
			current_char = source->next_char();
			text = current_char;
		}while(std::regex_match(text, p));
	}
	else {
#ifdef _DEBUG
		std::cout << current_char;
#endif
		if(current_char == '\\') current_char = source->next_char();
		x += current_char;
		current_char = source->next_char();
		text = current_char;
	}

	if(current_char != EOF) source->prev_char();

	tmp.set_chars(x);

	auto pair = map.find(x);
	if(pair != map.end()) {
		tmp.set_type(pair->second);
		return tmp;
	}

	tmp.set_type(Type::other);
	return tmp;
}