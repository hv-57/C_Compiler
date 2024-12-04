#ifndef AST
#define AST

#include "tac.h"

using std::pair;

class AST_Expression {
	public:
		char d_type;
		vector<TAC_Statement*> tac_stmt_list;

		AST_Expression(char d_type_);
		virtual ~AST_Expression() {}

		virtual void print_ast_expr(string prefix) = 0;
		virtual pair<vector<TAC_Statement*>, shared_ptr<TAC_Operand>> gen_tac_expr() = 0;
};

class AST_VarExpression : public AST_Expression {
	public:
		string var_name;

		AST_VarExpression(string var_name_);

		void print_ast_expr(string prefix);
		pair<vector<TAC_Statement*>, shared_ptr<TAC_Operand>> gen_tac_expr();
};

class AST_IntExpression : public AST_Expression {
	public:
		int val;

		AST_IntExpression(int val_);

		void print_ast_expr(string prefix);
		pair<vector<TAC_Statement*>, shared_ptr<TAC_Operand>> gen_tac_expr();
};

class AST_FloatExpression : public AST_Expression {
	public:
		double val;

		AST_FloatExpression(double val_);

		void print_ast_expr(string prefix);
		pair<vector<TAC_Statement*>, shared_ptr<TAC_Operand>> gen_tac_expr();
};

class AST_StringExpression : public AST_Expression {
	public:
		string val;

		AST_StringExpression(string val_);

		void print_ast_expr(string prefix);
		pair<vector<TAC_Statement*>, shared_ptr<TAC_Operand>> gen_tac_expr();
};

class AST_UnaryExpression : public AST_Expression {
	public:
		string op;
		AST_Expression* expr;

		AST_UnaryExpression(string op_, AST_Expression* expr_);
		~AST_UnaryExpression() {delete expr;}

		void print_ast_expr(string prefix);
		pair<vector<TAC_Statement*>, shared_ptr<TAC_Operand>> gen_tac_expr();
};

class AST_BinaryExpression : public AST_Expression {
	public:
		string op;
		AST_Expression* left_expr;
		AST_Expression* right_expr;
		char binexp_type;

		AST_BinaryExpression(string op_, AST_Expression* left_expr_, AST_Expression* right_expr_, char binexp_type_);
		~AST_BinaryExpression() {delete left_expr; delete right_expr;}

		void print_ast_expr(string prefix);
		pair<vector<TAC_Statement*>, shared_ptr<TAC_Operand>> gen_tac_expr();
};

class AST_TernaryExpression : public AST_Expression {
	public:
		AST_Expression* cond_expr;
		AST_Expression* left_expr;
		AST_Expression* right_expr;

		AST_TernaryExpression(AST_Expression* cond_expr_, AST_Expression* left_expr_, AST_Expression* right_expr_);
		~AST_TernaryExpression() {delete cond_expr; delete left_expr; delete right_expr;}

		void print_ast_expr(string prefix);
		pair<vector<TAC_Statement*>, shared_ptr<TAC_Operand>> gen_tac_expr();
};

class AST_FuncExpression : public AST_Expression {
	public:
		string func_name;
		vector<AST_Expression*>* arg_list;

		AST_FuncExpression(string func_name_, vector<AST_Expression*>* arg_list_);
		~AST_FuncExpression() {for (auto arg : *arg_list) delete arg; delete arg_list;}

		void print_ast_expr(string prefix);
		pair<vector<TAC_Statement*>, shared_ptr<TAC_Operand>> gen_tac_expr();
};

class AST_Statement {
	public:
		AST_Statement() {}
		virtual ~AST_Statement() {}

		virtual void print_ast_stmt(string prefix) = 0;
		virtual vector<TAC_Statement*> gen_tac_stmt() = 0;
};

class AST_ReadStatement : public AST_Statement {
	public:
		string var_name;

		AST_ReadStatement(string var_name_);
		~AST_ReadStatement() {}

		void print_ast_stmt(string prefix);
		vector<TAC_Statement*> gen_tac_stmt();
};

class AST_PrintStatement : public AST_Statement {
	public:
		AST_Expression* expr;

		AST_PrintStatement(AST_Expression* expr_);
		~AST_PrintStatement() {delete expr;}

		void print_ast_stmt(string prefix);
		vector<TAC_Statement*> gen_tac_stmt();
};

class AST_AssignStatement : public AST_Statement {
	public:
		string var_name;
		AST_Expression* expr;

		AST_AssignStatement(string var_name_, AST_Expression* expr_);
		~AST_AssignStatement() {delete expr;}

		void print_ast_stmt(string prefix);
		vector<TAC_Statement*> gen_tac_stmt();
};

class AST_IfElseStatement : public AST_Statement {
	public:
		AST_Expression* expr;
		AST_Statement* if_stmt;
		AST_Statement* else_stmt;

		AST_IfElseStatement(AST_Expression* expr_, AST_Statement* if_stmt_, AST_Statement* else_stmt_);
		~AST_IfElseStatement() {delete expr; delete if_stmt; delete else_stmt;}

		void print_ast_stmt(string prefix);
		vector<TAC_Statement*> gen_tac_stmt();
};

class AST_WhileStatement : public AST_Statement {
	public:
		AST_Expression* expr;
		AST_Statement* stmt;

		AST_WhileStatement(AST_Expression* expr_, AST_Statement* stmt_);
		~AST_WhileStatement() {delete expr; delete stmt;}

		void print_ast_stmt(string prefix);
		vector<TAC_Statement*> gen_tac_stmt();
};

class AST_DoWhileStatement : public AST_Statement {
	public:
		AST_Expression* expr;
		AST_Statement* stmt;

		AST_DoWhileStatement(AST_Expression* expr_, AST_Statement* stmt_);
		~AST_DoWhileStatement() {delete expr; delete stmt;}

		void print_ast_stmt(string prefix);
		vector<TAC_Statement*> gen_tac_stmt();
};

class AST_CompoundStatement : public AST_Statement {
	public:
		vector<AST_Statement*>* stmt_list;

		AST_CompoundStatement();
		~AST_CompoundStatement() {for (auto stmt : *stmt_list) delete stmt; delete stmt_list;}

		void print_ast_stmt(string prefix);
		vector<TAC_Statement*> gen_tac_stmt();
};

class AST_CallStatement : public AST_Statement {
	public:
		string lval;
		AST_FuncExpression* expr;

		AST_CallStatement(string lval_, AST_FuncExpression* expr_);
		~AST_CallStatement() {delete expr;}

		void print_ast_stmt(string prefix);
		vector<TAC_Statement*> gen_tac_stmt();
};

class AST_ReturnStatement : public AST_Statement {
	public:
		AST_Expression* expr;

		AST_ReturnStatement(AST_Expression* expr_);
		~AST_ReturnStatement() {delete expr;}

		void print_ast_stmt(string prefix);
		vector<TAC_Statement*> gen_tac_stmt();
};

#endif
