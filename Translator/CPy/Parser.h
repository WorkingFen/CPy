#ifndef PARSER_H
#define PARSER_H

#include "Token.h"
#include <stack>
#include <vector>
#include <exception>
#include <iostream>
#include <fstream>

namespace Translator {
	class Parser_Exception : public std::exception {
	private:
		std::string error_msg;

	public:
		Parser_Exception(std::string msg) : error_msg(msg) {}

		virtual const char* what() const throw() {
			return error_msg.c_str();
		}
	};

	/*class Match_Exception : public std::exception {
	private: 
		const char* func;
		std::string match_msg;
		
	public:
		Match_Exception(const char* f) : func(f) {
			match_msg = "X: ";
			match_msg += func;
		}

		virtual const char* what() const throw() {
			return match_msg.c_str();
		}
	};*/

	class Node {
	private:
        Token token{};
		const char* func_name = nullptr;
        int tabs{0};
        bool new_line{false};
        std::string translation{};
		std::vector<Node> children;

	public:
		Node() = default;
		Node(Token t, const char* f) : token(t), func_name(f) {}
		Node(Token t, const char* f, std::string tr) : token(t), func_name(f), translation(tr) {}
		Node(Token t, const char* f, std::string tr, int tab, bool nl) : token(t), func_name(f), translation(tr), tabs(tab), new_line(nl) {}

		void set_token(Token t) { token = t; }
		void push_back(const Node n) { children.push_back(n); }
		const char* get_func() { return func_name; }
		Type get_type() { return token.get_type(); }

		void read_node(int rec = 0) {
			if(token.get_type() == Type::parsing) {
				for(int i = 0; i < rec; i++) std::cout << "  ";
				std::cout << func_name << std::endl;
			}
			else {
				for(int i = 0; i < rec; i++) std::cout << "..";
				std::cout << token.get_chars() << std::endl;
			}

			for(auto node : children) node.read_node(rec + 1);
		}

        void translate(int& global_tab) {
            global_tab += tabs;

            std::cout << translation;
            if(new_line) {
                std::cout << std::endl;
                for(int i = 0; i < global_tab; i++)
                    std::cout << "\t";
            }

            for(auto node : children) node.translate(global_tab);
        }
	};

	class Parser {
        std::stack<int> offset;
		std::vector<Token> tokens;
		Node root;

		void func_log(const char*);

		Node primary_expression();
		Node postfix_expression(bool);
		Node argument_expression_list();
		Node unary_expression();
		Node unary_operator();
		Node cast_expression();
		Node multiplicative_expression();
		Node additive_expression();
		Node relational_expression();
		Node equality_expression();
		Node logical_and_expression();
		Node logical_or_expression();
		Node conditional_expression();
		Node assignment_expression();
		Node assignment_operator();
		Node expression();
		Node declaration();
		Node declaration_specifiers();
		Node init_declarator_list();
		Node init_declarator();
		Node type_specifier();
		Node struct_or_union_specifier();
		Node struct_declaration_list();
		Node specifier_qualifier_list();
		Node struct_declarator_list();
		Node struct_declarator();
		Node enum_specifier();
		Node enumerator_list();
		Node enumerator();
		Node direct_declarator(bool);
		Node parameter_list();
		Node parameter_declaration();
		Node identifier_list();
		Node type_name();
		Node direct_abstract_declarator(bool);
		Node initializer();
		Node initializer_list();
		Node statement();
		Node labeled_statement();
		Node compound_statement();
		Node declaration_list();
		Node statement_list();
		Node expression_statement();
		Node selection_statement();
		Node iteration_statement();
		Node jump_statement();
		Node translation_unit();
		Node external_declaration();
		Node function_definition();

        bool check_type(Node node, Type type = Type::none) { return node.get_type() == type; }
        bool check_type(int token_num, Type type) { return tokens[token_num].get_type() == type; }
        bool check_char(int token_num, std::string ch) { return tokens[token_num].get_chars() == ch; }

        void prepare() { offset.emplace(offset.top()); }
        void commit(int inc);

	public:
		Parser() = default;

		void push_back(Token value) { tokens.push_back(value); }
		void start_parsing();
		Node get_root() { return root; }
	};
}

#endif
