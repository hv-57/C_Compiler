#include <cstring>
#include <fstream>
#include <iomanip>

bool show_tokens = false;
bool show_ast = false;
bool show_tac = false;
bool show_rtl = false;
bool show_asm = true;

bool sa_scan = false;
bool sa_parse = false;
bool sa_ast = false;
bool sa_tac = false;
bool sa_rtl = false;

FILE* inp_file;

std::string input_file_name;
std::ofstream tok_output_file;
std::ofstream ast_output_file;
std::ofstream tac_output_file;
std::ofstream rtl_output_file;
std::ofstream asm_output_file;

extern int yyparse();
extern int yylex();
extern int yylex_destroy();
extern void perform_cleanup();

void process_options(int argc, char* argv[]){
	for (int i = 1; i < argc; ++i){
		if (!strcmp(argv[i], "--sa-rtl"))	{sa_rtl = 1;}
		if (!strcmp(argv[i], "--sa-tac"))	{sa_rtl = 1; sa_tac = 1;}
		if (!strcmp(argv[i], "--sa-ast"))	{sa_rtl = 1; sa_tac = 1; sa_ast = 1;}
		if (!strcmp(argv[i], "--sa-parse"))	{sa_rtl = 1; sa_tac = 1; sa_ast = 1; sa_parse = 1;}
		if (!strcmp(argv[i], "--sa-scan"))	{sa_rtl = 1; sa_tac = 1; sa_ast = 1; sa_parse = 1; sa_scan = 1;}
		if (strncmp(argv[i], "--show", 6)) {
			input_file_name = std::string(argv[i]);
			inp_file = freopen(argv[i], "r", stdin);
		}
	}

	if ((show_asm = !sa_rtl)) {
		asm_output_file.open(input_file_name + ".spim");
		asm_output_file << std::fixed << std::setprecision(2) << std::flush;
	}

	for (int i = 1; i < argc; ++i){
		if (!strcmp(argv[i], "--show-rtl") && (show_rtl = !sa_tac)) {
			rtl_output_file.open(input_file_name + ".rtl");
			rtl_output_file << std::fixed << std::setprecision(2) << std::flush;
		}
		if (!strcmp(argv[i], "--show-tac") && (show_tac = !sa_ast)) {
			tac_output_file.open(input_file_name + ".tac");
			tac_output_file << std::fixed << std::setprecision(2) << std::flush;
		}
		if (!strcmp(argv[i], "--show-ast") && (show_ast = !sa_parse)) {
			ast_output_file.open(input_file_name + ".ast");
			ast_output_file << std::fixed << std::setprecision(2) << std::flush;
		}
		if (!strcmp(argv[i], "--show-tokens") && (show_tokens = 1)) {tok_output_file.open(input_file_name + ".toks");}
	}
}

void cleanup() {
	fclose(inp_file);
	if (tok_output_file.is_open()) tok_output_file.close();
	if (ast_output_file.is_open()) ast_output_file.close();
	if (tac_output_file.is_open()) tac_output_file.close();
	if (rtl_output_file.is_open()) rtl_output_file.close();
	perform_cleanup();
	yylex_destroy();
}

int main(int argc, char* argv[]) {
	process_options(argc, argv);

	if (sa_scan) while (yylex());
	else if (yyparse()) {cleanup(); return 1;}
	
	cleanup();
}
