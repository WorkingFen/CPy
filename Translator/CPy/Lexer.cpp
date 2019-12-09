#include "Lexer.h"
#include <cctype>
#include <stdexcept>
#include <iostream>

namespace Translator {
    Lexer::Lexer(Source* s): source(s) {
        //source->next_line();
    }

    void Lexer::get_next_token() {
        bool gotit = false;
        std::string match{};

        while(!gotit) {
            while(is_whitespace());

            if(static_cast<char>(source->peek_file()) == std::char_traits<char>::eof()) {       // Get EOF token
                token.set_type(Type::eof);
                break;
            }
            else if(match_str(match)) {                                                         // Get string literal token
                token.set_type(Type::STRING_LITERAL);
                gotit = true;
            }
            else if(match_con(match) || match_hex(match) || match_dbl(match) ||                 // Get constant token, either it's a char, hexadecimal, floating point, 
                    match_oct(match) || match_dec(match)) {                                     // octal or decimal
                token.set_type(Type::CONSTANT);
                gotit = true;
            }
            else if(match_literals(match)) {                                                    // Get identifier token or special literal token (if, else, etc.)
                auto tmp = map.find(match);
                if(tmp != map.end())
                    token.set_type(tmp->second);
                else
                    token.set_type(Type::IDENTIFIER);
                gotit = true;
            }
            else if(match_line_comm()) {                                                        // Skip line comments
                source->change_row();
                source->change_char(1);
                source->change_col(1);
                continue;
            }
            else if(match_block_comm()) {                                                       // Skip block comments
                continue;
            }
            else if(match_graphic(match)) {                                                     // Get other token or special other token (==, <=, etc.)
                auto tmp = map.find(match);
                if(tmp != map.end())
                    token.set_type(tmp->second);
                else
                    token.set_type(Type::other);
                gotit = true;
            }
            else {
                token.set_type(Type::none);                                                     // Unknown token
                gotit = true;
            }

            if(gotit) {
                token.set_chars(match);
                unsigned int size = static_cast<unsigned int>(match.size());
                source->change_char(source->get_pos().char_num + size);
                source->change_col(source->get_pos().col_num + size);
            }
        }
    }

    bool Lexer::match_str(std::string& match) {
        std::string tmp{};
        char peeked;
        auto main_func = [&]() {
            tmp += static_cast<char>(source->get_file());
            while((peeked = static_cast<char>(source->peek_file())) != std::char_traits<char>::eof()) {
                tmp += static_cast<char>(source->get_file());
                if(peeked == '"' && !odd_escaping(tmp, tmp.size()-2)) {
                    match = tmp;
                    return true;
                }
            }
            return false;   //Throw an error here; And more exceptions
        };

        if((peeked = static_cast<char>(source->peek_file())) == 'L') {
            tmp += static_cast<char>(source->get_file());
            if(static_cast<char>(source->peek_file()) == '"') {
                main_func();
            }
            else {
                source->unget_file();
                return false;
            }
        }
        else if(peeked == '"') {
            main_func();
        }
        else
            return false;
    }

    bool Lexer::match_con(std::string& match) {
        std::string tmp{};
        char peeked;
        auto main_func = [&]() {
            tmp += static_cast<char>(source->get_file());
            for(int i = 0; i < 3 && (peeked = static_cast<char>(source->peek_file())) != std::char_traits<char>::eof(); ++i) {
                tmp += static_cast<char>(source->get_file());
                if(peeked == '\'' && !odd_escaping(tmp, tmp.size() - 2)) {
                    match = tmp;
                    return true;
                }
            }
            return false;   //Throw an error here; And more exceptions
        };

        if((peeked = static_cast<char>(source->peek_file())) == 'L') {
            tmp += static_cast<char>(source->get_file());
            if(static_cast<char>(source->peek_file()) == '\'') {
                main_func();
            }
            else {
                source->unget_file();
                return false;
            }
        }
        else if(peeked == '\'') {
            main_func();
        }
        else
            return false;
    }

    bool Lexer::match_hex(std::string& match) {
        std::string tmp{};
        char peeked;

        if(static_cast<char>(source->peek_file()) == '0') {
            tmp += static_cast<char>(source->get_file());
            if((peeked = static_cast<char>(source->peek_file())) == 'x' || peeked == 'X') {
                tmp += static_cast<char>(source->get_file());
                while(isxdigit(source->peek_file()))
                    tmp += static_cast<char>(source->get_file());
                match = tmp;
                return true;
            }
            else {
                source->unget_file();
                return false;
            }
        }
        else
            return false;
    }

    bool Lexer::match_dbl(std::string& match) {
        std::string tmp{};
        std::streampos starting_position = source->tellg_file();

        if(static_cast<char>(source->peek_file()) == '.') {
            tmp += static_cast<char>(source->get_file());
            if(isdigit(source->peek_file())) {
                tmp += static_cast<char>(source->get_file());
                while(isdigit(source->peek_file()))
                    tmp += static_cast<char>(source->get_file());
                match = tmp;
                return true;
            }
            else {
                source->unget_file();
                return false;
            }
        }
        else if(isdigit(source->peek_file())) {
            tmp += static_cast<char>(source->get_file());
            while(isdigit(source->peek_file()))
                tmp += static_cast<char>(source->get_file());
            if(static_cast<char>(source->peek_file()) == '.') {
                tmp += static_cast<char>(source->get_file());
                while(isdigit(source->peek_file()))
                    tmp += static_cast<char>(source->get_file());
                match = tmp;
                return true;
            }
            else {
                source->seekg_file(starting_position);
                return false;
            }
        }
        else
            return false;
    }

    bool Lexer::match_oct(std::string& match) {
        std::string tmp{};
        char peeked;

        if(static_cast<char>(source->peek_file()) == '0') {
            tmp += static_cast<char>(source->get_file());
            if((peeked = static_cast<char>(source->peek_file())) >= '0' && peeked <= '7') {
                tmp += static_cast<char>(source->get_file());
                while((peeked = static_cast<char>(source->peek_file())) >= '0' && peeked <= '7')
                    tmp += static_cast<char>(source->get_file());
                match = tmp;
                return true;
            }
            else {
                source->unget_file();
                return false;
            }
        }
        else
            return false;
    }

    bool Lexer::match_dec(std::string& match) {
        std::string tmp{};
        char peeked;

        if(static_cast<char>(source->peek_file()) == '0') {
            tmp += static_cast<char>(source->get_file());
            if(isdigit(source->peek_file())) {
                source->unget_file();
                return false;
            }
            else {
                match = tmp;
                return true;
            }
        }
        else if((peeked = static_cast<char>(source->peek_file())) >= '1' && peeked <= '9') {
            tmp += static_cast<char>(source->get_file());
            while(isdigit(source->peek_file()))
                tmp += static_cast<char>(source->get_file());
            match = tmp;
            return true;
        }
        else
            return false;
    }

    bool Lexer::match_literals(std::string& match) {
        std::string tmp{};

        if(isword(source->peek_file()) && !isdigit(source->peek_file())) {
            tmp += static_cast<char>(source->get_file());
            while(isword(source->peek_file()))
                tmp += static_cast<char>(source->get_file());
            match = tmp;
            return true;
        }
        else
            return false;
    }

    bool Lexer::match_line_comm() {
        if(static_cast<char>(source->peek_file()) == '/') {
            source->get_file();
            if(static_cast<char>(source->peek_file()) == '/') {
                source->get_file();
                source->getline_file();
                return true;
            }
            else {
                source->unget_file();
                return false;
            }
        }
        else
            return false;
    }
    
    bool Lexer::match_block_comm() {
        if(static_cast<char>(source->peek_file()) == '/') {
            source->get_file();
            source->change_pos();
            if(static_cast<char>(source->peek_file()) == '*') {
                while(static_cast<char>(source->get_file()) != std::char_traits<char>::eof()) {
                    source->change_pos();
                    if(static_cast<char>(source->peek_file()) == '\n') {
                        source->change_row();
                        source->change_char(1);
                        source->change_col(1);
                    }
                    else if(static_cast<char>(source->peek_file()) == '*') {
                        source->get_file();
                        source->change_pos();
                        char peeked = static_cast<char>(source->peek_file());
                        if(peeked == '/') {
                            source->get_file();
                            source->change_pos();
                            return true;
                        }
                        else if(peeked == std::char_traits<char>::eof())
                            return false;   //Throw an error here
                    }
                }
                return false;   //Throw an error here
            }
            else {
                source->unget_file();
                return false;
            }
        }
        else
            return false;
    }

    bool Lexer::match_graphic(std::string& match) {
        std::string tmp{};

        switch(static_cast<char>(source->peek_file())) {
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
                tmp += static_cast<char>(source->get_file());
                match = tmp;
                return true;
            }
            case '-':
            case '+': {
                tmp += static_cast<char>(source->get_file());
                if(static_cast<char>(source->peek_file()) == tmp.back()) {
                    tmp += static_cast<char>(source->get_file());
                    match = tmp;
                    return true;
                }
                else
                    source->unget_file();
            }
            case '=':
            case '<':
            case '>':
            case '!':
            case '%':
            case '*':
            case '/': {
                tmp = static_cast<char>(source->get_file());
                if(static_cast<char>(source->peek_file()) == '=')
                    tmp += static_cast<char>(source->get_file());
                match = tmp;
                return true;
            }
            case '&':
            case '|': {
                tmp += static_cast<char>(source->get_file());
                if(static_cast<char>(source->peek_file()) == tmp.back()) {
                    tmp += static_cast<char>(source->get_file());
                    match = tmp;
                    return true;
                }
                else {
                    source->unget_file();
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
        char peeked = static_cast<char>(source->peek_file());
        if(peeked == '\n') {
            source->get_file();
            source->change_row();
            source->change_char(1);
            source->change_col(1);
            new_line = true;
            return true;
        }
        else if(peeked == ' ') {
            source->get_file();
            source->change_pos();
            return true;
        }
        else if(peeked == '\t') {
            source->get_file();
            source->change_char();
            int tab = TAB_LEN - (source->get_pos().col_num - 1 % TAB_LEN);
            source->change_col(source->get_pos().col_num + tab);
            return true;
        }
        else
            return false;
    }

};  //namespace Translator
