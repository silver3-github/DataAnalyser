#pragma once
#include "ConfigDlg.h"
#include "DataAnalyDlg.h"
#include "SearchDlg.h"
#include "EditDataDlg.h"


// CDataAnalyserDlg 对话框
class CDataAnalyserDlg : public CDialogEx
{
	// 构造
public:
	CDataAnalyserDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DATAANALYSER_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	CListCtrl msgList;
	CConfigDlg configDlg;
	CDataAnalyDlg dataAnalyDlg;
	CSearchDlg searchDlg;
	CEditDataDlg editDataDlg;
	BOOL isPause;
	int msgCurSel;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();//防止 entry关闭对话框 
	virtual void OnCancel();//防止 esc关闭对话框 
	afx_msg void OnClose();//响应对话框关闭
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnHdnEndtrackMsglist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLvnInsertitemMsglist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLvnItemchangedMsglist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMCustomdrawMsglist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMClickMsglist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMDblclkMsglist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMRDblclkMsglist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMRClickMsglist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedClear();
	afx_msg void OnBnClickedDataSearch();
	afx_msg void OnBnClickedOpenLog();
	afx_msg void OnBnClickedNewConfig();
	afx_msg void OnBnClickedLoadConfig();
	afx_msg void OnBnClickedOpenConfig();
	afx_msg LRESULT OnRecvMsgHandler(WPARAM buf, LPARAM len);
	DECLARE_MESSAGE_MAP()

	// 成员函数
	void OnRecvMsg(char* buf, unsigned len, CDataAnalyserDlg* thiz);	
	void AdjustListClumn();
};
