#ifndef PARSER_H
#define PARSER_H
#include"component.h"

#include<vector>
#include<string>
#include<unordered_map>
#include<map>
#include<assert.h>
#include<set>
#include<stack>
#include<tuple>
#include<stdlib.h>
#include"Node.h"

class GrammarInfo{
public:
    using FirstVN = std::unordered_map<std::string, std::set<std::string>, std::hash<std::string>>;

	std::unordered_map<std::string, std::vector<GrammarEntry*>> grammar;
    GrammarEntry* startEntry;
    std::set < std::string > grammarSymbols;
	//std::unordered_map<std::string, std::set<std::string>,std::hash<std::string>> firstVN;
    FirstVN firstVN;

    ~GrammarInfo();
};

using DFA = std::map<std::tuple<int, std::string>, TableEntry>;

class SyntaxTree {
public:
    SyntaxTree(); 
    ~SyntaxTree();

    Node* getRoot(); 
    void setRoot(Node* _root);

private:
    void destroy(Node* root);
    Node* root; 
};

using Cluster = std::vector<std::set<Item>>;

struct IR;

class Parser {
private:
    Parser() = default;
    ~Parser();

public:
    static Parser& GetInstance(); 

    GrammarInfo* readGrammarY(const std::string& filename);

    void calFirstVN(GrammarInfo* grammarInfo);

    void printVNFirst(GrammarInfo::FirstVN& firstVn, const std::string& filename);
    void printCluster(Cluster& cluster, const std::string& filename);
    void printGrammar(GrammarInfo* grammarInfo, const std::string& filename);
    void printDFA(const std::string& filename);
    void printTable(GrammarInfo* info, DFA& table, const std::string& filename);

    DFA loadTable(GrammarInfo* info, const std::string filename);

    void closure(GrammarInfo* grammarInfo, std::set<Item>& itemSet);

    std::set<Item> GO(GrammarInfo* grammarInfo, const std::set<Item>& itemSet, const std::string& X);

    DFA genDFA(GrammarInfo* info);
    void constructTable(Cluster& cluster, DFA& table);

	std::set<std::string> calFirst(GrammarInfo::FirstVN& firstVN, const std::vector<Symbol>& rhs, size_t ofst, const Symbol& peek);
    // analyze
    std::tuple<SyntaxTree*,IR*> analyze(const std::vector<Node*>& inputs, const std::string& filename, const DFA& dfa);

    void save(const string& path);
    void printTree(SyntaxTree* tree, const std::string& filename);
};

class Loader{
public:
    static Symbol loadSymbol(const string& s){
        assert(s[0]=='<' || s[0] == '$' || s[0] == '@');
        if(s[0] == '<'){
            assert(s[s.length()-1]=='>');
            return Symbol(s,false);
        }
        else if (s[0] == '$'){
            return Symbol(s,true);
        }
        else if (s[0] == '@') {
            return Symbol(s, true, true);
        }
        assert(0);
    }
    static Symbol loadSymbolY(const string& s) {
        if (s[0] >= 'A' && s[0] <= 'Z'){
            // is terminal
            return Symbol(s, true);
        }
        else if (s[0] == '\'') {
            return Symbol(s.substr(1, s.length() - 2), true);
        }
        else if (s[0] >= 'a' && s[0] <= 'z') {
            // not terminal
            return Symbol(s, false);
        }
        else if (s == "@") {
            return Symbol(s, true, true);
        }
    }
};

#endif // PARSER_H

