#pragma once
#include "CScriptEng/CScriptEng.h"

#define TESTCALLBACKNAME "TestFunctionCallback"

class TestCallback : public runtime::runtimeObjectBase
{
public:
	TestCallback()
	{
	}

	virtual uint32_t GetObjectTypeId() const
	{
		return runtime::DT_UserTypeBegin;
	}

	virtual runtimeObjectBase* Add(const runtimeObjectBase *obj){ return NULL; }
	virtual runtimeObjectBase* Sub(const runtimeObjectBase *obj){ return NULL; }
	virtual runtimeObjectBase* Mul(const runtimeObjectBase *obj){ return NULL; }
	virtual runtimeObjectBase* Div(const runtimeObjectBase *obj){ return NULL; }

	// =��Ԫ����
	virtual runtimeObjectBase* SetValue(runtimeObjectBase *obj) { return NULL; }

	// ����.��������һԪ�ģ�
	virtual runtimeObjectBase* GetMember(const char *memName) { return NULL; }

	// docall����������һԪ���㣩
	virtual runtimeObjectBase* doCall(runtime::doCallContext *context)
	{
		if (context->GetParamCount() != 1)
			return this;

		if (context->GetParam(0)->GetObjectTypeId() != runtime::DT_function)
			return this;

		runtime::FunctionObject *f = static_cast<runtime::FunctionObject*>(context->GetParam(0));
		if (f->GetDesc()->paramCount != 1)
		{
			printf("Invalid param count for function %s\n",
				f->GetFuncName().c_str());
			return this;
		}
		// ���ûص�������������1������
		context->SetParamCount(1);
		runtime::intObject *iVal = new runtime::ObjectModule<runtime::intObject>;
		iVal->mVal = 100;
		context->PushObjectToStack(iVal);
		runtime::runtimeObjectBase *r = f->doCall(context);
		// ֮ǰ����TestFunctionCallbackʱ���ú�������һ���������ָ��������
		context->SetParamCount(1);
		return r;
	}

	// getindex����������һԪ���㣩
	virtual runtimeObjectBase* getIndex(int i) { return NULL; }
	// ����ת��Ϊ�ַ���
	virtual runtime::stringObject* toString() {
		runtime::stringObject *s = new runtime::ObjectModule<runtime::stringObject>;
		*s->mVal = "TestCallbackFunction";
		return s;
	}

	// �Ƚ�
	virtual bool isGreaterThan(runtimeObjectBase *obj) { return false; }

	virtual bool isEqual(runtimeObjectBase *obj) { return false; }
};
