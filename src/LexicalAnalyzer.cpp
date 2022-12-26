#include<iostream> 
#include <string>
#include <map>
#include<fstream>
#include <regex>
#include "token.h"
using namespace std;

void Error(string s)
{
    cerr<<s<<endl;
}
//预处理，剔除编辑性字符和注释
string Preprocess(string s) {
    size_t len=s.length();
    string filtered_comment = "";
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

    return filtered_comment;
}

//map<string, int> word;
//string word_list[] = {"else", "if", "int", "return", "void", "while", "(", ")", "{", "}",
                      //"*", "+", "-", "/", ",", ";", "=", "!=", "<", "<=", "==", ">", ">="};
std::map<string, string> word_map = {
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
std::map<string, string> m = { {"a","b"},{"c","c"}};


int identifier_code=1;
int num_code=2;
//void init() {
    //int code = 3;
    //for (auto str : word_list) 
        //word[str] = ++code;
//}

//int searchWord(string s) {
    //auto i = word.find(s);
    //if(i != word.end())
        //return i->second;
    //return -1;
//}

ofstream fout("out.txt");
//int GetToken(string s,int& i)
//{
    //int len=s.length()-1;//不算尾零
    //string token="";
    //char cur_ch;
    //while((cur_ch=s[i])==' '){
        //i++;
        //if(i>=len)
            //return -1;
    //}
    //if(isalpha(cur_ch)||cur_ch=='_')//字母或_开头
    //{
        //do{
            //token+=cur_ch;
            //cur_ch=s[++i];
        //}while(i<len && (isalpha(cur_ch)||isdigit(cur_ch)||cur_ch=='_'));//字母、数字、_组成
        //if(searchWord(token)!=-1)//标识符
        //{
            //fout<<"$id"<<"\t"<<token<<endl;
        //}
        //else{//关键字
            //fout<<'$'<<token<<"\t"<<token<<endl;   
        //}
        //return 0;
    //}
    //else if(isdigit(cur_ch))//整数
    //{
        //do{
            //token+=cur_ch;
            //cur_ch=s[++i];
        //}while(i<len && isdigit(cur_ch));
        //fout<<"$num"<<"\t"<<token<<endl;
        //return 0;
    //}
    //else{
        //token+=cur_ch;
//        string cur_ch_str=token;
//        if(i+1<len){
//            token+=s[i+1];
//            if(searchWord(token)!=-1)//双字符运算符
//            {
//                i+=2;
//                fout<<'$'<<token<<"\t"<<token<<endl;
//                return 0;
//            }
//        }
//        if(searchWord(cur_ch_str)!=-1)//单字符运算符
//        {
//            i++;
//            fout<<'$'<<cur_ch_str<<"\t"<<cur_ch_str<<endl;
//            return 0;
//        }
//        else
//            Error("Unknown, the next two character are \""+token+"\"");
//        return -1;
//    }
//}


//int main()
//{
//    // 读入程序
//    ifstream t("source.txt");
//    string s((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
//    s=Preprocess(s);
//    init();
//    int i=0;
//    int len=s.length()-1;//不算尾零
//    while (i<len && GetToken(s,i)==0)
//        ;
//    cout<<"Lexical analyzation has done!"<<endl;
//    return 0;
//}