// DataAnalyDlg.cpp: 实现文件
//

#include "pch.h"
#include "DataAnalyser.h"
#include "afxdialogex.h"
#include "DataAnalyDlg.h"


//静态的内部 thiz指针
CDataAnalyDlg* CDataAnalyDlg::thiz = nullptr;


// CDataAnalyDlg 对话框

IMPLEMENT_DYNAMIC(CDataAnalyDlg, CDialogEx)

CDataAnalyDlg::CDataAnalyDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DATA_ANALY, pParent)
	, startChar(0)
	, endChar(0)
	, tipText(nullptr)
	, isStrOnly(false)
	, startShow(GetTickCount64())
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
	DDX_Control(pDX, IDC_DATA, dataEdit);
}


BEGIN_MESSAGE_MAP(CDataAnalyDlg, CDialogEx)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, SetTipText)
END_MESSAGE_MAP()


// CDataAnalyDlg 消息处理程序

BOOL CDataAnalyDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//初始化控件
	searchDlg.dataType = "ASCII";

	//创建提示框
	tipBox.Create(this, TTS_NOPREFIX);
	tipBox.AddTool(&dataEdit);
	tipBox.SetMaxTipWidth(GetSystemMetrics(SM_CXFULLSCREEN) / 2);
	tipBox.SetTitle(NULL, "可能的内容（当前模式: 全部 shift切换）");
	tipBox.SetDelayTime(TTDT_AUTOPOP, 30000);//显示时长（最多30秒）
	tipBox.SetDelayTime(TTDT_INITIAL, 0);//停靠多久显示
	tipBox.SetDelayTime(TTDT_RESHOW, 0);//重新显示的间隔

	//添加定时器
	union {
		void (CDataAnalyDlg::* showDataTip)(HWND, UINT, CDataAnalyDlg*, DWORD);
		TIMERPROC timerProc;
	}func2;
	func2.showDataTip = &CDataAnalyDlg::ShowDataTip;
	SetTimer(0, 100, func2.timerProc);
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
	if (WM_KEYFIRST <= pMsg->message && pMsg->message <= WM_KEYLAST
		&& pMsg->wParam == 0x46 && GetKeyState(VK_CONTROL) < 0)
	{
		SearchData();
	}

	if (WM_KEYFIRST <= pMsg->message && pMsg->message <= WM_KEYLAST
		&& pMsg->wParam == VK_SHIFT && !((ULONG)pMsg->lParam >> 31))
	{
		//窗口未显示或数据框无焦点 退出
		if (IsWindowVisible() == false)
			return CDialogEx::PreTranslateMessage(pMsg);
		if (GetFocus() != &dataEdit)
			return CDialogEx::PreTranslateMessage(pMsg);

		//切换数据推算模式
		isStrOnly = !isStrOnly;
		CString title;
		title.Format("可能的内容（当前模式: %s shift切换）", isStrOnly ? "仅字符串" : "全部");
		tipBox.SetTitle(NULL, title);

		//推算数据
		InferredData();

		//更新tip文本
		int len = calcResult.GetLength();
		len = len > 7600 ? 7600 : len;
		char* tmp = new char[len + 1]{};
		memcpy(tmp, calcResult, len);
		if (tipText)delete[] tipText;
		tipText = tmp;

		//刷新tipBox
		tipBox.Update();
	}

	tipBox.RelayEvent(pMsg);
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CDataAnalyDlg::ShowDataTip(HWND, UINT, CDataAnalyDlg*, DWORD)
{
	//窗口未显示或未选择数据 退出
	if (thiz->IsWindowVisible() == false)
	{
		thiz->tipBox.Activate(false);
		thiz->startShow = GetTickCount64();
		return;
	}
	int startChar, endChar;
	thiz->dataEdit.GetSel(startChar, endChar);
	if ((startChar - endChar) == 0)
	{
		thiz->tipBox.Activate(false);
		thiz->startShow = GetTickCount64();
		return;
	}

	//数据推算
	if (startChar != thiz->startChar || endChar != thiz->endChar)
	{
		//推算数据
		thiz->InferredData();

		//更新tip文本
		int len = thiz->calcResult.GetLength();
		len = len > 7600 ? 7600 : len;
		char* tmp = new char[len + 1]{};
		memcpy(tmp, thiz->calcResult, len);
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
				if (GetTickCount64() - thiz->startShow > 29000)
				{
					thiz->tipBox.Update();
					thiz->startShow = GetTickCount64();
				}
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
		if (GetTickCount64() - thiz->startShow > 29000)
		{
			thiz->tipBox.Update();
			thiz->startShow = GetTickCount64();
		}
		return;
	}
	thiz->tipBox.Activate(false);
	thiz->startShow = GetTickCount64();
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

void CDataAnalyDlg::InferredData()
{
	//获取数据
	dataEdit.GetSel(startChar, endChar);
	CString data;
	dataEdit.GetWindowTextA(data);
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
	calcResult = "";
	CString str;
	if (len > 0)
	{
		str.Format("ASCII: %s\r\n", (char*)buf);
		calcResult += str;
	}

	CStringW wstr;
	if (len > 1)
	{
		wstr.Format(L"UTF-16: %s\r\n", (wchar_t*)buf);
		calcResult += wstr;
	}

	if (isStrOnly == false)
	{
		if (len > 0 && len <= 20)
		{
			calcResult += "char: ";
			for (int i = 0; i < len; i++)
			{
				str.Format("[%hhd] ", (char)buf[i]);
				calcResult += str;
			}
			calcResult += "\r\n";
		}

		if (len > 0 && len <= 20)
		{
			calcResult += "uchar: ";
			for (int i = 0; i < len; i++)
			{
				str.Format("[%hhu] ", buf[i]);
				calcResult += str;
			}
			calcResult += "\r\n";
		}


		if (len > 1)
		{
			calcResult += "short: ";
			for (int i = 0; i < len / 2; i++)
			{
				short* value = (short*)buf;
				str.Format("[%hd] ", value[i]);
				calcResult += str;
			}
			calcResult += "\r\n";

			calcResult += "ushort: ";
			for (int i = 0; i < len / 2; i++)
			{
				unsigned short* value = (unsigned short*)buf;
				str.Format("[%hu] ", value[i]);
				calcResult += str;
			}
			calcResult += "\r\n";
		}

		if (len > 3)
		{
			calcResult += "int: ";
			for (int i = 0; i < len / 4; i++)
			{
				int* value = (int*)buf;
				str.Format("[%d] ", value[i]);
				calcResult += str;
			}
			calcResult += "\r\n";

			calcResult += "uint: ";
			for (int i = 0; i < len / 4; i++)
			{
				unsigned int* value = (unsigned int*)buf;
				str.Format("[%u] ", value[i]);
				calcResult += str;
			}
			calcResult += "\r\n";
		}

		if (len > 7)
		{
			calcResult += "llong: ";
			for (int i = 0; i < len / 8; i++)
			{
				long long* value = (long long*)buf;
				str.Format("[%lld] ", value[i]);
				calcResult += str;
			}
			calcResult += "\r\n";

			calcResult += "ullong: ";
			for (int i = 0; i < len / 8; i++)
			{
				unsigned long long* value = (unsigned long long*)buf;
				str.Format("[%llu] ", value[i]);
				calcResult += str;
			}
			calcResult += "\r\n";
		}

		if (len > 3)
		{
			calcResult += "float: ";
			for (int i = 0; i < len / 4; i++)
			{
				float* value = (float*)buf;
				str.Format("[%f] ", value[i]);
				calcResult += str;
			}
			calcResult += "\r\n";
		}

		if (len > 7)
		{
			calcResult += "double: ";
			for (int i = 0; i < len / 8; i++)
			{
				double* value = (double*)buf;
				str.Format("[%lf] ", value[i]);
				calcResult += str;
			}
			calcResult += "\r\n";
		}
	}

	//清理数据
	delete[] buf;
}

void CDataAnalyDlg::SearchData()
{
	if (searchDlg.DoModal() == IDOK && searchDlg.value.GetLength() > 0)
	{
		//给焦点激活 光标
		dataEdit.SetFocus();

		//获取开始位置
		int start = 0;
		if (searchDlg.isSearchNext)
		{
			start = LOWORD(dataEdit.CharFromPos(dataEdit.GetCaretPos()));
		}

		//获取数据
		CString data;
		dataEdit.GetWindowTextA(data);

		//搜索数据
		CString tmp, value;
		if (searchDlg.dataType == "ASCII")
		{
			int len = searchDlg.value.GetLength();
			for (int i = 0; i < len; i++)
			{
				tmp.Format("%.2X ", (UCHAR)searchDlg.value[i]);
				value += tmp;
			}
		}

		CStringW wstr;
		if (searchDlg.dataType == "UTF-16")
		{
			wstr = searchDlg.value;
			int len = wstr.GetLength() * 2;
			unsigned char* data = (unsigned char*)wstr.GetBuffer();
			for (int i = 0; i < len; i++)
			{
				tmp.Format("%.2X ", data[i]);
				value += tmp;
			}
		}

		if (searchDlg.dataType == "char")
		{
			char data = strtol(searchDlg.value, NULL, searchDlg.isHex ? 16 : 10);
			value.Format("%.2X ", (UCHAR)data);
		}

		if (searchDlg.dataType == "unsigned char")
		{
			unsigned char data = strtoul(searchDlg.value, NULL, searchDlg.isHex ? 16 : 10);
			value.Format("%.2X ", data);
		}

		if (searchDlg.dataType == "short")
		{
			short data = strtol(searchDlg.value, NULL, searchDlg.isHex ? 16 : 10);
			unsigned char* p = (unsigned char*)&data;
			for (int i = 0; i < sizeof(short); i++)
			{
				tmp.Format("%.2X ", p[i]);
				value += tmp;
			}
		}

		if (searchDlg.dataType == "unsigned short" || searchDlg.dataType == "命令")
		{
			unsigned short data = strtoul(searchDlg.value, NULL, searchDlg.isHex ? 16 : 10);
			unsigned char* p = (unsigned char*)&data;
			for (int i = 0; i < sizeof(unsigned short); i++)
			{
				tmp.Format("%.2X ", p[i]);
				value += tmp;
			}
		}

		if (searchDlg.dataType == "int")
		{
			int data = strtol(searchDlg.value, NULL, searchDlg.isHex ? 16 : 10);
			unsigned char* p = (unsigned char*)&data;
			for (int i = 0; i < sizeof(int); i++)
			{
				tmp.Format("%.2X ", p[i]);
				value += tmp;
			}
		}

		if (searchDlg.dataType == "unsigned int")
		{
			unsigned int data = strtoul(searchDlg.value, NULL, searchDlg.isHex ? 16 : 10);
			unsigned char* p = (unsigned char*)&data;
			for (int i = 0; i < sizeof(unsigned int); i++)
			{
				tmp.Format("%.2X ", p[i]);
				value += tmp;
			}
		}

		if (searchDlg.dataType == "long long")
		{
			long long data = strtoll(searchDlg.value, NULL, searchDlg.isHex ? 16 : 10);
			unsigned char* p = (unsigned char*)&data;
			for (int i = 0; i < sizeof(long long); i++)
			{
				tmp.Format("%.2X ", p[i]);
				value += tmp;
			}
		}

		if (searchDlg.dataType == "unsigned long long")
		{
			unsigned long long data = strtoull(searchDlg.value, NULL, searchDlg.isHex ? 16 : 10);
			unsigned char* p = (unsigned char*)&data;
			for (int i = 0; i < sizeof(unsigned long long); i++)
			{
				tmp.Format("%.2X ", p[i]);
				value += tmp;
			}
		}

		if (searchDlg.dataType == "float")
		{
			float data = strtof(searchDlg.value, NULL);
			unsigned char* p = (unsigned char*)&data;
			for (int i = 0; i < sizeof(float); i++)
			{
				tmp.Format("%.2X ", p[i]);
				value += tmp;
			}
		}

		if (searchDlg.dataType == "double")
		{
			double data = strtod(searchDlg.value, NULL);
			unsigned char* p = (unsigned char*)&data;
			for (int i = 0; i < sizeof(double); i++)
			{
				tmp.Format("%.2X ", p[i]);
				value += tmp;
			}
		}

		if (searchDlg.dataType == "文本")
		{
			value = searchDlg.value;
		}

		CStringW dataW;
		CStringW valueW;
		dataW = data;
		valueW = value;
		int ret = dataW.Find(valueW, start);
		if (ret != -1)
		{
			dataEdit.SetFocus();
			dataEdit.SetSel(ret, ret + valueW.GetLength(), 0);
		}
		return;
	}
}
