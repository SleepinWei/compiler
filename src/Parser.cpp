#include"Parser.h"
#include<fstream>
#include<sstream>
#include<assert.h>
#include <algorithm>

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

    calFirstVN();
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
    // init 
    std::unordered_map<Symbol, std::set<Symbol>, Symbol_hash>().swap(firstMap);

    //
    while (true) {
        // TODO: decide haveChanged
        bool haveChanged = false;
        for (auto& entry : grammar) { // for every entry in grammar
            const auto& left = entry.state;
            const auto& right = entry.symbols;
            if (firstMap.find(left) == firstMap.end()) {
				// insert new symbol
                firstMap.insert({ left,std::set<Symbol>() });
            }
            // X -> Y1,Y2...Yn
            auto& symbolSet = firstMap[left]; // fisrt(X)
            bool allHasEmpty = true;
            auto iter = right.begin();
            for (; iter != right.end();++iter) {
                const auto& r_symbol = *iter;
                if (r_symbol.isTerminal && symbolSet.find(r_symbol)==symbolSet.end()) {
                    // if is terminal: add to set
                    // Y1: terminal
                    symbolSet.insert(r_symbol);
                    haveChanged = true;
                    allHasEmpty &= false;
                    break;
                }
                else if (r_symbol.isTerminal) {
                    // already added -> return 
                    // Y1: terminal but is already added
                    allHasEmpty &= false;
                    break;
                }
                else {
                    // Y1 non-terminal
                    if (firstMap.find(r_symbol) != firstMap.end()) {
                        // decide whether changes or not
                        auto& first_y1= firstMap[r_symbol]; // First[Y1]
                        bool hasEmpty = false;
                        for (auto& temp : first_y1) { // elements in first[Y1]
                            if (temp.isEmpty) {
                                // if encounter empty, skip
                                hasEmpty |= true;
                            }
                            else {
                                // not empty
                                if (symbolSet.find(temp) == symbolSet.end()) {
                                    //if haven't added,
									symbolSet.insert(temp);
                                    haveChanged |= true;
                                }
                            }
                        }
                        allHasEmpty &= hasEmpty;
                        if (!hasEmpty) {
                            // if there's no empty set, break;
                            break;
                        }
                    }
                    else {
                        // empty set
                        break;
                    }
                }
            }
            if (allHasEmpty && iter == right.end()) {
                // add empty to set
                Symbol empty("@empty", true, true);
                if (symbolSet.find(empty) == symbolSet.end()) {
                    // empty string is not added yet;
                    symbolSet.insert(empty);
                    haveChanged |= true;
                }
            }
        }
        if (!haveChanged) {
            // no changes -> quit;
            break;
        }
    }
}

template<typename T>
static bool notFound(const std::vector<T> &vec, const T& val)
{
    return (std::find(vec.cbegin(), vec.cend(), val) == vec.cend());
}

template<typename T>
static void append(std::vector<T> &vec, const std::vector<T> &other)
{
    vec.insert(vec.end(), other.cbegin(), other.cend());
}

template<typename T>
static void removeDup(std::vector<T> &v)
{
    auto end = v.end();
    for (auto it = v.begin(); it != end; ++it) {
        end = std::remove(it + 1, end, *it);
    }

    v.erase(end, v.end());
}

const std::string EMPTY("@");

void Parser::calFirstVN(){

    firstVN.clear();
    for (const GrammarEntry &g : grammar)
        if (firstVN.find(g.state.type) == firstVN.end())
            firstVN.insert(std::make_pair(g.state.type, std::vector<std::string>()));

    bool change = true;
    while(change) {
        change = false;
        for (const GrammarEntry &g : grammar) {
            auto sit = g.symbols.cbegin();
            std::vector<string> &fG = firstVN[g.state.type];
            const size_t isz = fG.size();

            for(; !sit->isTerminal && sit != g.symbols.cend(); sit++) {
                // append(fG, firstVN[sit->type]);
                bool noEmpty = notFound(fG, EMPTY);
                append(fG, firstVN[sit->type]);
                if (noEmpty)
                    std::remove(fG.begin(), fG.begin(), EMPTY);
                if (notFound(firstVN[sit->type], EMPTY))
                    break;
            }
            if (sit == g.symbols.cend()) {
                fG.insert(fG.begin(), EMPTY);
            } else if (sit->isTerminal) {
                fG.push_back(sit->isEmpty ? EMPTY : sit->type);
            }

            removeDup(fG);
            change |= (fG.size() > isz);
        }
    }
}

void Parser::printFirst(const std::string& filename) {
	std::ofstream f(filename);
    for (auto iter = firstMap.begin(); iter != firstMap.end(); ++iter) {
        f << iter->first.type << ": ";
        for (auto& sym : iter->second) {
            f << sym.type << ", ";
        }
        f << '\n';
    }
}

std::vector<std::string> Parser::calFirst(const std::vector<Symbol> &rhs, size_t ofst, const Symbol &peek)
{
    std::vector<std::string> result;
    for(; !rhs[ofst].isTerminal && ofst < rhs.size(); ofst++) {
        append(result, firstVN[rhs[ofst].type]);
        if (!notFound(firstVN[rhs[ofst].type], EMPTY))
            std::remove(result.begin(), result.end(), EMPTY);
        else
            break;
    }

    if (ofst == rhs.size()) {
        if (peek.isEmpty)
            result.push_back(EMPTY);
        else if (peek.isTerminal)
            result.push_back(peek.type);
        else
            append(result, firstVN[peek.type]);
    } else if (rhs[ofst].isTerminal) {
        result.push_back(rhs[ofst].type);
    }

    return result;
}

void Parser::closure(std::vector<Item> &itemSet)
{
    bool change = true;
    while(change) {
        const size_t isz = itemSet.size();
        for (const Item &itm : itemSet) {
            if (itm.entry->symbols[itm.dotPos].isTerminal)
                continue;
            const Symbol &nxtnt = itm.entry->symbols[itm.dotPos];
            for (const GrammarEntry &ety : grammar) {
                if (ety.state.type != nxtnt.type)
                    continue;
                for (const std::string &fb : calFirst(itm.entry->symbols, itm.dotPos, itm.peek)) {
                    Item newitm;
                    newitm.entry = &ety;
                    newitm.dotPos = 0;
                    newitm.peek.isEmpty = (fb == EMPTY);
                    newitm.peek.isTerminal = true;
                    newitm.peek.type = fb;
                    itemSet.emplace_back(newitm);
                }
            }
        }
        change = (itemSet.size() > isz);
    }
}
