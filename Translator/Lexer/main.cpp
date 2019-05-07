#include <iostream>
#include <sstream>
#include "Lexer.h"
#include "Semantics.h"

void cout_type(Lexer::Token token) {
	switch(token.get_type()) {
		case Lexer::Type::ADD_ASSIGN:
			std::cout << "'+='";
			break;
		case Lexer::Type::AND_OP:
			std::cout << "'&&'";
			break;
		case Lexer::Type::BREAK:
			std::cout << "BREAK";
			break;
		case Lexer::Type::CASE:
			std::cout << "CASE";
			break;
		case Lexer::Type::CHAR:
			std::cout << "CHAR";
			break;
		case Lexer::Type::CONSTANT:
			std::cout << "CONSTANT";
			break;
		case Lexer::Type::DEC_OP:
			std::cout << "'--'";
			break;
		case Lexer::Type::DEFAULT:
			std::cout << "DEFAULT";
			break;
		case Lexer::Type::DIV_ASSIGN:
			std::cout << "'/='";
			break;
		case Lexer::Type::DO:
			std::cout << "DO";
			break;
		case Lexer::Type::DOUBLE:
			std::cout << "DOUBLE";
			break;
		case Lexer::Type::ELSE:
			std::cout << "ELSE";
			break;
		case Lexer::Type::ENUM:
			std::cout << "ENUM";
			break;
		case Lexer::Type::eof:
			std::cout << "EOF";
			break;
		case Lexer::Type::EQ_OP:
			std::cout << "'=='";
			break;
		case Lexer::Type::error:
			std::cout << "ERROR";
			break;
		case Lexer::Type::FLOAT:
			std::cout << "FLOAT";
			break;
		case Lexer::Type::FOR:
			std::cout << "FOR";
			break;
		case Lexer::Type::GE_OP:
			std::cout << "'>='";
			break;
		case Lexer::Type::IDENTIFIER:
			std::cout << "IDENTIFIER";
			break;
		case Lexer::Type::IF:
			std::cout << "IF";
			break;
		case Lexer::Type::INC_OP:
			std::cout << "'++'";
			break;
		case Lexer::Type::INT:
			std::cout << "INT";
			break;
		case Lexer::Type::LE_OP:
			std::cout << "'<='";
			break;
		case Lexer::Type::LONG:
			std::cout << "LONG";
			break;
		case Lexer::Type::MOD_ASSIGN:
			std::cout << "'%='";
			break;
		case Lexer::Type::MUL_ASSIGN:
			std::cout << "'*='";
			break;
		case Lexer::Type::NE_OP:
			std::cout << "'!='";
			break;
		case Lexer::Type::none:
			std::cout << "UNKNOWN";
			break;
		case Lexer::Type::OR_OP:
			std::cout << "'||'";
			break;
		case Lexer::Type::RETURN:
			std::cout << "RETURN";
			break;
		case Lexer::Type::SHORT:
			std::cout << "SHORT";
			break;
		case Lexer::Type::SIGNED:
			std::cout << "SIGNED";
			break;
		case Lexer::Type::STRING_LITERAL:
			std::cout << "STRING_LITERAL";
			break;
		case Lexer::Type::STRUCT:
			std::cout << "STRUCT";
			break;
		case Lexer::Type::SUB_ASSIGN:
			std::cout << "'-='";
			break;
		case Lexer::Type::SWITCH:
			std::cout << "SWITCH";
			break;
		case Lexer::Type::TYPEDEF:
			std::cout << "TYPEDEF";
			break;
		case Lexer::Type::UNSIGNED:
			std::cout << "UNSIGNED";
			break;
		case Lexer::Type::VOID:
			std::cout << "VOID";
			break;
		case Lexer::Type::WHILE:
			std::cout << "WHILE";
			break;
		default:
			std::cout << "'" << token.get_chars() << "'";
			break;
	}
}

int main(int argc, char* argv[]) {
	if(argc < 2) {
		std::cerr << "You should give a proper .txt file";
		return -1;
	}

	std::string fileName = argv[1];
	Lexer::Source source(fileName);
	Lexer::Lexer scan(&source);
	Lexer::Semantics semantics;
	Lexer::Token token;
	token.set_type(Lexer::Type::none);

	while(token.get_type() != Lexer::Type::eof) {
		scan.get_next_token();
		token = scan.get_token();
#ifdef _DEBUG
		std::cout << scan.get_source()->is_new_line();
#endif
		if(token.get_type() != Lexer::Type::comment) {
			cout_type(token);
			if(token.get_type() != Lexer::Type::eof) std::cout << ", ";
			if(scan.get_source()->is_new_line()) {
				std::cout << std::endl;
				scan.get_source()->set_new_line(false);
			}
			semantics.push_back(token);
		}
	}

	for(auto i : semantics.get_tokens()) {
		std::cout << std::endl << "________________" << std::endl;
		cout_type(i);
	}
	// Check semantics here

	return 0;
}