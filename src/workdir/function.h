#ifndef FUNC_HANDLING
#define FUNC_HANDLING

#include "ast.h"
#include <iostream>
#include <map>
#include <unordered_set>

extern void cleanup();

template <typename T>
void operator+=(vector<T>& lhs, const vector<T>& rhs) {lhs.insert(lhs.end(), rhs.begin(), rhs.end());}

template <typename T, typename D>
void operator+=(vector<T>& lhs, const D& rhs) {if (rhs == nullptr) return; lhs.push_back(rhs);}

template <typename T>
void print_error(T t) {std::cerr << t << '\n';}

template <typename T, typename... Args>
void print_error(T t, Args... args) {
	std::cerr << t << " ";
	print_error(args...);
}

template <typename... Args>
void raise_error(Args... args) {
	print_error(args...);
	cleanup();
	exit(1);
}

class Function {
	public:
		char ret_type;
		string name;

		vector<pair<string, char>> formal_params;
		unordered_map<string, pair<char, int>> decl_vars;

		vector<AST_Statement*>* ast_statements;
		vector<TAC_Statement*>* tac_statements;
		vector<RTL_Statement*>* rtl_statements;

		shared_ptr<TAC_LabelOperand> return_label;
		shared_ptr<TAC_VarOperand> return_var;

		void print_ast();
		void print_tac();
		void print_rtl();

		void validate_decl();
		void validate_def();

		Function(char type, string name_) : ret_type(type), name(name_), ast_statements(nullptr), tac_statements(nullptr), rtl_statements(nullptr) {}
		~Function() {
			return_label.reset();
			return_var.reset();
			if (ast_statements) {for (auto stmt : *ast_statements) delete stmt; delete ast_statements;}
			if (tac_statements) {for (auto stmt : *tac_statements) delete stmt; delete tac_statements;}
			if (rtl_statements) {for (auto stmt : *rtl_statements) delete stmt; delete rtl_statements;}
		}
};

void add_string(string name);
void add_decl(char type, string name);
char get_type(string name);

void build_and_print_IRs();
void validate_symbol_table();
void perform_cleanup();

void print_asm();

inline unordered_map<string, pair<char, int>> global_symbol_table;
inline Function* curr_func = nullptr;
inline std::map<string, Function*> func_table;

inline vector<AST_FuncExpression*> called_funcs;
inline unordered_map<string, int> str_map;

enum Operator {
	Plus, Minus, Mult, Div, And, Or, Not, LessThan, GreaterThan, LessEqual, GreaterEqual, Equal, NotEqual
};
#endif
