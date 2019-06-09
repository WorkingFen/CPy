#include <iostream>
#include <sstream>
#include "Lexer.h"
#include "Parser.h"

void cout_type(Translator::Token token) {
	switch(token.get_type()) {
		case Translator::Type::ADD_ASSIGN:
			std::cout << "'+='";
			break;
		case Translator::Type::AND_OP:
			std::cout << "'&&'";
			break;
		case Translator::Type::BREAK:
			std::cout << "BREAK";
			break;
		case Translator::Type::CASE:
			std::cout << "CASE";
			break;
		case Translator::Type::CHAR:
			std::cout << "CHAR";
			break;
		case Translator::Type::CONSTANT:
			std::cout << "CONSTANT";
			break;
		case Translator::Type::DEC_OP:
			std::cout << "'--'";
			break;
		case Translator::Type::DEFAULT:
			std::cout << "DEFAULT";
			break;
		case Translator::Type::DIV_ASSIGN:
			std::cout << "'/='";
			break;
		case Translator::Type::DO:
			std::cout << "DO";
			break;
		case Translator::Type::DOUBLE:
			std::cout << "DOUBLE";
			break;
		case Translator::Type::ELSE:
			std::cout << "ELSE";
			break;
		case Translator::Type::ENUM:
			std::cout << "ENUM";
			break;
		case Translator::Type::eof:
			std::cout << "EOF";
			break;
		case Translator::Type::EQ_OP:
			std::cout << "'=='";
			break;
		case Translator::Type::error:
			std::cout << "ERROR";
			break;
		case Translator::Type::FLOAT:
			std::cout << "FLOAT";
			break;
		case Translator::Type::FOR:
			std::cout << "FOR";
			break;
		case Translator::Type::GE_OP:
			std::cout << "'>='";
			break;
		case Translator::Type::IDENTIFIER:
			std::cout << "IDENTIFIER";
			break;
		case Translator::Type::IF:
			std::cout << "IF";
			break;
		case Translator::Type::INC_OP:
			std::cout << "'++'";
			break;
		case Translator::Type::INT:
			std::cout << "INT";
			break;
		case Translator::Type::LE_OP:
			std::cout << "'<='";
			break;
		case Translator::Type::LONG:
			std::cout << "LONG";
			break;
		case Translator::Type::MOD_ASSIGN:
			std::cout << "'%='";
			break;
		case Translator::Type::MUL_ASSIGN:
			std::cout << "'*='";
			break;
		case Translator::Type::NE_OP:
			std::cout << "'!='";
			break;
		case Translator::Type::none:
			std::cout << "UNKNOWN";
			break;
		case Translator::Type::OR_OP:
			std::cout << "'||'";
			break;
		case Translator::Type::RETURN:
			std::cout << "RETURN";
			break;
		case Translator::Type::SHORT:
			std::cout << "SHORT";
			break;
		case Translator::Type::SIGNED:
			std::cout << "SIGNED";
			break;
		case Translator::Type::STRING_LITERAL:
			std::cout << "STRING_LITERAL";
			break;
		case Translator::Type::STRUCT:
			std::cout << "STRUCT";
			break;
		case Translator::Type::SUB_ASSIGN:
			std::cout << "'-='";
			break;
		case Translator::Type::SWITCH:
			std::cout << "SWITCH";
			break;
		case Translator::Type::TYPEDEF:
			std::cout << "TYPEDEF";
			break;
		case Translator::Type::UNSIGNED:
			std::cout << "UNSIGNED";
			break;
		case Translator::Type::VOID:
			std::cout << "VOID";
			break;
		case Translator::Type::WHILE:
			std::cout << "WHILE";
			break;
		default:
			std::cout << "'" << token.get_chars() << "'";
			break;
	}
}

int main(int argc, char* argv[]) {
	if(argc < 2) {
		std::cerr << "You should give a proper text file";
		return -1;
	}

	std::string fileName = argv[1];
	Translator::Source source(fileName);
	Translator::Lexer scan(&source);
	Translator::Parser parser;
	Translator::Token token;
	token.set_type(Translator::Type::none);

	std::cout << "##### LEXER #####" << std::endl << std::endl;

	while(token.get_type() != Translator::Type::eof) {
		scan.get_next_token();
		token = scan.get_token();
#ifdef _DEBUG
		std::cout << scan.get_source()->is_new_line();
#endif
		if(token.get_type() != Translator::Type::comment) {
			cout_type(token);
			if(token.get_type() != Translator::Type::eof) std::cout << ", ";
			if(scan.get_source()->is_new_line()) {
				std::cout << std::endl;
				scan.get_source()->set_new_line(false);
			}
			parser.push_back(token);
		}
	}

	/*for(auto i : parser.get_tokens()) {
		if(i.get_type() != Translator::Type::eof) {
			std::cout << std::endl << "____" << i.get_chars() << "______" << std::endl;
		}
		else {
			std::cout << std::endl << "__________" << std::endl;
		}
		cout_type(i);
	}*/
	// Check semantics here

	std::cout << std::endl << std::endl << "##### PARSER #####" << std::endl;

	parser.start_parsing();
	std::cout << std::endl << std::endl << "### Drzewo skladniowe ###" << std::endl << std::endl;
	parser.get_root().read_node(0);

	return 0;
}