#include"Node.h"
SymbolTable* mktable(SymbolTable* parent) {
	auto newTable = new SymbolTable();
	newTable->parent = parent;
	return newTable;
}

void enter(SymbolTable* table, string name, string type, int offset, int size, bool is_array, vector<int> dims) {
	table->symbols.insert({ name, {name, type,offset,size, is_array,dims} });
}

void addWidth(SymbolTable* table, int width) {
	table->width += width;
	table->offset -= width;
}

SymbolTable* enterproc(SymbolTable* table) {
	// TODO
	return nullptr;
}