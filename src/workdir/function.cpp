#include "function.h"

extern bool sa_parse;
extern bool sa_ast;
extern bool sa_tac;
extern bool sa_rtl;

extern bool show_ast;
extern bool show_tac;
extern bool show_rtl;
extern bool show_asm;

int func_decl_order;

using std::make_pair;

void build_and_print_IRs() {
	if (sa_parse) return;

	for (auto called_func : called_funcs) if (!(func_table[called_func -> func_name] -> ast_statements)) raise_error("Called procedure is not defined");

	for (auto [_, func] : func_table) {
		curr_func = func;

		if (show_ast) func -> print_ast();

		if (!sa_ast) {
			func -> tac_statements = new vector<TAC_Statement*>;
			TAC_VarOperand::sinc = 0;
			TAC_TempOperand::inc = 0;

			if (func -> ret_type != 'v') func -> return_var = shared_ptr<TAC_VarOperand>(new TAC_VarOperand(func -> ret_type, 1));
			for (auto stmt : *(func -> ast_statements)) *(func -> tac_statements) += stmt -> gen_tac_stmt();
			if (func -> ret_type != 'v') {
				*(func -> tac_statements) += new TAC_LabelStatement(func -> return_label);
				*(func -> tac_statements) += new TAC_ReturnStatement(func -> return_var);
			}
		}

		if (show_tac && func -> tac_statements -> size()) func -> print_tac();

		if (!sa_tac) {
			func -> rtl_statements = new vector<RTL_Statement*>;
			for (auto stmt : *(func -> tac_statements)) *(func -> rtl_statements) += stmt -> gen_rtl_stmt();
		}

		if (show_rtl && func -> rtl_statements -> size()) func -> print_rtl();
	}
	if (show_asm) print_asm();
}

char get_type(string name) {
	if (curr_func -> decl_vars.find(name) != curr_func -> decl_vars.end()) return curr_func -> decl_vars[name].first;
	for (auto [param_name, param_type] : curr_func -> formal_params) if (param_name == name) return param_type;
	if (global_symbol_table.find(name) != global_symbol_table.end()) return global_symbol_table[name].first;

	raise_error("Variable", name, "has not been declared");
	return '\0';
}

void validate_params(vector<pair<string, char>>& vec) {
	std::unordered_set<string> unique;
	for (auto [name, _] : vec) {
		if (func_table.find(name) != func_table.end()) {delete curr_func; raise_error("Variable", name, "coincides with a procedure name");}
		if (unique.find(name) != unique.end()) {delete curr_func; raise_error("Variable is declared twice in the same scope");}
		unique.insert(name);
	}
}

void Function::validate_decl() {
	if (global_symbol_table.find(name) != global_symbol_table.end()) {string tmp = name; delete this; raise_error("Procedure name", tmp, "coincides with that of a global variable");}

	if (ret_type != 'v') return_label = shared_ptr<TAC_LabelOperand>(new TAC_LabelOperand());

	if (func_table.find(name) != func_table.end()) {
		auto& lookup  = func_table[name];
		if (lookup -> ast_statements) {delete this; raise_error("Procedure", lookup -> name, "is already defined once");}
		if (ret_type == lookup -> ret_type) {delete this; raise_error("Procedure", lookup -> name, "is already declared once");}
		else {delete this; raise_error("A declaration with a different return type exists for procedure", lookup -> name);}
	}

	func_table[name] = this;
	validate_params(formal_params);
}

void Function::validate_def() {
	ast_statements = new vector<AST_Statement*>;

	if (global_symbol_table.find(name) != global_symbol_table.end()) {string tmp = name; delete this; raise_error("Procedure name", tmp, "coincides with that of a global variable");}

	if (func_table.find(name) != func_table.end()) {
		auto& lookup  = func_table[name];
		if (lookup -> ast_statements) {delete this; raise_error("Procedure", lookup -> name, "is already defined once");}
		if (ret_type != lookup -> ret_type) {delete this; raise_error("A declaration with a different return type exists for procedure", lookup -> name);}
		if (formal_params.size() != lookup -> formal_params.size()) {delete this; raise_error("Definition and declaration should have same number of params");}
		for (uint i = 0; i < formal_params.size(); ++i) if (formal_params[i].second != lookup -> formal_params[i].second) {delete this; raise_error("Types of parameters in declaration and definition do not match");}
		validate_params(formal_params);
		this -> return_label = lookup -> return_label;
		delete lookup;
		lookup = this;
		return;
	}

	if (ret_type != 'v') return_label = shared_ptr<TAC_LabelOperand>(new TAC_LabelOperand());

	func_table[name] = this;
	validate_params(formal_params);
	func_decl_order = 0;
}

void add_string(string name) {
	int prev_size = str_map.size();
	if (str_map.find(name) == str_map.end()) str_map[name] = prev_size;
}

void add_decl(char type, string name) {
	static int glob_order;
	if (sa_parse) return;
	if (func_table.find(name) != func_table.end()) raise_error("Variable", name, "coincides with a procedure name");
	if (type == 'v') raise_error("Variables cannot have a void type");
	if (!curr_func) {
		if (global_symbol_table.find(name) != global_symbol_table.end()) raise_error("Variable is declared twice in the same scope");
		global_symbol_table[name] = make_pair(type, glob_order++);
	}
	else {
		for (auto [param_name, _] : curr_func -> formal_params) if (param_name == name) raise_error("Variable", name, "is already declared as a formal parameter");
		if (curr_func -> decl_vars.find(name) != curr_func -> decl_vars.end()) raise_error("Variable is declared twice in the same scope");
		curr_func -> decl_vars[name] = make_pair(type, func_decl_order++);
	}
}

void validate_symbol_table() {
	if (!sa_parse) {
		if (func_table.find("main") == func_table.end()) raise_error("The main function should be defined");
	}
}

void perform_cleanup() {
	for (auto [_, func] : func_table) delete func;
}
