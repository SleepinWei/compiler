#include"Node.h"
#include"Generator.h"
#include<string>

void Generator::analyze(const GrammarEntry* rule, Node* root) {
	Statement(rule, root);
	Assignment(rule, root);
	BoolExpression(rule, root);
	Iteration(rule, root);
	Mquad(rule, root);
	Selection(rule, root);
}

string Generator::newtemp() {
	string temp = "T" + std::to_string(tempCnt);
	++tempCnt;
	return temp;
}

void Generator::BoolExpression(const GrammarEntry* rule, Node* root)
{
	auto state = rule->state;
	auto symbols = rule->symbols;
	if (state.type == "m_quad")
	{
		;// root->quad = nextquad();
	}
	else if (root->children.size() == 3)
	{
		if (state.type == "relational_expression" && symbols[0].type == "relational_expression" && symbols[2].type == "shift_expression")
		{	
			root->place = newtemp();
			quads.push_back(Quad(symbols[1].type, root->children[0]->place, root->children[2]->place, root->place));
		}
		
	}
	else if (root->children.size() == 4)
	{
		Node* e1 = root->children[0], * e2 = root->children[3], * m = root->children[1];
		if (state.type == "logical_and_expression" && symbols[1].type == "AND_OP")
		{
			root->place = newtemp();
			quads.push_back(Quad("AND_OP", e1->place, e2->place, root->place));
		}
		else if (state.type == "logical_or_expression" && symbols[1].type == "OR_OP")
		{
			root->place = newtemp();
			quads.push_back(Quad("OR_OP", e1->place, e2->place, root->place));
		}
	}
}

void Generator::Statement(const GrammarEntry* rule, Node* root) {
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
	else if (rule->state.type == "declaration") {
		if (rule->symbols.size() == 1) {
			// declaration = declaration_specifiers ; 
			root->var_type = root->children[0]->var_type;
		}
		else {
			// declaration = declaration_specifers init_declarator_list ; 
			//TODO:	
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
		}
		else {
			// initializer = { init_list } 
		}
	}
	else if (rule->state.type == "init_declarator") {
		if (rule->symbols.size() == 1) {
			//init_declarator = declarator
			root->place = root->children[0]->place;
		}
		else {
			// init_declarator = declarator = initialzier
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
void Generator::Assignment(const GrammarEntry* rule,Node* root) {
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
		}
		else {
			std::cout << "Shift expression use other rules" << "\n";
		}
	}
	else if (rule->state.type == "additive_expression") {
		if (rule->symbols.size() == 3) {
			// additive_expression = additive_expression +/- multiplicative_expression
			root->place = newtemp();
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
			root->place = newtemp();
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
			root->place = newtemp();
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
		if (rule->symbols[0].type == "IDENTIFIER"){
			root->place = root->children[0]->place;
			// 从表中查询类型
			//root->var_type = root->children[0]->var_type;
			//TODO:
		}
		else if(rule->symbols[0].type == "CONSTANT") {
			root->place = root->children[0]->place;
			root->var_type = root->children[0]->var_type;
		}
		else {
			std::cout << "Primary_expression use other rules" << '\n';
		}
	}
}

void Generator::output(const string& filename) {
	std::ofstream fout(filename);

	for (auto quad : quads) {
		fout << "(" << quad.op << " ," << quad.arg1 << " ," << quad.arg2 << " ," << quad.dst << ")\n";
	}
}
void Generator::Iteration(const GrammarEntry* rule, Node* root) {
	if (rule->state.type != "iteration_statement")
		return;
	if (rule->symbols[0].type == "WHILE") {
		// WHILE m_quad ( expression ) m_quad_jz statement
		quads.push_back({ "j", QUAD_EMPTY, QUAD_EMPTY, root->children[1]->place });
		int sbegin = std::stoi(root->children[5]->place) - QUAD_BEGIN;
		quads[sbegin].arg1 = root->children[3]->place;
		quads[sbegin].dst = std::to_string(nextquad());
	}
	else if (rule->symbols[0].type == "DO") {
		// DO m_quad statement WHILE ( expression ) ;
		quads.push_back({ "jnz", root->children[5]->place, QUAD_EMPTY, root->children[1]->place });
	}
	else if (rule->symbols[0].type == "FOR" && rule->symbols.size() == 11) {
		// FOR ( expression_statement m_quad expression_statement m_quad_jz m_quad_j expression ) m_quad_j statement
		int pupdate = std::stoi(root->children[6]->place) + 1;
		quads.push_back({ "j", QUAD_EMPTY, QUAD_EMPTY, std::to_string(pupdate) });
		int pexpjz = std::stoi(root->children[5]->place) - QUAD_BEGIN;
		int pstmt = std::stoi(root->children[9]->place) + 1;
		quads[pexpjz].arg1 = root->children[4]->place;
		quads[pexpjz].dst = std::to_string(nextquad());
		quads[pexpjz + 1].dst = std::to_string(pstmt);
		int pupdj = std::stoi(root->children[9]->place) - QUAD_BEGIN;
		quads[pupdj].dst = root->children[3]->place;
	}
	else {
		std::cout << "iteration_statement use other rules" << '\n';
	}
}

void Generator::Mquad(const GrammarEntry* rule, Node* root) {
	if (rule->state.type == "m_quad") {
		root->place = std::to_string(nextquad());
	}
	if (rule->state.type == "m_quad_jz") {
		root->place = std::to_string(nextquad());
		quads.push_back({ "jz", QUAD_EMPTY, QUAD_EMPTY, ""});
	}
	if (rule->state.type == "m_quad_j") {
		root->place = std::to_string(nextquad());
		quads.push_back({ "j", QUAD_EMPTY, QUAD_EMPTY, ""});
	}

}

void Generator::Selection(const GrammarEntry* rule, Node* root) {
	if (rule->state.type != "selection_statement")
		return;
	if (rule->symbols[0].type == "IF" && rule->symbols.size() == 6) {
		// IF ( expression ) m_quad_jz statement
		int pjz = std::stoi(root->children[4]->place) - QUAD_BEGIN;
		quads[pjz].arg1 = root->children[2]->place;
		quads[pjz].dst = std::to_string(nextquad());
	}
	else if (rule->symbols[0].type == "IF" && rule->symbols.size() == 2) {
		// IF ( expression ) m_quad_jz statement m_quad_j ELSE m_quad statement
		int pjz = std::stoi(root->children[4]->place) - QUAD_BEGIN;
		quads[pjz].arg1 = root->children[2]->place;
		quads[pjz].dst = root->children[8]->place;
		int pj = std::stoi(root->children[6]->place) - QUAD_BEGIN;
		quads[pj].dst = std::to_string(nextquad());
	}
	else {
		std::cout << "selection_statement use other rules\n";
	}
}

