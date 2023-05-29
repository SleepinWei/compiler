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
		// ��ʱ����ʹ�ù����ͷ� reg 
	}
	else if (var[0] == 'T' && isDst) {
		result = dest_code.getFreeReg(var);
	}
	else {
		// is variable
		auto symbols = ir->curTable->symbols;

		if (symbols.find(var) == symbols.end()) {
			//std::cout << "[Error] " << "variable "<< var<< " is not found!\n";
			// �Ҳ����������Ǻ��������� QUAD_EMPTY
			return ""; 
		}
		int offset = ir->curTable->symbols[var].offset;
		if (offset >= 0) {
			offset += 8; // ebp + return address ��8�ֽ�
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

		fout << "// "<< iter - ir->quads.begin() << "(" << op << ", " << arg1 << ", " << arg2 << ", " << dst << ")" << ' ' << label << "\n";


		if (label != "") {
			if (label[0] != 'L') {
				// entering a function 
				if (label == "main") {
					 //main function
					fout << "\t.globl " << "_main" << '\n';
					fout << "_main:\n";
				}
				else {
					 //Functions
					fout << "\t.globl " << label << '\n';
					fout << label << ":\n";
				}
				
				fout << "\tpush %ebp\n";
				fout << "\tmovl %esp, %ebp\n";

				// �ռ����
				auto& table = ir->symbolTables.at(label); // �л�����Ӧ��symboltable
				ir->curTable = table;
				fout << "\tsubl $" << table->width << ", %esp\n";
				if (label == "main") {
					fout << "\tcall ___main\n";
				}
			}
			else {
				// Label 
				fout << label << ":\n";
			}
		}

		string arg1_assembly = mapVarToAssembly(ir, dest_code, arg1);
		string arg2_assembly = mapVarToAssembly(ir, dest_code, arg2);
		string dst_assembly = mapVarToAssembly(ir,dest_code,dst,true);

		static const std::map<string, string> inst_map{ 
			{"+","addl"},
			{"-","subl"},
			{"*","mull"},
			{"/","divl"}
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
			// move out; 
			ir->curTable = ir->curTable->parent;
		}
		else if (op == "call") {
			fout << "\tcall ";
			fout << arg1 << "\n";
			if (dst != "-") {
				// ��ֵ
				fout << "\tmovl %eax, " << dst_assembly<< "\n"; // TODO: ��ʱ��������
			}
		}
		else if (op == "param") {
			// ����
			fout << "\tpush " << arg1_assembly<< "\n";
		}
		else if (op == "=") {
			fout << "\tmovl " << arg1_assembly << ", " << dst_assembly << "\n";
		}
		else if (op == "+" || op == "-") {
			fout << "\tmovl " << arg1_assembly << ", " << dst_assembly << "\n";
			fout << "\t" << inst_map.at(op) <<" "<< arg2_assembly << ", " << dst_assembly << "\n";
		}
		else if (op == "*" || op == "/") {
			fout << "\tmovl " << arg1_assembly << ", " << "%eax" << "\n";
			bool exist_edx = dest_code.allocation["%edx"] != "";
			if (exist_edx) {
				// �˳���������eax��edx�У���Ҫ����edxֵ
				fout << "\tpush " << "%edx\n";
			}
			fout << "\t" << inst_map.at(op) << " " << arg2_assembly << "\n";
			if (exist_edx) {
				fout << "\tpop %edx\n";
			}
		}
		else if (op == "jz") {
			int goto_index = std::stoi(dst);
			string label = ir->quads[goto_index-IR::QUAD_BEGIN].label; 
			if (label == "") {
				// 
				std::cout << "No Label for jz command\n"; 
				continue;
			}
			fout << "\tmovl $0, %eax\n";
			fout << "\tcmp " << arg1_assembly << ", %eax\n";
			fout << "\tjz " << label << '\n';
		}
		else if (op == "j"){
			int goto_index = std::stoi(dst);
			string label = ir->quads[goto_index-IR::QUAD_BEGIN].label; 
			if (label == "") {
				// 
				std::cout << "No Label for j command\n"; 
				continue;
			}
			fout << "\tjmp " << label << '\n';
		}
	}
}
