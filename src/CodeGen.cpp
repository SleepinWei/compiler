#include "CodeGen.h"
#include"Generator.h"
#include"Node.h"
#include<map>

const vector<string> DestCode::registers = {
	"%ebx","%ecx","%edx"
	};

string mapVarToAssembly(IR* ir,DestCode& dest_code, string var,bool isDst = false) {
	string result = ""; 
	if (var[0] >= '0' && var[0] <= '9'){
		// is number 
		result = "$" + var;
	}
	else if (var[0] == 'T' && !isDst) {
		// is temporary 
		result = dest_code.t_allocation[var];
		dest_code.releaseReg(var);
		// 临时变量使用过就释放 reg 
	}
	else if (var[0] == 'T' && isDst) {
		result = dest_code.getFreeReg(var);
	}
	else {
		// is variable
		int offset = ir->curTable->symbols[var].offset;
		if (offset > 0) {
			offset += 8; // ebp + return address 共8字节
		}
		result = std::to_string(offset)+"(%ebp)";
	}
	return result; 
}

/// <summary>
/// code gen process
/// </summary>
/// <param name="ir"></param>
/// <param name="fout"></param>
void CodeGen::codeGen(IR* ir, DestCode& dest_code)
{
	auto& fout = dest_code.fout;

	fout << "\t.text\n";
	for (auto iter = ir->quads.begin(); iter != ir->quads.end(); ++iter) {
		string op = iter->op; 
		string arg1 = iter->arg1;
		string arg2 = iter->arg2; 
		string dst = iter->dst;
		string label = iter->label;

		string arg1_assembly = mapVarToAssembly(ir, dest_code, arg1);
		string arg2_assembly = mapVarToAssembly(ir, dest_code, arg2);
		string dst_assembly = mapVarToAssembly(ir,dest_code,dst,true);

		if (label != "") {
			// entering a function 
			fout << "\t.globl " << label << '\n';
			if (label == "main") {
				fout << "_main:\n";
			}
			else {
				fout << label << ":\n";
			}
			fout << "\tpush %ebp\n";
			fout << "\tmovl %esp, %ebp\n";

			// 空间分配
			auto& table = ir->symbolTables.at(label); // 切换到对应的symboltable
			ir->curTable = table;
			fout << "\tsubl $" << table->width << ", %esp\n";
		}

		static const std::map<string, string> inst_map{ 
			{"+","addl"},
			{"-","subl"},
			{"*","mul"},
			{"/","div"}
		};

		if (op == "RET") {
			// 
			if (dst != "-") {
				fout << "\tmovl " << dst_assembly << ", %eax\n";
			}
			fout << "\tleave\n";
			fout << "\tret";
			if (ir->curTable->param_width != 0) {
				fout << " $" << ir->curTable->param_width;
			}
			fout << "\n";
		}
		else if (op == "call") {
			fout << "\tcall ";
			fout << arg1 << "\n";
			if (dst != "-") {
				// 赋值
				fout << "\tmovl %eax, " << dst_assembly<< "\n"; // TODO: 临时变量处理
			}
		}
		else if (op == "param") {
			// 传参
			fout << "\tpush " << arg1_assembly<< "\n";
		}
		else if (op == "=") {
			fout << "\tmovl " << arg1_assembly << ", " << dst_assembly << "\n";
		}
		else if (op == "+" || op == "-" || op == "*" || op == "/") {
			fout << "\tmovl " << arg1_assembly << ", " << dst_assembly << "\n";
			fout << "\t" << inst_map.at(op) <<" "<< arg2_assembly << ", " << dst_assembly << "\n";
		}
	}
}
