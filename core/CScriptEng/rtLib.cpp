#include "stdafx.h"
#include "rtlib.h"
#include "vm.h"
#include "arrayType.h"
#include <math.h>
#include <time.h>
#include "notstd/zipWrapper.h"
#include "objType.h"
#include "svnInfo.h"

namespace runtime {
	///////////////////////////////////////////////////////////////////////////////

	sleepObj::sleepObj()
	{
	}

	uint32_t sleepObj::GetObjectTypeId() const
	{
		return DT_UserTypeBegin;
	}

	runtimeObjectBase* sleepObj::doCall(runtime::doCallContext *context)
	{
		if (context->GetParamCount() != 1)
			return NULL;

#if defined(PLATFORM_WINDOWS)
		::Sleep(context->GetUint32Param(0));
#else
		uint32_t milisecond = context->GetUint32Param(0);
		timespec ts =
		{
			(time_t)(milisecond / 1000),
			(milisecond % 1000) * 1000000,
		};
		nanosleep(&ts, NULL);
#endif

		return new ObjectModule<runtime::baseObjDefault>();
	}



	bool rtLibHelper::RegistCScriptRuntimeLib(compiler::SimpleCScriptEngContext *context)
	{
		printObj *println = new ObjectModule<printObj>;
		println->SetIsPrintLine(true);

		scriptAPI::ScriptLibReg slrs[] =
		{
			{ "null", NullTypeObject::CreateNullTypeObject(), },
			{ "getversion", new ObjectModule<getVersionObj>, },
			{ "CreateArray", new runtime::ObjectModule<runtime::CreateArrayObj>, },
			{ "sleep", new ObjectModule<sleepObj>, },
			{ "print", new ObjectModule<printObj>, },
			{ "println", println, },
			{ "rand", new ObjectModule<randObj>, },
			{ "srand", new ObjectModule<srandObj>, },
			{ "sin", new ObjectModule<sinObj>, },
			{ "pow", new ObjectModule<powfObj>, },
			{ "time", new ObjectModule<timeObj>, },
			{ "system", new ObjectModule<systemCallObject>, },
			{ "unzipFile", new ObjectModule<unzipFileObj>, },
			{ "zipFile", new ObjectModule<zipFilesInDirectoryObj>, },
			{ "csOpenFile", new ObjectModule<csOpenFile>, },
			{ "DeleteFile", new ObjectModule<DeleteFileObj>, },
		};

		return context->GetLibRegister().RegistLib(
			"cscriptRuntime", slrs, sizeof(slrs) / sizeof(slrs[0])) >= 0;
	}

	///////////////////////////////////////////////////////////////////////////////

	runtimeObjectBase* getVersionObj::doCall(doCallContext *context)
	{
		intObject *r = new ObjectModule<intObject>;
		r->mVal = SVN_VERSION;
		return r;
	}

	///////////////////////////////////////////////////////////////////////////////

	printObj::printObj()
		: mPrintLine(false)
	{
	}

	void printObj::SetIsPrintLine(bool isPrintLine)
	{
		mPrintLine = isPrintLine;
	}

	runtimeObjectBase* printObj::doCall(runtime::doCallContext *context)
	{
		if (context->GetParamCount() != 1)
			return NULL;
		runtime::stringObject *s = context->GetParam(0)->toString();
		if (s)
		{
			printf("%s%s", s->mVal->c_str(), mPrintLine ? "\n" : "");
#if defined(PLATFORM_WINDOWS)
			OutputDebugStringA(s->mVal->c_str());
			if (mPrintLine)
				OutputDebugStringA("\n");
#endif
			return s;
		}
		return new runtime::ObjectModule<runtime::baseObjDefault>();
	}

	///////////////////////////////////////////////////////////////////////////////

	runtimeObjectBase* randObj::doCall(runtime::doCallContext *context)
	{
		runtime::floatObject *f = new runtime::ObjectModule<runtime::floatObject>;
		f->mVal = ((floatObject::InnerDataType)(rand() % RAND_MAX)) / RAND_MAX;
		return f;
	}

	///////////////////////////////////////////////////////////////////////////////

	runtimeObjectBase* srandObj::doCall(runtime::doCallContext *context)
	{
		switch (context->GetParamCount())
		{
		case 0:
			srand(unsigned(time(NULL)));
			break;

		case 1:
			srand(context->GetUint32Param(0));
			break;

		default:
			printf("srand fail\n");
			return NULL;
		}
		return this;
	}

	///////////////////////////////////////////////////////////////////////////////

	runtimeObjectBase* sinObj::doCall(runtime::doCallContext *context)
	{
		if (context->GetParamCount() != 1)
			return NULL;

		runtime::floatObject *f = new runtime::ObjectModule<runtime::floatObject>;

		f->mVal = static_cast<float>(sin(context->GetDoubleParam(0)));
		return f;
	}

	///////////////////////////////////////////////////////////////////////////////

	runtimeObjectBase* powfObj::doCall(runtime::doCallContext *context)
	{
		runtime::floatObject *f = new runtime::ObjectModule<runtime::floatObject>;
		f->mVal = 1.f;

		if (context->GetParamCount() != 2)
			return f;

		f->mVal = (float)pow(context->GetDoubleParam(0), context->GetDoubleParam(1));
		return f;
	}

	///////////////////////////////////////////////////////////////////////////////

	runtimeObjectBase* timeObj::doCall(runtime::doCallContext *context)
	{
		if (context->GetParamCount() != 0)
		{
			SCRIPT_TRACE("timeObj::doCall: Invalid param count.\n");
			return NULL;
		}

		time_t t = ::time(NULL);
		struct tm r;
#if defined(PLATFORM_WINDOWS)
		if (localtime_s(&r, &t))
			return NULL;
#else
		struct tm *x = localtime(&t);
		if (!x)
			return NULL;
		r = *x;
#endif

		runtime::arrayObject *a = new runtime::ObjectModule<runtime::arrayObject>;
		a->AddSub(runtime::intObject::CreateIntObject(r.tm_year + 1900));
		a->AddSub(runtime::intObject::CreateIntObject(r.tm_mon + 1));
		a->AddSub(runtime::intObject::CreateIntObject(r.tm_mday));
		a->AddSub(runtime::intObject::CreateIntObject(r.tm_hour));
		a->AddSub(runtime::intObject::CreateIntObject(r.tm_min));
		a->AddSub(runtime::intObject::CreateIntObject(r.tm_sec));

		return a;
	}

	///////////////////////////////////////////////////////////////////////////////

	runtimeObjectBase* systemCallObject::doCall(runtime::doCallContext *context)
	{
		runtime::intObject *r = new runtime::ObjectModule<runtime::intObject>;
		r->mVal = -1;

		const char *cmd;
		if (context->GetParamCount() < 1
			|| (cmd = context->GetStringParam(0)) == nullptr)
			return r;

		r->mVal = ::system(cmd);
		return r;
	}

	///////////////////////////////////////////////////////////////////////////////

	runtimeObjectBase* unzipFileObj::doCall(runtime::doCallContext *context)
	{
		runtime::intObject *r = new runtime::ObjectModule<runtime::intObject>;
		r->mVal = -notstd::zlibHelper::E_PARAMERROR;

		notstd::zlibHelper::UnzipParam up;
		memset(&up, 0, sizeof(up));

		if (context->GetParamCount() < 2)
			return r;
		if (context->GetParamCount() >= 3)
		{
			uint32_t ap = context->GetArrayParamElemCount(2);
			if (ap > 0)
			{
				up.showVerb = context->GetInt32ElemOfArrayParam(2, 0);
			}
		}
		std::string dir, zipFile;
		const char *p1 = context->GetStringParam(0);
		const char *p2 = context->GetStringParam(1);
		if (!p1 || !p2)
		{
			r->mVal = -notstd::zlibHelper::E_PARAMERROR;
			return r;
		}
		dir = p1; zipFile = p2;
		r->mVal = notstd::zlibHelper::unzipToDirectory(dir, zipFile, &up);
		return r;
	}

	///////////////////////////////////////////////////////////////////////////////

	runtimeObjectBase* zipFilesInDirectoryObj::doCall(runtime::doCallContext *context)
	{
		runtime::intObject *r = new runtime::ObjectModule<intObject>;
		r->mVal = -notstd::zlibHelper::E_PARAMERROR;

		if (context->GetParamCount() < 2)
			return r;

		const char *p1 = context->GetStringParam(0);
		const char *p2 = context->GetStringParam(1);
		if (!p1 || !p2)
		{
			r->mVal = -notstd::zlibHelper::E_PARAMERROR;
			return r;
		}
		std::string dir = p2, zipFilePath = p1;
		r->mVal = notstd::zlibHelper::zipDirectoryToFile(
			zipFilePath, dir);

		return r;
	}

	///////////////////////////////////////////////////////////////////////////////

	class FileObject;

	class WriteFileObject : public runtime::baseObjDefault
	{
		friend class FileObject;

	private:
		FileObject *mFileObject;

	public:
		virtual runtime::runtimeObjectBase* doCall(doCallContext *context);
	};

	class FileReadLineObject : public runtime::baseObjDefault
	{
		friend class FileObject;

	private:
		FileObject *mFileObject;

	public:
		virtual runtime::runtimeObjectBase* doCall(doCallContext *context);
	};

	class CloseFileObject : public runtime::baseObjDefault
	{
		friend class FileObject;

	private:
		FileObject *mFileObject;

	public:
		virtual runtime::runtimeObjectBase* doCall(doCallContext *context);
	};

	class FileObject : public runtime::baseObjDefault
	{
		friend class csOpenFile;

	private:
		FILE *mFile;

	public:
		FileObject()
			: mFile(nullptr)
		{
		}
		virtual runtimeObjectBase* GetMember(const char *memName)
		{
			if (!strcmp(memName, "WriteFile"))
			{
				WriteFileObject *o = new runtime::ContainModule<WriteFileObject>(this);
				o->mFileObject = this;
				return o;
			}
			if (!strcmp(memName, "ReadLine"))
			{
				FileReadLineObject *o = new ContainModule<FileReadLineObject>(this);
				o->mFileObject = this;
				return o;
			}
			if (!strcmp(memName, "Close"))
			{
				CloseFileObject *o = new runtime::ContainModule<CloseFileObject>(this);
				o->mFileObject = this;
				return o;
			}
			return runtime::baseObjDefault::GetMember(memName);
		}
		FILE* getFileHandle() { return mFile; }
	};

	runtimeObjectBase* csOpenFile::doCall(runtime::doCallContext *context)
	{
		if (context->GetParamCount() < 1)
			return NullTypeObject::CreateNullTypeObject();

		const char *fn = context->GetStringParam(0);
		if (!fn)
			return NullTypeObject::CreateNullTypeObject();

		std::string of = "wb";
		if (context->GetParamCount() == 2)
		{
			const char *openFlag = context->GetStringParam(1);
			if (!openFlag)
				return NullTypeObject::CreateNullTypeObject();
			of = openFlag;
		}

		std::string dir = fn;
		std::string::size_type f = dir.rfind(notstd::zlibHelper::mSplash[0]);
		if (f != dir.npos)
			dir.erase(f);
		notstd::zlibHelper::CreateDirectoryR(dir);

		FILE *file = ::fopen(fn, of.c_str());
		if (!file)
			return NullTypeObject::CreateNullTypeObject();

		FileObject *fo = new runtime::ObjectModule<FileObject>;
		fo->mFile = file;
		return fo;
	}

	///////////////////////////////////////////////////////////////////////////////

	runtime::runtimeObjectBase* WriteFileObject::doCall(doCallContext *context)
	{
		const char *s;
		if (context->GetParamCount() != 1
			|| ((s = context->GetStringParam(0)) == nullptr))
			return nullptr;
		runtime::intObject *r = new runtime::ObjectModule<runtime::intObject>;
		r->mVal = fwrite(s, 1, strlen(s), mFileObject->getFileHandle());
		return r;
	}

	runtime::runtimeObjectBase* FileReadLineObject::doCall(doCallContext *context)
	{
		NullTypeObject *retNull = NullTypeObject::CreateNullTypeObject();
		if (context->GetParamCount() != 1
			|| context->GetParam(0)->GetObjectTypeId() != DT_string)
			return retNull;
		stringObject *s = static_cast<stringObject*>(context->GetParam(0));
		if (!notstd::StringHelper::ReadLine<256>(mFileObject->getFileHandle(), *s->mVal, nullptr))
			return retNull;
		delete retNull;
		return s;
	}

	runtime::runtimeObjectBase* CloseFileObject::doCall(doCallContext *context)
	{
		runtime::intObject *r = new runtime::ObjectModule<runtime::intObject>;
		r->mVal = fclose(mFileObject->getFileHandle());
		return r;
	}

	///////////////////////////////////////////////////////////////////////////////

	runtimeObjectBase* DeleteFileObj::doCall(doCallContext *context)
	{
		NullTypeObject *retNull = NullTypeObject::CreateNullTypeObject();
		if (context->GetParamCount() != 1)
			return retNull;

		const char *filePathName = context->GetStringParam(0);
		if (!filePathName)
			return retNull;

		delete retNull;

		intObject *r = intObject::CreateBaseTypeObject<intObject>(false);
		r->mVal = ::unlink(filePathName);
		return r;
	}

	///////////////////////////////////////////////////////////////////////////////
}
