#include "stdafx.h"
#include "compile.h"

using namespace compiler;

SwitchStatement::SwitchCaseEntry::SwitchCaseEntry()
	: mIsDefaultSection(false)
{
}

IMPLEMENT_OBJINFO(SwitchStatement,Statement)

SwitchStatement::SwitchStatement()
{
}

SwitchStatement::~SwitchStatement()
{
	std::for_each(mCaseList.begin(), mCaseList.end(), 
		[this](SwitchCaseEntry *e)
	{
		ClearSwitchCaseEntry(e);
		delete e;
	});
}

void SwitchStatement::ClearSwitchCaseEntry(SwitchCaseEntry *entry)
{
	std::for_each(entry->mStatementBlocks.begin(), entry->mStatementBlocks.end(),
		[](StatementBlock *sb)
	{
		delete sb;
	});
}

int SwitchStatement::OnCaseOrDefault(SimpleCScriptEngContext *context, bool isDefault)
{
	int result;
	Symbol symbol;
	char c;

	SwitchCaseEntry *caseEntry = new SwitchCaseEntry;
	PostfixExpression caseExp;

	StatementBlock *sb;
	do {
		if (!isDefault)
		{
			if ((result = context->ParseExpressionEndWith(c, &caseExp, ":")) < 0)
				break;
			if (caseExp.isEmpty()) {
				result = -1;
				break;
			}
			if (!caseExp.IsConstIntergerExpression(caseEntry->mIntergerConst))
			{
				SCRIPT_TRACE("Invalid const integer expression.\n");
				result = -1;
				break;
			}
			if (HasSameCaseValue(caseEntry->mIntergerConst))
			{
				SCRIPT_TRACE("case value dumplicate.\n");
				result = -1;
				break;
			}
			mCaseValueSet.insert(caseEntry->mIntergerConst);
		}
		else
		{
			caseEntry->mIsDefaultSection = true;
			if ((result = context->GetNextSymbol(symbol)) != 0)
			{
				if (result < 0)
					break;
				result = -1;
				break;
			}
			if (symbol.symbolOrig != ":")
			{
				result = -1;
				break;
			}
		}

		while (1)
		{
			if ((result = context->GetNextSymbol(symbol)) != 0)
				RETHELP(result);
			if (symbol.symbolOrig == "case" || symbol.symbolOrig == "default" || symbol.symbolOrig == "}")
			{
				context->GoBack();
				break;
			}

			bool mayLackOfBrace = true;
			if (symbol.symbolOrig == "{")
				mayLackOfBrace = false;
			context->GoBack();

			sb = new StatementBlock;
			if ((result = sb->Compile(this, context, mayLackOfBrace)) < 0)
			{
				delete sb;
				break;
			}
			if (mayLackOfBrace)
			{
				std::list<Statement*> &sl = sb->GetStatementList();
				for (auto iter = sl.begin(); iter != sl.end(); iter++)
				{
					if ((*iter)->isInheritFrom(OBJECT_INFO(DeclareStatement)))
					{
						SCRIPT_TRACE("Do not declare variable in case or default block.\n");
						delete sb;
						result = -1;
						break;
					}
				}
				
				if (result < 0)
					break;
			}
			caseEntry->mStatementBlocks.push_back(sb);
		}
	} while (0);

	if (result < 0) {
		ClearSwitchCaseEntry(caseEntry);
		delete caseEntry;
	}
	else
		mCaseList.push_back(caseEntry);

	return result;
}

int SwitchStatement::Compile(Statement *parent, SimpleCScriptEngContext *context)
{
	char c;
	int result;
	Symbol symbol;

	//SCRIPT_TRACE("The switch statement has not been implement yet.\n");
	//return -1;

	mParentBlock = parent;

	if ((result = context->GetNextSymbol(symbol)) != 0)
		RETHELP(result);

	if ((result = context->ParseExpressionEndWith(c, &mSwitchExpression, ")")) < 0)
		return result;

	if ((result = context->GetNextSymbol(symbol)) != 0)
		RETHELP(result);
	if (symbol.symbolOrig != "{")
		return -1;

	// case��default������}
	if ((result = context->GetNextSymbol(symbol)) != 0)
		RETHELP(result);

	bool hasDefault = false;
	while (symbol.symbolOrig != "}")
	{
		if (symbol.type == Symbol::Keywords)
		{
			switch (symbol.keywordsType)
			{
			case KeywordsTransTable::CK_CASE:
				if ((result = OnCaseOrDefault(context, false)) < 0)
					return result;
				break;

			case KeywordsTransTable::CK_DEFAULT:
				if (hasDefault)
					return -1;
				if ((result = OnCaseOrDefault(context, true)) < 0)
					return result;
				hasDefault = true;
				break;

			default:
				return -1;
			}
		}
		else
			return -1;
		if ((result = context->GetNextSymbol(symbol)) != 0)
			RETHELP(result);
	}

	return 0;
}

// switch�����C�����������
// ����ʹ�����µ��㷨������switch��ָ���
// 1. ������ж��ı��ʽ��ֵ
// 2. ��ת���Ƚ����ȥ�Ƚϵ�һ������ֵ�Ƿ�ͱ��ʽ��ֵ��ͬ
//  2.1 �����ͬ�������¸��Ƚ����ȥ�ж��Ƿ���ͬ
//  2.2 �����ͬ����ת����case���Ĵ��뿪ʼ��
// 3. ������Ӧ�����顣��Ȼ�������Ϳ�֮�����û��break��䣬���ǻ�˳��������ȥ��
// 4. ����

int SwitchStatement::GenerateInstruction(CompileResult *compileResult)
{
	int result;
	std::list<int> lastPosToFillList;
	SwitchCaseEntry *entryDefault = nullptr;

	GenerateInstructionHelper gih(compileResult);
	
	uint32_t jumpToInitVarToCompare = gih.Insert_jump_Instruction(0);

	for (auto iter = mCaseList.begin(); iter != mCaseList.end(); iter++)
	{
		SwitchCaseEntry *entry = *iter;
		if (entry->mIsDefaultSection)
		{
			entryDefault = entry;
		}
		entry->beginPos = compileResult->SaveCurrentCodePosition();
		for (auto sbIter = entry->mStatementBlocks.begin();
			sbIter != entry->mStatementBlocks.end(); sbIter++)
		{
			(*sbIter)->SetSaveFrame(false);
			if ((result = (*sbIter)->GenerateInstruction(compileResult)) < 0)
				return result;
		}
	}

	// �������
	lastPosToFillList.push_back(gih.Insert_jump_Instruction(0));

	gih.SetCode(jumpToInitVarToCompare, compileResult->SaveCurrentCodePosition());

	// ������ʽ
	if ((result = mSwitchExpression.GenerateInstruction(this, compileResult)) < 0)
		return result;

	uint32_t jumpToCompareBeginToFill = gih.Insert_jump_Instruction(0);


	// ��ʼ�Ƚ�
	uint32_t compareLastPosToFill = -1;
	uint32_t lastCaseToFill = -1;
	bool hasFirstNotDefault = false;
	for (auto iter = mCaseList.begin(); iter != mCaseList.end(); iter++)
	{
		if (lastCaseToFill != -1)
			gih.SetCode(lastCaseToFill, compileResult->SaveCurrentCodePosition());
		lastCaseToFill = -1;

		if ((*iter)->mIsDefaultSection)
		{
			compareLastPosToFill = gih.Insert_jump_Instruction(0);
		}
		else
		{
			if (!hasFirstNotDefault)
			{
				// ����default������ǰ��ģ���һ��Ͳ�����default�Ĵ������У�����
				// ������������һ������default�������
				gih.SetCode(jumpToCompareBeginToFill, compileResult->SaveCurrentCodePosition());
				hasFirstNotDefault = true;
			}
			// TODO: ����ָ����Ҫ��֤������������ѹ���ջ������Ԫ��
			gih.Insert_push_Instruction(0);
			gih.Insert_createInt_Instruction((*iter)->mIntergerConst);
			gih.Insert_equal_Instruction();
			lastCaseToFill = gih.Insert_jz_Instruction(0);
			gih.Insert_jump_Instruction((*iter)->beginPos);
		}
	}
	if (lastCaseToFill != -1)
		gih.SetCode(lastCaseToFill, compileResult->SaveCurrentCodePosition());
	if (compareLastPosToFill != -1)
		gih.SetCode(compareLastPosToFill, compileResult->SaveCurrentCodePosition());
	if (entryDefault)
	{
		// �����default��䣬������default����λ��
		gih.Insert_jump_Instruction(entryDefault->beginPos);
	}
	// �������������λ��
	lastPosToFillList.push_back(gih.Insert_jump_Instruction(0));

	uint32_t switchEndPos = compileResult->SaveCurrentCodePosition();

	FillBreakList(compileResult, switchEndPos);

	for (auto lfIter = lastPosToFillList.begin();
		lfIter != lastPosToFillList.end(); lfIter++)
	{
		gih.SetCode(*lfIter, switchEndPos);
	}

	return result;
}

int SwitchStatement::GenerateBreakStatementCode(BreakStatement *bs, CompileResult *compileResult)
{
	GenerateInstructionHelper gih(compileResult);

	// ��ת���
	mBreakToFillList.push_back(gih.Insert_jump_Instruction(0));
	return 0;
}
