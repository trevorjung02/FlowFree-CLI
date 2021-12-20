#include <minisat/core/Solver.h>
#include "BoolExpr.hpp"
#include "Solver.hpp"
#include <cctype>
#include <fstream>

using std::cout;
using std::endl;
using std::shared_ptr;
using std::make_shared;
using std::to_string;
using std::cerr;
using std::ifstream;

void print_char_board(char_board b);
char_board board_to_char_board(board b);
unordered_map<pair<int, int>, int, boost::hash<pair<int, int>>> endpoints_from_board(board b);
board read_board(std::istream& in);
void usage();

int main(int argc, char** argv) {
	if (argc > 2) {
		cerr << "Error: number of arguments greater than 1" << endl;
		usage();
		return 1;
	}
	board b;
	if (argc == 2) {
		char* file = argv[1];
		ifstream f(file);
		if (f.is_open()) {
			try {
				b = read_board(f);
			}
			catch (const std::runtime_error& e) {
				cerr << e.what() << endl;
				return 1;
			}
		}
		else {
			cerr << "Error: could not open input file" << endl;
			usage();
			return 1;
		}
	}
	else {
		cout << "Input board below:" << endl;
		try {
			b = read_board(std::cin);
		}
		catch (const std::runtime_error& e) {
			cerr << e.what() << endl;
			return 1;
		}
	}
	unordered_map<pair<int, int>, int, boost::hash<pair<int, int>>> endpoints = endpoints_from_board(b);
	int n = b.size();

	Solver s(n, endpoints);
	if (s.solve()) {
		cout << "Solved!" << endl;
		print_char_board(board_to_char_board(s.get_solution()));
	}
	else {
		cout << "Board is not solvable" << endl;
	}
	cout << endl << "Press any key to close the program . . ." << endl;
	getchar();
}

void usage() {
	cout << "Usage: ./flowfree-cli <inputfile.txt>" << endl;
}

board read_board(std::istream& in) {
	board parsed;
	std::string line;
	int num_lines = 1;
	while (std::getline(in, line)) {
		if (line.size() == 0) {
			break;
		}
		vector<int> row;
		for (size_t ci = 0; ci < line.size(); ++ci) {
			char c = line[ci];
			if (c == '.') {
				row.push_back(-1);
			}
			else {
				if (!isalpha(c)) {
					throw std::runtime_error("Line #" + std::to_string(num_lines) + " contains invalid character: " + c);
				}
				c = tolower(c);
				row.push_back(c - 'a');
			}
		}
		parsed.push_back(row);
		num_lines++;
	}
	return parsed;
}

void print_char_board(char_board b) {
	int n = b.size();
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			cout << b[i][j] << " ";
		}
		cout << endl;
	}
}

unordered_map<pair<int, int>, int, boost::hash<pair<int, int>>> endpoints_from_board(board b) {
	unordered_map<pair<int, int>, int, boost::hash<pair<int, int>>> res;
	for (int r = 0; r < b.size(); r++) {
		for (int c = 0; c < b[0].size(); c++) {
			if (b[r][c] >= 0) {
				res[pair<int, int>(r, c)] = b[r][c];
			}
		}
	}
	return res;
}

char_board board_to_char_board(board b) {
	char_board res;
	for (int r = 0; r < b.size(); r++) {
		vector<char> row;
		for (int c = 0; c < b[0].size(); c++) {
			row.push_back('a' + b[r][c]);
		}
		res.push_back(row);
	}
	return res;
}