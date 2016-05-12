#include "stdafx.h"
#include "compile.h"

using namespace compiler;


IMPLEMENT_OBJINFO(IfConditionStatement,Statement)

IfConditionStatement::IfConditionStatement()
	: mElseStatement(nullptr)
{
}

IfConditionStatement::~IfConditionStatement()
{
	std::for_each(mElseIfStatementList.begin(), mElseIfStatementList.end(), 
		[](ElseIfBlock &bl)
	{
		delete bl.judg;
		delete bl.sb;
	});
	mElseIfStatementList.clear();

	if (mElseStatement)
		delete mElseStatement;
}

int IfConditionStatement::Compile(Statement *parent, SimpleCScriptEngContext *context)
{
	int result;
	Symbol symbol;

	mParentBlock = parent;

	result = context->GetNextSymbol(symbol);
	if (result != 0 || symbol.symbolOrig != "(")
		return -1;

	char c;
	result = context->ParseExpressionEndWith(c, &mJudgementStatement, ")");
	if (result != 0)
		return -1;

	bool hasBrace = false;
	if ((result = context->GetNextSymbol(symbol)) != 0)
		return -1;
	if (symbol.symbolOrig == "{")
		hasBrace = true;
	else
		context->GoBack();
	result = mTrueBlock.Compile(this, context, hasBrace);
	if (result < 0)
		return result;

	while (1)
	{
		result = context->GetNextSymbol(symbol);
		// ��ʾû��else if��Ҳû��else��Ӧ�ý�����
		if (result > 0)
			return 0;

		if (symbol.type == Symbol::Keywords && symbol.symbolOrig == "else")
		{
			result = context->GetNextSymbol(symbol);
			if (result != 0)
				return -1;
			if (symbol.symbolOrig == "if")
			{
				// ��else if���
				if ((result = context->GetNextSymbol(symbol)) != 0)
					return -1;
				if (symbol.symbolOrig != "(")
					return -1;
				ElseIfBlock eib;
				eib.judg = new PostfixExpression;
				eib.sb = new StatementBlock;
				if ((result = context->ParseExpressionEndWith(c, eib.judg, ")")) != 0)
					return -1;

				bool hasBrace = false;
				if ((result = context->GetNextSymbol(symbol)) != 0)
					return -1;
				if (symbol.symbolOrig == "{")
					hasBrace = true;
				else
					context->GoBack();
				if ((result = eib.sb->Compile(this, context, hasBrace)) != 0)
					return -1;
				mElseIfStatementList.push_back(eib);
				//StatementBlock *elseifSB = new StatementBlock;
				//result = elseifSB->Compile(this, context, true);
				//if (result != 0)
				//	return -1;
				//mElseIfStatementList.push_back(elseifSB);
			}
			else
			{
				// ��else���
				context->GoBack();
				mElseStatement = new StatementBlock;

				bool hasBrace = false;
				if ((result = context->GetNextSymbol(symbol)) != 0)
					return -1;
				if (symbol.symbolOrig == "{")
					hasBrace = true;
				else
					context->GoBack();
				result = mElseStatement->Compile(this, context, hasBrace);
				if (result != 0)
				{
					delete mElseStatement;
					mElseStatement = nullptr;
					return -1;
				}
				break;
			}
		}
		else
		{
			context->GoBack();
			return 0;
		}
	}

	return 0;
}

int IfConditionStatement::GenerateInstruction(CompileResult *compileResult)
{
	int r;

	// ��¼ElseIf�Ӿ���Ҫ�������תλ��
	struct ElseIfPos
	{
		//uint32_t toElse; // ���λ��Ҫ��дelse��俪ʼ�ĵط�
		uint32_t toEndif;// ���λ�ü�¼Ҫ��дif������λ�õĵط�
	};
	std::list<ElseIfPos> elseIfPosList;

	GenerateInstructionHelper gih(compileResult);

	if ((r = mJudgementStatement.CreateExpressionTree()) < 0)
		return r;
	if ((r = mJudgementStatement.GenerateInstruction(this, compileResult)) < 0)
		return r;

	// ��λ������д��else�Ӿ��λ��
	uint32_t lastJumpPosToFill = gih.Insert_jz_Instruction(0);

	// if�Ӿ�Ĵ���
	if ((r = mTrueBlock.GenerateInstruction(compileResult)) < 0)
		return r;

	uint32_t jumpIfTailPos = gih.Insert_jump_Instruction(0);

	for (auto iter = mElseIfStatementList.begin(); iter != mElseIfStatementList.end(); iter++)
	{
		ElseIfPos pos;

		gih.SetCode(lastJumpPosToFill, compileResult->SaveCurrentCodePosition());

		if ((r = (*iter).judg->CreateExpressionTree()) < 0)
			return r;
		// �ж����
		if ((r = (*iter).judg->GenerateInstruction(this, compileResult)) < 0)
			return r;
		lastJumpPosToFill = gih.Insert_jz_Instruction(0);

		// else if����
		if ((r = (*iter).sb->GenerateInstruction(compileResult)) < 0)
			return r;
		pos.toEndif = gih.Insert_jump_Instruction(0);

		elseIfPosList.push_back(pos);
	}

	gih.SetCode(lastJumpPosToFill, compileResult->SaveCurrentCodePosition());
	if (mElseStatement)
	{
		if ((r = mElseStatement->GenerateInstruction(compileResult)) < 0)
			return r;
	}
	uint32_t endIfPos = compileResult->SaveCurrentCodePosition();

	gih.SetCode(jumpIfTailPos, endIfPos);

	for (auto iter = elseIfPosList.begin(); iter != elseIfPosList.end(); iter++)
	{
		gih.SetCode((*iter).toEndif, endIfPos);
	}

	return 0;
}
