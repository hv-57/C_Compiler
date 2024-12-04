#include "function.h"

AST_Expression::AST_Expression(char d_type_ = 'u') : d_type(d_type_), tac_stmt_list({}) {}

AST_VarExpression::AST_VarExpression(string var_name_) : AST_Expression(get_type(var_name_)), var_name(var_name_) {}

AST_IntExpression::AST_IntExpression(int val_) : AST_Expression('i'), val(val_) {}

AST_FloatExpression::AST_FloatExpression(double val_) : AST_Expression('f'), val(val_) {}

AST_StringExpression::AST_StringExpression(string val_) : AST_Expression('s'), val(val_) {}

AST_UnaryExpression::AST_UnaryExpression(string op_, AST_Expression* expr_) : AST_Expression(expr_ -> d_type), op(op_), expr(expr_) {
	if (op_ == "NOT" && expr_ -> d_type != 'b') {delete this; raise_error("Wrong type for operand in Boolean expression");}
	if (op_ == "Uminus" && !(expr_ -> d_type == 'i' || expr_ -> d_type == 'f')) {delete this; raise_error("Wrong type of operand in UMINUS expression");}
}

AST_BinaryExpression::AST_BinaryExpression(string op_, AST_Expression* left_expr_, AST_Expression* right_expr_, char binexp_type_) : AST_Expression(binexp_type_ == 'a' ? left_expr_ -> d_type : 'b'), op(op_), left_expr(left_expr_), right_expr(right_expr_), binexp_type(binexp_type_) {
	switch(binexp_type_) {
		case 'a':
			if (left_expr_ -> d_type != 'i' && left_expr_ -> d_type != 'f') {delete this; raise_error("Wrong type of operand in", op_, "expression");}
			if (left_expr_ -> d_type != right_expr_ -> d_type) {delete this; raise_error("Type mismatch in", op_, "expression");}
			break;
		case 'b':
			if (left_expr_ -> d_type != 'b') {delete this; raise_error("Wrong type of operand in Boolean expression");}
			if (left_expr_ -> d_type != right_expr_ -> d_type) {delete this; raise_error("Type mismatch in Boolean expression");}
			break;
		case 'r':
			if (left_expr_ -> d_type != 'i' && left_expr_ -> d_type != 'f') {delete this; raise_error("Wrong type of operand in Relational expression");}
			if (left_expr_ -> d_type != right_expr_ -> d_type) {delete this; raise_error("Type mismatch between operands in Relational expression");}
			break;
	}
}

AST_TernaryExpression::AST_TernaryExpression(AST_Expression* cond_expr_, AST_Expression* left_expr_, AST_Expression* right_expr_) : AST_Expression(left_expr_ -> d_type), cond_expr(cond_expr_), left_expr(left_expr_), right_expr(right_expr_) {
	if (cond_expr_ -> d_type != 'b') {delete this; raise_error("Wrong type of condition in conditional ast");}
	if (left_expr_ -> d_type != right_expr_ -> d_type) {delete this; raise_error("Different data types of the two operands of conditional ast");}
}

AST_FuncExpression::AST_FuncExpression(string func_name_, vector<AST_Expression*>* arg_list_) : func_name(func_name_), arg_list(arg_list_) {
	if (func_table.find(func_name_) == func_table.end()) {delete this; raise_error("Procedure corresponding to the name is not found");}
	if ((func_table[func_name_] -> formal_params).size() != arg_list_ -> size()) {delete this; raise_error("Actual and formal parameter counts do not match");}
	for (uint i = 0; i < arg_list_ -> size(); ++i) if ((*arg_list_)[i] -> d_type != ((func_table[func_name_] -> formal_params)[i]).second) {delete this; raise_error("Actual and formal parameters data types do not match");}
	called_funcs.push_back(this);
	d_type = func_table[func_name_] -> ret_type;
}

AST_ReadStatement::AST_ReadStatement(string var_name_) : var_name(var_name_) {
	if (get_type(var_name_) != 'i' && get_type(var_name_) != 'f') {delete this; raise_error("Only Int and Float variables are allowed in a Read Stmt");}
}

AST_PrintStatement::AST_PrintStatement(AST_Expression* expr_) : expr(expr_) {
	if (expr_ -> d_type == 'b') {delete this; raise_error("A bool variable is not allowed in a Print Stmt");}
}

AST_AssignStatement::AST_AssignStatement(string var_name_, AST_Expression* expr_) : var_name(var_name_), expr(expr_) {
	if (expr_ -> d_type != get_type(var_name_)) {delete this; raise_error("Assignment statement data type not compatible");}
}

AST_IfElseStatement::AST_IfElseStatement(AST_Expression* expr_, AST_Statement* if_stmt_, AST_Statement* else_stmt_) : expr(expr_), if_stmt(if_stmt_), else_stmt(else_stmt_) {
	if (expr_ -> d_type != 'b') {delete this; raise_error("Wrong type of Condition in IF statement");}
}

AST_WhileStatement::AST_WhileStatement(AST_Expression* expr_, AST_Statement* stmt_) : expr(expr_), stmt(stmt_) {
	if (expr_ -> d_type != 'b') {delete this; raise_error("Wrong type of Condition in WHILE statement");}
}

AST_DoWhileStatement::AST_DoWhileStatement(AST_Expression* expr_, AST_Statement* stmt_) : expr(expr_), stmt(stmt_) {
	if (expr_ -> d_type != 'b') {delete this; raise_error("Wrong type of Condition in WHILE statement");}
}

AST_CompoundStatement::AST_CompoundStatement() : stmt_list(new vector<AST_Statement*>) {}

AST_CallStatement::AST_CallStatement(string lval_, AST_FuncExpression* expr_) : lval(lval_), expr(expr_) {
	if (lval_ != "" && get_type(lval_) != expr_ -> d_type) {delete this; raise_error("Assignment statement data type not compatible");}
	if (lval_ == "" && expr_ -> d_type != 'v') {delete this; raise_error("Return value of a procedure ignored");}
}

AST_ReturnStatement::AST_ReturnStatement(AST_Expression* expr_) : expr(expr_) {
	if (curr_func -> ret_type == 'v' || expr_ -> d_type != curr_func -> ret_type) {delete this; raise_error("Return type does not match");}
}
