#include"Parser.h"
#include<fstream>
#include<sstream>
#include<assert.h>

void Parser::readGrammar(const std::string& filename){
    std::ifstream f(filename);
    while(f.peek()!=EOF){
        // input state
        std::string state_type;
        f >> state_type;

        Symbol state = Loader::loadSymbol(state_type);
        vector<Symbol> symbols;

        std::string equal;
        f >> equal;
        assert(equal == "=");

        // input the rest
        string rest;
        std::getline(f,rest);
        std::stringstream stream(rest);

        while(stream.peek()!=EOF){
            string symbol_type;
            stream >> symbol_type;
            
            if (symbol_type == "") {
                break;
            }

            if(symbol_type == "|"){
               // create a new entry
               GrammarEntry gEntry(state,symbols);
               this->grammar.push_back(gEntry);

               // clear symbols
               vector<Symbol>().swap(symbols);
            }else{
               // add to current entry
                Symbol symbol = Loader::loadSymbol(symbol_type);
                symbols.push_back(symbol);
            }
        }

        if(symbols.size()){
            GrammarEntry gEntry(state,symbols);
            this->grammar.push_back(gEntry);
        }
    }
}

void Parser::printGrammar(const std::string &filename){
    std::ofstream f(filename);
    for(auto iter = grammar.begin(); iter != grammar.end(); ++iter){
        f << (*iter).state.type.c_str() << "->";
        auto& vec = (*iter).symbols;
        for(auto in_iter = vec.begin();in_iter != vec.end();++in_iter ){
            f << (*in_iter).type.c_str() << " ";
        }
        f << "\n";
    }
}

void Parser::calFirst(){

}

std::vector<std::string> calFirst(const std::vector<Symbol> &rhs, size_t ofst, std::string peek)
{
    std::vector<std::string> result;
    for(; ofst < rhs.size(); ofst++) {

    }
}

void Parser::closure(std::vector<Item> &itemSet)
{
    itemSet[0].entry->symbols.cbegin()
}
