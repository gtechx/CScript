// mz02005@qq.com
// http ssl֧��

#pragma once
#include "httpServerNew.h"
#include <openssl/ssl.h>
#include <openssl/err.h>

class SSLContext
{
private:
	SSL_CTX *mSSLCtx;

public:
	SSLContext();
	~SSLContext();

	operator SSL_CTX*() { return mSSLCtx; }
	bool UsePrivateKeyFile(const char *filePathName, char *password = nullptr);
	bool UseCertificateFile(const char *filePathName);

	bool SetVerify(bool verify = true);
};

///////////////////////////////////////////////////////////////////////////

class SSLConnection : public Connection
{
private:
	bool mHasHandshake;

	SSL_CTX *mSSLCtx;
	SSL *mSSL;
	
protected:
	void SetBaseInfo(payHttpServer *server, SOCKET sock, SSL_CTX *sslCtx);

public:
	SSLConnection();
	virtual ~SSLConnection();
	virtual int CreateSocketBuffer(event_base *eb) override;

	// �����µ����ӣ�����ʼ��������
	static Connection* NewConnection(payHttpServer *server, SOCKET sock);
};
