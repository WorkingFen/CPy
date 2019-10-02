#ifndef PARSER_H
#define PARSER_H

#include "Token.h"
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
			Token token;
			const char* func_name;
			std::vector<Node> children;
		public:
			Node() = default;
			Node(Token t, const char* f) : token(t), func_name(f) {}

			void set_token(Token t) { token = t; }
			void push_back(const Node n) { children.push_back(n); }
			const char* get_func() { return func_name; }
			Type get_type() { return token.get_type(); }

			void read_node(int rec) {
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
	};

	/*class Tokenizer {
		private:
			int& t_val;
			int t_new;

		public:
			Tokenizer(int& t): t_val(t), t_new(t) {}

			void inc(int i) { t_new += i; }
			void commit() { t_val = t_new; }
	};*/

	class Parser {
		std::vector<Token> tokens;
		Node root;
		bool end = false;

		void func_log(int, const char*);
		/*bool is_curr_token(int, std::string);
		bool is_curr_token(int, Type);*/

		Node primary_expression(int&);
		Node postfix_expression(int&, bool);
		Node argument_expression_list(int&);
		Node unary_expression(int&);
		Node unary_operator(int&);
		Node cast_expression(int&);
		Node multiplicative_expression(int&);
		Node additive_expression(int&);
		Node relational_expression(int&);
		Node equality_expression(int&);
		Node logical_and_expression(int&);
		Node logical_or_expression(int&);
		Node conditional_expression(int&);
		Node assignment_expression(int&);
		Node assignment_operator(int&);
		Node expression(int&);
		Node declaration(int&);
		Node declaration_specifiers(int&);
		Node init_declarator_list(int&);
		Node init_declarator(int&);
		Node type_specifier(int&);
		Node struct_or_union_specifier(int&);
		Node struct_declaration_list(int&);
		Node specifier_qualifier_list(int&);
		Node struct_declarator_list(int&);
		Node struct_declarator(int&);
		Node enum_specifier(int&);
		Node enumerator_list(int&);
		Node enumerator(int&);
		Node direct_declarator(int&, bool);
		Node parameter_list(int&);
		Node parameter_declaration(int&);
		Node identifier_list(int&);
		Node type_name(int&);
		Node direct_abstract_declarator(int&, bool);
		Node initializer(int&);
		Node initializer_list(int&);
		Node statement(int&);
		Node labeled_statement(int&);
		Node compound_statement(int&);
		Node declaration_list(int&);
		Node statement_list(int&);
		Node expression_statement(int&);
		Node selection_statement(int&);
		Node iteration_statement(int&);
		Node jump_statement(int&);
		Node translation_unit(int);
		Node external_declaration(int&);
		Node function_definition(int&);

	public:
		Parser() = default;

		void push_back(Token value) { tokens.push_back(value); }
		const std::vector<Token> get_tokens() const { return tokens; }
		void start_parsing();
		Node get_root() { return root; }
	};
}

#endif
