// EditDataDlg.cpp: 实现文件
//

#include "pch.h"
#include "DataAnalyser.h"
#include "EditDataDlg.h"
#include "afxdialogex.h"


//静态的内部 thiz指针
CEditDataDlg* CEditDataDlg::thiz = nullptr;


// CEditDataDlg 对话框

IMPLEMENT_DYNAMIC(CEditDataDlg, CDialogEx)

CEditDataDlg::CEditDataDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_EDIT_DATA, pParent)
	, title(_T(""))
	, dataType(_T("ASCII"))
	, startChar(0)
	, endChar(0)
	, tipText(nullptr)
	, isUpdateValue(false)
	, isCancelEdit(FALSE)
	, isStrOnly(false)
	, startShow(GetTickCount64())
{
	thiz = this;
}

CEditDataDlg::~CEditDataDlg()
{
	if (tipText)
	{
		delete[] tipText;
		tipText = nullptr;
	}
}

void CEditDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TITLE, title);
	DDX_Control(pDX, IDC_DATA, dataEdit);
	DDX_CBString(pDX, IDC_DATA_TYPE, dataType);
	DDX_Control(pDX, IDC_VALUE, valueEdit);
	DDX_Check(pDX, IDC_CANCEL_EDIT, isCancelEdit);
}

BOOL CEditDataDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//初始化数值
	isStrOnly = false;
	startShow = GetTickCount64();

	//初始化控件
	searchDlg.dataType = "ASCII";
	dataType = "ASCII";
	valueEdit.SetWindowTextA("");
	valueEdit.SetReadOnly(true);
	dataEdit.SetWindowTextA(data);
	UpdateData(false);

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
		void (CEditDataDlg::* timerHandler)(HWND, UINT, CEditDataDlg*, DWORD);
		TIMERPROC timerProc;
	}func;
	func.timerHandler = &CEditDataDlg::ShowDataTip;
	SetTimer(0, 100, func.timerProc);
	return TRUE;
}


BEGIN_MESSAGE_MAP(CEditDataDlg, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, SetTipText)
	ON_BN_CLICKED(IDC_SEARCH, &CEditDataDlg::OnBnClickedSearch)
	ON_CBN_SELCHANGE(IDC_DATA_TYPE, &CEditDataDlg::OnCbnSelchangeDataType)
	ON_BN_CLICKED(IDOK, &CEditDataDlg::OnBnClickedOk)
	ON_EN_CHANGE(IDC_VALUE, &CEditDataDlg::OnEnChangeValue)
END_MESSAGE_MAP()


// CEditDataDlg 消息处理程序


HBRUSH CEditDataDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd == &dataEdit)
	{
		pDC->SetTextColor(RGB(0, 0, 0)); //字体颜色
		pDC->SetBkMode(TRANSPARENT); //设置字体背景为透明
		return (HBRUSH)::GetStockObject(WHITE_BRUSH);//设置背景颜色
	}
	return hbr;
}

BOOL CEditDataDlg::SetTipText(UINT id, NMHDR* pTTTStruct, LRESULT* pResult)
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

BOOL CEditDataDlg::PreTranslateMessage(MSG* pMsg)
{
	if (WM_KEYFIRST <= pMsg->message && pMsg->message <= WM_KEYLAST
		&& pMsg->wParam == VK_SHIFT && !((ULONG)pMsg->lParam >> 31))
	{
		//数据框无焦点 退出
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
		UpdateTipText();

		//刷新tipBox
		tipBox.Update();
	}

	tipBox.RelayEvent(pMsg);
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CEditDataDlg::ShowDataTip(HWND, UINT, CEditDataDlg*, DWORD)
{
	//未选择数据 退出
	int startChar, endChar;
	thiz->dataEdit.GetSel(startChar, endChar);
	if ((startChar - endChar) == 0)
	{
		thiz->tipBox.Activate(false);
		thiz->startShow = GetTickCount64();
		return thiz->UpdateValueEdit();
	}

	//数据推算
	if (startChar != thiz->startChar || endChar != thiz->endChar)
	{
		//推算数据
		thiz->InferredData();

		//更新tip文本
		thiz->UpdateTipText();

		//更新 value编辑框
		thiz->UpdateValueEdit();
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

void CEditDataDlg::OnBnClickedSearch()
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
			dataType = searchDlg.dataType;
			UpdateData(false);
			UpdateValueEdit();
		}
		return;
	}
}

void CEditDataDlg::OnCbnSelchangeDataType()
{
	UpdateValueEdit();
}

void CEditDataDlg::OnEnChangeValue()
{
	//对值更新同步 退出
	if (isUpdateValue)return;

	//未选中退出
	int startChar, endChar;
	dataEdit.GetSel(startChar, endChar);
	if (startChar - endChar == 0)return;

	//获取数据
	UpdateData(true);
	CString selData, value;
	dataEdit.GetWindowTextA(selData);
	selData = selData.Mid(startChar, endChar - startChar);
	valueEdit.GetWindowTextA(value);

	//文本
	if (dataType == "文本")
	{
		if (value == selData)return;
		dataEdit.ReplaceSel(value);
		dataEdit.SetSel(startChar, startChar + value.GetLength());
		dataEdit.RedrawWindow();
		InferredData();
		UpdateTipText();
		return;
	}

	//数值
	CString selDataWSpace(selData);
	selData.Remove(' ');
	int strLen = selData.GetLength();
	if (strLen % 2)return;
	int len = strLen / 2;

	CString tmp, newData;
	if (dataType == "ASCII" && len > 0)
	{
		int valueLen = value.GetLength();
		for (int i = 0; i < valueLen; i++)
		{
			tmp.Format("%.2X ", (UCHAR)value[i]);
			newData += tmp;
		}

		int count = selDataWSpace.Find("00");
		if (count == -1)
		{
			if (selDataWSpace[selDataWSpace.GetLength() - 1] != ' ')
				newData.TrimRight();
		}
		else
		{
			newData += selDataWSpace.Right(selDataWSpace.GetLength() - count);
		}
		if (selDataWSpace[0] == ' ')
			newData = ' ' + newData;
		dataEdit.ReplaceSel(newData);
		int end = count == -1 ?
			newData.GetLength()
			: newData.Find("00") + 2 + (selDataWSpace[selDataWSpace.GetLength() - 1] == ' ');
		dataEdit.SetSel(startChar, startChar + end);
		dataEdit.RedrawWindow();
		InferredData();
		UpdateTipText();
		return;
	}

	CStringW wstr;
	if (dataType == "UTF-16" && len > 1 && len % 2 == 0)
	{
		wstr = value;
		int wstrLen = wstr.GetLength() * 2;
		unsigned char* p = (unsigned char*)wstr.GetBuffer();
		for (int i = 0; i < wstrLen; i++)
		{
			tmp.Format("%.2X ", p[i]);
			newData += tmp;
		}

		int count = 0, start = 0;
		do
		{
			count = selDataWSpace.Find("00 00", start);
			start = count + 2;
		} while (count != -1 && count % 2);

		int newDataCount;
		if (count == -1)
		{
			if (selDataWSpace[selDataWSpace.GetLength() - 1] != ' ')
				newData.TrimRight();
		}
		else
		{
			newDataCount = newData.GetLength();
			newData += selDataWSpace.Right(selDataWSpace.GetLength() - count);
		}
		if (selDataWSpace[0] == ' ')
			newData = ' ' + newData;
		dataEdit.ReplaceSel(newData);
		int end = count == -1 ?
			newData.GetLength()
			: newDataCount + 5 + (selDataWSpace[selDataWSpace.GetLength() - 1] == ' ');
		dataEdit.SetSel(startChar, startChar + end);
		dataEdit.RedrawWindow();
		InferredData();
		UpdateTipText();
		return;
	}

	if (dataType == "char" && len == 1)
	{
		char tmpData = strtol(value, NULL, 10);
		newData.Format("%.2X ", (UCHAR)tmpData);

		if (selDataWSpace[selDataWSpace.GetLength() - 1] != ' ')
			newData.TrimRight();
		if (selDataWSpace[0] == ' ')
			newData = ' ' + newData;
		dataEdit.ReplaceSel(newData);
		dataEdit.SetSel(startChar, startChar + newData.GetLength());
		dataEdit.RedrawWindow();
		InferredData();
		UpdateTipText();
		return;
	}

	if (dataType == "unsigned char" && len == 1)
	{
		unsigned char tmpData = strtoul(value, NULL, 10);
		newData.Format("%.2X ", tmpData);

		if (selDataWSpace[selDataWSpace.GetLength() - 1] != ' ')
			newData.TrimRight();
		if (selDataWSpace[0] == ' ')
			newData = ' ' + newData;
		dataEdit.ReplaceSel(newData);
		dataEdit.SetSel(startChar, startChar + newData.GetLength());
		dataEdit.RedrawWindow();
		InferredData();
		UpdateTipText();
		return;
	}

	if (dataType == "short" && len == 2)
	{
		short tmpData = strtol(value, NULL, 10);
		unsigned char* p = (unsigned char*)&tmpData;
		for (int i = 0; i < sizeof(short); i++)
		{
			tmp.Format("%.2X ", p[i]);
			newData += tmp;
		}

		if (selDataWSpace[selDataWSpace.GetLength() - 1] != ' ')
			newData.TrimRight();
		if (selDataWSpace[0] == ' ')
			newData = ' ' + newData;
		dataEdit.ReplaceSel(newData);
		dataEdit.SetSel(startChar, startChar + newData.GetLength());
		dataEdit.RedrawWindow();
		InferredData();
		UpdateTipText();
		return;
	}

	if (dataType == "unsigned short" && len == 2)
	{
		unsigned short tmpData = strtoul(value, NULL, 10);
		unsigned char* p = (unsigned char*)&tmpData;
		for (int i = 0; i < sizeof(unsigned short); i++)
		{
			tmp.Format("%.2X ", p[i]);
			newData += tmp;
		}

		if (selDataWSpace[selDataWSpace.GetLength() - 1] != ' ')
			newData.TrimRight();
		if (selDataWSpace[0] == ' ')
			newData = ' ' + newData;
		dataEdit.ReplaceSel(newData);
		dataEdit.SetSel(startChar, startChar + newData.GetLength());
		dataEdit.RedrawWindow();
		InferredData();
		UpdateTipText();
		return;
	}

	if (dataType == "int" && len == 4)
	{
		int tmpData = strtol(value, NULL, 10);
		unsigned char* p = (unsigned char*)&tmpData;
		for (int i = 0; i < sizeof(int); i++)
		{
			tmp.Format("%.2X ", p[i]);
			newData += tmp;
		}

		if (selDataWSpace[selDataWSpace.GetLength() - 1] != ' ')
			newData.TrimRight();
		if (selDataWSpace[0] == ' ')
			newData = ' ' + newData;
		dataEdit.ReplaceSel(newData);
		dataEdit.SetSel(startChar, startChar + newData.GetLength());
		dataEdit.RedrawWindow();
		InferredData();
		UpdateTipText();
		return;
	}

	if (dataType == "unsigned int" && len == 4)
	{
		unsigned int tmpData = strtoul(value, NULL, 10);
		unsigned char* p = (unsigned char*)&tmpData;
		for (int i = 0; i < sizeof(unsigned int); i++)
		{
			tmp.Format("%.2X ", p[i]);
			newData += tmp;
		}

		if (selDataWSpace[selDataWSpace.GetLength() - 1] != ' ')
			newData.TrimRight();
		if (selDataWSpace[0] == ' ')
			newData = ' ' + newData;
		dataEdit.ReplaceSel(newData);
		dataEdit.SetSel(startChar, startChar + newData.GetLength());
		dataEdit.RedrawWindow();
		InferredData();
		UpdateTipText();
		return;
	}

	if (dataType == "long long" && len == 8)
	{
		long long tmpData = strtoll(value, NULL, 10);
		unsigned char* p = (unsigned char*)&tmpData;
		for (int i = 0; i < sizeof(long long); i++)
		{
			tmp.Format("%.2X ", p[i]);
			newData += tmp;
		}

		if (selDataWSpace[selDataWSpace.GetLength() - 1] != ' ')
			newData.TrimRight();
		if (selDataWSpace[0] == ' ')
			newData = ' ' + newData;
		dataEdit.ReplaceSel(newData);
		dataEdit.SetSel(startChar, startChar + newData.GetLength());
		dataEdit.RedrawWindow();
		InferredData();
		UpdateTipText();
		return;
	}

	if (dataType == "unsigned long long" && len == 8)
	{
		unsigned long long tmpData = strtoull(value, NULL, 10);
		unsigned char* p = (unsigned char*)&tmpData;
		for (int i = 0; i < sizeof(unsigned long long); i++)
		{
			tmp.Format("%.2X ", p[i]);
			newData += tmp;
		}

		if (selDataWSpace[selDataWSpace.GetLength() - 1] != ' ')
			newData.TrimRight();
		if (selDataWSpace[0] == ' ')
			newData = ' ' + newData;
		dataEdit.ReplaceSel(newData);
		dataEdit.SetSel(startChar, startChar + newData.GetLength());
		dataEdit.RedrawWindow();
		InferredData();
		UpdateTipText();
		return;
	}

	if (dataType == "float" && len == 4)
	{
		float tmpData = strtof(value, NULL);
		unsigned char* p = (unsigned char*)&tmpData;
		for (int i = 0; i < sizeof(float); i++)
		{
			tmp.Format("%.2X ", p[i]);
			newData += tmp;
		}

		if (selDataWSpace[selDataWSpace.GetLength() - 1] != ' ')
			newData.TrimRight();
		if (selDataWSpace[0] == ' ')
			newData = ' ' + newData;
		dataEdit.ReplaceSel(newData);
		dataEdit.SetSel(startChar, startChar + newData.GetLength());
		dataEdit.RedrawWindow();
		InferredData();
		UpdateTipText();
		return;
	}

	if (dataType == "double" && len == 8)
	{
		double tmpData = strtod(value, NULL);
		unsigned char* p = (unsigned char*)&tmpData;
		for (int i = 0; i < sizeof(double); i++)
		{
			tmp.Format("%.2X ", p[i]);
			newData += tmp;
		}

		if (selDataWSpace[selDataWSpace.GetLength() - 1] != ' ')
			newData.TrimRight();
		if (selDataWSpace[0] == ' ')
			newData = ' ' + newData;
		dataEdit.ReplaceSel(newData);
		dataEdit.SetSel(startChar, startChar + newData.GetLength());
		dataEdit.RedrawWindow();
		InferredData();
		UpdateTipText();
		return;
	}
}

void CEditDataDlg::OnBnClickedOk()
{
	dataEdit.GetWindowTextA(data);
	CDialogEx::OnOK();
}


//成员函数实现

void CEditDataDlg::InferredData()
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

void CEditDataDlg::UpdateTipText()
{
	int len = calcResult.GetLength();
	len = len > 7600 ? 7600 : len;
	char* tmp = new char[len + 1]{};
	memcpy(tmp, calcResult, len);
	if (tipText)delete[] tipText;
	tipText = tmp;
}

void CEditDataDlg::UpdateValueEdit()
{
	//未选中 禁用值编辑框
	int startChar, endChar;
	dataEdit.GetSel(startChar, endChar);
	if (startChar - endChar == 0)
	{
		isUpdateValue = true;
		valueEdit.SetWindowTextA("");
		isUpdateValue = false;
		valueEdit.SetReadOnly(true);
		return;
	}

	//获取数据
	UpdateData(true);
	CString selData, value;
	dataEdit.GetWindowTextA(selData);
	selData = selData.Mid(startChar, endChar - startChar);
	valueEdit.GetWindowTextA(value);

	//文本
	if (dataType == "文本")
	{
		if (selData == value)return;
		isUpdateValue = true;
		valueEdit.SetWindowTextA(selData);
		isUpdateValue = false;
		valueEdit.SetReadOnly(false);
		return;
	}

	//数值
	selData.Remove(' ');
	int strLen = selData.GetLength();
	if (strLen % 2)
	{
		isUpdateValue = true;
		valueEdit.SetWindowTextA("");
		isUpdateValue = false;
		valueEdit.SetReadOnly(true);
		return;
	}

	int len = strLen / 2;
	unsigned char* buf = new unsigned char[len + 4]{};
	for (int i = 0; i < len; i++)
	{
		buf[i] = strtoul(selData.Mid(i * 2, 2), NULL, 16);
	}

	CString str;
	if (dataType == "ASCII" && len > 0)
	{
		str.Format("%s", (char*)buf);
		if (str == value)return delete[] buf;
		isUpdateValue = true;
		valueEdit.SetWindowTextA(str);
		isUpdateValue = false;
		valueEdit.SetReadOnly(false);
		return delete[] buf;
	}

	CStringW wstr;
	if (dataType == "UTF-16" && len > 1 && len % 2 == 0)
	{
		wstr.Format(L"%s", (wchar_t*)buf);
		str = wstr;
		if (str == value)return delete[] buf;
		isUpdateValue = true;
		valueEdit.SetWindowTextA(str);
		isUpdateValue = false;
		valueEdit.SetReadOnly(false);
		return delete[] buf;
	}

	if (dataType == "char" && len == 1)
	{
		str.Format("%hhd", (char)buf[0]);
		if (str == value)return delete[] buf;
		isUpdateValue = true;
		valueEdit.SetWindowTextA(str);
		isUpdateValue = false;
		valueEdit.SetReadOnly(false);
		return delete[] buf;
	}

	if (dataType == "unsigned char" && len == 1)
	{
		str.Format("%hhu", buf[0]);
		if (str == value)return delete[] buf;
		isUpdateValue = true;
		valueEdit.SetWindowTextA(str);
		isUpdateValue = false;
		valueEdit.SetReadOnly(false);
		return delete[] buf;
	}

	if (dataType == "short" && len == 2)
	{
		short* p = (short*)buf;
		str.Format("%hd", p[0]);
		if (str == value)return delete[] buf;
		isUpdateValue = true;
		valueEdit.SetWindowTextA(str);
		isUpdateValue = false;
		valueEdit.SetReadOnly(false);
		return delete[] buf;
	}

	if (dataType == "unsigned short" && len == 2)
	{
		unsigned short* p = (unsigned short*)buf;
		str.Format("%hu", p[0]);
		if (str == value)return delete[] buf;
		isUpdateValue = true;
		valueEdit.SetWindowTextA(str);
		isUpdateValue = false;
		valueEdit.SetReadOnly(false);
		return delete[] buf;
	}

	if (dataType == "int" && len == 4)
	{
		int* p = (int*)buf;
		str.Format("%d", p[0]);
		if (str == value)return delete[] buf;
		isUpdateValue = true;
		valueEdit.SetWindowTextA(str);
		isUpdateValue = false;
		valueEdit.SetReadOnly(false);
		return delete[] buf;
	}

	if (dataType == "unsigned int" && len == 4)
	{
		unsigned int* p = (unsigned int*)buf;
		str.Format("%u", p[0]);
		if (str == value)return delete[] buf;
		isUpdateValue = true;
		valueEdit.SetWindowTextA(str);
		isUpdateValue = false;
		valueEdit.SetReadOnly(false);
		return delete[] buf;
	}

	if (dataType == "long long" && len == 8)
	{
		long long* p = (long long*)buf;
		str.Format("%lld", p[0]);
		if (str == value)return delete[] buf;
		isUpdateValue = true;
		valueEdit.SetWindowTextA(str);
		isUpdateValue = false;
		valueEdit.SetReadOnly(false);
		return delete[] buf;
	}

	if (dataType == "unsigned long long" && len == 8)
	{
		unsigned long long* p = (unsigned long long*)buf;
		str.Format("%llu", p[0]);
		if (str == value)return delete[] buf;
		isUpdateValue = true;
		valueEdit.SetWindowTextA(str);
		isUpdateValue = false;
		valueEdit.SetReadOnly(false);
		return delete[] buf;
	}

	if (dataType == "float" && len == 4)
	{
		float* p = (float*)buf;
		str.Format("%f", p[0]);
		if (str == value)return delete[] buf;
		isUpdateValue = true;
		valueEdit.SetWindowTextA(str);
		isUpdateValue = false;
		valueEdit.SetReadOnly(false);
		return delete[] buf;

	}

	if (dataType == "double" && len == 8)
	{
		double* p = (double*)buf;
		str.Format("%lf", p[0]);
		if (str == value)return delete[] buf;
		isUpdateValue = true;
		valueEdit.SetWindowTextA(str);
		isUpdateValue = false;
		valueEdit.SetReadOnly(false);
		return delete[] buf;
	}

	//未满足条件 禁用值编辑框
	isUpdateValue = true;
	valueEdit.SetWindowTextA("");
	isUpdateValue = false;
	valueEdit.SetReadOnly(true);
	return delete[] buf;
}
