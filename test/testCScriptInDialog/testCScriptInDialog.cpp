#include "stdafx.h"
#include "testCScriptInDialog.h"
#include "testCScriptInDialogDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CtestCScriptInDialogApp

BEGIN_MESSAGE_MAP(CtestCScriptInDialogApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

// CtestCScriptInDialogApp ����

CtestCScriptInDialogApp::CtestCScriptInDialogApp()
{
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
}

// Ψһ��һ�� CtestCScriptInDialogApp ����

CtestCScriptInDialogApp theApp;

// CtestCScriptInDialogApp ��ʼ��

BOOL CtestCScriptInDialogApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	CShellManager *pShellManager = new CShellManager;

	SetRegistryKey(_T("testCScriptInDialog"));

	CtestCScriptInDialogDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}

	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	return FALSE;
}
