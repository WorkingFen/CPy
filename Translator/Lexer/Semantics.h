#ifndef SEMANTICS_H
#define SEMANTICS_H

#include "Token.h"
#include <vector>

namespace Lexer {
	class Semantics {
		std::vector<Token> tokens;

	public:
		Semantics() {};
		~Semantics() {};

		void push_back(Token value) { tokens.push_back(value); }
		std::vector<Token> get_tokens() { return tokens; }
	};

	bool primary_expression(Token);
	bool postfix_expression(Token);
	bool argument_expression_list(Token);
	bool unary_expression(Token);
	bool unary_operator(Token);
	bool cast_expression(Token);
	bool multiplicative_expression(Token);
	bool additive_expression(Token);
	bool relational_expression(Token);
	bool equality_expression(Token);
	bool logical_and_expression(Token);
	bool logical_or_expression(Token);
	bool conditional_expression(Token);
	bool assignment_expression(Token);
	bool assignment_operator(Token);
	bool expression(Token);
	bool declaration(Token);
	bool declaration_specifiers(Token);
	bool init_declarator_list(Token);
	bool init_declarator(Token);
	bool type_specifier(Token);
	bool struct_or_union_specifier(Token);
	bool struct_declaration_list(Token);
	bool specifier_qualifier_list(Token);
	bool struct_declarator_list(Token);
	bool struct_declarator(Token);
	bool enum_specifier(Token);
	bool enumerator_list(Token);
	bool enumerator(Token);
	bool direct_declarator(Token);
	bool parameter_list(Token);
	bool parameter_declaration(Token);
	bool identifier_list(Token);
	bool type_name(Token);
	bool direct_abstract_declarator(Token);
	bool initializer(Token);
	bool initializer_list(Token);
	bool statement(Token);
	bool labeled_statement(Token);
	bool compound_statement(Token);
	bool declaration_list(Token);
	bool statement_list(Token);
	bool expression_statement(Token);
	bool selection_statement(Token);
	bool iteration_statement(Token);
	bool jump_statement(Token);
	bool translation_unit(Token);
	bool external_declaration(Token);
	bool function_definition(Token);
}

#endif