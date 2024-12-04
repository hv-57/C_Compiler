#include "function.h"

using std::make_pair;

pair<vector<TAC_Statement*>, shared_ptr<TAC_Operand>> AST_VarExpression::gen_tac_expr() {
	return make_pair(vector<TAC_Statement*>{}, shared_ptr<TAC_Operand>(new TAC_VarOperand(var_name, d_type)));
}

pair<vector<TAC_Statement*>, shared_ptr<TAC_Operand>> AST_IntExpression::gen_tac_expr() {
	return make_pair(vector<TAC_Statement*>{}, shared_ptr<TAC_Operand>(new TAC_IntOperand(val)));
}

pair<vector<TAC_Statement*>, shared_ptr<TAC_Operand>> AST_FloatExpression::gen_tac_expr() {
	return make_pair(vector<TAC_Statement*>{}, shared_ptr<TAC_Operand>(new TAC_FloatOperand(val)));
}

pair<vector<TAC_Statement*>, shared_ptr<TAC_Operand>> AST_StringExpression::gen_tac_expr() {
	return make_pair(vector<TAC_Statement*>{}, shared_ptr<TAC_Operand>(new TAC_StringOperand(val)));
}

pair<vector<TAC_Statement*>, shared_ptr<TAC_Operand>> AST_UnaryExpression::gen_tac_expr() {
	vector<TAC_Statement*> tac_code;

	auto [expr_code, expr_name] = expr -> gen_tac_expr();
	auto name = shared_ptr<TAC_Operand>(new TAC_TempOperand(d_type));

	tac_code += expr_code;
	tac_code += new TAC_ComputeStatement(op, name, nullptr, expr_name);

	return make_pair(tac_code, name);
}

pair<vector<TAC_Statement*>, shared_ptr<TAC_Operand>> AST_BinaryExpression::gen_tac_expr() {
	vector<TAC_Statement*> tac_code;

	auto [left_expr_code, left_expr_name] = left_expr -> gen_tac_expr();
	auto [right_expr_code, right_expr_name] = right_expr -> gen_tac_expr();
	auto name = shared_ptr<TAC_Operand>(new TAC_TempOperand(d_type));

	tac_code += left_expr_code;
	tac_code += right_expr_code;
	tac_code += new TAC_ComputeStatement(op, name, left_expr_name, right_expr_name);

	return make_pair(tac_code, name);
}

pair<vector<TAC_Statement*>, shared_ptr<TAC_Operand>> AST_TernaryExpression::gen_tac_expr() {
	vector<TAC_Statement*> tac_code;

	auto [cond_expr_code, cond_expr_name] = cond_expr -> gen_tac_expr();
	auto false_lab = shared_ptr<TAC_LabelOperand>(new TAC_LabelOperand());
	auto true_lab = shared_ptr<TAC_LabelOperand>(new TAC_LabelOperand());
	auto name = shared_ptr<TAC_VarOperand>(new TAC_VarOperand(d_type));
	auto [left_expr_code, left_expr_name] = left_expr -> gen_tac_expr();
	auto [right_expr_code, right_expr_name] = right_expr -> gen_tac_expr();
	auto cond_temp = shared_ptr<TAC_Operand>(new TAC_TempOperand('b'));

	tac_code += cond_expr_code;
	tac_code += new TAC_ComputeStatement("NOT", cond_temp, nullptr, cond_expr_name);
	tac_code += new TAC_IfGotoStatement(cond_temp, false_lab);
	tac_code += left_expr_code;
	tac_code += new TAC_AssignStatement(name, left_expr_name);
	tac_code += new TAC_GotoStatement(true_lab);
	tac_code += new TAC_LabelStatement(false_lab);
	tac_code += right_expr_code;
	tac_code += new TAC_AssignStatement(name, right_expr_name);
	tac_code += new TAC_LabelStatement(true_lab);

	return make_pair(tac_code, name);
}

pair<vector<TAC_Statement*>, shared_ptr<TAC_Operand>> AST_FuncExpression::gen_tac_expr() {
	vector<TAC_Statement*> tac_code;
	vector<shared_ptr<TAC_Operand>> tac_arg_list;

	char return_type = func_table[func_name] -> ret_type;

	auto name = return_type != 'v' ? shared_ptr<TAC_TempOperand>(new TAC_TempOperand(return_type)) : nullptr;

	for (auto expr : *arg_list) {
		auto [arg_tac_stmt, arg_name] = expr -> gen_tac_expr();
		tac_code += arg_tac_stmt;
		tac_arg_list += arg_name;
	}

	tac_code += new TAC_CallStatement(name, shared_ptr<TAC_FuncOperand>(new TAC_FuncOperand(func_name, return_type, tac_arg_list)));

	return make_pair(tac_code, name);
}

vector<TAC_Statement*> AST_AssignStatement::gen_tac_stmt() {
	vector<TAC_Statement*> tac_code;

	auto [expr_code, expr_name] = expr -> gen_tac_expr();

	tac_code += expr_code;
	tac_code += new TAC_AssignStatement(shared_ptr<TAC_VarOperand>(new TAC_VarOperand(var_name, get_type(var_name))), expr_name);

	return tac_code;
}

vector<TAC_Statement*> AST_ReadStatement::gen_tac_stmt() {
	return {new TAC_ReadStatement(shared_ptr<TAC_VarOperand>(new TAC_VarOperand(var_name, get_type(var_name))))};
}

vector<TAC_Statement*> AST_PrintStatement::gen_tac_stmt() {
	vector<TAC_Statement*> tac_code;

	auto [expr_code, expr_name] = expr -> gen_tac_expr();

	tac_code += expr_code;
	tac_code += new TAC_WriteStatement(expr_name);

	return tac_code;
}

vector<TAC_Statement*> AST_IfElseStatement::gen_tac_stmt() {
	vector<TAC_Statement*> tac_code;

	auto [expr_code, expr_name] = expr -> gen_tac_expr();
	auto if_stmt_code = if_stmt -> gen_tac_stmt();
	auto cond_temp = shared_ptr<TAC_Operand>(new TAC_TempOperand('b'));
	auto if_lab = else_stmt ? shared_ptr<TAC_LabelOperand>(new TAC_LabelOperand()) : nullptr;
	auto else_lab = shared_ptr<TAC_LabelOperand>(new TAC_LabelOperand());
	auto else_stmt_code = else_stmt ? else_stmt -> gen_tac_stmt() : vector<TAC_Statement*>{};

	tac_code += expr_code;
	tac_code += new TAC_ComputeStatement("NOT", cond_temp, nullptr, expr_name);
	tac_code += new TAC_IfGotoStatement(cond_temp, else_lab);
	tac_code += if_stmt_code;
	tac_code += new TAC_GotoStatement((else_stmt ? if_lab : else_lab));
	tac_code += new TAC_LabelStatement(else_lab);

	if (!else_stmt) return tac_code;

	tac_code += else_stmt_code;
	tac_code += new TAC_LabelStatement(if_lab);

	return tac_code;
}

vector<TAC_Statement*> AST_WhileStatement::gen_tac_stmt() {
	vector<TAC_Statement*> tac_code;

	auto [expr_code, expr_name] = expr -> gen_tac_expr();
	auto stmt_code = stmt -> gen_tac_stmt();
	auto cond_temp = shared_ptr<TAC_Operand>(new TAC_TempOperand('b'));
	auto continue_lab = shared_ptr<TAC_LabelOperand>(new TAC_LabelOperand());
	auto exit_lab = shared_ptr<TAC_LabelOperand>(new TAC_LabelOperand());

	tac_code += new TAC_LabelStatement(continue_lab);
	tac_code += expr_code;
	tac_code += new TAC_ComputeStatement("NOT", cond_temp, nullptr, expr_name);
	tac_code += new TAC_IfGotoStatement(cond_temp, exit_lab);
	tac_code += stmt_code;
	tac_code += new TAC_GotoStatement(continue_lab);
	tac_code += new TAC_LabelStatement(exit_lab);

	return tac_code;
}

vector<TAC_Statement*> AST_DoWhileStatement::gen_tac_stmt() {
	vector<TAC_Statement*> tac_code;

	auto stmt_code = stmt -> gen_tac_stmt();
	auto [expr_code, expr_name] = expr -> gen_tac_expr();
	auto loop_lab = shared_ptr<TAC_LabelOperand>(new TAC_LabelOperand());

	tac_code += new TAC_LabelStatement(loop_lab);
	tac_code += stmt_code;
	tac_code += expr_code;
	tac_code += new TAC_IfGotoStatement(expr_name, loop_lab);

	return tac_code;
}

vector<TAC_Statement*> AST_CompoundStatement::gen_tac_stmt() {
	vector<TAC_Statement*> tac_code;

	for (auto stmt : *stmt_list) tac_code += stmt -> gen_tac_stmt();

	return tac_code;
}

vector<TAC_Statement*> AST_CallStatement::gen_tac_stmt() {
	vector<TAC_Statement*> tac_code;

	auto [func_code, func_name] = expr -> gen_tac_expr();

	tac_code += func_code;
	if (lval != "") tac_code += new TAC_AssignStatement(shared_ptr<TAC_VarOperand>(new TAC_VarOperand(lval, get_type(lval))), func_name);

	return tac_code;
}

vector<TAC_Statement*> AST_ReturnStatement::gen_tac_stmt() {
	vector<TAC_Statement*> tac_code;

	auto [expr_code, expr_name] = expr -> gen_tac_expr();

	tac_code += expr_code;
	tac_code += new TAC_AssignStatement(curr_func -> return_var, expr_name);
	tac_code += new TAC_GotoStatement(curr_func -> return_label);

	return tac_code;
}

void TAC_VarOperand::add_stemp(char d_type, bool is_first) {
	curr_func -> decl_vars[name] = make_pair(d_type, is_first ? -1 : sinc+1000);
}