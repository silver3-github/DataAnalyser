
// DataAnalyserDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "DataAnalyser.h"
#include "DataAnalyserDlg.h"
#include "afxdialogex.h"
#include "CServer.h"
#include "MsgData.h"
#include "CConfig.h"
#include <io.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//自定义窗口消息
#define WM_RECVMSG (WM_USER+100)


// CDataAnalyserDlg 对话框



CDataAnalyserDlg::CDataAnalyserDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DATAANALYSER_DIALOG, pParent)
	, isPause(FALSE)
	, msgCurSel(-1)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDataAnalyserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MSGLIST, msgList);
	DDX_Check(pDX, IDC_PAUSE, isPause);
}

BEGIN_MESSAGE_MAP(CDataAnalyserDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_RECVMSG, &CDataAnalyserDlg::OnRecvMsgHandler)
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_NOTIFY(HDN_ENDTRACK, 0, &CDataAnalyserDlg::OnHdnEndtrackMsglist)
	ON_NOTIFY(LVN_INSERTITEM, IDC_MSGLIST, &CDataAnalyserDlg::OnLvnInsertitemMsglist)
	ON_BN_CLICKED(IDC_OPEN_CONFIG, &CDataAnalyserDlg::OnBnClickedOpenConfig)
	ON_BN_CLICKED(IDC_CLEAR, &CDataAnalyserDlg::OnBnClickedClear)
	ON_BN_CLICKED(IDC_LOAD_CONFIG, &CDataAnalyserDlg::OnBnClickedLoadConfig)
	ON_BN_CLICKED(IDC_NEW_CONFIG, &CDataAnalyserDlg::OnBnClickedNewConfig)
	ON_BN_CLICKED(IDC_OPEN_LOG, &CDataAnalyserDlg::OnBnClickedOpenLog)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_MSGLIST, &CDataAnalyserDlg::OnLvnItemchangedMsglist)
	ON_WM_MOVE()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_MSGLIST, &CDataAnalyserDlg::OnNMCustomdrawMsglist)
	ON_NOTIFY(NM_CLICK, IDC_MSGLIST, &CDataAnalyserDlg::OnNMClickMsglist)
	ON_NOTIFY(NM_DBLCLK, IDC_MSGLIST, &CDataAnalyserDlg::OnNMDblclkMsglist)
	ON_BN_CLICKED(IDC_DATA_SEARCH, &CDataAnalyserDlg::OnBnClickedDataSearch)
	ON_NOTIFY(NM_RDBLCLK, IDC_MSGLIST, &CDataAnalyserDlg::OnNMRDblclkMsglist)
	ON_NOTIFY(NM_RCLICK, IDC_MSGLIST, &CDataAnalyserDlg::OnNMRClickMsglist)
END_MESSAGE_MAP()


// CDataAnalyserDlg 消息处理程序

BOOL CDataAnalyserDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	//设置消息列表样式
	DWORD dwStyle = msgList.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;//整行选中
	dwStyle |= LVS_EX_GRIDLINES;//网格线
	msgList.SetExtendedStyle(dwStyle);
	msgList.InsertColumn(0, "保留");
	msgList.InsertColumn(1, "消息", LVCFMT_CENTER, 100);
	msgList.InsertColumn(2, "命令", LVCFMT_CENTER, 120);
	msgList.InsertColumn(3, "数据", LVCFMT_CENTER, 450);
	msgList.InsertColumn(4, "长度", LVCFMT_CENTER, 65);
	msgList.InsertColumn(5, "时间", LVCFMT_CENTER, 80);
	msgList.DeleteColumn(0);

	//创建子对话框
	dataAnalyDlg.Create(IDD_DATA_ANALY, this);

	//激活服务器
	union {
		void(__stdcall* recvHandler)(char*, unsigned, void*);
		void(CDataAnalyserDlg::* onRecvMsg)(char*, unsigned, CDataAnalyserDlg*);
	}func;
	func.onRecvMsg = &CDataAnalyserDlg::OnRecvMsg;
	CServer::Instance.Invoke("127.0.0.1", 13132, func.recvHandler, this);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CDataAnalyserDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CDataAnalyserDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDataAnalyserDlg::OnOK() {}
void CDataAnalyserDlg::OnCancel() {}
void CDataAnalyserDlg::OnClose()
{
	CDialogEx::OnCancel();
}

void CDataAnalyserDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (nType == 0 || nType == 2)
	{
		AdjustListClumn();
		if (dataAnalyDlg.IsWindowVisible())
			dataAnalyDlg.AdjustPosition();
	}
}

void CDataAnalyserDlg::OnMove(int x, int y)
{
	CDialogEx::OnMove(x, y);

	if (dataAnalyDlg.IsWindowVisible())
		dataAnalyDlg.AdjustPosition();
}


//消息列表处理

void CDataAnalyserDlg::OnHdnEndtrackMsglist(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	*pResult = 0;
	AdjustListClumn();
}

void CDataAnalyserDlg::OnLvnInsertitemMsglist(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;
	AdjustListClumn();
}

void CDataAnalyserDlg::OnLvnItemchangedMsglist(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW p = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;

	if (p->uChanged == LVIF_STATE)
	{
		if (p->uOldState == LVNI_SELECTED) //取消选中消息项
		{
			msgCurSel = -1;
			dataAnalyDlg.SetWindowTextA("数据分析");
			dataAnalyDlg.dataEdit.SetWindowTextA("");
			dataAnalyDlg.UpdateData(false);
		}

		if (p->uNewState == (LVNI_FOCUSED | LVNI_SELECTED)) //选中消息项
		{
			msgCurSel = p->iItem;
			CString title;
			title.Format("消息: %s  命令: %s  长度: %s  时间: %s",
				msgList.GetItemText(p->iItem, 0).GetBuffer(),
				msgList.GetItemText(p->iItem, 1).GetBuffer(),
				msgList.GetItemText(p->iItem, 3).GetBuffer(),
				msgList.GetItemText(p->iItem, 4).GetBuffer());
			dataAnalyDlg.SetWindowTextA(title);
			dataAnalyDlg.dataEdit.SetWindowTextA(msgList.GetItemText(p->iItem, 2));
			dataAnalyDlg.UpdateData(false);

			if (!dataAnalyDlg.IsWindowVisible())
			{
				dataAnalyDlg.ShowWindow(true);
				dataAnalyDlg.AdjustPosition();
			}
		}
	}
}

void CDataAnalyserDlg::OnNMCustomdrawMsglist(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLVCUSTOMDRAW p = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);
	*pResult = CDRF_DODEFAULT;

	switch (p->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW;
		break;
	case CDDS_ITEMPREPAINT:
		CmdDefine* cmd = (CmdDefine*)msgList.GetItemData(p->nmcd.dwItemSpec);
		if (cmd)
		{
			p->clrText = cmd->textColor;
			p->clrTextBk = cmd->bgColor;
		}
		*pResult = CDRF_DODEFAULT;
		break;
	}
}

void CDataAnalyserDlg::OnNMClickMsglist(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE p = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	if (msgCurSel != -1 && !dataAnalyDlg.IsWindowVisible())
	{
		dataAnalyDlg.ShowWindow(true);
		dataAnalyDlg.AdjustPosition();
	}

	if (p->iItem == -1)
	{
		dataAnalyDlg.ShowWindow(false);
	}
}

void CDataAnalyserDlg::OnNMDblclkMsglist(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE p = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	if (msgCurSel != -1 && !dataAnalyDlg.IsWindowVisible())
	{
		dataAnalyDlg.ShowWindow(true);
		dataAnalyDlg.AdjustPosition();
	}

	if (p->iItem == -1)
	{
		dataAnalyDlg.ShowWindow(false);
	}
}

void CDataAnalyserDlg::OnNMRDblclkMsglist(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	if (msgCurSel != -1)
	{
		msgList.SetItemState(msgCurSel, 0, LVIS_FOCUSED | LVIS_SELECTED);
		msgCurSel = -1;
	}
	dataAnalyDlg.ShowWindow(false);
}

void CDataAnalyserDlg::OnNMRClickMsglist(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	if (msgCurSel != -1)
	{
		msgList.SetItemState(msgCurSel, 0, LVIS_FOCUSED | LVIS_SELECTED);
		msgCurSel = -1;
	}
	dataAnalyDlg.ShowWindow(false);
}


//按钮处理

void CDataAnalyserDlg::OnBnClickedClear()
{
	msgList.DeleteAllItems();
	AdjustListClumn();
}

void CDataAnalyserDlg::OnBnClickedDataSearch()
{
	if (searchDlg.DoModal() == IDOK && searchDlg.value.GetLength() > 0)
	{
		//获取开始位置
		int startItem = msgCurSel >= 0 ? msgCurSel + searchDlg.isSearchNext : 0;

		//命令
		CString value;
		if (searchDlg.dataType == "命令")
		{
			value.Format("%u", strtoul(searchDlg.value, NULL, searchDlg.isHex ? 16 : 10));

			for (int i = startItem; i < msgList.GetItemCount(); i++)
			{
				CString cmdId;
				cmdId = msgList.GetItemText(i, 1);
				int first = cmdId.Find('(');
				if (first != -1)
					cmdId = cmdId.Mid(first + 1, cmdId.Find(')') - first - 1);
				if (cmdId == value)
				{
					msgList.SetItemState(i, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
					msgList.SetFocus();
					msgList.EnsureVisible(i, false);
					return;
				}
			}
			return;
		}

		//数值和文本
		CString tmp;
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

		if (searchDlg.dataType == "unsigned short")
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
		for (int i = startItem; i < msgList.GetItemCount(); i++)
		{
			dataW = msgList.GetItemText(i, 2);
			valueW = value;
			int ret = dataW.Find(valueW);
			if (ret != -1)
			{
				msgList.SetItemState(i, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
				dataAnalyDlg.dataEdit.SetSel(ret, ret + valueW.GetLength(), 0);
				msgList.SetFocus();
				msgList.EnsureVisible(i, false);
				return;
			}
		}
	}
}

void CDataAnalyserDlg::OnBnClickedOpenLog()
{
	//检测目录
	CConfig* config = &CConfig::Instance;
	if (_access(config->logFPath.c_str(), 0))
	{
		CreateDirectoryA(config->logFPath.c_str(), NULL);
	}

	std::string folder = config->logFPath + config->configFile + "\\";
	if (_access(folder.c_str(), 0))
	{
		CreateDirectoryA(folder.c_str(), NULL);
	}

	//打开日志目录
	ShellExecuteA(NULL, NULL, folder.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

void CDataAnalyserDlg::OnBnClickedNewConfig()
{
	CConfig* config = &CConfig::Instance;
	CFileDialog fileDlg(false, ".ini", "新建配置文件", 6, "配置文件 (*.ini)|*.ini||", this);
	fileDlg.m_ofn.lpstrInitialDir = config->configFPath.c_str();
	if (fileDlg.DoModal() == IDOK)
	{
		//检查文件夹
		if (_access(config->configFPath.c_str(), 0))
		{
			CreateDirectoryA(config->configFPath.c_str(), NULL);
		}

		//创建新文件
		std::string newFile = config->configFPath + fileDlg.GetFileTitle().GetBuffer() + ".ini";
		FILE* f;
		if (fopen_s(&f, newFile.c_str(), "w+"))
		{
			MessageBoxA("新建失败,请输入合法的配置文件名", "新建配置文件");
			return;
		}
		fclose(f);

		//保存当前的配置文件
		if (config->configFile.size())
		{
			config->SaveToFile(config->configFile.c_str());
		}

		//加载新配置文件
		config->LoadFromFile(fileDlg.GetFileTitle());
	}
}

void CDataAnalyserDlg::OnBnClickedLoadConfig()
{
	CConfig* config = &CConfig::Instance;
	CFileDialog fileDlg(true, 0, 0, 6, "配置文件 (*.ini)|*.ini||", this);
	fileDlg.m_ofn.lpstrInitialDir = config->configFPath.c_str();
	if (fileDlg.DoModal() == IDOK)
	{
		//保存当前的配置文件
		if (config->configFile.size())
		{
			config->SaveToFile(config->configFile.c_str());
		}

		//加载所选的配置文件
		if (!config->LoadFromFile(fileDlg.GetFileTitle()))
		{
			MessageBoxA("加载失败,配置文件不存在！", "加载配置文件");
			return;
		}
	}
}

void CDataAnalyserDlg::OnBnClickedOpenConfig()
{
	configDlg.configFileName = CConfig::Instance.configFile.c_str();
	if (configDlg.DoModal() == IDOK)
	{
		if (!CConfig::Instance.SaveToFile(configDlg.configFileName))
		{
			MessageBoxA("创建配置文件失败，请输入合法的文件名", "保存配置数据");
			return;
		}
	}
}


//接收消息的处理程序

LRESULT CDataAnalyserDlg::OnRecvMsgHandler(WPARAM buf, LPARAM len)
{
	//暂停实现
	MsgData retMsgData;
	UpdateData(true);
	if (isPause)
	{
		delete[](char*)buf;
		retMsgData.Init("", 0, 0, 0, 0);
		retMsgData.ToBuffer();
		CServer::Instance.Send(retMsgData.buffer, retMsgData.bufLen);
		return 0;
	}

	//获取消息数据
	MsgData msgData;
	msgData.Parse((char*)buf, len);
	delete[](char*)buf;

	//检测消息是否定义
	CConfig* config = &CConfig::Instance;
	auto msgIt = config->msgDefs.find(msgData.msg);
	if (msgIt == config->msgDefs.end())
	{
		retMsgData.Init("", 0, 0, 0, 0);
		retMsgData.ToBuffer();
		CServer::Instance.Send(retMsgData.buffer, retMsgData.bufLen);
		return 0;
	}

	//无需同步编辑,先回发继续执行（让客户端和服务端同时处理数据，防止卡顿）
	auto cmdIt = msgIt->second.cmdDefs.find(msgData.cmd);
	if (cmdIt == msgIt->second.cmdDefs.end() || cmdIt->second.isEdit == false)
	{
		retMsgData.Init("", 0, 0, 0, 0);
		retMsgData.ToBuffer();
		CServer::Instance.Send(retMsgData.buffer, retMsgData.bufLen);
	}

	//组织数据
	tm ptm{};
	CString tmp, data, cmd, dataLen, time;
	if ((cmdIt != msgIt->second.cmdDefs.end() && cmdIt->second.isEdit)
		|| (cmdIt == msgIt->second.cmdDefs.end() || cmdIt->second.isShow))
	{
		if (msgIt->second.handleMethod == "数据包")
		{
			for (int i = 0; i < msgData.dataLen; i++)
			{
				tmp.Format("%.2X ", (UCHAR)msgData.data[i]);
				data += tmp;
			}
		}
		else if (msgIt->second.handleMethod == "ASCII")
		{
			data.Format("%s", msgData.data);
		}
		else if (msgIt->second.handleMethod == "UTF-16")
		{
			CStringW str;
			str.Format(L"%s", (wchar_t*)msgData.data);
			data = str;
		}
		gmtime_s(&ptm, &msgData.time);
		if (cmdIt == msgIt->second.cmdDefs.end())
			cmd.Format("%d", msgData.cmd);
		else cmd.Format("%s(%d)", cmdIt->second.cmdName.c_str(), cmdIt->first);
		dataLen.Format("%d", msgData.dataLen);
		time.Format("%d:%d:%d", ptm.tm_hour + 8, ptm.tm_min, ptm.tm_sec);
	}

	//编辑消息
	if (cmdIt != msgIt->second.cmdDefs.end() && cmdIt->second.isEdit)
	{
		tmp.Format("消息: %s   命令: %s   长度: %s   时间: %s",
			msgData.msg, cmd.GetBuffer(), dataLen.GetBuffer(), time.GetBuffer());
		editDataDlg.title = tmp;
		editDataDlg.data = data;
		editDataDlg.isCancelEdit = false;
		if (editDataDlg.DoModal() == IDOK)
		{
			if (editDataDlg.isCancelEdit == true)
				cmdIt->second.isEdit = false;
			data = editDataDlg.data;
			CString data(editDataDlg.data);
			if (msgIt->second.handleMethod == "数据包") //数据
			{
				data.Remove(' ');
				int len = data.GetLength() / 2;
				unsigned char* buffer = new unsigned char[len] {};
				for (int i = 0; i < len; i++)
				{
					buffer[i] = strtoul(data.Mid(i * 2, 2), NULL, 16);
				}
				retMsgData.Init("", 1, (const char*)buffer, len, 0);
				retMsgData.ToBuffer();
				CServer::Instance.Send(retMsgData.buffer, retMsgData.bufLen);
				delete[] buffer;
			}
			else if (msgIt->second.handleMethod == "ASCII")
			{
				retMsgData.Init("", 1, data.GetBuffer(), data.GetLength() + 1, 0);
				retMsgData.ToBuffer();
				CServer::Instance.Send(retMsgData.buffer, retMsgData.bufLen);
			}
			else if (msgIt->second.handleMethod == "UTF-16")
			{
				CStringW wstr(data);
				retMsgData.Init("", 1, (char*)wstr.GetBuffer(), (wstr.GetLength() + 1) * 2, 0);
				retMsgData.ToBuffer();
				CServer::Instance.Send(retMsgData.buffer, retMsgData.bufLen);
			}
		}
		else
		{
			retMsgData.Init("", 0, 0, 0, 0);
			retMsgData.ToBuffer();
			CServer::Instance.Send(retMsgData.buffer, retMsgData.bufLen);
		}
	}

	//显示
	if (cmdIt == msgIt->second.cmdDefs.end()
		|| cmdIt->second.isShow)
	{
		msgList.InsertItem(0, msgData.msg);
		if (cmdIt != msgIt->second.cmdDefs.end()) //设置颜色
			msgList.SetItemData(0, (DWORD_PTR)&cmdIt->second);
		msgList.SetItemText(0, 1, cmd);
		msgList.SetItemText(0, 2, data);
		msgList.SetItemText(0, 3, dataLen);
		msgList.SetItemText(0, 4, time);
	}

	//输出消息日志
	config->Log(&msgData);
	return 0;
}


//成员函数实现

void CDataAnalyserDlg::OnRecvMsg(char* buf, unsigned len, CDataAnalyserDlg* thiz)//数据有效期只限于当前函数
{
	char* data = new char[len];
	memcpy(data, buf, len);    //保存数据
	thiz->PostMessageA(WM_RECVMSG, (WPARAM)data, (LPARAM)len);//异步发送，让接收和处理同时进行
}

void CDataAnalyserDlg::AdjustListClumn()
{
	CRect rect;
	msgList.GetClientRect(rect);
	int width = 0;
	width += msgList.GetColumnWidth(0);
	width += msgList.GetColumnWidth(1);
	width += msgList.GetColumnWidth(3);
	width += msgList.GetColumnWidth(4);
	width = rect.Width() - width;
	if (width < 0)width = 0;
	msgList.SetColumnWidth(2, width);
}
