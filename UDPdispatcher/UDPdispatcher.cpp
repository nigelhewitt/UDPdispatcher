//=================================================================================================
//
// UDPdispatcher.cpp : I out most of the code for the displays and data management in here.
//
//=================================================================================================

#include "framework.h"

HINSTANCE hInstance;
HWND hMain{}, hTraffic{};
_dispatcher* dispatcher{};

//=================================================================================================
// Message handler for Edit Client dialog
//=================================================================================================

INT_PTR CALLBACK EditClient(HWND hDlg, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	static _config::client_details* cd;	// passed in client details to edit

	switch(uMessage){
	case WM_INITDIALOG:
		cd = (_config::client_details*) lParam;
		SetDlgItemText(hDlg, IDC_URL, cd->url);
		SetDlgItemInt(hDlg, IDC_PORT, cd->port, FALSE);
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		switch LOWORD(wParam) {
		case IDC_SAVE:
		{
			char temp[100];
			GetDlgItemText(hDlg, IDC_URL, temp, sizeof temp);
			if(cd->url) delete cd->url;
			cd->url = _strdup(temp);
			cd->port = GetDlgItemInt(hDlg, IDC_PORT, nullptr, FALSE);
			EndDialog(hDlg, TRUE);
		}
			return (INT_PTR)TRUE;

		case IDCLOSE:
			EndDialog(hDlg, FALSE);
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
//=================================================================================================
// Message handler for 'transactions' dialog which just shows the recent blocks of text
//=================================================================================================

char szTraffic[5000]{};	// nominal amount of text
int nTraffic{};
bool bAutoScroll{true}, bFreeze{false};

void AddTraffic(const char* text)
{
	if(hTraffic && !bFreeze){
		int i = (int)strlen(text)+2;				// how much text to add
		if(i>sizeof szTraffic) return;				// not even going to consider it, that's probably an unterminated string

		// now we want to chop complete lines from the start of szTraffic until the addition fits
		int k=0;														// end of trim area
		while(szTraffic[k] && nTraffic - k + i + 1 > sizeof szTraffic){	// while it is still too big
			while(szTraffic[k] && szTraffic[k]!='\n')					// move k to next \n
				++k;
			if(szTraffic[k]) ++k;										// step over the \n
		}
		if(k){
			if(!szTraffic[k]) return;									// better a loss of data than a crash
			memmove(szTraffic, szTraffic+k, nTraffic-k+1);
			nTraffic -= k;
		}
		strcat_s(szTraffic, sizeof szTraffic, text);
		strcat_s(szTraffic, sizeof szTraffic, "\r\n");
		nTraffic += i;
		SetDlgItemText(hTraffic, IDC_TRAFFIC, szTraffic);
		if(bAutoScroll)
			SendMessage(GetDlgItem(hTraffic, IDC_TRAFFIC), EM_LINESCROLL, 0, 100);
	}
}
void clearTraffic()
{
	szTraffic[0] = 0;
	nTraffic = 0;
	SetDlgItemText(hTraffic, IDC_TRAFFIC, szTraffic);
}


INT_PTR CALLBACK TrafficDialog(HWND hDlg, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	switch(uMessage){
	case WM_INITDIALOG:
		SetDlgItemText(hDlg, IDC_TRAFFIC, szTraffic);
		CheckDlgButton(hDlg, IDC_AUTOSCROLL, bAutoScroll ? BST_CHECKED : BST_UNCHECKED);
		CheckDlgButton(hDlg, IDC_FREEZE, bFreeze ? BST_CHECKED : BST_UNCHECKED);
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		switch LOWORD(wParam) {
		case IDC_TRAFFIC:
			bAutoScroll = IsDlgButtonChecked(hDlg, IDC_AUTOSCROLL)==BST_CHECKED;
			return (INT_PTR)TRUE;

		case IDC_FREEZE:
			bFreeze = IsDlgButtonChecked(hDlg, IDC_FREEZE)==BST_CHECKED;
			return (INT_PTR)TRUE;

		case IDCANCEL:							// clear
			clearTraffic();
			return (INT_PTR)TRUE;

		case IDOK:								// close
			CheckDlgButton(hMain, IDC_TRAFFIC, BST_UNCHECKED);
			DestroyWindow(hDlg);
			return (INT_PTR)TRUE;
		}
		break;

	case WM_SIZE:
		int w, h;
		// positioning on my main screen : 4 to the mm
		w = LOWORD(lParam);
		h = HIWORD(lParam);
		MoveWindow(GetDlgItem(hDlg, IDC_TRAFFIC), 12, 34, w-24, h-73, TRUE);
		MoveWindow(GetDlgItem(hDlg, IDCANCEL), w-92, 0, 80, 24, TRUE);
		MoveWindow(GetDlgItem(hDlg, IDOK), w-92, h-34, 80, 24, TRUE);
		break;

	case WM_DESTROY:
		hTraffic = nullptr;
		break;
	}
	return (INT_PTR)FALSE;
}
//=================================================================================================
// Message handler for main dialog
//=================================================================================================

INT_PTR CALLBACK MainWindow(HWND hDlg, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	switch(uMessage){
	case WM_INITDIALOG:
		// get the details in config
		readConfiguration();
		{	// set to the previous position
			int x, y;
			char temp[30];
			readINI("SetUp", "Position", "0 -1", temp, sizeof temp);
			sscanf_s(temp, "%d %d", &x, &y);
			if(y>=0){
				RECT rect;
				GetWindowRect(hDlg, &rect);
				SetWindowPos(hDlg, HWND_TOP, x, y, rect.right-rect.left, rect.bottom-rect.top, 0);
			}
		}
		// display the server stuff
		SetDlgItemInt(hDlg, IDC_SERVERPORT, config.server.port, FALSE);

		// display the client list
		int t;
		t = 100;		// set one tab (well one every 100 dialog units so effectively one)
		SendDlgItemMessage(hDlg, IDC_CLIENTLIST, LB_SETTABSTOPS, 1, (LPARAM)&t);		// for all servers
		for(_config::client_details s : config.clients){
			char temp[100];
			sprintf_s(temp, sizeof temp, "%s\t%u", s.url, s.port);
			SendDlgItemMessage(hDlg, IDC_CLIENTLIST, LB_ADDSTRING, 0, (LPARAM)temp);
		}
		// the actual dispatcher will be started on the first tick of the timer so everything is in-place
		SetTimer(hDlg, 100, 500, nullptr);
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		switch LOWORD(wParam){
		case IDC_SAVE:
			delete dispatcher;
			dispatcher = nullptr;
			writeConfiguration();
			dispatcher = new _dispatcher;
			AddTraffic("System restarted");
			return (INT_PTR)TRUE;

		case IDCLOSE:
			DestroyWindow(hDlg);
			return (INT_PTR)TRUE;

		case IDC_ADD:
			_config::client_details* sd; sd = new _config::client_details;		// create empty
			if(DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_EDITCLIENT), hDlg, EditClient, (LPARAM)sd)){
				config.clients.push_back(*sd);
				char temp[100];
				sprintf_s(temp, sizeof temp, "%s\t%u", sd->url, sd->port);
				SendDlgItemMessage(hDlg, IDC_CLIENTLIST, LB_ADDSTRING, 0, (LPARAM)temp);
			}
			return (INT_PTR)TRUE;

		case IDC_EDIT:
			int sel; sel = (int)SendDlgItemMessage(hDlg, IDC_CLIENTLIST, LB_GETCURSEL, 0, 0);
			if(sel!=LB_ERR){
				DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_EDITCLIENT), hDlg, EditClient, (LPARAM)&config.clients[sel]);
				char temp[100];
				sprintf_s(temp, sizeof temp, "%s\t%u", config.clients[sel].url, config.clients[sel].port);
				SendDlgItemMessage(hDlg, IDC_CLIENTLIST, LB_DELETESTRING, sel, 0);
				SendDlgItemMessage(hDlg, IDC_CLIENTLIST, LB_INSERTSTRING, sel, (LPARAM)temp);
			}
			return (INT_PTR)TRUE;

		case IDC_REMOVE:
			sel = (int)SendDlgItemMessage(hDlg, IDC_CLIENTLIST, LB_GETCURSEL, 0, 0);
			if(sel!=LB_ERR){
				config.clients.erase(config.clients.begin()+sel);
				SendDlgItemMessage(hDlg, IDC_CLIENTLIST, LB_DELETESTRING, sel, 0);
			}
			return (INT_PTR)TRUE;

		case IDC_TRAFFIC:
			bool t; t = IsDlgButtonChecked(hDlg, IDC_TRAFFIC)==BST_CHECKED;
			if(t && hTraffic==nullptr){
				hTraffic = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_TRAFFIC), hDlg, TrafficDialog);
				ShowWindow(hTraffic, SW_SHOW);
			}
			else if(!t && hTraffic)
				DestroyWindow(hTraffic);
			return (INT_PTR)TRUE;
		}
		break;

	case WM_TIMER:
		if(dispatcher==nullptr){		// first tick
			// start the system
			dispatcher = new _dispatcher;
			AddTraffic("System started");
		}
		else{
			int serverSent, serverReceived, clientSent, clientReceived, nActive;
			dispatcher->getCounts(serverSent, serverReceived, clientSent, clientReceived, nActive);
			char temp[50];
			sprintf_s(temp, sizeof temp, "%d/%d", serverSent, serverReceived);
			SetDlgItemText(hDlg, IDC_SERVERCOUNTS, temp);
			sprintf_s(temp, sizeof temp, "%d/%d", clientSent, clientReceived);
			SetDlgItemText(hDlg, IDC_CLIENTCOUNTS, temp);
			sprintf_s(temp, sizeof temp, "%d", nActive);
			SetDlgItemText(hDlg, IDC_NCLIENTS, temp);
		}
		return (INT_PTR)TRUE;

	case WM_SYSCOMMAND:
		switch(LOWORD(wParam)){
		case SC_CLOSE:
			DestroyWindow(hDlg);
			return (INT_PTR)TRUE;
		}
		break;

	case WM_DESTROY:
		if(hTraffic)
			DestroyWindow(hTraffic);
		if(!IsIconic(hDlg) && !IsZoomed(hDlg)){
			RECT rect;
			GetWindowRect(hDlg, &rect);
			char temp[40];
			sprintf_s(temp, sizeof temp, "%d %d", rect.left, rect.top);
			writeINI("SetUp", "Position", temp);
		}
        PostQuitMessage(0);
		break;
	}
	return (INT_PTR)FALSE;
}
//=================================================================================================
// WinMain where we start and end things
//=================================================================================================

int APIENTRY wWinMain(	_In_ HINSTANCE		hInstance,
						_In_opt_ HINSTANCE	hPrevInstance,
						_In_ LPWSTR			lpCmdLine,
						_In_ int			nCmdShow)
{
	::hInstance = hInstance;

	// initialise Windows Sockets
	WSADATA ws;
	if(WSAStartup(MAKEWORD(2, 2), &ws) != 0)
		return 99;

	// normally in a windows program that only used a dialog for its GUI I would just
	// use DialogBox() and let it sort out the message loop. However as I want to run
	// a child in modeless I need a more complex message loop so I can hand off the
	// child windows stuff. Hence the main window has to be modeless too so we get a
	// nice clear message loop.

	hMain = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_MAINWINDOW), nullptr, MainWindow);
	ShowWindow(hMain, SW_SHOW);

	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0)){
        if(!hTraffic || !IsDialogMessage(hTraffic, &msg)){
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

	WSACleanup();
	return 0;
}
