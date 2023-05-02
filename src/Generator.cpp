#include"Node.h"
#include"Generator.h"
#include<string>
std::map<string, int> Generator::TYPE_WIDTH {
	{"INT",4},
	{"DOUBLE",8},
	{"FLOAT",4}
};

Generator& Generator::GetInstance()
{
	static Generator generator;
	return generator;
	// TODO: 在此处插入 return 语句
}

void Generator::analyze(IR* ir, const GrammarEntry* rule, Node* root) {
	Statement(ir, rule, root);
	Assignment(ir, rule, root);
	BoolExpression(ir, rule, root);
	Iteration(ir, rule, root);
	Mquad(ir, rule, root);
	Function(ir, rule, root);
	Selection(ir, rule, root);
}

string IR::newtemp() {
	string temp = "T" + std::to_string(tempCnt);
	++tempCnt;
	return temp;
}

void Generator::BoolExpression(IR* ir, const GrammarEntry* rule, Node* root)
{
	auto state = rule->state;
	auto symbols = rule->symbols;

	auto& quads = ir->quads;

	if (root->children.size() == 2)
	{
		if (state.type == "unary_expression" && symbols[0].type == "unary_operator" && symbols[1].type == "cast_expression")
		{
			if (root->children[0]->children[0]->type == "!") {
				root->place = ir->newtemp();
				quads.push_back(Quad("!", root->children[1]->place, ir->QUAD_EMPTY, root->place));
			}
		}
	}
	else if (root->children.size() == 3)
	{
		if (state.type == "relational_expression" && symbols[0].type == "relational_expression" && symbols[2].type == "shift_expression")
		{
			root->place = ir->newtemp();
			quads.push_back(Quad(symbols[1].type, root->children[0]->place, root->children[2]->place, root->place));
		}
		else if (state.type == "equality_expression" && symbols[0].type == "equality_expression" && symbols[2].type == "relational_expression")
		{
			root->place = ir->newtemp();
			quads.push_back(Quad(symbols[1].type, root->children[0]->place, root->children[2]->place, root->place));
		}
	}
	else if (root->children.size() == 4)
	{
		Node* e1 = root->children[0], * e2 = root->children[3], * m = root->children[1];
		if (state.type == "logical_and_expression" && symbols[1].type == "AND_OP")
		{
			root->place = ir->newtemp();
			quads.push_back(Quad("AND_OP", e1->place, e2->place, root->place));
		}
		else if (state.type == "logical_or_expression" && symbols[1].type == "OR_OP")
		{
			root->place = ir->newtemp();
			quads.push_back(Quad("OR_OP", e1->place, e2->place, root->place));
		}
	}
}

void getParameterList(Node* root, vector<string>& result) {
	if (root->type != "parameter_list") {
		return;
	}
	Node* para_decl = nullptr;
	if (root->children.size() == 3) {
		getParameterList(root->children[0], result);
		para_decl = root->children[2];
	}
	else {
		para_decl = root->children[0];
	}
	result.push_back(para_decl->children[0]->var_type);
}

void Generator::Function(IR* ir, const GrammarEntry* rule, Node* root)
{
	const string& lhs = rule->state.type; 
	if (lhs == "m_func_enter") {
		// 进入函数定义阶段，创建函数表，但此时无名称（无法获得函数名）
		auto newTable = mktable(ir->curTable);
		ir->curTable = newTable;
	}
	else if (lhs == "direct_declarator" && rule->symbols.size() == 4) {
		// direct_declarator -> direct_declarator ( parameter_type_list )
		// register a function entry 
		const auto& rhs = rule->symbols;
		string func_name = root->children[0]->place;
		FunctionEntry entry; 
		// parameters
		Node* parameter_list = root->children[2]->children[0];
		vector<string> parameters;
		getParameterList(parameter_list, parameters);
		entry.args = parameters;
		entry.addr = ir->nextquad(); // 函数标号地址

		ir->functionTable->insert({ func_name,entry });
	}
	else if (lhs == "function_definition" && rule->symbols.size() == 4) {
		const auto& rhs = rule->symbols; 
		if (rhs[0].type == "declaration_specifiers") {
			// function_definition -> declaration_spec declarator compound_statement
			// 退出当前函数表

			// 获取函数名（作用域名）
			// function_definition -> declarator -> direct_declarator -> direc_declarator
			string func_name = root->children[2]->children[0]->children[0]->place; 
			// add to function table 
			ir->functionTable->operator[](func_name).ret_type = root->children[0]->var_type;
			// add to symbol tables 
			ir->symbolTables.insert({func_name, ir->curTable});
			// change current table. 	
			ir->curTable = ir->curTable->parent; 
		}
	}
}

void Generator::Statement(IR* ir, const GrammarEntry* rule, Node* root) {
	auto& curTable = ir->curTable;
	auto& quads = ir->quads;
	auto QUAD_EMPTY = ir->QUAD_EMPTY;

	if (rule->state.type == "type_specifier") {
		root->var_type = rule->symbols[0].type;
		root->width = root->children[0]->width;
	}
	else if (rule->state.type == "declaration_specifiers"){
		if (root->children[0]->type == "type_specifier") {
			root->var_type = root->children[0]->var_type;
			root->width = root->children[0]->width;
		}
	}
	else if (rule->state.type == "parameter_declaration" && rule->symbols.size() == 2) {
		// parameter_declaration -> declaration_specifiers declarator
		auto specifier = root->children[0];
		string var_type = specifier->var_type;
		enter(curTable, root->children[1]->place, var_type, curTable->offset);
		addWidth(curTable, Generator::TYPE_WIDTH[var_type]);
	}
	else if (rule->state.type == "declaration") {
		if (rule->symbols.size() == 1) {
			// declaration = declaration_specifiers ; 
			root->var_type = root->children[0]->var_type;
		}
		else {
			// declaration = declaration_specifers init_declarator_list ; 
			// add to symbol stack
			enter(curTable, root->children[1]->place,root->children[0]->var_type,curTable->offset);
			int width = 0; 
			if (root->children[0]->var_type == "DOUBLE") {
				width = 8;
			}
			else if(root->children[0]->var_type == "INT") {
				width = 4;
			}
			addWidth(curTable, width);
		}
	}
	else if (rule->state.type == "init_declarator_list") {
		if (rule->symbols.size() == 1) {
			//root-> type
			root->place = root->children[0]->place;
		}
	}
	else if (rule->state.type == "initializer") {
		if (rule->symbols.size() == 1) {
			root->place = root->children[0]->place;
			root->var_type = root->children[0]->var_type;
		}
		else {
			// initializer = { init_list } 
		}
	}
	else if (rule->state.type == "init_declarator") {
		if (rule->symbols.size() == 1) {
			//init_declarator = declarator
			root->place = root->children[0]->place;
			root->var_type = root->children[0]->var_type;
		}
		else {
			// init_declarator => declarator = initialzier
			root->place = root->children[0]->place;
			quads.push_back({ "=",root->children[2]->place,QUAD_EMPTY,root->children[0]->place });
		}
	}
	else if (rule->state.type == "direct_declarator") {
		if (rule->symbols.size() == 1) {
			// direct = identifier
			root->place = root->children[0]->place;
		}
	}
	else if (rule->state.type == "declarator") {
		if (rule->symbols.size() == 1) {
			root->place = root->children[0]->place;
		}
	}
	else if (rule->state.type == "assignment_operator") {
		root->place = root->children[0]->place;
	}
}
void Generator::Assignment(IR* ir, const GrammarEntry* rule,Node* root) {

	//auto& symbolTableStack = ir->symbolTableStack;
	auto curTable = ir->curTable;
	auto& quads = ir->quads;
	auto QUAD_EMPTY = ir->QUAD_EMPTY;
// 最高级为 assignment_expression
	if (rule->state.type == "expression") {
		if (rule->symbols.size() == 1) {
			// exp = assign_exp 
			root->place = root->children[0]->place;
			root->var_type = root->children[0]->var_type;
		}
		else {
			// exp = exp , assign_exp 
		}
	}
	else if (rule->state.type == "assignment_expression") {
		if (rule->symbols.size() == 1) {
			// assign = conditional 
			root->place = root->children[0]->place;
			root->var_type = root->children[0]->var_type;
		}
		else {
			// assign = unary assign_op assign_expression
			root->place = root->children[0]->place;
			auto symbols= curTable->symbols;
			if (symbols.find(root->place) == symbols.end()) {
				// not found! 
				std::cout << "ERROR: variable " << root->place << " is referred before declaration!\n";
			}
			else {
				auto var_type = symbols[root->place].type;
				if (var_type != root->children[2]->var_type) {
					;
				}
			}
			quads.push_back({ root->children[1]->place,root->children[2]->place,QUAD_EMPTY,root->place });
		}
	}
	else if (rule->state.type == "inclusive_or_expression" || rule->state.type == "logical_and_expression" || rule->state.type == "logicial_and_expression"
		|| rule->state.type == "logical_or_expression" || rule->state.type == "conditional_expression" ) {
		root->place = root->children[0]->place;
		root->var_type = root->children[0]->var_type;

		if (rule->symbols.size() > 1) {
			std::cout << "use other rules\n";
		}
	}
	else if (rule->state.type == "exclusive_or_expression") {
		if (rule->symbols.size() == 1) {
			// shift = add 
			root->place = root->children[0]->place;
			root->var_type = root->children[0]->var_type;
		}
		else {
			std::cout << "Exclusive_or_expression use other rules" << "\n";
		}
	}
	else if (rule->state.type == "and_expression") {
		if (rule->symbols.size() == 1) {
			// and = equality
			root->place = root->children[0]->place;
			root->var_type = root->children[0]->var_type;
		}
		else {
			std::cout << "And expression use other rules" << "\n";
		}
	}
	else if (rule->state.type == "equality_expression") {
		if (rule->symbols.size() == 1) {
			// equality = relational
			root->place = root->children[0]->place;
			root->var_type = root->children[0]->var_type;
		}
		else {
			std::cout << "equality expression use other rules" << "\n";
		}
	}
	else if (rule->state.type == "relational_expression") {
		if (rule->symbols.size() == 1) {
			// relational = shift
			root->place = root->children[0]->place;
			root->var_type = root->children[0]->var_type;
		}
		else {
			std::cout << "Relational expression use other rules" << "\n";
		}
	}
	else if (rule->state.type == "shift_expression") {
		if (rule->symbols.size() == 1) {
			// shift = add 
			root->place = root->children[0]->place;
			root->var_type = root->children[0]->var_type;
		}
		else {
			std::cout << "Shift expression use other rules" << "\n";
		}
	}
	else if (rule->state.type == "additive_expression") {
		if (rule->symbols.size() == 3) {
			// additive_expression = additive_expression +/- multiplicative_expression
			root->place = ir->newtemp();
			if (root->children[0]->var_type == "DOUBLE" || root->children[2]->var_type == "DOUBLE") {
				root->var_type = "DOUBLE";
			}
			else {
				root->var_type = "INT";
			}
			quads.push_back({ root->children[1]->place,root->children[0]->place,root->children[2]->place,root->place });
		}
		else if (rule->symbols.size() == 1) {
			// add = mult
			root->var_type = root->children[0]->var_type;
			root->place = root->children[0]->place;
		}
	}
	else if (rule->state.type == "multiplicative_expression") {
		if (rule->symbols.size() == 1) {
			// mult = cast
			root->place = root->children[0]->place;
			root->var_type = root->children[0]->var_type;
		}
		else {
			// mult = mult * cast
			root->place = ir->newtemp();
			if (root->children[0]->var_type == "DOUBLE" || root->children[2]->var_type == "DOUBLE") {
				root->var_type = "DOUBLE";
			}
			else {
				root->var_type = "INT";
			}
			quads.push_back({ root->children[1]->place,root->children[0]->place,root->children[2]->place,root->place });
		}
	}
	else if (rule->state.type == "cast_expression") {
		if (rule->symbols.size() == 1) {
			// cast  = unary 
			root->place = root->children[0]->place;
			root->var_type = root->children[0]->var_type;
		}
		else {
			//cast = (typename) cast_expression
			root->place = ir->newtemp();
			quads.push_back({ "type_cast",root->children[1]->place,root->children[3]->place,root->place });
			root->var_type = root->children[1]->place;
		}
	}
	else if (rule->state.type == "unary_expression") {
		if (rule->symbols.size() == 1) {
			// unary = postfix 
			root->place = root->children[0]->place;
			root->var_type = root->children[0]->var_type;
		}
		else {
			std::cout << "Unary_expression use other rules" << '\n';
		}
	}
	else if (rule->state.type == "unary_operator") {
		root->place = root->children[0]->place;
		root->var_type = root->children[0]->var_type;
	}
	else if (rule->state.type == "postfix_expression") {
		if (rule->symbols.size() == 1) {
			// postfix = primary 
			root->place = root->children[0]->place;
			root->var_type = root->children[0]->var_type;
		}
		else {
			std::cout << "Postfix_expression use other rules" << '\n';
		}
	}
	else if (rule->state.type == "primary_expression") {
		if (rule->symbols[0].type == "IDENTIFIER") {
			root->place = root->children[0]->place;
			auto symbols = curTable->symbols;
			if (symbols.find(root->place) == symbols.end()) {
				// not found! 
				std::cout << "ERROR: variable " << root->place << " is referred before declaration!\n";
			}
			else {
				auto var_type = symbols[root->place].type;
				root->var_type = var_type;
			}
		} 
		else if (rule->symbols[0].type == "CONSTANT") {
			root->place = root->children[0]->place;
			root->var_type = root->children[0]->var_type;
		}
		else {
			std::cout << "Primary_expression use other rules" << '\n';
		}
	}
}

void Generator::output(IR* ir, const string& filename) {

	//auto& symbolTableStack = ir->symbolTableStack;
	auto curTable = ir->curTable;
	auto& quads = ir->quads;
	auto QUAD_EMPTY = ir->QUAD_EMPTY;
	std::ofstream fout(filename);

	for (int i = 0; i < quads.size(); ++i) {
		const auto& quad = quads[i];
        fout << "(" << i + 100<<")" << " : (" << quad.op << " ," << quad.arg1 << " ," << quad.arg2 << " ," << quad.dst << ")\n";
	}

	fout << "-----------------------\n";
	fout << "Symbol Table:\n";
	//auto table = curTable;
	for(auto& iter : ir->symbolTables){
		fout << "Table " << iter.first << " : \n";
		auto table = iter.second; 
		fout << "offset: " << table->offset << "\n";
		fout << "width: " << table->width << "\n";
		for (auto iter = table->symbols.begin(); iter != table->symbols.end(); ++iter) {
			fout << "[" << iter->first << ", " << iter->second.offset << ", " << iter->second.type << "]\n";
		}
	}
	fout << "GLobal Symbol Table: \n";
	auto table = ir->globalTable; 
	fout << "offset: " << table->offset << "\n";
	fout << "width: " << table->width << "\n";
	for (auto iter = table->symbols.begin(); iter != table->symbols.end(); ++iter) {
		fout << "[" << iter->first << ", " << iter->second.offset << ", " << iter->second.type << "]\n";
	}
	fout << "-----------------------\n";
	fout << "Function Table: \n";
	auto functionTable = ir->functionTable;
	for (auto iter : *functionTable) {
		auto entry = iter.second;
		fout << iter.first << " : " << "addr: " << entry.addr << " type: " << entry.ret_type << "(";
		for (auto arg : entry.args) {
			fout << arg << ", ";
		}
		fout << ") \n";
	}
}
void Generator::Iteration(IR* ir, const GrammarEntry* rule, Node* root) {

		//auto& symbolTableStack = ir->symbolTableStack;
	auto curTable = ir->curTable;
	auto& quads = ir->quads;
	auto QUAD_EMPTY = ir->QUAD_EMPTY;
if (rule->state.type != "iteration_statement")
		return;
	if (rule->symbols[0].type == "WHILE") {
		// WHILE m_quad ( expression ) m_quad_jz statement
		quads.push_back({ "j", QUAD_EMPTY, QUAD_EMPTY, root->children[1]->place });
		int sbegin = std::stoi(root->children[5]->place) - ir->QUAD_BEGIN;
		quads[sbegin].arg1 = root->children[3]->place;
		quads[sbegin].dst = std::to_string(ir->nextquad());
	}
	else if (rule->symbols[0].type == "DO") {
		// DO m_quad statement WHILE ( expression ) ;
		quads.push_back({ "jnz", root->children[5]->place, QUAD_EMPTY, root->children[1]->place });
	}
	else if (rule->symbols[0].type == "FOR" && rule->symbols.size() == 11) {
		// FOR ( expression_statement m_quad expression_statement m_quad_jz m_quad_j expression ) m_quad_j statement
		int pupdate = std::stoi(root->children[6]->place) + 1;
		quads.push_back({ "j", QUAD_EMPTY, QUAD_EMPTY, std::to_string(pupdate) });
		int pexpjz = std::stoi(root->children[5]->place) - ir->QUAD_BEGIN;
		int pstmt = std::stoi(root->children[9]->place) + 1;
		quads[pexpjz].arg1 = root->children[4]->place;
		quads[pexpjz].dst = std::to_string(ir->nextquad());
		quads[pexpjz + 1].dst = std::to_string(pstmt);
		int pupdj = std::stoi(root->children[9]->place) - ir->QUAD_BEGIN;
		quads[pupdj].dst = root->children[3]->place;
	}
	else {
		std::cout << "iteration_statement use other rules" << '\n';
	}
}

void Generator::Mquad(IR* ir, const GrammarEntry* rule, Node* root) {

		//auto& symbolTableStack = ir->symbolTableStack;
	auto curTable = ir->curTable;
	auto& quads = ir->quads;
	auto QUAD_EMPTY = ir->QUAD_EMPTY;
if (rule->state.type == "m_quad") {
		root->place = std::to_string(ir->nextquad());
	}
	if (rule->state.type == "m_quad_jz") {
		root->place = std::to_string(ir->nextquad());
		quads.push_back({ "jz", QUAD_EMPTY, QUAD_EMPTY, ""});
	}
	if (rule->state.type == "m_quad_j") {
		root->place = std::to_string(ir->nextquad());
		quads.push_back({ "j", QUAD_EMPTY, QUAD_EMPTY, ""});
	}

}

void Generator::Selection(IR* ir, const GrammarEntry* rule, Node* root) {

		//auto& symbolTableStack = ir->symbolTableStack;
	auto curTable = ir->curTable;
	auto& quads = ir->quads;
	auto QUAD_EMPTY = ir->QUAD_EMPTY;
if (rule->state.type != "selection_statement")
		return;
	if (rule->symbols[0].type == "IF" && rule->symbols.size() == 6) {
		// IF ( expression ) m_quad_jz statement
		int pjz = std::stoi(root->children[4]->place) - ir->QUAD_BEGIN;
		quads[pjz].arg1 = root->children[2]->place;
		quads[pjz].dst = std::to_string(ir->nextquad());
	}
	else if (rule->symbols[0].type == "IF" && rule->symbols.size() == 2) {
		// IF ( expression ) m_quad_jz statement m_quad_j ELSE m_quad statement
		int pjz = std::stoi(root->children[4]->place) - ir->QUAD_BEGIN;
		quads[pjz].arg1 = root->children[2]->place;
		quads[pjz].dst = root->children[8]->place;
		int pj = std::stoi(root->children[6]->place) - ir->QUAD_BEGIN;
		quads[pj].dst = std::to_string(ir->nextquad());
	}
	else {
		std::cout << "selection_statement use other rules\n";
	}
}

