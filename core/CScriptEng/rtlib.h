#pragma once
#include "rtTypes.h"
#include "compile.h"

namespace runtime {
	class sleepObj : public runtime::baseObjDefault
	{
	public:
		sleepObj();
		virtual uint32_t GetObjectTypeId() const;
		virtual runtimeObjectBase* doCall(runtime::doCallContext *context);
	};

	class rtLibHelper
	{
	public:
		static bool RegistObjNames(compiler::FunctionStatement *sb);
		static bool RegistRuntimeObjs(runtimeContext *context);
	};

	// ���ڴ�ӡ����Ķ���
	class printObj : public runtime::baseObjDefault
	{
	private:
		bool mPrintLine;

	public:
		printObj();

		void SetIsPrintLine(bool isPrintLine);

		virtual runtimeObjectBase* doCall(runtime::doCallContext *context);
	};

	// ���������
	class randObj : public runtime::baseObjDefault
	{
	public:
		virtual runtimeObjectBase* doCall(runtime::doCallContext *context);
	};

	class srandObj : public runtime::baseObjDefault
	{
	public:
		virtual runtimeObjectBase* doCall(runtime::doCallContext *context);
	};

	// sin����
	class sinObj : public runtime::baseObjDefault
	{
	public:
		virtual runtimeObjectBase* doCall(runtime::doCallContext *context);
	};
	
	class powfObj : public runtime::baseObjDefault
	{
	public:
		virtual runtimeObjectBase* doCall(runtime::doCallContext *context);
	};

	// ��ȡʱ��
	class timeObj : public runtime::baseObjDefault
	{
	public:
		virtual runtimeObjectBase* doCall(runtime::doCallContext *context);
	};

	class systemCallObject : public runtime::baseObjDefault
	{
	public:
		virtual runtimeObjectBase* doCall(runtime::doCallContext *context);
	};

	class unzipFileObj : public runtime::baseObjDefault
	{
	public:
		virtual runtimeObjectBase* doCall(runtime::doCallContext *context);
	};

	class zipFilesInDirectoryObj : public runtime::baseObjDefault
	{
	public:
		virtual runtimeObjectBase* doCall(runtime::doCallContext *context);
	};

	class csOpenFile : public runtime::baseObjDefault
	{
	public:
		virtual runtimeObjectBase* doCall(runtime::doCallContext *context);
	};
}
