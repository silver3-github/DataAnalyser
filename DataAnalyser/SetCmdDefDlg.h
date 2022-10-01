#pragma once


// CSetCmdDefDlg 对话框

class CSetCmdDefDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSetCmdDefDlg)

public:
	CSetCmdDefDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CSetCmdDefDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SET_CMD_DEFINE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	DECLARE_MESSAGE_MAP()

protected:
	CMFCColorButton setTextColor;
	CMFCColorButton setBgColor;

public:
	CString title;
	CString cmdName;
	DWORD cmdId;
	BOOL isShow;
	BOOL isEdit;
	BOOL isInput;
	BOOL isAloneInput;
	COLORREF textColor;
	COLORREF bgColor;
};
