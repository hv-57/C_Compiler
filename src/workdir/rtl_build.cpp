#include "function.h"

inline shared_ptr<RTL_RegisterOperand> int_use_list[] = {
	shared_ptr<RTL_RegisterOperand>(new RTL_RegisterOperand("v0")),
	shared_ptr<RTL_RegisterOperand>(new RTL_RegisterOperand("t0")),
	shared_ptr<RTL_RegisterOperand>(new RTL_RegisterOperand("t1")),
	shared_ptr<RTL_RegisterOperand>(new RTL_RegisterOperand("t2")),
	shared_ptr<RTL_RegisterOperand>(new RTL_RegisterOperand("t3")),
	shared_ptr<RTL_RegisterOperand>(new RTL_RegisterOperand("t4")),
	shared_ptr<RTL_RegisterOperand>(new RTL_RegisterOperand("t5")),
	shared_ptr<RTL_RegisterOperand>(new RTL_RegisterOperand("t6")),
	shared_ptr<RTL_RegisterOperand>(new RTL_RegisterOperand("t7")),
	shared_ptr<RTL_RegisterOperand>(new RTL_RegisterOperand("t8")),
	shared_ptr<RTL_RegisterOperand>(new RTL_RegisterOperand("t9")),
	shared_ptr<RTL_RegisterOperand>(new RTL_RegisterOperand("s0")),
	shared_ptr<RTL_RegisterOperand>(new RTL_RegisterOperand("s1")),
	shared_ptr<RTL_RegisterOperand>(new RTL_RegisterOperand("s2")),
	shared_ptr<RTL_RegisterOperand>(new RTL_RegisterOperand("s3")),
	shared_ptr<RTL_RegisterOperand>(new RTL_RegisterOperand("s4")),
	shared_ptr<RTL_RegisterOperand>(new RTL_RegisterOperand("s5")),
	shared_ptr<RTL_RegisterOperand>(new RTL_RegisterOperand("s6")),
	shared_ptr<RTL_RegisterOperand>(new RTL_RegisterOperand("s7")),
};

inline shared_ptr<RTL_RegisterOperand> double_use_list[] = {
	shared_ptr<RTL_RegisterOperand>(new RTL_RegisterOperand("f2")),
	shared_ptr<RTL_RegisterOperand>(new RTL_RegisterOperand("f4")),
	shared_ptr<RTL_RegisterOperand>(new RTL_RegisterOperand("f6")),
	shared_ptr<RTL_RegisterOperand>(new RTL_RegisterOperand("f8")),
	shared_ptr<RTL_RegisterOperand>(new RTL_RegisterOperand("f10")),
	shared_ptr<RTL_RegisterOperand>(new RTL_RegisterOperand("f12")),
	shared_ptr<RTL_RegisterOperand>(new RTL_RegisterOperand("f14")),
	shared_ptr<RTL_RegisterOperand>(new RTL_RegisterOperand("f16")),
	shared_ptr<RTL_RegisterOperand>(new RTL_RegisterOperand("f18")),
	shared_ptr<RTL_RegisterOperand>(new RTL_RegisterOperand("f20")),
	shared_ptr<RTL_RegisterOperand>(new RTL_RegisterOperand("f22")),
	shared_ptr<RTL_RegisterOperand>(new RTL_RegisterOperand("f24")),
	shared_ptr<RTL_RegisterOperand>(new RTL_RegisterOperand("f26")),
	shared_ptr<RTL_RegisterOperand>(new RTL_RegisterOperand("f28")),
	shared_ptr<RTL_RegisterOperand>(new RTL_RegisterOperand("f30")),
};

inline shared_ptr<RTL_RegisterOperand> v0_reg = int_use_list[0];
inline shared_ptr<RTL_RegisterOperand> v1_reg = shared_ptr<RTL_RegisterOperand>(new RTL_RegisterOperand("v1"));
inline shared_ptr<RTL_RegisterOperand> zero_reg = shared_ptr<RTL_RegisterOperand>(new RTL_RegisterOperand("zero"));
inline shared_ptr<RTL_RegisterOperand> f0_reg = shared_ptr<RTL_RegisterOperand>(new RTL_RegisterOperand("f0"));
inline shared_ptr<RTL_RegisterOperand> a0_reg = shared_ptr<RTL_RegisterOperand>(new RTL_RegisterOperand("a0"));
inline shared_ptr<RTL_RegisterOperand> f12_reg = double_use_list[5];

bool int_free_list[] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
bool double_free_list[] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

int write_syscall(char type) {
	switch(type) {
		case 'i' : return 1;
		case 'f' : return 3;
		case 's' : return 4;
	}
	return -1;
}

shared_ptr<RTL_RegisterOperand> get_free_reg(bool is_d) {
	if (!is_d) {for (int i = 0; i < 19; ++i) if (int_free_list[i]) {int_free_list[i] = 0; return int_use_list[i];}}
	else {for (int i = 0; i < 15; ++i) if (double_free_list[i]) {double_free_list[i] = 0; return double_use_list[i];}}
	raise_error("Fatal Error: No more free", (is_d ? "double" : "integer"), "registers available");
	__builtin_unreachable();
}

void free_reg(shared_ptr<RTL_RegisterOperand> to_free) {
	for (int i = 0; i < 19; ++i) if (to_free == int_use_list[i]) {int_free_list[i] = 1; return;}
	for (int i = 0; i < 15; ++i) if (to_free == double_use_list[i]) {double_free_list[i] = 1; return;}
}

string op_map(string op, bool inv = 0) {
	if (op == "+") return "add";
	if (op == "*") return "mul";
	if (op == "/") return "div";
	if (op == "-") return "sub";
	if (op == "!") return "not";
	if (op == "||") return "or";
	if (op == "&&") return "and";
	if (op == "<") return "slt";
	if (op == "<=") return "sle";
	if (op == "==") return "seq";
	if (op == ">") return inv ? "sle" : "sgt";
	if (op == ">=") return inv ? "slt" : "sge";
	if (op == "!=") return inv ? "seq" : "sne";
	return op;
}

shared_ptr<RTL_Operand> TAC_IntOperand::gen_rtl_op() {return shared_ptr<RTL_Operand>(new RTL_IntOperand(val));}
shared_ptr<RTL_Operand> TAC_FloatOperand::gen_rtl_op() {return shared_ptr<RTL_Operand>(new RTL_FloatOperand(val));}
shared_ptr<RTL_Operand> TAC_StringOperand::gen_rtl_op() {return shared_ptr<RTL_Operand>(new RTL_StringOperand(str_const));}
shared_ptr<RTL_Operand> TAC_VarOperand::gen_rtl_op() {return shared_ptr<RTL_Operand>(new RTL_VarOperand(name));}
shared_ptr<RTL_Operand> TAC_TempOperand::gen_rtl_op() {return reg;}
shared_ptr<RTL_Operand> TAC_FuncOperand::gen_rtl_op() {__builtin_unreachable();}

RTL_Statement* TAC_IntOperand::gen_rtl_stmt() {return new RTL_MoveStatement(RTL_Opcode::Load_i, reg = get_free_reg(0), shared_ptr<RTL_Operand>(new RTL_IntOperand(val)));}
RTL_Statement* TAC_FloatOperand::gen_rtl_stmt() {return new RTL_MoveStatement(RTL_Opcode::Load_id, reg = get_free_reg(1), shared_ptr<RTL_Operand>(new RTL_FloatOperand(val)));}
RTL_Statement* TAC_StringOperand::gen_rtl_stmt() {return new RTL_MoveStatement(RTL_Opcode::Load_a, reg = get_free_reg(0), shared_ptr<RTL_Operand>(new RTL_StringOperand(str_const)));}
RTL_Statement* TAC_VarOperand::gen_rtl_stmt() {return new RTL_MoveStatement((d_type == 'f') ? RTL_Opcode::Load_d : RTL_Opcode::Load, reg = get_free_reg(d_type == 'f'), shared_ptr<RTL_Operand>(new RTL_VarOperand(name)));}
RTL_Statement* TAC_TempOperand::gen_rtl_stmt() {return nullptr;}
RTL_Statement* TAC_FuncOperand::gen_rtl_stmt() {return nullptr;}

vector<RTL_Statement*> TAC_ComputeStatement::gen_rtl_stmt() {
	vector<RTL_Statement*> rtl_code;

	if (!l_op) {
		rtl_code += r_op -> gen_rtl_stmt();

		if (op == "-") rtl_code += new RTL_ComputeStatement((res -> d_type == 'f') ? RTL_Opcode::Uminus_d : RTL_Opcode::Uminus, res -> reg = get_free_reg(res -> d_type == 'f'), nullptr, r_op -> reg);
		if (op == "!") rtl_code += new RTL_ComputeStatement(RTL_Opcode::Not, res -> reg = get_free_reg(0), nullptr, r_op -> reg);

		free_reg(r_op -> reg);
	}ss
	else {
		if (!(l_op -> d_type == 'f') || (op == "+" || op == "-" || op == "*" || op == "/")) {
			rtl_code += l_op -> gen_rtl_stmt();
			res -> reg = get_free_reg(l_op -> d_type == 'f');
			rtl_code += r_op -> gen_rtl_stmt();

			rtl_code += new RTL_ComputeStatement(op_map(op) + (l_op -> d_type == 'f' ? ".d" : ""), res -> reg, l_op -> reg, r_op -> reg);

			free_reg(l_op -> reg);
			free_reg(r_op -> reg);
		}
		else {
			rtl_code += l_op -> gen_rtl_stmt();
			rtl_code += r_op -> gen_rtl_stmt();

			auto loader = get_free_reg(0);
			res -> reg = get_free_reg(0);

			bool inv_op = op == ">=" || op == ">" || op == "!=";

			rtl_code += new RTL_ComputeStatement(op_map(op, inv_op) + ".d", nullptr, l_op -> reg, r_op -> reg);
			rtl_code += new RTL_MoveStatement(RTL_Opcode::Load_i, loader, shared_ptr<RTL_Operand>(new RTL_IntOperand(1)));
			rtl_code += new RTL_MoveStatement(RTL_Opcode::Move, res -> reg, zero_reg);
			rtl_code += new RTL_MoveStatement(inv_op ? RTL_Opcode::Movf : RTL_Opcode::Movt, res -> reg, loader, shared_ptr<RTL_IntOperand>(new RTL_IntOperand(0)));

			free_reg(l_op -> reg);
			free_reg(r_op -> reg);
			free_reg(loader);
		}
	}
	return rtl_code;
}

vector<RTL_Statement*> TAC_AssignStatement::gen_rtl_stmt() {
	vector<RTL_Statement*> rtl_code;

	rtl_code += rval -> gen_rtl_stmt();
	rtl_code += new RTL_MoveStatement((rval -> d_type == 'f') ? RTL_Opcode::Store_d : RTL_Opcode::Store, shared_ptr<RTL_Operand>(new RTL_VarOperand(lval -> name)), rval -> reg);

	free_reg(rval -> reg);

	return rtl_code;
}

vector<RTL_Statement*> TAC_GotoStatement::gen_rtl_stmt() {
	return {new RTL_GotoStatement(shared_ptr<RTL_LabelOperand>(new RTL_LabelOperand(label -> lab_no)))};
}

vector<RTL_Statement*> TAC_IfGotoStatement::gen_rtl_stmt() {
	vector<RTL_Statement*> rtl_code;

	rtl_code += if_cond -> gen_rtl_stmt();
	rtl_code += new RTL_IfGotoStatement(if_cond -> reg, shared_ptr<RTL_LabelOperand>(new RTL_LabelOperand(label -> lab_no)));

	free_reg(if_cond -> reg);

	return rtl_code;
}

vector<RTL_Statement*> TAC_LabelStatement::gen_rtl_stmt() {
	return {new RTL_LabelStatement(shared_ptr<RTL_LabelOperand>(new RTL_LabelOperand(label -> lab_no)))};
}

vector<RTL_Statement*> TAC_ReadStatement::gen_rtl_stmt() {
	vector<RTL_Statement*> rtl_code;

	bool is_f = var -> d_type == 'f';

	rtl_code += new RTL_MoveStatement(RTL_Opcode::Load_i, v0_reg, shared_ptr<RTL_Operand>(new RTL_IntOperand(is_f ? 7 : 5)));
	rtl_code += new RTL_ReadStatement();
	rtl_code += new RTL_MoveStatement(is_f ? RTL_Opcode::Store_d : RTL_Opcode::Store, shared_ptr<RTL_Operand>(new RTL_VarOperand(var -> name)), is_f ? f0_reg : v0_reg);

	return rtl_code;
}

vector<RTL_Statement*> TAC_WriteStatement::gen_rtl_stmt() {
	vector<RTL_Statement*> rtl_code;

	if (!int_free_list[0]) {
		shared_ptr<RTL_RegisterOperand> place_holder = get_free_reg(0);
		rtl_code += new RTL_MoveStatement(RTL_Opcode::Move, place_holder, v0_reg);
		rtl_code += new RTL_MoveStatement(RTL_Opcode::Load_i, v0_reg, shared_ptr<RTL_Operand>(new RTL_IntOperand(1)));
		rtl_code += new RTL_MoveStatement(RTL_Opcode::Move, a0_reg, place_holder);
		rtl_code += new RTL_WriteStatement();

		free_reg(place_holder);
		free_reg(v0_reg);
	}
	else {
		rtl_code += new RTL_MoveStatement(RTL_Opcode::Load_i, v0_reg, shared_ptr<RTL_Operand>(new RTL_IntOperand(write_syscall(val -> d_type))));
		rtl_code += new RTL_MoveStatement(val -> rtl_instr, (val -> d_type == 'f') ? f12_reg : a0_reg, val -> gen_rtl_op());
		rtl_code += new RTL_WriteStatement();

		free_reg(val -> reg);
	}
	return rtl_code;
}

vector<RTL_Statement*> TAC_CallStatement::gen_rtl_stmt() {
	vector<RTL_Statement*> rtl_code;

	auto ret_reg = lval ? lval -> d_type == 'f' ? f0_reg : v1_reg : nullptr;

	for (int i = func -> args.size() - 1; i >= 0; --i) {
		rtl_code += func -> args[i] -> gen_rtl_stmt();
		rtl_code += new RTL_PushStatement(func -> args[i] -> reg);
		free_reg(func -> args[i] -> reg);
	}

	rtl_code += new RTL_CallStatement(shared_ptr<RTL_FuncOperand>(new RTL_FuncOperand(func -> func_name)), ret_reg);

	for (auto arg : func -> args) rtl_code += new RTL_PopStatement(arg -> d_type == 'f');

	if (lval) rtl_code += new RTL_MoveStatement((lval -> d_type == 'f') ? RTL_Opcode::Move_d : RTL_Opcode::Move, lval -> reg = get_free_reg(lval -> d_type == 'f'), ret_reg);

	return rtl_code;
}

vector<RTL_Statement*> TAC_ReturnStatement::gen_rtl_stmt() {
	vector<RTL_Statement*> rtl_code;

	rtl_code += new RTL_MoveStatement(ret_var -> rtl_instr, (ret_var -> d_type == 'f') ? f0_reg : v1_reg, ret_var -> gen_rtl_op());
	rtl_code += new RTL_ReturnStatement((ret_var -> d_type == 'f') ? f0_reg : v1_reg);

	return rtl_code;
}