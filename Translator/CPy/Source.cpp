#include "Source.h"

namespace Translator {
    Source<std::ifstream>::Source(const std::string streamName) {
        upper_stream.open(streamName);

        if(stream.fail()) {
            std::cerr << "Couldn't open the stream" << std::endl;
            exit(1);
        }
    }

    Source<std::ifstream>::~Source() {
        upper_stream.close();
    }

    Source<std::istringstream>::Source(const std::string string) {
        upper_stream.str(string);
    }

    Source<std::istringstream>::~Source() {}

    template<typename S>
    void Source<S>::next_pos() {
        ++curr_position.col_num;
        ++curr_position.char_num;
    }

    template<typename S>
    void Source<S>::new_line_pos() {
        ++curr_position.row_num;
        curr_position.col_num = 1;
        curr_position.char_num = 1;
    }

    // If you want to use Source class with other parameters, you have to create line for it like below
    template class Source<std::ifstream>;
    template class Source<std::istringstream>;

};  //namespace Translator
