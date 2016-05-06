#pragma once

// ��ɶ˿�ģ��ʵ�ֵ�socket��ʩ

#include "ioCompletionManager.h"
#include "sockbase.h"

class NOTSTD_API Socket : public IOOperationBase
{
	friend class IOCompletionManager;
	friend class IOManagerUseSystemThreadPool;
	friend class SockByAccept;
	
protected:
	IOCompletionManager *mIOManager;

protected:
	SocketHandle mSocket;

protected:
	virtual void OnCustumMsg(IOCompleteData *completeData);
	virtual void OnReceive(IOCompleteData *completeData);
	virtual void OnSend(IOCompleteData *completeData);
	virtual void OnConnect(bool connectOK);
	virtual void OnAccept(const NetAddress &client, const NetAddress &serv);
	virtual void OnClose();
	virtual bool IsError();

	virtual bool SendPartial(IOCompleteData *completeData);

public:
	Socket();
	virtual ~Socket();

	virtual bool Connect(IOCompleteData *completeData, const std::string &ipAddr, PORT_T port);
	virtual bool Connect(IOCompleteData *completeData, const NetAddress &netAddr);
	virtual bool Receive(IOCompleteData *completeData);
	virtual bool Send(IOCompleteData *completeData);
	bool SendSync(IOCompleteData *completeData);
	bool SendSync(const char *buf, std::size_t size);

	// ����������û�о������ԣ�����֤��ȷ��
	// RecvFrom�ķ���ֵaddrFromӦ�û���OnReceive����ʱ��ϵͳ���
	// ��֤�ò������������ں���Ч������Ҫ�ر�ע���
	virtual bool RecvFrom(IOCompleteData *completeData, NetAddress *addrFrom);
	virtual bool SendTo(IOCompleteData *completeData, const NetAddress *addrTo);

	virtual bool Create(IOCompletionManager *ioManager, SocketType nSocketType = Stream, 
		ProtocolType nProtocolType = Tcp, DWORD dwFlags = 0);
	virtual bool Close();

	// �ɽ��ڲ�״̬��Ϊĳ��ֵ��Ȼ����֪ͨ�õײ㴦��
	// ���ϵͳ�����OnCustumMsg����
	virtual bool PostCustumMsg(IOCompleteData *completeData);

	SocketHandle& GetHandle();
};

class NOTSTD_API SockByAccept : public Socket {
	friend class IOCompletionManager;
	friend class IOManagerUseSystemThreadPool;

private:
	SOCKET mServerSock;

	// ����Accept���صķ���˿ͻ��˵�ַ
	char mSockAddr[(sizeof(NetAddress) + 16) * 2];

	NetAddress mServer, mClient;

public:
	virtual bool Accept(IOCompleteData *completeData, Socket &sockServer);
};
