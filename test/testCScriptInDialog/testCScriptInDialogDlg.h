#pragma once
#include "CScriptEng/CScriptEng.h"
#include "notstd/notstd.h"
#include "afxwin.h"

class CtestCScriptInDialogDlg : public CDialogEx
{
	DECLARE_MESSAGE_MAP()

public:
	CtestCScriptInDialogDlg(CWnd* pParent = NULL);

	enum { IDD = IDD_TESTCSCRIPTINDIALOG_DIALOG };

public:
	CListBox mPrintHostBox;
	BOOL mLoadCodeExists;
	CString mSourcePath;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

protected:
	SimpleThread mThread;
	HICON m_hIcon;

	static void ExecuteThreadProc(void *param);
	void ExecuteThreadInner(void *param);

	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedSelectSource();
};
