#ifndef SOURCE_H
#define SOURCE_H

#include <fstream>
#include <iostream>
#include <string>

#define TAB_LEN 4

namespace Translator {
	struct TextPos {
		unsigned int row_num{ 0 };		// Numer wiersza, znaku czytanego, znaku pisanego
        unsigned int col_num{ 1 };
        unsigned int char_num{ 1 };
	};

	class Source {
		std::ifstream file;
		TextPos curr_position;

	public:
		Source(const std::string fileName);
		~Source();

        template<typename T>
        T peek() { return static_cast<T>(file.peek()); }
        template<typename T>
        T get() { return static_cast<T>(file.get()); }
        void unget() { file.unget(); }
        void seekg(std::streampos pos) { file.seekg(pos); }
        std::streampos tellg() { return file.tellg(); }
        void skipline() { file.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); }

		const TextPos& get_pos() const { return curr_position; }
        void next_char() { ++curr_position.char_num; }
        void change_col(unsigned int value) { curr_position.col_num = value; }
        void change_char(unsigned int value) { curr_position.char_num = value; }
        void next_pos();
        void new_line_pos();
	};
}

#endif
