#include "function.h"

extern std::ofstream tac_output_file;

void TAC_IntOperand::print_tac_op() {tac_output_file << val;}

void TAC_FloatOperand::print_tac_op() {tac_output_file << val;}

void TAC_StringOperand::print_tac_op() {tac_output_file << str_const;}

void TAC_VarOperand::print_tac_op() {tac_output_file << name;}

void TAC_TempOperand::print_tac_op() {tac_output_file << "temp" << temp_no;}

void TAC_FuncOperand::print_tac_op() {
	tac_output_file << func_name << "(";
	if (!args.size()) {tac_output_file << ")"; return;}
	for (uint i = 0; i < args.size() - 1; ++i) {
		args[i] -> print_tac_op();
		tac_output_file << ", ";
	}
	args.back() -> print_tac_op();
	tac_output_file << ")";
}

void TAC_LabelOperand::print_tac_op() {tac_output_file << "Label" << lab_no;}

void TAC_ComputeStatement::print_tac_stmt() {
	tac_output_file << "\n\t";
	res -> print_tac_op();
	tac_output_file << " = ";
	if (l_op) {l_op -> print_tac_op(); tac_output_file << " ";}
	tac_output_file << op << " ";
	r_op -> print_tac_op();
}

void TAC_AssignStatement::print_tac_stmt() {
	tac_output_file << "\n\t";
	lval -> print_tac_op();
	tac_output_file << " = ";
	rval -> print_tac_op();
}

void TAC_GotoStatement::print_tac_stmt() {
	tac_output_file << "\n\tgoto ";
	label -> print_tac_op();
}

void TAC_IfGotoStatement::print_tac_stmt() {
	tac_output_file << "\n\tif(";
	if_cond -> print_tac_op();
	tac_output_file << ") goto ";
	label -> print_tac_op();
}

void TAC_LabelStatement::print_tac_stmt() {
	tac_output_file << "\n";
	label -> print_tac_op();
	tac_output_file << ":";
}

void TAC_ReadStatement::print_tac_stmt() {
	tac_output_file << "\n\tread  ";
	var -> print_tac_op();
}

void TAC_WriteStatement::print_tac_stmt() {
	tac_output_file << "\n\twrite  ";
	val -> print_tac_op();
}

void TAC_CallStatement::print_tac_stmt() {
	tac_output_file << "\n\t";
	if (lval) {lval -> print_tac_op(); tac_output_file << " = ";}
	func -> print_tac_op();
}

void TAC_ReturnStatement::print_tac_stmt() {
	tac_output_file << "\n\t return ";
	ret_var -> print_tac_op();
}

void Function::print_tac() {
	tac_output_file << "**PROCEDURE " << name << "\n";
	tac_output_file << "**BEGIN: Three Address Code Statements";
	for (auto tac_stmt : *tac_statements) tac_stmt -> print_tac_stmt();
	tac_output_file << "\n**END: Three Address Code Statements\n" << std::flush;
}