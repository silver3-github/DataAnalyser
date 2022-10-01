#pragma once


// CSearchDlg 对话框

class CSearchDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSearchDlg)

public:
	CSearchDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CSearchDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SEARCH };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持


	//实现:
protected:

	//生成的消息映射函数
	afx_msg void OnBnClickedSearch();
	afx_msg void OnBnClickedSearchNext();
	DECLARE_MESSAGE_MAP()

public:
	CString dataType;
	CString value;
	BOOL isHex;
	BOOL isSearchNext;
};
