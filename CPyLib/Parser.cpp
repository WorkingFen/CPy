#include "Parser.h"

namespace Translator {
    void Parser::func_log(const char* func) {
    #ifdef _DEBUG
	    std::cout << std::endl << offset.top() << ": " << tokens[offset.top()].get_chars() << " -> " << func;
    #endif
    }

    void Parser::require(bool condition, std::string error_msg) {
        if(!condition)
            throw Parser_Exception(error_msg);
    }

    void Parser::commit() {
        int token_number = offset.top();
        offset.pop();
        offset.top() = token_number;
    }

    Node Parser::undo(const char* func) {
        offset.pop();
        return Node(Token(), func);
    }

    void Parser::start_parsing() {
        offset.emplace(0);
        func_log(__func__);

	    try {
		    root = translation_unit();
	    }
	    catch(Parser_Exception e) {
		    std::cerr << e.what() << std::endl;
	    }
	    std::cout << std::endl << "Parsing finished";
    }

    Node Parser::primary_expression() {
        prepare();
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    switch(tokens[offset.top()].get_type()) {
		    case Type::IDENTIFIER:
		    case Type::CONSTANT:
		    case Type::STRING_LITERAL:
			    p_node.push_back(Node(tokens[offset.top()], __func__, tokens[offset.top()].get_chars()));
                ++offset.top();
			    break;
		    default: {
			    if(check_char(offset.top(), "(")) {
				    p_node.push_back(Node(tokens[offset.top()++], __func__));
                    require(!check_type(result = expression()));
					p_node.push_back(result);
                    require(check_char(offset.top(), ")"));
					p_node.push_back(Node(tokens[offset.top()++], __func__));
			    }
                else
                    return undo(__func__);
			    break;
		    }
	    }
        commit();
		return p_node;
    }

    Node Parser::postfix_expression() {			// Recursive
        prepare();
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

		if(!check_type(result = primary_expression())) {
			p_node.push_back(result);
			if(!check_type(result = postfix_expression_rec()))
				p_node.push_back(result);
		}
        else
            return undo(__func__);

        commit();
		return p_node;
    }

    Node Parser::postfix_expression_rec() {
        prepare();
        Node p_node(Token(Type::parsing), __func__);
        Node result;

        func_log(__func__);

        if(check_char(offset.top(), "(")) {
            p_node.push_back(Node(tokens[offset.top()], __func__, "("));
            if(check_char(++offset.top(), ")")) {
                p_node.push_back(Node(tokens[offset.top()++], __func__, ")"));
                if(!check_type(result = postfix_expression_rec()))
                    p_node.push_back(result);
            }
            else {
                require(!check_type(result = argument_expression_list()));
                p_node.push_back(result);
                require(check_char(offset.top(), ")"));
                p_node.push_back(Node(tokens[offset.top()++], __func__, ")"));
                if(!check_type(result = postfix_expression_rec()))
                    p_node.push_back(result);
            }
        }
        else if(check_char(offset.top(), ".")) {
            p_node.push_back(Node(tokens[offset.top()], __func__, "."));
            require(check_type(++offset.top(), Type::IDENTIFIER));
            p_node.push_back(Node(tokens[offset.top()], __func__, tokens[offset.top()].get_chars()));
            ++offset.top();
            if(!check_type(result = postfix_expression_rec()))
                p_node.push_back(result);
        }
        else if(check_type(offset.top(), Type::INC_OP)) {
            p_node.push_back(Node(tokens[offset.top()++], __func__, "++"));
            if(!check_type(result = postfix_expression_rec()))
                p_node.push_back(result);
        }
        else if(check_type(offset.top(), Type::DEC_OP)) {
            p_node.push_back(Node(tokens[offset.top()++], __func__, "--"));
            if(!check_type(result = postfix_expression_rec()))
                p_node.push_back(result);
        }
        else if(check_char(offset.top(), "[")) {
            p_node.push_back(Node(tokens[offset.top()++], __func__, "["));
            require(!check_type(result = expression()));
            p_node.push_back(result);
            require(check_char(offset.top(), "]"));
            p_node.push_back(Node(tokens[offset.top()++], __func__, "]"));
            if(!check_type(result = postfix_expression_rec()))
                p_node.push_back(result);
        }
        else
            return undo(__func__);

        commit();
        return p_node;
    }

    Node Parser::argument_expression_list() {	// Recursive
        prepare();
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = assignment_expression())) {
		    p_node.push_back(result);
		    if(check_char(offset.top(), ",")) {
			    p_node.push_back(Node(tokens[offset.top()++], __func__, ", "));
                require(!check_type(result = argument_expression_list()));
				p_node.push_back(result);
		    }
	    }
        else
            return undo(__func__);

        commit();
		return p_node;
    }

    Node Parser::unary_expression() {				// Recursive
        prepare();
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = postfix_expression())) {
		    p_node.push_back(result);
	    }
	    else if(check_type(offset.top(), Type::INC_OP) || check_type(offset.top(), Type::DEC_OP)) {
		    p_node.push_back(Node(tokens[offset.top()], __func__, tokens[offset.top()].get_chars()));
            ++offset.top();
            require(!check_type(result = unary_expression()));
			p_node.push_back(result);
	    }
	    else if(!check_type(result = unary_operator())) {
		    p_node.push_back(result);
            require(!check_type(result = cast_expression()));
			p_node.push_back(result);
	    }
        else
            return undo(__func__);

        commit();
		return p_node;
    }

    Node Parser::unary_operator() {
        prepare();
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

        func_log(__func__);

	    std::string tokens_chars = tokens[offset.top()].get_chars();

	    if(tokens_chars == "*" || tokens_chars == "+" || tokens_chars == "-")
		    p_node.push_back(Node(tokens[offset.top()++], __func__, tokens_chars));
        else if(tokens_chars == "!")
            p_node.push_back(Node(tokens[offset.top()++], __func__, "not "));
        else
            return undo(__func__);

        commit();
		return p_node;
    }

    Node Parser::cast_expression() {					// Recursive
        prepare();
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = unary_expression())) {
		    p_node.push_back(result);
	    }
	    else if(check_char(offset.top(), "(")) {
		    p_node.push_back(Node(tokens[offset.top()++], __func__, "("));
            require(!check_type(result = type_name()));
			p_node.push_back(result);
            require(check_char(offset.top(), ")"));
			p_node.push_back(Node(tokens[offset.top()++], __func__, ")"));
            require(!check_type(result = cast_expression()));
			p_node.push_back(result);
	    }
        else
            return undo(__func__);

        commit();
		return p_node;
    }

    Node Parser::multiplicative_expression() {		// Recursive
        prepare();
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = cast_expression())) {
		    p_node.push_back(result);
            std::string tokens_chars = tokens[offset.top()].get_chars();
		    if(tokens_chars == "*" || tokens_chars == "/" || tokens_chars == "%") {
			    p_node.push_back(Node(tokens[offset.top()++], __func__, tokens_chars));
                require(!check_type(result = multiplicative_expression()));
				p_node.push_back(result);
		    }
	    }
        else
            return undo(__func__);

        commit();
		return p_node;
    }

    Node Parser::additive_expression() {				// Recursive
        prepare();
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = multiplicative_expression())) {
		    p_node.push_back(result);
		    if(check_char(offset.top(), "+") || check_char(offset.top(), "-")) {
			    p_node.push_back(Node(tokens[offset.top()], __func__, tokens[offset.top()].get_chars()));
                ++offset.top();
                require(!check_type(result = additive_expression()));
				p_node.push_back(result);
		    }
	    }
        else
            return undo(__func__);

        commit();
		return p_node;
    }

    Node Parser::relational_expression() {			// Recursive
        prepare();
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = additive_expression())) {
		    p_node.push_back(result);
		    if(check_char(offset.top(), "<") || check_char(offset.top(), ">") ||
			        check_type(offset.top(), Type::LE_OP) || check_type(offset.top(), Type::GE_OP)) {
			    p_node.push_back(Node(tokens[offset.top()], __func__, tokens[offset.top()].get_chars()));
                ++offset.top();
                require(!check_type(result = relational_expression()));
				p_node.push_back(result);
		    }
	    }
        else
            return undo(__func__);

        commit();
		return p_node;
    }

    Node Parser::equality_expression() {				// Recursive
        prepare();
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = relational_expression())) {
		    p_node.push_back(result);
		    if(check_type(offset.top(), Type::EQ_OP) || check_type(offset.top(), Type::NE_OP)) {
			    p_node.push_back(Node(tokens[offset.top()], __func__, tokens[offset.top()].get_chars()));
                ++offset.top();
                require(!check_type(result = equality_expression()));
				p_node.push_back(result);
		    }
	    }
        else
            return undo(__func__);

        commit();
		return p_node;
    }

    Node Parser::logical_and_expression() {			// Recursive
        prepare();
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = equality_expression())) {
		    p_node.push_back(result);
		    if(check_type(offset.top(), Type::AND_OP)) {
			    p_node.push_back(Node(tokens[offset.top()++], __func__, " and "));
                require(!check_type(result = logical_and_expression()));
				p_node.push_back(result);
		    }
	    }
        else
            return undo(__func__);

        commit();
		return p_node;
    }

    Node Parser::logical_or_expression() {			// Recursive
        prepare();
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = logical_and_expression())) {
		    p_node.push_back(result);
		    if(check_type(offset.top(), Type::OR_OP)) {
			    p_node.push_back(Node(tokens[offset.top()++], __func__, " or "));
                require(!check_type(result = logical_or_expression()));
				p_node.push_back(result);
		    }
	    }
        else
            return undo(__func__);

		commit();
		return p_node;
    }

    Node Parser::conditional_expression() {			// Recursive
	    prepare();
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = logical_or_expression())) {
		    p_node.push_back(result);
		    if(check_char(offset.top(), "?")) {
			    p_node.push_back(Node(tokens[offset.top()++], __func__));
                require(!check_type(result = expression()));
				p_node.push_back(result);
                require(check_char(offset.top(), ":"));
				p_node.push_back(Node(tokens[offset.top()++], __func__));
                require(!check_type(result = conditional_expression()));
				p_node.push_back(result);
		    }
	    }
        else
            return undo(__func__);

		commit();
		return p_node;
    }

    Node Parser::assignment_expression() {			// Recursive
	    prepare();
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = conditional_expression())) {
		    p_node.push_back(result);
	    }
	    else if(!check_type(result = unary_expression())) {
		    p_node.push_back(result);
            require(!check_type(result = assignment_operator()));
			p_node.push_back(result);
            require(!check_type(result = assignment_expression()));
			p_node.push_back(result);
	    }
        else
            return undo(__func__);

		commit();
		return p_node;
    }

    Node Parser::assignment_operator() {
	    prepare();
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    switch(tokens[offset.top()].get_type()) {
		    case Type::MUL_ASSIGN:
		    case Type::DIV_ASSIGN:
		    case Type::MOD_ASSIGN:
		    case Type::ADD_ASSIGN:
		    case Type::SUB_ASSIGN:
			    p_node.push_back(Node(tokens[offset.top()], __func__, tokens[offset.top()].get_chars()));
                ++offset.top();
			    break;
		    default: {
			    if(check_char(offset.top(), "="))
				    p_node.push_back(Node(tokens[offset.top()++], __func__, " = "));
                else
                    return undo(__func__);
			    break;
		    }
	    }

		commit();
		return p_node;
    }

    Node Parser::expression() {						// Recursive
	    prepare();
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = assignment_expression())) {
		    p_node.push_back(result);
		    if(check_char(offset.top(), ",")) {
			    p_node.push_back(Node(tokens[offset.top()++], __func__, ", "));
                require(!check_type(result = expression()));
				p_node.push_back(result);
		    }
	    }
        else
            return undo(__func__);

		commit();
		return p_node;
    }

    Node Parser::declaration() {
	    prepare();
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = declaration_specifiers())) {
		    p_node.push_back(result);
		    if(check_char(offset.top(), ";")) {
			    p_node.push_back(Node(tokens[offset.top()++], __func__, "", 0, true));
		    }
            else {
                require(!check_type(result = init_declarator_list()));
                p_node.push_back(result);
                require(check_char(offset.top(), ";"));
                p_node.push_back(Node(tokens[offset.top()++], __func__, "", 0, true));
            }
	    }
        else
            return undo(__func__);

		commit();
		return p_node;
    }

    Node Parser::declaration_specifiers() {						// Recursive
	    prepare();
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(check_type(offset.top(), Type::TYPEDEF)) {
		    p_node.push_back(Node(tokens[offset.top()++], __func__));
		    if(!check_type(result = declaration_specifiers()))
			    p_node.push_back(result);
	    }
	    else if(!check_type(result = type_specifier())) {
		    p_node.push_back(result);
		    if(!check_type(result = declaration_specifiers()))
			    p_node.push_back(result);
	    }
        else
            return undo(__func__);

		commit();
		return p_node;
    }

    Node Parser::init_declarator_list() {						// Recursive
	    prepare();
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = init_declarator())) {
		    p_node.push_back(result);
		    if(check_char(offset.top(), ",")) {
			    p_node.push_back(Node(tokens[offset.top()++], __func__, ", "));
                require(!check_type(result = init_declarator_list()));
				p_node.push_back(result);
		    }
	    }
        else
            return undo(__func__);

		commit();
		return p_node;
    }

    Node Parser::init_declarator() {
	    prepare();
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = direct_declarator())) {
		    p_node.push_back(result);
		    if(check_char(offset.top(), "=")) {
			    p_node.push_back(Node(tokens[offset.top()++], __func__, " = "));
                require(!check_type(result = initializer()));
				p_node.push_back(result);
		    }
	    }
        else
            return undo(__func__);

		commit();
		return p_node;
    }

    Node Parser::type_specifier() {
	    prepare();
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    switch(tokens[offset.top()].get_type()) {
		    case Type::VOID:
		    case Type::CHAR:
		    case Type::SHORT:
		    case Type::INT:
		    case Type::LONG:
		    case Type::FLOAT:
		    case Type::DOUBLE:
		    case Type::SIGNED:
		    case Type::UNSIGNED:
			    p_node.push_back(Node(tokens[offset.top()++], __func__));
			    break;
		    default: {
			    if(!check_type(result = struct_or_union_specifier()))
				    p_node.push_back(result);
			    else if(!check_type(result = enum_specifier()))
				    p_node.push_back(result);
                else
                    return undo(__func__);
			    break;
		    }
	    }

		commit();
		return p_node;
    }

    Node Parser::struct_or_union_specifier() {
	    prepare();
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(check_type(offset.top(), Type::STRUCT)) {
		    p_node.push_back(Node(tokens[offset.top()], __func__, "class "));
		    if(check_type(++offset.top(), Type::IDENTIFIER)) {
			    p_node.push_back(Node(tokens[offset.top()], __func__, tokens[offset.top()].get_chars()));
                ++offset.top();
			    if(check_char(offset.top(), "{")) {
				    p_node.push_back(Node(tokens[offset.top()++], __func__, ": ", 1, true));
                    require(!check_type(result = struct_declaration_list()));
					p_node.push_back(result);
                    require(check_char(offset.top(), "}"));
					p_node.push_back(Node(tokens[offset.top()++], __func__, "", -1, true));
			    }
		    }
            else {
                require(check_char(offset.top(), "{"));
                p_node.push_back(Node(tokens[offset.top()++], __func__, "struct: ", 1, true));
                require(!check_type(result = struct_declaration_list()));
                p_node.push_back(result);
                require(check_char(offset.top(), "}"));
                p_node.push_back(Node(tokens[offset.top()++], __func__, "", -1, true));
            }
	    }
        else
            return undo(__func__);

        commit();
		return p_node;
    }

    Node Parser::struct_declaration_list() {							// Recursive
	    prepare();
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = specifier_qualifier_list())) {
		    p_node.push_back(result);
            require(!check_type(result = struct_declarator_list()));
			p_node.push_back(result);
            require(check_char(offset.top(), ";"));
			p_node.push_back(Node(tokens[offset.top()++], __func__));
			if(!check_type(result = struct_declaration_list()))
				p_node.push_back(result);
	    }
        else
            return undo(__func__);

		commit();
		return p_node;
    }

    Node Parser::specifier_qualifier_list() {						// Recursive
	    prepare();
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = type_specifier())) {
		    p_node.push_back(result);
		    if(!check_type(result = specifier_qualifier_list()))
			    p_node.push_back(result);
	    }
        else
            return undo(__func__);

		commit();
		return p_node;
    }

    Node Parser::struct_declarator_list() {							// Recursive
	    prepare();
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = struct_declarator())) {
		    p_node.push_back(result);
		    if(check_char(offset.top(), ",")) {
			    p_node.push_back(Node(tokens[offset.top()++], __func__));
                require(!check_type(result = struct_declarator_list()));
				p_node.push_back(result);
		    }
	    }
        else
            return undo(__func__);
	
		commit();
		return p_node;
    }

    Node Parser::struct_declarator() {
	    prepare();
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = direct_declarator())) {
		    p_node.push_back(result);
		    if(check_char(offset.top(), ":")) {
			    p_node.push_back(Node(tokens[offset.top()++], __func__, ": "));
                require(!check_type(result = conditional_expression()));
				p_node.push_back(result);
		    }
	    }
	    else if(check_char(offset.top(), ":")) {
		    p_node.push_back(Node(tokens[offset.top()++], __func__, ": "));
            require(!check_type(result = conditional_expression()));
			p_node.push_back(result);
	    }
        else
            return undo(__func__);

		commit();
		return p_node;
    }

    Node Parser::enum_specifier() {
	    prepare();
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(check_type(offset.top(), Type::ENUM)) {
		    p_node.push_back(Node(tokens[offset.top()], __func__, "class "));
		    if(check_type(++offset.top(), Type::IDENTIFIER)) {
			    p_node.push_back(Node(tokens[offset.top()], __func__, tokens[offset.top()].get_chars()));
                ++offset.top();
			    if(check_char(offset.top(), "{")) {
				    p_node.push_back(Node(tokens[offset.top()++], __func__, "(Enum): ", 1, true));
                    require(!check_type(result = enumerator_list()));
					p_node.push_back(result);
                    require(check_char(offset.top(), "}"));
					p_node.push_back(Node(tokens[offset.top()++], __func__, "", -1, true));
			    }
		    }
            else {
                require(check_char(offset.top(), "{"));
                p_node.push_back(Node(tokens[offset.top()++], __func__, "enum(Enum): ", 1, true));
                require(!check_type(result = enumerator_list()));
                p_node.push_back(result);
                require(check_char(offset.top(), "}"));
                p_node.push_back(Node(tokens[offset.top()++], __func__, "", -1, true));
            }
	    }
        else
            return undo(__func__);

        commit();
		return p_node;
    }

    Node Parser::enumerator_list() {							// Recursive
	    prepare();
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = enumerator())) {
		    p_node.push_back(result);
		    if(check_char(offset.top(), ",")) {
			    p_node.push_back(Node(tokens[offset.top()++], __func__, ", "));
                require(!check_type(result = enumerator_list()));
				p_node.push_back(result);
		    }
	    }
        else
            return undo(__func__);

		commit();
		return p_node;
    }

    Node Parser::enumerator() {
	    prepare();
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(check_type(offset.top(), Type::IDENTIFIER)) {
		    p_node.push_back(Node(tokens[offset.top()], __func__, tokens[offset.top()].get_chars()));
		    if(check_char(++offset.top(), "=")) {
			    p_node.push_back(Node(tokens[offset.top()++], __func__, " = "));
                require(!check_type(result = conditional_expression()));
				p_node.push_back(result);
		    }
	    }
        else
            return undo(__func__);

        commit();
		return p_node;
    }

    Node Parser::direct_declarator() {							// Recursive
	    prepare();
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

		if(check_type(offset.top(), Type::IDENTIFIER)) {
			p_node.push_back(Node(tokens[offset.top()], __func__, tokens[offset.top()].get_chars()));
            ++offset.top();
			if(!check_type(result = direct_declarator_rec()))
				p_node.push_back(result);
		}
		else if(check_char(offset.top(), "(")) {
			p_node.push_back(Node(tokens[offset.top()++], __func__, "("));
            require(!check_type(result = direct_declarator()));
			p_node.push_back(result);
            require(check_char(offset.top(), ")"));
			p_node.push_back(Node(tokens[offset.top()++], __func__, "): ", 1, true));
			if(!check_type(result = direct_declarator_rec()))
				p_node.push_back(result);
		}
        else
            return undo(__func__);

		commit();
		return p_node;
    }

    Node Parser::direct_declarator_rec() {
        prepare();
        Node p_node(Token(Type::parsing), __func__);
        Node result;

        func_log(__func__);

        if(check_char(offset.top(), "[")) {
            p_node.push_back(Node(tokens[offset.top()], __func__, "["));
            if(check_char(++offset.top(), "]")) {
                p_node.push_back(Node(tokens[offset.top()++], __func__, "]"));
                if(!check_type(result = direct_declarator_rec()))
                    p_node.push_back(result);
            }
            else {
                require(!check_type(result = conditional_expression()));
                p_node.push_back(result);
                require(check_char(offset.top(), "]"));
                p_node.push_back(Node(tokens[offset.top()++], __func__, "]"));
                if(!check_type(result = direct_declarator_rec()))
                    p_node.push_back(result);
            }
        }
        else if(check_char(offset.top(), "(")) {
            p_node.push_back(Node(tokens[offset.top()], __func__, "("));
            if(check_char(++offset.top(), ")")) {
                p_node.push_back(Node(tokens[offset.top()++], __func__, "): ", 1, true));
                if(!check_type(result = direct_declarator_rec()))
                    p_node.push_back(result);
            }
            else if(!check_type(result = parameter_list())) {
                p_node.push_back(result);
                require(check_char(offset.top(), ")"));
                p_node.push_back(Node(tokens[offset.top()++], __func__, "): ", 1, true));
                if(!check_type(result = direct_declarator_rec()))
                    p_node.push_back(result);
            }
            else {
                require(!check_type(result = identifier_list()));
                p_node.push_back(result);
                require(check_char(offset.top(), ")"));
                p_node.push_back(Node(tokens[offset.top()++], __func__, "): ", 1, true));
                if(!check_type(result = direct_declarator_rec()))
                    p_node.push_back(result);
            }
        }
        else
            return undo(__func__);

        commit();
        return p_node;
    }

    Node Parser::parameter_list() {					// Recursive
	    prepare();
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = parameter_declaration())) {
		    p_node.push_back(result);
		    if(check_char(offset.top(), ",")) {
			    p_node.push_back(Node(tokens[offset.top()++], __func__));
                require(!check_type(result = parameter_list()));
				p_node.push_back(result);
		    }
	    }
        else
            return undo(__func__);

		commit();
		return p_node;
    }

    Node Parser::parameter_declaration() {
	    prepare();
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = declaration_specifiers())) {
		    p_node.push_back(result);
		    if(!check_type(result = direct_declarator()))
			    p_node.push_back(result);
		    else if(!check_type(result = direct_abstract_declarator(false)))
			    p_node.push_back(result);
	    }
        else
            return undo(__func__);

		commit();
		return p_node;
    }

    Node Parser::identifier_list() {						// Recursive
	    prepare();
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(check_type(offset.top(), Type::IDENTIFIER)) {
		    p_node.push_back(Node(tokens[offset.top()], __func__));
		    if(check_char(++offset.top(), ",")) {
			    p_node.push_back(Node(tokens[offset.top()++], __func__));
                require(!check_type(result = identifier_list()));
				p_node.push_back(result);
		    }
	    }
        else
            return undo(__func__);

		commit();
		return p_node;
    }

    Node Parser::type_name() {
	    prepare();
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = specifier_qualifier_list())) {
		    p_node.push_back(result);
		    if(!check_type(result = direct_abstract_declarator(false)))
			    p_node.push_back(result);
	    }
        else
            return undo(__func__);

		commit();
		return p_node;
    }

    Node Parser::direct_abstract_declarator(bool recursive) {								// Recursive
	    prepare();
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(check_char(offset.top(), "(")) {
		    p_node.push_back(Node(tokens[offset.top()++], __func__));
		    if(!recursive) {
                require(!check_type(result = direct_abstract_declarator(false)));
				p_node.push_back(result);
                require(check_char(offset.top(), ")"));
				p_node.push_back(Node(tokens[offset.top()++], __func__));
				if(!check_type(result = direct_abstract_declarator(true)))
					p_node.push_back(result);
		    }
		    else {
			    if(check_char(offset.top(), ")")) {
				    p_node.push_back(Node(tokens[offset.top()++], __func__));
				    if(!check_type(result = direct_abstract_declarator(true)))
					    p_node.push_back(result);
			    }
                else {
                    require(!check_type(result = parameter_list()));
                    p_node.push_back(result);
                    require(check_char(offset.top(), ")"));
                    p_node.push_back(Node(tokens[offset.top()++], __func__));
                    if(!check_type(result = direct_abstract_declarator(true)))
                        p_node.push_back(result);
                }
		    }
	    }
	    else if(check_char(offset.top(), "[")) {
		    p_node.push_back(Node(tokens[offset.top()], __func__));
		    if(check_char(++offset.top(), "]")) {
			    p_node.push_back(Node(tokens[offset.top()++], __func__));
			    if(!check_type(result = direct_abstract_declarator(true)))
				    p_node.push_back(result);
		    }
            else {
                require(!check_type(result = conditional_expression()));
                p_node.push_back(result);
                require(check_char(offset.top(), "]"));
                p_node.push_back(Node(tokens[offset.top()++], __func__));
                if(!check_type(result = direct_abstract_declarator(true)))
                    p_node.push_back(result);
            }
	    }
        else
            return undo(__func__);

		commit();
		return p_node;
    }

    Node Parser::initializer() {
	    prepare();
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = assignment_expression())) {
		    p_node.push_back(result);
	    }
	    else if(check_char(offset.top(), "{")) {
		    p_node.push_back(Node(tokens[offset.top()++], __func__));
            require(!check_type(result = initializer_list()));
			p_node.push_back(result);
            require(check_char(offset.top(), "}"));
			p_node.push_back(Node(tokens[offset.top()++], __func__));
	    }
        else
            return undo(__func__);

		commit();
		return p_node;
    }

    Node Parser::initializer_list() {			// Recursive
	    prepare();
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = initializer())) {
		    p_node.push_back(result);
		    if(check_char(offset.top(), ",")) {
			    p_node.push_back(Node(tokens[offset.top()++], __func__));
                require(!check_type(result = initializer_list()));
				p_node.push_back(result);
		    }
	    }
        else
            return undo(__func__);

		commit();
		return p_node;
    }

    Node Parser::statement() {
	    prepare();
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

        if(!check_type(result = labeled_statement()) || !check_type(result = compound_statement()) ||
                !check_type(result = expression_statement()) || !check_type(result = selection_statement()) ||
                !check_type(result = iteration_statement()) || !check_type(result = jump_statement())) {
            p_node.push_back(result);
            commit();
            return p_node;
        }
        else
            return undo(__func__);
    }

    Node Parser::labeled_statement() {
	    prepare();
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(check_type(offset.top(), Type::CASE)) {
		    p_node.push_back(Node(tokens[offset.top()++], __func__));
            require(!check_type(result = conditional_expression()));
			p_node.push_back(result);
            require(check_char(offset.top(), ":"));
			p_node.push_back(Node(tokens[offset.top()++], __func__));
            require(!check_type(result = statement()));
			p_node.push_back(result);
	    }
	    else if(check_type(offset.top(), Type::DEFAULT)) {
		    p_node.push_back(Node(tokens[offset.top()], __func__));
            require(check_char(++offset.top(), ":"));
			p_node.push_back(Node(tokens[offset.top()++], __func__));
            require(!check_type(result = statement()));
			p_node.push_back(result);
	    }
        else
            return undo(__func__);

		commit();
		return p_node;
    }

    Node Parser::compound_statement() {
	    prepare();
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(check_char(offset.top(), "{")) {
		    p_node.push_back(Node(tokens[offset.top()], __func__));
		    if(check_char(++offset.top(), "}")) {
			    p_node.push_back(Node(tokens[offset.top()++], __func__));
		    }
		    else if(!check_type(result = statement_list())) {
			    p_node.push_back(result);
                require(check_char(offset.top(), "}"));
				p_node.push_back(Node(tokens[offset.top()++], __func__));
		    }
            else {
                require(!check_type(result = declaration_list()));
				p_node.push_back(result);
				if(check_char(offset.top(), "}")) {
					p_node.push_back(Node(tokens[offset.top()++], __func__));
				}
                else {
                    require(!check_type(result = statement_list()));
                    p_node.push_back(result);
                    require(check_char(offset.top(), "}"));
                    p_node.push_back(Node(tokens[offset.top()++], __func__));
                }
		    }
	    }
        else
            return undo(__func__);

		commit();
		return p_node;
    }

    Node Parser::declaration_list() {			// Recursive
	    prepare();
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = declaration())) {
		    p_node.push_back(result);
		    if(!check_type(result = declaration_list()))
			    p_node.push_back(result);
	    }
        else
            return undo(__func__);

		commit();
		return p_node;
    }

    Node Parser::statement_list() {				// Recursive
	    prepare();
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = statement())) {
		    p_node.push_back(result);
		    if(!check_type(result = statement_list()))
			    p_node.push_back(result);
	    }
        else
            return undo(__func__);

		commit();
		return p_node;
    }

    Node Parser::expression_statement() {
	    prepare();
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(check_char(offset.top(), ";")) {
		    p_node.push_back(Node(tokens[offset.top()++], __func__, "", 0, true));
	    }
	    else if(!check_type(result = expression())) {
		    p_node.push_back(result);
            require(check_char(offset.top(), ";"));
			p_node.push_back(Node(tokens[offset.top()++], __func__, "", 0, true));
	    }
        else
            return undo(__func__);

		commit();
		return p_node;
    }

    Node Parser::selection_statement() {
	    prepare();
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(check_type(offset.top(), Type::IF)) {
            if(tokens[offset.top() - 1].get_type() == Type::ELSE)
                p_node.push_back(Node(tokens[offset.top()], __func__));
            else
		        p_node.push_back(Node(tokens[offset.top()], __func__, "if "));
            require(check_char(++offset.top(), "("));
			p_node.push_back(Node(tokens[offset.top()++], __func__));
            require(!check_type(result = expression()));
			p_node.push_back(result);
            require(check_char(offset.top(), ")"));
			p_node.push_back(Node(tokens[offset.top()++], __func__, ": ", 1, true));
            require(!check_type(result = statement()));
			p_node.push_back(result);
			if(check_type(offset.top(), Type::ELSE)) {
                if(tokens[offset.top() + 1].get_type() == Type::IF)
                    p_node.push_back(Node(tokens[offset.top()++], __func__, "elif "));
                else
					p_node.push_back(Node(tokens[offset.top()++], __func__, "else: ", 1, true));
                require(!check_type(result = statement()));
				p_node.push_back(result);
			}
	    }
	    else if(check_type(offset.top(), Type::SWITCH)) {
		    p_node.push_back(Node(tokens[offset.top()], __func__));
            require(check_char(++offset.top(), "("));
			p_node.push_back(Node(tokens[offset.top()++], __func__));
            require(!check_type(result = expression()));
			p_node.push_back(result);
            require(check_char(offset.top(), ")"));
			p_node.push_back(Node(tokens[offset.top()++], __func__));
            require(!check_type(result = statement()));
			p_node.push_back(result);
	    }
        else
            return undo(__func__);

		commit();
		return p_node;
    }

    Node Parser::iteration_statement() {
	    prepare();
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(check_type(offset.top(), Type::WHILE)) {
		    p_node.push_back(Node(tokens[offset.top()], __func__, "while "));
            require(check_char(++offset.top(), "("));
			p_node.push_back(Node(tokens[offset.top()++], __func__));
            require(!check_type(result = expression()));
			p_node.push_back(result);
            require(check_char(offset.top(), ")"));
			p_node.push_back(Node(tokens[offset.top()++], __func__, ": ", 1, true));
            require(!check_type(result = statement()));
			p_node.push_back(result);
	    }
	    else if(check_type(offset.top(), Type::DO)) {
		    p_node.push_back(Node(tokens[offset.top()++], __func__));
            require(!check_type(result = statement()));
			p_node.push_back(result);
            require(check_type(offset.top(), Type::WHILE));
			p_node.push_back(Node(tokens[offset.top()], __func__));
            require(check_char(++offset.top(), "("));
			p_node.push_back(Node(tokens[offset.top()++], __func__));
            require(!check_type(result = expression()));
			p_node.push_back(result);
            require(check_char(offset.top(), ")"));
			p_node.push_back(Node(tokens[offset.top()], __func__));
            require(check_char(++offset.top(), ";"));
			p_node.push_back(Node(tokens[offset.top()++], __func__));
	    }
	    else if(check_type(offset.top(), Type::FOR)) {
		    p_node.push_back(Node(tokens[offset.top()], __func__, "for "));
            require(check_char(++offset.top(), "("));
			p_node.push_back(Node(tokens[offset.top()++], __func__));
            require(!check_type(result = expression_statement()));
			p_node.push_back(result);
            require(!check_type(result = expression_statement()));
			p_node.push_back(result);
			if(check_char(offset.top(), ")")) {
				p_node.push_back(Node(tokens[offset.top()++], __func__, ": ", 1, true));
                require(!check_type(result = statement()));
				p_node.push_back(result);
			}
            else {
                require(!check_type(result = expression()));
                p_node.push_back(result);
                require(check_char(offset.top(), ")"));
                p_node.push_back(Node(tokens[offset.top()++], __func__, ": ", 1, true));
                require(!check_type(result = statement()));
                p_node.push_back(result);
            }
	    }
        else
            return undo(__func__);

		commit();
		return p_node;
    }

    Node Parser::jump_statement() {
	    prepare();
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(check_type(offset.top(), Type::BREAK)) {
		    p_node.push_back(Node(tokens[offset.top()], __func__, "break"));
            require(check_char(++offset.top(), ";"));
			p_node.push_back(Node(tokens[offset.top()], __func__, "", -1, true));
	    }
	    else if(check_type(offset.top(), Type::RETURN)) {
		    p_node.push_back(Node(tokens[offset.top()], __func__, "return "));
		    if(check_char(++offset.top(), ";")) {
			    p_node.push_back(Node(tokens[offset.top()++], __func__, "", -1, true));
		    }
            else {
                require(!check_type(result = expression()));
                p_node.push_back(result);
                require(check_char(offset.top(), ";"));
                p_node.push_back(Node(tokens[offset.top()++], __func__, "", -1, true));
            }
	    }
        else
            return undo(__func__);

		commit();
		return p_node;
    }

    Node Parser::translation_unit() {			// Recursive
        prepare();
	    Node p_node(Token(Type::parsing), __func__, "# After translation\n# Remember, you should check the code!\n\n");
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = external_declaration())) {
		    p_node.push_back(result);
		    if(!check_type(result = translation_unit()))
			    p_node.push_back(result);
	    }
        else
            return undo(__func__);

        commit();
		return p_node;
    }

    Node Parser::external_declaration() {
	    prepare();
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = function_definition()) || !check_type(result = declaration())) {
		    p_node.push_back(result);
            commit();
            return p_node;
	    }
        else
            return undo(__func__);
    }

    Node Parser::function_definition() {
	    prepare();
	    Node p_node(Token(Type::parsing), __func__, "def ");
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = declaration_specifiers())) {
		    p_node.push_back(result);
            require(!check_type(result = direct_declarator()));
			p_node.push_back(result);
			if(!check_type(result = declaration_list())) {
				p_node.push_back(result);
                require(!check_type(result = compound_statement()));
				p_node.push_back(result);
			}
            else if(!check_type(result = compound_statement())) {
                p_node.push_back(result);
            }
            else
                return undo(__func__);
	    }
	    else if(!check_type(result = direct_declarator())) {
		    p_node.push_back(result);
		    if(!check_type(result = declaration_list())) {
			    p_node.push_back(result);
                require(!check_type(result = compound_statement()));
				p_node.push_back(result);
		    }
            else {
                require(!check_type(result = compound_statement()));
                p_node.push_back(result);
            }
	    }
        else
            return undo(__func__);

		commit();
		return p_node;
    }

};
