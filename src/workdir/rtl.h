#ifndef RTL
#define RTL

#include <string>
#include <fstream>
#include <memory>

using std::string;
using std::shared_ptr;

class RTL_Operand {
	public:
		virtual void print_rtl_op() = 0;
		virtual void print_asm_op() = 0;
};

class RTL_VarOperand : public RTL_Operand {
	public:
		string name;

		RTL_VarOperand(string name_) : name(name_) {}

		void print_rtl_op();
		void print_asm_op();
};

class RTL_IntOperand : public RTL_Operand {
	public:
		int val;

		RTL_IntOperand(int val_) : val(val_) {}

		void print_rtl_op();
		void print_asm_op();
};

class RTL_FloatOperand : public RTL_Operand {
	public:
		double val;

		RTL_FloatOperand(double val_) : val(val_) {}

		void print_rtl_op();
		void print_asm_op();
};

class RTL_StringOperand : public RTL_Operand {
	public:
		string str_const;

		RTL_StringOperand(string str_const_) : str_const(str_const_) {}

		void print_rtl_op();
		void print_asm_op();
};

class RTL_LabelOperand : public RTL_Operand {
	public:
		int lab_no;

		RTL_LabelOperand(int lab_no_) : lab_no(lab_no_) {}

		void print_rtl_op();
		void print_asm_op();
};

class RTL_RegisterOperand : public RTL_Operand {
	public:
		string reg_name;

		RTL_RegisterOperand(string reg_name_) : reg_name(reg_name_) {}

		void print_rtl_op();
		void print_asm_op();
};

class RTL_FuncOperand : public RTL_Operand {
	public:
		string func_name;

		RTL_FuncOperand(string func_name_) : func_name(func_name_) {}

		void print_rtl_op();
		void print_asm_op();
};

enum class RTL_Opcode : char {
	null, Add, Sub, Mul, Div, Uminus, Uminus_d, Mod, Neg, And, Or, Not, Gt, Ge, Lt, Le, Eq, Ne, Read, Write, Goto, Call, Return, Push, Pop, BGTZ, Label, Load, Load_i, Load_d, Load_a, Load_id, Move, Move_d, Movt, Movf, Store, Store_d 
};

class RTL_Statement {
	public:
		RTL_Opcode op;

		RTL_Statement(RTL_Opcode op_) : op(op_) {}
		virtual ~RTL_Statement() {}

		virtual void print_rtl_stmt() = 0;
		virtual void gen_asm_stmt() = 0;
};

class RTL_ComputeStatement : public RTL_Statement {
	public:
		shared_ptr<RTL_RegisterOperand> result_register;
		shared_ptr<RTL_RegisterOperand> lval;
		shared_ptr<RTL_RegisterOperand> rval;

		RTL_ComputeStatement(RTL_Opcode op_, shared_ptr<RTL_RegisterOperand> result_register_, shared_ptr<RTL_RegisterOperand> lval_, shared_ptr<RTL_RegisterOperand> rval_) : RTL_Statement(op_), result_register(result_register_), lval(lval_), rval(rval_) {}
		~RTL_ComputeStatement() {result_register.reset(); lval.reset(); rval.reset();}

		void print_rtl_stmt();
		void gen_asm_stmt();
};

class RTL_ControlStatement : public RTL_Statement {
	public:
		RTL_ControlStatement(RTL_Opcode op_) : RTL_Statement(op_) {}
		virtual ~RTL_ControlStatement() {}

		virtual void print_rtl_stmt() = 0;
		virtual void gen_asm_stmt() = 0;
};

class RTL_LabelStatement : public RTL_Statement {
	public:
		shared_ptr<RTL_LabelOperand> lab_op;

		RTL_LabelStatement(shared_ptr<RTL_LabelOperand> lab_op_) : RTL_Statement(RTL_Opcode::Label), lab_op(lab_op_) {}
		~RTL_LabelStatement() {lab_op.reset();}

		void print_rtl_stmt();
		void gen_asm_stmt();
};

class RTL_MoveStatement : public RTL_Statement {
	public:
		shared_ptr<RTL_Operand> target;
		shared_ptr<RTL_Operand> val;
		shared_ptr<RTL_IntOperand> aux_val;

		RTL_MoveStatement(RTL_Opcode op_, shared_ptr<RTL_Operand> target_, shared_ptr<RTL_Operand> val_, shared_ptr<RTL_IntOperand> aux_val_ = nullptr) : RTL_Statement(op_), target(target_), val(val_), aux_val(aux_val_) {}
		~RTL_MoveStatement() {target.reset(); val.reset(); aux_val.reset();}

		void print_rtl_stmt();
		void gen_asm_stmt();
};

class RTL_ReadStatement : public RTL_Statement {
	public:
		RTL_ReadStatement() : RTL_Statement(RTL_Opcode::Read) {}

		void print_rtl_stmt();
		void gen_asm_stmt();
};

class RTL_WriteStatement : public RTL_Statement {
	public:
		RTL_WriteStatement() : RTL_Statement(RTL_Opcode::Write) {}

		void print_rtl_stmt();
		void gen_asm_stmt();
};

class RTL_GotoStatement : public RTL_ControlStatement {
	public:
		shared_ptr<RTL_LabelOperand> lab_op;

		RTL_GotoStatement(shared_ptr<RTL_LabelOperand> lab_op_) : RTL_ControlStatement(RTL_Opcode::Goto), lab_op(lab_op_) {}
		~RTL_GotoStatement() {lab_op.reset();}

		void print_rtl_stmt();
		void gen_asm_stmt();
};

class RTL_IfGotoStatement : public RTL_ControlStatement {
	public:
		shared_ptr<RTL_Operand> cond;
		shared_ptr<RTL_LabelOperand> lab_op;

		RTL_IfGotoStatement(shared_ptr<RTL_Operand> cond_, shared_ptr<RTL_LabelOperand> lab_op_) : RTL_ControlStatement(RTL_Opcode::BGTZ), cond(cond_), lab_op(lab_op_) {}
		~RTL_IfGotoStatement() {cond.reset(); lab_op.reset();}

		void print_rtl_stmt();
		void gen_asm_stmt();
};

class RTL_CallStatement : public RTL_ControlStatement {
	public:
		shared_ptr<RTL_FuncOperand> func;
		shared_ptr<RTL_RegisterOperand> ret_reg;

		RTL_CallStatement(shared_ptr<RTL_FuncOperand> func_, shared_ptr<RTL_RegisterOperand> ret_reg_) : RTL_ControlStatement(RTL_Opcode::Call), func(func_), ret_reg(ret_reg_) {}
		~RTL_CallStatement() {func.reset(); ret_reg.reset();}

		void print_rtl_stmt();
		void gen_asm_stmt();
};

class RTL_ReturnStatement : public RTL_ControlStatement {
	public:
		shared_ptr<RTL_RegisterOperand> ret_reg;

		RTL_ReturnStatement(shared_ptr<RTL_RegisterOperand> ret_reg_) : RTL_ControlStatement(RTL_Opcode::Return), ret_reg(ret_reg_) {}
		~RTL_ReturnStatement() {ret_reg.reset();}

		void print_rtl_stmt();
		void gen_asm_stmt();
};

class RTL_PushStatement : public RTL_Statement {
	public:
		shared_ptr<RTL_RegisterOperand> push_reg;

		RTL_PushStatement(shared_ptr<RTL_RegisterOperand> push_reg_) : RTL_Statement(RTL_Opcode::Push), push_reg(push_reg_) {}
		~RTL_PushStatement() {push_reg.reset();}

		void print_rtl_stmt();
		void gen_asm_stmt();
};

class RTL_PopStatement : public RTL_Statement {
	public:
		bool is_double;

		RTL_PopStatement(bool is_double_) : RTL_Statement(RTL_Opcode::Pop), is_double(is_double_) {}

		void print_rtl_stmt();
		void gen_asm_stmt();
};

#endif
