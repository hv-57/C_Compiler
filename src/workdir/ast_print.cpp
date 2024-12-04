#include "function.h"

extern std::ofstream ast_output_file;

string print_type(char type) {
	switch(type) {
		case 'i': return "<int>";
		case 'f': return "<float>";
		case 's': return "<string>";
		case 'b': return "<bool>";
		case 'v': return "<void>";
	}
	return "<>";
}

void AST_VarExpression::print_ast_expr(string) {
	ast_output_file << "Name : " << var_name << print_type(d_type);
}

void AST_IntExpression::print_ast_expr(string) {
	ast_output_file << "Num : " << val << print_type(d_type);
}

void AST_FloatExpression::print_ast_expr(string) {
	ast_output_file << "Num : " << val << print_type(d_type);
}

void AST_StringExpression::print_ast_expr(string) {
	ast_output_file << "String : " << val << print_type(d_type);
}

void AST_UnaryExpression::print_ast_expr(string prefix) {
	ast_output_file << prefix << ((op == "Uminus") ? "Arith: Uminus" : "Condition: NOT") << print_type(d_type);

	ast_output_file << prefix << "  L_Opd (";
	expr -> print_ast_expr(prefix + "    ");
	ast_output_file << ")";
}

void AST_BinaryExpression::print_ast_expr(string prefix) {
	ast_output_file << prefix << ((binexp_type == 'a') ? "Arith: " : "Condition: ") << op << print_type(d_type);

	ast_output_file << prefix << "  L_Opd (";
	left_expr -> print_ast_expr(prefix + "    ");
	ast_output_file << ")";

	ast_output_file << prefix << "  R_Opd (";
	right_expr -> print_ast_expr(prefix + "    ");
	ast_output_file << ")";
}

void AST_TernaryExpression::print_ast_expr(string prefix) {
	cond_expr -> print_ast_expr(prefix + "    ");

	ast_output_file << prefix << "    True_Part (";
	left_expr -> print_ast_expr(prefix + "    ");
	ast_output_file << ")";

	ast_output_file << prefix << "    False_Part (";
	right_expr -> print_ast_expr(prefix + "    ");
	ast_output_file << ")";
}

void AST_FuncExpression::print_ast_expr(string prefix) {
	ast_output_file << prefix << "FN CALL: " << func_name << "(";

	for(auto expr : *arg_list) {
		ast_output_file << prefix;
		expr -> print_ast_expr(prefix + "  ");
	}

	ast_output_file << ")";
}

void AST_AssignStatement::print_ast_stmt(string prefix) {
	ast_output_file << prefix << "Asgn: ";

	ast_output_file << prefix << "  LHS (Name : " << var_name << print_type(expr -> d_type) << ")";

	ast_output_file << prefix << "  RHS (";
	expr -> print_ast_expr(prefix + "    ");
	ast_output_file << ")";
}

void AST_PrintStatement::print_ast_stmt(string prefix) {
	ast_output_file << prefix << "Write: ";
	expr -> print_ast_expr(prefix + "  ");
}

void AST_ReadStatement::print_ast_stmt(string prefix) {
	ast_output_file << prefix << "Read: Name: " << var_name << print_type(get_type(var_name));
}

void AST_IfElseStatement::print_ast_stmt(string prefix) {
	ast_output_file << prefix << "If: ";

	ast_output_file << prefix << "  Condition (";
	expr -> print_ast_expr(prefix + "    ");
	ast_output_file << ")";

	ast_output_file << prefix << "  Then (";
	if_stmt -> print_ast_stmt(prefix + "    ");
	ast_output_file << ")";

	if (else_stmt) {
		ast_output_file << prefix << "  Else (";
		else_stmt -> print_ast_stmt(prefix + "    ");
		ast_output_file << ")";
	}
}

void AST_WhileStatement::print_ast_stmt(string prefix) {
	ast_output_file << prefix << "While: ";

	ast_output_file << prefix << "  Condition (";
	expr -> print_ast_expr(prefix + "    ");
	ast_output_file << ")";

	ast_output_file << prefix << "  Body (";
	stmt -> print_ast_stmt(prefix + "    ");
	ast_output_file << ")";
}

void AST_DoWhileStatement::print_ast_stmt(string prefix) {
	ast_output_file << prefix << "Do: ";

	ast_output_file << prefix << "  Body (";
	stmt -> print_ast_stmt(prefix + "    ");
	ast_output_file << ")";

	ast_output_file << prefix << "  While Condition (";
	expr -> print_ast_expr(prefix + "    ");
	ast_output_file << ")";
}

void AST_CompoundStatement::print_ast_stmt(string prefix) {
	for(auto stmt : *stmt_list) stmt -> print_ast_stmt(prefix);
}

void AST_CallStatement::print_ast_stmt(string prefix) {
	if (lval == "") {expr -> print_ast_expr(prefix); return;}

	ast_output_file << prefix << "Asgn:";

	ast_output_file << prefix << "  LHS (Name : " << lval << print_type(expr -> d_type) << ")";

	ast_output_file << prefix << "  RHS (";
	expr -> print_ast_expr(prefix + "    ");
	ast_output_file << ")";
}

void AST_ReturnStatement::print_ast_stmt(string prefix) {
	ast_output_file << prefix << "Return: ";
	expr -> print_ast_expr(prefix + "  ");
}

void Function::print_ast() {
	ast_output_file << "**PROCEDURE: " << name << "\n";
	ast_output_file << "\tReturn Type: " << print_type(ret_type) << "\n";

	ast_output_file << "\tFormal Parameters:\n";
	for (auto [param_name, param_type] : formal_params) ast_output_file << "\t\t" << param_name << "  Type: " << print_type(param_type) << "\n";

	ast_output_file << "**BEGIN: Abstract Syntax Tree ";
	for(auto stmt : *ast_statements) stmt -> print_ast_stmt("\n         ");
	ast_output_file << "\n**END: Abstract Syntax Tree \n" << std::flush;
}