#include"Node.h"
SymbolTable* mktable(SymbolTable* previous) {
	auto newTable = new SymbolTable;
	newTable->previous = previous;
	return newTable;
}

void enter(SymbolTable* table, string name, string type, int offset) {
	table->symbols.insert({ name, {type,offset} });
}

void addWidth(SymbolTable* table, int width) {
	table->width += width;
	table->offset += width;
}

SymbolTable* enterproc(SymbolTable* table) {
	// TODO
	return nullptr;
}