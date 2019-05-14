#include "Parser.h"

void Translator::Parser::start_parsing() {
	int token_number = 0;

	std::cout << std::endl << __func__;

	if(!translation_unit(token_number)) exit(-1);					// It will be an error

}

bool Translator::Parser::primary_expression(int& curr_token) {
	int token_number = curr_token;
	bool found = false;

	std::cout << std::endl << token_number << ": " << tokens[token_number].get_chars() <<  " -> " << __func__;

	if(tokens[token_number].get_type() == Type::IDENTIFIER)  found = true;
	else if(tokens[token_number].get_type() == Type::CONSTANT) found = true;
	else if(tokens[token_number].get_type() == Type::STRING_LITERAL) found = true;
	else if(tokens[token_number].get_chars() == "(") {
		if(expression(++token_number)) {
			if(tokens[token_number].get_chars() == ")") found = true;
			else {}													// It will be an error
		}
		else {}														// It will be an error
	}

	if(found) {
		curr_token = token_number + 1;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else {
		std::cout << std::endl << "X: " << __func__;
		return false;												// It will be an error (?)
	}
}

bool Translator::Parser::postfix_expression(int& curr_token, bool recursive) {			// Recursive
	int token_number = curr_token;
	bool found = false;

	std::cout << std::endl << token_number << ": " << tokens[token_number].get_chars() <<  " -> " << __func__;

	if(!recursive) {
		if(primary_expression(token_number)) {
			if(postfix_expression(token_number, true)) found = true;
			found = true;
		}
	}
	else {
		if(tokens[token_number].get_chars() == "(") {
			if(tokens[token_number + 1].get_chars() == ")") {
				if(postfix_expression(token_number += 2, true)) found = true;
				found = true;
			}
			else if(argument_expression_list(++token_number)) {
				if(tokens[token_number].get_chars() == ")") {
					if(postfix_expression(++token_number, true)) found = true;
					found = true;
				}
				else {}																	// It will be an error
			}
			else {}																		// It will be an error
		}
		else if(tokens[token_number].get_chars() == ".") {
			if(tokens[token_number + 1].get_type() == Type::IDENTIFIER) {
				if(postfix_expression(token_number += 2, true)) found = true;
				found = true;
			}
			else {}																		// It will be an error
		}
		else if(tokens[token_number].get_type() == Type::INC_OP) {
			if(postfix_expression(++token_number, true)) found = true;
			found = true;
		}
		else if(tokens[token_number].get_type() == Type::DEC_OP) {
			if(postfix_expression(++token_number, true)) found = true;
			found = true;
		}
		else if(tokens[token_number].get_chars() == "[") {
			if(expression(++token_number)) {
				if(tokens[token_number].get_chars() == "]") {
					if(postfix_expression(++token_number, true)) found = true;
					found = true;
				}
				else {}																	// It will be an error
			}
			else {}																		// It will be an error
		}
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else {
		std::cout << std::endl << "X: " << __func__;
		return false;
	}
}

bool Translator::Parser::argument_expression_list(int& curr_token) {	// Recursive
	int token_number = curr_token;
	bool found = false;

	std::cout << std::endl << token_number << ": " << tokens[token_number].get_chars() <<  " -> " << __func__;

	if(assignment_expression(token_number)) { 
		if(tokens[token_number].get_chars() == ",") {
			if(argument_expression_list(++token_number)) found = true;
			else {}																	// It will be an error
		}
		found = true; 
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else {
		std::cout << std::endl << "X: " << __func__;
		return false;																// It will be an error (?)
	}
}

bool Translator::Parser::unary_expression(int& curr_token) {				// Recursive
	int token_number = curr_token;
	bool found = false;

	std::cout << std::endl << token_number << ": " << tokens[token_number].get_chars() <<  " -> " << __func__;

	if(postfix_expression(token_number, false)) found = true;
	else if(tokens[token_number].get_type() == Type::INC_OP || tokens[token_number].get_type() == Type::DEC_OP) {
		if(unary_expression(++token_number)) found = true;
		else {}																// It will be an error
	}
	else if(unary_operator(token_number)) {
		if(cast_expression(token_number)) found = true;
		else {}																// It will be an error
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else {
		std::cout << std::endl << "X: " << __func__;
		return false;														// It will be an error (?)
	}
}

bool Translator::Parser::unary_operator(int& curr_token) {
	int token_number = curr_token;
	bool found = false;

	std::cout << std::endl << token_number << ": " << tokens[token_number].get_chars() <<  " -> " << __func__;

	if(tokens[token_number].get_chars() == "*") found = true;
	else if(tokens[token_number].get_chars() == "+") found = true;
	else if(tokens[token_number].get_chars() == "-") found = true;
	else if(tokens[token_number].get_chars() == "!") found = true;

	if(found) {
		curr_token++;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else {
		std::cout << std::endl << "X: " << __func__;
		return false;														// It will be an error (?)
	}
}

bool Translator::Parser::cast_expression(int& curr_token) {					// Recursive
	int token_number = curr_token;
	bool found = false;

	std::cout << std::endl << token_number << ": " << tokens[token_number].get_chars() <<  " -> " << __func__;

	if(unary_expression(token_number)) found = true;
	else if(tokens[token_number].get_chars() == "(") {
		if(type_name(++token_number)) {
			if(tokens[token_number].get_chars() == ")") {
				if(cast_expression(++token_number)) found = true;
				else {}																// It will be an error
			}
			else {}																	// It will be an error
		}
		else {}																		// It will be an error
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else {
		std::cout << std::endl << "X: " << __func__;
		return false;																// It will be an error (?)
	}
}

bool Translator::Parser::multiplicative_expression(int& curr_token) {		// Recursive
	int token_number = curr_token;
	bool found = false;

	std::cout << std::endl << token_number << ": " << tokens[token_number].get_chars() <<  " -> " << __func__;

	if(cast_expression(token_number)) {
		if(tokens[token_number].get_chars() == "*" ||
			tokens[token_number].get_chars() == "/" ||
			tokens[token_number].get_chars() == "%") {
			if(multiplicative_expression(++token_number)) found = true;
			else {}																		// It will be an error
		}
		found = true; 
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else {
		std::cout << std::endl << "X: " << __func__;
		return false;
	}
}

bool Translator::Parser::additive_expression(int& curr_token) {				// Recursive
	int token_number = curr_token;
	bool found = false;

	std::cout << std::endl << token_number << ": " << tokens[token_number].get_chars() <<  " -> " << __func__;

	if(multiplicative_expression(token_number)) { 
		if(tokens[token_number].get_chars() == "+" || tokens[token_number].get_chars() == "-") {
			if(additive_expression(++token_number)) found = true;
			else {}																	// It will be an error
		}
		found = true; 
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else {
		std::cout << std::endl << "X: " << __func__;
		return false;																// It will be an error (?)
	}
}

bool Translator::Parser::relational_expression(int& curr_token) {			// Recursive
	int token_number = curr_token;
	bool found = false;

	std::cout << std::endl << token_number << ": " << tokens[token_number].get_chars() <<  " -> " << __func__;

	if(additive_expression(token_number)) { 
		if(tokens[token_number].get_chars() == "<" ||
			tokens[token_number].get_chars() == ">" ||
			tokens[token_number].get_type() == Type::LE_OP ||
			tokens[token_number].get_type() == Type::GE_OP) {
			if(relational_expression(++token_number)) found = true;
			else {}																// It will be an error
		}
		found = true; 
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else {
		std::cout << std::endl << "X: " << __func__;
		return false;															// It will be an error (?)
	}
}

bool Translator::Parser::equality_expression(int& curr_token) {				// Recursive
	int token_number = curr_token;
	bool found = false;

	std::cout << std::endl << token_number << ": " << tokens[token_number].get_chars() <<  " -> " << __func__;

	if(relational_expression(token_number)) { 
		if(tokens[token_number].get_type() == Type::EQ_OP || tokens[token_number].get_type() == Type::NE_OP) {
			if(equality_expression(++token_number)) found = true;
			else {}																// It will be an error
		}
		found = true; 
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else {
		std::cout << std::endl << "X: " << __func__;
		return false;															// It will be an error (?)
	}
}

bool Translator::Parser::logical_and_expression(int& curr_token) {			// Recursive
	int token_number = curr_token;
	bool found = false;

	std::cout << std::endl << token_number << ": " << tokens[token_number].get_chars() <<  " -> " << __func__;

	if(equality_expression(token_number)) {
		if(tokens[token_number].get_type() == Type::AND_OP) {
			if(logical_and_expression(++token_number)) found = true;
			else {}																			// It will be an error
		}
		found = true; 
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else {
		std::cout << std::endl << "X: " << __func__;
		return false;																		// It will be an error (?)
	}
}

bool Translator::Parser::logical_or_expression(int& curr_token) {			// Recursive
	int token_number = curr_token;
	bool found = false;

	std::cout << std::endl << token_number << ": " << tokens[token_number].get_chars() <<  " -> " << __func__;

	if(logical_and_expression(token_number)) {
		if(tokens[token_number].get_type() == Type::OR_OP) {
			if(logical_or_expression(++token_number)) found = true;
			else {}															// It will be an error
		}
		found = true; 
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else {
		std::cout << std::endl << "X: " << __func__;
		return false;														// It will be an error (?)
	}
}

bool Translator::Parser::conditional_expression(int& curr_token) {			// Recursive
	int token_number = curr_token;
	bool found = false;

	std::cout << std::endl << token_number << ": " << tokens[token_number].get_chars() <<  " -> " << __func__;

	if(logical_or_expression(token_number)) { 
		if(tokens[token_number].get_chars() == "?") {
			if(expression(++token_number)) {
				if(tokens[token_number].get_chars() == ":") {
					if(conditional_expression(++token_number)) found = true;
					else {}															// It will be an error
				}
				else {}																// It will be an error
			}
			else {}																	// It will be an error
		}
		found = true; 
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else {
		std::cout << std::endl << "X: " << __func__;
		return false;																// It will be an error (?)
	}
}

bool Translator::Parser::assignment_expression(int& curr_token) {			// Recursive
	int token_number = curr_token;
	bool found = false;

	std::cout << std::endl << token_number << ": " << tokens[token_number].get_chars() <<  " -> " << __func__;

	if(conditional_expression(token_number)) found = true;
	else if(unary_expression(token_number)) {
		if(assignment_operator(token_number)) {
			if(assignment_expression(token_number)) found = true;
			else {}																	// It will be an error
		}
		else {}																		// It will be an error
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else {
		std::cout << std::endl << "X: " << __func__;
		return false;																// It will be an error (?)
	}
}

bool Translator::Parser::assignment_operator(int& curr_token) {
	int token_number = curr_token;
	bool found = false;

	std::cout << std::endl << token_number << ": " << tokens[token_number].get_chars() <<  " -> " << __func__;

	if(tokens[token_number].get_chars() == "=") found = true;
	else if(tokens[token_number].get_type() == Type::MUL_ASSIGN) found = true;
	else if(tokens[token_number].get_type() == Type::DIV_ASSIGN) found = true;
	else if(tokens[token_number].get_type() == Type::MOD_ASSIGN) found = true;
	else if(tokens[token_number].get_type() == Type::ADD_ASSIGN) found = true;
	else if(tokens[token_number].get_type() == Type::SUB_ASSIGN) found = true;

	if(found) {
		curr_token++;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else {
		std::cout << std::endl << "X: " << __func__;
		return false;														// It will be an error (?)
	}
}

bool Translator::Parser::expression(int& curr_token) {						// Recursive
	int token_number = curr_token;
	bool found = false;

	std::cout << std::endl << token_number << ": " << tokens[token_number].get_chars() <<  " -> " << __func__;

	if(assignment_expression(token_number)) {
		if(tokens[token_number].get_chars() == ",") {
			if(expression(++token_number)) found = true;
			else {}																// It will be an error
		}
		found = true; 
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else {
		std::cout << std::endl << "X: " << __func__;
		return false;															// It will be an error (?)
	}
}

bool Translator::Parser::declaration(int& curr_token) {
	int token_number = curr_token;
	bool found = false;

	std::cout << std::endl << token_number << ": " << tokens[token_number].get_chars() <<  " -> " << __func__;

	if(declaration_specifiers(token_number)) {
		if(tokens[token_number].get_chars() == ";") found = true;
		else if(init_declarator_list(token_number)) {
			if(tokens[token_number].get_chars() == ";") found = true;
			else {}															// It will be an error
		}
		else {}																// It will be an error
	}

	if(found) {
		curr_token = token_number + 1;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else {
		std::cout << std::endl << "X: " << __func__;
		return false;														// It will be an error (?)
	}
}

bool Translator::Parser::declaration_specifiers(int& curr_token) {						// Recursive
	int token_number = curr_token;
	bool found = false;

	std::cout << std::endl << token_number << ": " << tokens[token_number].get_chars() <<  " -> " << __func__;

	if(tokens[token_number].get_type() == Type::TYPEDEF) {
		if(declaration_specifiers(++token_number)) found = true;
		found = true;
	}
	else if(type_specifier(token_number)) {
		if(declaration_specifiers(token_number)) found = true;
		found = true;
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else {
		std::cout << std::endl << "X: " << __func__;
		return false;															// It will be an error (?)
	}
}

bool Translator::Parser::init_declarator_list(int& curr_token) {						// Recursive
	int token_number = curr_token;
	bool found = false;

	std::cout << std::endl << token_number << ": " << tokens[token_number].get_chars() <<  " -> " << __func__;

	if(init_declarator(token_number)) {
		if(tokens[token_number].get_chars() == ",") {
			if(init_declarator_list(++token_number)) found = true;
			else {}																		// It will be an error
		}
		found = true; 
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else {
		std::cout << std::endl << "X: " << __func__;
		return false;																	// It will be ab error (?)
	}
}

bool Translator::Parser::init_declarator(int& curr_token) {
	int token_number = curr_token;
	bool found = false;

	std::cout << std::endl << token_number << ": " << tokens[token_number].get_chars() <<  " -> " << __func__;

	if(direct_declarator(token_number, false)) { 
		if(tokens[token_number].get_chars() == "=") {
			if(initializer(++token_number)) found = true;
			else {}																		// It will be an error
		}
		found = true;
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else {
		std::cout << std::endl << "X: " << __func__;
		return false;																	// It will be an error (?)
	}
}

bool Translator::Parser::type_specifier(int& curr_token) {
	int token_number = curr_token;
	int found = 0;	// Types: 0 - nothing found, 1 - found function, 2 - found token

	std::cout << std::endl << token_number << ": " << tokens[token_number].get_chars() <<  " -> " << __func__;

	if(tokens[token_number].get_type() == Type::VOID) found = 2;
	else if(tokens[token_number].get_type() == Type::CHAR) found = 2;
	else if(tokens[token_number].get_type() == Type::SHORT) found = 2;
	else if(tokens[token_number].get_type() == Type::INT) found = 2;
	else if(tokens[token_number].get_type() == Type::LONG) found = 2;
	else if(tokens[token_number].get_type() == Type::FLOAT) found = 2;
	else if(tokens[token_number].get_type() == Type::DOUBLE) found = 2;
	else if(tokens[token_number].get_type() == Type::SIGNED) found = 2;
	else if(tokens[token_number].get_type() == Type::UNSIGNED) found = 2;
	else if(struct_or_union_specifier(token_number)) found = 1;
	else if(enum_specifier(token_number)) found = 1;

	if(found == 2) {
		curr_token++;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else if(found == 1) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else {
		std::cout << std::endl << "X: " << __func__;
		return false;																	// It will be an error (?)
	}
}

bool Translator::Parser::struct_or_union_specifier(int& curr_token) {
	int token_number = curr_token;
	int found = 0;	// Types: 0 - nothing found, 1 - found other token, 2 - found IDENTIFIER as last

	std::cout << std::endl << token_number << ": " << tokens[token_number].get_chars() <<  " -> " << __func__;

	if(tokens[token_number].get_type() == Type::STRUCT) {
		if(tokens[token_number + 1].get_type() == Type::IDENTIFIER) {
			if(tokens[token_number + 2].get_chars() == "{") {
				if(struct_declaration_list(token_number += 3)) {
					if(tokens[token_number].get_chars() == "}") found = 1;
					else {}																	// It will be an error
				}
				else {}																		// It will be an error
			}
			found = 2;
		}
		else if(tokens[token_number + 1].get_chars() == "{") {
			if(struct_declaration_list(token_number += 2)) {
				if(tokens[token_number].get_chars() == "}") found = 1;
				else {}																		// It will be an error
			}
			else {}																			// It will be an error
		}
		else {}																				// It will be an error
	}

	if(found) {
		curr_token = token_number + found;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else {
		std::cout << std::endl << "X: " << __func__;
		return false;																		// It will be an error (?)
	}
}

bool Translator::Parser::struct_declaration_list(int& curr_token) {							// Recursive
	int token_number = curr_token;
	bool found = true;

	std::cout << std::endl << token_number << ": " << tokens[token_number].get_chars() <<  " -> " << __func__;

	if(specifier_qualifier_list(token_number)) {
		if(struct_declarator_list(token_number)) {
			if(tokens[token_number].get_chars() == ";") {
				if(struct_declaration_list(++token_number)) found = true;
				found = true;
			}
			else {}																			// It will be an error
		}
		else {}																				// It will be an error
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else {
		std::cout << std::endl << "X: " << __func__;
		return false;																		// It will be an error (?)
	}
}

bool Translator::Parser::specifier_qualifier_list(int& curr_token) {						// Recursive
	int token_number = curr_token;
	bool found = false;

	std::cout << std::endl << token_number << ": " << tokens[token_number].get_chars() <<  " -> " << __func__;

	if(type_specifier(token_number)) { 
		if(specifier_qualifier_list(token_number)) found = true;
		found = true; 
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else {
		std::cout << std::endl << "X: " << __func__;
		return false;															// It will be an error (?)
	}
}

bool Translator::Parser::struct_declarator_list(int& curr_token) {							// Recursive
	int token_number = curr_token;
	bool found = false;

	std::cout << std::endl << token_number << ": " << tokens[token_number].get_chars() <<  " -> " << __func__;

	if(struct_declarator(token_number)) { 
		if(tokens[token_number].get_chars() == ",") {
			if(struct_declarator_list(++token_number)) found = true;
			else {}																			// It will be an error
		}
		found = true; 
	}
	
	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else {
		std::cout << std::endl << "X: " << __func__;
		return false;																		// It will be an error (?)
	}
}

bool Translator::Parser::struct_declarator(int& curr_token) {
	int token_number = curr_token;
	bool found = false;

	std::cout << std::endl << token_number << ": " << tokens[token_number].get_chars() <<  " -> " << __func__;

	if(direct_declarator(token_number, false)) {
		if(tokens[token_number].get_chars() == ":") {
			if(conditional_expression(++token_number)) found = true;
			else {}																// It will be an error
		}
		found = true;
	}
	else if(tokens[token_number].get_chars() == ":") {
		if(conditional_expression(++token_number)) found = true;
		else {}																	// It will be an error
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else {
		std::cout << std::endl << "X: " << __func__;
		return false;															// It will be an error (?)
	}
}

bool Translator::Parser::enum_specifier(int& curr_token) {
	int token_number = curr_token;
	int found = 0; // Types: 0 - nothing found, 1 - found other token, 2 - found IDENTIFIER as last

	std::cout << std::endl << token_number << ": " << tokens[token_number].get_chars() <<  " -> " << __func__;

	if(tokens[token_number].get_type() == Type::ENUM) {
		if(tokens[token_number + 1].get_type() == Type::IDENTIFIER) {
			if(tokens[token_number + 2].get_chars() == "{") {
				if(enumerator_list(token_number += 3)) {
					if(tokens[token_number].get_chars() == "}") found = 1;
					else {}																// It will be an error
				}
				else {}																	// It will be an error
			}
			found = 2;
		}
		else if(tokens[token_number + 1].get_chars() == "{") {
			if(enumerator_list(token_number += 2)) {
				if(tokens[token_number].get_chars() == "}") found = 1;
				else {}																	// It will be an error
			}
			else {}																		// It will be an error
		}
		else {}																			// It will be an error
	}

	if(found) {
		curr_token = token_number + found;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else {
		std::cout << std::endl << "X: " << __func__;
		return false;																	// It will be an error (?)
	}
}

bool Translator::Parser::enumerator_list(int& curr_token) {							// Recursive
	int token_number = curr_token;
	bool found = false;

	std::cout << std::endl << token_number << ": " << tokens[token_number].get_chars() <<  " -> " << __func__;

	if(enumerator(token_number)) { 
		if(tokens[token_number].get_chars() == ",") {
			if(enumerator_list(++token_number)) found = true;
			else {}																	// It will be an error
		}
		found = true; 
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else {
		std::cout << std::endl << "X: " << __func__;
		return false;																// It will be an error (?)
	}
}

bool Translator::Parser::enumerator(int& curr_token) {
	int token_number = curr_token;
	int found = 0;	// Types: 0 - nothing found, 1 - found function, 2 - found token

	std::cout << std::endl << token_number << ": " << tokens[token_number].get_chars() <<  " -> " << __func__;

	if(tokens[token_number].get_type() == Type::IDENTIFIER) {
		if(tokens[token_number + 1].get_chars() == "=") {
			if(conditional_expression(token_number += 2)) found = 1;
			else {}																		// It will be an error
		}
		found = 2;
	}

	if(found) {
		curr_token = token_number + found - 1;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else {
		std::cout << std::endl << "X: " << __func__;
		return false;																	// It will be an error (?)
	}
}

bool Translator::Parser::direct_declarator(int& curr_token, bool recursive) {							// Recursive
	int token_number = curr_token;
	bool found = false;

	std::cout << std::endl << token_number << ": " << tokens[token_number].get_chars() <<  " -> " << __func__;

	if(!recursive) {
		if(tokens[token_number].get_type() == Type::IDENTIFIER) {
			if(direct_declarator(++token_number, true)) found = true;
			found = true;
		}
		else if(tokens[token_number].get_chars() == "(") {
			if(direct_declarator(++token_number, false)) {
				if(tokens[token_number].get_chars() == ")") {
					if(direct_declarator(++token_number, true)) found = true;
					found = true;
				}
				else {}																		// It will be an error
			}
			else {}																			// It will be an error
		}
	}
	else {
		if(tokens[token_number].get_chars() == "[") {
			if(tokens[token_number + 1].get_chars() == "]") {
				if(direct_declarator(token_number += 2, true)) found = true;
				found = true;
			}
			else if(conditional_expression(++token_number)) {
				if(tokens[token_number].get_chars() == "]") {
					if(direct_declarator(++token_number, true)) found = true;
					found = true;
				}
				else {}																		// It will be an error
			}
			else {}																			// It will be an error
		}
		else if(tokens[token_number].get_chars() == "(") {
			if(tokens[token_number + 1].get_chars() == ")") {
				if(direct_declarator(token_number += 2, true)) found = true;
				found = true;
			}
			else if(parameter_list(++token_number)) {
				if(tokens[token_number].get_chars() == ")") {
					if(direct_declarator(++token_number, true)) found = true;
					found = true;
				}
				else {}																		// It will be an error
			}

			if(!found) {
				token_number = curr_token;
				if(identifier_list(++token_number)) {
					if(tokens[token_number].get_chars() == ")") {
						if(direct_declarator(++token_number, true)) found = true;
						found = true;
					}
					else {}																	// It will be an error
				}
				else {}																		// It will be an error
			}
		}
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else {
		std::cout << std::endl << "X: " << __func__;
		return false;																		// It will be an error (?)
	}
}

bool Translator::Parser::parameter_list(int& curr_token) {					// Recursive
	int token_number = curr_token;
	bool found = false;

	std::cout << std::endl << token_number << ": " << tokens[token_number].get_chars() <<  " -> " << __func__;

	if(parameter_declaration(token_number)) {
		if(tokens[token_number].get_chars() == ",") {
			if(parameter_list(++token_number)) found = true;
			else {}																// It will be an error
		}
		found = true; 
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else {
		std::cout << std::endl << "X: " << __func__;
		return false;															// It will be an error (?)
	}
}

bool Translator::Parser::parameter_declaration(int& curr_token) {
	int token_number = curr_token;
	bool found = false;

	std::cout << std::endl << token_number << ": " << tokens[token_number].get_chars() <<  " -> " << __func__;

	if(declaration_specifiers(token_number)) {
		if(direct_declarator(token_number, false)) found = true;
		else if(direct_abstract_declarator(token_number, false)) found = true;
		found = true;
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else {
		std::cout << std::endl << "X: " << __func__;
		return false;															// It will be an error (?)
	}
}

bool Translator::Parser::identifier_list(int& curr_token) {						// Recursive
	int token_number = curr_token;
	int found = 0;	// Types: 0 - nothing found, 1 - end of recursion, 2 - ends with token

	std::cout << std::endl << token_number << ": " << tokens[token_number].get_chars() <<  " -> " << __func__;

	if(tokens[token_number].get_type() == Type::IDENTIFIER) { 
		if(tokens[token_number + 1].get_chars() == ",") {
			if(identifier_list(token_number += 2)) found = 1;
			else {}															// It will be an error
		}
		found = 2;
	}

	if(found) {
		curr_token = token_number + found - 1;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else {
		std::cout << std::endl << "X: " << __func__;
		return false;														// It will be an error (?)
	}
}

bool Translator::Parser::type_name(int& curr_token) {
	int token_number = curr_token;
	bool found = false;

	std::cout << std::endl << token_number << ": " << tokens[token_number].get_chars() <<  " -> " << __func__;

	if(specifier_qualifier_list(token_number)) {
		if(direct_abstract_declarator(token_number, false)) found = true;
		found = true;
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else {
		std::cout << std::endl << "X: " << __func__;
		return false;																	// It will be an error (?)
	}
}

bool Translator::Parser::direct_abstract_declarator(int& curr_token, bool recursive) {								// Recursive
	int token_number = curr_token;
	bool found = false;

	std::cout << std::endl << token_number << ": " << tokens[token_number].get_chars() <<  " -> " << __func__;

	if(tokens[token_number].get_chars() == "(") {
		if(!recursive) {
			if(direct_abstract_declarator(++token_number, false)) {
				if(tokens[token_number].get_chars() == ")") {
					if(direct_abstract_declarator(++token_number, true)) found = true;
					found = true;
				}
				else {}																		// It will be an error
			}
			else {}																			// It will be an error
		}
		else {
			if(tokens[token_number + 1].get_chars() == ")") {
				if(direct_abstract_declarator(token_number += 2, true)) found = true;
				found = true;
			}
			else if(parameter_list(++token_number)) {
				if(tokens[token_number].get_chars() == ")") {
					if(direct_abstract_declarator(++token_number, true)) found = true;
					found = true;
				}
				else {}																		// It will be an error
			}
			else {}																			// It will be an error
		}
	}
	else if(tokens[token_number].get_chars() == "[") {
		if(tokens[token_number + 1].get_chars() == "]") {
			if(direct_abstract_declarator(token_number += 2, true)) found = true;
			found = true;
		}
		else if(conditional_expression(++token_number)) {
			if(tokens[token_number].get_chars() == "]") {
				if(direct_abstract_declarator(++token_number, true)) found = true;
				found = true;
			}
			else {}																			// It will be an error
		}
		else {}																				// It will be an error
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else {
		std::cout << std::endl << "X: " << __func__;
		return false;																		// It will be an error (?)
	}
}

bool Translator::Parser::initializer(int& curr_token) {
	int token_number = curr_token;
	int found = 0;	// Types: 0 - nothing found, 1 - found function, 2 - found token

	std::cout << std::endl << token_number << ": " << tokens[token_number].get_chars() <<  " -> " << __func__;

	if(assignment_expression(token_number)) found = 1;
	else if(tokens[token_number].get_chars() == "{") {
		if(initializer_list(++token_number)) {
			if(tokens[token_number].get_chars() == "}") found = 2;
			else {}																		// It will be an error
		}
		else {}																			// It will be an error
	}

	if(found) {
		curr_token = token_number + found - 1;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else {
		std::cout << std::endl << "X: " << __func__;
		return false;																	// It will be an error (?)
	}
}

bool Translator::Parser::initializer_list(int& curr_token) {			// Recursive
	int token_number = curr_token;
	bool found = false;

	std::cout << std::endl << token_number << ": " << tokens[token_number].get_chars() <<  " -> " << __func__;

	if(initializer(token_number)) { 
		if(tokens[token_number].get_chars() == ",") {
			if(initializer_list(++token_number)) found = true;
			else {}															// It will be an error
		}
		found = true; 
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else {
		std::cout << std::endl << "X: " << __func__;
		return false;														// It will be an error (?)
	}
}

bool Translator::Parser::statement(int& curr_token) {
	int token_number = curr_token;
	bool found = false;

	std::cout << std::endl << token_number << ": " << tokens[token_number].get_chars() <<  " -> " << __func__;

	if(labeled_statement(token_number)) found = true;
	else if(compound_statement(token_number)) found = true;
	else if(expression_statement(token_number)) found = true;
	else if(selection_statement(token_number)) found = true;
	else if(iteration_statement(token_number)) found = true;
	else if(jump_statement(token_number)) found = true;

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else {
		std::cout << std::endl << "X: " << __func__;
		return false;														// It will be an error (?)
	}
}

bool Translator::Parser::labeled_statement(int& curr_token) {
	int token_number = curr_token;
	bool found = false;

	std::cout << std::endl << token_number << ": " << tokens[token_number].get_chars() <<  " -> " << __func__;

	if(tokens[token_number].get_type() == Type::CASE) {
		if(conditional_expression(++token_number)) {
			if(tokens[token_number].get_chars() == ":") {
				if(statement(++token_number)) found = true;
				else {}															// It will be an error
			}
			else {}																// It will be an error
		}
		else {}																	// It will be an error
	}
	else if(tokens[token_number].get_type() == Type::DEFAULT) {
		if(tokens[token_number + 1].get_chars() == ":") {
			if(statement(token_number += 2)) found = true;
			else {}															// It will be an error
		}
		else {}																// It will be an error
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else {
		std::cout << std::endl << "X: " << __func__;
		return false;															// It will be an error (?)
	}
}

bool Translator::Parser::compound_statement(int& curr_token) {
	int token_number = curr_token;
	int found = 0;	// Types: 0 - nothing found, 1 - found parentesis with function, 2 - found only parentesis

	std::cout << std::endl << token_number << ": " << tokens[token_number].get_chars() <<  " -> " << __func__;

	if(tokens[token_number].get_chars() == "{" ) {
		if(tokens[token_number + 1].get_chars() == "}") found = 2;
		else if(statement_list(++token_number)) {
			if(tokens[token_number].get_chars() == "}") found = 1;
			else {}																	// It will be an error
		}

		if(!found) {
			token_number = curr_token;
			if(declaration_list(++token_number)) {
				if(tokens[token_number].get_chars() == "}") found = 1;
				else if(statement_list(token_number)) {
					if(tokens[token_number].get_chars() == "}") found = 1;
					else {}															// It will be an error
				}
				else {}																// It will be an error
			}
			else {}																	// It will be an error
		}
	}

	if(found) {
		curr_token = token_number + found;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else {
		std::cout << std::endl << "X: " << __func__;
		return false;																// It will be an error (?)
	}
}

bool Translator::Parser::declaration_list(int& curr_token) {			// Recursive
	int token_number = curr_token;
	bool found = false;

	std::cout << std::endl << token_number << ": " << tokens[token_number].get_chars() <<  " -> " << __func__;

	if(declaration(token_number)) {
		if(declaration_list(token_number)) found = true;
		found = true; 
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else {
		std::cout << std::endl << "X: " << __func__;
		return false;													// It will be an error (?)
	}
}

bool Translator::Parser::statement_list(int& curr_token) {				// Recursive
	int token_number = curr_token;
	bool found = false;

	std::cout << std::endl << token_number << ": " << tokens[token_number].get_chars() <<  " -> " << __func__;

	if(statement(token_number)) {
		if(statement_list(token_number)) found = true;
		found = true; 
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else {
		std::cout << std::endl << "X: " << __func__;
		return false;													// It will be an error (?)
	}
}

bool Translator::Parser::expression_statement(int& curr_token) {
	int token_number = curr_token;
	bool found = false;

	std::cout << std::endl << token_number << ": " << tokens[token_number].get_chars() <<  " -> " << __func__;

	if(tokens[token_number].get_chars() == ";") found = true;
	else if(expression(token_number)) {
		if(tokens[token_number].get_chars() == ";") found = true;
		else {}																// It will be an error
	}

	if(found) {
		curr_token = token_number + 1;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else {
		std::cout << std::endl << "X: " << __func__;
		return false;														// It will be an error (?)
	}
}

bool Translator::Parser::selection_statement(int& curr_token) {
	int token_number = curr_token;
	bool found = false;

	std::cout << std::endl << token_number << ": " << tokens[token_number].get_chars() <<  " -> " << __func__;

	if(tokens[token_number].get_type() == Type::IF) {
		if(tokens[token_number + 1].get_chars() == "(") {
			if(expression(token_number += 2)) {
				if(tokens[token_number].get_chars() == ")") {
					if(statement(++token_number)) {
						if(tokens[token_number].get_type() == Type::ELSE) {
							if(statement(++token_number)) found = true;
							else {}													// It will be an error
						}
						found = true;
					}
					else {}															// It will be an error
				}
				else {}																// It will be an error
			}
			else {}																	// It will be an error
		}
		else {}																		// It will be an error
	}
	else if(tokens[token_number].get_type() == Type::SWITCH) {
		if(tokens[token_number + 1].get_chars() == "(") {
			if(expression(token_number += 2)) {
				if(tokens[token_number].get_chars() == ")") {
					if(statement(++token_number)) found = true;
					else {}															// It will be an error
				}
				else {}																// It will be an error
			}
			else {}																	// It will be an error
		}
		else {}																		// It will be an error
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else {
		std::cout << std::endl << "X: " << __func__;
		return false;																// It will be an error (?)
	}
}

bool Translator::Parser::iteration_statement(int& curr_token) {
	int token_number = curr_token;
	int found = 0;	// Types: 0 - nothing found, 1 - found function, 2 - found token

	std::cout << std::endl << token_number << ": " << tokens[token_number].get_chars() <<  " -> " << __func__;

	if(tokens[token_number].get_type() == Type::WHILE) {
		if(tokens[token_number + 1].get_chars() == "(") {
			if(expression(token_number += 2)) {
				if(tokens[token_number].get_chars() == ")") {
					if(statement(++token_number)) found = 1;
					else {}																// It will be an error
				}
				else {}																	// It will be an error
			}
			else {}																		// It will be an error
		}
		else {}																			// It will be an error
	}
	else if(tokens[token_number].get_type() == Type::DO) {
		if(statement(++token_number)) {
			if(tokens[token_number].get_type() == Type::WHILE) {
				if(tokens[token_number + 1].get_chars() == "(") {
					if(expression(token_number += 2)) {
						if(tokens[token_number].get_chars() == ")") {
							if(tokens[token_number + 1].get_chars() == ";") found = 2;
							else {}														// It will be an error
						}
						else {}															// It will be an error
					}
					else {}																// It will be an error
				}
				else {}																	// It will be an error
			}
			else {}																		// It will be an error
		}
		else {}																			// It will be an error
	}
	else if(tokens[token_number].get_type() == Type::FOR) {
		if(tokens[token_number + 1].get_chars() == "(") {
			if(expression_statement(token_number += 2)) {
				if(expression_statement(token_number)) {
					if(tokens[token_number].get_chars() == ")") {
						if(statement(++token_number)) found = 1;
						else {}															// It will be an error
					}
					else if(expression(token_number)) {
						if(tokens[token_number].get_chars() == ")") {
							if(statement(++token_number)) found = 1;
							else {}														// It will be an error
						}
						else {}															// It will be an error
					}
					else {}																// It will be an error
				}
				else {}																	// It will be an error
			}
			else {}																		// It will be an error
		}
		else {}																			// It will be an error
	}

	if(found == 2) {
		curr_token = token_number + 2;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else if(found == 1) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else {
		std::cout << std::endl << "X: " << __func__;
		return false;																	// It will be an error (?)
	}
}

bool Translator::Parser::jump_statement(int& curr_token) {
	int token_number = curr_token;
	int found = 0;	// Types: 0 - nothing found, 1 - found token after function, 2 - found only token

	std::cout << std::endl << token_number << ": " << tokens[token_number].get_chars() <<  " -> " << __func__;

	if(tokens[token_number].get_type() == Type::BREAK) {
		if(tokens[token_number + 1].get_chars() == ";") found = 2;
		else {}															// It will be an error
	}
	else if(tokens[token_number].get_type() == Type::RETURN) {
		if(tokens[token_number + 1].get_chars() == ";") found = 2;
		else if(expression(++token_number)) {
			if(tokens[token_number].get_chars() == ";") found = 1;
			else {}														// It will be an error
		}
		else {}															// It will be an error
	}

	if(found) {
		curr_token = token_number + found;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else {
		std::cout << std::endl << "X: " << __func__;
		return false;													// It will be an error (?)
	}
}

bool Translator::Parser::translation_unit(int& curr_token) {			// Recursive
	int token_number = curr_token;
	bool found = false;

	std::cout << std::endl << token_number << ": " << tokens[token_number].get_chars() <<  " -> " << __func__;

	if(external_declaration(token_number)) {
		if(translation_unit(token_number)) found = true;
		found = true; 
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else {
		std::cout << std::endl << "X: " << __func__;
		return false;													// It will be an error (?)
	}
}

bool Translator::Parser::external_declaration(int& curr_token) {
	int token_number = curr_token;
	bool found = false;

	std::cout << std::endl << token_number << ": " << tokens[token_number].get_chars() <<  " -> " << __func__;

	if(function_definition(token_number)) found = true;
	else if(declaration(token_number)) found = true;

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else {
		std::cout << std::endl << "X: " << __func__;
		return false;												// It will be an error (?)
	}
}

bool Translator::Parser::function_definition(int& curr_token) {
	int token_number = curr_token;
	bool found = false;

	std::cout << std::endl << token_number << ": " << tokens[token_number].get_chars() <<  " -> " << __func__;

	if(declaration_specifiers(token_number)) {
		if(direct_declarator(token_number, false)) {
			if(declaration_list(token_number)) {
				if(compound_statement(token_number)) found = true;
				else {}																	// It will be an error
			}
			else if(compound_statement(token_number)) found = true;
			else {}																		// It will be an error
		}
		else {}																			// It will be an error
	}
	else if(direct_declarator(token_number, false)) {
		if(declaration_list(token_number)) {
			if(compound_statement(token_number)) found = true;
			else {}																		// It will be an error
		}
		else if(compound_statement(token_number)) found = true;
		else {}																			// It will be an error
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) { std::cout << std::endl << "Parsing finished"; exit(1); }
		return true;
	}
	else {
		std::cout << std::endl << "X: " << __func__;
		return false;																	// It will be an error (?)
	}
}