#pragma once


// CDataAnalyDlg 对话框

class CDataAnalyDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDataAnalyDlg)

public:
	CDataAnalyDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDataAnalyDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DATA_ANALY };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持


//实现
protected:
	static CDataAnalyDlg* thiz;
	CToolTipCtrl tipBox;
	char* tipText;
	int startChar;
	int endChar;
	CString calcResult;
		
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	BOOL SetTipText(UINT id, NMHDR* pTTTStruct, LRESULT* pResult);
	BOOL PreTranslateMessage(MSG* pMsg);
	void ShowDataTip(HWND, UINT, CDataAnalyDlg*, DWORD);
	DECLARE_MESSAGE_MAP()

public:
	void AdjustPosition();
	CEdit dataEdit;
};
