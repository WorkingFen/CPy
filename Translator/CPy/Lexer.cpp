#include "Lexer.h"
#include <cctype>
#include <stdexcept>
#include <iostream>

namespace Translator {
    Lexer::Lexer(Source* s): source(s) {
        source->next_line();
    }

    void Lexer::get_next_token() {
        bool gotit = false;

        std::regex word("(.*?)(\\s|$)");
        std::regex str("(L?\".*\")(.*)");                    // TODO: Ogarn¹æ, jakie znaki mog¹ wystêpowaæ, albo jakie nie mog¹, zale¿y co ³atwiejsze
        std::regex con("(L?'.*')(.*)");
        std::regex hex("(0[xX][[:xdigit:]]+)(.*)");
        std::regex dbl("(\\d+\\.\\d*|\\d*\\.\\d+)(.*)");
        std::regex oct("(0[0-7]+)(.*)");
        std::regex dec("(0|\\d+)(.*)");
        std::regex literals("([^\\W\\d]\\w*)(.*)");
        std::regex line_comm("\\/\\/.*");
        std::regex block_comm("\\/\\*.*");
        std::regex end_block("(.*\\*\\/)(.*)");
        std::regex graphic("([()[\\]{},;.:?]|[-+&|]{2}|[-+=<>!%*\\/]=?)(.*)");

        std::smatch sm;         // string match
        std::smatch wm;         // word match
        std::string line;

        while(std::regex_search((line = source->get_line()), sm, word)) {
            if(sm[1].str() == "") {                                                             // Get EOF token
                if(sm[2].str() == "") {
                    token.set_type(Type::eof);
                    break;
                }
            }
            else if(std::regex_match((line = sm[1].str()), wm, str)) {                          // Get string literal token
                token.set_type(Type::STRING_LITERAL);
            }
            else if(std::regex_match(line, wm, con) || std::regex_match(line, wm, hex) ||       // Get constant token, either it's a char, hexadecimal,
                std::regex_match(line, wm, dbl) || std::regex_match(line, wm, oct) ||           // floating point, octal or decimal
                std::regex_match(line, wm, dec)) {
                token.set_type(Type::CONSTANT);
            }
            else if(std::regex_match(line, wm, literals)) {                                     // Get identifier token or special literal token (if, else, etc.)
                auto tmp = map.find(wm[1].str());
                if(tmp != map.end())
                    token.set_type(tmp->second);
                else
                    token.set_type(Type::IDENTIFIER);
            }
            else if(std::regex_match(line, wm, line_comm)) {                                    // Skip line comments
                source->next_line();
                continue;
            }
            else if(std::regex_match(line, wm, block_comm)) {                                   // Skip block comments
                line = sm.suffix();
                while(!std::regex_search(line, wm, end_block)) {
                    source->next_line();
                    line = source->get_line();
                }
                source->change_char(source->get_pos().char_num + wm[1].str().size());
                source->change_col(source->get_pos().col_num + wm[1].str().size());
                source->set_line(wm[2].str());
                continue;
            }
            else if(std::regex_match(line, wm, graphic)) {                                      // Get other token or special other token (==, <=, etc.)
                auto tmp = map.find(wm[1].str());
                if(tmp != map.end())
                    token.set_type(tmp->second);
                else
                    token.set_type(Type::other);
            }
            else
                token.set_type(Type::none);                                                     // Unknown token

            token.set_chars(wm[1].str());
            source->change_char(source->get_pos().char_num + wm[1].str().size());
            source->change_col(source->get_pos().col_num + wm[1].str().size());
            if(wm[2].str() != "") {
                source->set_line(wm[2].str() + sm[2].str() + std::string(sm.suffix()));
                break;
            }
            else {
                source->set_line(sm.suffix());
                gotit = true;
            }

            if(sm[2].str() == "")
                source->set_line("");
            else if(sm[2].str() == "\n")
                source->next_line();
            else if(sm[2].str() == " ") {
                source->change_char(source->get_pos().char_num + 1);
                source->change_col(source->get_pos().col_num + 1);
            }
            else if(sm[2].str() == "\t") {
                source->change_char(source->get_pos().char_num + 1);
                int tab = TAB_LEN - (source->get_pos().col_num - 1 % TAB_LEN);
                source->change_col(source->get_pos().col_num + tab);
            }

            if(gotit)
                break;
        }
																
    }

//    Token Lexer::get_other_token() {
//        std::regex p("[!%&*+-/<=>|]");
//
//        Token tmp;
//        std::string x = "";
//        std::string text = "";
//
//        text = current_char;
//
//        if(std::regex_match(text, p)) {
//            do {
//#ifdef _DEBUG
//                std::cout << current_char;
//#endif
//                x += current_char;
//                current_char = source->next_char();
//                if(source->is_new_line() || isspace(current_char)) break;
//                text = current_char;
//            } while(std::regex_match(text, p));
//        }
//        else {
//#ifdef _DEBUG
//            std::cout << current_char;
//#endif
//            if(current_char == '\\') current_char = source->next_char();
//            // Error gdy nic nie ma po "\" (Ogarn¹æ)
//            x += current_char;
//            current_char = source->next_char();
//        }
//
//        if(current_char != EOF) source->prev_char();
//
//        tmp.set_chars(x);
//
//        auto pair = map.find(x);
//        if(pair != map.end()) {
//            tmp.set_type(pair->second);
//            return tmp;
//        }
//
//        tmp.set_type(Type::other);
//        return tmp;
//    }

};  //namespace Translator
