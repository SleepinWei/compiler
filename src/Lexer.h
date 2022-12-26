#pragma once
#include<iostream> 
#include <string>
#include <map>
#include<fstream>
#include <regex>
#include<sstream>
#include"token.h"
#include"component.h"
using namespace std;

class Lexer {
public:
    void Error(string s)
    {
        cerr << s << endl;
    }
    //预处理，剔除编辑性字符和注释
    void Preprocess(string s) {
        size_t len = s.length();
        filtered_comment = "";
        for (unsigned int i = 0; i <= len; i++) {
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

        filtered_comment = regex_replace(filtered_comment, regex("\\s"), " ");  // 空白全变空格
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
    {")","')'"},{"(","("}, {"{","{"}, {"}","}"},{"*","*"},
    {"+","+"},{"-","-"},{"/","/"},{"\\","\\"},{".","."},{",",","},{"~","~"},{"!","!"},{"&","&"},{"%","%"},
    {">",">"},{"<","<"},{"|","|"},{"?","?"},{":",":"},{"=","="},{"[","["},{"]","]"},{"...","ELLIPSIS"},
    {";",";"},
    };

    int identifier_code = 1;
    int num_code = 2;
    //ofstream fout ("out.txt");
    vector<std::string> inputs;
    string filtered_comment;

    //void init() {
    //    int code = 3;
    //    for (auto str : word_list)
    //        word[str] = ++code;
    //}
    //int searchWord_(string s) {
    //    auto i = word.find(s);
    //    if (i != word.end())
    //        return i->second;
    //    return -1;
    //}
    //int searchWord(string s) {
    //    auto iter = std::find(word_list.begin(), word_list.end(), s);
    //    if (iter == word_list.end()) {
    //        return -1;
    //    }
    //    return iter - word_list.begin();
    //}

    //int GetToken(string s, int& i)
    //{
    //    int len = s.length() - 1;//不算尾零
    //    string token = "";
    //    char cur_ch;
    //    while ((cur_ch = s[i]) == ' ') {
    //        i++;
    //        if (i >= len)
    //            return -1;
    //    }
    //    if (isalpha(cur_ch) || cur_ch == '_')//字母或_开头
    //    {
    //        do {
    //            token += cur_ch;
    //            cur_ch = s[++i];
    //        } while (i < len && (isalpha(cur_ch) || isdigit(cur_ch) || cur_ch == '_'));//字母、数字、_组成
    //        if (searchWord(token) == -1)//标识符
    //        //if(std::find(word_list.begin(),word_list.end(),token)==word_list.end())
    //        {
    //            //fout << "$id" << "\t" << token << endl;
    //            inputs.push_back("$id");
    //        }
    //        else {//关键字
    //            //fout << '$' << token << "\t" << token << endl;
    //            inputs.push_back("$" + token);
    //        }
    //        return 0;
    //    }
    //    else if (isdigit(cur_ch))//整数
    //    {
    //        do {
    //            token += cur_ch;
    //            cur_ch = s[++i];
    //        } while (i < len && isdigit(cur_ch));
    //        //fout << "$num" << "\t" << token << endl;
    //        inputs.push_back("$num");
    //        return 0;
    //    }
    //    else {
    //        token += cur_ch;
    //        string cur_ch_str = token;
    //        if (i + 1 < len) {
    //            token += s[i + 1];
    //            if (searchWord(token) != -1)//双字符运算符
    //            {
    //                i += 2;
    //                //fout << '$' << token << "\t" << token << endl;
    //                inputs.push_back("$" + token);
    //                return 0;
    //            }
    //        }
    //        if (searchWord(cur_ch_str) != -1)//单字符运算符
    //        {
    //            i++;
    //            //fout << '$' << cur_ch_str << "\t" << cur_ch_str << endl;
    //            inputs.push_back("$" + cur_ch_str);
    //            return 0;
    //        }
    //        else
    //            Error("Unknown, the next two character are \"" + token + "\"");
    //        return -1;
    //    }
    //}

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
                inputs.push_back(IDENTIFIER);
            }
            else {
                // 如果是保留字
                inputs.push_back(reservedWord[token]);
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
        }
        else {
            // 
            token += s[pos];
            ++pos;

            if (reservedWord.find(token) != reservedWord.end()) {
                // found
                inputs.push_back(token);
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
        string src = ss.str();
        int pos = 0;
        while (pos < src.length()) {
            readOneToken(src, pos);
        }
    }

    void print(const std::string& filename) {
        ofstream f(filename);
        for (auto& token : inputs) {
            f << token<< '\n';
        }
    }
public:
};

