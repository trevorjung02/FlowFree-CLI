#include "BoolExpr.hpp"
#include <memory>

using std::make_shared;

ostream& operator<<(ostream& os, const BoolExpr& b) {
	if (b.l && b.r) {
		if (b.parens) {
			return os << "(" << *b.l << " " << b.op << " " << *b.r << ")";
		}
		return os << *b.l << " " << b.op << " " << *b.r;
	}
	else {
		return os << b.val.x;
	}
}

shared_ptr<BoolExpr> combine(queue<shared_ptr<BoolExpr>> q, string op) {
	if (q.size() == 0) {
		return nullptr;
	}
	shared_ptr<BoolExpr> res = q.front();
	q.pop();
	while (q.size() > 0) {
		res = combine(res, q.front(), op);
		q.pop();
	}
	return res;
}

shared_ptr<BoolExpr> combine(shared_ptr<BoolExpr> l, shared_ptr<BoolExpr> r, string op) {
	return make_shared<BoolExpr>(BoolExpr(l, r, op));
}

shared_ptr<BoolExpr> lit(Minisat::Lit val) {
	return make_shared<BoolExpr>(BoolExpr(val));
}

shared_ptr<BoolExpr> lit(int val) {
	return make_shared<BoolExpr>(BoolExpr(val));
}

const shared_ptr<BoolExpr>& parens(const shared_ptr<BoolExpr>& b) {
	b->parens = true;
	return b;
}

shared_ptr<BoolExpr> neg(const shared_ptr<BoolExpr>& b) {
	return lit(~b->val);
}