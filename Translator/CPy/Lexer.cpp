#include "Lexer.h"
#include <cctype>
#include <stdexcept>
#include <iostream>

/*** TODO ***/
/* Ogarn¹æ, jakie znaki mog¹ wystêpowaæ, albo jakie nie mog¹ (zale¿y od tego co ³atwiejsze) w regexach jako nastêpniki
 * Dorobiæ obs³ugê b³êdów takich jak z³e nastêpniki
 * Przygotowaæ program na mo¿liwoœæ wyst¹pienia z³oœliwego przypadku (linii która sk³ada siê z miliona znaków)
 * Obs³uga sytuacji, gdy brakuje zakoñczenia bloku komentarza
 */

namespace Translator {
    Lexer::Lexer(Source* s): source(s) {
        source->next_line();
    }

    void Lexer::get_next_token() {
        bool gotit = false;

        std::regex word("(.*?)(\\s|$)");
        std::regex str("(L?\".*\")(.*)");
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
        std::string line;

        while(!gotit && std::regex_search((line = source->get_line()), sm, word)) {
            std::string match(sm[1].str());
            std::string whitespace(sm[2].str());
            std::string suffix(sm.suffix().str());
            if(match == "") {                                                                   // Get EOF token
                if(whitespace == "") {
                    token.set_type(Type::eof);
                    break;
                }
            }
            else if(std::regex_match((line = match), sm, str)) {                                // Get string literal token
                token.set_type(Type::STRING_LITERAL);
                gotit = true;
            }
            else if(std::regex_match(line, sm, con) || std::regex_match(line, sm, hex) ||       // Get constant token, either it's a char, hexadecimal,
                std::regex_match(line, sm, dbl) || std::regex_match(line, sm, oct) ||           // floating point, octal or decimal
                std::regex_match(line, sm, dec)) {
                token.set_type(Type::CONSTANT);
                gotit = true;
            }
            else if(std::regex_match(line, sm, literals)) {                                     // Get identifier token or special literal token (if, else, etc.)
                auto tmp = map.find(sm[1].str());
                if(tmp != map.end())
                    token.set_type(tmp->second);
                else
                    token.set_type(Type::IDENTIFIER);
                gotit = true;
            }
            else if(std::regex_match(line, sm, line_comm)) {                                    // Skip line comments
                if(source->next_line() == Status::SEOF)
                    source->set_line("");
                continue;
            }
            else if(std::regex_match(line, sm, block_comm)) {                                   // Skip block comments
                line = suffix;
                while(!std::regex_search(line, sm, end_block)) {
                    source->next_line();
                    line = source->get_line();
                }
                unsigned int size = static_cast<unsigned int>(sm[1].str().size());
                source->change_char(source->get_pos().char_num + size);
                source->change_col(source->get_pos().col_num + size);
                source->set_line(sm[2].str());
                continue;
            }
            else if(std::regex_match(line, sm, graphic)) {                                      // Get other token or special other token (==, <=, etc.)
                auto tmp = map.find(sm[1].str());
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
                token.set_chars(sm[1].str());
                unsigned int size = static_cast<unsigned int>(sm[1].str().size());
                source->change_char(source->get_pos().char_num + size);
                source->change_col(source->get_pos().col_num + size);
                if(sm[2].str() != "") {
                    source->set_line(sm[2].str() + whitespace + suffix);
                    break;
                }
                else
                    source->set_line(suffix);
            }
            else
                source->set_line(suffix);

            if(whitespace == "\n") {
                if(source->next_line() == Status::SEOF) 
                    source->set_line("");
                new_line = true;
            }
            else if(whitespace == " ") {
                source->change_char(source->get_pos().char_num + 1);
                source->change_col(source->get_pos().col_num + 1);
            }
            else if(whitespace == "\t") {
                source->change_char(source->get_pos().char_num + 1);
                int tab = TAB_LEN - (source->get_pos().col_num - 1 % TAB_LEN);
                source->change_col(source->get_pos().col_num + tab);
            }
        }														
    }

};  //namespace Translator
