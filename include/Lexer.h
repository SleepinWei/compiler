#pragma once
#include<iostream> 
#include <string>
#include <map>
#include<fstream>
#include <regex>
#include<sstream>
#include"token.h"
#include"component.h"
#include"Node.h"
using namespace std;

class Lexer {
public:
    ~Lexer() {
        //for (auto n : inputs) {
        //    delete n;
        //}
    }
    void Error(string s)
    {
        cerr << s << endl;
    }

    string Preprocess(string s) {
        size_t len = s.length();
        filtered_comment = "";
        for (unsigned int i = 0; i < len; i++) {
            if (s[i] == '/' && s[i + 1] == '/')  // 略过单行注释
                while (s[i] != '\n')
                    i++;
            if (s[i] == '/' && s[i + 1] == '*')  // 多行注释去除[/* , */]
            {
                i++;
                while (i + 1 < len && (s[i] != '*' || s[i + 1] != '/'))
                    i++;
                if (i + 1 >= len)
                    Error("没有找到 */");
                i += 2;     //s[i] == '*' && s[i + 1] == '/'
            }
            filtered_comment += s[i];
        }
        return filtered_comment;
    }
public:
    //map<string, int> word;
    //vector<string> word_list = { "else", "if", "int", "return", "void", "while", "(", ")", "{", "}",
                          //"*", "+", "-", "/", ",", ";", "=", "!=", "<", "<=", "==", ">", ">=" };
    std::map<string, string> reservedWord= {
    {"sizeof","SIZEOF"},
    {"->","PTR_OP"},{"++","INC_OP"},{"--","DEC_OP"},{"<<","LEFT_OP"},{">>","RIGHT_OP"},{"<=","LE_OP"} ,{">=","GE_OP"},{"==","EQ_OP"},{"!=","NE_OP"},
    {"&&","AND_OP"},{"||","OR_OP"},{"*=","MUL_ASSIGN"},{"/=","DIV_ASSIGN"},{"%=","MOD_ASSIGN"},{"+=","ADD_ASSIGN"},
    {"-=","SUB_ASSIGN"},{"<<=","LEFT_ASSIGN"},{">>=","RIGHT_ASSIGN"},{"&=","AND_ASSIGN"},
    {"^=","XOR_ASSIGN"},{"|=","OR_ASSIGN"},
    {"typedef","TYPEDEF"},{"extern","EXTERN"},{"static","STATIC"},{"auto","AUTO"},{"register","REGISTER"},
    {"int","INT"},{"char","CHAR"},{"short","SHORT"},{"unsigned","UNSIGNED"},{"float","FLOAT"},
    {"double","DOUBLE"},{"const","CONST"},{"volatile","VOLATILE"},{"void","VOID"},
    {"struct","STRUCT"},{"union","UNION"},{"enum","ENUM"},
    {"case","CASE"},{"default","DEFAULT"},{"if","IF"},{"else","ELSE"},{"while","WHILE"},{"(","("},
    {"switch","SWITCH"},{"while","WHILE"},{"do","DO"},{"continue","CONTINUE"},
    {"break","BREAK"},{"return","RETURN"},{"for","FOR"},
    {")",")"},{"(","("}, {"{","{"}, {"}","}"},{"*","*"},
    {"+","+"},{"-","-"},{"/","/"},{"\\","\\"},{".","."},{",",","},{"~","~"},{"!","!"},{"&","&"},{"%","%"},
    {">",">"},{"<","<"},{"|","|"},{"?","?"},{":",":"},{"=","="},{"[","["},{"]","]"},{"...","ELLIPSIS"},
    {";",";"},{"@","@"}
    };

    int identifier_code = 1;
    int num_code = 2;
    //ofstream fout ("out.txt");
    vector<Node*> inputs;
    string filtered_comment;

    void readSpace(const std::string& src, int& pos) {
        while (pos < src.length() && (src[pos] == ' ' || src[pos] == '\t' || src[pos] == '\n')) {
            ++pos;
        }
    }

    void readOneToken(const std::string& s,int& pos){
        readSpace(s,pos);
        if (pos == s.length()) {
            return;
        }
        // recognize _a
        string token = "";
        // read one token
        if (s[pos] == '_' || s[pos] >= 'a' && s[pos] <= 'z' || s[pos] >= 'A' && s[pos] <= 'Z') {
            // read IDENTIFIER
            token += s[pos];
            ++pos;
			while (true)
			{
                if (pos >= s.length())
                    break;
				if(s[pos] >= 'a' && s[pos] <='z' || s[pos] >= 'A' && s[pos] <= 'Z'
					|| s[pos] == '_' || s[pos] >= '0' && s[pos] <= '9'){
					// identifier
					token += s[pos];
				}
				else {
					break;
				}
				++pos;
			}
            // 判断是否为保留字
            if (reservedWord.find(token) == reservedWord.end()) {
                // not found: IDENTIFIER
                inputs.push_back(new Node(IDENTIFIER,token,true));
            }
            else {
                // 如果是保留字
                inputs.push_back(new Node(reservedWord[token], token,true));
            }
        }
        else if(s[pos] >= '0' && s[pos] <= '9') {
            // read CONSTANT: numbers
            token += s[pos];
            ++pos;
            int dotCnt = 0;
            int tailCnt = 0; // not used. for tailing char in number like 1.0f
			while (true)
			{
                if (pos >= s.length())
                    break;
				if(s[pos] >= '0' && s[pos] <= '9' || s[pos] == '.') {
					// identifier
                    if (s[pos] == '.' && dotCnt == 0) {
                        ++dotCnt;
                    }
                    else if (s[pos] == '.' && dotCnt >= 1) {
                        std::cout << "ERROR: WRONG CONSTANT FORMAT, EXTRA DOT\n";
                        return;
                    }
					token += s[pos];
				}
				else {
					break;
				}
				++pos;
			}
            auto newNode = new Node(NUM, token, true);
            if (dotCnt == 0) {
                newNode->width = 4;
                newNode->var_type = "INT";
            }
            else if (dotCnt ==1){
                newNode->width = 8;
                newNode->var_type = "DOUBLE";
            }
            inputs.push_back(newNode);
        }
        else {
            // 
            token += s[pos];
            ++pos;
            if (pos < s.length() && reservedWord.find(token + s[pos]) != reservedWord.end())
            {
                // found
                token += s[pos];
                ++pos;
                inputs.push_back(new Node(reservedWord[token], token, true));
            }
            else if (reservedWord.find(token) != reservedWord.end()) {
                // found
                inputs.push_back(new Node(reservedWord[token],token,true));
            }
            else {
                // not found
                std::cout << "ERROR: WORD " << token << " CAN'T BE FOUND\n";
            }
        }
    }

    void analyze(const std::string filename) {
        std::ifstream f(filename);
        ostringstream ss;
        ss << f.rdbuf();
        string src = Preprocess(ss.str());
        int pos = 0;
        while (pos < src.length()) {
            readOneToken(src, pos);
        }
        inputs.push_back(new Node(END, END, true));
    }
    void print(const std::string& filename) {
        ofstream f(filename);
        for (auto& token : inputs) {
            f << token->type << ' ' << token->place << '\n';
        }
    }
public:
};

