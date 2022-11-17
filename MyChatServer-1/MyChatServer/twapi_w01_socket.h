#ifndef _TWAPI_W01_SOCKET_H_
#define _TWAPI_W01_SOCKET_H_

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <WinSock2.h>

#define LM_SEND_COMPLETED      29001
#define LM_RECV_COMPLETED      29002


// 전송과 수신에서 공통적으로 작업할 내용을 구현한 클래스
class TW_ExchangeManager
{
protected:
	// 전송 또는 수신을 위해 할당된 메모리의 전체 크기와 현재 작업중인 크기
	UINT32 m_total_size, m_cur_size;
	// 전송 또는 수신을 위해서 할당된 메모리의 시작 주소
	UINT8 *mp_data;

public:
	TW_ExchangeManager();  // 객체 생성자
	~TW_ExchangeManager(); // 객체 파괴자

	// 전송 또는 수신에 사용할 메모리를 할당한다.
	// a_data_size에 할당할 크기를 명시하면 이 함수의 반환 값으로
	// 할당된 메모리의 주소를 반환한다.(반환된 주소는 이 클래스가 관리하고 있다)
	UINT8 *MemoryAlloc(UINT32 a_data_size);
	// 전송 또는 수신에 사용되던 메모리를 제거한다.
	void DeleteData();

	inline UINT32 GetTotalSize() { return m_total_size; } // 할당된 메모리의 크기를 반환한다.
	inline UINT32 GetCurSize() { return m_cur_size; }     // 현재 작업중인 메모리의 위치를 반환한다.
};


class TW_SendManager : public TW_ExchangeManager
{
public:
	// 현재 전송할 위치와 크기를 계산한다.
	UINT32 GetPosition(UINT8 **ap_data, UINT32 a_data_size = 2048);
	// 전송이 완료되었는지 체크한다.
	inline INT32 IsProcessing() { return m_total_size != m_cur_size; }
};

class TW_RecvManager : public TW_ExchangeManager
{
public:
	// 수신된 데이터를 기존 수신된 데이터에 추가한다.
	UINT32 AddData(UINT8 *ap_data, UINT32 a_size);
	// 수신된 데이터를 하나로 합친 메모리의 시작 주소를 얻는다.
	inline UINT8 *GetData() { return mp_data; }
};

class TW_Socket
{
protected:
	UINT8 m_valid_key;  // 프로토콜의 유효성을 체크하기 위한 구분 값을 저장할 변수
	UINT8 m_tipsware_socket_type;  // 0 -> 기본 소켓(4바이트), 1 -> tipsware socket
	UINT8 m_nodelay_mode;          // 0 -> 일반 소켓, 1 -> 지연을 사용하지 않는 소켓(부하증가)
	UINT8 *mp_send_data, *mp_recv_data; // 전송과 수신에 사용할 버퍼
	// 데이터가 수신되거나 상대편이 접속을 해제했을 때 메시지를 수신할 윈도우 핸들 값
	HWND mh_notify_wnd;         
	// 데이터가 수신되거나 상대편이 접속을 해제했을 때 사용할 메시지 ID
	UINT32 m_data_notify_id;
	UINT32 m_tipsware_socket_index;

public:
	// 객체 생성시에 프로토콜 구분 값과 데이터 수신 및 연결 해제에 사용할 메시지 ID를 지정해야 한다.
	TW_Socket(UINT8 a_valid_key, UINT32 a_data_notify_id, UINT8 a_nodelay_mode = 0, UINT8 a_tipsware_type = 0);
	virtual ~TW_Socket();

	// 연결된 대상에게 네트워크로 데이터를 전송할 때 사용할 함수
	// 이 함수는 전달된 정보를 가지고 하나의 메모리에 약속된 Head 정보를 구성해서 전송합니다.
	INT32 SendFrameData(SOCKET ah_socket, UINT8 a_message_id, UINT8 *ap_body_data, UINT32 a_body_size);
	// 재시도 수신을 지원하는 함수
	INT32 ReceiveData(SOCKET ah_socket, UINT32 a_body_size);
	// 데이터가 수신되었을때 수신된 데이터를 처리하는 함수
	void ProcessRecvEvent(SOCKET ah_socket);

	// 접속된 대상을 끊을 때 사용하는 함수 - 자식 클래스에서 꼭 재정의해서 사용해야 함
	virtual void DisconnectSocket(SOCKET ah_socket, INT32 a_error_code);
	// 수신된 데이터를 처리하는 함수 - 자식 클래스에서 꼭 재정의해서 사용해야 함
	virtual INT32 ProcessRecvData(SOCKET ah_socket, UINT8 a_msg_id, UINT8 *ap_recv_data, UINT32 a_body_size);

	// ASCII 형식의 문자열을 유니코드로 변환합니다.
	static void AsciiToUnicode_IP(wchar_t *ap_dest_ip, char *ap_src_ip);
	// 유니코드 형식의 문자열을 ASCII로 변환합니다.
	static void UnicodeToAscii_IP(char *ap_dest_ip, wchar_t *ap_src_ip);
};

class TW_UserData
{
protected:
	// 클라이언트와 통신하기 위해 사용할 소켓 핸들
	SOCKET mh_socket;
	// 클라이언트에게 큰 데이터를 전송하기 위해 사용할 객체
	TW_SendManager *mp_send_man;
	// 클라이언트에게서 큰 데이터를 수신하기 위해 사용할 객체
	TW_RecvManager *mp_recv_man;
	// 클라이언트의 IP 주소
	wchar_t m_ip_address[16];

public:
	TW_UserData();
	virtual ~TW_UserData();

	inline SOCKET GetHandle() { return mh_socket; }  // 소켓 핸들 값을 얻는다.
	inline void SetHandle(SOCKET ah_socket) { mh_socket = ah_socket; } // 소켓 핸들 값을 설정한다.
	inline TW_SendManager *GetSendMan() { return mp_send_man; }  // 전송용 객체의 주소를 얻는다.
	inline TW_RecvManager *GetRecvMan()	{ return mp_recv_man; }  // 수신용 객체의 주소를 얻는다.

	wchar_t *GetIP();                          // 클라이언트의 IP 주소를 얻는다.
	void SetIP(const wchar_t *ap_ip_address);  // 클라이언트의 IP 주소를 설정한다.
	// 소켓 정보가 할당되어 있다면 소켓을 닫고 초기화한다. 
	// a_linger_flag에 1을 명시하면 소켓이 데이터를 수신중이더라도 즉시 소켓을 제거한다.
	void CloseSocket(UINT8 a_linger_flag = 0);      

	// 다형성 적용 시, 동일한 클래스를 확장할때 이 함수를 사용한다.
	virtual TW_UserData *CreateObject() { return new TW_UserData; }
};

class TW_ServerSocket : public TW_Socket
{
protected:
	// listen 작업에 사용할 소켓 핸들
	SOCKET mh_listen_socket;
	// 새로운 클라이언트가 접속했을 때 발생할 메시지 ID 값
	UINT32 m_accept_notify_id;
	// 서버가 관리할 최대 사용자 수
	UINT16 m_max_user_count;
	// 최대 사용자를 저장하기 위해서 사용할 객체들
	TW_UserData **mp_user_list;

public:
	TW_ServerSocket(UINT8 a_valid_key, UINT16 a_max_user_count, TW_UserData *ap_user_data,
							UINT32 a_accept_notify_id = 25001, UINT32 a_data_notify_id = 25002);
	virtual ~TW_ServerSocket();

	// 서버 서비스를 시작한다. 반환값이 1이면 성공. -1이면 소켓 생성 실패, -2이면 생성된 소켓을 바인드하는데 실패
	INT32 StartServer(const wchar_t *ap_ip_address, UINT16 a_port, HWND ah_notify_wnd);
	// Accept 시에 추가적으로 해야할 작업이 있다면 이 함수를 오버라이딩해서 처리해야함
	virtual void AddWorkForAccept(TW_UserData *ap_user) { }
	// 최대 사용자수 초과시에 추가적으로 해야할 작업이 있다면 이 함수를 오버라이딩해서 처리해야함
	virtual void ShowLimitError(const wchar_t *ap_ip_address) { }
	// 새로운 클라이언트가 접속할때 사용할 함수 - FD_ACCEPT와 연결된 메시지가 발생했을때 사용하면 된다.
	// 반환값이 1이면 성공. -1이면 accept 실패, -2이면 접속 가능한 최대 사용자수 초과
	INT32 ProcessToAccept(WPARAM wParam, LPARAM lParam);

	// 클라이언트가 접속해제시에 추가적으로 해야할 작업이 있다면 이 함수를 오버라이딩해서 처리해야함
	// a_error_code : 0이면 정상종료, -1이면 키값이 유효한지 않아서 종료, -2이면 바디정보 수신중에 오류 발생
	virtual void AddWorkForCloseUser(TW_UserData  *ap_user, INT32 a_error_code);

	// 새로운 데이터가 수신(FD_READ)되거나 클라이언트가 연결이 해제(FD_CLOSE)될때 발생하는 
	// 메시지에서 서버 소켓과 관련된 작업을 처리해주는 함수
	void ProcessClientEvent(WPARAM wParam, LPARAM lParam);

	// 소켓 핸들을 사용하여 어떤 사용자인지 찾는다. 찾으면 사용자의 위치를 반환한다.
	inline UINT16 FindUserIndex(SOCKET ah_socket)
	{
		for (UINT16 i = 0; i < m_max_user_count; i++) if (mp_user_list[i]->GetHandle() == ah_socket) return i;
		return (UINT16)0xFFFF;
	}

	// 소켓 핸들을 사용하여 어떤 사용자인지 찾는다. 찾으면 사용자 정보를 관리하는 객체의 주소를 반환한다.
	inline TW_UserData *FindUserData(SOCKET ah_socket)
	{
		for (UINT16 i = 0; i < m_max_user_count; i++) if (mp_user_list[i]->GetHandle() == ah_socket) return mp_user_list[i];
		return NULL;
	}

	// 전체 사용자에 대한 정보를 얻는 경우
	inline TW_UserData **GetUserList() { return mp_user_list; }
	// 색인을 사용하여 특정 사용자에 대한 정보를 얻는 경우
	inline TW_UserData *GetUserData(int a_index) { return mp_user_list[a_index]; }
	// 최대 사용자 수를 얻는 경우
	UINT16 GetMaxUserCount() { return m_max_user_count; }

	// ah_socket 핸들 값을 사용하는 소켓 사용자를 강제로 종료 시킨다.
	virtual void DisconnectSocket(SOCKET ah_socket, INT32 a_error_code);
	// FD_READ 이벤트가 발생했을때 실제 데이터를 처리하는 함수 
	// (서버 소켓을 상속 받은 클래스에서 반드시 재정의하여 자신이 정의한 메시지를 처리해야 한다.)
	virtual INT32 ProcessRecvData(SOCKET ah_socket, UINT8 a_msg_id, UINT8 *ap_recv_data, UINT32 a_body_size);
};

class TW_ClientSocket : public TW_Socket
{
protected:
	// 서버와 통신하기 위해 사용할 소켓 핸들
	SOCKET mh_socket;
	// 0 : 접속 안됨, 1 : 접속 시도중, 2 : 접속 중
	UINT8 m_connect_flag;
	// 서버에 접속을 시도한 결과를 알려줄 윈도우 메시지를 저장한다.
	UINT32 m_connect_notify_id;
	// 서버에 큰 데이터를 전송하기 위해 사용할 객체
	TW_SendManager m_send_man;
	// 서버에 큰 데이터를 수신하기 위해 사용할 객체
	TW_RecvManager m_recv_man;

public:
	TW_ClientSocket(UINT8 a_valid_key, UINT32 a_connect_notify_id = 26001, UINT32 a_data_notify_id = 26002);
	virtual ~TW_ClientSocket();

	inline TW_SendManager *GetSendMan() { return &m_send_man; }  // 전송용 객체의 주소를 얻는다.
	inline TW_RecvManager *GetRecvMan() { return &m_recv_man; }  // 수신용 객체의 주소를 얻는다.

	// 서버에 접속을 시도한다.
	INT32 ConnectToServer(const wchar_t *ap_ip_address, UINT16 a_port_num, HWND ah_notify_wnd);
	// 서버 접속에 대한 결과가 메시지로 전달되었을때 이 함수를 사용하면 된다.
	// 반환값이 1이면 서버에 접속을 성공함, 0이면 서버에 접속을 실패함
	INT32 ResultOfConnection(LPARAM lParam);
	// 서버가 데이터를 전송하거나 연결을 해제했을때 발생하는 윈도우 메시지에서 이 함수를 사용하면 됩니다.
	// 반환값이 0 이면 서버가 접속을 해제, 1이면 서버에서 데이터를 수신
	INT32 ProcessServerEvent(WPARAM wParam, LPARAM lParam);
	// 서버와 강제로 접속을 해제할 때 사용합니다.
	virtual void DisconnectSocket(SOCKET ah_socket, INT32 a_error_code);
	// FD_READ 이벤트가 발생했을때 실제 데이터를 처리하는 함수 
	// (서버 소켓을 상속 받은 클래스에서 반드시 재정의하여 자신이 정의한 메시지를 처리해야 한다.)
	virtual INT32 ProcessRecvData(SOCKET ah_socket, UINT8 a_msg_id, UINT8 *ap_recv_data, UINT32 a_body_size);
	// 서버로 데이터를 전송할 때 사용합니다. 메시지 ID와 전송할 데이터의 주소, 데이터의 크기를 넘겨주면
	// 약속된 프로토콜 형식으로 구성해서 전송합니다.
	INT32 SendFrameData(UINT8 a_message_id, UINT8 *ap_body_data, UINT32 a_body_size);

	// 서버와의 접속상태를 알고 싶을때 사용한다. 0 : 해제상태, 1 : 접속상태
	// 내부적으로는 상태를 세가지로 관리하지만 외부에 알려줄때는 두가지 상태로 알려준다.
	// '접속 시도중' 상태는 해제로 간주한다.
	inline INT32 IsConnected() { return m_connect_flag == 2; }
	// 서버와 통신하기 위해 생성한 소켓의 핸들 값을 알고 싶을 때 사용한다.
	inline SOCKET GetHandle() { return mh_socket; }
};

#endif