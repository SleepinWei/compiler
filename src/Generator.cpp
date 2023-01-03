#include"Node.h"
#include"Generator.h"

void Generator::analyze(const GrammarEntry* rule, Node* root) {
	Statement(rule, root);
}

void Generator::Statement(const GrammarEntry* rule, Node* root) {
	if (rule->state.type == "type_specifier") {
		root->var_type = rule->symbols[0].type;
	}
	else if (rule->state.type == "declaration_specifiers" && rule->symbols[0].type == "type_specifier") {
		for (auto child: root->children) {
			if (child->place == "type_specifier") {
				root->var_type = child->var_type;
				break;
			}
		}
	}
	else if (rule->state.type == "declaration") {
		for (auto child: root->children) {
			if (child->place == "declaration_specifiers") {
				root->var_type = child->var_type;
				break;
			}
		}
	}
}
