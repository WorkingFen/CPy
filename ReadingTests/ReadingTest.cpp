#include "pch.h"
#include "ReadingTest.h"

bool ReadingTest::match_str(const std::string& line, std::string& match) {
    std::string tmp{};
    char peeked;
    auto main_func = [&]() {
        tmp += static_cast<char>(get_file(line));
        while((peeked = static_cast<char>(peek_file(line))) != std::char_traits<char>::eof()) {
            tmp += static_cast<char>(get_file(line));
            if(peeked == '"' && !odd_escaping(tmp, tmp.size() - 2)) {
                match = tmp;
                return true;
            }
        }
        return false;   //Throw an error here; And more exceptions
    };

    if((peeked = static_cast<char>(peek_file(line))) == 'L') {
        tmp += static_cast<char>(get_file(line));
        if(static_cast<char>(peek_file(line)) == '"')
            return main_func();
        else {
            unget_file();
            return false;
        }
    }
    else if(peeked == '"')
        return main_func();
    else
        return false;
}

bool ReadingTest::match_con(const std::string& line, std::string& match) {
    std::string tmp{};
    char peeked;
    auto main_func = [&]() {
        tmp += static_cast<char>(get_file(line));
        for(int i = 0; i < 3 && (peeked = static_cast<char>(peek_file(line))) != std::char_traits<char>::eof(); ++i) {
            tmp += static_cast<char>(get_file(line));
            if(peeked == '\'' && !odd_escaping(tmp, tmp.size() - 2)) {
                match = tmp;
                return true;
            }
        }
        return false;   //Throw an error here; And more exceptions
    };

    if((peeked = static_cast<char>(peek_file(line))) == 'L') {
        tmp += static_cast<char>(get_file(line));
        if(static_cast<char>(peek_file(line)) == '\'')
            return main_func();
        else {
            unget_file();
            return false;
        }
    }
    else if(peeked == '\'')
        return main_func();
    else
        return false;
}

bool ReadingTest::match_hex(const std::string& line, std::string& match) {
    std::string tmp{};
    char peeked;

    if(static_cast<char>(peek_file(line)) == '0') {
        tmp += static_cast<char>(get_file(line));
        if((peeked = static_cast<char>(peek_file(line))) == 'x' || peeked == 'X') {
            tmp += static_cast<char>(get_file(line));
            while(isxdigit(peek_file(line)))
                tmp += static_cast<char>(get_file(line));
            match = tmp;
            return true;
        }
        else {
            unget_file();
            return false;
        }
    }
    else
        return false;
}

bool ReadingTest::match_dbl(const std::string& line, std::string& match) {
    std::string tmp{};
    int starting_position = tellg_file();

    if(static_cast<char>(peek_file(line)) == '.') {
        tmp += static_cast<char>(get_file(line));
        if(isdigit(peek_file(line))) {
            tmp += static_cast<char>(get_file(line));
            while(isdigit(peek_file(line)))
                tmp += static_cast<char>(get_file(line));
            match = tmp;
            return true;
        }
        else {
            unget_file();
            return false;
        }
    }
    else if(isdigit(peek_file(line))) {
        tmp += static_cast<char>(get_file(line));
        while(isdigit(peek_file(line)))
            tmp += static_cast<char>(get_file(line));
        if(static_cast<char>(peek_file(line)) == '.') {
            tmp += static_cast<char>(get_file(line));
            while(isdigit(peek_file(line)))
                tmp += static_cast<char>(get_file(line));
            match = tmp;
            return true;
        }
        else {
            seekg_file(starting_position);
            return false;
        }
    }
    else
        return false;
}

bool ReadingTest::match_oct(const std::string& line, std::string& match) {
    std::string tmp{};
    char peeked;

    if(static_cast<char>(peek_file(line)) == '0') {
        tmp += static_cast<char>(get_file(line));
        if((peeked = static_cast<char>(peek_file(line))) >= '0' && peeked <= '7') {
            tmp += static_cast<char>(get_file(line));
            while((peeked = static_cast<char>(peek_file(line))) >= '0' && peeked <= '7')
                tmp += static_cast<char>(get_file(line));
            match = tmp;
            return true;
        }
        else {
            unget_file();
            return false;
        }
    }
    else
        return false;
}

bool ReadingTest::match_dec(const std::string& line, std::string& match) {
    std::string tmp{};
    char peeked;

    if(static_cast<char>(peek_file(line)) == '0') {
        tmp += static_cast<char>(get_file(line));
        if(isdigit(peek_file(line))) {
            unget_file();
            return false;
        }
        else {
            match = tmp;
            return true;
        }
    }
    else if((peeked = static_cast<char>(peek_file(line))) >= '1' && peeked <= '9') {
        tmp += static_cast<char>(get_file(line));
        while(isdigit(peek_file(line)))
            tmp += static_cast<char>(get_file(line));
        match = tmp;
        return true;
    }
    else
        return false;
}

bool ReadingTest::match_literals(const std::string& line, std::string& match) {
    std::string tmp{};

    if(isword(peek_file(line)) && !isdigit(peek_file(line))) {
        tmp += static_cast<char>(get_file(line));
        while(isword(peek_file(line)))
            tmp += static_cast<char>(get_file(line));
        match = tmp;
        return true;
    }
    else
        return false;
}

bool ReadingTest::match_line_comm(const std::string& line) {
    if(static_cast<char>(peek_file(line)) == '/') {
        get_file(line);
        if(static_cast<char>(peek_file(line)) == '/') {
            get_file(line);
            return true;
        }
        else {
            unget_file();
            return false;
        }
    }
    else
        return false;
}

bool ReadingTest::match_block_comm(const std::string& line) {
    if(static_cast<char>(peek_file(line)) == '/') {
        get_file(line);
        if(static_cast<char>(peek_file(line)) == '*') {
            while(static_cast<char>(get_file(line)) != std::char_traits<char>::eof()) {
                /*if(static_cast<char>(peek_file(line)) == '\n')
                    Ustawiamy odpowiednio rows*/
                if(static_cast<char>(peek_file(line)) == '*') {
                    get_file(line);
                    char peeked = static_cast<char>(peek_file(line));
                    if(peeked == '/') {
                        get_file(line);
                        return true;
                    }
                    else if(peeked == std::char_traits<char>::eof())
                        return false;   //Throw an error here
                }
            }
            return false;   //Throw an error here
        }
        else {
            unget_file();
            return false;
        }
    }
    else
        return false;
}

bool ReadingTest::match_graphic(const std::string& line, std::string& match) {
    std::string tmp{};

    switch(static_cast<char>(peek_file(line))) {
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
        tmp += static_cast<char>(get_file(line));
        match = tmp;
        return true;
    }
    case '-':
    case '+': {
        tmp += static_cast<char>(get_file(line));
        if(static_cast<char>(peek_file(line)) == tmp.back()) {
            tmp += static_cast<char>(get_file(line));
            match = tmp;
            return true;
        }
        else
            unget_file();
    }
    case '=':
    case '<':
    case '>':
    case '!':
    case '%':
    case '*':
    case '/': {
        tmp = static_cast<char>(get_file(line));
        if(static_cast<char>(peek_file(line)) == '=')
            tmp += static_cast<char>(get_file(line));
        match = tmp;
        return true;
    }
    case '&':
    case '|': {
        tmp += static_cast<char>(get_file(line));
        if(static_cast<char>(peek_file(line)) == tmp.back()) {
            tmp += static_cast<char>(get_file(line));
            match = tmp;
            return true;
        }
        else {
            unget_file();
            return false;
        }
    }
    default:
        return false;
    }
}

bool ReadingTest::odd_escaping(std::string& str, size_t start_pos) {
    int no_escaping{0};
    for(int i = 0; str.at(start_pos - i) == '\\'; ++i, ++no_escaping);
    return (no_escaping % 2);
}
