#ifndef SOURCE_H
#define SOURCE_H

#include <fstream>
#include <iostream>
#include <string>

namespace Translator {
	struct TextPos {
		unsigned int rn, cn, sn;               // Numer wiersza, znaku pisanego, znaku czytanego
		TextPos(int r = 0, int c = 0, int s = 0) : rn(r), cn(c), sn(s) {}
	};

	class Source {
		std::fstream file;
		TextPos current_position;
		bool is_beginning;
		std::string line;
		bool new_line;
		bool after_comment;

	public:
		Source(const std::string fileName);
		~Source();

		int prev_char();
		int next_char();
		int next_line();
		const TextPos& get_pos() const { return current_position; }
		bool is_new_line() const { return new_line; }
		void set_new_line(bool value) { new_line = value; }
		void set_after_comment(bool value) { after_comment = value; }

	};
}

#endif