#include "function.h"
#include <algorithm>

extern std::ofstream asm_output_file;

using std::pair;
using std::sort;

string rtl_to_asm(string instr) {
	if (instr == "not") return "xori";
	if (instr == "uminus") return "neg";
	if (instr == "uminus.d") return "neg.d";
	if (instr == "sle.d") return "c.le.d";
	if (instr == "slt.d") return "c.lt.d";
	if (instr == "seq.d") return "c.eq.d";
	if (instr == "store") return "sw";
	if (instr == "store.d") return "s.d";
	if (instr == "load") return "lw";
	if (instr == "load.d") return "l.d";
	if (instr == "load_addr") return "la";
	if (instr == "iLoad") return "li";
	if (instr == "iLoad.d") return "li.d";
	if (instr == "goto") return "j";
	if (instr == "call") return "jal";
	if (instr == "move.d") return "mov.d";
	return instr;
}

template<typename T>
void print_asm_args(T t) {t -> print_asm_op(); asm_output_file << '\n';}

void print_asm_args(int t) {asm_output_file << t << '\n';}

template<typename T, typename... Args>
void print_asm_args(T t, Args... args) {
	if (t != nullptr) {t -> print_asm_op(); asm_output_file << ", ";}
	print_asm_args(args...);
}

template<typename... Args>
void print_asm(string t, Args... args) {
	asm_output_file << '\t' << rtl_to_asm(t) << " ";
	print_asm_args(args...);
}

unordered_map<string, int> offsets;

void RTL_IntOperand::print_asm_op() {asm_output_file << val;}
void RTL_FloatOperand::print_asm_op() {asm_output_file << val;}
void RTL_StringOperand::print_asm_op() {asm_output_file << "_str_" << str_map[str_const];}
void RTL_LabelOperand::print_asm_op() {asm_output_file << "Label" << lab_no;}
void RTL_RegisterOperand::print_asm_op() {asm_output_file << "$" << reg_name;}
void RTL_FuncOperand::print_asm_op() {asm_output_file << func_name;}
void RTL_VarOperand::print_asm_op() {
	if (offsets.find(name) != offsets.end()) asm_output_file << offsets[name] << "($fp)";
	else asm_output_file << name;
}

void RTL_ComputeStatement::gen_asm_stmt() {
	if (!result_register) print_asm(instr, lval, rval);
	else if (!lval) {
		if (instr == "not") print_asm(instr, result_register, rval, 1);
		else print_asm(instr, result_register, rval);
	}
	else print_asm(instr, result_register, lval, rval);
}

void RTL_MoveStatement::gen_asm_stmt() {
	if (aux_val) print_asm(instr, target, val, aux_val);
	else if (instr == "store" || instr == "store.d") print_asm(instr, val, target);
	else print_asm(instr, target, val);
}

void RTL_LabelStatement::gen_asm_stmt() {lab_op -> print_asm_op(); asm_output_file << ":\n";}
void RTL_ReadStatement::gen_asm_stmt() {asm_output_file << "\tsyscall\n";}
void RTL_WriteStatement::gen_asm_stmt() {asm_output_file << "\tsyscall\n";}
void RTL_GotoStatement::gen_asm_stmt() {print_asm(instr, lab_op);}
void RTL_IfGotoStatement::gen_asm_stmt() {print_asm(instr, cond, lab_op);}
void RTL_CallStatement::gen_asm_stmt() {print_asm(instr, func);}
void RTL_ReturnStatement::gen_asm_stmt() {asm_output_file << "\tj epilogue_" << curr_func -> name << "\n";}
void RTL_PopStatement::gen_asm_stmt() {asm_output_file << "\tadd $sp, $sp, " << (is_double ? 8 : 4) << "\n";}
void RTL_PushStatement::gen_asm_stmt() {
	bool is_double = (push_reg -> reg_name)[0] == 'f';
	asm_output_file << (is_double ? "\ts.d " : "\tsw ");
	push_reg -> print_asm_op();
	asm_output_file << ", " << (is_double ? -4 : 0) << "($sp)\n";
	asm_output_file << "\tsub $sp, $sp, " << (is_double ? 8 : 4) << "\n";
}

void print_asm() {
	if (global_symbol_table.size() || str_map.size()) {
		asm_output_file << "\n\t.data\n";

		vector<pair<string, pair<char, int>>> glob_print(global_symbol_table.begin(), global_symbol_table.end());
		sort(glob_print.begin(), glob_print.end(), [](auto a, auto b){return a.second.second < b.second.second;});

		for (auto glob_var : glob_print) {
			asm_output_file << glob_var.first << ":\t";
			asm_output_file << ((glob_var.second.first == 'f') ? ".double 0.0\n" : ".word 0\n");
		}

		vector<pair<string, int>> glob_str_print(str_map.begin(), str_map.end());
		sort(glob_str_print.begin(), glob_str_print.end(), [](auto a, auto b){return a.second < b.second;});

		for (auto glob_str : glob_str_print) {
			asm_output_file << "_str_" << glob_str.second << ":\t.asciiz " << glob_str.first << '\n';
		}
	}

	for (auto [_, func] : func_table) {
		curr_func = func;
		asm_output_file << "\n\t.text \n";
		asm_output_file << "\t.globl " << curr_func -> name << "\t\t\n";
		asm_output_file << curr_func -> name << ":\n";

		offsets.clear();

		int offset_p = 8;

		for (auto param : curr_func -> formal_params) {
			offsets[param.first] = offset_p;
			offset_p += (param.second == 'f' ? 8 : 4);
		}

		int offset_v = 0;

		vector<pair<string, pair<char, int>>> var_sorted(curr_func -> decl_vars.begin(), curr_func -> decl_vars.end());
		sort(var_sorted.begin(), var_sorted.end(), [](auto a, auto b){return a.second.second < b.second.second;});

		for (auto var : var_sorted) {
			offset_v -= (var.second.first == 'f' ? 8 : 4);
			offsets[var.first] = offset_v;
		}

		offset_v -= 8;

		asm_output_file << "\tsw $ra, 0($sp)\n";
		asm_output_file << "\tsw $fp, -4($sp)\n";
		asm_output_file << "\tsub $fp, $sp, 4\n";
		asm_output_file << "\tsub $sp, $sp, " << -offset_v << "\n";

		for (auto rtl_stmt : *(curr_func -> rtl_statements)) rtl_stmt -> gen_asm_stmt();

		asm_output_file << "epilogue_" << curr_func -> name << ":\n";
		asm_output_file << "\tadd $sp, $sp, " << -offset_v << "\n";
		asm_output_file << "\tlw $fp, -4($sp)\n";
		asm_output_file << "\tlw $ra, 0($sp)\n";
		asm_output_file << "\tjr $ra";
	}
}