#ifndef SOURCE_H
#define SOURCE_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#define TAB_LEN 4

namespace Translator {
	struct TextPos {
		unsigned int row_num{ 0 };		// Numer wiersza, znaku czytanego, znaku pisanego
        unsigned int col_num{ 1 };
        unsigned int char_num{ 1 };
	};

    template<typename S>
	class Source {
        S upper_stream;
		std::istream& stream = upper_stream;
		TextPos curr_position;

	public:
        Source() = default;
		Source(const std::string streamName);
		~Source();

        template<typename T>
        T peek() { return static_cast<T>(stream.peek()); }
        template<typename T>
        T get() { return static_cast<T>(stream.get()); }
        void unget() { stream.unget(); }
        void seekg(std::streampos pos) { stream.seekg(pos); }
        std::streampos tellg() { return stream.tellg(); }
        void skipline() { stream.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); }

		const TextPos& get_pos() const { return curr_position; }
        void next_char() { ++curr_position.char_num; }
        void change_col(unsigned int value) { curr_position.col_num = value; }
        void change_char(unsigned int value) { curr_position.char_num = value; }
        void next_pos();
        void new_line_pos();
	};
}

#endif
