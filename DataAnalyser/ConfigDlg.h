#pragma once
#include "SetMsgDefDlg.h"
#include "SetCmdDefDlg.h"

// CConfigDlg 对话框

class CConfigDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CConfigDlg)

public:
	CConfigDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CConfigDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CONFIG_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持


// 实现
protected:
	CListCtrl lstMsgDefine;
	CListCtrl lstCmdDefine;
	CSetMsgDefDlg setMsgDefDlg;
	CSetCmdDefDlg setCmdDefDlg;
	int msgCurSel;
	int cmdCurSel;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedBuildDefine();
	afx_msg void OnNMRClickMsgDefine(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMRClickCmdDefine(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAddMsgDef();
	afx_msg void OnAddCmdDef();
	afx_msg void OnDelCmdDef();
	afx_msg void OnDelMsgDef();
	afx_msg void OnNMDblclkMsgDefine(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMDblclkCmdDefine(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLvnItemchangedMsgDefine(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()

public:
	CString configFileName;
};
