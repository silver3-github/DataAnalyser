#pragma once
#include "SearchDlg.h"

// CEditDataDlg 对话框

class CEditDataDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CEditDataDlg)

public:
	CEditDataDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CEditDataDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EDIT_DATA };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持


//实现：
protected:
	static CEditDataDlg* thiz;
	CSearchDlg searchDlg;
	CString dataType;
	CEdit valueEdit;
	CEdit dataEdit;
	CToolTipCtrl tipBox;
	char* tipText;
	int startChar;
	int endChar;
	CString calcResult;
	BOOL isStrOnly;
	long long startShow;
	BOOL isUpdateValue;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	BOOL SetTipText(UINT id, NMHDR* pTTTStruct, LRESULT* pResult);
	BOOL PreTranslateMessage(MSG* pMsg);
	void ShowDataTip(HWND, UINT, CEditDataDlg*, DWORD);
	afx_msg void OnBnClickedSearch();
	afx_msg void OnCbnSelchangeDataType();
	afx_msg void OnEnChangeValue();
	afx_msg void OnBnClickedOk();
	DECLARE_MESSAGE_MAP()

	void InferredData();
	void UpdateTipText();
	void UpdateValueEdit();

public:
	CString title;
	CString data;
	BOOL isCancelEdit;
};
