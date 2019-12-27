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

	std::string streamName(argv[1]);
	Translator::Source<std::ifstream> source(streamName);
	Translator::Lexer scan(&source);
	Translator::Parser parser;
	Translator::Token token;

	std::cout << "##### LEXER #####" << std::endl << std::endl;
	while(token.get_type() != Translator::Type::eof) {
		scan.get_next_token();
		token = scan.get_token();
        if(scan.is_new_line()) {
            std::cout << std::endl;
            scan.set_new_line(false);
        }
		cout_type(token);
		if(token.get_type() != Translator::Type::eof) 
            std::cout << ", ";
		parser.push_back(token);
	}

#ifdef _DEBUG
	for(auto i : parser.get_tokens()) {
		std::cout << std::endl << "___" << i.get_chars() << "___: ";
		cout_type(i);
	}
#endif

	std::cout << std::endl << std::endl << "##### PARSER #####" << std::endl;
	parser.start_parsing();

	std::cout << std::endl << std::endl << "### Drzewo skladniowe ###" << std::endl << std::endl;
	parser.get_root().read_node();

	return 0;
}
