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

    Status Source::next_line() {
        // Error while reading from file
        if(file.fail()) {
            std::cerr << "IO error" << std::endl;
            exit(1);
        }
        else if(std::getline(file, line)) {
            curr_position.row_num++;

            curr_position.col_num = 1;
            curr_position.char_num = 1;

            return Status::OK;
        }
        else
            return Status::SEOF;
    }

};  //namespace Translator
