#pragma once
#include<fstream>
#include<map>
#include<vector>
#include<stack>
#include<string>
#include<iostream>
using std::ofstream;
using std::vector;
using std::string;
using std::stack;

struct Quad;
class IR; 

/// <summary>
/// 目标代码生成时是否需要记录状态？
/// </summary>
class DestCode{
public:
	static const vector<string> registers;

	ofstream fout; 
	std::map<string, string> allocation; // register -> temp 
	std::map<string, string> t_allocation; // temp ->register
	std::stack<string> free_regs; 
public:
	DestCode() {
		for (auto reg : registers) {
			allocation.insert({ reg,""});
		}
		for (auto iter = DestCode::registers.rbegin(); iter != DestCode::registers.rend(); ++iter) {
			free_regs.push(*iter);
		}
	}

	string getFreeReg(string temp) {
		auto result = free_regs.top();
		free_regs.pop();

		this->allocation[result] = temp;
		this->t_allocation.insert({ temp,result });

		return result;
	}

	void releaseReg(string temp) {
		// temp : 临时变量名
		if(t_allocation.find(temp) == t_allocation.end()){
			std::cerr <<"WARNING: "<< temp << " has already been released\n";
			return;
		}
		auto reg = this->t_allocation[temp];
		free_regs.push(reg);
		allocation[reg] = ""; 
		this->t_allocation.erase(temp);
	}
};


class CodeGen {
public:
	static void codeGen(IR* ir, DestCode& dest_code); 
};
