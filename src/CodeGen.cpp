#include "CodeGen.h"
#include"Generator.h"
#include"Node.h"

const vector<string> DestCode::registers = {
	"%ebx","%ecx","%edx"
	};

string mapVarToAssembly(IR* ir,DestCode& dest_code, string var) {
	string result = ""; 
	if (var[0] >= '0' && var[0] <= '9'){
		// is number 
		result = "$" + var;
	}
	else if (var[0] == 'T') {
		// is temporary 
		result = dest_code.t_allocation[var];
	}
	else {
		// is variable
		int offset = ir->curTable->symbols[var].offset;
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

		// 临时变量使用过就释放 reg 
		if (arg1[0] == 'T') {
			// temp 
			dest_code.releaseReg(arg1);
		}
		if (arg2[0] == 'T') {
			// temp 
			dest_code.releaseReg(arg2);
		}

		if (label != "") {
			// entering a function 
			fout << label << ":\n";
			fout << "\t.globl " << label << '\n';
			fout << "\tpushq %ebp\n";
			fout << "\tmovq %esp, %ebp\n";

			// 空间分配
			auto& table = ir->symbolTables.at(label); // 切换到对应的symboltable
			ir->curTable = table;
			fout << "\tsubl $" << table->width << ", %esp\n";
		}

		if (op == "RET") {
			// 
			if (dst != "-") {
				string dst_assembly = mapVarToAssembly(ir,dest_code,dst);
				fout << "\tmovl " << dst << ", %eax\n";
			}
			fout << "\tleave\n";
			fout << "\tret\n";
		}
		else if (op == "call") {
			fout << "\tcall ";
			fout << arg1 << "\n";
			if (dst != "-") {
				// 赋值
				string reg = dest_code.getFreeReg(dst);
				fout << "\tmovl %eax, " << reg << "\n"; // TODO: 临时变量处理
			}
		}
	}
}
