#pragma once


// CSetMsgDefDlg 对话框

class CSetMsgDefDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSetMsgDefDlg)

public:
	CSetMsgDefDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CSetMsgDefDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SET_MSG_DEFINE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	DECLARE_MESSAGE_MAP()

public:
	CString title;
	CString msgName;
	CString handleMethod;
	BOOL isInput;
};
