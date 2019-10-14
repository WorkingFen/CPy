#ifndef LEXER_H
#define LEXER_H

#include "Token.h"
#include <unordered_map>
#include <regex>

namespace Translator {
	class Lexer {
		Source* source;
		Token token;
        std::unordered_map<std::string, Type> map{
            {"++", Type::INC_OP},
            {"--", Type::DEC_OP},
            {"<=", Type::LE_OP},
            {">=", Type::GE_OP},
            {"==", Type::EQ_OP},
            {"!=", Type::NE_OP},
            {"&&", Type::AND_OP},
            {"||", Type::OR_OP},
            {"*=", Type::MUL_ASSIGN},
            {"/=", Type::DIV_ASSIGN},
            {"%=", Type::MOD_ASSIGN},
            {"+=", Type::ADD_ASSIGN},
            {"-=", Type::SUB_ASSIGN},
            {"typedef", Type::TYPEDEF},
            {"char", Type::CHAR},
            {"short", Type::SHORT},
            {"int", Type::INT},
            {"long", Type::LONG},
            {"signed", Type::SIGNED},
            {"unsigned", Type::UNSIGNED},
            {"float", Type::FLOAT},
            {"double", Type::DOUBLE},
            {"void", Type::VOID},
            {"struct", Type::STRUCT},
            {"enum", Type::ENUM},
            {"case", Type::CASE},
            {"default", Type::DEFAULT},
            {"if", Type::IF},
            {"else", Type::ELSE},
            {"switch", Type::SWITCH},
            {"while", Type::WHILE},
            {"do", Type::DO},
            {"for", Type::FOR},
            {"break", Type::BREAK},
            {"return", Type::RETURN}
        };

	public:
		Lexer(Source*);

		Token get_token() const { return token; }
		void get_next_token();
		Source* get_source() const { return source; }
	};
}

#endif
