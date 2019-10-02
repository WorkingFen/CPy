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
        unsigned int col_num{ 0 };
        unsigned int char_num{ 0 };
	};

	class Source {
		std::fstream file;
		TextPos curr_position;
        std::string line{ "" };
		bool new_line{ false };
		bool after_comment{ false };

	public:
		Source(const std::string fileName);
		~Source();

		char prev_char();
		char next_char();
		Status next_line();

		const TextPos& get_pos() const { return curr_position; }
		bool is_new_line() const { return new_line; }
		void set_new_line(bool value) { new_line = value; }
		void set_after_comment(bool value) { after_comment = value; }

	};
}

#endif
