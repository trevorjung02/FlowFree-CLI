#include <minisat/core/Solver.h>
#include "BoolExpr.hpp"
#include "Solver.hpp"
#include <chrono>
#include <cctype>
#include <fstream>

using std::cout;
using std::endl;
using std::shared_ptr;
using std::make_shared;
using std::to_string;
using std::cerr;
using std::ifstream;

void print_board(board b);
void print_char_board(char_board b);
void test1();
void test2();
void test3();
void test_classic_9x9_1();
void test_classic_5x5_1();
void test_jumbo_14x14_1();
void test_tseitin();
char_board board_to_char(board b);
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

	auto t1 = std::chrono::high_resolution_clock::now();
	Solver s(n, endpoints);
	if (s.solve()) {
		std::chrono::duration<double, std::milli> time_taken = std::chrono::high_resolution_clock::now() - t1;
		cout << "Solved in " << time_taken.count() << "ms." << endl;
		print_char_board(board_to_char(s.get_solution()));
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

void test1() {
	int n = 2;
	unordered_map<pair<int, int>, int, boost::hash<pair<int, int>>> endpoints;
	endpoints[pair<int, int>{0, 0}] = 0;
	endpoints[pair<int, int>{0, 1}] = 0;
	endpoints[pair<int, int>{1, 0}] = 1;
	endpoints[pair<int, int>{1, 1}] = 1;

	Solver s(n, endpoints);
	cout << "s.solve() = " << s.solve() << endl;
	board res = s.get_solution();
	print_char_board(board_to_char(res));
}

void test2() {
	int n = 3;
	unordered_map<pair<int, int>, int, boost::hash<pair<int, int>>> endpoints;
	endpoints[pair<int, int>{0, 0}] = 0;
	endpoints[pair<int, int>{0, 2}] = 0;
	endpoints[pair<int, int>{1, 0}] = 1;
	endpoints[pair<int, int>{1, 2}] = 1;
	endpoints[pair<int, int>{2, 0}] = 2;
	endpoints[pair<int, int>{2, 2}] = 2;

	Solver s(n, endpoints);
	bool solved = s.solve();
	cout << "s.solve() = " << solved << endl;
	if (solved) {
		board res = s.get_solution();
		print_board(res);
	}

	// 0 0 0 
	// 1 1 1 
	// 2 2 2 
}

void test3() {
	int n = 4;
	unordered_map<pair<int, int>, int, boost::hash<pair<int, int>>> endpoints;
	endpoints[pair<int, int>{0, 0}] = 0;
	endpoints[pair<int, int>{0, 1}] = 0;
	endpoints[pair<int, int>{1, 0}] = 1;
	endpoints[pair<int, int>{1, 1}] = 1;
	endpoints[pair<int, int>{2, 0}] = 2;
	endpoints[pair<int, int>{2, 1}] = 2;
	endpoints[pair<int, int>{3, 0}] = 3;
	endpoints[pair<int, int>{3, 1}] = 3;

	endpoints[pair<int, int>{0, 2}] = 4;
	endpoints[pair<int, int>{0, 3}] = 4;
	endpoints[pair<int, int>{1, 2}] = 5;
	endpoints[pair<int, int>{1, 3}] = 5;
	endpoints[pair<int, int>{2, 2}] = 6;
	endpoints[pair<int, int>{2, 3}] = 6;
	endpoints[pair<int, int>{3, 2}] = 7;
	endpoints[pair<int, int>{3, 3}] = 7;

	// 0 0 4 4
	// 1 1 5 5
	// 2 2 6 6
	// 3 3 7 7

	Solver s(n, endpoints);
	bool solved = s.solve();
	cout << "s.solve() = " << solved << endl;
	if (solved) {
		board res = s.get_solution();
		print_board(res);
	}
}

void test_tseitin() {
	Solver s;
	shared_ptr<BoolExpr> a = lit(s.makeVar());
	shared_ptr<BoolExpr> b = lit(s.makeVar());
	shared_ptr<BoolExpr> c = lit(s.makeVar());
	shared_ptr<BoolExpr> phi = combine(a, combine(b, c, "|"), "&");
	s.tseitin(phi);
	cout << "solvable = " << s.solve() << endl;
	s.print_model_vals();
}


void print_board(board b) {
	int n = b.size();
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			cout << b[i][j] << " ";
		}
		cout << endl;
	}
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

void test_classic_5x5_1() {
	int a = 0;
	int b = 1;
	int c = 2;
	int d = 3;
	int e = 4;

	board input_board{
		{ a,-1, b,-1, d},
		{-1,-1, c,-1, e},
		{-1,-1,-1,-1,-1},
		{-1, b,-1, d,-1},
		{-1, a, c, e,-1},
	};
	int n = input_board.size();

	unordered_map<pair<int, int>, int, boost::hash<pair<int, int>>> endpoints = endpoints_from_board(input_board);
	Solver s(n, endpoints);
	bool solved = s.solve();
	cout << "s.solve() = " << solved << endl;
	if (solved) {
		board res = s.get_solution();
		print_char_board(board_to_char(res));
	}
}

void test_classic_9x9_1() {
	int a = 0;
	int b = 1;
	int c = 2;
	int d = 3;
	int e = 4;
	int f = 5;
	int g = 6;
	int h = 7;
	int i = 8;

	board input_board{
		{-1,-1,-1,-1,-1,-1,-1,-1,-1},
		{-1, a, f, c,-1,-1,-1,-1,-1},
		{-1,-1,-1, f, e,-1, e, c,-1},
		{-1,-1,-1,-1,-1,-1,-1, g,-1},
		{-1, b,-1, b, a,-1,-1,-1,-1},
		{ d, g,-1,-1,-1,-1,-1, h, i},
		{-1, i, d,-1,-1,-1,-1,-1,-1},
		{-1, h,-1,-1,-1,-1,-1,-1,-1},
		{-1,-1,-1,-1,-1,-1,-1,-1,-1},
	};
	int n = input_board.size();

	unordered_map<pair<int, int>, int, boost::hash<pair<int, int>>> endpoints = endpoints_from_board(input_board);
	Solver s(n, endpoints);
	bool solved = s.solve();
	if (solved) {
		cout << "Solved! See solution below" << endl;
		board res = s.get_solution();
		print_char_board(board_to_char(res));
	}
	else {
		cout << "Could not solve" << endl;
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

char_board board_to_char(board b) {
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

void test_jumbo_14x14_1() {
	int a = 0;
	int b = 1;
	int c = 2;
	int d = 3;
	int e = 4;
	int f = 5;
	int g = 6;
	int h = 7;
	int i = 8;
	int j = 9;
	int k = 10;
	int l = 11;
	int m = 12;
	int n = 13;
	int o = 14;

	board input_board{
		{-1,-1,-1,-1, g,-1,-1,-1,-1,-1,-1,-1,-1,-1},
		{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
		{-1,-1,-1,-1,-1,-1,-1, o,-1,-1,-1,-1,-1,-1},
		{-1,-1, h,-1,-1, c, f, k,-1,-1, i,-1,-1,-1},
		{-1,-1,-1, c, f,-1,-1,-1,-1,-1,-1,-1,-1,-1},
		{-1,-1,-1,-1,-1,-1,-1,-1, o,-1,-1,-1,-1,-1},
		{ e,-1, b,-1,-1,-1,-1,-1, b,-1,-1,-1,-1,-1},
		{-1,-1,-1, l,-1,-1,-1,-1, l,-1,-1,-1,-1,-1},
		{-1,-1,-1,-1,-1,-1,-1, g,-1,-1,-1,-1,-1,-1},
		{ a,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
		{-1,-1,-1, m,-1,-1, a,-1, i,-1, d,-1,-1,-1},
		{-1,-1,-1,-1, n,-1, d,-1, k,-1, h,-1,-1,-1},
		{-1,-1, e,-1,-1,-1,-1,-1, m,-1,-1,-1,-1,-1},
		{-1,-1,-1,-1,-1,-1,-1,-1, n, j,-1,-1,-1, j},
	};
	int board_size = input_board.size();

	unordered_map<pair<int, int>, int, boost::hash<pair<int, int>>> endpoints = endpoints_from_board(input_board);

	auto t1 = std::chrono::high_resolution_clock::now();
	Solver s(board_size, endpoints);
	bool solved = s.solve();
	std::chrono::duration<double, std::milli> time_taken = std::chrono::high_resolution_clock::now() - t1;

	if (solved) {
		cout << "Built+Solved in " << time_taken.count() << "ms! See solution below" << endl;
		board res = s.get_solution();
		print_char_board(board_to_char(res));
	}
	else {
		cout << "Could not solve" << endl;
	}
}