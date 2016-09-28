#include "stdafx.h"
#include "notstd/ioServer.h"
#include <functional>
#include <thread>

const static char *hostName = "www.cnbeta.com";
const static char *url = "/";

///ͨ������ʶ
#define	MDM_GP_LOGON					100								///������½

// -- add by liyuanxing(mz02005@qq.com)
#define ASS_GP_LOGIN2_BASE				16
// ͨ���ֻ��Ž��е�¼
#define ASS_GP_LOGIN2_BY_MOBILE_NUMBER	(ASS_GP_LOGIN2_BASE + 0)
// ָ�����ֻ���û��ע��
#define ASS_GP_LOGIN2_MOBILENO_NOT_REG	(ASS_GP_LOGIN2_BASE + 1)
// ͨ��΢���˺ŵ�¼
#define ASS_GP_LOGIN2_BY_WECHAT			(ASS_GP_LOGIN2_BASE + 2)
// ΢���˺�û�а�
#define ASS_GP_LOGIN2_WECHAT_NOT_BIND	(ASS_GP_LOGIN2_BASE + 3)
// ͨ��QQ�˺ŵ�¼
#define ASS_GP_LOGIN2_BY_QQ				(ASS_GP_LOGIN2_BASE + 4)
// QQ�˺�û�а�
#define ASS_GP_LOGIN2_QQ_NOT_BIND		(ASS_GP_LOGIN2_BASE + 5)
// ��ʱ��¼
#define ASS_GP_LOGIN2_TEMP		(ASS_GP_LOGIN2_BASE + 6)

#pragma pack(1)
///�������ݰ��ṹͷ
struct NetMessageHead
{
	UINT uMessageSize;						///���ݰ���С
	UINT bMainID;							///����������
	UINT bAssistantID;						///������������ ID
	UINT bHandleCode;						///���ݰ��������
	UINT bReserve;							///�����ֶ�
};

///�û���½���ʺţ��ṹ
struct MSG_GP_S_LogonByNameStruct
{
	UINT								uRoomVer;							///�����汾
	char								szName[64];							///��½����
	char								TML_SN[128];
	char								szMD5Pass[52];						///��½����
	char								szMathineCode[64];					///���������� ��������
	char                                szCPUID[24];						//CPU��ID
	char                                szHardID[24];						//Ӳ�̵�ID
	char								szIDcardNo[64];						//֤����
	char								szMobileVCode[8];					//�ֻ���֤��
	int									gsqPs;
	int									iUserID;							//�û�ID��¼�����ID>0��ID��¼
};

// ASS_GP_LOGIN2***��Ӧ�Ľṹ
struct Login2_LoginByMobileNumber : public MSG_GP_S_LogonByNameStruct
{
	char openid[50];

	// �ֻ���
	char mobileSN[20];

	// ��¼���루����md5����
	char password[52];
};


#pragma pack()

class AAA
{
public:
	void OnSend(notstd::IOServer *ioServer, const notstd::IOErrorCode &e, notstd::IOServerData *data, size_t trans)
	{
		if (e)
			return;
		notstd::SendIOServerData *sd = static_cast<notstd::SendIOServerData*>(data);
		mIOSocket.AsyncRecv(&mRecvData, IOSOCKET_MEMBER_BIND(&AAA::OnReceive1, this));
	}

	void OnConnect(notstd::IOServer *ioServer, const notstd::IOErrorCode &e, notstd::IOServerData *data, size_t trans)
	{
		if (e)
		{
			printf("Connect fail %s\n", e.what().c_str());
			mIOSocket.Close();
			return;
		}

		printf("Connect succeeded\n");

		// ��ʱ�Ȳ�����������ݶ�ȡ��
		mIOSocket.Close();
		return;

		std::string s;
		s.append("GET ").append(url).append(" HTTP/1.1\r\n");
		s += "Connection: close\r\n"
			"Host: ";
		s += hostName;
		s += "\r\n\r\n";
		mSendData.GetDataBuffer()->SetBufferSize(s.c_str(), s.size());
		mIOSocket.AsyncSend(&mSendData, IOSOCKET_MEMBER_BIND(&AAA::OnSend, this));
	}

	void OnReceive(notstd::IOServer *ioServer, const notstd::IOErrorCode &e, notstd::IOServerData *data, size_t trans)
	{
		if (e)
		{
			if (trans == 0)
			{
				printf("\nDisconnect\n");
				return;
			}
			printf("Connect fail %s\n", e.what().c_str());
			mIOSocket.Close();
			return;
		}

		std::string s;
		notstd::ReceiveIOServerData *rd = static_cast<notstd::ReceiveIOServerData*>(data);
		s.append(rd->GetDataBuffer()->mBegin, trans);
		std::cout << s;
		mIOSocket.AsyncRecv(&mRecvData);
	}

	void OnReceive1(notstd::IOServer *ioServer, const notstd::IOErrorCode &e, notstd::IOServerData *data, size_t trans)
	{
		if (e)
			return;
		std::string s;
		notstd::ReceiveIOServerData *rd = static_cast<notstd::ReceiveIOServerData*>(data);
		s.append(rd->GetDataBuffer()->mBegin, trans);
		std::cout << s;
		mAcceptSocket.AsyncRecv(&mRecvData1);
	}
	
	void OnAccept(notstd::IOServer *ioServer, const notstd::IOErrorCode &e, notstd::IOServerData *data, size_t trans)
	{
		if (e)
			return;

		mAcceptSocket.AsyncRecv(&mRecvData1, IOSOCKET_MEMBER_BIND(&AAA::OnReceive1, this));
	}

	void OnTimer(notstd::IOServer *ioServer, const notstd::IOErrorCode &e, notstd::IOServerData *data, size_t trans)
	{
		static int xx = 0;
		if (++xx == 10)
		{
			//ioServer->PostQuit();
		}
		std::cout << "HAHA " << xx << std::endl;
	}

private:
	notstd::IOSocket &mIOSocket;
	notstd::ReceiveIOServerData mRecvData;
	notstd::SendIOServerData mSendData;

	notstd::ReceiveIOServerData mRecvData1;
	notstd::AcceptSocket &mAcceptSocket;
	notstd::IOSocket &mListenSocket;

public:
	AAA(notstd::IOSocket &ioSocket, notstd::IOSocket &listenSocket, notstd::AcceptSocket &acceptSock)
		: mIOSocket(ioSocket)
		, mListenSocket(listenSocket)
		, mAcceptSocket(acceptSock)
	{
	}
};

class TestConnection
{
public:
	notstd::ConnectIOServerData mCd;
	notstd::SendIOServerData mSd;
	notstd::IOServer mIOServer;
	notstd::IOSocket mConnSocket;

public:
	TestConnection(notstd::IOServer &ioServer)
		: mIOServer(ioServer)
		, mConnSocket(ioServer)
	{
		mConnSocket.Create();
	}

	notstd::IOSocket& GetConnSocket() { return mConnSocket; }

	void OnConnect(notstd::IOServer *ioServer, const notstd::IOErrorCode &e, notstd::IOServerData *data, size_t trans)
	{
		if (e) {
			std::cerr << "Connect fail\n";
			return;
		}

		std::string sd;
		NetMessageHead h;
		h.uMessageSize = sizeof(NetMessageHead) + sizeof(Login2_LoginByMobileNumber);
		h.bMainID = MDM_GP_LOGON;
		h.bAssistantID = ASS_GP_LOGIN2_BY_MOBILE_NUMBER;
		h.bHandleCode = 0;
		h.bReserve = 0;
		sd.append(reinterpret_cast<const char*>(&h), sizeof(h));
		NetMessageHead *p = reinterpret_cast<NetMessageHead*>(&sd[0]);
		Login2_LoginByMobileNumber zero;
		memset(&zero, 0, sizeof(zero));
		strcpy(zero.mobileSN, "2523534");
		sd.append(reinterpret_cast<const char*>(&zero), sizeof(zero));
		mSd.GetDataBuffer()->SetBufferSize(sd.c_str(), sd.size());
		mConnSocket.AsyncSend(&mSd, IOSOCKET_MEMBER_BIND(&TestConnection::OnSend, this));
	}

	void OnSend(notstd::IOServer *ioServer, const notstd::IOErrorCode &e, notstd::IOServerData *data, size_t trans)
	{
		if (e) {
			std::cerr << "Send fail\n";
			return;
		}

		std::cout << "send over\n";
	}

	void DoConnect()
	{
		mConnSocket.AsyncConnect(notstd::NetAddress("127.0.0.1", 3015), &mCd,
			IOSOCKET_MEMBER_BIND(&TestConnection::OnConnect, this));
	}
};

int _tmain(int argc, _TCHAR* argv[])
{
	notstd::NetEnviroment::Init();
	notstd::IOServer ioServer;
	ioServer.CreateIOServer();
	notstd::IOSocket ioSocket(ioServer);

	notstd::IOSocket sockListen(ioServer);
	sockListen.Create();
	if (!sockListen.GetHandle().Bind(notstd::NetAddress("0.0.0.0", 8080)))
	{
		printf("Bind local fail\n");
		return 1;
	}
	if (!sockListen.GetHandle().Listen(SOMAXCONN))
	{
		printf("Listen fail\n");
		return 2;
	}

	notstd::AcceptSocket as(ioServer);
	notstd::AcceptIOServerData aiod;

	AAA aaa(ioSocket, sockListen, as);
	notstd::IOTimer theTimer(ioServer);

	theTimer.CreateTimer(1000, IOSOCKET_MEMBER_BIND(&AAA::OnTimer, &aaa));

	as.Create();
	as.AsyncAccept(&aiod, &sockListen,
		IOSOCKET_MEMBER_BIND(&AAA::OnAccept, &aaa));

	TestConnection tc(ioServer);
	tc.DoConnect();

	std::thread x(std::bind(&notstd::IOServer::Run, &ioServer));

	x.join();

	return 0;
}
