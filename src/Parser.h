#ifndef PARSER_H
#define PARSER_H
#include"component.h"

#include<vector>
#include<string>
#include<unordered_map>

class Parser{
public:
    std::vector<GrammarEntry> grammar;
    std::unordered_map<std::string, std::vector<std::string>> firstVN;
public:
    void readGrammar(const std::string& filename);
private:
    void calFirstVN();
    void calFirst();
    void calFollow();
    void closure(std::vector<Item> &itemSet);
};

#endif // PARSER_H
