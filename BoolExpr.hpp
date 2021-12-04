#pragma once

#include <minisat/core/Solver.h>

#include <map>
#include <string>
#include <iostream>
#include <memory>
#include <queue>

using std::map;
using std::string;
using std::ostream;
using std::shared_ptr;
using std::queue;

struct BoolExpr {
public:
	Minisat::Lit val;
	shared_ptr<BoolExpr> l = nullptr;
	shared_ptr<BoolExpr> r = nullptr;
	string op;
	bool parens = false;

	BoolExpr(Minisat::Lit val) {
		this->val = val;
	}

	BoolExpr(int val) {
		this->val = Minisat::mkLit(val);
	}

	BoolExpr(shared_ptr<BoolExpr> l, shared_ptr<BoolExpr> r, string op) {
		this->l = l;
		this->r = r;
		this->op = op;
	}
};

ostream& operator<<(ostream& os, const BoolExpr& b);
shared_ptr<BoolExpr> combine(queue<shared_ptr<BoolExpr>> q, string op);
shared_ptr<BoolExpr> combine(shared_ptr<BoolExpr> l, shared_ptr<BoolExpr> r, string op);
shared_ptr<BoolExpr> lit(Minisat::Lit val);
shared_ptr<BoolExpr> lit(int val);
const shared_ptr<BoolExpr>& parens(const shared_ptr<BoolExpr>& b);

// Only use on literal
shared_ptr<BoolExpr> neg(const shared_ptr<BoolExpr>& b);