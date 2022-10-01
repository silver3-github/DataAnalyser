// ConfigDlg.cpp: 实现文件
//

#include "pch.h"
#include "DataAnalyser.h"
#include "ConfigDlg.h"
#include "afxdialogex.h"
#include "CConfig.h"


// CConfigDlg 对话框

IMPLEMENT_DYNAMIC(CConfigDlg, CDialogEx)

CConfigDlg::CConfigDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CONFIG_DIALOG, pParent)
	, configFileName(_T(""))
	, msgCurSel(-1)
	, cmdCurSel(-1)
{

}

CConfigDlg::~CConfigDlg()
{
}

void CConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_CONFIG_FILE, configFileName);
	DDX_Control(pDX, IDC_MSG_DEFINE, lstMsgDefine);
	DDX_Control(pDX, IDC_CMD_DEFINE, lstCmdDefine);
}


BEGIN_MESSAGE_MAP(CConfigDlg, CDialogEx)
	ON_BN_CLICKED(IDC_SAVE, &CConfigDlg::OnBnClickedSave)
	ON_NOTIFY(NM_RCLICK, IDC_MSG_DEFINE, &CConfigDlg::OnNMRClickMsgDefine)
	ON_NOTIFY(NM_RCLICK, IDC_CMD_DEFINE, &CConfigDlg::OnNMRClickCmdDefine)
	ON_COMMAND(ID_32771, &CConfigDlg::OnAddMsgDef)
	ON_COMMAND(ID_32773, &CConfigDlg::OnAddMsgDef)
	ON_COMMAND(ID_32772, &CConfigDlg::OnAddCmdDef)
	ON_COMMAND(ID_32775, &CConfigDlg::OnAddCmdDef)
	ON_NOTIFY(NM_DBLCLK, IDC_MSG_DEFINE, &CConfigDlg::OnNMDblclkMsgDefine)
	ON_NOTIFY(NM_DBLCLK, IDC_CMD_DEFINE, &CConfigDlg::OnNMDblclkCmdDefine)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_MSG_DEFINE, &CConfigDlg::OnLvnItemchangedMsgDefine)
	ON_COMMAND(ID_32776, &CConfigDlg::OnDelCmdDef)
	ON_COMMAND(ID_32774, &CConfigDlg::OnDelMsgDef)
	ON_BN_CLICKED(IDC_BUILD_DEFINE, &CConfigDlg::OnBnClickedBuildDefine)
END_MESSAGE_MAP()


// CConfigDlg 消息处理程序


BOOL CConfigDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//设置消息定义列表样式
	DWORD dwStyle = lstMsgDefine.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;//整行选中
	dwStyle |= LVS_EX_GRIDLINES;//网格线
	lstMsgDefine.SetExtendedStyle(dwStyle);
	lstCmdDefine.InsertColumn(0, "保留");
	lstMsgDefine.InsertColumn(1, "消息名", LVCFMT_CENTER, 288);
	lstMsgDefine.InsertColumn(2, "处理方式", LVCFMT_CENTER, 200);
	lstMsgDefine.InsertColumn(3, "输出到文件", LVCFMT_CENTER, 200);
	lstCmdDefine.DeleteColumn(0);
	//更新消息定义数据
	CConfig* config = &CConfig::Instance;
	for (auto it : config->msgDefs) {
		int count = lstMsgDefine.GetItemCount();
		lstMsgDefine.InsertItem(count, it.second.msgName.c_str());
		lstMsgDefine.SetItemText(count, 1, it.second.handleMethod.c_str());
		lstMsgDefine.SetItemText(count, 2, it.second.isInput ? "是" : "否");
	}
	msgCurSel = -1;

	//设置命令定义列表样式
	dwStyle = lstCmdDefine.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;//整行选中
	dwStyle |= LVS_EX_GRIDLINES;//网格线
	lstCmdDefine.SetExtendedStyle(dwStyle);
	lstCmdDefine.InsertColumn(0, "保留");
	lstCmdDefine.InsertColumn(1, "命令名", LVCFMT_CENTER, 180);
	lstCmdDefine.InsertColumn(2, "命令编号", LVCFMT_CENTER, 108);
	lstCmdDefine.InsertColumn(3, "显示", LVCFMT_CENTER, 80);
	lstCmdDefine.InsertColumn(4, "编辑", LVCFMT_CENTER, 80);
	lstCmdDefine.InsertColumn(5, "输出", LVCFMT_CENTER, 80);
	lstCmdDefine.InsertColumn(6, "单独输出", LVCFMT_CENTER, 80);
	lstCmdDefine.InsertColumn(7, "文本颜色", LVCFMT_CENTER, 80);
	lstCmdDefine.InsertColumn(8, "背景颜色", LVCFMT_CENTER, 80);
	lstCmdDefine.DeleteColumn(0);

	return TRUE;
}

void CConfigDlg::OnBnClickedSave()
{
	UpdateData(true);
	if (configFileName.GetLength() == 0) 
	{
		MessageBoxA("请输入配置文件名", "保存配置数据");
		return;
	}
	CDialogEx::OnOK();
}

void CConfigDlg::OnBnClickedBuildDefine()
{
	//生成定义
	CConfig* config = &CConfig::Instance;
	if (config->configFile.size()) 
	{
		config->BuiltDefine();
		//打开生成目录，方便用户查看
		MessageBoxA("生成定义文件成功！", "生成定义文件");
		std::string folder = config->defineFPath + config->configFile + '\\';
		ShellExecuteA(NULL, NULL, folder.c_str(), NULL, NULL, SW_SHOWNORMAL);
	}
	else 
	{
		MessageBoxA("请先加载配置文件", "生成定义文件");
	}
}


// 消息定义列表 右键单击
void CConfigDlg::OnNMRClickMsgDefine(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE p = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	msgCurSel = p->iItem;
	CMenu Menu;
	Menu.LoadMenu(IDR_MENU1);
	CMenu* Popup = Menu.GetSubMenu(p->iItem == -1 ? 0 : 2);
	if (Popup) {
		CPoint pos(GetMessagePos());
		Popup->TrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN, pos.x, pos.y, this);
	}
}

void CConfigDlg::OnAddMsgDef()
{
	setMsgDefDlg.title = "添加消息定义";
	setMsgDefDlg.msgName = "";
	setMsgDefDlg.handleMethod = "数据包";
	setMsgDefDlg.isInput = false;
	if (setMsgDefDlg.DoModal() == IDOK)
	{
		//判断消息定义是否存在
		CConfig* config = &CConfig::Instance;
		if (config->IsExistMsgDef(setMsgDefDlg.msgName))
		{
			MessageBoxA("该消息定义已存在", setMsgDefDlg.title);
			return;
		}

		//添加 消息定义数据
		config->AddMsgDef(
			setMsgDefDlg.msgName,
			setMsgDefDlg.handleMethod,
			setMsgDefDlg.isInput);

		//添加 消息定义到列表
		int count = lstMsgDefine.GetItemCount();
		lstMsgDefine.InsertItem(count, setMsgDefDlg.msgName);
		lstMsgDefine.SetItemText(count, 1, setMsgDefDlg.handleMethod);
		lstMsgDefine.SetItemText(count, 2, setMsgDefDlg.isInput ? "是" : "否");
	}
}


// 命令定义列表 右键单击
void CConfigDlg::OnNMRClickCmdDefine(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE p = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	cmdCurSel = p->iItem;
	CMenu Menu;
	Menu.LoadMenu(IDR_MENU1);
	CMenu* Popup = Menu.GetSubMenu(p->iItem == -1 ? 1 : 3);
	if (Popup) {
		CPoint pos(GetMessagePos());
		Popup->TrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN, pos.x, pos.y, this);
	}
}

void CConfigDlg::OnAddCmdDef()
{
	if (msgCurSel == -1)//未选中消息定义
	{
		MessageBoxA("请先选择消息定义", "添加命令定义");
		return;
	}

	setCmdDefDlg.title = "添加命令定义";
	setCmdDefDlg.cmdName = "Undefined";
	setCmdDefDlg.cmdId = 0;
	setCmdDefDlg.isShow = true;
	setCmdDefDlg.isEdit = false;
	setCmdDefDlg.isInput = false;
	setCmdDefDlg.isAloneInput = false;
	setCmdDefDlg.textColor = 0;
	setCmdDefDlg.bgColor = 0xFFFFFF;
	if (setCmdDefDlg.DoModal() == IDOK)
	{
		//判断命令定义是否存在
		CConfig* config = &CConfig::Instance;
		CString msgName = lstMsgDefine.GetItemText(msgCurSel, 0);
		if (config->IsExistCmdDef(msgName, setCmdDefDlg.cmdId))
		{
			MessageBoxA("该命令定义已存在", setCmdDefDlg.title);
			return;
		}

		//添加 命令定义数据
		config->AddCmdDef(
			msgName,
			setCmdDefDlg.cmdName,
			setCmdDefDlg.cmdId,
			setCmdDefDlg.isShow,
			setCmdDefDlg.isEdit,
			setCmdDefDlg.isInput,
			setCmdDefDlg.isAloneInput,
			setCmdDefDlg.textColor,
			setCmdDefDlg.bgColor);

		//添加 命令定义到列表
		CString value;
		int count = lstCmdDefine.GetItemCount();
		lstCmdDefine.InsertItem(count, setCmdDefDlg.cmdName);
		value.Format("%u", setCmdDefDlg.cmdId);
		lstCmdDefine.SetItemText(count, 1, value);
		lstCmdDefine.SetItemText(count, 2, setCmdDefDlg.isShow ? "是" : "否");
		lstCmdDefine.SetItemText(count, 3, setCmdDefDlg.isEdit ? "是" : "否");
		lstCmdDefine.SetItemText(count, 4, setCmdDefDlg.isInput ? "是" : "否");
		lstCmdDefine.SetItemText(count, 5, setCmdDefDlg.isAloneInput ? "是" : "否");
		value.Format("%X", setCmdDefDlg.textColor);
		lstCmdDefine.SetItemText(count, 6, value);
		value.Format("%X", setCmdDefDlg.bgColor);
		lstCmdDefine.SetItemText(count, 7, value);
	}
}


// 消息定义列表 左键双击（修改定义）
void CConfigDlg::OnNMDblclkMsgDefine(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE p = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	int item = p->iItem;
	if (item == -1)return;//未选择项退出

	setMsgDefDlg.title = "修改消息定义";
	setMsgDefDlg.msgName = lstMsgDefine.GetItemText(item, 0);
	setMsgDefDlg.handleMethod = lstMsgDefine.GetItemText(item, 1);
	setMsgDefDlg.isInput = lstMsgDefine.GetItemText(item, 2) == "是" ? true : false;
	if (setMsgDefDlg.DoModal() == IDOK)
	{
		//修改 消息定义数据
		CConfig* config = &CConfig::Instance;
		if (!config->SetMsgDef(
			lstMsgDefine.GetItemText(item, 0),
			setMsgDefDlg.msgName,
			setMsgDefDlg.handleMethod,
			setMsgDefDlg.isInput))
		{
			MessageBoxA("该消息定义已存在", setMsgDefDlg.title);
			return;
		}

		//修改 列表中的消息定义
		lstMsgDefine.SetItemText(item, 0, setMsgDefDlg.msgName);
		lstMsgDefine.SetItemText(item, 1, setMsgDefDlg.handleMethod);
		lstMsgDefine.SetItemText(item, 2, setMsgDefDlg.isInput ? "是" : "否");
	}
}


// 命令定义列表 左键双击（修改定义）
void CConfigDlg::OnNMDblclkCmdDefine(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE p = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	int item = p->iItem;
	if (item == -1)return;//未选择项退出

	setCmdDefDlg.title = "修改命令定义";
	setCmdDefDlg.cmdName = lstCmdDefine.GetItemText(item, 0);
	setCmdDefDlg.cmdId = strtoul(lstCmdDefine.GetItemText(item, 1), NULL, 10);
	setCmdDefDlg.isShow = lstCmdDefine.GetItemText(item, 2) == "是" ? true : false;
	setCmdDefDlg.isEdit = lstCmdDefine.GetItemText(item, 3) == "是" ? true : false;
	setCmdDefDlg.isInput = lstCmdDefine.GetItemText(item, 4) == "是" ? true : false;
	setCmdDefDlg.isAloneInput = lstCmdDefine.GetItemText(item, 5) == "是" ? true : false;
	setCmdDefDlg.textColor= strtoul(lstCmdDefine.GetItemText(item, 6), NULL, 16);
	setCmdDefDlg.bgColor = strtoul(lstCmdDefine.GetItemText(item, 7), NULL, 16);
	if (setCmdDefDlg.DoModal() == IDOK)
	{
		//修改 命令定义数据
		CConfig* config = &CConfig::Instance;
		CString msgName = lstMsgDefine.GetItemText(msgCurSel, 0);
		unsigned cmdId = strtoul(lstCmdDefine.GetItemText(item, 1), NULL, 10);
		if (!config->SetCmdDef(
			msgName,
			cmdId,
			setCmdDefDlg.cmdName,
			setCmdDefDlg.cmdId,
			setCmdDefDlg.isShow,
			setCmdDefDlg.isEdit,
			setCmdDefDlg.isInput,
			setCmdDefDlg.isAloneInput,
			setCmdDefDlg.textColor,
			setCmdDefDlg.bgColor))
		{
			MessageBoxA("该命令定义已存在", setMsgDefDlg.title);
			return;
		}

		//修改 列表中的命令定义
		CString value;
		lstCmdDefine.SetItemText(item, 0, setCmdDefDlg.cmdName);
		value.Format("%u", setCmdDefDlg.cmdId);
		lstCmdDefine.SetItemText(item, 1, value);
		lstCmdDefine.SetItemText(item, 2, setCmdDefDlg.isShow ? "是" : "否");
		lstCmdDefine.SetItemText(item, 3, setCmdDefDlg.isEdit ? "是" : "否");
		lstCmdDefine.SetItemText(item, 4, setCmdDefDlg.isInput ? "是" : "否");
		lstCmdDefine.SetItemText(item, 5, setCmdDefDlg.isAloneInput ? "是" : "否");
		value.Format("%X", setCmdDefDlg.textColor);
		lstCmdDefine.SetItemText(item, 6, value);
		value.Format("%X", setCmdDefDlg.bgColor);
		lstCmdDefine.SetItemText(item, 7, value);
	}
}

//删除消息定义
void CConfigDlg::OnDelMsgDef()
{
	//删除数据
	CConfig* config = &CConfig::Instance;
	CString msgName = lstMsgDefine.GetItemText(msgCurSel, 0);
	config->DeleteMsgDef(msgName);

	//删除列表项
	lstMsgDefine.DeleteItem(msgCurSel);
}

//删除命令定义
void CConfigDlg::OnDelCmdDef()
{
	//删除数据
	CConfig* config = &CConfig::Instance;
	CString msgName = lstMsgDefine.GetItemText(msgCurSel, 0);
	unsigned cmdId = strtoul(lstCmdDefine.GetItemText(cmdCurSel, 1), NULL, 10);
	config->DeleteCmdDef(msgName, cmdId);

	//删除列表项
	lstCmdDefine.DeleteItem(cmdCurSel);
}


//切换选中的消息定义，显示它的命令定义
void CConfigDlg::OnLvnItemchangedMsgDefine(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW p = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;

	if (p->uChanged == LVIF_STATE)
	{
		if (p->uOldState == LVNI_SELECTED) //取消选中消息项
		{
			msgCurSel = -1;
			lstCmdDefine.DeleteAllItems();
			return;
		}

		if (p->uNewState == (LVNI_FOCUSED | LVNI_SELECTED)) //选中消息项
		{
			msgCurSel = p->iItem;
			//显示命令定义数据
			std::string msgName = lstMsgDefine.GetItemText(msgCurSel, 0);
			CConfig* config = &CConfig::Instance;
			for (auto it : config->msgDefs[msgName].cmdDefs) {
				int count = lstCmdDefine.GetItemCount();
				CString value;
				lstCmdDefine.InsertItem(count, it.second.cmdName.c_str());
				value.Format("%u", it.second.cmdId);
				lstCmdDefine.SetItemText(count, 1, value);
				lstCmdDefine.SetItemText(count, 2, it.second.isShow ? "是" : "否");
				lstCmdDefine.SetItemText(count, 3, it.second.isEdit ? "是" : "否");
				lstCmdDefine.SetItemText(count, 4, it.second.isInput ? "是" : "否");
				lstCmdDefine.SetItemText(count, 5, it.second.isAloneInput ? "是" : "否");
				value.Format("%X", setCmdDefDlg.textColor);
				lstCmdDefine.SetItemText(count, 6, value);
				value.Format("%X", setCmdDefDlg.bgColor);
				lstCmdDefine.SetItemText(count, 7, value);
			}
		}
	}
}
