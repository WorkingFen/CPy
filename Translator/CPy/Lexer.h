#ifndef LEXER_H
#define LEXER_H

#include "Token.h"
#include <unordered_map>
#include <regex>

namespace Translator {
	class Lexer {
    private:
        bool new_line{false};
        std::string match{};
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

        bool is_new_line() const { return new_line; }
        void set_new_line(bool value) { new_line = value; }

		Token get_token() const { return token; }

        void get_next_token();

        bool match_str();
        bool match_con();
        bool match_hex();
        bool match_dbl();
        bool match_oct();
        bool match_dec();
        bool match_literals();
        bool match_line_comm();
        bool match_block_comm();
        bool match_graphic();

        bool odd_escaping(std::string& str, size_t start_pos);
        bool isword(int c) { return (isalnum(c) || c == '_'); }

        bool is_whitespace();
        bool is_comment();
	};
}

#endif
