#include "Source.h"
#include <string>

namespace Lexer {
	enum class Type
	{
		IDENTIFIER,
		CONSTANT,
		STRING_LITERAL,
		TYPE_NAME,
		INC_OP,
		DEC_OP,
		LE_OP,
		GE_OP,
		EQ_OP,
		NE_OP,
		AND_OP,
		OR_OP,
		MUL_ASSIGN,
		DIV_ASSIGN,
		MOD_ASSIGN,
		ADD_ASSIGN,
		SUB_ASSIGN,
		TYPEDEF,
		CHAR,
		SHORT,
		INT,
		LONG,
		SIGNED,
		UNSGINED,
		FLOAT,
		DOUBLE,
		VOID,
		STRUCT,
		ENUM,
		CASE,
		DEFAULT,
		IF,
		ELSE,
		SWITCH,
		WHILE,
		DO,
		FOR,
		BREAK,
		RETURN,
		other,
		none,
		eof,
		error
	};

	class Token
	{
		Type type;

		std::string chars;
		char function[100];

	public:
		Token() : type(Type::eof) {}

		Type get_type() const { return type; }

		void set_type(Type setType) {
			type = setType;
		}

		void set_chars(std::string characters) {
			chars = characters;
		}

		std::string get_chars() {
			return chars;
		}
	};
}