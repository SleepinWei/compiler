#include"Parser.h"
#include"token.h"
#include<fstream>
#include<sstream>
#include<iostream>
#include<assert.h>
#include <algorithm>
#include<queue>
using std::string;

void Parser::readGrammarY(const std::string& filename) {
	std::ifstream f(filename);
    while(f.peek()!=EOF){
        // input state
        std::string state_type;
        f >> state_type;

        if (state_type == "")
            break;
        Symbol state = Loader::loadSymbolY(state_type);
        grammarSymbols.insert(state_type);

        vector<Symbol> symbols;

        std::string equal;
        f >> equal;
        assert(equal == ":");

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
                Symbol symbol = Loader::loadSymbolY(symbol_type);
                symbols.push_back(symbol);

                if (symbol.type != EMPTY) {
                    grammarSymbols.insert(symbol.type);
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
		startEntry = grammar[START][0];
                //}
            //}
        //}
    }

    //calFirstVN();
}

void Parser::readGrammar(const std::string& filename){
    std::ifstream f(filename);
    while(f.peek()!=EOF){
        // input state
        std::string state_type;
        f >> state_type;

        if (state_type == "")
            break;
        Symbol state = Loader::loadSymbol(state_type);
        grammarSymbols.insert(state_type);

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
                    grammarSymbols.insert(symbol.type);
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

    //calFirstVN();
}

void Parser::printGrammar(const std::string &filename){
    std::ofstream f(filename);
    f << "Symbols:\n";
    for(auto sym:grammarSymbols){
        f << sym << '\n';
    }
    f << "--------------------------------------\n";
    for(auto iter = grammar.begin(); iter != grammar.end(); ++iter){
        f << iter->first << "->";
        for (auto _iter = iter->second.begin(); _iter != iter->second.end(); ++_iter) {
            if (_iter != iter->second.begin()) {
                f << " | ";
            }
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

    std::unordered_map<std::string,std::set<std::string>,std::hash<std::string>>().swap(firstVN);

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
        if (firstVN[rhs[ofst].type].find(EMPTY)!=firstVN[rhs[ofst].type].end()) {
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

void Parser::closure(std::set<Item>& itemSet)
{
    //bool change = true;
    //std::set<Item> resultSet = itemSet;
    std::queue<Item> candidate;
    for (const Item& it : itemSet) {
        candidate.push(it);
    }

    while(candidate.size()) {
        //const size_t isz = resultSet.size();
        //for(size_t i=0;i<isz;i++){
        //for (const auto& itm : itemSet) {
        const auto itm = candidate.front();
        candidate.pop();
            //auto& itm = itemSet[i];
		if (itm.dotPos >= itm.entry->symbols.size()) {
			continue;
		}
		if (itm.entry->symbols[itm.dotPos].isTerminal)
			continue;
		// A-> alpha dot B beta 
		const Symbol& nxtnt = itm.entry->symbols[itm.dotPos];// B 
		auto calResult = calFirst(itm.entry->symbols, itm.dotPos + 1, itm.peek);

		//int i_ = 1;
		//for (const GrammarEntry &ety : grammar) {
			//if (ety.state.type != nxtnt.type)
				//continue;
			// find B->dot C
		auto& vec = grammar[nxtnt.type];
		for (auto iter = vec.begin(); iter != vec.end(); ++iter) {
			GrammarEntry* ety = *iter;
			for (const std::string& fb : calResult) {
				// first(beta a), a = item.peek
				if (fb == EMPTY) // skip empty
					continue;
				Item newitm;
				newitm.entry = ety;
				newitm.dotPos = 0;
				newitm.peek.isEmpty = (fb == EMPTY);
				newitm.peek.isTerminal = true;
				newitm.peek.type = fb;
				//if (notFound(itemSet, newitm)) {
					//itemSet.emplace_back(newitm);
                auto found_pos = itemSet.find(newitm);
				if(found_pos == itemSet.end()){
					itemSet.insert(newitm);
					candidate.push(newitm);
				}
			}
		}
        //}
        //change = (resultSet.size() > isz);
    }
}

std::set<Item> Parser::GO(const std::set<Item>& itemSet, const std::string& X) {
    assert(X != EMPTY);
    std::set<Item> result;

    // construct J 
    for (const auto& item : itemSet) {
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
                //if (notFound(result, jitem)) {
                    result.insert(jitem);
                //}
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

int FoundCluster(const std::vector<std::set<Item>>& cluster, const std::set<Item>& goSet) {
    
    for (int iter = 0; iter < cluster.size();++iter){
        auto& itemset = cluster[iter];
        if (itemset.size() == goSet.size() && goSet == itemset) 
        {
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
    startItem.peek = Symbol(END, true, false);

    std::set<Item> startItemSet = { startItem };
    closure(startItemSet);
    std::vector<std::set<Item>>().swap(cluster);
    cluster.emplace_back(startItemSet);
    //std::vector<std::vector<Item>>{startItemSet}.swap(cluster);

    bool change = true;
    std::ofstream f("./asset/go.txt");

    // candidate queue
    std::queue<int> candidate;  
    candidate.push(0);

    while (candidate.size()){
        std::cout << "remaining candidate : " << candidate.size() << '\n';
		auto itemIndex = candidate.front();
		candidate.pop();

		for(const auto& sym:grammarSymbols){
			//string sym = sym_iter->type;
            const auto& itemSet = cluster[itemIndex];

			std::set<Item> goSet = GO(itemSet, sym);

			if (goSet.size()) {
				int pos = FoundCluster(cluster, goSet);

				// std::cout << pos << '\n';
				if (pos ==-1) {
					// add goset to cluster
					cluster.emplace_back(goSet);

					pos = cluster.size() - 1;
                    candidate.push(pos);
				}
                // TODO: construct table here
                assert(pos != -1);

                //Action action;
                TableEntry entry;
                entry.useStack = true;
                entry.destState = pos;
                entry.gen = nullptr;

                table.insert({ std::tuple<int,std::string>{itemIndex,sym},entry});
			}
		}
	}
}

void Parser::constructTable() {
    for (int citer= 0; citer< cluster.size();++citer) {
        // cluster iter
        // std::cout << citer << '\n';
        auto& itemSet = cluster[citer];
        // Action table
        //for (int iiter = 0; iiter < itemSet.size(); iiter++) {
        for(const auto& item:itemSet){
            // item iter
            // std::cout << iiter << '\n';
            //auto& item = itemSet[iiter];
    //        if (item.dotPos < item.entry->symbols.size() && item.entry->symbols[0].type != EMPTY){
				//// compute GO(Ik,a)
    //            // sym : a
    //            Symbol _s = item.entry->symbols[item.dotPos];
    //            if (!_s.isTerminal) {
    //                continue;
    //            }
    //            string sym = _s.type;
    //            if (sym == "@empty") {
    //                continue;
    //            }
				//auto goSet = GO(itemSet, sym);
    //            if (goSet.size() == 0) {
    //                continue;
    //            }
    //            int pos = FoundCluster(cluster, goSet);

    //            Action action;
    //            action.state = citer;
    //            action.inString = sym;
    //            action.useStack = true;
    //            action.j = pos;
    //            action.gen = nullptr;
    //            // 
    //            if (std::find(actions.begin(), actions.end(), action) == actions.end()) {
    //                actions.emplace_back(action);
    //            }
    //        }
           //else 
            if (item.dotPos == item.entry->symbols.size() || item.entry->symbols[0].type == EMPTY) {
                // [A -> alpha dot, a]

                /*Action action;
                action.state = citer;
                action.inString = item.peek.type;
                action.useStack = false;
                action.gen = item.entry;
                action.j = -1;*/
                TableEntry entry;
                entry.destState = -1;
                entry.gen = item.entry;
                entry.isAcc = false;

                if (item.entry->state.type == START && item.dotPos == 1 && item.peek.type == END) {
                    // S' -> S dot, #
                    entry.isAcc = true;
                }
				//if (std::find(actions.begin(), actions.end(), action) == actions.end()) {
                    //actions.emplace_back(action);
                //}
                table.insert({ {citer,item.peek.type},entry});
            }
        }
        // GOTO table
        /*for (auto& sym : grammarSymbols) {
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
        }*/
    }
}


void Parser::constructDFA(const std::string& filename) {
    std::ofstream f(filename);
    f << "digraph btree{\n";
    f << "size = \" 50, 50\"; \n";
    //f << "fontname = \"Microsoft YaHei\";\n";
    f << "fontsize = 10; \n";
    f << "node [shape=circle, fontsize=22, fontname=Consolas];\n";
    f << "edge [style=bold]\n";

    //outputTree(f, this->root);

    f << "}\n";
    f.close();
    auto dir = 
    system(".\\asset\\gen.bat");
}

void Parser::printTable(const std::string& filename) {
    std::ofstream f(filename);
	f << "Table\n";
    //for (auto& a : actions) {
        //f << a.state << ' ' << a.inString << ' ' << (a.useStack ? "s" : "r") << a.j << ' ' << (int)a.gen 
            //<< ' ' << (a.isAcc ? "acc" : "") << '\n';
    //}
    //f << "----------------------\n";
	//f << "GOTO Table\n";
    //for (auto& g : gotos) {
        //f << g.state << ' ' << g.inState << ' ' << g.gotoState << '\n';
    //}
    for (auto iter = table.begin(); iter != table.end(); ++iter) {
        auto& t = iter->first;
        auto& v = iter->second;
        f << std::get<0>(t) << ' ' << std::get<1>(t) << (v.useStack ? "s" : "r") << v.destState << ' ' << (int)v.gen
            << ' ' << (v.isAcc ? "acc" : "") << '\n';
    }
}

//Action* Parser::findAction(int s,string in) {
    //for (auto& a : actions) {
        //if (a.state == s && a.inString == in) {
            //return &a;
        //}
     //}
    //return nullptr;
//}

//Goto* Parser::findGoto(int s, std::string sym) {
    //for (auto& g : gotos) {
        //if (g.state == s && g.inState == sym) {
            //return &g;
        //}
    //}
    //return nullptr;
//}

void Parser::analyze(const std::vector<std::string>& inputs, const std::string& filename) {
    inputPos = 0;
    stateStack.push(0);
    // construct a node for every non-op state

    std::ofstream f(filename);
    f << "Action Table\n";
    while (true) {
        if (inputPos >= inputs.size()) {
            std::cout << "inputPos > size of inputs\n";
            break;
        }
        string iSym = inputs[inputPos]; // ai 
        int topState = stateStack.top();

        //Action* act = findAction(topState, iSym);
        auto pos = table.find({topState, iSym});
        //Goto* g = findGoto(topState, iSym);
        if (pos != table.end()) {
            auto tableEntry = pos->second;
            if (tableEntry.isAcc) {
                this->root = nodeStack.top();
                nodeStack.pop();
				std::cout << "remaining node in stack: " << nodeStack.size() << '\n';
                f << "Done!";
                std::cout<<"Regulation Process Done";
                return;
            }
            else if (tableEntry.useStack) {
                // ÒÆ½ø
                stateStack.push(tableEntry.destState);
                symbolStack.push(Symbol(iSym, true, false));
                if (iSym == NUM || iSym == IDENTIFIER) {
                    // non-op
                    auto newNode = new Node;
                    newNode->place = iSym;
                    nodeStack.push(newNode);
                }
                if(iSym !=END)
					++inputPos; // move to next ;
                // output 
                f << "Move: " << "read " << iSym << ", " << "push state " << tableEntry.destState<< '\n';
            }
            else {
                // ¹éÔ¼
                const GrammarEntry* rule = tableEntry.gen;
                const Symbol& A = rule->state;
                int r = rule->symbols.size();
                if (rule->symbols[0].type == EMPTY) {
                    r = 0;
                }
                for (int i = 0; i < r; i++) {
                    stateStack.pop();
                }
                int newTop = stateStack.top();
                //Goto* g = findGoto(newTop, A.type);
                auto giter = table.find({newTop,A.type});
                if (giter!=table.end()) {
                    int newS = giter->second.destState;
                    stateStack.push(newS);
                    //TODO:if_then_else Óï¾ä
                    auto resultNode = new Node;
                    resultNode->place = rule->state.type;

                    int stateCnt = 0;
                    for (int i = 0; i < r; ++i) {
                        auto top = symbolStack.top();
                        symbolStack.pop();

                        if (!top.isTerminal || top.type == IDENTIFIER || top.type == NUM ) {
                            if (stateCnt == 0) {
								auto topNode = nodeStack.top();
                                nodeStack.pop();
                                resultNode->right = topNode;
                            }
                            else if (stateCnt == 1) {
                                auto topNode = nodeStack.top();
                                nodeStack.pop();
                                resultNode->left = topNode;
                            }
                            else {
                                std::cout << "Error: More than 2 operands: Rule: " << rule->state.type;
                                for (auto& sym : rule->symbols) {
                                    std::cout << sym.type << ' ';
                                }
                                std::cout << "\n";
                            }
                            ++stateCnt;
                        }
                    }
                    symbolStack.push(A);
                    nodeStack.push(resultNode);

                    f << "Conclude: " << "use rule ";
                    rule->print(f);
                }
                else {
                    // error
                    std::cout << "Error: No matching GOTO At state " << newTop << " Read State " << A.type << "\n";
                }
            }
        }
        else {
            // error 
            std::cout << "Error No Matching Action: At state ";
            std::cout << topState << " Read Symbol "<< iSym<< '\n';
            break;
        }
    }
}

void outputTree(std::ofstream& f, Node* root) {
    f << (int)root << "[label=\"op: " << root->op << " state: " << root->place << "\"];\n";
    if (root->left) {
        f << (int)root << "->" << (int)root->left<< ";\n";
        outputTree(f, root->left);
    }
    if (root->right) {
        f << (int)root << "->" << (int)root->right << ";\n";
        outputTree(f, root->right);
    }
}

void Parser::printTree(const std::string& filename) {
    std::ofstream f(filename);
    f << "digraph btree{\n";
    f << "size = \" 50, 50\"; \n";
    //f << "fontname = \"Microsoft YaHei\";\n";
    f << "fontsize = 10; \n";
    f << "node [shape=box, fontsize=22, fontname=Consolas];\n";
    f << "edge [style=bold]\n";

    outputTree(f, this->root);

    f << "}\n";
    f.close();
}
