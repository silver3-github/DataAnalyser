// SearchDlg.cpp: 实现文件
//

#include "pch.h"
#include "DataAnalyser.h"
#include "SearchDlg.h"
#include "afxdialogex.h"


// CSearchDlg 对话框

IMPLEMENT_DYNAMIC(CSearchDlg, CDialogEx)

CSearchDlg::CSearchDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SEARCH, pParent)
	, dataType(_T("命令"))
	, value(_T(""))
	, isHex(FALSE)
	, isSearchNext(FALSE)
{

}

CSearchDlg::~CSearchDlg()
{
}

void CSearchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_CBString(pDX, IDC_TYPE, dataType);
	DDX_Text(pDX, IDC_VALUE, value);
	DDX_Check(pDX, IDC_HEX, isHex);
}


BEGIN_MESSAGE_MAP(CSearchDlg, CDialogEx)
	ON_BN_CLICKED(IDC_SEARCH, &CSearchDlg::OnBnClickedSearch)
	ON_BN_CLICKED(IDC_SEARCH_NEXT, &CSearchDlg::OnBnClickedSearchNext)
END_MESSAGE_MAP()


// CSearchDlg 消息处理程序


void CSearchDlg::OnBnClickedSearch()
{
	isSearchNext = false;
	CDialogEx::OnOK();
}

void CSearchDlg::OnBnClickedSearchNext()
{
	isSearchNext = true;
	CDialogEx::OnOK();
}
