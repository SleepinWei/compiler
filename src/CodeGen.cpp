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
	else if (var[0] == '"') {
		result = ir->stringTable.at(var);
		if (!isDst) {
			result = "$" + result; // 如果是 mov 类似的指令，需要加 $Label
		}
	}
	else {
		// is variable
		auto symbols = ir->curTable->symbols;

		// 解析数组
		int left_bracket = 0;
		for (left_bracket = 0; left_bracket < var.size(); ++left_bracket) {
			if (var[left_bracket] == '[') {
				break;
			}
		}

		string var_name = var;
		if (left_bracket < var.size()) {
			// is array 
			var_name = var.substr(0, left_bracket);
		}

		if (symbols.find(var_name) == symbols.end()) {
			//std::cout << "[Error] " << "variable "<< var<< " is not found!\n";
			// 找不到，可能是函数名或者 QUAD_EMPTY
			return ""; 
		}

		int offset = 0;

		if (left_bracket < var.size()) {
			// is array element
			auto var_info = symbols.find(var_name);
			int right_bracket = 0; 
			int arg_cnt = 0; // 第几个参数
			while (right_bracket < var.size()) {
				right_bracket = left_bracket + 1; 

				while (right_bracket < var.size() && var[right_bracket] != ']') {
					++right_bracket;
				}

				if (right_bracket == var.size()) {
					break; 
				}

				int length = right_bracket - left_bracket - 1;
				int index =std::stoi(var.substr(left_bracket + 1, length));

				if (arg_cnt >= 1) {
					offset *= var_info->second.dims[arg_cnt];
				}
				offset += index; 

				// end of var
				if (right_bracket == var.size() - 1) {
					break; 
				}

				arg_cnt ++ ;

				// find next [ 
				left_bracket = right_bracket + 1; 
				while (left_bracket < var.size() && var[left_bracket] != '[') {
					++left_bracket;
				}
			}
			offset *= Generator::getWidth(var_info->second.type);
		}

		offset += ir->curTable->symbols[var_name].offset;

		if (offset >= 0) {
			offset += 8; // ebp + return address 共8字节
		}
		result = std::to_string(offset)+"(%ebp)";
	}
	return result; 
}

void mov_command(string src,string src_assem, string dst,string dst_assem, ofstream& fout) {
	if (src[0] != 'T' && dst[0] != 'T') {
		// 都不是临时变量, 全在内存上
		fout << "\tmovl " << src_assem << ", %eax\n";
		fout << "\tmovl %eax" << ", " << dst_assem << "\n";
	}
	else {
		fout << "\tmovl " << src_assem << ", " << dst_assem << "\n";
	}
}

/// <summary>
/// code gen process
/// </summary>
/// <param name="ir"></param>
/// <param name="fout"></param>
void CodeGen::codeGen(IR* ir, DestCode& dest_code)
{
	auto& fout = dest_code.fout;

	fout << "\t.section .rdata,\"dr\"\n";
	for (auto iter : ir->stringTable) {
		fout << iter.second << ":\n";
		string str = iter.first;

		fout << "\t.ascii " << str.substr(0,str.length()-1) << "\\0\"\n";
	}

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

				// 空间分配
				auto& table = ir->symbolTables.at(label); // 切换到对应的symboltable
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
			if (arg1 == "printf") {
				// 如果是 call output , call 的字符串label放在
				fout << "\tcall ";
				fout << "_printf" << "\n";
			}
			else{
				fout << "\tcall ";
				fout << arg1 << "\n";
				if (dst != "-") {
					// 赋值
					fout << "\tmovl %eax, " << dst_assembly << "\n"; // TODO: 临时变量处理
				}
			}
		}
		else if (op == "param") {
			// 传参
			fout << "\tpush " << arg1_assembly<< "\n";
		}
		else if (op == "=") {
			//fout << "\tmovl " << arg1_assembly << ", " << dst_assembly << "\n";
			mov_command(arg1, arg1_assembly, dst, dst_assembly, fout);
		}
		else if (op == "+" || op == "-") {
			//fout << "\tmovl " << arg1_assembly << ", " << dst_assembly << "\n";
			mov_command(arg1, arg1_assembly, dst, dst_assembly, fout);
			fout << "\t" << inst_map.at(op) <<" "<< arg2_assembly << ", " << dst_assembly << "\n";
		}
		else if (op == "*" || op == "/") {
			fout << "\tmovl " << arg2_assembly << ", " << "%eax" << "\n";
			//bool exist_edx = dest_code.allocation["%edx"] != "";
			//if (exist_edx) {
				// 乘除结果存放在eax和edx中，需要保存edx值
				//fout << "\tpush " << "%edx\n";
			//}
			//fout << "\t" << inst_map.at(op) << " " << arg2_assembly << "\n";
			//if (exist_edx) {
				//fout << "\tpop %edx\n";
			//}
			fout << "\timull " << arg1_assembly << ", " << "%eax\n";
			fout << "\tmovl " << "%eax, " << dst_assembly << "\n";
		}
		else if (op == "jz") {
			int goto_index = std::stoi(dst);
			string label = ir->quads[goto_index-IR::QUAD_BEGIN].label; 
			if (label == "") {
				// 
				std::cerr << "No Label for jz command\n"; 
				continue;
			}
			fout << "\tmovl $0, %eax\n";
			fout << "\tcmpl " << arg1_assembly << ", %eax\n";
			fout << "\tjz " << label << '\n';
		}
		else if (op == "j"){
			int goto_index = std::stoi(dst);
			string label = ir->quads[goto_index-IR::QUAD_BEGIN].label; 
			if (label == "") {
				// 
				std::cerr << "No Label for j command\n"; 
				continue;
			}
			fout << "\tjmp " << label << '\n';
		}
		else if (op == "j>" || op == "j<" || op == "jLE_OP" || op == "jGE_OP") {
			if (arg1[0] >= '0' && arg1[0] <= '9') {
				fout << "\tmovl " << arg1_assembly << " ,%eax\n";
				fout << "\tcmpl " << arg2_assembly << " ,%eax\n";
			}
			else {
				fout << "\tcmpl " << arg2_assembly << " ," << arg1_assembly << "\n";
			}
			if (op == "jLE_OP") {
				op = "jle";
			}
			else if (op == "jGE_OP") {
				op = "jge";
			}
			else if (op == "j>") {
				op = "jl";
			}
			else if (op == "j>") {
				op = "jg";
			}
			int goto_index = std::stoi(dst);
			string label = ir->quads[goto_index - IR::QUAD_BEGIN].label;
			fout << "\t" << op << ' ' << label << '\n';
		}
	}
}
