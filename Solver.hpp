#pragma once

#include <minisat/core/Solver.h>
#include <boost/functional/hash.hpp>
#include "BoolExpr.hpp"

#include <memory>
#include <vector>
#include <unordered_map>
#include <utility>

using std::shared_ptr;
using std::vector;
using std::unordered_map;
using std::pair;

using board = vector<vector<int>>;
using char_board = vector<vector<char>>;

class Solver {
private:
	Minisat::Solver solver;
	int num_vars = 0;
	int num_colors;
	int n;

public:
	Solver();
	Solver(int n, unordered_map<pair<int, int>, int, boost::hash<pair<int, int>>> endpoints);
	bool solve();
	board get_solution();
	void tseitin(shared_ptr<BoolExpr> b);
	Minisat::Lit makeVar();

private:
	void init_vars();
	Minisat::Var to_var(int r, int c, int color);
	void tseitin_helper(shared_ptr<BoolExpr> b, Minisat::Lit cur);
	void create_expression(unordered_map<pair<int, int>, int, boost::hash<pair<int, int>>> endpoints);
	void at_most_one_color(int r, int c);
	void exact_num_neighbors(int r, int c, int color);
	void at_least_one_working_neighbors(int r, int c);
	vector<pair<int, int>> get_neighbors(int r, int c);
	bool is_valid_space(int r, int c);
};