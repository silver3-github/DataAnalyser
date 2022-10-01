// SetCmdDefDlg.cpp: 实现文件
//

#include "pch.h"
#include "DataAnalyser.h"
#include "SetCmdDefDlg.h"
#include "afxdialogex.h"
#include "CTool.h"


// CSetCmdDefDlg 对话框

IMPLEMENT_DYNAMIC(CSetCmdDefDlg, CDialogEx)

CSetCmdDefDlg::CSetCmdDefDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SET_CMD_DEFINE, pParent)
	, cmdName(_T(""))
	, cmdId(0)
	, isShow(FALSE)
	, isInput(FALSE)
	, isAloneInput(FALSE)
	, isEdit(FALSE)
	, textColor(0)
	, bgColor(0xFFFFFF)
{
	
}

CSetCmdDefDlg::~CSetCmdDefDlg()
{
}

void CSetCmdDefDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, cmdName);
	DDX_Text(pDX, IDC_EDIT2, cmdId);
	DDX_Check(pDX, IDC_CHECK1, isShow);
	DDX_Check(pDX, IDC_CHECK2, isInput);
	DDX_Check(pDX, IDC_CHECK3, isAloneInput);
	DDX_Check(pDX, IDC_CHECK4, isEdit);
	DDX_Control(pDX, IDC_TEXTCOLOR, setTextColor);
	DDX_Control(pDX, IDC_BGCOLOR, setBgColor);
}


BEGIN_MESSAGE_MAP(CSetCmdDefDlg, CDialogEx)

	ON_BN_CLICKED(IDOK, &CSetCmdDefDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CSetCmdDefDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CSetCmdDefDlg 消息处理程序


BOOL CSetCmdDefDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	SetWindowTextA(title);
	setTextColor.SetColor(textColor);
	setBgColor.SetColor(bgColor);
	return TRUE;
}


void CSetCmdDefDlg::OnBnClickedOk()
{
	//检测数据
	UpdateData(true);
	if (!CTool::IsValidCppName(cmdName))
	{
		MessageBoxA("命令名必须满足 C++命名规范",title);
		return;
	}
	textColor = setTextColor.GetColor();
	bgColor = setBgColor.GetColor();
	CDialogEx::OnOK();
}


void CSetCmdDefDlg::OnBnClickedCancel()
{
	CDialogEx::OnCancel();
}

