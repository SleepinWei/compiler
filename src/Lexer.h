#pragma once
#include<iostream> 
#include <string>
#include <map>
#include<fstream>
#include <regex>
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
    map<string, int> word;
    vector<string> word_list = { "else", "if", "int", "return", "void", "while", "(", ")", "{", "}",
                          "*", "+", "-", "/", ",", ";", "=", "!=", "<", "<=", "==", ">", ">=" };
    int identifier_code = 1;
    int num_code = 2;
    //ofstream fout ("out.txt");
    vector<Symbol> inputs;
    string filtered_comment;

    void init() {
        int code = 3;
        for (auto str : word_list)
            word[str] = ++code;
    }
    int searchWord_(string s) {
        auto i = word.find(s);
        if (i != word.end())
            return i->second;
        return -1;
    }
    int searchWord(string s) {
        auto iter = std::find(word_list.begin(), word_list.end(), s);
        if (iter == word_list.end()) {
            return -1;
        }
        return iter - word_list.begin();
    }

    int GetToken(string s, int& i)
    {
        int len = s.length() - 1;//不算尾零
        string token = "";
        char cur_ch;
        while ((cur_ch = s[i]) == ' ') {
            i++;
            if (i >= len)
                return -1;
        }
        if (isalpha(cur_ch) || cur_ch == '_')//字母或_开头
        {
            do {
                token += cur_ch;
                cur_ch = s[++i];
            } while (i < len && (isalpha(cur_ch) || isdigit(cur_ch) || cur_ch == '_'));//字母、数字、_组成
            if (searchWord(token) == -1)//标识符
            //if(std::find(word_list.begin(),word_list.end(),token)==word_list.end())
            {
                //fout << "$id" << "\t" << token << endl;
                inputs.push_back(Symbol("$id", true, false));
            }
            else {//关键字
                //fout << '$' << token << "\t" << token << endl;
                inputs.push_back(Symbol("$" + token, true, false));
            }
            return 0;
        }
        else if (isdigit(cur_ch))//整数
        {
            do {
                token += cur_ch;
                cur_ch = s[++i];
            } while (i < len && isdigit(cur_ch));
            //fout << "$num" << "\t" << token << endl;
            inputs.push_back(Symbol("$num", true, false));
            return 0;
        }
        else {
            token += cur_ch;
            string cur_ch_str = token;
            if (i + 1 < len) {
                token += s[i + 1];
                if (searchWord(token) != -1)//双字符运算符
                {
                    i += 2;
                    //fout << '$' << token << "\t" << token << endl;
                    inputs.push_back(Symbol("$" + token, true, false));
                    return 0;
                }
            }
            if (searchWord(cur_ch_str) != -1)//单字符运算符
            {
                i++;
                //fout << '$' << cur_ch_str << "\t" << cur_ch_str << endl;
                inputs.push_back(Symbol("$" + cur_ch_str,true,false));
                return 0;
            }
            else
                Error("Unknown, the next two character are \"" + token + "\"");
            return -1;
        }
    }

    void analyze() {
		init();
		int i = 0;
		int len = filtered_comment.length() - 1;
		while (i < len && GetToken(filtered_comment, i) == 0)
			;
    }

    void print(const std::string& filename) {
        ofstream f(filename);
        for (auto& token : inputs) {
            f << token.type << '\n';
        }
    }
};

