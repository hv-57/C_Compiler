#ifndef TAC
#define TAC

#include "rtl.h"
#include <vector>
#include <unordered_map>

using std::vector;
using std::unordered_map;

class TAC_Operand {
	public:
		char d_type;
		RTL_Opcode rtl_instr;
		shared_ptr<RTL_RegisterOperand> reg;

		TAC_Operand() {}
		TAC_Operand(char d_type_, RTL_Opcode rtl_instr_) : d_type(d_type_), rtl_instr(rtl_instr_), reg(nullptr) {}

		virtual void print_tac_op() = 0;
		virtual RTL_Statement* gen_rtl_stmt() = 0;
		virtual shared_ptr<RTL_Operand> gen_rtl_op() = 0;
		virtual ~TAC_Operand() {}
};

class TAC_IntOperand : public TAC_Operand {
	public:
		int val;

		TAC_IntOperand(int val_) : TAC_Operand('i', RTL_Opcode::Load_i), val(val_) {}

		void print_tac_op();
		RTL_Statement* gen_rtl_stmt();
		shared_ptr<RTL_Operand> gen_rtl_op();
};

class TAC_FloatOperand : public TAC_Operand {
	public:
		double val;

		TAC_FloatOperand(double val_) : TAC_Operand('f', RTL_Opcode::Load_id), val(val_) {}

		void print_tac_op();
		RTL_Statement* gen_rtl_stmt();
		shared_ptr<RTL_Operand> gen_rtl_op();
};

class TAC_StringOperand : public TAC_Operand {
	public:
		string str_const;

		TAC_StringOperand(string str_const_) : TAC_Operand('s', RTL_Opcode::Load_a), str_const(str_const_) {}

		void print_tac_op();
		RTL_Statement* gen_rtl_stmt();
		shared_ptr<RTL_Operand> gen_rtl_op();
};

class TAC_VarOperand : public TAC_Operand {
	public:
		string name;
		static uint sinc;

		TAC_VarOperand(string name_, char d_type_) : TAC_Operand(d_type_, d_type_ == 'f' ? RTL_Opcode::Load_d : RTL_Opcode::Load), name(name_) {}
		TAC_VarOperand(char d_type_, bool is_first = 0) : TAC_Operand(d_type_, d_type_ == 'f' ? RTL_Opcode::Load_d : RTL_Opcode::Load), name("stemp" + std::to_string(sinc++)) {add_stemp(d_type_, is_first);}

		void print_tac_op();
		RTL_Statement* gen_rtl_stmt();
		shared_ptr<RTL_Operand> gen_rtl_op();

		void add_stemp(char d_type, bool is_first);
};

inline uint TAC_VarOperand::sinc = 0;

class TAC_FuncOperand : public TAC_Operand {
	public:
		string func_name;
		vector<shared_ptr<TAC_Operand>> args;

		TAC_FuncOperand(string func_name_, char d_type_, vector<shared_ptr<TAC_Operand>> args_) : TAC_Operand(d_type_, RTL_Opcode::null), func_name(func_name_), args(args_) {}

		void print_tac_op();
		RTL_Statement* gen_rtl_stmt();
		shared_ptr<RTL_Operand> gen_rtl_op();
};

class TAC_TempOperand : public TAC_Operand {
	public:
		int temp_no;
		static int inc;

		TAC_TempOperand(char d_type_) : TAC_Operand(d_type_ == 'b' ? 'i' : d_type_, d_type_ == 'f' ? RTL_Opcode::Move_d : RTL_Opcode::Move), temp_no(inc++) {}

		void print_tac_op();
		RTL_Statement* gen_rtl_stmt();
		shared_ptr<RTL_Operand> gen_rtl_op();
};

inline int TAC_TempOperand::inc = 0;

class TAC_LabelOperand {
	public:
		int lab_no;
		static int linc;

		TAC_LabelOperand() : lab_no(linc++) {}

		void print_tac_op();
};

inline int TAC_LabelOperand::linc = 0;

class TAC_Statement {
	public:
		virtual vector<RTL_Statement*> gen_rtl_stmt() = 0;
		virtual void print_tac_stmt() = 0;
		virtual ~TAC_Statement() {}
};

inline unordered_map<string, string> conv_op = {{"GT", ">"}, {"LT", "<"}, {"GE", ">="}, {"LE", "<="}, {"NE", "!="}, {"EQ", "=="}, {"OR", "||"}, {"NOT", "!"}, {"AND", "&&"}, {"Plus", "+"}, {"Mult", "*"}, {"Div", "/"}, {"Minus", "-"}, {"Uminus", "-"}};

class TAC_ComputeStatement : public TAC_Statement {
	public:
		shared_ptr<TAC_Operand> res;
		shared_ptr<TAC_Operand> l_op;
		shared_ptr<TAC_Operand> r_op;
		string op;

		TAC_ComputeStatement(string op_, shared_ptr<TAC_Operand> res_, shared_ptr<TAC_Operand> l_op_, shared_ptr<TAC_Operand> r_op_) : res(res_), l_op(l_op_), r_op(r_op_), op(conv_op[op_]) {}
		~TAC_ComputeStatement() {res.reset(); l_op.reset(); r_op.reset();}

		vector<RTL_Statement*> gen_rtl_stmt();
		void print_tac_stmt();
};

class TAC_AssignStatement : public TAC_Statement {
	public:
		shared_ptr<TAC_VarOperand> lval;
		shared_ptr<TAC_Operand> rval;

		TAC_AssignStatement(shared_ptr<TAC_VarOperand> lval_, shared_ptr<TAC_Operand> rval_) : lval(lval_), rval(rval_) {}
		~TAC_AssignStatement() {lval.reset(); rval.reset();}

		vector<RTL_Statement*> gen_rtl_stmt();
		void print_tac_stmt();
};

class TAC_GotoStatement : public TAC_Statement {
	public:
		shared_ptr<TAC_LabelOperand> label;

		TAC_GotoStatement(shared_ptr<TAC_LabelOperand> label_) : label(label_) {}
		~TAC_GotoStatement() {label.reset();}

		vector<RTL_Statement*> gen_rtl_stmt();
		void print_tac_stmt();
};

class TAC_IfGotoStatement : public TAC_Statement {
	public:
		shared_ptr<TAC_Operand> if_cond;
		shared_ptr<TAC_LabelOperand> label;

		TAC_IfGotoStatement(shared_ptr<TAC_Operand> if_cond_, shared_ptr<TAC_LabelOperand> label_) : if_cond(if_cond_), label(label_) {}
		~TAC_IfGotoStatement() {if_cond.reset(); label.reset();}

		vector<RTL_Statement*> gen_rtl_stmt();
		void print_tac_stmt();
};

class TAC_LabelStatement : public TAC_Statement {
	public:
		shared_ptr<TAC_LabelOperand> label;

		TAC_LabelStatement(shared_ptr<TAC_LabelOperand> label_) : label(label_) {}
		~TAC_LabelStatement() {label.reset();}

		vector<RTL_Statement*> gen_rtl_stmt();
		void print_tac_stmt();
};

class TAC_ReadStatement : public TAC_Statement {
	public:
		shared_ptr<TAC_VarOperand> var;

		TAC_ReadStatement(shared_ptr<TAC_VarOperand> var_) : var(var_) {}
		~TAC_ReadStatement() {var.reset();}

		vector<RTL_Statement*> gen_rtl_stmt();
		void print_tac_stmt();
};

class TAC_WriteStatement : public TAC_Statement {
	public:
		shared_ptr<TAC_Operand> val;

		TAC_WriteStatement(shared_ptr<TAC_Operand> val_) : val(val_) {}
		~TAC_WriteStatement() {val.reset();}

		vector<RTL_Statement*> gen_rtl_stmt();
		void print_tac_stmt();
};

class TAC_CallStatement : public TAC_Statement {
	public:
		shared_ptr<TAC_TempOperand> lval;
		shared_ptr<TAC_FuncOperand> func;

		TAC_CallStatement(shared_ptr<TAC_TempOperand> lval_, shared_ptr<TAC_FuncOperand> func_) : lval(lval_), func(func_) {}
		~TAC_CallStatement() {lval.reset(); func.reset();}

		vector<RTL_Statement*> gen_rtl_stmt();
		void print_tac_stmt();
};

class TAC_ReturnStatement : public TAC_Statement {
	public:
		shared_ptr<TAC_VarOperand> ret_var;

		TAC_ReturnStatement(shared_ptr<TAC_VarOperand> ret_var_) : ret_var(ret_var_) {}
		~TAC_ReturnStatement() {ret_var.reset();}

		vector<RTL_Statement*> gen_rtl_stmt();
		void print_tac_stmt();
};

#endif
