#include "Lexer.h"
#include <cctype>
#include <stdexcept>
#include <iostream>

namespace Translator {
    Lexer::Lexer(Source* s): source(s) {
        current_char = source->next_char();
    }

    void Lexer::get_next_token() {
        std::regex l("[[:alpha:]_]");
        std::regex d("[[:digit:]]");
        std::regex g("[[:graph:]]");

        ignore_whitespaces();

        std::string text{ current_char };

        //if(current_char == EOF)
        //    token.set_type(Type::eof);
        //else if(std::regex_match(text, g)) {

        //}
        //else
        //    token.set_type(Type::none);                                                                 // Unknown token

        if(std::regex_match(text, l)) {																	// Get string or constant char that starts with L"", L'' or get Identifier
            if(current_char == 'L')
                token = get_string_token();
            else 
                token = get_literals_token();
        }
        else if(current_char == '\'' || current_char == '"') {											// Get string or constant char
            std::string x = "";
            token = get_string_token(x, current_char);
        }
        else if(current_char == '0')                                                                    // Get octal and hexal numbers OR zero
            token = get_number_token();										
        else if(std::regex_match(text, d))                                                              // Get digits
            token = get_digit_token(1);									
        else if(current_char == '.') {
            char tmp = source->next_char();
            text = tmp;
            if(std::regex_match(text, d)) {
                source->prev_char();
                token = get_digit_token(0);																// Get digits
            }
            else {
                source->prev_char();
                token = get_other_token();																// Get other tokens
            }
        }
        else if(current_char == EOF)                                                                    // Get EOF
            token.set_type(Type::eof);											
        else if(std::regex_match(text, g)) {
            if(current_char == '/') {
                char tmp;
                if((tmp = source->next_char()) == '/') 
                    token = comment(1);
                else if(tmp == '*') 
                    token = comment(0);
                else {
                    source->prev_char();
                    token = get_other_token();															// Get other tokens
                }
            }
            else
                token = get_other_token();																// Get other tokens
        }
        else                                                                                            // Unknown token
            token.set_type(Type::none);																
    }

    void Lexer::ignore_whitespaces() {
        while(isspace(current_char)) {
            current_char = source->next_char();
#ifdef _DEBUG
            std::cout << static_cast<int>(current_char) << " '" << current_char << "' " << "symbol number: " << source->get_pos().char_num << std::endl;
#endif
        }
    }

    // Type 0 is block "/*", type 1 is line "//"
    Token Lexer::comment(bool type) {
        Token tmp;
        if(type) source->next_line();
        else {
            char c;
            do {
                while((c = source->next_char()) != '*' && c != 0);
            } while(((c = source->next_char()) != '/' && c != 0));
        }
        source->set_new_line(false);
        source->set_after_comment(true);
        tmp.set_type(Type::comment);
        return tmp;
    }

    Token Lexer::get_literals_token() {
        std::regex d("[[:digit:]]");
        std::regex l("[[:alpha:]_]");

        Token tmp;
        std::string x = "";
        std::string text = "";

        do {
#ifdef _DEBUG
            std::cout << current_char;
#endif
            x += current_char;
            current_char = source->next_char();
            if(source->is_new_line() || isspace(current_char)) break;
            text = current_char;
        } while(std::regex_match(text, l) || std::regex_match(text, d));

        if(current_char != EOF) source->prev_char();

        tmp.set_chars(x);

        auto pair = map.find(x);
        if(pair != map.end()) {
            tmp.set_type(pair->second);
            return tmp;
        }

        tmp.set_type(Type::IDENTIFIER);
        return tmp;
    }

    Token Lexer::get_string_token() {
        std::string x = "";
        x += current_char;
        current_char = source->next_char();
        return get_string_token(x, current_char);
    }

    Token Lexer::get_string_token(std::string z, char curr) {
        std::regex p("[[:print:]]");

        Token tmp;
        std::string x = z;
        std::string text = "";

#ifdef _DEBUG
        std::cout << current_char;
#endif

        x += current_char;
        current_char = source->next_char();

        do {
#ifdef _DEBUG
            std::cout << current_char;
#endif
            if(current_char == '\\') {
                x += current_char;
                current_char = source->next_char();
                if(source->is_new_line() && current_char == curr) {
                    x += current_char;
                    current_char = source->next_char();
                    break;
                }
            }
            else if(current_char == curr) {
                x += current_char;
                current_char = source->next_char();
                break;
            }
            x += current_char;
            current_char = source->next_char();
            if(source->is_new_line()) {								//It will be an error
                tmp.set_type(Type::error);
                return tmp;
            }
            text = current_char;
        } while(std::regex_match(text, p));

        if(current_char != EOF) source->prev_char();

        tmp.set_chars(x);

        if(curr == '\'') tmp.set_type(Type::CONSTANT);
        else if(curr == '"') tmp.set_type(Type::STRING_LITERAL);
        return tmp;
    }

    Token Lexer::get_number_token() {
        std::regex o("[0-7]");
        std::regex d("[[:digit:]]");
        std::regex h("[[:xdigit:]]");
        std::regex xs("[xX]");
        std::regex g("[;,)]");

        Token tmp;
        std::string x = "";
        std::string text = "";

        x += current_char;
        current_char = source->next_char();
        text = current_char;

        if(std::regex_match(text, xs)) {						//Hexadecimal
            x += current_char;
            current_char = source->next_char();
            if(source->is_new_line() || isspace(current_char)) {			//It will be an error
                tmp.set_type(Type::error);
                return tmp;
            }
            text = current_char;
            while(std::regex_match(text, h)) {
#ifdef _DEBUG
                std::cout << current_char;
#endif
                x += current_char;
                current_char = source->next_char();
                if(source->is_new_line() || isspace(current_char)) break;
                text = current_char;
                if(!std::regex_match(text, h) && !std::regex_match(text, g)) {		//It will be an error
                    tmp.set_type(Type::error);
                    return tmp;
                }
            }
        }
        else if(std::regex_match(text, o)) {					//Octal
            do {
#ifdef _DEBUG
                std::cout << current_char;
#endif
                x += current_char;
                current_char = source->next_char();
                if(source->is_new_line() || isspace(current_char)) break;
                text = current_char;
                if(!std::regex_match(text, o) && !std::regex_match(text, g)) {		//It will be an error
                    tmp.set_type(Type::error);
                    return tmp;
                }
            } while(std::regex_match(text, o));
        }
        else if(current_char == '.') {							//Decimal starting with 0
            do {
#ifdef _DEBUG
                std::cout << current_char;
#endif
                x += current_char;
                current_char = source->next_char();
                if(source->is_new_line() || isspace(current_char)) break;
                text = current_char;
                if(!std::regex_match(text, d) && !std::regex_match(text, g)) {		//It will be an error
                    tmp.set_type(Type::error);
                    return tmp;
                }
            } while(std::regex_match(text, d));
        }
        else if(!std::regex_match(text, g) && !source->is_new_line() && !isspace(current_char)) {			//It will be an error
            tmp.set_type(Type::error);
            return tmp;
        }

        if(current_char != EOF) source->prev_char();

        tmp.set_chars(x);
        tmp.set_type(Type::CONSTANT);
        return tmp;
    }

    // Type 0 starts with '.', type 1 starts with digit
    Token Lexer::get_digit_token(bool type) {
        std::regex d("[[:digit:]]");
        std::regex g("[;,)]");

        Token tmp;
        std::string x = "";
        std::string text = "";

        do {
#ifdef _DEBUG
            std::cout << current_char;
#endif
            x += current_char;
            current_char = source->next_char();
            if(source->is_new_line() || isspace(current_char)) break;
            text = current_char;
            if(!std::regex_match(text, d) && !std::regex_match(text, g)) {
                if(type && current_char == '.') {
                    do {
#ifdef _DEBUG
                        std::cout << current_char;
#endif
                        x += current_char;
                        current_char = source->next_char();
                        if(source->is_new_line() || isspace(current_char)) break;
                        text = current_char;
                        if(!std::regex_match(text, d) && !std::regex_match(text, g)) {		//It will be an error
                            tmp.set_type(Type::error);
                            return tmp;
                        }
                    } while(std::regex_match(text, d));
                }
                else {												//It will be an error
                    tmp.set_type(Type::error);
                    return tmp;
                }
                break;
            }
        } while(std::regex_match(text, d));

        if(current_char != EOF) source->prev_char();

        tmp.set_chars(x);

        tmp.set_type(Type::CONSTANT);
        return tmp;
    }

    Token Lexer::get_other_token() {
        std::regex p("[!%&*+-/<=>|]");

        Token tmp;
        std::string x = "";
        std::string text = "";

        text = current_char;

        if(std::regex_match(text, p)) {
            do {
#ifdef _DEBUG
                std::cout << current_char;
#endif
                x += current_char;
                current_char = source->next_char();
                if(source->is_new_line() || isspace(current_char)) break;
                text = current_char;
            } while(std::regex_match(text, p));
        }
        else {
#ifdef _DEBUG
            std::cout << current_char;
#endif
            if(current_char == '\\') current_char = source->next_char();
            // Error gdy nic nie ma po "\" (Ogarn¹æ)
            x += current_char;
            current_char = source->next_char();
        }

        if(current_char != EOF) source->prev_char();

        tmp.set_chars(x);

        auto pair = map.find(x);
        if(pair != map.end()) {
            tmp.set_type(pair->second);
            return tmp;
        }

        tmp.set_type(Type::other);
        return tmp;
    }

};  //namespace Translator
