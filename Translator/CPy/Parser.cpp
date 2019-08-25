#include "Parser.h"

void Translator::Parser::func_log(int tn, const char* func) {
#ifdef _DEBUG
	std::cout << std::endl << tn << ": " << tokens[tn].get_chars() << " -> " << func;
#endif
}

//bool Translator::Parser::is_curr_token(int tn, std::string s) {
//	return tokens[tn].get_chars() == s;
//}
//
//bool Translator::Parser::is_curr_token(int tn, Type t) {
//	return tokens[tn].get_type() == t;
//}

void Translator::Parser::start_parsing() {
	// std::cout << std::endl << __func__;

	try {
		root = translation_unit(0);
	}
	catch(Parser_Exception e) {
		std::cerr << e.what() << std::endl;
	}
	std::cout << std::endl << "Parsing finished";
}

Translator::Node Translator::Parser::primary_expression(int& curr_token) {
	int token_number = curr_token;
	bool found = false;
	Node p_node(Token(Type::parsing), __func__);
	Node result;

	if(end) return Node(Token(Type::none), __func__);

	func_log(token_number, __func__);

	switch(tokens[token_number].get_type()) {
		case Type::IDENTIFIER:
		case Type::CONSTANT:
		case Type::STRING_LITERAL:
			p_node.push_back(Node(tokens[token_number], __func__));
			found = true;
			break;
		default: {
			if(tokens[token_number].get_chars() == "(") {
				p_node.push_back(Node(tokens[token_number], __func__));
				if((result = expression(++token_number)).get_type() != Type::none) {
					p_node.push_back(result);
					if(tokens[token_number].get_chars() == ")") {
						p_node.push_back(Node(tokens[token_number], __func__));
						found = true;
					}
					else {}													// It will be an error
				}
				else {}														// It will be an error
			}
			break;
		}
	}

	if(found) {
		curr_token = token_number + 1;
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else {
		// std::cout << std::endl << "X: " << __func__;
		return Node(Token(Type::none), __func__);												// It will be an error (?)
	}
}

Translator::Node Translator::Parser::postfix_expression(int& curr_token, bool recursive) {			// Recursive
	int token_number = curr_token;
	bool found = false;
	Node p_node(Token(Type::parsing), __func__);
	Node result;

	if(end) return Node(Token(Type::none), __func__);

	func_log(token_number, __func__);

	if(!recursive) {
		if((result = primary_expression(token_number)).get_type() != Type::none) {
			p_node.push_back(result);
			if((result = postfix_expression(token_number, true)).get_type() != Type::none) {
				p_node.push_back(result);
				found = true;
			}
			found = true;
		}
	}
	else {
		if(tokens[token_number].get_chars() == "(") {
			p_node.push_back(Node(tokens[token_number], __func__));
			if(tokens[token_number + 1].get_chars() == ")") {
				p_node.push_back(Node(tokens[token_number + 1], __func__));
				if((result = postfix_expression(token_number += 2, true)).get_type() != Type::none) {
					p_node.push_back(result);
					found = true;
				}
				found = true;
			}
			else if((result = argument_expression_list(++token_number)).get_type() != Type::none) {
				p_node.push_back(result);
				if(tokens[token_number].get_chars() == ")") {
					p_node.push_back(Node(tokens[token_number], __func__));
					if((result = postfix_expression(++token_number, true)).get_type() != Type::none) {
						p_node.push_back(result);
						found = true;
					}
					found = true;
				}
				else {}																	// It will be an error
			}
			else {}																		// It will be an error
		}
		else if(tokens[token_number].get_chars() == ".") {
			p_node.push_back(Node(tokens[token_number], __func__));
			if(tokens[token_number + 1].get_type() == Type::IDENTIFIER) {
				p_node.push_back(Node(tokens[token_number + 1], __func__));
				if((result = postfix_expression(token_number += 2, true)).get_type() != Type::none) {
					p_node.push_back(result);
					found = true;
				}
				found = true;
			}
			else {}																		// It will be an error
		}
		else if(tokens[token_number].get_type() == Type::INC_OP) {
			p_node.push_back(Node(tokens[token_number], __func__));
			if((result = postfix_expression(++token_number, true)).get_type() != Type::none) {
				p_node.push_back(result);
				found = true;
			}
			found = true;
		}
		else if(tokens[token_number].get_type() == Type::DEC_OP) {
			p_node.push_back(Node(tokens[token_number], __func__));
			if((result = postfix_expression(++token_number, true)).get_type() != Type::none) {
				p_node.push_back(result);
				found = true;
			}
			found = true;
		}
		else if(tokens[token_number].get_chars() == "[") {
			p_node.push_back(Node(tokens[token_number], __func__));
			if((result = expression(++token_number)).get_type() != Type::none) {
				p_node.push_back(result);
				if(tokens[token_number].get_chars() == "]") {
					p_node.push_back(Node(tokens[token_number], __func__));
					if((result = postfix_expression(++token_number, true)).get_type() != Type::none) {
						p_node.push_back(result);
						found = true;
					}
					found = true;
				}
				else {}																	// It will be an error
			}
			else {}																		// It will be an error
		}
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else {
		// std::cout << std::endl << "X: " << __func__;
		return Node(Token(Type::none), __func__);
	}
}

Translator::Node Translator::Parser::argument_expression_list(int& curr_token) {	// Recursive
	int token_number = curr_token;
	bool found = false;
	Node p_node(Token(Type::parsing), __func__);
	Node result;

	if(end) return Node(Token(Type::none), __func__);

	func_log(token_number, __func__);

	if((result = assignment_expression(token_number)).get_type() != Type::none) {
		p_node.push_back(result);
		if(tokens[token_number].get_chars() == ",") {
			p_node.push_back(Node(tokens[token_number], __func__));
			if((result = argument_expression_list(++token_number)).get_type() != Type::none) {
				p_node.push_back(result);
				found = true;
			}
			else {}																	// It will be an error
		}
		found = true; 
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else {
		// std::cout << std::endl << "X: " << __func__;
		return Node(Token(Type::none), __func__);																// It will be an error (?)
	}
}

Translator::Node Translator::Parser::unary_expression(int& curr_token) {				// Recursive
	int token_number = curr_token;
	bool found = false;
	Node p_node(Token(Type::parsing), __func__);
	Node result;

	if(end) return Node(Token(Type::none), __func__);

	func_log(token_number, __func__);

	if((result = postfix_expression(token_number, false)).get_type() != Type::none) {
		p_node.push_back(result);
		found = true;
	}
	else if(tokens[token_number].get_type() == Type::INC_OP || tokens[token_number].get_type() == Type::DEC_OP) {
		p_node.push_back(Node(tokens[token_number], __func__));
		if((result = unary_expression(++token_number)).get_type() != Type::none) {
			p_node.push_back(result);
			found = true;
		}
		else {}																// It will be an error
	}
	else if((result = unary_operator(token_number)).get_type() != Type::none) {
		p_node.push_back(result);
		if((result = cast_expression(token_number)).get_type() != Type::none) {
			p_node.push_back(result);
			found = true;
		}
		else {}																// It will be an error
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else {
		// std::cout << std::endl << "X: " << __func__;
		return Node(Token(Type::none), __func__);														// It will be an error (?)
	}
}

Translator::Node Translator::Parser::unary_operator(int& curr_token) {
	int token_number = curr_token;
	bool found = false;
	Node p_node(Token(Type::parsing), __func__);
	Node result;

	if(end) return Node(Token(Type::none), __func__);

	func_log(token_number, __func__);

	std::string tokens_chars = tokens[token_number].get_chars();

	if(tokens_chars == "*" || tokens_chars == "+" || tokens_chars == "-" || tokens_chars == "!") {
		p_node.push_back(Node(tokens[token_number], __func__));
		found = true;
	}

	if(found) {
		curr_token++;
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else {
		// std::cout << std::endl << "X: " << __func__;
		return Node(Token(Type::none), __func__);														// It will be an error (?)
	}
}

Translator::Node Translator::Parser::cast_expression(int& curr_token) {					// Recursive
	int token_number = curr_token;
	bool found = false;
	Node p_node(Token(Type::parsing), __func__);
	Node result;

	if(end) return Node(Token(Type::none), __func__);

	func_log(token_number, __func__);

	if((result = unary_expression(token_number)).get_type() != Type::none) {
		p_node.push_back(result);
		found = true;
	}
	else if(tokens[token_number].get_chars() == "(") {
		p_node.push_back(Node(tokens[token_number], __func__));
		if((result = type_name(++token_number)).get_type() != Type::none) {
			p_node.push_back(result);
			if(tokens[token_number].get_chars() == ")") {
				p_node.push_back(Node(tokens[token_number], __func__));
				if((result = cast_expression(++token_number)).get_type() != Type::none) {
					p_node.push_back(result);
					found = true;
				}
				else {}																// It will be an error
			}
			else {}																	// It will be an error
		}
		else {}																		// It will be an error
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else {
		// std::cout << std::endl << "X: " << __func__;
		return Node(Token(Type::none), __func__);																// It will be an error (?)
	}
}

Translator::Node Translator::Parser::multiplicative_expression(int& curr_token) {		// Recursive
	int token_number = curr_token;
	bool found = false;
	Node p_node(Token(Type::parsing), __func__);
	Node result;

	if(end) return Node(Token(Type::none), __func__);

	func_log(token_number, __func__);

	if((result = cast_expression(token_number)).get_type() != Type::none) {
		p_node.push_back(result);
		if(tokens[token_number].get_chars() == "*" ||
			tokens[token_number].get_chars() == "/" ||
			tokens[token_number].get_chars() == "%") {
			p_node.push_back(Node(tokens[token_number], __func__));
			if((result = multiplicative_expression(++token_number)).get_type() != Type::none) {
				p_node.push_back(result);
				found = true;
			}
			else {}																		// It will be an error
		}
		found = true; 
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else {
		// std::cout << std::endl << "X: " << __func__;
		return Node(Token(Type::none), __func__);
	}
}

Translator::Node Translator::Parser::additive_expression(int& curr_token) {				// Recursive
	int token_number = curr_token;
	bool found = false;
	Node p_node(Token(Type::parsing), __func__);
	Node result;

	if(end) return Node(Token(Type::none), __func__);

	func_log(token_number, __func__);

	if((result = multiplicative_expression(token_number)).get_type() != Type::none) {
		p_node.push_back(result);
		if(tokens[token_number].get_chars() == "+" || tokens[token_number].get_chars() == "-") {
			p_node.push_back(Node(tokens[token_number], __func__));
			if((result = additive_expression(++token_number)).get_type() != Type::none) {
				p_node.push_back(result);
				found = true;
			}
			else {}																	// It will be an error
		}
		found = true; 
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else {
		// std::cout << std::endl << "X: " << __func__;
		return Node(Token(Type::none), __func__);																// It will be an error (?)
	}
}

Translator::Node Translator::Parser::relational_expression(int& curr_token) {			// Recursive
	int token_number = curr_token;
	bool found = false;
	Node p_node(Token(Type::parsing), __func__);
	Node result;

	if(end) return Node(Token(Type::none), __func__);

	func_log(token_number, __func__);

	if((result = additive_expression(token_number)).get_type() != Type::none) {
		p_node.push_back(result);
		if(tokens[token_number].get_chars() == "<" ||
			tokens[token_number].get_chars() == ">" ||
			tokens[token_number].get_type() == Type::LE_OP ||
			tokens[token_number].get_type() == Type::GE_OP) {
			p_node.push_back(Node(tokens[token_number], __func__));
			if((result = relational_expression(++token_number)).get_type() != Type::none) {
				p_node.push_back(result);
				found = true;
			}
			else {}																// It will be an error
		}
		found = true; 
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else {
		// std::cout << std::endl << "X: " << __func__;
		return Node(Token(Type::none), __func__);															// It will be an error (?)
	}
}

Translator::Node Translator::Parser::equality_expression(int& curr_token) {				// Recursive
	int token_number = curr_token;
	bool found = false;
	Node p_node(Token(Type::parsing), __func__);
	Node result;

	if(end) return Node(Token(Type::none), __func__);

	func_log(token_number, __func__);

	if((result = relational_expression(token_number)).get_type() != Type::none) {
		p_node.push_back(result);
		if(tokens[token_number].get_type() == Type::EQ_OP || tokens[token_number].get_type() == Type::NE_OP) {
			p_node.push_back(Node(tokens[token_number], __func__));
			if((result = equality_expression(++token_number)).get_type() != Type::none) {
				p_node.push_back(result);
				found = true;
			}
			else {}																// It will be an error
		}
		found = true; 
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else {
		// std::cout << std::endl << "X: " << __func__;
		return Node(Token(Type::none), __func__);															// It will be an error (?)
	}
}

Translator::Node Translator::Parser::logical_and_expression(int& curr_token) {			// Recursive
	int token_number = curr_token;
	bool found = false;
	Node p_node(Token(Type::parsing), __func__);
	Node result;

	if(end) return Node(Token(Type::none), __func__);

	func_log(token_number, __func__);

	if((result = equality_expression(token_number)).get_type() != Type::none) {
		p_node.push_back(result);
		if(tokens[token_number].get_type() == Type::AND_OP) {
			p_node.push_back(Node(tokens[token_number], __func__));
			if((result = logical_and_expression(++token_number)).get_type() != Type::none) {
				p_node.push_back(result);
				found = true;
			}
			else {}																			// It will be an error
		}
		found = true; 
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else {
		// std::cout << std::endl << "X: " << __func__;
		return Node(Token(Type::none), __func__);																		// It will be an error (?)
	}
}

Translator::Node Translator::Parser::logical_or_expression(int& curr_token) {			// Recursive
	int token_number = curr_token;
	bool found = false;
	Node p_node(Token(Type::parsing), __func__);
	Node result;

	if(end) return Node(Token(Type::none), __func__);

	func_log(token_number, __func__);

	if((result = logical_and_expression(token_number)).get_type() != Type::none) {
		p_node.push_back(result);
		if(tokens[token_number].get_type() == Type::OR_OP) {
			p_node.push_back(Node(tokens[token_number], __func__));
			if((result = logical_or_expression(++token_number)).get_type() != Type::none) {
				p_node.push_back(result);
				found = true;
			}
			else {}															// It will be an error
		}
		found = true; 
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else {
		// std::cout << std::endl << "X: " << __func__;
		return Node(Token(Type::none), __func__);														// It will be an error (?)
	}
}

Translator::Node Translator::Parser::conditional_expression(int& curr_token) {			// Recursive
	int token_number = curr_token;
	bool found = false;
	Node p_node(Token(Type::parsing), __func__);
	Node result;

	if(end) return Node(Token(Type::none), __func__);

	func_log(token_number, __func__);

	if((result = logical_or_expression(token_number)).get_type() != Type::none) {
		p_node.push_back(result);
		if(tokens[token_number].get_chars() == "?") {
			p_node.push_back(Node(tokens[token_number], __func__));
			if((result = expression(++token_number)).get_type() != Type::none) {
				p_node.push_back(result);
				if(tokens[token_number].get_chars() == ":") {
					p_node.push_back(Node(tokens[token_number], __func__));
					if((result = conditional_expression(++token_number)).get_type() != Type::none) {
						p_node.push_back(result);
						found = true;
					}
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
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else {
		// std::cout << std::endl << "X: " << __func__;
		return Node(Token(Type::none), __func__);																// It will be an error (?)
	}
}

Translator::Node Translator::Parser::assignment_expression(int& curr_token) {			// Recursive
	int token_number = curr_token;
	bool found = false;
	Node p_node(Token(Type::parsing), __func__);
	Node result;

	if(end) return Node(Token(Type::none), __func__);

	func_log(token_number, __func__);

	if((result = conditional_expression(token_number)).get_type() != Type::none) {
		p_node.push_back(result);
		found = true;
	}
	else if((result = unary_expression(token_number)).get_type() != Type::none) {
		p_node.push_back(result);
		if((result = assignment_operator(token_number)).get_type() != Type::none) {
			p_node.push_back(result);
			if((result = assignment_expression(token_number)).get_type() != Type::none) {
				p_node.push_back(result);
				found = true;
			}
			else {}																	// It will be an error
		}
		else {}																		// It will be an error
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else {
		// std::cout << std::endl << "X: " << __func__;
		return Node(Token(Type::none), __func__);																// It will be an error (?)
	}
}

Translator::Node Translator::Parser::assignment_operator(int& curr_token) {
	int token_number = curr_token;
	bool found = false;
	Node p_node(Token(Type::parsing), __func__);
	Node result;

	if(end) return Node(Token(Type::none), __func__);

	func_log(token_number, __func__);

	switch(tokens[token_number].get_type()) {
		case Type::MUL_ASSIGN:
		case Type::DIV_ASSIGN:
		case Type::MOD_ASSIGN:
		case Type::ADD_ASSIGN:
		case Type::SUB_ASSIGN:
			p_node.push_back(Node(tokens[token_number], __func__));
			found = true;
			break;
		default: {
			if(tokens[token_number].get_chars() == "=") {
				p_node.push_back(Node(tokens[token_number], __func__));
				found = true;
			}
			break;
		}
	}

	if(found) {
		curr_token++;
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else {
		// std::cout << std::endl << "X: " << __func__;
		return Node(Token(Type::none), __func__);														// It will be an error (?)
	}
}

Translator::Node Translator::Parser::expression(int& curr_token) {						// Recursive
	int token_number = curr_token;
	bool found = false;
	Node p_node(Token(Type::parsing), __func__);
	Node result;

	if(end) return Node(Token(Type::none), __func__);

	func_log(token_number, __func__);

	if((result = assignment_expression(token_number)).get_type() != Type::none) {
		p_node.push_back(result);
		if(tokens[token_number].get_chars() == ",") {
			p_node.push_back(Node(tokens[token_number], __func__));
			if((result = expression(++token_number)).get_type() != Type::none) {
				p_node.push_back(result);
				found = true;
			}
			else {}																// It will be an error
		}
		found = true; 
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else {
		// std::cout << std::endl << "X: " << __func__;
		return Node(Token(Type::none), __func__);															// It will be an error (?)
	}
}

Translator::Node Translator::Parser::declaration(int& curr_token) {
	int token_number = curr_token;
	bool found = false;
	Node p_node(Token(Type::parsing), __func__);
	Node result;

	if(end) return Node(Token(Type::none), __func__);

	func_log(token_number, __func__);

	if((result = declaration_specifiers(token_number)).get_type() != Type::none) {
		p_node.push_back(result);
		if(tokens[token_number].get_chars() == ";") {
			p_node.push_back(Node(tokens[token_number], __func__));
			found = true;
		}
		else if((result = init_declarator_list(token_number)).get_type() != Type::none) {
			p_node.push_back(result);
			if(tokens[token_number].get_chars() == ";") {
				p_node.push_back(Node(tokens[token_number], __func__));
				found = true;
			}
			else {}															// It will be an error
		}
		else {}																// It will be an error
	}

	if(found) {
		curr_token = token_number + 1;
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else {
		// std::cout << std::endl << "X: " << __func__;
		return Node(Token(Type::none), __func__);														// It will be an error (?)
	}
}

Translator::Node Translator::Parser::declaration_specifiers(int& curr_token) {						// Recursive
	int token_number = curr_token;
	bool found = false;
	Node p_node(Token(Type::parsing), __func__);
	Node result;

	if(end) return Node(Token(Type::none), __func__);

	func_log(token_number, __func__);

	if(tokens[token_number].get_type() == Type::TYPEDEF) {
		p_node.push_back(Node(tokens[token_number], __func__));
		if((result = declaration_specifiers(++token_number)).get_type() != Type::none) {
			p_node.push_back(result);
			found = true;
		}
		found = true;
	}
	else if((result = type_specifier(token_number)).get_type() != Type::none) {
		p_node.push_back(result);
		if((result = declaration_specifiers(token_number)).get_type() != Type::none) {
			p_node.push_back(result);
			found = true;
		}
		found = true;
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else {
		// std::cout << std::endl << "X: " << __func__;
		return Node(Token(Type::none), __func__);															// It will be an error (?)
	}
}

Translator::Node Translator::Parser::init_declarator_list(int& curr_token) {						// Recursive
	int token_number = curr_token;
	bool found = false;
	Node p_node(Token(Type::parsing), __func__);
	Node result;

	if(end) return Node(Token(Type::none), __func__);

	func_log(token_number, __func__);

	if((result = init_declarator(token_number)).get_type() != Type::none) {
		p_node.push_back(result);
		if(tokens[token_number].get_chars() == ",") {
			p_node.push_back(Node(tokens[token_number], __func__));
			if((result = init_declarator_list(++token_number)).get_type() != Type::none) {
				p_node.push_back(result);
				found = true;
			}
			else {}																		// It will be an error
		}
		found = true; 
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else {
		// std::cout << std::endl << "X: " << __func__;
		return Node(Token(Type::none), __func__);																	// It will be ab error (?)
	}
}

Translator::Node Translator::Parser::init_declarator(int& curr_token) {
	int token_number = curr_token;
	bool found = false;
	Node p_node(Token(Type::parsing), __func__);
	Node result;

	if(end) return Node(Token(Type::none), __func__);

	func_log(token_number, __func__);

	if((result = direct_declarator(token_number, false)).get_type() != Type::none) {
		p_node.push_back(result);
		if(tokens[token_number].get_chars() == "=") {
			p_node.push_back(Node(tokens[token_number], __func__));
			if((result = initializer(++token_number)).get_type() != Type::none) {
				p_node.push_back(result);
				found = true;
			}
			else {}																		// It will be an error
		}
		found = true;
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else {
		// std::cout << std::endl << "X: " << __func__;
		return Node(Token(Type::none), __func__);																	// It will be an error (?)
	}
}

Translator::Node Translator::Parser::type_specifier(int& curr_token) {
	int token_number = curr_token;
	int found = 0;	// Types: 0 - nothing found, 1 - found function, 2 - found token
	Node p_node(Token(Type::parsing), __func__);
	Node result;

	if(end) return Node(Token(Type::none), __func__);

	func_log(token_number, __func__);

	switch(tokens[token_number].get_type()) {
		case Type::VOID:
		case Type::CHAR:
		case Type::SHORT:
		case Type::INT:
		case Type::LONG:
		case Type::FLOAT:
		case Type::DOUBLE:
		case Type::SIGNED:
		case Type::UNSIGNED:
			p_node.push_back(Node(tokens[token_number], __func__));
			found = 2;
			break;
		default: {
			if((result = struct_or_union_specifier(token_number)).get_type() != Type::none) {
				p_node.push_back(result);
				found = 1;
			}
			else if((result = enum_specifier(token_number)).get_type() != Type::none) {
				p_node.push_back(result);
				found = 1;
			}
			break;
		}
	}

	if(found == 2) {
		curr_token++;
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else if(found == 1) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else {
		// std::cout << std::endl << "X: " << __func__;
		return Node(Token(Type::none), __func__);																	// It will be an error (?)
	}
}

Translator::Node Translator::Parser::struct_or_union_specifier(int& curr_token) {
	int token_number = curr_token;
	int found = 0;	// Types: 0 - nothing found, 1 - found other token, 2 - found IDENTIFIER as last
	Node p_node(Token(Type::parsing), __func__);
	Node result;

	if(end) return Node(Token(Type::none), __func__);

	func_log(token_number, __func__);

	if(tokens[token_number].get_type() == Type::STRUCT) {
		p_node.push_back(Node(tokens[token_number], __func__));
		if(tokens[token_number + 1].get_type() == Type::IDENTIFIER) {
			p_node.push_back(Node(tokens[token_number + 1], __func__));
			if(tokens[token_number + 2].get_chars() == "{") {
				p_node.push_back(Node(tokens[token_number + 2], __func__));
				if((result = struct_declaration_list(token_number += 3)).get_type() != Type::none) {
					p_node.push_back(result);
					if(tokens[token_number].get_chars() == "}") {
						p_node.push_back(Node(tokens[token_number], __func__));
						found = 1;
					}
					else {}																	// It will be an error
				}
				else {}																		// It will be an error
			}
			found = 2;
		}
		else if(tokens[token_number + 1].get_chars() == "{") {
			p_node.push_back(Node(tokens[token_number + 1], __func__));
			if((result = struct_declaration_list(token_number += 2)).get_type() != Type::none) {
				p_node.push_back(result);
				if(tokens[token_number].get_chars() == "}") {
					p_node.push_back(Node(tokens[token_number], __func__));
					found = 1;
				}
				else {}																		// It will be an error
			}
			else {}																			// It will be an error
		}
		else {}																				// It will be an error
	}

	if(found) {
		curr_token = token_number + found;
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else {
		// std::cout << std::endl << "X: " << __func__;
		return Node(Token(Type::none), __func__);																		// It will be an error (?)
	}
}

Translator::Node Translator::Parser::struct_declaration_list(int& curr_token) {							// Recursive
	int token_number = curr_token;
	bool found = true;
	Node p_node(Token(Type::parsing), __func__);
	Node result;

	if(end) return Node(Token(Type::none), __func__);

	func_log(token_number, __func__);

	if((result = specifier_qualifier_list(token_number)).get_type() != Type::none) {
		p_node.push_back(result);
		if((result = struct_declarator_list(token_number)).get_type() != Type::none) {
			p_node.push_back(result);
			if(tokens[token_number].get_chars() == ";") {
				p_node.push_back(Node(tokens[token_number], __func__));
				if((result = struct_declaration_list(++token_number)).get_type() != Type::none) {
					p_node.push_back(result);
					found = true;
				}
				found = true;
			}
			else {}																			// It will be an error
		}
		else {}																				// It will be an error
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else {
		// std::cout << std::endl << "X: " << __func__;
		return Node(Token(Type::none), __func__);																		// It will be an error (?)
	}
}

Translator::Node Translator::Parser::specifier_qualifier_list(int& curr_token) {						// Recursive
	int token_number = curr_token;
	bool found = false;
	Node p_node(Token(Type::parsing), __func__);
	Node result;

	if(end) return Node(Token(Type::none), __func__);

	func_log(token_number, __func__);

	if((result = type_specifier(token_number)).get_type() != Type::none) {
		p_node.push_back(result);
		if((result = specifier_qualifier_list(token_number)).get_type() != Type::none) {
			p_node.push_back(result);
			found = true;
		}
		found = true; 
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else {
		// std::cout << std::endl << "X: " << __func__;
		return Node(Token(Type::none), __func__);															// It will be an error (?)
	}
}

Translator::Node Translator::Parser::struct_declarator_list(int& curr_token) {							// Recursive
	int token_number = curr_token;
	bool found = false;
	Node p_node(Token(Type::parsing), __func__);
	Node result;

	if(end) return Node(Token(Type::none), __func__);

	func_log(token_number, __func__);

	if((result = struct_declarator(token_number)).get_type() != Type::none) {
		p_node.push_back(result);
		if(tokens[token_number].get_chars() == ",") {
			p_node.push_back(Node(tokens[token_number], __func__));
			if((result = struct_declarator_list(++token_number)).get_type() != Type::none) {
				p_node.push_back(result);
				found = true;
			}
			else {}																			// It will be an error
		}
		found = true; 
	}
	
	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else {
		// std::cout << std::endl << "X: " << __func__;
		return Node(Token(Type::none), __func__);																		// It will be an error (?)
	}
}

Translator::Node Translator::Parser::struct_declarator(int& curr_token) {
	int token_number = curr_token;
	bool found = false;
	Node p_node(Token(Type::parsing), __func__);
	Node result;

	if(end) return Node(Token(Type::none), __func__);

	func_log(token_number, __func__);

	if((result = direct_declarator(token_number, false)).get_type() != Type::none) {
		p_node.push_back(result);
		if(tokens[token_number].get_chars() == ":") {
			p_node.push_back(Node(tokens[token_number], __func__));
			if((result = conditional_expression(++token_number)).get_type() != Type::none) {
				p_node.push_back(result);
				found = true;
			}
			else {}																// It will be an error
		}
		found = true;
	}
	else if(tokens[token_number].get_chars() == ":") {
		p_node.push_back(Node(tokens[token_number], __func__));
		if((result = conditional_expression(++token_number)).get_type() != Type::none) {
			p_node.push_back(result);
			found = true;
		}
		else {}																	// It will be an error
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else {
		// std::cout << std::endl << "X: " << __func__;
		return Node(Token(Type::none), __func__);															// It will be an error (?)
	}
}

Translator::Node Translator::Parser::enum_specifier(int& curr_token) {
	int token_number = curr_token;
	int found = 0; // Types: 0 - nothing found, 1 - found other token, 2 - found IDENTIFIER as last
	Node p_node(Token(Type::parsing), __func__);
	Node result;

	if(end) return Node(Token(Type::none), __func__);

	func_log(token_number, __func__);

	if(tokens[token_number].get_type() == Type::ENUM) {
		p_node.push_back(Node(tokens[token_number], __func__));
		if(tokens[token_number + 1].get_type() == Type::IDENTIFIER) {
			p_node.push_back(Node(tokens[token_number + 1], __func__));
			if(tokens[token_number + 2].get_chars() == "{") {
				p_node.push_back(Node(tokens[token_number + 2], __func__));
				if((result = enumerator_list(token_number += 3)).get_type() != Type::none) {
					p_node.push_back(result);
					if(tokens[token_number].get_chars() == "}") {
						p_node.push_back(Node(tokens[token_number], __func__));
						found = 1;
					}
					else {}																// It will be an error
				}
				else {}																	// It will be an error
			}
			found = 2;
		}
		else if(tokens[token_number + 1].get_chars() == "{") {
			p_node.push_back(Node(tokens[token_number + 1], __func__));
			if((result = enumerator_list(token_number += 2)).get_type() != Type::none) {
				p_node.push_back(result);
				if(tokens[token_number].get_chars() == "}") {
					p_node.push_back(Node(tokens[token_number], __func__));
					found = 1;
				}
				else {}																	// It will be an error
			}
			else {}																		// It will be an error
		}
		else {}																			// It will be an error
	}

	if(found) {
		curr_token = token_number + found;
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else {
		// std::cout << std::endl << "X: " << __func__;
		return Node(Token(Type::none), __func__);																	// It will be an error (?)
	}
}

Translator::Node Translator::Parser::enumerator_list(int& curr_token) {							// Recursive
	int token_number = curr_token;
	bool found = false;
	Node p_node(Token(Type::parsing), __func__);
	Node result;

	if(end) return Node(Token(Type::none), __func__);

	func_log(token_number, __func__);

	if((result = enumerator(token_number)).get_type() != Type::none) {
		p_node.push_back(result);
		if(tokens[token_number].get_chars() == ",") {
			p_node.push_back(Node(tokens[token_number], __func__));
			if((result = enumerator_list(++token_number)).get_type() != Type::none) {
				p_node.push_back(result);
				found = true;
			}
			else {}																	// It will be an error
		}
		found = true; 
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else {
		// std::cout << std::endl << "X: " << __func__;
		return Node(Token(Type::none), __func__);																// It will be an error (?)
	}
}

Translator::Node Translator::Parser::enumerator(int& curr_token) {
	int token_number = curr_token;
	int found = 0;	// Types: 0 - nothing found, 1 - found function, 2 - found token
	Node p_node(Token(Type::parsing), __func__);
	Node result;

	if(end) return Node(Token(Type::none), __func__);

	func_log(token_number, __func__);

	if(tokens[token_number].get_type() == Type::IDENTIFIER) {
		p_node.push_back(Node(tokens[token_number], __func__));
		if(tokens[token_number + 1].get_chars() == "=") {
			p_node.push_back(Node(tokens[token_number + 1], __func__));
			if((result = conditional_expression(token_number += 2)).get_type() != Type::none) {
				p_node.push_back(result);
				found = 1;
			}
			else {}																		// It will be an error
		}
		found = 2;
	}

	if(found) {
		curr_token = token_number + found - 1;
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else {
		// std::cout << std::endl << "X: " << __func__;
		return Node(Token(Type::none), __func__);																	// It will be an error (?)
	}
}

Translator::Node Translator::Parser::direct_declarator(int& curr_token, bool recursive) {							// Recursive
	int token_number = curr_token;
	bool found = false;
	Node p_node(Token(Type::parsing), __func__);
	Node result;

	if(end) return Node(Token(Type::none), __func__);

	func_log(token_number, __func__);

	if(!recursive) {
		if(tokens[token_number].get_type() == Type::IDENTIFIER) {
			p_node.push_back(Node(tokens[token_number], __func__));
			if((result = direct_declarator(++token_number, true)).get_type() != Type::none) {
				p_node.push_back(result);
				found = true;
			}
			found = true;
		}
		else if(tokens[token_number].get_chars() == "(") {
			p_node.push_back(Node(tokens[token_number], __func__));
			if((result = direct_declarator(++token_number, false)).get_type() != Type::none) {
				p_node.push_back(result);
				if(tokens[token_number].get_chars() == ")") {
					p_node.push_back(Node(tokens[token_number], __func__));
					if((result = direct_declarator(++token_number, true)).get_type() != Type::none) {
						p_node.push_back(result);
						found = true;
					}
					found = true;
				}
				else {}																		// It will be an error
			}
			else {}																			// It will be an error
		}
	}
	else {
		if(tokens[token_number].get_chars() == "[") {
			p_node.push_back(Node(tokens[token_number], __func__));
			if(tokens[token_number + 1].get_chars() == "]") {
				p_node.push_back(Node(tokens[token_number + 1], __func__));
				if((result = direct_declarator(token_number += 2, true)).get_type() != Type::none) {
					p_node.push_back(result);
					found = true;
				}
				found = true;
			}
			else if((result = conditional_expression(++token_number)).get_type() != Type::none) {
				p_node.push_back(result);
				if(tokens[token_number].get_chars() == "]") {
					p_node.push_back(Node(tokens[token_number], __func__));
					if((result = direct_declarator(++token_number, true)).get_type() != Type::none) {
						p_node.push_back(result);
						found = true;
					}
					found = true;
				}
				else {}																		// It will be an error
			}
			else {}																			// It will be an error
		}
		else if(tokens[token_number].get_chars() == "(") {
			p_node.push_back(Node(tokens[token_number], __func__));
			if(tokens[token_number + 1].get_chars() == ")") {
				p_node.push_back(Node(tokens[token_number + 1], __func__));
				if((result = direct_declarator(token_number += 2, true)).get_type() != Type::none) {
					p_node.push_back(result);
					found = true;
				}
				found = true;
			}
			else if((result = parameter_list(++token_number)).get_type() != Type::none) {
				p_node.push_back(result);
				if(tokens[token_number].get_chars() == ")") {
					p_node.push_back(Node(tokens[token_number], __func__));
					if((result = direct_declarator(++token_number, true)).get_type() != Type::none) {
						p_node.push_back(result);
						found = true;
					}
					found = true;
				}
				else {}																		// It will be an error
			}

			if(!found) {
				token_number = curr_token;
				if((result = identifier_list(++token_number)).get_type() != Type::none) {
					p_node.push_back(result);
					if(tokens[token_number].get_chars() == ")") {
						p_node.push_back(Node(tokens[token_number], __func__));
						if((result = direct_declarator(++token_number, true)).get_type() != Type::none) {
							p_node.push_back(result);
							found = true;
						}
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
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else {
		// std::cout << std::endl << "X: " << __func__;
		return Node(Token(Type::none), __func__);																		// It will be an error (?)
	}
}

Translator::Node Translator::Parser::parameter_list(int& curr_token) {					// Recursive
	int token_number = curr_token;
	bool found = false;
	Node p_node(Token(Type::parsing), __func__);
	Node result;

	if(end) return Node(Token(Type::none), __func__);

	func_log(token_number, __func__);

	if((result = parameter_declaration(token_number)).get_type() != Type::none) {
		p_node.push_back(result);
		if(tokens[token_number].get_chars() == ",") {
			p_node.push_back(Node(tokens[token_number], __func__));
			if((result = parameter_list(++token_number)).get_type() != Type::none) {
				p_node.push_back(result);
				found = true;
			}
			else {}																// It will be an error
		}
		found = true; 
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else {
		// std::cout << std::endl << "X: " << __func__;
		return Node(Token(Type::none), __func__);															// It will be an error (?)
	}
}

Translator::Node Translator::Parser::parameter_declaration(int& curr_token) {
	int token_number = curr_token;
	bool found = false;
	Node p_node(Token(Type::parsing), __func__);
	Node result;

	if(end) return Node(Token(Type::none), __func__);

	func_log(token_number, __func__);

	if((result = declaration_specifiers(token_number)).get_type() != Type::none) {
		p_node.push_back(result);
		if((result = direct_declarator(token_number, false)).get_type() != Type::none) {
			p_node.push_back(result);
			found = true;
		}
		else if((result = direct_abstract_declarator(token_number, false)).get_type() != Type::none) {
			p_node.push_back(result);
			found = true;
		}
		found = true;
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else {
		// std::cout << std::endl << "X: " << __func__;
		return Node(Token(Type::none), __func__);															// It will be an error (?)
	}
}

Translator::Node Translator::Parser::identifier_list(int& curr_token) {						// Recursive
	int token_number = curr_token;
	int found = 0;	// Types: 0 - nothing found, 1 - end of recursion, 2 - ends with token
	Node p_node(Token(Type::parsing), __func__);
	Node result;

	if(end) return Node(Token(Type::none), __func__);

	func_log(token_number, __func__);

	if(tokens[token_number].get_type() == Type::IDENTIFIER) {
		p_node.push_back(Node(tokens[token_number], __func__));
		if(tokens[token_number + 1].get_chars() == ",") {
			p_node.push_back(Node(tokens[token_number + 1], __func__));
			if((result = identifier_list(token_number += 2)).get_type() != Type::none) {
				p_node.push_back(result);
				found = 1;
			}
			else {}															// It will be an error
		}
		found = 2;
	}

	if(found) {
		curr_token = token_number + found - 1;
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else {
		// std::cout << std::endl << "X: " << __func__;
		return Node(Token(Type::none), __func__);														// It will be an error (?)
	}
}

Translator::Node Translator::Parser::type_name(int& curr_token) {
	int token_number = curr_token;
	bool found = false;
	Node p_node(Token(Type::parsing), __func__);
	Node result;

	if(end) return Node(Token(Type::none), __func__);

	func_log(token_number, __func__);

	if((result = specifier_qualifier_list(token_number)).get_type() != Type::none) {
		p_node.push_back(result);
		if((result = direct_abstract_declarator(token_number, false)).get_type() != Type::none) {
			p_node.push_back(result);
			found = true;
		}
		found = true;
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else {
		// std::cout << std::endl << "X: " << __func__;
		return Node(Token(Type::none), __func__);																	// It will be an error (?)
	}
}

Translator::Node Translator::Parser::direct_abstract_declarator(int& curr_token, bool recursive) {								// Recursive
	int token_number = curr_token;
	bool found = false;
	Node p_node(Token(Type::parsing), __func__);
	Node result;

	if(end) return Node(Token(Type::none), __func__);

	func_log(token_number, __func__);

	if(tokens[token_number].get_chars() == "(") {
		p_node.push_back(Node(tokens[token_number], __func__));
		if(!recursive) {
			if((result = direct_abstract_declarator(++token_number, false)).get_type() != Type::none) {
				p_node.push_back(result);
				if(tokens[token_number].get_chars() == ")") {
					p_node.push_back(Node(tokens[token_number], __func__));
					if((result = direct_abstract_declarator(++token_number, true)).get_type() != Type::none) {
						p_node.push_back(result);
						found = true;
					}
					found = true;
				}
				else {}																		// It will be an error
			}
			else {}																			// It will be an error
		}
		else {
			if(tokens[token_number + 1].get_chars() == ")") {
				p_node.push_back(Node(tokens[token_number + 1], __func__));
				if((result = direct_abstract_declarator(token_number += 2, true)).get_type() != Type::none) {
					p_node.push_back(result);
					found = true;
				}
				found = true;
			}
			else if((result = parameter_list(++token_number)).get_type() != Type::none) {
				p_node.push_back(result);
				if(tokens[token_number].get_chars() == ")") {
					p_node.push_back(Node(tokens[token_number], __func__));
					if((result = direct_abstract_declarator(++token_number, true)).get_type() != Type::none) {
						p_node.push_back(result);
						found = true;
					}
					found = true;
				}
				else {}																		// It will be an error
			}
			else {}																			// It will be an error
		}
	}
	else if(tokens[token_number].get_chars() == "[") {
		p_node.push_back(Node(tokens[token_number], __func__));
		if(tokens[token_number + 1].get_chars() == "]") {
			p_node.push_back(Node(tokens[token_number + 1], __func__));
			if((result = direct_abstract_declarator(token_number += 2, true)).get_type() != Type::none) {
				p_node.push_back(result);
				found = true;
			}
			found = true;
		}
		else if((result = conditional_expression(++token_number)).get_type() != Type::none) {
			p_node.push_back(result);
			if(tokens[token_number].get_chars() == "]") {
				p_node.push_back(Node(tokens[token_number], __func__));
				if((result = direct_abstract_declarator(++token_number, true)).get_type() != Type::none) {
					p_node.push_back(result);
					found = true;
				}
				found = true;
			}
			else {}																			// It will be an error
		}
		else {}																				// It will be an error
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else {
		// std::cout << std::endl << "X: " << __func__;
		return Node(Token(Type::none), __func__);																		// It will be an error (?)
	}
}

Translator::Node Translator::Parser::initializer(int& curr_token) {
	int token_number = curr_token;
	int found = 0;	// Types: 0 - nothing found, 1 - found function, 2 - found token
	Node p_node(Token(Type::parsing), __func__);
	Node result;

	if(end) return Node(Token(Type::none), __func__);

	func_log(token_number, __func__);

	if((result = assignment_expression(token_number)).get_type() != Type::none) {
		p_node.push_back(result);
		found = 1;
	}
	else if(tokens[token_number].get_chars() == "{") {
		p_node.push_back(Node(tokens[token_number], __func__));
		if((result = initializer_list(++token_number)).get_type() != Type::none) {
			p_node.push_back(result);
			if(tokens[token_number].get_chars() == "}") {
				p_node.push_back(Node(tokens[token_number], __func__));
				found = 2;
			}
			else {}																		// It will be an error
		}
		else {}																			// It will be an error
	}

	if(found) {
		curr_token = token_number + found - 1;
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else {
		// std::cout << std::endl << "X: " << __func__;
		return Node(Token(Type::none), __func__);																	// It will be an error (?)
	}
}

Translator::Node Translator::Parser::initializer_list(int& curr_token) {			// Recursive
	int token_number = curr_token;
	bool found = false;
	Node p_node(Token(Type::parsing), __func__);
	Node result;

	if(end) return Node(Token(Type::none), __func__);

	func_log(token_number, __func__);

	if((result = initializer(token_number)).get_type() != Type::none) {
		p_node.push_back(result);
		if(tokens[token_number].get_chars() == ",") {
			p_node.push_back(Node(tokens[token_number], __func__));
			if((result = initializer_list(++token_number)).get_type() != Type::none) {
				p_node.push_back(result);
				found = true;
			}
			else {}															// It will be an error
		}
		found = true; 
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else {
		// std::cout << std::endl << "X: " << __func__;
		return Node(Token(Type::none), __func__);														// It will be an error (?)
	}
}

Translator::Node Translator::Parser::statement(int& curr_token) {
	int token_number = curr_token;
	bool found = false;
	Node p_node(Token(Type::parsing), __func__);
	Node result;

	if(end) return Node(Token(Type::none), __func__);

	func_log(token_number, __func__);

	if((result = labeled_statement(token_number)).get_type() != Type::none) {
		p_node.push_back(result);
		found = true;
	}
	else if((result = compound_statement(token_number)).get_type() != Type::none) {
		p_node.push_back(result);
		found = true;
	}
	else if((result = expression_statement(token_number)).get_type() != Type::none) {
		p_node.push_back(result);
		found = true;
	}
	else if((result = selection_statement(token_number)).get_type() != Type::none) {
		p_node.push_back(result);
		found = true;
	}
	else if((result = iteration_statement(token_number)).get_type() != Type::none) {
		p_node.push_back(result);
		found = true;
	}
	else if((result = jump_statement(token_number)).get_type() != Type::none) {
		p_node.push_back(result);
		found = true;
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else {
		// std::cout << std::endl << "X: " << __func__;
		return Node(Token(Type::none), __func__);														// It will be an error (?)
	}
}

Translator::Node Translator::Parser::labeled_statement(int& curr_token) {
	int token_number = curr_token;
	bool found = false;
	Node p_node(Token(Type::parsing), __func__);
	Node result;

	if(end) return Node(Token(Type::none), __func__);

	func_log(token_number, __func__);

	if(tokens[token_number].get_type() == Type::CASE) {
		p_node.push_back(Node(tokens[token_number], __func__));
		if((result = conditional_expression(++token_number)).get_type() != Type::none) {
			p_node.push_back(result);
			if(tokens[token_number].get_chars() == ":") {
				p_node.push_back(Node(tokens[token_number], __func__));
				if((result = statement(++token_number)).get_type() != Type::none) {
					p_node.push_back(result);
					found = true;
				}
				else {}															// It will be an error
			}
			else {}																// It will be an error
		}
		else {}																	// It will be an error
	}
	else if(tokens[token_number].get_type() == Type::DEFAULT) {
		p_node.push_back(Node(tokens[token_number], __func__));
		if(tokens[token_number + 1].get_chars() == ":") {
			p_node.push_back(Node(tokens[token_number + 1], __func__));
			if((result = statement(token_number += 2)).get_type() != Type::none) {
				p_node.push_back(result);
				found = true;
			}
			else {}															// It will be an error
		}
		else {}																// It will be an error
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else {
		// std::cout << std::endl << "X: " << __func__;
		return Node(Token(Type::none), __func__);															// It will be an error (?)
	}
}

Translator::Node Translator::Parser::compound_statement(int& curr_token) {
	int token_number = curr_token;
	int found = 0;	// Types: 0 - nothing found, 1 - found parentesis with function, 2 - found only parentesis
	Node p_node(Token(Type::parsing), __func__);
	Node result;

	if(end) return Node(Token(Type::none), __func__);

	func_log(token_number, __func__);

	if(tokens[token_number].get_chars() == "{" ) {
		p_node.push_back(Node(tokens[token_number], __func__));
		if(tokens[token_number + 1].get_chars() == "}") {
			p_node.push_back(Node(tokens[token_number], __func__));
			found = 2;
		}
		else if((result = statement_list(++token_number)).get_type() != Type::none) {
			p_node.push_back(result);
			if(tokens[token_number].get_chars() == "}") {
				p_node.push_back(Node(tokens[token_number], __func__));
				found = 1;
			}
			else {}																	// It will be an error
		}

		if(!found) {
			token_number = curr_token;
			if((result = declaration_list(++token_number)).get_type() != Type::none) {
				p_node.push_back(result);
				if(tokens[token_number].get_chars() == "}") {
					p_node.push_back(Node(tokens[token_number], __func__));
					found = 1;
				}
				else if((result = statement_list(token_number)).get_type() != Type::none) {
					p_node.push_back(result);
					if(tokens[token_number].get_chars() == "}") {
						p_node.push_back(Node(tokens[token_number], __func__));
						found = 1;
					}
					else {}															// It will be an error
				}
				else {}																// It will be an error
			}
			else {}																	// It will be an error
		}
	}

	if(found) {
		curr_token = token_number + found;
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else {
		// std::cout << std::endl << "X: " << __func__;
		return Node(Token(Type::none), __func__);																// It will be an error (?)
	}
}

Translator::Node Translator::Parser::declaration_list(int& curr_token) {			// Recursive
	int token_number = curr_token;
	bool found = false;
	Node p_node(Token(Type::parsing), __func__);
	Node result;

	if(end) return Node(Token(Type::none), __func__);

	func_log(token_number, __func__);

	if((result = declaration(token_number)).get_type() != Type::none) {
		p_node.push_back(result);
		if((result = declaration_list(token_number)).get_type() != Type::none) {
			p_node.push_back(result);
			found = true;
		}
		found = true; 
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else {
		// std::cout << std::endl << "X: " << __func__;
		return Node(Token(Type::none), __func__);													// It will be an error (?)
	}
}

Translator::Node Translator::Parser::statement_list(int& curr_token) {				// Recursive
	int token_number = curr_token;
	bool found = false;
	Node p_node(Token(Type::parsing), __func__);
	Node result;

	if(end) return Node(Token(Type::none), __func__);

	func_log(token_number, __func__);

	if((result = statement(token_number)).get_type() != Type::none) {
		p_node.push_back(result);
		if((result = statement_list(token_number)).get_type() != Type::none) {
			p_node.push_back(result);
			found = true;
		}
		found = true; 
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else {
		// std::cout << std::endl << "X: " << __func__;
		return Node(Token(Type::none), __func__);													// It will be an error (?)
	}
}

Translator::Node Translator::Parser::expression_statement(int& curr_token) {
	int token_number = curr_token;
	bool found = false;
	Node p_node(Token(Type::parsing), __func__);
	Node result;

	if(end) return Node(Token(Type::none), __func__);

	func_log(token_number, __func__);

	if(tokens[token_number].get_chars() == ";") {
		p_node.push_back(Node(tokens[token_number], __func__));
		found = true;
	}
	else if((result = expression(token_number)).get_type() != Type::none) {
		p_node.push_back(result);
		if(tokens[token_number].get_chars() == ";") {
			p_node.push_back(Node(tokens[token_number], __func__));
			found = true;
		}
		else {}																// It will be an error
	}

	if(found) {
		curr_token = token_number + 1;
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else {
		// std::cout << std::endl << "X: " << __func__;
		return Node(Token(Type::none), __func__);														// It will be an error (?)
	}
}

Translator::Node Translator::Parser::selection_statement(int& curr_token) {
	int token_number = curr_token;
	bool found = false;
	Node p_node(Token(Type::parsing), __func__);
	Node result;

	if(end) return Node(Token(Type::none), __func__);

	func_log(token_number, __func__);

	if(tokens[token_number].get_type() == Type::IF) {
		p_node.push_back(Node(tokens[token_number], __func__));
		if(tokens[token_number + 1].get_chars() == "(") {
			p_node.push_back(Node(tokens[token_number + 1], __func__));
			if((result = expression(token_number += 2)).get_type() != Type::none) {
				p_node.push_back(result);
				if(tokens[token_number].get_chars() == ")") {
					p_node.push_back(Node(tokens[token_number], __func__));
					if((result = statement(++token_number)).get_type() != Type::none) {
						p_node.push_back(result);
						if(tokens[token_number].get_type() == Type::ELSE) {
							p_node.push_back(Node(tokens[token_number], __func__));
							if((result = statement(++token_number)).get_type() != Type::none) {
								p_node.push_back(result);
								found = true;
							}
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
		p_node.push_back(Node(tokens[token_number], __func__));
		if(tokens[token_number + 1].get_chars() == "(") {
			p_node.push_back(Node(tokens[token_number + 1], __func__));
			if((result = expression(token_number += 2)).get_type() != Type::none) {
				p_node.push_back(result);
				if(tokens[token_number].get_chars() == ")") {
					p_node.push_back(Node(tokens[token_number], __func__));
					if((result = statement(++token_number)).get_type() != Type::none) {
						p_node.push_back(result);
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

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else {
		// std::cout << std::endl << "X: " << __func__;
		return Node(Token(Type::none), __func__);																// It will be an error (?)
	}
}

Translator::Node Translator::Parser::iteration_statement(int& curr_token) {
	int token_number = curr_token;
	int found = 0;	// Types: 0 - nothing found, 1 - found function, 2 - found token
	Node p_node(Token(Type::parsing), __func__);
	Node result;

	if(end) return Node(Token(Type::none), __func__);

	func_log(token_number, __func__);

	if(tokens[token_number].get_type() == Type::WHILE) {
		p_node.push_back(Node(tokens[token_number], __func__));
		if(tokens[token_number + 1].get_chars() == "(") {
			p_node.push_back(Node(tokens[token_number + 1], __func__));
			if((result = expression(token_number += 2)).get_type() != Type::none) {
				p_node.push_back(result);
				if(tokens[token_number].get_chars() == ")") {
					p_node.push_back(Node(tokens[token_number], __func__));
					if((result = statement(++token_number)).get_type() != Type::none) {
						p_node.push_back(result);
						found = 1;
					}
					else {}																// It will be an error
				}
				else {}																	// It will be an error
			}
			else {}																		// It will be an error
		}
		else {}																			// It will be an error
	}
	else if(tokens[token_number].get_type() == Type::DO) {
		p_node.push_back(Node(tokens[token_number], __func__));
		if((result = statement(++token_number)).get_type() != Type::none) {
			p_node.push_back(result);
			if(tokens[token_number].get_type() == Type::WHILE) {
				p_node.push_back(Node(tokens[token_number], __func__));
				if(tokens[token_number + 1].get_chars() == "(") {
					p_node.push_back(Node(tokens[token_number + 1], __func__));
					if((result = expression(token_number += 2)).get_type() != Type::none) {
						p_node.push_back(result);
						if(tokens[token_number].get_chars() == ")") {
							p_node.push_back(Node(tokens[token_number], __func__));
							if(tokens[token_number + 1].get_chars() == ";") {
								p_node.push_back(Node(tokens[token_number + 1], __func__));
								found = 2;
							}
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
		p_node.push_back(Node(tokens[token_number], __func__));
		if(tokens[token_number + 1].get_chars() == "(") {
			p_node.push_back(Node(tokens[token_number + 1], __func__));
			if((result = expression_statement(token_number += 2)).get_type() != Type::none) {
				p_node.push_back(result);
				if((result = expression_statement(token_number)).get_type() != Type::none) {
					p_node.push_back(result);
					if(tokens[token_number].get_chars() == ")") {
						p_node.push_back(Node(tokens[token_number], __func__));
						if((result = statement(++token_number)).get_type() != Type::none) {
							p_node.push_back(result);
							found = 1;
						}
						else {}															// It will be an error
					}
					else if((result = expression(token_number)).get_type() != Type::none) {
						p_node.push_back(result);
						if(tokens[token_number].get_chars() == ")") {
							p_node.push_back(Node(tokens[token_number], __func__));
							if((result = statement(++token_number)).get_type() != Type::none) {
								p_node.push_back(result);
								found = 1;
							}
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
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else if(found == 1) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else {
		// std::cout << std::endl << "X: " << __func__;
		return Node(Token(Type::none), __func__);																	// It will be an error (?)
	}
}

Translator::Node Translator::Parser::jump_statement(int& curr_token) {
	int token_number = curr_token;
	int found = 0;	// Types: 0 - nothing found, 1 - found token after function, 2 - found only token
	Node p_node(Token(Type::parsing), __func__);
	Node result;

	if(end) return Node(Token(Type::none), __func__);

	func_log(token_number, __func__);

	if(tokens[token_number].get_type() == Type::BREAK) {
		p_node.push_back(Node(tokens[token_number], __func__));
		if(tokens[token_number + 1].get_chars() == ";") {
			p_node.push_back(Node(tokens[token_number + 1], __func__));
			found = 2;
		}
		else {}															// It will be an error
	}
	else if(tokens[token_number].get_type() == Type::RETURN) {
		p_node.push_back(Node(tokens[token_number], __func__));
		if(tokens[token_number + 1].get_chars() == ";") {
			p_node.push_back(Node(tokens[token_number + 1], __func__));
			found = 2;
		}
		else if((result = expression(++token_number)).get_type() != Type::none) {
			p_node.push_back(result);
			if(tokens[token_number].get_chars() == ";") {
				p_node.push_back(Node(tokens[token_number], __func__));
				found = 1;
			}
			else {}														// It will be an error
		}
		else {}															// It will be an error
	}

	if(found) {
		curr_token = token_number + found;
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else {
		// std::cout << std::endl << "X: " << __func__;
		return Node(Token(Type::none), __func__);													// It will be an error (?)
	}
}

Translator::Node Translator::Parser::translation_unit(int curr_token) {			// Recursive
	int token_number = curr_token;
	bool found = false;
	Node p_node(Token(Type::parsing), __func__);
	Node result;

	if(end) return Node(Token(Type::none), __func__);

	func_log(token_number, __func__);

	if((result = external_declaration(token_number)).get_type() != Type::none) {
		p_node.push_back(result);
		if((result = translation_unit(token_number)).get_type() != Type::none) {
			p_node.push_back(result);
			found = true;
		}
		found = true; 
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else {
		// std::cout << std::endl << "X: " << __func__;
		return Node(Token(Type::none), __func__);													// It will be an error (?)
	}
}

Translator::Node Translator::Parser::external_declaration(int& curr_token) {
	int token_number = curr_token;
	bool found = false;
	Node p_node(Token(Type::parsing), __func__);
	Node result;

	if(end) return Node(Token(Type::none), __func__);

	func_log(token_number, __func__);

	if((result = function_definition(token_number)).get_type() != Type::none) {
		p_node.push_back(result);
		found = true;
	}
	else if((result = declaration(token_number)).get_type() != Type::none) {
		p_node.push_back(result);
		found = true;
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else {
		// std::cout << std::endl << "X: " << __func__;
		return Node(Token(Type::none), __func__);												// It will be an error (?)
	}
}

Translator::Node Translator::Parser::function_definition(int& curr_token) {
	int token_number = curr_token;
	bool found = false;
	Node p_node(Token(Type::parsing), __func__);
	Node result;

	if(end) return Node(Token(Type::none), __func__);

	func_log(token_number, __func__);

	if((result = declaration_specifiers(token_number)).get_type() != Type::none) {
		p_node.push_back(result);
		if((result = direct_declarator(token_number, false)).get_type() != Type::none) {
			p_node.push_back(result);
			if((result = declaration_list(token_number)).get_type() != Type::none) {
				p_node.push_back(result);
				if((result = compound_statement(token_number)).get_type() != Type::none) {
					p_node.push_back(result);
					found = true;
				}
				else {}																	// It will be an error
			}
			else if((result = compound_statement(token_number)).get_type() != Type::none) {
				p_node.push_back(result);
				found = true;
			}
			else {}																		// It will be an error
		}
		else {}																			// It will be an error
	}
	else if((result = direct_declarator(token_number, false)).get_type() != Type::none) {
		p_node.push_back(result);
		if((result = declaration_list(token_number)).get_type() != Type::none) {
			p_node.push_back(result);
			if((result = compound_statement(token_number)).get_type() != Type::none) {
				p_node.push_back(result);
				found = true;
			}
			else {}																		// It will be an error
		}
		else if((result = compound_statement(token_number)).get_type() != Type::none) {
			p_node.push_back(result);
			found = true;
		}
		else {}																			// It will be an error
	}

	if(found) {
		curr_token = token_number;
		if(tokens[curr_token].get_type() == Type::eof) end = true; 
		return p_node;
	}
	else {
		// std::cout << std::endl << "X: " << __func__;
		return Node(Token(Type::none), __func__);																	// It will be an error (?)
	}
}