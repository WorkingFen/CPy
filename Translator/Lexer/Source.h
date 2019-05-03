#include <fstream>
#include <iostream>
#include <string>

namespace Lexer {
	struct TextPos {
		unsigned int rn, cn;               // Numer wiersza, znaku
		TextPos(int r = 0, int c = 0) : rn(r), cn(c) {}
	};

	class Source {
		std::fstream file;
		TextPos current_position;
		bool is_beginning;
		std::string line;
		bool new_line;

	public:
		Source(const std::string fileName);
		~Source();

		int prev_char();
		int next_char();
		int next_line();
		const TextPos& get_pos() const { return current_position; }
		bool is_new_line() const { return new_line; }
		void set_new_line(bool value) { new_line = value; }

	};
}