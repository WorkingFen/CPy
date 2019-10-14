#ifndef SOURCE_H
#define SOURCE_H

#include <fstream>
#include <iostream>
#include <string>

#define TAB_LEN 4

namespace Translator {
    enum class Status {
        SEOF,
        OK
    };

	struct TextPos {
		unsigned int row_num{ 0 };		// Numer wiersza, znaku czytanego, znaku pisanego
        unsigned int col_num{ 1 };
        unsigned int char_num{ 1 };
	};

	class Source {
		std::fstream file;
		TextPos curr_position;
        std::string line{ "" };

	public:
		Source(const std::string fileName);
		~Source();

		Status next_line();

		const TextPos& get_pos() const { return curr_position; }
        void change_row(unsigned int value) { curr_position.row_num = value; }
        void change_col(unsigned int value) { curr_position.col_num = value; }
        void change_char(unsigned int value) { curr_position.char_num = value; }
        const std::string& get_line() const { return line; }
        void set_line(std::string value) { line = value; }

	};
}

#endif
