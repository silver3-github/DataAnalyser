// SetMsgDefDlg.cpp: 实现文件
//

#include "pch.h"
#include "DataAnalyser.h"
#include "SetMsgDefDlg.h"
#include "afxdialogex.h"
#include "CTool.h"


// CSetMsgDefDlg 对话框

IMPLEMENT_DYNAMIC(CSetMsgDefDlg, CDialogEx)

CSetMsgDefDlg::CSetMsgDefDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SET_MSG_DEFINE, pParent)
	, msgName(_T(""))
	, handleMethod(_T("数据包"))
	, isInput(FALSE)
{

}

CSetMsgDefDlg::~CSetMsgDefDlg()
{
}

void CSetMsgDefDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, msgName);
	DDX_CBString(pDX, IDC_COMBO1, handleMethod);
	DDX_Check(pDX, IDC_CHECK1, isInput);
}


BEGIN_MESSAGE_MAP(CSetMsgDefDlg, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CSetMsgDefDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CSetMsgDefDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CSetMsgDefDlg 消息处理程序


BOOL CSetMsgDefDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	SetWindowTextA(title);
	return TRUE;
}

void CSetMsgDefDlg::OnBnClickedOk()
{
	//检测数据
	UpdateData(true);
	if (!CTool::IsValidCppName(msgName))
	{
		MessageBoxA("消息名必须满足 C++命名规范",title);
		return;
	}
	CDialogEx::OnOK();
}

void CSetMsgDefDlg::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}


