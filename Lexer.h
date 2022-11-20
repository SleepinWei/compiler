#pragma once
#include<iostream> 
#include <string>
#include <map>
#include<fstream>
#include <regex>
using namespace std;

void Error(string s)
{
    cerr<<s<<endl;
}
//Ԥ�����޳��༭���ַ���ע��
string Preprocess(string s) {
    size_t len=s.length();
    string filtered_comment = "";
    for (unsigned int i = 0; i <= len; i++) {
        if (s[i] == '/' && s[i + 1] == '/')  // �Թ�����ע��
            while (s[i] != '\n') 
                i++;
        if (s[i] == '/' && s[i + 1] == '*')  // ����ע��ȥ��[/* , */]
        { 
            i++;
            while (i + 1 < len && (s[i] != '*' || s[i + 1] != '/')) 
                i++;
            if (i + 1 >= len) 
                Error("û���ҵ� */");
            i += 2;     //s[i] == '*' && s[i + 1] == '/'
        }
        filtered_comment += s[i];
    }

    filtered_comment = regex_replace(filtered_comment, regex("\\s"), " ");  // �հ�ȫ��ո�

    return filtered_comment;
}
map<string, int> word;
string word_list[] = {"else", "if", "int", "return", "void", "while", "(", ")", "{", "}",
                      "*", "+", "-", "/", ",", ";", "=", "!=", "<", "<=", "==", ">", ">="};
int identifier_code=1;
int num_code=2;
void init() {
    int code = 3;
    for (auto str : word_list) 
        word[str] = ++code;
}
int searchWord(string s) {
    auto i = word.find(s);
    if(i != word.end())
        return i->second;
    return -1;
}

ofstream fout("out.txt");
int GetToken(string s,int& i)
{
    int len=s.length()-1;//����β��
    string token="";
    char cur_ch;
    while((cur_ch=s[i])==' '){
        i++;
        if(i>=len)
            return -1;
    }
    if(isalpha(cur_ch)||cur_ch=='_')//��ĸ��_��ͷ
    {
        do{
            token+=cur_ch;
            cur_ch=s[++i];
        }while(i<len && (isalpha(cur_ch)||isdigit(cur_ch)||cur_ch=='_'));//��ĸ�����֡�_���
        if(searchWord(token)!=-1)//��ʶ��
        {
            fout<<"$id"<<"\t"<<token<<endl;
        }
        else{//�ؼ���
            fout<<'$'<<token<<"\t"<<token<<endl;   
        }
        return 0;
    }
    else if(isdigit(cur_ch))//����
    {
        do{
            token+=cur_ch;
            cur_ch=s[++i];
        }while(i<len && isdigit(cur_ch));
        fout<<"$num"<<"\t"<<token<<endl;
        return 0;
    }
    else{
        token+=cur_ch;
        string cur_ch_str=token;
        if(i+1<len){
            token+=s[i+1];
            if(searchWord(token)!=-1)//˫�ַ������
            {
                i+=2;
                fout<<'$'<<token<<"\t"<<token<<endl;
                return 0;
            }
        }
        if(searchWord(cur_ch_str)!=-1)//���ַ������
        {
            i++;
            fout<<'$'<<cur_ch_str<<"\t"<<cur_ch_str<<endl;
            return 0;
        }
        else
            Error("Unknown, the next two character are \""+token+"\"");
        return -1;
    }
}

