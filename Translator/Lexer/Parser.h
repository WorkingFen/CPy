#ifndef PARSER_H
#define PARSER_H

#include "Token.h"
#include <vector>

namespace Translator {
	class Parser {
		std::vector<Token> tokens;

		bool primary_expression(int&);
		bool postfix_expression(int&, bool);
		bool argument_expression_list(int&);
		bool unary_expression(int&);
		bool unary_operator(int&);
		bool cast_expression(int&);
		bool multiplicative_expression(int&);
		bool additive_expression(int&);
		bool relational_expression(int&);
		bool equality_expression(int&);
		bool logical_and_expression(int&);
		bool logical_or_expression(int&);
		bool conditional_expression(int&);
		bool assignment_expression(int&);
		bool assignment_operator(int&);
		bool expression(int&);
		bool declaration(int&);
		bool declaration_specifiers(int&);
		bool init_declarator_list(int&);
		bool init_declarator(int&);
		bool type_specifier(int&);
		bool struct_or_union_specifier(int&);
		bool struct_declaration_list(int&);
		bool specifier_qualifier_list(int&);
		bool struct_declarator_list(int&);
		bool struct_declarator(int&);
		bool enum_specifier(int&);
		bool enumerator_list(int&);
		bool enumerator(int&);
		bool direct_declarator(int&, bool);
		bool parameter_list(int&);
		bool parameter_declaration(int&);
		bool identifier_list(int&);
		bool type_name(int&);
		bool direct_abstract_declarator(int&, bool);
		bool initializer(int&);
		bool initializer_list(int&);
		bool statement(int&);
		bool labeled_statement(int&);
		bool compound_statement(int&);
		bool declaration_list(int&);
		bool statement_list(int&);
		bool expression_statement(int&);
		bool selection_statement(int&);
		bool iteration_statement(int&);
		bool jump_statement(int&);
		bool translation_unit(int&);
		bool external_declaration(int&);
		bool function_definition(int&);

	public:
		Parser() {};
		~Parser() {};

		void push_back(Token value) { tokens.push_back(value); }
		std::vector<Token> get_tokens() { return tokens; }
		void start_parsing();
	};
}

#endif