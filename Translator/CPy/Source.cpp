#include "Source.h"

namespace Translator {
    Source::Source(const std::string fileName) {
        file.open(fileName, std::ios::in);

        if(file.fail()) {
            std::cerr << "Couldn't open the file" << std::endl;
            exit(1);
        }
    }

    Source::~Source() {
        file.close();
    }

    // Move to the previous char; operating on one line
    char Source::prev_char() {
        if(curr_position.char_num == 0)
            return -1;

        if(line[curr_position.char_num--] == '\t')
            curr_position.col_num -= ((curr_position.col_num - curr_position.char_num) % TAB_LEN);

        curr_position.col_num--;

        return 1;
    } 

    // Move to next char
    char Source::next_char() {
        if(line.length() <= curr_position.char_num) {
            do {
                if(next_line() == Status::SEOF) return EOF;
            } while(line.empty());
        }

        if(line[curr_position.char_num] == '\t')
            curr_position.col_num += TAB_LEN - (curr_position.col_num % TAB_LEN);
        else
            curr_position.col_num++;

        return line[curr_position.char_num++];
    }

    Status Source::next_line() {
        // Error while reading from file
        if(file.fail()) {
            std::cerr << "IO error" << std::endl;
            exit(1);
        }
        else if(std::getline(file, line)) {
            curr_position.row_num++;
            if(after_comment) after_comment = false;
            else new_line = true;

            curr_position.col_num = 0;
            curr_position.char_num = 0;

            return Status::OK;
        }
        else
            return Status::SEOF;
    }

};  //namespace Translator
