/*
	����dataManager���������ṩ����ʱ��ͨ�������в����������ṩ�����ṩ�˺ܶ�
	��Ϣ�����������еĴ��������������ṩ����Ҫ�漰�ġ������ڱ�����У��ܶ�
	�������Ϊ��mfc�������ԣ���Ҫ����дһ�������в��������ģ�顣
*/

#pragma once
#include "config.h"
#include "simpleTool.h"

namespace notstd {

	class NOTSTD_API CommandlineParser {
	protected:
		virtual bool OnCommandlinePart(const std::string &param, bool isOption,
			const std::string &nextParam, bool &hasParam);

	public:
		virtual ~CommandlineParser();

		bool parseCommandLine(const std::string &commandLine);
		//bool parseCommandLine(int argc, char *argv[]);
	};

	///////////////////////////////////////////////////////////////////////////////

	struct NOTSTD_API ApplicationData {
		Handle<NormalHandleType> readOnlyHandle;
		int readOnlySize;

		Handle<NormalHandleType> readWriteHandle;
		int readWriteSize;

		Handle<NormalHandleType> stringHandle;
		int stringSize;

		std::string *namedPipeName;

		unsigned long parentProcessId;

		std::string *projRootPath;
		std::string *devTypeName;

		uint32_t theId;

		// for ps
		ULONG userData;

		ApplicationData();
		~ApplicationData();
	};

	class NOTSTD_API appCommandLine : public CommandlineParser {
		ApplicationData &mAppData;

	protected:
		virtual bool OnCommandlinePart(const std::string &param,
			bool isOption, const std::string &nextParam, bool &hasParam);

	public:
		appCommandLine(ApplicationData &appData);
	};

}
