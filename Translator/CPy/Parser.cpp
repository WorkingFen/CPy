#include "Parser.h"

namespace Translator {
    void Parser::func_log(const char* func) {
    #ifdef _DEBUG
	    std::cout << std::endl << offset.top() << ": " << tokens[offset.top()].get_chars() << " -> " << func;
    #endif
    }

    void Parser::commit(int inc) {
        int token_number = offset.top();
        offset.pop();
        offset.top() = token_number + inc;
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
	    bool found = false;
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    switch(tokens[offset.top()].get_type()) {
		    case Type::IDENTIFIER:
		    case Type::CONSTANT:
		    case Type::STRING_LITERAL:
			    p_node.push_back(Node(tokens[offset.top()], __func__));
			    found = true;
			    break;
		    default: {
			    if(check_char(offset.top(), "(")) {
				    p_node.push_back(Node(tokens[offset.top()], __func__));
                    ++offset.top();
				    if(!check_type(result = expression())) {
					    p_node.push_back(result);
					    if(check_char(offset.top(), ")")) {
						    p_node.push_back(Node(tokens[offset.top()], __func__));
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
            commit(1);
		    return p_node;
	    }
	    else {
		    return Node(Token(), __func__);
	    }
    }

    Node Parser::postfix_expression(bool recursive) {			// Recursive
        prepare();
	    bool found = false;
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

        auto condition_inc = [&]() {
            ++offset.top();
            return !check_type(result = argument_expression_list());
        };

	    if(!recursive) {
		    if(!check_type(result = primary_expression())) {
			    p_node.push_back(result);
			    if(!check_type(result = postfix_expression(true))) {
				    p_node.push_back(result);
				    found = true;
			    }
			    found = true;
		    }
	    }
	    else {
		    if(check_char(offset.top(),"(")) {
			    p_node.push_back(Node(tokens[offset.top()], __func__));
			    if(check_char(offset.top() + 1, ")")) {
				    p_node.push_back(Node(tokens[offset.top() + 1], __func__));
                    offset.top() += 2;
				    if(!check_type(result = postfix_expression(true))) {
					    p_node.push_back(result);
					    found = true;
				    }
				    found = true;
			    }
			    else if(condition_inc()) {
				    p_node.push_back(result);
				    if(check_char(offset.top(), ")")) {
					    p_node.push_back(Node(tokens[offset.top()], __func__));
                        ++offset.top();
					    if(!check_type(result = postfix_expression(true))) {
						    p_node.push_back(result);
						    found = true;
					    }
					    found = true;
				    }
				    else {}																	// It will be an error
			    }
			    else {}																		// It will be an error
		    }
		    else if(check_char(offset.top(), ".")) {
			    p_node.push_back(Node(tokens[offset.top()], __func__));
			    if(check_type(offset.top() + 1, Type::IDENTIFIER)) {
				    p_node.push_back(Node(tokens[offset.top() + 1], __func__));
                    offset.top() += 2;
				    if(!check_type(result = postfix_expression(true))) {
					    p_node.push_back(result);
					    found = true;
				    }
				    found = true;
			    }
			    else {}																		// It will be an error
		    }
		    else if(check_type(offset.top(), Type::INC_OP)) {
			    p_node.push_back(Node(tokens[offset.top()], __func__));
                ++offset.top();
			    if(!check_type(result = postfix_expression(true))) {
				    p_node.push_back(result);
				    found = true;
			    }
			    found = true;
		    }
		    else if(check_type(offset.top(), Type::DEC_OP)) {
			    p_node.push_back(Node(tokens[offset.top()], __func__));
                ++offset.top();
			    if(!check_type(result = postfix_expression(true))) {
				    p_node.push_back(result);
				    found = true;
			    }
			    found = true;
		    }
		    else if(check_char(offset.top(), "[")) {
			    p_node.push_back(Node(tokens[offset.top()], __func__));
                ++offset.top();
			    if(!check_type(result = expression())) {
				    p_node.push_back(result);
				    if(check_char(offset.top(), "]")) {
					    p_node.push_back(Node(tokens[offset.top()], __func__));
                        ++offset.top();
					    if(!check_type(result = postfix_expression(true))) {
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
            commit(0);
		    return p_node;
	    }
	    else {

		    return Node(Token(), __func__);
	    }
    }

    Node Parser::argument_expression_list() {	// Recursive
        prepare();
	    bool found = false;
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = assignment_expression())) {
		    p_node.push_back(result);
		    if(check_char(offset.top(), ",")) {
			    p_node.push_back(Node(tokens[offset.top()], __func__));
                ++offset.top();
			    if(!check_type(result = argument_expression_list())) {
				    p_node.push_back(result);
				    found = true;
			    }
			    else {}																	// It will be an error
		    }
		    found = true; 
	    }

	    if(found) {
            commit(0);
		    return p_node;
	    }
	    else {

		    return Node(Token(), __func__);
	    }
    }

    Node Parser::unary_expression() {				// Recursive
        prepare();
	    bool found = false;
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = postfix_expression(false))) {
		    p_node.push_back(result);
		    found = true;
	    }
	    else if(check_type(offset.top(), Type::INC_OP) || check_type(offset.top(), Type::DEC_OP)) {
		    p_node.push_back(Node(tokens[offset.top()], __func__));
            ++offset.top();
		    if(!check_type(result = unary_expression())) {
			    p_node.push_back(result);
			    found = true;
		    }
		    else {}																// It will be an error
	    }
	    else if(!check_type(result = unary_operator())) {
		    p_node.push_back(result);
		    if(!check_type(result = cast_expression())) {
			    p_node.push_back(result);
			    found = true;
		    }
		    else {}																// It will be an error
	    }

	    if(found) {
            commit(0);
		    return p_node;
	    }
	    else {

		    return Node(Token(), __func__);
	    }
    }

    Node Parser::unary_operator() {
        prepare();
	    bool found = false;
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

        func_log(__func__);

	    std::string tokens_chars = tokens[offset.top()].get_chars();

	    if(tokens_chars == "*" || tokens_chars == "+" || tokens_chars == "-" || tokens_chars == "!") {
		    p_node.push_back(Node(tokens[offset.top()], __func__));
		    found = true;
	    }

	    if(found) {
            commit(1);
		    return p_node;
	    }
	    else {

		    return Node(Token(), __func__);
	    }
    }

    Node Parser::cast_expression() {					// Recursive
        prepare();
	    bool found = false;
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = unary_expression())) {
		    p_node.push_back(result);
		    found = true;
	    }
	    else if(check_char(offset.top(), "(")) {
		    p_node.push_back(Node(tokens[offset.top()], __func__));
            ++offset.top();
		    if(!check_type(result = type_name())) {
			    p_node.push_back(result);
			    if(check_char(offset.top(), ")")) {
				    p_node.push_back(Node(tokens[offset.top()], __func__));
                    ++offset.top();
				    if(!check_type(result = cast_expression())) {
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
            commit(0);
		    return p_node;
	    }
	    else {

		    return Node(Token(), __func__);
	    }
    }

    Node Parser::multiplicative_expression() {		// Recursive
        prepare();
	    bool found = false;
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = cast_expression())) {
		    p_node.push_back(result);
            std::string tokens_chars = tokens[offset.top()].get_chars();
		    if(tokens_chars == "*" || tokens_chars == "/" || tokens_chars == "%") {
			    p_node.push_back(Node(tokens[offset.top()], __func__));
                ++offset.top();
			    if(!check_type(result = multiplicative_expression())) {
				    p_node.push_back(result);
				    found = true;
			    }
			    else {}																		// It will be an error
		    }
		    found = true; 
	    }

	    if(found) {
            commit(0);
		    return p_node;
	    }
	    else {

		    return Node(Token(), __func__);
	    }
    }

    Node Parser::additive_expression() {				// Recursive
        prepare();
	    bool found = false;
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = multiplicative_expression())) {
		    p_node.push_back(result);
		    if(check_char(offset.top(), "+") || check_char(offset.top(), "-")) {
			    p_node.push_back(Node(tokens[offset.top()], __func__));
                ++offset.top();
			    if(!check_type(result = additive_expression())) {
				    p_node.push_back(result);
				    found = true;
			    }
			    else {}																	// It will be an error
		    }
		    found = true; 
	    }

	    if(found) {
            commit(0);
		    return p_node;
	    }
	    else {

		    return Node(Token(), __func__);
	    }
    }

    Node Parser::relational_expression() {			// Recursive
        prepare();
	    bool found = false;
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = additive_expression())) {
		    p_node.push_back(result);
		    if(check_char(offset.top(), "<") || check_char(offset.top(), ">") ||
			        check_type(offset.top(), Type::LE_OP) || check_type(offset.top(), Type::GE_OP)) {
			    p_node.push_back(Node(tokens[offset.top()], __func__));
                ++offset.top();
			    if(!check_type(result = relational_expression())) {
				    p_node.push_back(result);
				    found = true;
			    }
			    else {}																// It will be an error
		    }
		    found = true; 
	    }

	    if(found) {
            commit(0);
		    return p_node;
	    }
	    else {

		    return Node(Token(), __func__);
	    }
    }

    Node Parser::equality_expression() {				// Recursive
        prepare();
	    bool found = false;
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = relational_expression())) {
		    p_node.push_back(result);
		    if(check_type(offset.top(), Type::EQ_OP) || check_type(offset.top(), Type::NE_OP)) {
			    p_node.push_back(Node(tokens[offset.top()], __func__));
                ++offset.top();
			    if(!check_type(result = equality_expression())) {
				    p_node.push_back(result);
				    found = true;
			    }
			    else {}																// It will be an error
		    }
		    found = true; 
	    }

	    if(found) {
            commit(0);
		    return p_node;
	    }
	    else {

		    return Node(Token(), __func__);
	    }
    }

    Node Parser::logical_and_expression() {			// Recursive
        prepare();
	    bool found = false;
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = equality_expression())) {
		    p_node.push_back(result);
		    if(check_type(offset.top(), Type::AND_OP)) {
			    p_node.push_back(Node(tokens[offset.top()], __func__));
                ++offset.top();
			    if(!check_type(result = logical_and_expression())) {
				    p_node.push_back(result);
				    found = true;
			    }
			    else {}																			// It will be an error
		    }
		    found = true; 
	    }

	    if(found) {
            commit(0);
		    return p_node;
	    }
	    else {

		    return Node(Token(), __func__);
	    }
    }

    Node Parser::logical_or_expression() {			// Recursive
        prepare();
	    bool found = false;
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = logical_and_expression())) {
		    p_node.push_back(result);
		    if(check_type(offset.top(), Type::OR_OP)) {
			    p_node.push_back(Node(tokens[offset.top()], __func__));
                ++offset.top();
			    if(!check_type(result = logical_or_expression())) {
				    p_node.push_back(result);
				    found = true;
			    }
			    else {}															// It will be an error
		    }
		    found = true; 
	    }

	    if(found) {
		    commit(0);
		    return p_node;
	    }
	    else {

		    return Node(Token(), __func__);
	    }
    }

    Node Parser::conditional_expression() {			// Recursive
	    prepare();
	    bool found = false;
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = logical_or_expression())) {
		    p_node.push_back(result);
		    if(check_char(offset.top(), "?")) {
			    p_node.push_back(Node(tokens[offset.top()], __func__));
                ++offset.top();
			    if(!check_type(result = expression())) {
				    p_node.push_back(result);
				    if(check_char(offset.top(), ":")) {
					    p_node.push_back(Node(tokens[offset.top()], __func__));
                        ++offset.top();
					    if(!check_type(result = conditional_expression())) {
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
		    commit(0);
		    return p_node;
	    }
	    else {

		    return Node(Token(), __func__);
	    }
    }

    Node Parser::assignment_expression() {			// Recursive
	    prepare();
	    bool found = false;
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = conditional_expression())) {
		    p_node.push_back(result);
		    found = true;
	    }
	    else if(!check_type(result = unary_expression())) {
		    p_node.push_back(result);
		    if(!check_type(result = assignment_operator())) {
			    p_node.push_back(result);
			    if(!check_type(result = assignment_expression())) {
				    p_node.push_back(result);
				    found = true;
			    }
			    else {}																	// It will be an error
		    }
		    else {}																		// It will be an error
	    }

	    if(found) {
		    commit(0);
		    return p_node;
	    }
	    else {

		    return Node(Token(), __func__);
	    }
    }

    Node Parser::assignment_operator() {
	    prepare();
	    bool found = false;
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    switch(tokens[offset.top()].get_type()) {
		    case Type::MUL_ASSIGN:
		    case Type::DIV_ASSIGN:
		    case Type::MOD_ASSIGN:
		    case Type::ADD_ASSIGN:
		    case Type::SUB_ASSIGN:
			    p_node.push_back(Node(tokens[offset.top()], __func__));
			    found = true;
			    break;
		    default: {
			    if(check_char(offset.top(), "=")) {
				    p_node.push_back(Node(tokens[offset.top()], __func__));
				    found = true;
			    }
			    break;
		    }
	    }

	    if(found) {
		    commit(1);
		    return p_node;
	    }
	    else {

		    return Node(Token(), __func__);
	    }
    }

    Node Parser::expression() {						// Recursive
	    prepare();
	    bool found = false;
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = assignment_expression())) {
		    p_node.push_back(result);
		    if(check_char(offset.top(), ",")) {
			    p_node.push_back(Node(tokens[offset.top()], __func__));
                ++offset.top();
			    if(!check_type(result = expression())) {
				    p_node.push_back(result);
				    found = true;
			    }
			    else {}																// It will be an error
		    }
		    found = true; 
	    }

	    if(found) {
		    commit(0);
		    return p_node;
	    }
	    else {

		    return Node(Token(), __func__);
	    }
    }

    Node Parser::declaration() {
	    prepare();
	    bool found = false;
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = declaration_specifiers())) {
		    p_node.push_back(result);
		    if(check_char(offset.top(), ";")) {
			    p_node.push_back(Node(tokens[offset.top()], __func__));
			    found = true;
		    }
		    else if(!check_type(result = init_declarator_list())) {
			    p_node.push_back(result);
			    if(check_char(offset.top(), ";")) {
				    p_node.push_back(Node(tokens[offset.top()], __func__));
				    found = true;
			    }
			    else {}															// It will be an error
		    }
		    else {}																// It will be an error
	    }

	    if(found) {
		    commit(1);
		    return p_node;
	    }
	    else {

		    return Node(Token(), __func__);
	    }
    }

    Node Parser::declaration_specifiers() {						// Recursive
	    prepare();
	    bool found = false;
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(check_type(offset.top(), Type::TYPEDEF)) {
		    p_node.push_back(Node(tokens[offset.top()], __func__));
            ++offset.top();
		    if(!check_type(result = declaration_specifiers())) {
			    p_node.push_back(result);
			    found = true;
		    }
		    found = true;
	    }
	    else if(!check_type(result = type_specifier())) {
		    p_node.push_back(result);
		    if(!check_type(result = declaration_specifiers())) {
			    p_node.push_back(result);
			    found = true;
		    }
		    found = true;
	    }

	    if(found) {
		    commit(0);
		    return p_node;
	    }
	    else {

		    return Node(Token(), __func__);
	    }
    }

    Node Parser::init_declarator_list() {						// Recursive
	    prepare();
	    bool found = false;
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = init_declarator())) {
		    p_node.push_back(result);
		    if(check_char(offset.top(), ",")) {
			    p_node.push_back(Node(tokens[offset.top()], __func__));
                ++offset.top();
			    if(!check_type(result = init_declarator_list())) {
				    p_node.push_back(result);
				    found = true;
			    }
			    else {}																		// It will be an error
		    }
		    found = true; 
	    }

	    if(found) {
		    commit(0);
		    return p_node;
	    }
	    else {

		    return Node(Token(), __func__);
	    }
    }

    Node Parser::init_declarator() {
	    prepare();
	    bool found = false;
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = direct_declarator(false))) {
		    p_node.push_back(result);
		    if(check_char(offset.top(), "=")) {
			    p_node.push_back(Node(tokens[offset.top()], __func__));
                ++offset.top();
			    if(!check_type(result = initializer())) {
				    p_node.push_back(result);
				    found = true;
			    }
			    else {}																		// It will be an error
		    }
		    found = true;
	    }

	    if(found) {
		    commit(0);
		    return p_node;
	    }
	    else {

		    return Node(Token(), __func__);
	    }
    }

    Node Parser::type_specifier() {
	    prepare();
	    int found = 0;	// Types: 0 - nothing found, 1 - found function, 2 - found token
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
			    p_node.push_back(Node(tokens[offset.top()], __func__));
			    found = 2;
			    break;
		    default: {
			    if(!check_type(result = struct_or_union_specifier())) {
				    p_node.push_back(result);
				    found = 1;
			    }
			    else if(!check_type(result = enum_specifier())) {
				    p_node.push_back(result);
				    found = 1;
			    }
			    break;
		    }
	    }

	    if(found) {
		    commit(found - 1);
		    return p_node;
	    }
	    else {

		    return Node(Token(), __func__);
	    }
    }

    Node Parser::struct_or_union_specifier() {
	    prepare();
	    int found = 0;	// Types: 0 - nothing found, 1 - found other token, 2 - found IDENTIFIER as last
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(check_type(offset.top(), Type::STRUCT)) {
		    p_node.push_back(Node(tokens[offset.top()], __func__));
		    if(check_type(offset.top() + 1, Type::IDENTIFIER)) {
			    p_node.push_back(Node(tokens[offset.top() + 1], __func__));
			    if(check_char(offset.top() + 2, "{")) {
				    p_node.push_back(Node(tokens[offset.top() + 2], __func__));
                    offset.top() += 3;
				    if(!check_type(result = struct_declaration_list())) {
					    p_node.push_back(result);
					    if(check_char(offset.top(), "}")) {
						    p_node.push_back(Node(tokens[offset.top()], __func__));
						    found = 1;
					    }
					    else {}																	// It will be an error
				    }
				    else {}																		// It will be an error
			    }
			    found = 2;
		    }
		    else if(check_char(offset.top() + 1, "{")) {
			    p_node.push_back(Node(tokens[offset.top() + 1], __func__));
                offset.top() += 2;
			    if(!check_type(result = struct_declaration_list())) {
				    p_node.push_back(result);
				    if(check_char(offset.top(), "}")) {
					    p_node.push_back(Node(tokens[offset.top()], __func__));
					    found = 1;
				    }
				    else {}																		// It will be an error
			    }
			    else {}																			// It will be an error
		    }
		    else {}																				// It will be an error
	    }

	    if(found) {
            commit(found);
		    return p_node;
	    }
	    else {

		    return Node(Token(), __func__);
	    }
    }

    Node Parser::struct_declaration_list() {							// Recursive
	    prepare();
	    bool found = true;
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = specifier_qualifier_list())) {
		    p_node.push_back(result);
		    if(!check_type(result = struct_declarator_list())) {
			    p_node.push_back(result);
			    if(check_char(offset.top(), ";")) {
				    p_node.push_back(Node(tokens[offset.top()], __func__));
                    ++offset.top();
				    if(!check_type(result = struct_declaration_list())) {
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
		    commit(0);
		    return p_node;
	    }
	    else {

		    return Node(Token(), __func__);
	    }
    }

    Node Parser::specifier_qualifier_list() {						// Recursive
	    prepare();
	    bool found = false;
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = type_specifier())) {
		    p_node.push_back(result);
		    if(!check_type(result = specifier_qualifier_list())) {
			    p_node.push_back(result);
			    found = true;
		    }
		    found = true; 
	    }

	    if(found) {
		    commit(0);
		    return p_node;
	    }
	    else {

		    return Node(Token(), __func__);
	    }
    }

    Node Parser::struct_declarator_list() {							// Recursive
	    prepare();
	    bool found = false;
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = struct_declarator())) {
		    p_node.push_back(result);
		    if(check_char(offset.top(), ",")) {
			    p_node.push_back(Node(tokens[offset.top()], __func__));
                ++offset.top();
			    if(!check_type(result = struct_declarator_list())) {
				    p_node.push_back(result);
				    found = true;
			    }
			    else {}																			// It will be an error
		    }
		    found = true; 
	    }
	
	    if(found) {
		    commit(0);
		    return p_node;
	    }
	    else {

		    return Node(Token(), __func__);
	    }
    }

    Node Parser::struct_declarator() {
	    prepare();
	    bool found = false;
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = direct_declarator(false))) {
		    p_node.push_back(result);
		    if(check_char(offset.top(), ":")) {
			    p_node.push_back(Node(tokens[offset.top()], __func__));
                ++offset.top();
			    if(!check_type(result = conditional_expression())) {
				    p_node.push_back(result);
				    found = true;
			    }
			    else {}																// It will be an error
		    }
		    found = true;
	    }
	    else if(check_char(offset.top(), ":")) {
		    p_node.push_back(Node(tokens[offset.top()], __func__));
            ++offset.top();
		    if(!check_type(result = conditional_expression())) {
			    p_node.push_back(result);
			    found = true;
		    }
		    else {}																	// It will be an error
	    }

	    if(found) {
		    commit(0);
		    return p_node;
	    }
	    else {

		    return Node(Token(), __func__);
	    }
    }

    Node Parser::enum_specifier() {
	    prepare();
	    int found = 0; // Types: 0 - nothing found, 1 - found other token, 2 - found IDENTIFIER as last
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(check_type(offset.top(), Type::ENUM)) {
		    p_node.push_back(Node(tokens[offset.top()], __func__));
		    if(check_type(offset.top() + 1, Type::IDENTIFIER)) {
			    p_node.push_back(Node(tokens[offset.top() + 1], __func__));
			    if(check_char(offset.top() + 2, "{")) {
				    p_node.push_back(Node(tokens[offset.top() + 2], __func__));
                    offset.top() += 3;
				    if(!check_type(result = enumerator_list())) {
					    p_node.push_back(result);
					    if(check_char(offset.top(), "}")) {
						    p_node.push_back(Node(tokens[offset.top()], __func__));
						    found = 1;
					    }
					    else {}																// It will be an error
				    }
				    else {}																	// It will be an error
			    }
			    found = 2;
		    }
		    else if(check_char(offset.top() + 1, "{")) {
			    p_node.push_back(Node(tokens[offset.top() + 1], __func__));
                offset.top() += 2;
			    if(!check_type(result = enumerator_list())) {
				    p_node.push_back(result);
				    if(check_char(offset.top(), "}")) {
					    p_node.push_back(Node(tokens[offset.top()], __func__));
					    found = 1;
				    }
				    else {}																	// It will be an error
			    }
			    else {}																		// It will be an error
		    }
		    else {}																			// It will be an error
	    }

	    if(found) {
            commit(found);
		    return p_node;
	    }
	    else {

		    return Node(Token(), __func__);
	    }
    }

    Node Parser::enumerator_list() {							// Recursive
	    prepare();
	    bool found = false;
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = enumerator())) {
		    p_node.push_back(result);
		    if(check_char(offset.top(), ",")) {
			    p_node.push_back(Node(tokens[offset.top()], __func__));
                ++offset.top();
			    if(!check_type(result = enumerator_list())) {
				    p_node.push_back(result);
				    found = true;
			    }
			    else {}																	// It will be an error
		    }
		    found = true; 
	    }

	    if(found) {
		    commit(0);
		    return p_node;
	    }
	    else {

		    return Node(Token(), __func__);
	    }
    }

    Node Parser::enumerator() {
	    prepare();
	    int found = 0;	// Types: 0 - nothing found, 1 - found function, 2 - found token
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(check_type(offset.top(), Type::IDENTIFIER)) {
		    p_node.push_back(Node(tokens[offset.top()], __func__));
		    if(check_char(offset.top() + 1, "=")) {
			    p_node.push_back(Node(tokens[offset.top() + 1], __func__));
                offset.top() += 2;
			    if(!check_type(result = conditional_expression())) {
				    p_node.push_back(result);
				    found = 1;
			    }
			    else {}																		// It will be an error
		    }
		    found = 2;
	    }

	    if(found) {
            commit(found - 1);
		    return p_node;
	    }
	    else {

		    return Node(Token(), __func__);
	    }
    }

    Node Parser::direct_declarator(bool recursive) {							// Recursive
	    prepare();
	    bool found = false;
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

        auto conditional_inc = [&]() {
            ++offset.top();
            return !check_type(result = conditional_expression());
        };

        auto parameter_inc = [&]() {
            ++offset.top();
            return !check_type(result = parameter_list());
        };

	    if(!recursive) {
		    if(check_type(offset.top(), Type::IDENTIFIER)) {
			    p_node.push_back(Node(tokens[offset.top()], __func__));
                ++offset.top();
			    if(!check_type(result = direct_declarator(true))) {
				    p_node.push_back(result);
				    found = true;
			    }
			    found = true;
		    }
		    else if(check_char(offset.top(), "(")) {
			    p_node.push_back(Node(tokens[offset.top()], __func__));
                ++offset.top();
			    if(!check_type(result = direct_declarator(false))) {
				    p_node.push_back(result);
				    if(check_char(offset.top(), ")")) {
					    p_node.push_back(Node(tokens[offset.top()], __func__));
                        ++offset.top();
					    if(!check_type(result = direct_declarator(true))) {
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
		    if(check_char(offset.top(), "[")) {
			    p_node.push_back(Node(tokens[offset.top()], __func__));
			    if(check_char(offset.top() + 1, "]")) {
				    p_node.push_back(Node(tokens[offset.top() + 1], __func__));
                    offset.top() += 2;
				    if(!check_type(result = direct_declarator(true))) {
					    p_node.push_back(result);
					    found = true;
				    }
				    found = true;
			    }
			    else if(conditional_inc()) {
				    p_node.push_back(result);
				    if(check_char(offset.top(), "]")) {
					    p_node.push_back(Node(tokens[offset.top()], __func__));
                        ++offset.top();
					    if(!check_type(result = direct_declarator(true))) {
						    p_node.push_back(result);
						    found = true;
					    }
					    found = true;
				    }
				    else {}																		// It will be an error
			    }
			    else {}																			// It will be an error
		    }
		    else if(check_char(offset.top(), "(")) {
			    p_node.push_back(Node(tokens[offset.top()], __func__));
			    if(check_char(offset.top() + 1, ")")) {
				    p_node.push_back(Node(tokens[offset.top() + 1], __func__));
                    offset.top() += 2;
				    if(!check_type(result = direct_declarator(true))) {
					    p_node.push_back(result);
					    found = true;
				    }
				    found = true;
			    }
			    else if(parameter_inc()) {
				    p_node.push_back(result);
				    if(check_char(offset.top(), ")")) {
					    p_node.push_back(Node(tokens[offset.top()], __func__));
                        ++offset.top();
					    if(!check_type(result = direct_declarator(true))) {
						    p_node.push_back(result);
						    found = true;
					    }
					    found = true;
				    }
				    else {}																		// It will be an error
			    }

			    if(!found) {
                    offset.pop();
                    prepare();
                    ++offset.top();
				    if(!check_type(result = identifier_list())) {
					    p_node.push_back(result);
					    if(check_char(offset.top(), ")")) {
						    p_node.push_back(Node(tokens[offset.top()], __func__));
                            ++offset.top();
						    if(!check_type(result = direct_declarator(true))) {
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
		    commit(0);
		    return p_node;
	    }
	    else {

		    return Node(Token(), __func__);
	    }
    }

    Node Parser::parameter_list() {					// Recursive
	    prepare();
	    bool found = false;
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = parameter_declaration())) {
		    p_node.push_back(result);
		    if(check_char(offset.top(), ",")) {
			    p_node.push_back(Node(tokens[offset.top()], __func__));
                ++offset.top();
			    if(!check_type(result = parameter_list())) {
				    p_node.push_back(result);
				    found = true;
			    }
			    else {}																// It will be an error
		    }
		    found = true; 
	    }

	    if(found) {
		    commit(0);
		    return p_node;
	    }
	    else {

		    return Node(Token(), __func__);
	    }
    }

    Node Parser::parameter_declaration() {
	    prepare();
	    bool found = false;
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = declaration_specifiers())) {
		    p_node.push_back(result);
		    if(!check_type(result = direct_declarator(false))) {
			    p_node.push_back(result);
			    found = true;
		    }
		    else if(!check_type(result = direct_abstract_declarator(false))) {
			    p_node.push_back(result);
			    found = true;
		    }
		    found = true;
	    }

	    if(found) {
		    commit(0);
		    return p_node;
	    }
	    else {

		    return Node(Token(), __func__);
	    }
    }

    Node Parser::identifier_list() {						// Recursive
	    prepare();
	    int found = 0;	// Types: 0 - nothing found, 1 - end of recursion, 2 - ends with token
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(check_type(offset.top(), Type::IDENTIFIER)) {
		    p_node.push_back(Node(tokens[offset.top()], __func__));
		    if(check_char(offset.top() + 1, ",")) {
			    p_node.push_back(Node(tokens[offset.top() + 1], __func__));
                offset.top() += 2;
			    if(!check_type(result = identifier_list())) {
				    p_node.push_back(result);
				    found = 1;
			    }
			    else {}															// It will be an error
		    }
		    found = 2;
	    }

	    if(found) {
		    commit(found - 1);
		    return p_node;
	    }
	    else {

		    return Node(Token(), __func__);
	    }
    }

    Node Parser::type_name() {
	    prepare();
	    bool found = false;
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = specifier_qualifier_list())) {
		    p_node.push_back(result);
		    if(!check_type(result = direct_abstract_declarator(false))) {
			    p_node.push_back(result);
			    found = true;
		    }
		    found = true;
	    }

	    if(found) {
		    commit(0);
		    return p_node;
	    }
	    else {

		    return Node(Token(), __func__);
	    }
    }

    Node Parser::direct_abstract_declarator(bool recursive) {								// Recursive
	    prepare();
	    bool found = false;
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

        auto parameter_inc = [&]() {
            ++offset.top();
            return !check_type(result = parameter_list());
        };

        auto conditional_inc = [&]() {
            ++offset.top();
            return !check_type(result = conditional_expression());
        };

	    if(check_char(offset.top(), "(")) {
		    p_node.push_back(Node(tokens[offset.top()], __func__));
		    if(!recursive) {
                ++offset.top();
			    if(!check_type(result = direct_abstract_declarator(false))) {
				    p_node.push_back(result);
				    if(check_char(offset.top(), ")")) {
					    p_node.push_back(Node(tokens[offset.top()], __func__));
                        ++offset.top();
					    if(!check_type(result = direct_abstract_declarator(true))) {
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
			    if(check_char(offset.top() + 1, ")")) {
				    p_node.push_back(Node(tokens[offset.top() + 1], __func__));
                    offset.top() += 2;
				    if(!check_type(result = direct_abstract_declarator(true))) {
					    p_node.push_back(result);
					    found = true;
				    }
				    found = true;
			    }
			    else if(parameter_inc()) {
				    p_node.push_back(result);
				    if(check_char(offset.top(), ")")) {
					    p_node.push_back(Node(tokens[offset.top()], __func__));
                        ++offset.top();
					    if(!check_type(result = direct_abstract_declarator(true))) {
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
	    else if(check_char(offset.top(), "[")) {
		    p_node.push_back(Node(tokens[offset.top()], __func__));
		    if(check_char(offset.top() + 1, "]")) {
			    p_node.push_back(Node(tokens[offset.top() + 1], __func__));
                offset.top() += 2;
			    if(!check_type(result = direct_abstract_declarator(true))) {
				    p_node.push_back(result);
				    found = true;
			    }
			    found = true;
		    }
		    else if(conditional_inc()) {
			    p_node.push_back(result);
			    if(check_char(offset.top(), "]")) {
				    p_node.push_back(Node(tokens[offset.top()], __func__));
                    ++offset.top();
				    if(!check_type(result = direct_abstract_declarator(true))) {
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
		    commit(0);
		    return p_node;
	    }
	    else {

		    return Node(Token(), __func__);
	    }
    }

    Node Parser::initializer() {
	    prepare();
	    int found = 0;	// Types: 0 - nothing found, 1 - found function, 2 - found token
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = assignment_expression())) {
		    p_node.push_back(result);
		    found = 1;
	    }
	    else if(check_char(offset.top(), "{")) {
		    p_node.push_back(Node(tokens[offset.top()], __func__));
            ++offset.top();
		    if(!check_type(result = initializer_list())) {
			    p_node.push_back(result);
			    if(check_char(offset.top(), "}")) {
				    p_node.push_back(Node(tokens[offset.top()], __func__));
				    found = 2;
			    }
			    else {}																		// It will be an error
		    }
		    else {}																			// It will be an error
	    }

	    if(found) {
		    commit(found - 1);
		    return p_node;
	    }
	    else {

		    return Node(Token(), __func__);
	    }
    }

    Node Parser::initializer_list() {			// Recursive
	    prepare();
	    bool found = false;
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = initializer())) {
		    p_node.push_back(result);
		    if(check_char(offset.top(), ",")) {
			    p_node.push_back(Node(tokens[offset.top()], __func__));
                ++offset.top();
			    if(!check_type(result = initializer_list())) {
				    p_node.push_back(result);
				    found = true;
			    }
			    else {}															// It will be an error
		    }
		    found = true; 
	    }

	    if(found) {
		    commit(0);
		    return p_node;
	    }
	    else {

		    return Node(Token(), __func__);
	    }
    }

    Node Parser::statement() {
	    prepare();
	    bool found = false;
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = labeled_statement())) {
		    p_node.push_back(result);
		    found = true;
	    }
	    else if(!check_type(result = compound_statement())) {
		    p_node.push_back(result);
		    found = true;
	    }
	    else if(!check_type(result = expression_statement())) {
		    p_node.push_back(result);
		    found = true;
	    }
	    else if(!check_type(result = selection_statement())) {
		    p_node.push_back(result);
		    found = true;
	    }
	    else if(!check_type(result = iteration_statement())) {
		    p_node.push_back(result);
		    found = true;
	    }
	    else if(!check_type(result = jump_statement())) {
		    p_node.push_back(result);
		    found = true;
	    }

	    if(found) {
		    commit(0);
		    return p_node;
	    }
	    else {

		    return Node(Token(), __func__);
	    }
    }

    Node Parser::labeled_statement() {
	    prepare();
	    bool found = false;
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(check_type(offset.top(), Type::CASE)) {
		    p_node.push_back(Node(tokens[offset.top()], __func__));
            ++offset.top();
		    if(!check_type(result = conditional_expression())) {
			    p_node.push_back(result);
			    if(check_char(offset.top(), ":")) {
				    p_node.push_back(Node(tokens[offset.top()], __func__));
                    ++offset.top();
				    if(!check_type(result = statement())) {
					    p_node.push_back(result);
					    found = true;
				    }
				    else {}															// It will be an error
			    }
			    else {}																// It will be an error
		    }
		    else {}																	// It will be an error
	    }
	    else if(check_type(offset.top(), Type::DEFAULT)) {
		    p_node.push_back(Node(tokens[offset.top()], __func__));
		    if(check_char(offset.top() + 1, ":")) {
			    p_node.push_back(Node(tokens[offset.top() + 1], __func__));
                offset.top() += 2;
			    if(!check_type(result = statement())) {
				    p_node.push_back(result);
				    found = true;
			    }
			    else {}															// It will be an error
		    }
		    else {}																// It will be an error
	    }

	    if(found) {
		    commit(0);
		    return p_node;
	    }
	    else {

		    return Node(Token(), __func__);
	    }
    }

    Node Parser::compound_statement() {
	    prepare();
	    int found = 0;	// Types: 0 - nothing found, 1 - found parentesis with function, 2 - found only parentesis
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

        auto statement_inc = [&]() {
            ++offset.top();
            return !check_type(result = statement_list());
        };

	    if(check_char(offset.top(), "{")) {
		    p_node.push_back(Node(tokens[offset.top()], __func__));
		    if(check_char(offset.top() + 1, "}")) {
			    p_node.push_back(Node(tokens[offset.top()], __func__));
			    found = 2;
		    }
		    else if(statement_inc()) {
			    p_node.push_back(result);
			    if(check_char(offset.top(), "}")) {
				    p_node.push_back(Node(tokens[offset.top()], __func__));
				    found = 1;
			    }
			    else {}																	// It will be an error
		    }

		    if(!found) {
                offset.pop();
                prepare();
                ++offset.top();
			    if(!check_type(result = declaration_list())) {
				    p_node.push_back(result);
				    if(check_char(offset.top(), "}")) {
					    p_node.push_back(Node(tokens[offset.top()], __func__));
					    found = 1;
				    }
				    else if(!check_type(result = statement_list())) {
					    p_node.push_back(result);
					    if(check_char(offset.top(), "}")) {
						    p_node.push_back(Node(tokens[offset.top()], __func__));
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
		    commit(found);
		    return p_node;
	    }
	    else {

		    return Node(Token(), __func__);
	    }
    }

    Node Parser::declaration_list() {			// Recursive
	    prepare();
	    bool found = false;
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = declaration())) {
		    p_node.push_back(result);
		    if(!check_type(result = declaration_list())) {
			    p_node.push_back(result);
			    found = true;
		    }
		    found = true; 
	    }

	    if(found) {
		    commit(0);
		    return p_node;
	    }
	    else {

		    return Node(Token(), __func__);
	    }
    }

    Node Parser::statement_list() {				// Recursive
	    prepare();
	    bool found = false;
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = statement())) {
		    p_node.push_back(result);
		    if(!check_type(result = statement_list())) {
			    p_node.push_back(result);
			    found = true;
		    }
		    found = true; 
	    }

	    if(found) {
		    commit(0);
		    return p_node;
	    }
	    else {

		    return Node(Token(), __func__);
	    }
    }

    Node Parser::expression_statement() {
	    prepare();
	    bool found = false;
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(check_char(offset.top(), ";")) {
		    p_node.push_back(Node(tokens[offset.top()], __func__));
		    found = true;
	    }
	    else if(!check_type(result = expression())) {
		    p_node.push_back(result);
		    if(check_char(offset.top(), ";")) {
			    p_node.push_back(Node(tokens[offset.top()], __func__));
			    found = true;
		    }
		    else {}																// It will be an error
	    }

	    if(found) {
		    commit(1);
		    return p_node;
	    }
	    else {

		    return Node(Token(), __func__);
	    }
    }

    Node Parser::selection_statement() {
	    prepare();
	    bool found = false;
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(check_type(offset.top(), Type::IF)) {
		    p_node.push_back(Node(tokens[offset.top()], __func__));
		    if(check_char(offset.top() + 1, "(")) {
			    p_node.push_back(Node(tokens[offset.top() + 1], __func__));
                offset.top() += 2;
			    if(!check_type(result = expression())) {
				    p_node.push_back(result);
				    if(check_char(offset.top(), ")")) {
					    p_node.push_back(Node(tokens[offset.top()], __func__));
                        ++offset.top();
					    if(!check_type(result = statement())) {
						    p_node.push_back(result);
						    if(check_type(offset.top(), Type::ELSE)) {
							    p_node.push_back(Node(tokens[offset.top()], __func__));
                                ++offset.top();
							    if(!check_type(result = statement())) {
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
	    else if(check_type(offset.top(), Type::SWITCH)) {
		    p_node.push_back(Node(tokens[offset.top()], __func__));
		    if(check_char(offset.top() + 1, "(")) {
			    p_node.push_back(Node(tokens[offset.top() + 1], __func__));
                offset.top() += 2;
			    if(!check_type(result = expression())) {
				    p_node.push_back(result);
				    if(check_char(offset.top(), ")")) {
					    p_node.push_back(Node(tokens[offset.top()], __func__));
                        ++offset.top();
					    if(!check_type(result = statement())) {
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
		    commit(0);
		    return p_node;
	    }
	    else {

		    return Node(Token(), __func__);
	    }
    }

    Node Parser::iteration_statement() {
	    prepare();
	    int found = 0;	// Types: 0 - nothing found, 1 - found function, 2 - found token
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(check_type(offset.top(), Type::WHILE)) {
		    p_node.push_back(Node(tokens[offset.top()], __func__));
		    if(check_char(offset.top() + 1, "(")) {
			    p_node.push_back(Node(tokens[offset.top() + 1], __func__));
                offset.top() += 2;
			    if(!check_type(result = expression())) {
				    p_node.push_back(result);
				    if(check_char(offset.top(), ")")) {
					    p_node.push_back(Node(tokens[offset.top()], __func__));
                        ++offset.top();
					    if(!check_type(result = statement())) {
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
	    else if(check_type(offset.top(), Type::DO)) {
		    p_node.push_back(Node(tokens[offset.top()], __func__));
            ++offset.top();
		    if(!check_type(result = statement())) {
			    p_node.push_back(result);
			    if(check_type(offset.top(), Type::WHILE)) {
				    p_node.push_back(Node(tokens[offset.top()], __func__));
				    if(check_char(offset.top() + 1, "(")) {
					    p_node.push_back(Node(tokens[offset.top() + 1], __func__));
                        offset.top() += 2;
					    if(!check_type(result = expression())) {
						    p_node.push_back(result);
						    if(check_char(offset.top(), ")")) {
							    p_node.push_back(Node(tokens[offset.top()], __func__));
							    if(check_char(offset.top() + 1, ";")) {
								    p_node.push_back(Node(tokens[offset.top() + 1], __func__));
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
	    else if(check_type(offset.top(), Type::FOR)) {
		    p_node.push_back(Node(tokens[offset.top()], __func__));
		    if(check_char(offset.top() + 1, "(")) {
			    p_node.push_back(Node(tokens[offset.top() + 1], __func__));
                offset.top() += 2;
			    if(!check_type(result = expression_statement())) {
				    p_node.push_back(result);
				    if(!check_type(result = expression_statement())) {
					    p_node.push_back(result);
					    if(check_char(offset.top(), ")")) {
						    p_node.push_back(Node(tokens[offset.top()], __func__));
                            ++offset.top();
						    if(!check_type(result = statement())) {
							    p_node.push_back(result);
							    found = 1;
						    }
						    else {}															// It will be an error
					    }
					    else if(!check_type(result = expression())) {
						    p_node.push_back(result);
						    if(check_char(offset.top(), ")")) {
							    p_node.push_back(Node(tokens[offset.top()], __func__));
                                ++offset.top();
							    if(!check_type(result = statement())) {
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
		    commit(2);
		    return p_node;
	    }
	    else if(found == 1) {
		    commit(0);
		    return p_node;
	    }
	    else {

		    return Node(Token(), __func__);
	    }
    }

    Node Parser::jump_statement() {
	    prepare();
	    int found = 0;	// Types: 0 - nothing found, 1 - found token after function, 2 - found only token
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

        auto expression_inc = [&]() {
            ++offset.top();
            return !check_type(result = expression());
        };

	    if(check_type(offset.top(), Type::BREAK)) {
		    p_node.push_back(Node(tokens[offset.top()], __func__));
		    if(check_char(offset.top() + 1, ";")) {
			    p_node.push_back(Node(tokens[offset.top() + 1], __func__));
			    found = 2;
		    }
		    else {}															// It will be an error
	    }
	    else if(check_type(offset.top(), Type::RETURN)) {
		    p_node.push_back(Node(tokens[offset.top()], __func__));
		    if(check_char(offset.top() + 1, ";")) {
			    p_node.push_back(Node(tokens[offset.top() + 1], __func__));
			    found = 2;
		    }
		    else if(expression_inc()) {
			    p_node.push_back(result);
			    if(check_char(offset.top(), ";")) {
				    p_node.push_back(Node(tokens[offset.top()], __func__));
				    found = 1;
			    }
			    else {}														// It will be an error
		    }
		    else {}															// It will be an error
	    }

	    if(found) {
		    commit(found);
		    return p_node;
	    }
	    else {

		    return Node(Token(), __func__);
	    }
    }

    Node Parser::translation_unit() {			// Recursive
        prepare();
	    bool found = false;
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = external_declaration())) {
		    p_node.push_back(result);
		    if(!check_type(result = translation_unit())) {
			    p_node.push_back(result);
			    found = true;
		    }
		    found = true; 
	    }

	    if(found) {
            commit(0);
		    return p_node;
	    }
	    else {
		    return Node(Token(), __func__);
	    }
    }

    Node Parser::external_declaration() {
	    prepare();
	    bool found = false;
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = function_definition())) {
		    p_node.push_back(result);
		    found = true;
	    }
	    else if(!check_type(result = declaration())) {
		    p_node.push_back(result);
		    found = true;
	    }

	    if(found) {
		    commit(0);
		    return p_node;
	    }
	    else {

		    return Node(Token(), __func__);
	    }
    }

    Node Parser::function_definition() {
	    prepare();
	    bool found = false;
	    Node p_node(Token(Type::parsing), __func__);
	    Node result;

	    func_log(__func__);

	    if(!check_type(result = declaration_specifiers())) {
		    p_node.push_back(result);
		    if(!check_type(result = direct_declarator(false))) {
			    p_node.push_back(result);
			    if(!check_type(result = declaration_list())) {
				    p_node.push_back(result);
				    if(!check_type(result = compound_statement())) {
					    p_node.push_back(result);
					    found = true;
				    }
				    else {}																	// It will be an error
			    }
			    else if(!check_type(result = compound_statement())) {
				    p_node.push_back(result);
				    found = true;
			    }
			    else {}																		// It will be an error
		    }
		    else {}																			// It will be an error
	    }
	    else if(!check_type(result = direct_declarator(false))) {
		    p_node.push_back(result);
		    if(!check_type(result = declaration_list())) {
			    p_node.push_back(result);
			    if(!check_type(result = compound_statement())) {
				    p_node.push_back(result);
				    found = true;
			    }
			    else {}																		// It will be an error
		    }
		    else if(!check_type(result = compound_statement())) {
			    p_node.push_back(result);
			    found = true;
		    }
		    else {}																			// It will be an error
	    }

	    if(found) {
		    commit(0);
		    return p_node;
	    }
	    else {

		    return Node(Token(), __func__);
	    }
    }

};
