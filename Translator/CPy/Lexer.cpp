#include "Lexer.h"
#include <cctype>
#include <stdexcept>
#include <iostream>

namespace Translator {
    Lexer::Lexer(Source* s): source(s) {}

    void Lexer::get_next_token() {
        while(is_whitespace() || is_comment());

        if(source->peek<char>() == std::char_traits<char>::eof()) {                         // Get EOF token
            token.set_type(Type::eof);
        }
        else if(match_str()) {                                                              // Get string literal token
            token.set_type(Type::STRING_LITERAL);
        }
        else if(match_con() || match_hex() || match_dbl() ||                                // Get constant token, either it's a char, hexadecimal, floating point, 
                match_oct() || match_dec()) {                                               // octal or decimal
            token.set_type(Type::CONSTANT);
        }
        else if(match_literals()) {                                                         // Get identifier token or special literal token (if, else, etc.)
            auto found = map.find(match);
            if(found != map.end())
                token.set_type(found->second);
            else
                token.set_type(Type::IDENTIFIER);
        }
        else if(match_graphic()) {                                                          // Get other token or special other token (==, <=, etc.)
            auto found = map.find(match);
            if(found != map.end())
                token.set_type(found->second);
            else
                token.set_type(Type::other);
        }
        else {
            token.set_type(Type::none);                                                     // Unknown token or an error
            match.clear();
        }

        token.set_chars(match);
        unsigned int size = static_cast<unsigned int>(match.size());
        source->change_char(source->get_pos().char_num + size);
        source->change_col(source->get_pos().col_num + size);
    }

    bool Lexer::match_str() {
        match.clear();
        char peeked;
        auto main_func = [&]() {
            match += source->get<char>();
            while((peeked = source->peek<char>()) != std::char_traits<char>::eof()) {
                match += source->get<char>();
                if(peeked == '"' && !odd_escaping(match, match.size()-2))
                    return true;
            }
            return false;   //Throw an error here; And more exceptions
        };

        if((peeked = source->peek<char>()) == 'L') {
            match += source->get<char>();
            if(source->peek<char>() == '"') {
                return main_func();
            }
            else {
                source->unget();
                return false;
            }
        }
        else if(peeked == '"') {
            return main_func();
        }
        else
            return false;
    }

    bool Lexer::match_con() {
        match.clear();
        char peeked;
        auto main_func = [&]() {
            match += source->get<char>();
            for(int i = 0; i < 3 && (peeked = source->peek<char>()) != std::char_traits<char>::eof(); ++i) {
                match += source->get<char>();
                if(peeked == '\'' && !odd_escaping(match, match.size() - 2))
                    return true;
            }
            return false;   //Throw an error here; And more exceptions
        };

        if((peeked = source->peek<char>()) == 'L') {
            match += source->get<char>();
            if(source->peek<char>() == '\'') {
                return main_func();
            }
            else {
                source->unget();
                return false;
            }
        }
        else if(peeked == '\'') {
            return main_func();
        }
        else
            return false;
    }

    bool Lexer::match_hex() {
        match.clear();
        char peeked;

        if(source->peek<char>() == '0') {
            match += source->get<char>();
            if((peeked = source->peek<char>()) == 'x' || peeked == 'X') {
                match += source->get<char>();
                while(isxdigit(source->peek<int>()))
                    match += source->get<char>();
                return true;
            }
            else {
                source->unget();
                return false;
            }
        }
        else
            return false;
    }

    bool Lexer::match_dbl() {
        match.clear();
        std::streampos starting_position = source->tellg();

        if(source->peek<char>() == '.') {
            match += source->get<char>();
            if(isdigit(source->peek<int>())) {
                match += source->get<char>();
                while(isdigit(source->peek<int>()))
                    match += source->get<char>();
                return true;
            }
            else {
                source->unget();
                return false;
            }
        }
        else if(isdigit(source->peek<int>())) {
            match += source->get<char>();
            while(isdigit(source->peek<int>()))
                match += source->get<char>();
            if(source->peek<char>() == '.') {
                match += source->get<char>();
                while(isdigit(source->peek<int>()))
                    match += source->get<char>();
                return true;
            }
            else {
                source->seekg(starting_position);
                return false;
            }
        }
        else
            return false;
    }

    bool Lexer::match_oct() {
        match.clear();
        char peeked;

        if(source->peek<char>() == '0') {
            match += source->get<char>();
            if((peeked = source->peek<char>()) >= '0' && peeked <= '7') {
                match += source->get<char>();
                while((peeked = source->peek<char>()) >= '0' && peeked <= '7')
                    match += source->get<char>();
                return true;
            }
            else {
                source->unget();
                return false;
            }
        }
        else
            return false;
    }

    bool Lexer::match_dec() {
        match.clear();
        char peeked;

        if(source->peek<char>() == '0') {
            match += source->get<char>();
            if(isdigit(source->peek<int>())) {
                source->unget();
                return false;
            }
            else
                return true;
        }
        else if((peeked = source->peek<char>()) >= '1' && peeked <= '9') {
            match += source->get<char>();
            while(isdigit(source->peek<int>()))
                match += source->get<char>();
            return true;
        }
        else
            return false;
    }

    bool Lexer::match_literals() {
        match.clear();

        if(isword(source->peek<int>()) && !isdigit(source->peek<int>())) {
            match += source->get<char>();
            while(isword(source->peek<int>()))
                match += source->get<char>();
            return true;
        }
        else
            return false;
    }

    bool Lexer::match_line_comm() {
        if(source->peek<char>() == '/') {
            source->get<int>();
            if(source->peek<char>() == '/') {
                source->get<int>();
                source->skipline();
                return true;
            }
            else {
                source->unget();
                return false;
            }
        }
        else
            return false;
    }
    
    bool Lexer::match_block_comm() {
        if(source->peek<char>() == '/') {
            source->get<int>();
            source->next_pos();
            if(source->peek<char>() == '*') {
                while(source->get<char>() != std::char_traits<char>::eof()) {
                    source->next_pos();
                    if(source->peek<char>() == '\n') {
                        source->new_line_pos();
                    }
                    else if(source->peek<char>() == '*') {
                        source->get<int>();
                        source->next_pos();
                        char peeked = source->peek<char>();
                        if(peeked == '/') {
                            source->get<int>();
                            source->next_pos();
                            return true;
                        }
                        else if(peeked == std::char_traits<char>::eof())
                            return false;   //Throw an error here
                    }
                }
                return false;   //Throw an error here
            }
            else {
                source->unget();
                return false;
            }
        }
        else
            return false;
    }

    bool Lexer::match_graphic() {
        match.clear();

        switch(source->peek<char>()) {
            case '(':
            case ')':
            case '[':
            case ']':
            case '{':
            case '}':
            case ',':
            case ';':
            case '.':
            case ':':
            case '?': {
                match = source->get<char>();
                return true;
            }
            case '-':
            case '+': {
                match = source->get<char>();
                if(source->peek<char>() == match.back()) {
                    match += source->get<char>();
                    return true;
                }
                else
                    source->unget();
            }
            case '=':
            case '<':
            case '>':
            case '!':
            case '%':
            case '*':
            case '/': {
                match = source->get<char>();
                if(source->peek<char>() == '=')
                    match += source->get<char>();
                return true;
            }
            case '&':
            case '|': {
                match = source->get<char>();
                if(source->peek<char>() == match.back()) {
                    match += source->get<char>();
                    return true;
                }
                else {
                    source->unget();
                    return false;
                }
            }
            default:
                return false;
        }
    }

    bool Lexer::odd_escaping(std::string& str, size_t start_pos) {
        int no_escaping{0};
        for(int i = 0; str.at(start_pos - i) == '\\'; ++i, ++no_escaping);
        return (no_escaping % 2);
    }

    bool Lexer::is_whitespace() {
        char peeked = source->peek<char>();
        if(peeked == '\n') {
            source->get<int>();
            source->new_line_pos();
            new_line = true;
            return true;
        }
        else if(peeked == ' ') {
            source->get<int>();
            source->next_pos();
            return true;
        }
        else if(peeked == '\t') {
            source->get<int>();
            source->next_char();
            int tab = TAB_LEN - (source->get_pos().col_num - 1 % TAB_LEN);
            source->change_col(source->get_pos().col_num + tab);
            return true;
        }
        else
            return false;
    }

    bool Lexer::is_comment() {
        if(match_line_comm()) {                                                         // Skip line comments
            source->new_line_pos();
            return true;
        }
        else if(match_block_comm())                                                     // Skip block comments
            return true;
        else
            return false;
    }

};  //namespace Translator
