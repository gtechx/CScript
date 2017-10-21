#pragma once
#include "config.h"
#include "simpleTool.h"

namespace notstd {

	class PipeManager;
	class NamedPipe;

#define PIPE_MAX_SIZE							(64 * 1024 * 2)
#define PIPE_MAX_READ_BUFFER			PIPE_MAX_SIZE
#define PIPE_MAX_WRITE_BUFFER			PIPE_MAX_SIZE
#define PIPE_OPERATION_READ				1
#define PIPE_OPERATION_WRITE			2
#define PIPE_OPERATION_ACCEPT			3
#define PIPE_OPERATION_TERMINATE	4

	struct NOTSTD_API PipeData : public OVERLAPPED
	{
		DWORD operation;
		NamedPipe *pipe;
		char buffer[PIPE_MAX_WRITE_BUFFER];

		PipeData();
	};

	class NOTSTD_API NamedPipe
	{
		friend class PipeManager;

	protected:
		Handle<FileHandleType> mNamePipe;
		//Handle<(LONG)INVALID_HANDLE_VALUE> mNamePipe;
		PipeManager *mPipeManager;
		PipeData mPipeDataRead;
		PipeData mPipeDataWrite;

	protected:
		virtual void OnAccept();
		virtual void OnRead(const char *buffer, std::size_t size);
		virtual void OnWrite(std::size_t size);
		virtual void OnClose();

	public:
		NamedPipe();
		virtual ~NamedPipe();

		bool CreateNamedPipeServer(PipeManager *pipeManager, const std::wstring &pipeName);
		bool CreatenamedPipeClient(PipeManager *pipeManager, const std::wstring &pipeName);
		bool Write(const char *buffer, std::size_t size);
		bool Read();
		bool SyncWrite(const char *buffer, std::size_t size);
		void Close();
	};

	class NOTSTD_API PipeManager
	{
		friend class NamedPipe;

	private:
		Handle<NormalHandleType> mIOCP;

		DWORD mThreadCount;
		HANDLE *mConcurrentThreads;

		static UINT WINAPI CompThread(LPVOID lParam);
		void CompletionProc();

	public:
		PipeManager();
		~PipeManager();

		// ȱʡֵ��Ϊ1����Ϊ���Ѿ��з���ʹ��1���߳��������ر��Ǳ���ע��
		// Ϊ�˷�ֹ�÷�����Ϊ����߳�ʹ�ñ���ע�ᴦ��������⣬�ʴ˴��޸Ļ���
		// ����ȱʡֵΪ1
		bool StartManager(DWORD threadCount = 1);
		void StopManager();
		bool IsManagerStopped();
		void Join();
	};

}
