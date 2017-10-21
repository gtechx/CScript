#pragma once
#include "config.h"

#if !defined(PLATFORM_WINDOWS)
#include <sys/time.h>
#include <pthread.h>
#include "eventbase.h"

namespace notstd {
	class NOTSTD_API Event : public WaitableObject
	{
	private:
		bool mIsCondCreate;
		bool mIsMutexCreate;
		bool mIsManualReset;
		bool mIsSignalStatus;

		// ��׼�����mutex���ǲ����
		pthread_mutex_t mMutex;

		// ��׼�����variable_condition��̫���룬������linuxϵͳ�Դ���
		pthread_cond_t mCond;

		inline bool isInitialized() const { return mIsCondCreate && mIsMutexCreate; }

	public:
		Event();
		virtual ~Event();

		bool CreateEvent(bool manualReset, bool initState = false);
		void CloseEvent();

		bool SetEvent();
		bool ResetEvent();

		virtual bool WaitObject(uint32_t timeout = 0) override;
	};
}

#endif
