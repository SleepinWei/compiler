#include"Parser.h"
#include<fstream>
#include<sstream>
#include<iostream>
#include<assert.h>
#include <algorithm>
#include<ios>
#include<filesystem>
using std::string;
const std::string EMPTY("@empty");

void Parser::readGrammar(const std::string& filename){
    std::ifstream f(filename);
    while(f.peek()!=EOF){
        // input state
        std::string state_type;
        f >> state_type;

        if (state_type == "")
            break;
        Symbol state = Loader::loadSymbol(state_type);
        grammarSymbols.insert(state);

        vector<Symbol> symbols;

        std::string equal;
        f >> equal;
        assert(equal == "=");

        // input the rest
        //string rest;
        //std::getline(f,rest);
        //std::stringstream stream(rest);

        while(f.peek()!=EOF){
            string symbol_type;
            f >> symbol_type;
            
            if (symbol_type == "") {
                break;
            }

            if(symbol_type == "|" || symbol_type == ";") {
               // create a new entry
               auto gEntry = new GrammarEntry(state,symbols);
               //this->grammar.push_back(gEntry);
               if (grammar.find(state_type) == grammar.end()) {
                   // if not found, insert a state_type
                   grammar.insert({ state_type,std::vector<GrammarEntry*>{gEntry} });
               }
               else {
                   grammar[state_type].push_back(gEntry);
               }
               if (symbol_type == ";")
                   break;

               // clear symbols
               vector<Symbol>().swap(symbols);
            }else{
               // add to current entry
                Symbol symbol = Loader::loadSymbol(symbol_type);
                symbols.push_back(symbol);

                if (symbol.type != EMPTY) {
                    grammarSymbols.insert(symbol);
                }
            }
        }

        //if(symbols.size()){
        //    GrammarEntry gEntry(state,symbols);

        //    //if (state.type == "<Program>") {
        //    //    startEntry = gEntry;
        //    //}
        //    this->grammar.push_back(gEntry);
            //for (auto& entry : grammar) {
                //if (entry.state.type == "<Program>") {
		startEntry = grammar["<Program>"][0];
                //}
            //}
        //}
    }

    calFirstVN();
}

void Parser::printGrammar(const std::string &filename){
    std::ofstream f(filename);
    f << "Symbols:\n";
    for (auto iter = grammarSymbols.begin(); iter != grammarSymbols.end(); ++iter) {
        f << iter->type<< '\n';
    }
    f << "--------------------------------------\n";
    for(auto iter = grammar.begin(); iter != grammar.end(); ++iter){
        f << iter->first << "->";
        for (auto _iter = iter->second.begin(); _iter != iter->second.end(); ++_iter) {
            const auto& vec = (*_iter)->symbols;
            for (auto in_iter = vec.begin(); in_iter != vec.end(); ++in_iter) {
                f << (*in_iter).type.c_str() << " ";
            }
        }
		f << "\n";
    }
}

template<typename T>
static bool notFound(const std::vector<T> &vec, const T& val)
{
    return (std::find(vec.cbegin(), vec.cend(), val) == vec.cend());
}

template<typename T>
static bool notFound(const std::set<T> &vec, const T& val)
{
    return (std::find(vec.cbegin(), vec.cend(), val) == vec.cend());
}

template<typename T>
static void append(std::vector<T> &vec, const std::vector<T> &other)
{
    if (vec == other) {
        return;
    }
    vec.insert(vec.end(), other.cbegin(), other.cend());
}

template<typename T>
static void append(std::set<T> &vec, const std::set<T> &other)
{
    if (vec == other) {
        return;
    }
    vec.insert(other.cbegin(), other.cend());
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

Parser::~Parser() {
    for (auto iter = grammar.begin(); iter != grammar.end(); ++iter) {
        for (auto _iter = iter->second.begin(); _iter != iter->second.end(); ++_iter) {
            if (*_iter != nullptr) {
                delete *_iter;
            }
        }
    }
}

void Parser::calFirstVN(){

    std::unordered_map<std::string,std::set<std::string>>().swap(firstVN);

    //for (const GrammarEntry &g : grammar)
    for (auto iter = grammar.begin(); iter != grammar.end(); ++iter) {
        // iter->first : state string
        if (firstVN.find(iter->first) == firstVN.end())
            firstVN.insert(std::make_pair(iter->first, std::set<std::string>()));
    }

    bool change = true;
    while(change) {
        change = false;
        //for (const GrammarEntry &g : grammar) {
        for(auto iter = grammar.begin();iter!=grammar.end();++iter){

            std::set<string> &fG = firstVN[iter->first];
            const size_t isz = fG.size();

            for (auto _iter = iter->second.begin(); _iter != iter->second.end(); ++_iter) {
                auto& vec = (*_iter)->symbols;
                auto sit = vec.begin();
                for (; sit != vec.end() && !(sit->isTerminal); ++sit) {
                    bool noEmpty = notFound(fG, EMPTY);
                    append(fG, firstVN[sit->type]);
                    if(noEmpty)
                        fG.erase(EMPTY);
                    if (notFound(firstVN[sit->type], EMPTY))
                        break;
                }
                if (sit == vec.cend()) {
                    fG.insert(fG.begin(), EMPTY);
                }
                else if (sit->isTerminal) {
                    //fG.push_back(sit->isEmpty ? EMPTY : sit->type);
                    fG.insert(sit->isEmpty ? EMPTY : sit->type);
                }

                //removeDup(fG); // no dup for set
                change |= (fG.size() > isz);
            }
        }
    }
}

//void Parser::printFirst(const std::string& filename) {
	//std::ofstream f(filename);
    //for (auto iter = firstMap.begin(); iter != firstMap.end(); ++iter) {
        //f << iter->first.type << ": ";
        //for (auto& sym : iter->second) {
            //f << sym.type << ", ";
        //}
        //f << '\n';
    //}
//}

void Parser::printVNFirst(const std::string& filename) {
	std::ofstream f(filename);
    for (auto iter = firstVN.begin(); iter != firstVN.end(); ++iter) {
        f << iter->first<< ": ";
        for (auto& sym : iter->second) {
            f << sym << ", ";
        }
        f << '\n';
    }
}

void Parser::printCluster(const std::string& filename) {
    std::ofstream f(filename);
    int index = 0;
    for (auto& itemset : cluster) {
        f << "ItemSet " << index++<<"{\n";
        for (auto& item : itemset) {
            item.print(f);
        }
        f << "}\n";
    }
}

std::set<std::string> Parser::calFirst(const std::vector<Symbol> &rhs, size_t ofst, const Symbol &peek)
{
    std::set<std::string> result;
    bool finalempty = true;
    for(; ofst < rhs.size() && !rhs[ofst].isTerminal; ofst++) {
        append(result, firstVN[rhs[ofst].type]);
        if (!notFound(firstVN[rhs[ofst].type], EMPTY)) {
            // find empty: remove empty and continue
            finalempty = true;
            result.erase(EMPTY);
        }
        else {
            // no empty, then try returning
            finalempty = false;
            break;
        }
    }

    if (ofst == rhs.size() && finalempty) {
        if (peek.isEmpty)
            result.insert(EMPTY);
        else if (peek.isTerminal)
            result.insert(peek.type);
        else
            append(result, firstVN[peek.type]);
    } 
    else if (ofst == rhs.size()) {
        // final symbol has no empty, then don't add peek
        ;
    }
    else if (rhs[ofst].isTerminal) {
        result.insert(rhs[ofst].type);
    }

    return result;
}

void Parser::closure(std::vector<Item> &itemSet)
{
    bool change = true;
    while(change) {
        const size_t isz = itemSet.size();
        for(size_t i=0;i<isz;i++){
        //for (const Item &itm : itemSet) {
            auto& itm = itemSet[i];
            if (itm.dotPos >= itm.entry->symbols.size()) {
                continue;
            }
            if (itm.entry->symbols[itm.dotPos].isTerminal)
                continue;
            // A-> alpha dot B beta 
            const Symbol &nxtnt = itm.entry->symbols[itm.dotPos];// B 
			auto calResult = calFirst(itm.entry->symbols, itm.dotPos+1, itm.peek);

            int i_ = 1;
            //for (const GrammarEntry &ety : grammar) {
                //if (ety.state.type != nxtnt.type)
                    //continue;
                // find B->dot C
            auto& vec = grammar[nxtnt.type];
            for(auto iter = vec.begin();iter!=vec.end();++iter){
                GrammarEntry* ety = *iter;
                for (const std::string &fb : calResult) {
                    // first(beta a), a = item.peek
                    if (fb == EMPTY) // skip empty
                        continue;
                    Item newitm;
                    newitm.entry = ety;
                    newitm.dotPos = 0;
                    newitm.peek.isEmpty = (fb == EMPTY);
                    newitm.peek.isTerminal = true;
                    newitm.peek.type = fb;
                    if (notFound(itemSet, newitm)) {
                        itemSet.emplace_back(newitm);
                    }
                }
            }
        }
        change = (itemSet.size() > isz);
    }
}

std::vector<Item> Parser::GO(const std::vector<Item>& itemSet, const std::string& X) {
    assert(X != EMPTY);
    std::vector<Item> result;

    // construct J 
    for (auto item : itemSet) {
        // for all items in itemSet
        if (item.dotPos < item.entry->symbols.size()) {
            auto& symbols = item.entry->symbols;
            if (symbols[item.dotPos].type == X) {
                // A->alpha dot X beta 
                Item jitem;
                jitem.entry = item.entry;
                //if(X != EMPTY)
				jitem.dotPos = item.dotPos + 1;
                //else {
                    //assert(item.dotPos == 0);
                    //jitem.dotPos = 0;
                //}
                jitem.peek = item.peek;
                assert(jitem.entry != nullptr);
                assert(jitem.dotPos >= 0 && jitem.dotPos <= item.entry->symbols.size());
                if (notFound(result, jitem)) {
                    result.emplace_back(jitem);
                }
            }
        }
    }

    // compute closure
    closure(result);

    int debug = 0;
    if (debug){
        std::ofstream out("./asset/go.txt");
        for (auto& item : result) {
            item.print(out);
        }
	}

    return result;
}

int FoundCluster(const std::vector<std::vector<Item>>& cluster, const std::vector<Item>& goSet) {
    
    for (int iter = 0; iter < cluster.size();iter++) {
        auto& itemset = cluster[iter];
        bool equal = true;
        if (itemset.size() != goSet.size()) {
            continue;
        }
        else {
            for (int i = 0; i < itemset.size(); i++) {
                equal &= (itemset[i] == goSet[i]);
                if (!equal)
                    break;
            }
        }
        if (equal) {
            return iter;
        }
    }
    return -1;
}

void print(std::ofstream& f, const std::vector<Item>& itemSet) {
    f << "ItemSet {\n";
    for(auto& item : itemSet) {
		item.print(f);
	}
	f << "}\n";
}

void Parser::constructCluster() {
    Item startItem;
    startItem.dotPos = 0;
    startItem.entry = startEntry;
    startItem.peek = Symbol("#", true, false);

    std::vector<Item> startItemSet = { startItem };
    closure(startItemSet);
    std::vector<std::vector<Item>>().swap(cluster);
    cluster.emplace_back(startItemSet);
    //std::vector<std::vector<Item>>{startItemSet}.swap(cluster);

    bool change = true;
	std::ofstream f("./asset/go.txt");
    int startIndex = 0;
    while (true){
        //change = false; 
        int nextStartIndex = cluster.size();
        // std::cout << "iters" << '\n';
        for(int i=startIndex;i<nextStartIndex;i++){
        //for (auto& itemSet : cluster) {
            //for (string sym : grammarSymbols) {
            for(auto sym_iter = grammarSymbols.begin();sym_iter!=grammarSymbols.end();++sym_iter){
				const auto& itemSet = cluster[i];
                string sym = sym_iter->type;

                std::vector<Item> goSet = GO(itemSet, sym);

                if (goSet.size()) {
                    int pos = FoundCluster(cluster, goSet);
                    // std::cout << pos << '\n';
                    if (pos ==-1) {
                        // add goset to cluster
                        
                        //change |= true;
                        cluster.emplace_back(goSet);
                        pos = cluster.size() - 1;
                    }
                     
                }
            }
        }
		startIndex = nextStartIndex;
        if (nextStartIndex == cluster.size()) {
            break;
        }
    }
}

void Parser::constructTable() {
    for (int citer= 0; citer< cluster.size();++citer) {
        // cluster iter
        // std::cout << citer << '\n';
        auto& itemSet = cluster[citer];
        // Action table
        for (int iiter = 0; iiter < itemSet.size(); iiter++) {
            // item iter
            // std::cout << iiter << '\n';
            auto& item = itemSet[iiter];
            if (item.dotPos < item.entry->symbols.size() && item.entry->symbols[0].type != EMPTY){
				// compute GO(Ik,a)
                // sym : a
                Symbol _s = item.entry->symbols[item.dotPos];
                if (!_s.isTerminal) {
                    continue;
                }
                string sym = _s.type;
                if (sym == "@empty") {
                    continue;
                }
				auto goSet = GO(itemSet, sym);
                if (goSet.size() == 0) {
                    continue;
                }
                int pos = FoundCluster(cluster, goSet);

                Action action;
                action.state = citer;
                action.inString = sym;
                action.useStack = true;
                action.j = pos;
                action.gen = nullptr;
                // 
                if (std::find(actions.begin(), actions.end(), action) == actions.end()) {
                    actions.emplace_back(action);
                }
            }
            else if (item.dotPos == item.entry->symbols.size() || item.entry->symbols[0].type == EMPTY) {
                // [A -> alpha dot, a]

                Action action;
                action.state = citer;
                action.inString = item.peek.type;
                action.useStack = false;
                action.gen = item.entry;
                action.j = -1;

                if (item.entry->state.type == "<Program>" && item.dotPos == 1 && item.peek.type == "#") {
                    // S' -> S dot, #
                    action.isAcc = true;
                }
				if (std::find(actions.begin(), actions.end(), action) == actions.end()) {
                    actions.emplace_back(action);
                }
            }
        }
        // GOTO table
        for (auto& sym : grammarSymbols) {
            if (sym.isTerminal) {
                continue;
            }
			auto goSet = GO(itemSet, sym.type);
			if (goSet.size() == 0) {
				continue;
			}
			int pos = FoundCluster(cluster, goSet);
            if (pos != -1) {
                Goto g;
                g.state = citer;
                g.inState = sym.type;
                g.gotoState = pos;
                if (std::find(gotos.begin(), gotos.end(), g) == gotos.end()) {
                    gotos.emplace_back(g);
                }
            }
        }
    }
}

void Parser::constructDFA(const std::string& filename) {
    std::ofstream f(filename);
    f << "digraph dfa{\n";
    f << "size = \" 50, 50\"; \n";
    f << "fontname = \"Microsoft YaHei\";\n";
    f << "fontsize = 10; \n";
    f << "node [shape = circle, fontname = \"Microsfot YaHei\", fontsize = 10];\n";
    f << "edge [fontname = \"Microsoft YaHei\", fontsize = 10];\n";

    for (int i = 0; i < cluster.size(); i++) {
        f << i << ";\n";
    }
    
    for (auto& action : actions) {
        if (action.useStack) {
            f << action.state << " -> " << action.j << "[label = \"" << action.inString << "\" ]; \n";
        }
    }
    for (auto& go : gotos) {
		f << go.state << " -> " << go.gotoState << "[label = \"" << go.inState << "\" ]; \n";
    }

    f << "}\n";
    f.close();
    auto dir = 
    system(".\\asset\\gen.bat");
}

void Parser::printTable(const std::string& filename) {
    std::ofstream f(filename);
	f << "Action Table\n";
    for (auto& a : actions) {
        f << a.state << ' ' << a.inString << ' ' << (a.useStack ? "s" : "r") << a.j << ' ' << (int)a.gen 
            << ' ' << (a.isAcc ? "acc" : "") << '\n';
    }
    f << "----------------------\n";
	f << "GOTO Table\n";
    for (auto& g : gotos) {
        f << g.state << ' ' << g.inState << ' ' << g.gotoState << '\n';
    }
}

Action* Parser::findAction(int s,string in) {
    for (auto& a : actions) {
        if (a.state == s && a.inString == in) {
            return &a;
        }
     }
    return nullptr;
}

Goto* Parser::findGoto(int s, std::string sym) {
    for (auto& g : gotos) {
        if (g.state == s && g.inState == sym) {
            return &g;
        }
    }
    return nullptr;
}

void Parser::analyze(const std::vector<std::string>& inputs, const std::string& filename) {
    inputPos = 0;
    stateStack.push(0);
    std::ofstream f(filename);
    f << "Action Table\n";
    while (true) {
        string iSym = inputs[inputPos]; // ai 
        int topState = stateStack.top();

        Action* act = findAction(topState, iSym);
        //Goto* g = findGoto(topState, iSym);
        if (act) {
            if (act->isAcc) {
                f << "Done!";
                std::cout<<"Regulation Process Done";
                return;
            }
            else if (act->useStack) {
                // �ƽ�
                stateStack.push(act->j);
                symbolStack.push(Symbol(iSym, true, false));
                if(iSym != "#")
					++inputPos; // move to next ;
                // output 
                f << "Move: " << "read " << iSym << ", " << "push state " << act->j<< '\n';
            }
            else {
                // ��Լ
                const GrammarEntry* rule = act->gen;
                const Symbol& A = rule->state;
                int r = rule->symbols.size();
                if (rule->symbols[0].type == EMPTY) {
                    r = 0;
                }
                for (int i = 0; i < r; i++) {
                    stateStack.pop();
                }
                int newTop = stateStack.top();
                Goto* g = findGoto(newTop, A.type);
                if (g) {
                    int newS = g->gotoState;
                    stateStack.push(newS);
                    for (int i = 0; i < r; i++) {
                        symbolStack.pop();
                    }
                    symbolStack.push(A);

                    f << "Conclude: " << "use rule ";
                    rule->print(f);
                }
                else {
                    // error
                }
            }
        }
        else {
            // error 
            std::cout << "Error" << '\n';
            std::cout << iSym << '\n';
            std::cout << "State: " << topState << '\n';
            break;
        }
    }
}
