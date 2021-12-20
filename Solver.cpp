#include "Solver.hpp"
#include "BoolExpr.hpp"

#include <iostream>
#include <string>
#include <memory>
#include <queue>
#include <exception>
#include <cmath>

using std::string;
using std::make_shared;
using std::to_string;
using std::shared_ptr;
using std::cout;
using std::endl;

vector<vector<int>> combination(int n, int k);

void Solver::tseitin(shared_ptr<BoolExpr> b) {
	Minisat::Lit y_0 = makeVar();
	solver.addClause(y_0);
	tseitin_helper(b, y_0);
}

void Solver::tseitin_helper(shared_ptr<BoolExpr> b, Minisat::Lit cur) {
	if (!b->l || !b->r) {
		return;
	}
	else {
		Minisat::Lit y_1;
		if (!b->l->l || !b->l->r) {
			y_1 = b->l->val;
		}
		else {
			y_1 = makeVar();
			tseitin_helper(b->l, y_1);
		}

		Minisat::Lit y_2;
		if (!b->r->l || !b->r->r) {
			y_2 = b->r->val;
		}
		else {
			y_2 = makeVar();
			tseitin_helper(b->r, y_2);
		}

		if (b->op == "&") {
			solver.addClause(~cur, y_1);
			solver.addClause(~cur, y_2);
			solver.addClause(cur, ~y_1, ~y_2);
		}
		else if (b->op == "|") {
			solver.addClause(cur, ~y_1);
			solver.addClause(cur, ~y_2);
			solver.addClause(~cur, y_1, y_2);
		}
		else {
			throw(std::runtime_error(b->op + " is not a valid operator"));
		}
	}
}

Solver::Solver() {
	n = 0;
	num_colors = 0;
}

Solver::Solver(int n, unordered_map<pair<int, int>, int, boost::hash<pair<int, int>>> endpoints) : n(n) {
	num_colors = endpoints.size() / 2;
	init_vars();
	create_expression(endpoints);
}

void Solver::init_vars() {
	for (int color = 0; color < num_colors; color++) {
		for (int r = 0; r < n; r++) {
			for (int c = 0; c < n; c++) {
				makeVar();
			}
		}
	}
}

Minisat::Lit Solver::makeVar() {
	solver.newVar();
	return Minisat::mkLit(num_vars++);
}

bool Solver::solve() {
	return solver.solve();
}

Minisat::Var Solver::to_var(int r, int c, int color) {
	return c + r * n + color * pow(n, 2);
}

board Solver::get_solution() {
	board res = board();
	for (int r = 0; r < n; r++) {
		vector<int> row;
		for (int c = 0; c < n; c++) {
			int found = 0;
			for (int color = 0; color < num_colors; color++) {
				if (solver.modelValue(to_var(r, c, color)).isTrue()) {
					found++;
					row.push_back(color);
				}
			}
			assert(found == 1 && "Number of colors assigned to space not equal to 1");
		}
		res.push_back(row);
	}
	return res;
}


void Solver::create_expression(unordered_map<pair<int, int>, int, boost::hash<pair<int, int>>> endpoints) {
	for (int r = 0; r < n; r++) {
		for (int c = 0; c < n; c++) {
			pair<int, int> cur_point(r, c);
			auto color = endpoints.find(cur_point);
			if (color != endpoints.end()) {
				at_most_one_color(r, c);
				exact_num_neighbors(r, c, color->second);
			}
			else {
				at_most_one_color(r, c);
				at_least_one_working_neighbors(r, c);
			}
		}
	}
}

void Solver::at_most_one_color(int r, int c) {
	for (int i = 0; i < num_colors; i++) {
		for (int j = i + 1; j < num_colors; j++) {
			solver.addClause(~Minisat::mkLit(to_var(r, c, i)), ~Minisat::mkLit(to_var(r, c, j)));
		}
	}
}

void Solver::exact_num_neighbors(int r, int c, int color) {
	solver.addClause(Minisat::mkLit(to_var(r, c, color)));
	vector<pair<int, int>> neighbors = get_neighbors(r, c);
	Minisat::vec<Minisat::Lit> v;
	for (auto& neighbor : neighbors) {
		v.push(Minisat::mkLit(to_var(neighbor.first, neighbor.second, color)));
	}
	solver.addClause(v);
	for (auto& combo : combination(neighbors.size(), 2)) {
		pair<int, int> neighbor1 = neighbors[combo[0]];
		pair<int, int> neighbor2 = neighbors[combo[1]];
		solver.addClause(~Minisat::mkLit(to_var(neighbor1.first, neighbor1.second, color)), ~Minisat::mkLit(to_var(neighbor2.first, neighbor2.second, color)));
	}
}

void Solver::at_least_one_working_neighbors(int r, int c) {
	vector<pair<int, int>> neighbors = get_neighbors(r, c);
	vector<vector<int>> choose3 = combination(neighbors.size(), 3);
	vector<vector<int>> choose2 = combination(neighbors.size(), 2);

	shared_ptr<BoolExpr> res = nullptr;
	for (int color = 0; color < num_colors; color++) {
		shared_ptr<BoolExpr> b(lit(to_var(r, c, color)));
		for (auto& combo : choose3) {
			queue<shared_ptr<BoolExpr>> lits;
			for (int index : combo) {
				pair<int, int> neighbor = neighbors[index];
				lits.push(neg(lit(to_var(neighbor.first, neighbor.second, color))));
			}
			shared_ptr<BoolExpr> disjunction = combine(lits, "|");
			b = combine(b, disjunction, "&");
		}
		shared_ptr<BoolExpr> at_least_2 = nullptr;
		for (auto& combo : choose2) {
			queue<shared_ptr<BoolExpr>> lits;
			for (int index : combo) {
				pair<int, int> neighbor = neighbors[index];
				lits.push(lit(to_var(neighbor.first, neighbor.second, color)));
			}
			shared_ptr<BoolExpr> conjunction = combine(lits, "&");
			if (!at_least_2) {
				at_least_2 = conjunction;
			}
			else {
				at_least_2 = combine(at_least_2, conjunction, "|");
			}
		}
		b = combine(b, at_least_2, "&");
		if (!res) {
			res = b;
		}
		else {
			res = combine(res, b, "|");
		}
	}
	tseitin(res);
}

vector<pair<int, int>> Solver::get_neighbors(int r, int c) {
	vector<pair<int, int>> dirs = { {1,0},{-1,0},{0,1},{0,-1} };
	vector<pair<int, int>> res;
	for (auto dir : dirs) {
		if (is_valid_space(r + dir.first, c + dir.second)) {
			res.push_back(pair<int, int> {r + dir.first, c + dir.second});
		}
	}
	return res;
}

bool Solver::is_valid_space(int r, int c) {
	return !(r < 0 || c < 0 || r >= n || c >= n);
}

vector<vector<int>> combination(int n, int k) {
	vector<vector<int>> res;
	if (n < k) {
		return res;
	}
	vector<int> cur;
	for (int i = 0; i < k; i++) {
		cur.push_back(i);
	}
	res.push_back(vector<int>(cur));
	int cur_index = k - 1;
	while (true) {
		while (cur_index >= 0 && cur[cur_index] >= n + cur_index - k) {
			cur_index--;
		}
		if (cur_index < 0) {
			break;
		}
		cur[cur_index]++;
		for (int j = cur_index + 1; j < k; j++) {
			cur[j] = cur[cur_index] + j - cur_index;
		}
		cur_index = k - 1;
		res.push_back(vector<int>(cur));
	}
	return res;
}