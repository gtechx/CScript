
// testCScriptInDialog.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CtestCScriptInDialogApp:
// �йش����ʵ�֣������ testCScriptInDialog.cpp
//

class CtestCScriptInDialogApp : public CWinApp
{
public:
	CtestCScriptInDialogApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CtestCScriptInDialogApp theApp;