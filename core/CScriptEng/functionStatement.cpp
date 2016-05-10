#include "stdafx.h"
#include "compile.h"

using namespace compiler;

IMPLEMENT_OBJINFO(FunctionStatement,StatementBlock)

FunctionStatement::FunctionStatement(const std::string &name)
	: mName(name)
{
}

int FunctionStatement::ParseParamList(SimpleCScriptEngContext *context)
{
	int parseResult;
	Symbol symbol;

	if ((parseResult = context->GetNextSymbol(symbol)) != 0)
		RETHELP(parseResult);
	if (symbol.symbolOrig == ")")
		return 0;

	context->GoBack();
	for (;;)
	{
		//int dataType;

		//if ((parseResult = context->GetNextSymbol(symbol)) != 0)
		//	RETHELP(parseResult);
		//if (symbol.type != symbol.Keywords
		//	|| !KeywordsTransTable::isDataType(symbol.keywordsType))
		//	return -1;
		//dataType = symbol.keywordsType;
		// �βΣ��������
		if ((parseResult = context->GetNextSymbol(symbol)) != 0)
			RETHELP(parseResult);
		// �βα����Ƿ�������
		if (symbol.type != Symbol::CommonSymbol)
			return -1;

		Param param;
		param.name = symbol.symbolOrig;
		mParamList.emplace_back(param);

		// ����-1��ʹ�����ɴ���ʱ���������ɴ�������Ĵ��룬��Ϊ
		// ѹ����ǲ���
		PushName(symbol.symbolOrig.c_str(), -1);//dataType);

		if ((parseResult = context->GetNextSymbol(symbol)) != 0)
			RETHELP(parseResult);
		if (symbol.symbolOrig == ")")
			break;
		if (symbol.symbolOrig != ",")
			return -1;
	}
	return 0;
}

int FunctionStatement::Compile(Statement *parent, SimpleCScriptEngContext *context)
{
	int parseResult;
	Symbol symbol;

	if ((parseResult = parent->GetBlockParent()->PushName(mName.c_str(), compiler::KeywordsTransTable::CK_FUNCTION)) < 0)
		return parseResult;

	if ((parseResult = context->GetNextSymbol(symbol)) != 0)
		RETHELP(parseResult);
	if (symbol.symbolOrig != "(")
		return -1;

	if ((parseResult = ParseParamList(context)) != 0)
		RETHELP(parseResult);

	if ((parseResult = context->GetNextSymbol(symbol)) != 0)
		RETHELP(parseResult);
	if (symbol.symbolOrig != "{")
		return -1;
	context->GoBack();

	if ((parseResult = __super::Compile(parent, context)) < 0)
		return parseResult;
	
	return parseResult;
}

int FunctionStatement::GenerateInstruction(CompileResult *compileResult)
{
	GenerateInstructionHelper gih(compileResult);

	uint32_t l, i;
	bool throughFunc = false;
	GetParent()->GetBlockParent()->FindName(mName.c_str(), throughFunc, l, i);

	// ����׼���ٲ���һ����ֵ������������
	gih.Insert_copyAtFrame_Instruction(throughFunc, l, i);
	uint32_t x = gih.Insert_createFunction_Instruction();
	uint32_t jumpToFuncEnd = gih.Insert_jump_Instruction(0);

	uint32_t functionStart = compileResult->SaveCurrentCodePosition();
	runtime::FunctionDesc fd;
	fd.len = 0;
	fd.paramCount = mParamList.size();
	fd.stringId = gih.RegistName(mName.c_str());
	gih.InsertFunctionDesc(&fd);

	// ������������
	__super::SetSaveFrame(false);
	__super::SetCallLayerCounter(true);
	int r = __super::GenerateInstruction(compileResult);
	if (r < 0)
		return r;

	uint32_t functionEnd = compileResult->SaveCurrentCodePosition();
	// �����ĳ���
	gih.SetCode(functionStart, (functionEnd - functionStart) * 4);
	gih.SetCode(jumpToFuncEnd, functionEnd);
	// �����Ŀ�ʼ��
	gih.SetCode(x, functionStart);
	gih.Insert_setVal_Instruction();

	return 0;
}
