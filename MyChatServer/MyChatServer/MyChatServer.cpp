// MyChatServer.cpp: 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include "MyChatServer.h"
#include "twapi_w01.h"
#include "twapi_w01_gdi.h"
#include "twapi_w01_util.h"
#include "twapi_w01_socket.h"
#include "twapi_w01_lib_info.h"

#define NM_CHAT_DATA           1      // 채팅 메시지
#define NM_SEND_SCREEN_IMAGE   2      // 화면 전송 메시지

#define PORT 3600
#define IP L"192.168.0.224"

class MyServer : public TW_ServerSocket
{
private:
	// ListBox의 주소를 저장할 변수
	TW_ListBoxWnd* mp_chat_list;

public:
	// 프로토콜 헤더의 시작 바이트를 0x27값으로 하고 최대 사용자의 수를 50명으로 정한다.
	// 그리고 사용자 관리용 클래스는 TW_UserData를 사용한다.
	MyServer() : TW_ServerSocket(0x27, 50, new TW_UserData) { }
	// 외부에서 ListBox의 주소를 설정할 함수
	void SetChatList(TW_ListBoxWnd* ap_chat_list)
	{
		mp_chat_list = ap_chat_list;
	}
	// 클라이언트가 서버에 접속될때 호출되는 함수
	virtual void AddWorkForAccept(TW_UserData* ap_user)
	{
		TW_String str;
		// 접속한 클라이언트의 IP를 ListBox에 추가한다.
		str.Format(L"새로운 사용자 접속 : %s", ap_user->GetIP());
		mp_chat_list->InsertString(-1, str);
	}
	// 클라이언트가 접속을 해제할때 호출되는 함수
	virtual void AddWorkForCloseUser(TW_UserData* ap_user, INT32 a_error_code)
	{
		TW_String str;
		// 접속을 해제한 클라이언트의 IP를 ListBox에 추가한다.
		str.Format(L"사용자 해제 : %s", ap_user->GetIP());
		mp_chat_list->InsertString(-1, str);
	}
	// 클라이언트에서 보낸 데이터가 수신되었을때 호출되는 함수
	virtual INT32 ProcessRecvData(SOCKET ah_socket, UINT8 a_msg_id, UINT8* ap_recv_data, UINT32 a_body_size)
	{
		// 대용량 데이터가 전송 또는 수신될 때, 필요한 기본 코드를 수행
		TW_ServerSocket::ProcessRecvData(ah_socket, a_msg_id, ap_recv_data, a_body_size);

		// 수신된 데이터가 채팅 데이터인 경우
		if (a_msg_id == NM_CHAT_DATA) {
			// 수신된 데이터에 채팅 정보가 유니코드 형태의 문자열로 저장되어 있기 때문에
			// 유니코드 문자열 형태로 캐스팅하여 사용한다.
			wchar_t* p_chat_data = (wchar_t*)ap_recv_data;
			TW_String str;
			// ah_socket 핸들을 사용하는 클라이언트를 목록에서 찾아서 해당 클라이언트의 주소를
			// 채팅글 앞에 추가한다. 예) "192.168.0.50 : 안녕하세요~"의 형태로 구성한다.
			str.Format(L"%s : %s", FindUserData(ah_socket)->GetIP(), p_chat_data);
			// 구성된 문자열을 리스트 박스에 추가한다.
			mp_chat_list->InsertString(-1, str);

			// 서버에 접속 가능한 사용자 수를 얻는다.
			UINT16 user_count = GetMaxUserCount();
			// 전송할 데이터 형식이 UINT8 *이기 때문에 유니코드 형식의 문자열을 UINT8 * 형식으로 변경한다.
			UINT8* p_send_data = (UINT8*)(const wchar_t*)str;
			// 전송할 크기를 구한다. 문자열을 전송할 때는 NULL 문자를 포함해서 전송하기 때문에 
			// 문자열 길이에 1을 더하고 유니코드에서 문자 1개는 2바이트를 차지하기 때문에 
			// 문자열 길이에 2를 곱한다.
			UINT32 send_data_size = (str.GetLength() + 1) * 2;
			// 접속된 모든 사용자에게 채팅 정보를 전송한다.
			for (UINT16 i = 0; i < user_count; i++) {
				// 현재 사용자가 접속 상태인지 확인한다.
				if (GetUserData(i)->GetHandle() != INVALID_SOCKET) {
					// 접속 상태라면 채팅 정보를 전송한다.
					SendFrameData(GetUserData(i)->GetHandle(), NM_CHAT_DATA, p_send_data, send_data_size);
				}
			}
		}
		else if (a_msg_id == NM_SEND_SCREEN_IMAGE) {
			// 클라이언트가 자신의 화면을 전송하기 위해 화면을 저장한 PNG 파일의
			// 크기를 전송한 경우, PNG 파일을 수신하기 위한 메모리를 생성한다.
			FindUserData(ah_socket)->GetRecvMan()->MemoryAlloc(*(int*)ap_recv_data);
			// TW_ExchangeManager 객체를 사용하여 클라이언트가 보내준 
			// 대용량 파일의 수신을 시작한다.
			SendFrameData(ah_socket, 251, NULL, 0);
		}
		return 1;
	}
};

class MyWnd : public TW_Wnd
{
private:
	TW_ListBoxWnd m_chat_list;   // 채팅 목록을 관리할 ListBox 객체
	MyServer m_server;           // 서버 소켓용 객체
	ID2D1Bitmap* mp_image;       // 클라이언트 화면을 보여줄 비트맵 객체
	D2D1_RECT_F m_image_rect;    // 클라이언트 화면을 보여줄 좌표

public:
	MyWnd() // 객체 생성자
	{
		// 수신한 클라이언트 화면은 ListBox 아래쪽에 출력한다.
		TW_SetFRect(m_image_rect, 3, 218, 601, 556);
		mp_image = NULL;
	}
	// ListBox 항목의 높이를 조정하는 함수
	virtual INT32 OnMeasureItem(UINT32 a_is_ctrl, MEASUREITEMSTRUCT* ap_mis)
	{
		if (a_is_ctrl && ap_mis->CtlID == 25001) {
			ap_mis->itemHeight = 19;  // 높이를 12에서 19으로 변경한다.
			return 1;
		}
		return 0;
	}
	// ListBox의 각 항목을 사용자가 정의한대로 출력하는 함수
	virtual INT32 OnDrawItem(INT32 a_ctrl_id, DRAWITEMSTRUCT* ap_dis)
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
		if (m_chat_list.GetHandle() == ah_wnd)
			return m_chat_list.OnCtrlColor(ah_dc, ah_wnd);	// 기본 테마 색상으로 변경
		return 0;
	}

	// 수신되 클라이언트 화면을 my_screen.png 파일로 저장하고 
	// 해당 파일에서 이미지를 읽어 ID2D1Bitmap 객체로 만든다.
	void SaveScreenImage(TW_RecvManager* ap_recv)
	{
		TW_IRelease(&mp_image);  // 기존 이미지를 제거한다.

		// 전송받은 PNG 이미지 데이터를 사용하여 ID2D1Bitmap 객체로 만든다.
		mp_image = TWAPI_LoadImageFromMemory(mp_target, ap_recv->GetData(), ap_recv->GetTotalSize());
		// 화면을 갱신하여 읽은 이미지를 화면에 출력한다.
		Invalidate();
		// 수신에 사용한 메모리를 제거한다.
		ap_recv->DeleteData();
	}

	virtual LRESULT UserMessageProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (uMsg == 25001) { // 새로운 클라이언트가 접속!!
			m_server.ProcessToAccept(wParam, lParam);  // FD_ACCEPT
			return 1;
		}
		else if (uMsg == 25002) { // 클라이언트가 데이터를 전송하거나 접속을 해제!!
			m_server.ProcessClientEvent(wParam, lParam); // FD_READ, FD_CLOSE
			return 1;
		}
		else if (uMsg == LM_RECV_COMPLETED) {
			// 클라이언트 화면의 수신이 완료됨!
			SaveScreenImage(((TW_UserData*)wParam)->GetRecvMan());
		}
		return 0;
	}

	virtual int OnCreate(CREATESTRUCT* ap_create_info)
	{
		TW_Wnd::OnCreate(ap_create_info);

		TWAPI_CreateWIC();

		// 시작점이 (3, 3)이고 끝점이(601, 215)이고 ID가 25001인 ListBox를 생성한다.
		m_chat_list.CreateEx(mh_wnd, 3, 3, 601, 215, 25001, LBS_OWNERDRAWFIXED | LBS_HASSTRINGS);

		// m_chat_list의 주소를 MyServer 클래스에서 사용할 수 있도록 넘겨준다.
		m_server.SetChatList(&m_chat_list);
		// 서버 서비스를 '127.0.0.1'에서 19999 포트로 시작합니다.
		// '127.0.0.1' 주소는 자신의 컴퓨터에서 실행된 클라이언트만 접속이 가능하기 때문에 
		// 외부에서 접속이 가능하게 하려면 ipconfig 명령으로 자신의 IP 주소를 확인해서 
		// 그 주소를 여기에 적으면 됩니다.
		m_server.StartServer(IP, PORT, mh_wnd);
		return 0;
	}

	virtual void OnPaint()
	{
		// 부모 Window에 WM_PAINT 메시지가 발생하면 
		// 자식 Window에도 WM_PAINT 메시지를 전달해야 한다.
		m_chat_list.Invalidate();

		mp_target->BeginDraw();
		// 파란색으로 클라이언트 영역을 채운다.
		mp_target->Clear(ColorF(0.1875f, 0.2343f, 0.3203f));
		// 수신된 클라이언트 화면이 있다면 화면에 출력합니다.
		if (mp_image != NULL) mp_target->DrawBitmap(mp_image, m_image_rect);

		mp_target->EndDraw();
	}
};

class MyApp : public TW_WinApp
{
public:
	MyApp(HINSTANCE ah_instance, const wchar_t* ap_class_name)
		: TW_WinApp(ah_instance, ap_class_name) { }

	virtual void InitInstance()
	{
		mp_wnd = new MyWnd;
		mp_wnd->Create(L"Chat Server - Tipssoft.com", 50, 50, 620, 600,
			WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU);
		mp_wnd->ShowWindow();
		mp_wnd->UpdateWindow();
	}
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	MyApp tips_app(hInstance, L"MyChatServer");
	return tips_app.NormalProcess();
}
