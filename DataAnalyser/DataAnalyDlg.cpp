// DataAnalyDlg.cpp: 实现文件
//

#include "pch.h"
#include "DataAnalyser.h"
#include "afxdialogex.h"
#include "DataAnalyDlg.h"


//静态的内部 thiz指针
CDataAnalyDlg* CDataAnalyDlg::thiz=nullptr;


// CDataAnalyDlg 对话框

IMPLEMENT_DYNAMIC(CDataAnalyDlg, CDialogEx)

CDataAnalyDlg::CDataAnalyDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DATA_ANALY, pParent)
	, startChar(0)
	, endChar(0)
	, tipText(nullptr)
{
	thiz = this;
}

CDataAnalyDlg::~CDataAnalyDlg()
{
	if (tipText)delete[] tipText;
}

void CDataAnalyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, dataEdit);
}


BEGIN_MESSAGE_MAP(CDataAnalyDlg, CDialogEx)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, SetTipText)
END_MESSAGE_MAP()


// CDataAnalyDlg 消息处理程序

BOOL CDataAnalyDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//创建提示框
	tipBox.Create(this, TTS_NOPREFIX);
	tipBox.AddTool(&dataEdit);
	tipBox.SetMaxTipWidth(GetSystemMetrics(SM_CXFULLSCREEN));
	tipBox.SetTitle(NULL, "可能的内容");
	tipBox.SetDelayTime(TTDT_AUTOPOP, 30000);//显示时长（最多30秒）
	tipBox.SetDelayTime(TTDT_INITIAL, 0);//停靠多久显示
	tipBox.SetDelayTime(TTDT_RESHOW, 0);//重新显示的间隔

	//添加定时器
	union {
		void (CDataAnalyDlg::* showDataTip)(HWND, UINT, CDataAnalyDlg*, DWORD);
		TIMERPROC timerProc;
	}func;
	func.showDataTip = &CDataAnalyDlg::ShowDataTip;
	SetTimer(0, 100, func.timerProc);
	return TRUE;
}

BOOL CDataAnalyDlg::SetTipText(UINT id, NMHDR* pTTTStruct, LRESULT* pResult)
{
	LPTOOLTIPTEXT pTTT = reinterpret_cast<LPTOOLTIPTEXT>(pTTTStruct);
	*pResult = 0;

	UINT_PTR nID = pTTTStruct->idFrom; //得到相应窗口ID，有可能是HWND
	if ((pTTT->uFlags & TTF_IDISHWND) && ((HWND)nID == dataEdit.m_hWnd))
	{
		if (tipText == nullptr)pTTT->lpszText = "";
		else pTTT->lpszText = tipText;
	}
	return TRUE;
}

BOOL CDataAnalyDlg::PreTranslateMessage(MSG* pMsg)
{
	tipBox.RelayEvent(pMsg);
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CDataAnalyDlg::ShowDataTip(HWND, UINT, CDataAnalyDlg*, DWORD)
{
	//窗口未显示或未选择数据 退出
	if (thiz->IsWindowVisible() == false)return;
	int startChar, endChar;
	thiz->dataEdit.GetSel(startChar, endChar);
	if ((startChar - endChar) == 0)return;

	//数据推算
	if (startChar != thiz->startChar || endChar != thiz->endChar)
	{
		//获取数据
		thiz->startChar = startChar;
		thiz->endChar = endChar;
		CString data;
		thiz->dataEdit.GetWindowTextA(data);
		data = data.Mid(startChar, endChar - startChar);
		data.Remove(' ');
		data.Remove('\r');
		data.Remove('\n');
		int len = data.GetLength() / 2;
		unsigned char* buf = new unsigned char[len + 4]{};
		for (int i = 0; i < len; i++)
		{
			buf[i] = strtoul(data.Mid(i * 2, 2), NULL, 16);
		}

		//推算数据
		thiz->calcResult = "";
		CString str;
		if (len > 0)
		{
			str.Format("ASCII: %s\r\n", (char*)buf);
			thiz->calcResult += str;
		}

		CStringW wstr;
		if (len > 1)
		{
			wstr.Format(L"UTF-16: %s\r\n", (wchar_t*)buf);
			thiz->calcResult += wstr;
		}

		if (len > 0)
		{
			thiz->calcResult += "char: ";
			for (int i = 0; i < len; i++)
			{
				str.Format("[%hhd] ", (char)buf[i]);
				thiz->calcResult += str;
			}
			thiz->calcResult += "\r\n";
		}

		if (len > 0)
		{
			thiz->calcResult += "uchar: ";
			for (int i = 0; i < len; i++)
			{
				str.Format("[%hhu] ", buf[i]);
				thiz->calcResult += str;
			}
			thiz->calcResult += "\r\n";
		}


		if (len > 1)
		{
			thiz->calcResult += "short: ";
			for (int i = 0; i < len / 2; i++)
			{
				short* value = (short*)buf;
				str.Format("[%hd] ", value[i]);
				thiz->calcResult += str;
			}
			thiz->calcResult += "\r\n";

			thiz->calcResult += "ushort: ";
			for (int i = 0; i < len / 2; i++)
			{
				unsigned short* value = (unsigned short*)buf;
				str.Format("[%hu] ", value[i]);
				thiz->calcResult += str;
			}
			thiz->calcResult += "\r\n";
		}

		if (len > 3)
		{
			thiz->calcResult += "int: ";
			for (int i = 0; i < len / 4; i++)
			{
				int* value = (int*)buf;
				str.Format("[%d] ", value[i]);
				thiz->calcResult += str;
			}
			thiz->calcResult += "\r\n";

			thiz->calcResult += "uint: ";
			for (int i = 0; i < len / 4; i++)
			{
				unsigned int* value = (unsigned int*)buf;
				str.Format("[%u] ", value[i]);
				thiz->calcResult += str;
			}
			thiz->calcResult += "\r\n";
		}

		if (len > 7)
		{
			thiz->calcResult += "llong: ";
			for (int i = 0; i < len / 8; i++)
			{
				long long* value = (long long*)buf;
				str.Format("[%lld] ", value[i]);
				thiz->calcResult += str;
			}
			thiz->calcResult += "\r\n";

			thiz->calcResult += "ullong: ";
			for (int i = 0; i < len / 8; i++)
			{
				unsigned long long* value = (unsigned long long*)buf;
				str.Format("[%llu] ", value[i]);
				thiz->calcResult += str;
			}
			thiz->calcResult += "\r\n";
		}

		if (len > 3)
		{
			thiz->calcResult += "float: ";
			for (int i = 0; i < len / 4; i++)
			{
				float* value = (float*)buf;
				str.Format("[%f] ", value[i]);
				thiz->calcResult += str;
			}
			thiz->calcResult += "\r\n";
		}

		if (len > 7)
		{
			thiz->calcResult += "double: ";
			for (int i = 0; i < len / 8; i++)
			{
				double* value = (double*)buf;
				str.Format("[%lf] ", value[i]);
				thiz->calcResult += str;
			}
			thiz->calcResult += "\r\n";
		}

		//清理数据
		delete[] buf;

		//更新tip文本
		len = thiz->calcResult.GetLength();
		char* tmp = new char[len + 1]{};
		memcpy(tmp, thiz->calcResult, len + 1);
		if (thiz->tipText)delete[] thiz->tipText;
		thiz->tipText = tmp;
	}

	//显示推算结果
	CPoint startPoint, endPoint, cursor;
	GetCursorPos(&cursor);
	int line = thiz->dataEdit.LineFromChar(startChar);
	for (int i = startChar; i < endChar; i++)
	{
		if (line != thiz->dataEdit.LineFromChar(i))
		{
			startPoint = thiz->dataEdit.PosFromChar(startChar);
			endPoint = thiz->dataEdit.PosFromChar(i - 1);
			thiz->dataEdit.ClientToScreen(&startPoint);
			thiz->dataEdit.ClientToScreen(&endPoint);
			endPoint.x += 12;
			endPoint.y += 30;
			if (cursor.x > startPoint.x && cursor.x<endPoint.x
				&& cursor.y>startPoint.y && cursor.y < endPoint.y)
			{
				thiz->tipBox.Activate(true);
				return;
			}

			startChar = i;
			line = thiz->dataEdit.LineFromChar(startChar);
		}
	}

	startPoint = thiz->dataEdit.PosFromChar(startChar);
	endPoint = thiz->dataEdit.PosFromChar(endChar - 1);
	thiz->dataEdit.ClientToScreen(&startPoint);
	thiz->dataEdit.ClientToScreen(&endPoint);
	endPoint.x += 12;
	endPoint.y += 30;
	if (cursor.x > startPoint.x && cursor.x<endPoint.x
		&& cursor.y>startPoint.y && cursor.y < endPoint.y)
	{
		thiz->tipBox.Activate(true);
		return;
	}
	thiz->tipBox.Activate(false);
}


//成员函数实现

void CDataAnalyDlg::AdjustPosition()
{
	CWnd* parent = GetParent();
	if (parent)
	{
		//获取参数
		CRect rect, parentRect;
		GetWindowRect(rect);
		parent->GetWindowRect(parentRect);
		int withScreen = GetSystemMetrics(SM_CXFULLSCREEN);

		//调整位置大小
		int with = rect.Width();
		rect.top = parentRect.top;
		rect.bottom = parentRect.bottom;
		int subWith = (parentRect.right - 13) + with - (withScreen + 8);
		rect.left = (parentRect.right - 13) - (subWith > 0 ? subWith : 0);
		rect.right = rect.left + with;
		MoveWindow(rect);
	}
}