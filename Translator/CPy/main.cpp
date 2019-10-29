#include <iostream>
#include <sstream>
#include <unordered_map>
#include "Lexer.h"
#include "Parser.h"

void cout_type(Translator::Token token) {
    std::unordered_map<Translator::Type, std::string> map{
        {Translator::Type::INC_OP, "'++'"},
        {Translator::Type::DEC_OP, "'--'"},
        {Translator::Type::LE_OP, "'<='"},
        {Translator::Type::GE_OP, "'>='"},
        {Translator::Type::EQ_OP, "'=='"},
        {Translator::Type::NE_OP, "'!='"},
        {Translator::Type::AND_OP, "'&&'"},
        {Translator::Type::OR_OP, "'||'"},
        {Translator::Type::MUL_ASSIGN, "'*='"},
        {Translator::Type::DIV_ASSIGN, "'/='"},
        {Translator::Type::MOD_ASSIGN, "'%='"},
        {Translator::Type::ADD_ASSIGN, "'+='"},
        {Translator::Type::SUB_ASSIGN, "'-='"},
        {Translator::Type::TYPEDEF, "TYPEDEF"},
        {Translator::Type::CHAR, "CHAR"},
        {Translator::Type::SHORT, "SHORT"},
        {Translator::Type::INT, "INT"},
        {Translator::Type::LONG, "LONG"},
        {Translator::Type::SIGNED, "SIGNED"},
        {Translator::Type::UNSIGNED, "UNSIGNED"},
        {Translator::Type::FLOAT, "FLOAT"},
        {Translator::Type::DOUBLE, "DOUBLE"},
        {Translator::Type::VOID, "VOID"},
        {Translator::Type::STRUCT, "STRUCT"},
        {Translator::Type::ENUM, "ENUM"},
        {Translator::Type::CASE, "CASE"},
        {Translator::Type::DEFAULT, "DEFAULT"},
        {Translator::Type::IF, "IF"},
        {Translator::Type::ELSE, "ELSE"},
        {Translator::Type::SWITCH, "SWITCH"},
        {Translator::Type::WHILE, "WHILE"},
        {Translator::Type::DO, "DO"},
        {Translator::Type::FOR, "FOR"},
        {Translator::Type::BREAK, "BREAK"},
        {Translator::Type::RETURN, "RETURN"},
        {Translator::Type::CONSTANT, "CONSTANT"},
        {Translator::Type::IDENTIFIER, "IDENTIFIER"},
        {Translator::Type::STRING_LITERAL, "STRING_LITERAL"},
        {Translator::Type::eof, "EOF"},
        {Translator::Type::error, "ERROR"},
        {Translator::Type::none, "UNKNOWN"}
    };

    if(map[token.get_type()] != "")
        std::cout << map[token.get_type()];
    else
        std::cout << "'" << token.get_chars() << "'";

}

int main(int argc, char* argv[]) {
	if(argc < 2) {
		std::cerr << "You should give a proper text file";
		return -1;
	}

	std::string fileName(argv[1]);
	Translator::Source source(fileName);
	Translator::Lexer scan(&source);
	//Translator::Parser parser;
	Translator::Token token(Translator::Type::none);

	std::cout << "##### LEXER #####" << std::endl << std::endl;

	while(token.get_type() != Translator::Type::eof) {
		scan.get_next_token();
		token = scan.get_token();
		cout_type(token);
		if(token.get_type() != Translator::Type::eof) 
            std::cout << ", ";
		if(scan.is_new_line()) {
			std::cout << std::endl;
			scan.set_new_line(false);
		}
		//parser.push_back(token);
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

	/*std::cout << std::endl << std::endl << "##### PARSER #####" << std::endl;

	parser.start_parsing();
	std::cout << std::endl << std::endl << "### Drzewo skladniowe ###" << std::endl << std::endl;
	parser.get_root().read_node(0);*/

	return 0;
}
