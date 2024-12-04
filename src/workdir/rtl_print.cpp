#include "function.h"

extern std::ofstream rtl_output_file;

void RTL_VarOperand::print_rtl_op() {rtl_output_file << name;}
void RTL_IntOperand::print_rtl_op() {rtl_output_file << val;}
void RTL_FloatOperand::print_rtl_op() {rtl_output_file << val;}
void RTL_StringOperand::print_rtl_op() {rtl_output_file << "_str_" << str_map[str_const];}
void RTL_LabelOperand::print_rtl_op() {rtl_output_file << "Label" << lab_no;}
void RTL_RegisterOperand::print_rtl_op() {rtl_output_file << reg_name;}
void RTL_FuncOperand::print_rtl_op() {rtl_output_file << func_name;}

string padded_instr(string instr) {
	if (instr != "read" && instr != "write" && instr != "pop") instr += ':';
	instr.append(13 - instr.length(), ' ');
	return instr;
}

void RTL_ComputeStatement::print_rtl_stmt() {
	if (!result_register) {
		rtl_output_file << "\n\t" << padded_instr(instr);
		lval -> print_rtl_op();
		rtl_output_file << " , ";
		rval -> print_rtl_op();
	}
	else {
		rtl_output_file << "\n\t" << padded_instr(instr);
		result_register -> print_rtl_op();
		rtl_output_file << " <- ";
		if (lval) {lval -> print_rtl_op(); rtl_output_file << " , ";}
		rval -> print_rtl_op();
	}
}

void RTL_MoveStatement::print_rtl_stmt() {
	rtl_output_file << "\n\t" << padded_instr(instr);
	target -> print_rtl_op();
	rtl_output_file << " <- ";
	val -> print_rtl_op();
	if (aux_val) {rtl_output_file << " , "; aux_val -> print_rtl_op();}
}

void RTL_LabelStatement::print_rtl_stmt() {rtl_output_file << "\n\n  " << padded_instr(string("Label") + std::to_string(lab_op -> lab_no));}

void RTL_ReadStatement::print_rtl_stmt() {rtl_output_file << "\n\t" << padded_instr(instr);}

void RTL_WriteStatement::print_rtl_stmt() {rtl_output_file << "\n\t" << padded_instr(instr);}

void RTL_GotoStatement::print_rtl_stmt() {
	rtl_output_file << "\n\t" << padded_instr(instr);
	lab_op -> print_rtl_op();
}

void RTL_IfGotoStatement::print_rtl_stmt() {
	rtl_output_file << "\n\t" << padded_instr(instr);
	cond -> print_rtl_op();
	rtl_output_file << " , ";
	lab_op -> print_rtl_op();
}

void RTL_CallStatement::print_rtl_stmt() {
	rtl_output_file << "\n\t";
	if (ret_reg) {ret_reg -> print_rtl_op(); rtl_output_file << " = ";}
	rtl_output_file << "call ";
	func -> print_rtl_op();
}

void RTL_ReturnStatement::print_rtl_stmt() {
	rtl_output_file << "\n\treturn      ";
	ret_reg -> print_rtl_op();
}

void RTL_PushStatement::print_rtl_stmt() {
	rtl_output_file << "\n\t" << padded_instr(instr);
	push_reg -> print_rtl_op();
}

void RTL_PopStatement::print_rtl_stmt() {rtl_output_file << "\n\t" << padded_instr(instr);}

void Function::print_rtl() {
	rtl_output_file << "**PROCEDURE: " << name;
	rtl_output_file << "\n**BEGIN: RTL Statements";
	for (auto rtl_stmt : *rtl_statements) rtl_stmt -> print_rtl_stmt();
	rtl_output_file << "\n**END: RTL Statements\n" << std::flush;
}
