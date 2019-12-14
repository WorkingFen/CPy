#include "Source.h"

namespace Translator {
    Source::Source(const std::string fileName) {
        file.open(fileName);

        if(file.fail()) {
            std::cerr << "Couldn't open the file" << std::endl;
            exit(1);
        }
    }

    Source::~Source() {
        file.close();
    }

    void Source::next_pos() {
        ++curr_position.col_num;
        ++curr_position.char_num;
    }

    void Source::new_line_pos() {
        ++curr_position.row_num;
        curr_position.col_num = 1;
        curr_position.char_num = 1;
    }

};  //namespace Translator
