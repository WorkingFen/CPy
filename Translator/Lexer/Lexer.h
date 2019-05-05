#include "Token.h"
#include <map>
#include <regex>

namespace Lexer {
	class Lexer
	{
		Source* source;
		Token token;
		char current_char;
		std::map<std::string, Type> map;

	public:
		Lexer(Source* source);

		Token get_token() { return token; }
		void get_next_token();
		void ignore_whitespaces();
		Source* get_source() { return source; }

		Token comment(bool type);
		Token get_literals_token();
		Token get_string_token();
		Token get_string_token(std::string, char);
		Token get_number_token();
		Token get_digit_token(bool type);
		Token get_other_token();
	};
}