#include "stdafx.h"
#include "compile.h"
#include "vm.h"
#include <io.h>
#include <algorithm>

#ifdef min
#undef min
#endif

using namespace scriptAPI;
using namespace runtime;

///////////////////////////////////////////////////////////////////////////////

StringStream::StringStream(const char *str, size_t size)
	: mBuf(new char[size+1])
	, mSize(size)
	, mCur(mBuf)
{
	memcpy(mBuf, str, size + 1);
}

StringStream::~StringStream()
{
	delete[] mBuf;
}

void StringStream::Close()
{
}

int StringStream::Flush()
{
	return 0;
}

int StringStream::Read(uint8_t *data, int offset, int count)
{
	if (offset >= static_cast<int>(mSize))
		return 0;
	int rSize = std::min(count, int(mSize - offset));
	memcpy(data, mBuf + offset, rSize);
	return rSize;
}

int64_t StringStream::Seek(int64_t offset, int origPosition)
{
	assert(0);
	return 0;
}

int StringStream::SetLength(int64_t length)
{
	assert(0);
	return 0;
}

int StringStream::Write(const uint8_t *data, int offset, int count)
{
	assert(0);
	return 0;
}

///////////////////////////////////////////////////////////////////////////////

FileStream::FileStream(const char *filePathName)
	: mFile(nullptr)
{
	Open(filePathName);
}

FileStream::FileStream()
	: mFile(nullptr)
{
}

FileStream::~FileStream()
{
	Close();
}

int FileStream::Open(const char *filePathName)
{
	Close();
	return ::fopen_s(&mFile, filePathName, "rb");
}

void FileStream::Close()
{
	if (mFile)
	{
		::fclose(mFile);
		mFile = nullptr;
	}
}

int FileStream::Flush()
{
	if (mFile)
		return fflush(mFile);
	return -1;
}

int FileStream::Read(uint8_t *data, int offset, int count)
{
	if (mFile)
	{
		return (int)::fread(reinterpret_cast<void*>(data + offset), 1, count, mFile);
	}
	return -1;
}

int64_t FileStream::Seek(int64_t offset, int origPosition)
{
	if (mFile)
	{
		if (::_fseeki64(mFile, offset, origPosition))
			return 0;
		return _ftelli64(mFile);
	}
	return 0;
}

int FileStream::SetLength(int64_t length)
{
	if (mFile)
	{
		HANDLE h = reinterpret_cast<HANDLE>(_get_osfhandle(_fileno(mFile)));
		if (!::SetFileValidData(h, length))
			return -1;
		return 0;
	}
	return -1;
}

int FileStream::Write(const uint8_t *data, int offset, int count)
{
	if (mFile)
	{
		return (::fwrite(reinterpret_cast<const void*>(data + offset), 1, count, mFile) == count) ? 0 : -1;
	}
	return -1;
}

///////////////////////////////////////////////////////////////////////////////

void SimpleCScriptEng::Init()
{
	compiler::SimpleCScriptEngContext::Init();
	compiler::OperatorHelper::InitEntryTable();
}

void SimpleCScriptEng::Term()
{
	compiler::OperatorHelper::TermEntryTable();
	compiler::SimpleCScriptEngContext::Term();
}

///////////////////////////////////////////////////////////////////////////////

ScriptCompiler::ScriptCompiler()
	: mCompilerContext(new compiler::SimpleCScriptEngContext)
{
}

ScriptCompiler::~ScriptCompiler()
{
	delete mCompilerContext;
}

int ScriptCompiler::PushName(const char *name)
{
	return mCompilerContext->PushName(name);
}

HANDLE ScriptCompiler::Compile(ScriptSourceCodeStream *stream, bool end)
{
	return mCompilerContext->Compile(stream, end);
}

int ScriptCompiler::SaveConstStringTableInResultToFile(HANDLE crHandle, FILE *file)
{
	return reinterpret_cast<compiler::CompileResult*>(crHandle)->SaveConstStringDataToFile(file);
}

int ScriptCompiler::LoadConstStringTableToResultFromFile(HANDLE crHandle, FILE *file)
{
	return reinterpret_cast<compiler::CompileResult*>(crHandle)->LoadConstStringDataFromFile(file);
}

int ScriptCompiler::SaveCodeToFile(HANDLE crHandle, FILE *file)
{
	return reinterpret_cast<compiler::CompileResult*>(crHandle)->SaveCodeToFile(file);
}

int ScriptCompiler::LoadCodeFromFile(HANDLE crHandle, FILE *file)
{
	return reinterpret_cast<compiler::CompileResult*>(crHandle)->LoadCodeFromFile(file);
}

HANDLE ScriptCompiler::CopyAndClearCompileResult(HANDLE compileResult)
{
	compiler::CompileResult *cr = reinterpret_cast<compiler::CompileResult*>(compileResult);
	compiler::CompileResult::ScriptCode &sc = cr->GetCode();

	CompileCode *code = new CompileCode;
	uint32_t count = (uint32_t)sc.size();
	code->sizeInUint32 = count;
	code->code = new uint32_t[count + 1];
	memcpy(code->code, &sc[0], sizeof(uint32_t) * count);
	return code;
}

void ScriptCompiler::ReleaseCompileResult(HANDLE result)
{
	compiler::CompileResult *compileResult = reinterpret_cast<compiler::CompileResult*>(result);
	delete compileResult;
}

void ScriptCompiler::ReleaseCompileCode(HANDLE code)
{
	CompileCode *cc = reinterpret_cast<CompileCode*>(code);
	delete [] cc->code;
	delete cc;
}

///////////////////////////////////////////////////////////////////////////////

ScriptRuntimeContext::ScriptRuntimeContext(uint32_t stackSize, uint32_t stackFrameSize)
	: mContextInner(nullptr)
{
	VMConfig conf;
	conf.stackFrameSize = stackFrameSize;
	conf.stackSize = stackSize;
	mContextInner = new runtime::runtimeContext(&conf);
}

ScriptRuntimeContext::~ScriptRuntimeContext()
{
	delete mContextInner;
}

int ScriptRuntimeContext::PushRuntimeObject(runtime::runtimeObjectBase *obj)
{
	mContextInner->PushObject(obj);
	return 0;
}

int ScriptRuntimeContext::Execute(HANDLE compileResult)
{
	return mContextInner->Execute(reinterpret_cast<compiler::CompileResult*>(compileResult));
}

int ScriptRuntimeContext::ExecuteCode(HANDLE code, HANDLE compileResult)
{
	return 0;
}

void ScriptRuntimeContext::DestroyScriptRuntimeContext(ScriptRuntimeContext *context)
{
	delete context;
}

ScriptRuntimeContext* ScriptRuntimeContext::CreateScriptRuntimeContext(uint32_t stackSize, uint32_t stackFrameSize)
{
	return new ScriptRuntimeContext(stackSize, stackFrameSize);
}