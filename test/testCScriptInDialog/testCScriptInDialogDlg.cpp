#include "stdafx.h"
#include "testCScriptInDialog.h"
#include "testCScriptInDialogDlg.h"
#include "afxdialogex.h"
#include "CScriptEng/cscriptBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

///////////////////////////////////////////////////////////////////////////////

class println2 : public runtime::runtimeObjectBase
{
public:
	CtestCScriptInDialogDlg *mDlg;
	
public:
	println2()
		: mDlg(nullptr)
	{
	}

	virtual uint32_t GetObjectTypeId() const
	{
		return runtime::DT_UserTypeBegin;
	}

	virtual runtimeObjectBase* Add(const runtimeObjectBase *obj){ return nullptr; }
	virtual runtimeObjectBase* Sub(const runtimeObjectBase *obj){ return nullptr; }
	virtual runtimeObjectBase* Mul(const runtimeObjectBase *obj){ return nullptr; }
	virtual runtimeObjectBase* Div(const runtimeObjectBase *obj){ return nullptr; }

	// =��Ԫ����
	virtual runtimeObjectBase* SetValue(const runtimeObjectBase *obj) { return nullptr; }

	// ����.��������һԪ�ģ�
	virtual runtimeObjectBase* GetMember(const char *memName) { return nullptr; }

	// docall����������һԪ���㣩
	virtual runtimeObjectBase* doCall(runtime::doCallContext *context)
	{
		if (context->GetParamCount() < 1)
			return this;

		const char *s = context->GetStringParam(0);
		if (!s)
			return this;

		if (mDlg && ::IsWindow(mDlg->GetSafeHwnd()))
		{
			mDlg->mPrintHostBox.AddString(CA2W(s).m_psz);
			mDlg->mPrintHostBox.SetCurSel(
				mDlg->mPrintHostBox.GetCount() - 1);
		}
		return this;
	}

	// getindex����������һԪ���㣩
	virtual runtimeObjectBase* getIndex(int i) { return nullptr; }
	// ����ת��Ϊ�ַ���
	virtual runtime::stringObject* toString() { return nullptr; }

	// �Ƚ�
	virtual bool isGreaterThan(const runtimeObjectBase *obj) { return false; }

	virtual bool isEqual(const runtimeObjectBase *obj) { return false; }
};

///////////////////////////////////////////////////////////////////////////////

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////

CtestCScriptInDialogDlg::CtestCScriptInDialogDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CtestCScriptInDialogDlg::IDD, pParent)
	, mLoadCodeExists(FALSE)
	, mSourcePath(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CtestCScriptInDialogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PRINTHOST, mPrintHostBox);
	DDX_Check(pDX, IDC_LOADCODE_EXISTS, mLoadCodeExists);
	DDX_Text(pDX, IDC_SOURCE_PATH, mSourcePath);
}

void CtestCScriptInDialogDlg::ExecuteThreadProc(void *param)
{
	SimpleThread *thread = reinterpret_cast<SimpleThread*>(param);
	reinterpret_cast<CtestCScriptInDialogDlg*>(thread->GetUserData())->ExecuteThreadInner(thread);
}

void CtestCScriptInDialogDlg::ExecuteThreadInner(void *param)
{
	SimpleThread *thread = reinterpret_cast<SimpleThread*>(param);

	//scriptAPI::SimpleCScriptEng::Init();
	//scriptAPI::FileStream fs("c:\\work\\test.c");
	//scriptAPI::ScriptCompiler compiler;

	//HANDLE h = compiler.Compile(&fs, true);
	//if (h)
	//{
	//	scriptAPI::ScriptRuntimeContext *runtimeContext
	//		= scriptAPI::ScriptRuntimeContext::CreateScriptRuntimeContext(512, 512);
	//	runtimeContext->Execute(h);
	//	scriptAPI::ScriptCompiler::ReleaseCompileResult(h);
	//	scriptAPI::ScriptRuntimeContext::DestroyScriptRuntimeContext(runtimeContext);
	//}
	//scriptAPI::SimpleCScriptEng::Term();

	CompilerHandle cHandle = nullptr;
	CompileResultHandle crHandle = nullptr;
	VirtualMachineHandle virtualMachine = nullptr;

	FILE *file = nullptr;
	println2 *println = nullptr;

	do {

		if (InitializeCScriptEngine() < 0)
			break;
		
		if (CreateCScriptCompile(&cHandle) < 0)
			break;

		PushCompileTimeName(cHandle, "println2");

		if ((crHandle = CompileCode(CW2A(mSourcePath).m_psz, cHandle, 1)) == nullptr)
			break;

		if (mLoadCodeExists)
		{
			if (fopen_s(&file, "c:\\work\\codeoutput.txt", "rb"))
				break;
			LoadCodeFromFile(crHandle, file);
			LoadConstStringTableFromFile(crHandle, file);
		}
		else
		{
			if (fopen_s(&file, "c:\\work\\codeoutput.txt", "wb"))
				break;
			SaveCodeToFile(crHandle, file);
			SaveConstStringTableToFile(crHandle, file);
		}
		if (file)
		{
			fclose(file);
			file = nullptr;
		}

		if (CreateVirtualMachine(&virtualMachine, 512, 512) < 0)
			break;
		println = new runtime::ObjectModule<println2>;
		println->mDlg = this;
		PushRuntimeObject(virtualMachine, println);

		if (VirtualMachineExecute(virtualMachine, crHandle) < 0)
			break;

		MessageBox(L"ִ�����");

	} while (0);
	
	if (file)
		fclose(file);

	if (virtualMachine)
		DestroyVirtualMachine(virtualMachine);
	if (crHandle)
		ReleaseCompileResult(crHandle);
	if (cHandle)
		DestroyCScriptCompile(cHandle);
	UninitializeCScriptEngine();
}

BEGIN_MESSAGE_MAP(CtestCScriptInDialogDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CtestCScriptInDialogDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_SELECT_SOURCE, &CtestCScriptInDialogDlg::OnBnClickedSelectSource)
END_MESSAGE_MAP()


// CtestCScriptInDialogDlg ��Ϣ�������

BOOL CtestCScriptInDialogDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CtestCScriptInDialogDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CtestCScriptInDialogDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CtestCScriptInDialogDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CtestCScriptInDialogDlg::OnBnClickedOk()
{
	mPrintHostBox.ResetContent();
	mThread.stopThread();
	mThread.startThread(&ExecuteThreadProc, this);
}

void CtestCScriptInDialogDlg::OnBnClickedSelectSource()
{
	CFileDialog fd(TRUE, L".c", L"test", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
		L"(*.c c source file)|*.c||");
	if (fd.DoModal() == IDOK)
	{
		mSourcePath = fd.GetPathName();
		UpdateData(FALSE);
	}
}
