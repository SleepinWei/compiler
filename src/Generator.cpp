#include"Node.h"
#include"Generator.h"
#include<string>

void Generator::analyze(const GrammarEntry* rule, Node* root) {
	Statement(rule, root);
	Assignment(rule, root);
}

string Generator::newtemp() {
	string temp = "T" + std::to_string(tempCnt);
	++tempCnt;
	return temp;
}

void Generator::Statement(const GrammarEntry* rule, Node* root) {
	if (rule->state.type == "type_specifier") {
		root->var_type = rule->symbols[0].type;
	}
	else if (rule->state.type == "declaration_specifiers" && rule->symbols[0].type == "type_specifier") {
		auto child = root->children[0];
		if (child->place == "type_specifier") {
			root->var_type = child->var_type;
		}
	}
	else if (rule->state.type == "declaration") {
		if (rule->symbols.size() == 1) {
			// declaration = declaration_specifiers ; 
			root->var_type = root->children[0]->var_type;
		}
		else {
			// declaration = declaration_specifers init_declarator_list ; 
			//root->
		}
	}
}
void Generator::Assignment(const GrammarEntry* rule,Node* root) {
	// 最高级为 assignment_expression
	if (rule->state.type == "assignment_expression") {
		if (rule->symbols.size() == 1) {
			// assign = conditional 
			root->place = root->children[0]->place;
		}
		else {
			// assign = unary assign_op assign_expression
			root->place = root->children[0]->place;
			quads.push_back({ root->children[1]->place,root->place,root->children[2]->place,root->children[3]->place });
		}
	}
	else if (rule->state.type == "inclusive_or_expression" || rule->state.type == "logical_and_expression" || rule->state.type == "logicial_and_expression"
		|| rule->state.type == "logical_or_expression" || rule->state.type == "conditional_expression" ) {
		root->place = root->children[0]->place;
		if (rule->symbols.size() > 1) {
			std::cout << "use other rules\n";
		}
	}
	else if (rule->state.type == "exclusive_or_expression") {
		if (rule->symbols.size() == 1) {
			// shift = add 
			root->place = root->children[0]->place;
		}
		else {
			std::cout << "Exclusive_or_expression use other rules" << "\n";
		}
	}
	else if (rule->state.type == "and_expression") {
		if (rule->symbols.size() == 1) {
			// and = equality
			root->place = root->children[0]->place;
		}
		else {
			std::cout << "And expression use other rules" << "\n";
		}
	}
	else if (rule->state.type == "equality_expression") {
		if (rule->symbols.size() == 1) {
			// equality = relational
			root->place = root->children[0]->place;
		}
		else {
			std::cout << "equality expression use other rules" << "\n";
		}
	}
	else if (rule->state.type == "relational_expression") {
		if (rule->symbols.size() == 1) {
			// relational = shift
			root->place = root->children[0]->place;
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
			quads.push_back({ root->children[1]->place,root->children[0]->place,root->children[2]->place,root->place });
		}
		else if (rule->symbols.size() == 1) {
			// add = mult
			root->place = root->children[0]->place;
		}
	}
	else if (rule->state.type == "multiplicative_expression") {
		if (rule->symbols.size() == 1) {
			// mult = cast
			root->place = root->children[0]->place;
		}
		else {
			// mult = mult * cast
			root->place = newtemp();
			quads.push_back({ root->children[1]->place,root->children[0]->place,root->children[2]->place,root->place });
		}
	}
	else if (rule->state.type == "cast_expression") {
		if (rule->symbols.size() == 1) {
			// cast  = unary 
			root->place = root->children[0]->place;
		}
		else {
			root->place = newtemp();
			quads.push_back({ "type_cast",root->children[1]->place,root->children[3]->place,root->place });
		}
	}
	else if (rule->state.type == "unary_expression") {
		if (rule->symbols.size() == 1) {
			// unary = postfix 
			root->place = root->children[0]->place;
		}
		else {
			std::cout << "Unary_expression use other rules" << '\n';
		}
	}
	else if (rule->state.type == "unary_operator") {
		root->place = root->children[0]->place;
	}
	else if (rule->state.type == "postfix_expression") {
		if (rule->symbols.size() == 1) {
			// postfix = primary 
			root->place = root->children[0]->place;
		}
		else {
			std::cout << "Postfix_expression use other rules" << '\n';
		}
	}
	else if (rule->state.type == "primary_expression") {
		if (rule->symbols[0].type == "IDENTIFIER" || rule->symbols[0].type == "CONSTANT") {
			root->place = root->children[0]->place;
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
