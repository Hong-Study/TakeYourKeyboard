#ifndef _TWAPI_W01_H_
#define _TWAPI_W01_H_

/*
  이 라이브러리는 Win32와 Direct2D를 사용해서 프로그램을 하는 
  개발자들이 좀더 편하게 프로그램을 개발할 수 있도록 만든 라이브러리 입니다.
  기존에 제가 만들었던 인터페이스 라이브러리와 유사하게 만들었고
  안드로이드용 라이브러리인 TWAPI_A01과 클래스나 함수를 비슷하게 유지하여
  Win32 프로그램과 안드로이드 앱간에 소스 공유를 편하게 할 수 있도록 했습니다.
  이 라이브러리에 대한 문제점을 발견하거나 개선점이 필요하다면 아래의 이 메일로
  의견 남겨주시거나 블로그에 댓글로 남겨주세요.
  이메일 : tipsware@naver.com
  블로그 : http://blog.naver.com/tipsware
  
  라이브러리 생성 : 2017년 11월 2일 ( 김성엽 )
  라이브러리 최종 업데이트 : 2017년 12월 14일 ( 김성엽 )

  그리고 이 라이브러리를 사용하는 사람들이 편하게 사용할 수 있도록 
  필요한 라이브러리를 추가하는 전처리기 코드를 남깁니다.

  #pragma comment(lib, "D2D1.lib")
  #pragma comment(lib, "D3D11.lib")
  #pragma comment(lib, "DXGI.lib")
  #pragma comment(lib, "dxguid.lib")
  #pragma comment(lib, "DWRITE.lib")
  #pragma comment(lib, "Msimg32.lib")
  #pragma comment(lib, "WS2_32.lib")

  #ifdef _DEBUG
      #pragma comment(lib, "DSH_TWAPI_W01.lib")
  #else
      #pragma comment(lib, "RST_TWAPI_W01.lib")
  #endif
*/

#include <Windows.h>
#include <Windowsx.h>
#include <stdio.h>

#include <wrl.h>
#include <d3d11.h>
#include <dxgi.h>
#include <DXGI1_2.h>
#include <d2d1effectauthor.h>
#include <dwrite.h>

using namespace Microsoft::WRL;

// Direct2D를 사용하기 위한 헤더 파일과 라이브러리 파일을 포함시킨다.
#include <d2d1.h>
#include <Wincodec.h>     // IWICImagingFactory를 사용하기 위해서 추가

// Direct2D에서 자주 사용할 네임 스페이스를 생략할 수 있도록 설정한다.
using namespace D2D1;

#define TW_SetIRect(x, l, t, r, b){ x.left = l; x.top = t; x.right = r; x.bottom = b; }
#define TW_SetFRect(x, l, t, r, b){ x.left = (float)(l); x.top = (float)(t); x.right = (float)(r); x.bottom = (float)(b); }
#define TW_PosInRect(pos, l, t, r, b) (l <= pos.x && t <= pos.y && r >= pos.x && b >= pos.y) 

#define TW_SetFPos(pt, x_pos, y_pos){ pt.x = (FLOAT)x_pos; pt.y = (FLOAT)y_pos; }
#define TW_SetFSize(size, x_pos, y_pos){ size.width = (FLOAT)x_pos; size.height = (FLOAT)y_pos; }

#define TW_COLOR(color) (color/256.0f)
#define TW_RGB(color, red, green, blue) { color.r = (FLOAT)(red/255.0); color.g = (FLOAT)(green/255.0); color.b = (FLOAT)(blue/255.0); color.a = 1.0f; }
#define TW_FRGB(color, red, green, blue) { color.r = (FLOAT)red; color.g = (FLOAT)green; color.b = (FLOAT)blue; color.a = 1.0f; }

#define TW_Release(x) { if(x != NULL){ x->Release(); x = NULL; }}

#define TW_IMAGE_BMP       0
#define TW_IMAGE_PNG       1
#define TW_IMAGE_JPEG      2

template<class Interface>
inline void TW_IRelease(Interface **ap_interface_object)
{
	if (*ap_interface_object != NULL) {
		(*ap_interface_object)->Release();
		(*ap_interface_object) = NULL;
	}
}

// Direct2D를 구성하는 각종 객체를 생성하는 Factory 객체
extern ID2D1Factory *gp_factory;
// Windows 8.1 이상의 운영체제에서 사용하기 위한 Factory 객체
extern ID2D1Factory1 *gp_factory_ext;
// WIC(Windows Imaging Component)관련 객체를 생성하기 위한 Factory 객체
extern IWICImagingFactory *gp_wic_factory;

// TW_String 클래스는 문자열을 편리하게 관리할 수 있도록 만든 클래스입니다. 이 클래스는 문자열 처리를 좀더 빠르게 하기 
// 위해서 문자열의 길이를 같이 사용하는 경우가 많은데 이 때 사용하는 문자열의 길이는 순수 문자열의 길이를 의미합니다. 
// 예를 들어, 이 객체가 "abc" 문자열을 가지고 있으면 문자열의 길이가 저장된 m_length에는 3이 저장 됩니다. 그리고 인자에
// a_length를 사용할때도 이 규칙은 동일하게 적용됩니다.
class TW_String
{
protected:
	wchar_t *mp_string;   // 이 객체가 관리하는 문자열의 주소
	int m_length;         // 이 객체가 관리하는 문자열의 길이 ("abc"이면 3이 저장)

public:
	TW_String();  // 기본 생성자
	TW_String(const char *ap_string);
	TW_String(const wchar_t *ap_string);
	TW_String(const TW_String &ar_string);  // 복사 생성자

	virtual ~TW_String();

	void InitialObject(const wchar_t *ap_string);  // 객체 초기화 함수
	inline int GetLength() { return m_length; }   // 문자열의 길이를 알려주는 함수
	inline const wchar_t *GetString() { return mp_string; } // 문자열의 내용을 알려주는 함수
	void RemoveString();                           // 이 객체가 관리하던 문자열을 제거한다.

	// 전달된 문자열을 복사해서 관리한다.
	void SetString(const wchar_t *ap_string);
	// 전달된 문자열의 주소를 그대로 사용한다. (복사를 하지않고 원본을 그대로 사용함)
	void AttachString(wchar_t *ap_string, int a_length);
	// 저장된 문자열에 다른 문자열을 추가하는 경우에 사용한다.
	// 예를 들어, "abc"가 저장되어 있는데 AddString("def", 3); 이라고 사용하면 "abcdef"가 됩니다.
	void AddString(const wchar_t *ap_string, int a_length);

	// 전달된 아스키 문자열을 유니코드로 변경하여 클래스 내부에 저장한다. 반환값은 변환된 문자의 갯수이다 (wcslen 값과 일치한다 )
	int AsciiToUnicode(char *ap_src_string);
	// 전달된 아스키 문자열을 유니코드로 변경하여 ap_dst_string에 반환한다. 반환값은 변환된 문자의 갯수이다 (wcslen 값과 일치한다 )
	// ap_dest_string 에 NULL을 명시하면 변환했을때의 문자열 길이만 알아낼수 있다.
	static int AsciiToUnicode(char *ap_src_string, wchar_t *ap_dest_string);
	// 클래스가 관리하는 문자열을 UTF8 형식으로 변경하여 ap_dest_string에 반환한다. 반환값은 UTF8 문자열에 사용된 메모리 크기이다.
	// ap_dest_string 에 NULL을 명시하면 변환했을때의 문자열 길이만 알아낼수 있다.
	int UnicodeToUTF8(char *ap_dest_string);
	// ap_src_string 문자열을 UTF8 형식으로 변경하여 ap_dest_string에 반환한다. 반환값은 UTF8 문자열에 사용된 메모리 크기이다.
	// ap_dest_string 에 NULL을 명시하면 변환했을때의 문자열 길이만 알아낼수 있다.
	static int UnicodeToUTF8(wchar_t *ap_src_string, char *ap_dest_string);
	// 클래스가 관리하는 문자열을 아스키코드로 변경하여 ap_dst_string에 반환한다. 반환값은 변환된 문자의 갯수이다 (strlen 값과 일치한다 )
	// ap_dest_string 에 NULL을 명시하면 변환했을때의 문자열 길이만 알아낼수 있다.
	int UnicodeToAscii(char *ap_dest_string);
	// 전달된 유니코드 문자열을 아스키코드로 변경하여 ap_dst_string에 반환한다. 반환값은 변환된 문자의 갯수이다 (strlen 값과 일치한다 )
	// ap_dest_string 에 NULL을 명시하면 변환했을때의 문자열 길이만 알아낼수 있다.
	static int UnicodeToAscii(wchar_t *ap_src_string, char *ap_dest_string);

	// printf와 같은 형식을 사용하고 싶을 때 사용하는 함수. 이 함수의 결과에 해당하는 문자열은 화면에 출력되지 않고
	// 이 객체의 문자열(mp_string)에 저장됩니다.
	int Format(const wchar_t *ap_format, ...);

	// 대입 연산자 오버로딩 str = L"test";
	void operator=(const TW_String& ar_string);
	// 문자열 확장(append)를 위한 연산자 오버로딩
	void operator+=(const TW_String& ar_str);
	// 문자열을 더하기 위한 연산자 오버로딩
	friend TW_String operator+(const TW_String& ar_str1, const TW_String& ar_str2)
	{
		TW_String str = ar_str1;
		str.AddString(ar_str2.mp_string, ar_str2.m_length);
		return str;
	}
	// 문자열 비교를 위한 연산자 오버로딩 (같은지 확인하기 위한 연산자)
	friend char operator==(const TW_String& ar_str1, const TW_String& ar_str2)
	{
		if (ar_str1.m_length != ar_str2.m_length) return 0;
		return memcmp(ar_str1.mp_string, ar_str2.mp_string, ar_str1.m_length << 1) == 0;
	}
	// 문자열 비교를 위한 연산자 오버로딩 (다른지 확인하기 위한 연산자)
	friend char operator!=(const TW_String& ar_str1, const TW_String& ar_str2)
	{
		if (ar_str1.m_length != ar_str2.m_length) return 1;
		return memcmp(ar_str1.mp_string, ar_str2.mp_string, ar_str1.m_length << 1) != 0;
	}

	friend char operator!=(const TW_String& ar_str1, const wchar_t *ap_str2)
	{
		if (ar_str1.m_length != wcslen(ap_str2)) return 1;
		return memcmp(ar_str1.mp_string, ap_str2, ar_str1.m_length << 1) != 0;
	}

	operator LPCWSTR() const
	{
		return mp_string;
	}

	operator LPWSTR()
	{
		return mp_string;
	}
};

// 연결 리스트를 구성하게 될 노드의 구조
struct node_ext {
	// 사용자 정의 데이터를 저장하기 위해 사용할 포인터
	void *p_object;
	// 연결된 다음 노드를 가리키는 포인터
	node_ext *p_next;
};

class TW_StockListExt;

class TW_ListExt
{
private:
	// 연결 리스트의 처음과 끝을 가리키는 포인터.
	node_ext *mp_head, *mp_tail;
	// 연결 리스트에 등록된 노드의 갯수를 가지고 있는 변수.
	int m_node_ext_count;

	// 사용자가 사용하던 메모리를 제거할때 TW_ListExt 클래스에 의해 호출될 함수의 주소를 관리할 포인터
	void(*mp_delete_user_data)(void *ap_object);

public:
	// 객체 생성자. 사용자가 전달한 사용자 정의 데이터를 정확하게 제거하기 위해서 각 노드에 연결된
	// 데이터를 정리할때 호출할 함수를 포인터 형식으로 넘겨 받는다.
	// ( Example ) 
	//
	//    void DeleteUserData(void *parm_object)
	//    {
	//        UserData *p = (UserData *)parm_object;  // 자신이 할당한 데이터 형으로 변환한다.
	//
	//        delete[] p->p_name; // 해당 객체나 구조체에서 추가적으로 할당한 메모리가 있다면 정리한다.
	//        delete p; // 해당 노드에 할당된 메모리를 해제한다.
	//    }
	//
	//    TW_ListExt my_list(DeleteUserData);
	//
	// 만약 NULL을 인자로 넘기는 경우, TW_ListExt 클래스는 자신이 파괴될때 노드만 삭제하고 노드에
	// 연결된 데이터는 그대로 둔다. 원본을 참조하는 형식으로 구성된 연결리스트를 구성할때는 원본을
	// 제거하지 않도록 NULL 을 넘겨서 사용하면 된다.
	TW_ListExt(void(*afp_delete_user_data)(void *ap_object) = NULL);
	// 객체 파괴자
	~TW_ListExt();

	// 사용자가 등록한 메모리를 파괴할때 호출할 함수를 변경
	void ChangeDeleteUserData(void(*afp_delete_user_data)(void *ap_object));
	// 연결 리스트를 초기화하는 함수
	void InitListObject();

	// [ 새로운 노드를 추가하는 함수들 ]

	// AddObject, AddObjectByIndex 함수는 사용자가 전달한 메모리를 그대로 노드에 연결해서 사용하기 때문에 
	// 아래와 같은 두가지 형태로 사용하는건 오류가 발생하니 주의해야 합니다.
	// 리스트의 끝에 새로운 노드를 추가하는 함수. 
	//
	//  1. 지역 변수의 주소를 넘겨서는 안됩니다.
	//
	//    UserData data;
	//    m_my_list.AddObject(&data);
	//    (주의) data 변수는 지역변수이기 때문에 함수의 종료시에 메모리에서 사라지게 됩니다. 하지만 그 사실을
	//           m_my_list가 알지 못하기 때문에 해당 주소를 그대로 사용하게 되어 문제가 발생하게 됩니다.
	//
	//  2. 동적할당된 주소라고 해도 TW_ListExt에 넘겨진 주소는 TW_ListExt가 관리하기 직접 삭제하면 오류가 발생합니다.
	//
	//    UserData *p_data = new UserData;
	//    m_my_list.AddObject(p_data);
	//    delete p_data;  <-- (주의) 이렇게 직접 삭제해버리면 m_my_list 에 전달된 주소가 삭제된 메모리를 가리키게 되어
	//                                문제가 발생합니다.
	//

	// 리스트의 끝에 새로운 노드를 추가하는 함수. 
	void AddObject(void *parm_object);
	// 원하는 위치에 노드를 추가하는 함수, 0 을 명시하면 리스트의 가장 첫 위치에 노드가 추가된다.
	// 추가할 위치를 잘못 지정하면 노드 추가가 실패하고 -1 값을 반환한다.
	char AddObjectByIndex(int parm_index, void *parm_object);
	node_ext *AddObjectAfterThisNode(node_ext *parm_this_node, void *parm_p_object);

	// [ 노드를 삭제하는 함수들 ]

	// DeleteNode, DeleteAllObject, DeleteNodeByIndex 함수의 두번째 인자로 사용된 parm_delete_object_flag 변수는
	// 노드에 연결된 사용자 데이터를 삭제할것인지 여부를 지정하나다. 
	// ( 0 -> 노드만 삭제, 1 -> 노드와 사용자 데이터 모두 삭제)

	// 리스트에 등록된 노드들 중에서 parm_node에 지정된 노드를 찾아서 해당 노드를 삭제한다.
	char DeleteNode(node_ext *parm_node, char parm_delete_object_flag = 1);
	// 리스트에 등록된 노드들 중에서 parm_object에 지정된 정보를 가지고 있는 노드를 찾아서 해당 노드를 삭제한다.
	char DeleteNode(void *parm_object, char parm_delete_object_flag = 1);
	// 리스트에 등록된 모든 노드를 삭제한다.
	void DeleteAllNode(char parm_delete_object_flag = 1);
	// 사용자가 지정한 위치에 있는 노드를 찾아서 해당 노드를 삭제한다.
	char DeleteNodeByIndex(int parm_index, char parm_delete_object_flag = 1);

	// [ 노드를 검색하는 함수들 ]

	// 연결리스트에서 지정한 위치에 있는 노드의 주소를 얻는다.
	node_ext *GetNodeByIndex(int parm_index);
	// 연결리스트에서 지정한 위치에 있는 노드가 가리키는 데이터를 얻는다.
	void *GetObjectByIndex(int parm_index);
	// 연결리스트에 등록된 노드들중에서 parm_object에 명시된 주소와 동일한 주소를 가진 데이터가 있는지
	// 찾아서 그 위치를 반환한다. ( -1 이면 검색에 실패했다는 뜻이다. )
	int FindNodeIndexByObject(void *parm_object);

	// 가장 처음 위치에 있는 노드의 주소를 얻는다.
	node_ext *GetHeadNode();
	// 지정한 노드의 다음에 있는 노드를 얻는다.
	node_ext *GetNextNode(node_ext *parm_current_node);
	// 가장 마지막 위치에 있는 노드의 주소를 얻는다.
	node_ext *GetTailNode();

	void CloneTipsListExt(TW_ListExt *parm_src_list, char parm_delete_src_list = 1);
	void CloneTipsListExt(TW_StockListExt *parm_src_list);

	// 연결리스트에 등록된 노드의 갯수를 얻는다.
	int GetNodeCount();
};

class TW_StockListExt
{
private:
	// 연결 리스트의 처음과 끝을 가리키는 포인터.
	node_ext *mp_head, *mp_tail;
	// 연결 리스트에 등록된 노드의 갯수를 가지고 있는 변수.
	int m_node_ext_count;

	// 고정해서 미리 할당한 노드의 주소, 할당 가능한 주소의 위치
	node_ext *mp_fixed_node_memory, *mp_alloc_node_pos;
	// 미리 할당한 노드의 최대 갯수
	unsigned int m_max_fixed_node_count;

	// 사용자가 사용하던 메모리를 제거할때 TW_StockListExt 클래스에 의해 호출될 함수의 주소를 관리할 포인터
	void(*mp_delete_user_data)(void *parm_object);

public:
	// 이 객체가 사용할 노드의 최대 개수와 노드 삭제시 사용할 함수를 설정한다.
	// ( parm_delete_user_data 에 대해서는 TW_ListExt::TW_ListExt 를 참고 )
	TW_StockListExt(unsigned int parm_max_fixed_node_count, void(*parm_delete_user_data)(void *parm_object) = NULL);
	// 객체 파괴자
	~TW_StockListExt();

	// 사용자가 등록한 메모리를 파괴할때 호출할 함수를 변경
	void ChangeDeleteUserData(void(*parm_delete_user_data)(void *parm_object));
	// 연결 리스트를 초기화하는 함수
	void InitListObject();

	// 리스트의 끝에 새로운 노드를 추가하는 함수. ( TW_ListExt::AddObject 주의사항 참고 )
	void AddObject(void *parm_object);
	// 원하는 위치에 노드를 추가하는 함수, 0 을 명시하면 리스트의 가장 첫 위치에 노드가 추가된다.
	// 추가할 위치를 잘못 지정하면 노드 추가가 실패하고 -1 값을 반환한다.
	char AddObjectByIndex(int parm_index, void *parm_object);
	node_ext *AddObjectAfterThisNode(node_ext *parm_this_node, void *parm_p_object);

	// [ 노드를 삭제하는 함수들 ]

	// DeleteNode, DeleteAllObject, DeleteNodeByIndex 함수의 두번째 인자로 사용된 parm_delete_object_flag 변수는
	// 노드에 연결된 사용자 데이터를 삭제할것인지 여부를 지정하나다. 
	// ( 0 -> 노드만 삭제, 1 -> 노드와 사용자 데이터 모두 삭제)

	// 리스트에 등록된 노드들 중에서 parm_node에 지정된 노드를 찾아서 해당 노드를 삭제한다.
	char DeleteNode(node_ext *parm_node, char parm_delete_object_flag = 1);
	// 리스트에 등록된 모든 노드를 삭제한다.
	void DeleteAllNode(char parm_delete_object_flag = 1);
	// 사용자가 지정한 위치에 있는 노드를 찾아서 해당 노드를 삭제한다.
	char DeleteNodeByIndex(int parm_index, char parm_delete_object_flag = 1);

	// [ 노드를 검색하는 함수들 ]

	// 연결리스트에서 지정한 위치에 있는 노드의 주소를 얻는다.
	node_ext *GetNodeByIndex(int parm_index);
	// 연결리스트에서 지정한 위치에 있는 노드가 가리키는 데이터를 얻는다.
	void *GetObjectByIndex(int parm_index);
	// 연결리스트에 등록된 노드들중에서 parm_object에 명시된 주소와 동일한 주소를 가진 데이터가 있는지
	// 찾아서 그 위치를 반환한다. ( -1 이면 검색에 실패했다는 뜻이다. )
	int FindNodeIndexByObject(void *parm_object);

	// 가장 처음 위치에 있는 노드의 주소를 얻는다.
	node_ext *GetHeadNode();
	// 지정한 노드의 다음에 있는 노드를 얻는다.
	node_ext *GetNextNode(node_ext *parm_current_node);
	// 가장 마지막 위치에 있는 노드의 주소를 얻는다.
	node_ext *GetTailNode();

	// 연결리스트에 등록된 노드의 갯수를 얻는다.
	int GetNodeCount();
};

class TW_Font_D2D
{
protected:
	IDWriteTextFormat * mp_text_format;
	TW_String m_font_name;
	FLOAT m_font_size;

public:
	TW_Font_D2D();
	virtual ~TW_Font_D2D();

	INT32 CreateFont(const wchar_t *ap_name, FLOAT a_size, DWRITE_FONT_WEIGHT a_weight = DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE  a_style = DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH  a_stretch = DWRITE_FONT_STRETCH_NORMAL, UINT8 is_korean = 1);
	void DestroyFont();

	// DWRITE_TEXT_ALIGNMENT_CENTER(0), DWRITE_TEXT_ALIGNMENT_LEADING(1), DWRITE_TEXT_ALIGNMENT_CENTER(2), DWRITE_TEXT_ALIGNMENT_TRAILING(3)
	void SetAlignment(UINT8 a_type = 1);
	IDWriteTextFormat *GetFormat();
	D2D1_SIZE_F GetTextSize(const wchar_t *ap_text);
};

#define MAX_STOCK_SOLID_BRUSH          100

class TW_StockSolidBrushManager
{
private:
	ID2D1SolidColorBrush * mp_brush[MAX_STOCK_SOLID_BRUSH];
	D2D1_COLOR_F m_brush_color[MAX_STOCK_SOLID_BRUSH];
	int m_brush_count;

public:
	TW_StockSolidBrushManager();
	~TW_StockSolidBrushManager();

	ID2D1SolidColorBrush *GetMyBrush(ID2D1RenderTarget *ap_render_target, D2D1_COLOR_F *ap_color);
	void DeleteAllBrush();
};

class TW_CtrlWnd
{
protected:
	char m_attached_flag;  // 컨트롤 직접 생성여부
	HWND mh_wnd;           // 컨트롤의 윈도우 핸들 값
	COLORREF m_fill_color, m_text_color; // 컨트롤의 배경색과 글자색
	COLORREF m_in_border_color, m_out_border_color; // 컨트롤의 테두리 색
	UINT m_control_id;  // 컨트롤의 ID 값
	HFONT mh_font;  // 컨트롤에 사용할 글꼴 핸들 값을 저장할 변수

public:
	TW_CtrlWnd();           // 객체 생성자
	virtual ~TW_CtrlWnd();  // 객체 파괴자

	inline HWND GetHandle() { return mh_wnd; }
	// 외부에서 만들어진 컨트롤을 이 클래스가 관리하는 경우에 사용
	void Attach(HWND ah_attach_wnd);  
	// 컨트롤의 색상을 변경할 때 사용하는 함수
	void ChangeCtrlColor(COLORREF a_fill_color, COLORREF a_out_border_color, 
						 COLORREF a_in_border_color, COLORREF a_text_color);
	// 컨트롤의 Owner Draw 속성이 True인 경우에 처리하는 함수
	virtual void OnDrawItem(DRAWITEMSTRUCT *ap_dis);
	virtual INT32 OnCtrlColor(HDC ah_dc, HWND ah_wnd);

	// 컨트롤을 다시 그릴 필요가 있을때 호출하는 함수
	void Invalidate();
	// 컨트롤에 글꼴을 설정한다. a_auto_delete에 1을 넘겨주면 이 핸들 값을
	// mh_font에 저장했다가 컨트롤이 제거될때 글꼴도 같이 제거합니다.
	void SetFont(HFONT ah_font, UINT32 a_auto_delete = 1);
};

class TW_ButtonCtrl : public TW_CtrlWnd
{
public:
	TW_ButtonCtrl();   // 객체 생성자
	~TW_ButtonCtrl();  // 객체 파괴자

	// 컨트롤의 Owner Draw 속성이 True인 경우에 처리하는 함수
	// 버튼을 그리기 위해 재정의함
	virtual void OnDrawItem(DRAWITEMSTRUCT *ap_dis);
};

#define EN_ENTER_KEYUP          1000

// Edit 컨트롤에 새롭게 설정할 윈도우 프로시저
LRESULT APIENTRY TW_EditProcForEnterKeyUp(HWND ah_wnd, UINT message_id, WPARAM wParam, LPARAM lParam);

class TW_EditWnd : public TW_CtrlWnd
{
protected:
	// Edit 컨트롤이 본래 사용하던 프로시저의 주소를 저장하기 위한 함수의 포인터
	WNDPROC m_org_edit_proc;

public:
	TW_EditWnd();  // 객체 생성자
	virtual ~TW_EditWnd(); // 객체 파괴자

    // 기본 속성으로 Edit 컨트롤을 생성하는 함수
	HWND Create(HWND ah_parent_wnd, INT32 a_sx, INT32 a_sy, INT32 a_ex, INT32 a_ey, INT32 a_ctrl_id, UINT32 a_style = 0);
	// 서브클래싱을 하는 함수. 기본으로 제공되는 TW_EditProcForEnterKeyUp 프러시저는 
	// 엔터키를 체크해서 EN_ENTER_KEYUP 메시지를 전달하는 기능이 추가되어 있다.
	void SubclassWnd(WNDPROC a_new_proc = TW_EditProcForEnterKeyUp);
	// WM_CTRLCOLOREDIT 메시지에서 사용하는 함수
	virtual INT32 OnCtrlColor(HDC ah_dc, HWND ah_wnd);
	// Edit 컨트롤에서 문자열의 길이를 얻는 함수
	UINT32 GetTextLen();
	// Edit 컨트롤에서 문자열을 얻는 함수
	UINT32 GetText(TW_String &r_str);
	// Edit 컨트롤에서 문자열을 얻는 함수
	UINT32 GetText(wchar_t *ap_string, UINT32 a_max_length);
	// Edit 컨트롤에 문자열을 설정하는 함수
	void SetText(const wchar_t *ap_string);
};

class TW_ListBoxWnd : public TW_CtrlWnd
{
public:
	TW_ListBoxWnd(); // 객체 생성자
	virtual ~TW_ListBoxWnd(); // 객체 파괴자

	// 기본 속성으로 ListBox를 생성하는 함수
	HWND Create(HWND ah_parent_wnd, INT32 a_sx, INT32 a_sy, INT32 a_ex, INT32 a_ey, INT32 a_ctrl_id, UINT32 a_style = LBS_STANDARD);
	// 확장 속성을 사용해서 ListBox를 생성하는 함수
	HWND CreateEx(HWND ah_parent_wnd, INT32 a_sx, INT32 a_sy, INT32 a_ex, INT32 a_ey, INT32 a_ctrl_id, UINT32 a_style = 0, UINT32 a_ex_style = WS_EX_STATICEDGE);
	// 문자열을 ListBox에 추가하는 함수, LBS_SORT 옵션을 가지고 있다면 오름차순으로 정렬해서 추가한다.
	INT32 AddString(const wchar_t *ap_string, UINT8 a_auto_select = 1);
	// 문자열을 사용자가 지정한 위치에 추가하는 함수
	INT32 InsertString(INT32 a_index, const wchar_t *ap_string, UINT8 a_auto_select = 1);
	// 사용자가 지정한 항목에 추가된 문자열의 길이를 얻는다.
	INT32 GetTextLength(INT32 a_index);
	// 사용자가 지정한 항목에서 문자열을 복사해서 가져온다.
	INT32 GetText(INT32 a_index, wchar_t *ap_string, int a_max_len);
	// 사용자가 지정한 항목에서 문자열을 복사해서 가져온다. 
	INT32 GetText(INT32 a_index, TW_String &r_str);
	// ListBox의 특정 항목을 선택한다.
	void SetCurSel(INT32 a_index);
	// ListBox에서 어떤 항목이 선택되어 있는지 위치 값을 얻는다.
	INT32 GetCurSel();
	// ListBox에 추가된 항목의 개수를 얻는다.
	INT32 GetCount();
	// ListBox에 추가된 모든 항목을 제거한다.
	void ResetContent();
	// 지정한 항목에 추가 4바이트 정보를 설정한다. (포인터 형식)
	void SetItemDataPtr(INT32 a_index, void *ap_data);
	// 보통의 경우 InsertString을 사용하고 SetItemDataPtr 함수를 사용해야 하지만
	// 이 함수를 사용하면 두 함수의 기능을 동시에 수행합니다.  (포인터 형식)
	void SetItemDataPtrEx(INT32 a_index, const wchar_t *ap_string, void *ap_data, UINT8 a_auto_select = 1);
	// 항목에 추가된 4바이트 메모리 값을 얻는다. (포인터 형식)
	void *GetItemDataPtr(INT32 a_index);
	// 지정한 항목에 추가 4바이트 정보를 설정한다. (정수 형식)
	void SetItemData(INT32 a_index, INT32 a_data);
	// 보통의 경우 InsertString을 사용하고 SetItemDataPtr 함수를 사용해야 하지만
	// 이 함수를 사용하면 두 함수의 기능을 동시에 수행합니다.  (정수 형식)
	void SetItemDataEx(INT32 a_index, const wchar_t *ap_string, INT32 a_data);
	// 항목에 추가된 4바이트 메모리 값을 얻는다. (정수 형식)
	INT32 GetItemData(INT32 a_index);
	// ListBox에 추가된 문자열에서 원하는 문자열을 검색한다. 이 함수를 사용하면
	// 자신이 찾고 싶은 문자열이 'abc'인 경우에 'abc'뿐만 아니라 'abcdef'와 같이
	// 부분적으로 일치하는 문자열도 찾을 수 있습니다.
	INT32 FindString(INT32 a_index, const wchar_t *ap_string);
	// ListBox에 추가된 문자열에서 원하는 문자열을 검색한다. 이 함수는 정확하게
	// 일치하는 문자열만 검색합니다.
	INT32 FindStringExact(INT32 a_index, const wchar_t *ap_string);

	virtual void OnDrawItem(DRAWITEMSTRUCT *ap_dis);
	virtual INT32 OnCtrlColor(HDC ah_dc, HWND ah_wnd);
};

class TW_Wnd
{
protected:
	// 이 객체가 관리할 윈도우의 핸들
	HWND mh_wnd;
	// Direct2D에서 윈도우의 클라이언트 영역에 그림을 그릴 객체
	ID2D1HwndRenderTarget *mp_target;

	// Windows 8.1 이상에서 사용 가능한 그림 그리기 기능들!
	ID2D1Device *mp_device;
	ID2D1DeviceContext *mp_dc;
	ID2D1Bitmap1 *mp_dc_bitmap;
	IDXGISwapChain1 *mp_sc;
	DXGI_PRESENT_PARAMETERS m_sc_parameters;
	TW_StockSolidBrushManager *mp_sb_man;

	// 마우스의 이전 좌표를 저장하는 변수
	POINT m_prev_pos;
	// 왼쪽 마우스 버튼의 클릭 여부와 클라이언트 영역(Client Region)을 클릭해서 이동할 것인지 여부.
	unsigned char m_is_clicked, m_is_cr_moving;

public:
	TW_Wnd();  // 객체 생성자
	virtual ~TW_Wnd();  // 객체 파괴자

    // 객체가 관리하는 mh_wnd 값을 얻거나 새로운 핸들 값을 설정하는 함수
	HWND GetHandle() { return mh_wnd; }
	void SetHandle(HWND ah_wnd) { mh_wnd = ah_wnd; }
	// 윈도우를 생성할 때 사용하는 함수
	void Create(const wchar_t *ap_title_name, int a_x = 50, int a_y = 50,
		int a_width = 500, int a_height = 300, DWORD a_style = WS_OVERLAPPEDWINDOW);
	void CreateEx(const wchar_t *ap_title_name, int a_x = 50, int a_y = 50,
		int a_width = 500, int a_height = 300, DWORD a_style = WS_OVERLAPPEDWINDOW, 
		DWORD a_ex_style = WS_EX_TOPMOST | WS_EX_ACCEPTFILES);
	// 윈도우를 파괴할 때 사용하는 함수
	void DestoryWindow();
	// 윈도우를 갱신을 대기하지 않고 즉시 갱신하도록 하는 함수
	void UpdateWindow();
	// 윈도우를 화면에 어떻게 출력시킬 것인지를 결정하는 함수
	void ShowWindow(int a_show_type = SW_SHOW);
	// 윈도우의 클라이언트 영역을 기준으로 윈도우 크기를 변경하는 함수
	void ResizeWindow(int a_width, int a_height);
	// 윈도우 화면을 무효화 시켜서 WM_PAINT 메시지를 발생시키는 함수
	void Invalidate();
	// 타이머를 등록하는 함수
	void SetTimer(UINT a_timer_id, UINT a_elapse_time);
	// 타이머를 제거하는 함수
	void KillTimer(UINT a_timer_id);
	void GetClientRect(D2D1_RECT_F *ap_rect);
	// 자신의 윈도우를 타원형으로 변경합니다.
	void ChangeEllipticWnd(int a_x_radius, int a_y_radius);
	inline TW_StockSolidBrushManager *GetSbMan()
	{
		return mp_sb_man;
	}

	// WM_PAINT 메시지가 발생할 때 작업할 함수. 이 함수는 파생 클래스에서 대부분 재정의 함
	virtual void OnPaint();
	// WM_CREATE 메시지가 발생할 때 작업할 함수.
	virtual int OnCreate(CREATESTRUCT *ap_create_info);
	// WM_TIMER 메시지가 발생할 때 작업할 함수.
	virtual void OnTimer(UINT a_timer_id);
	// WM_COMMAND 메시지가 발생할 때 작업할 함수.
	virtual void OnCommand(INT32 a_ctrl_id, INT32 a_notify_code, HWND ah_ctrl);
	// WM_DESTROY 메시지가 발생할 때 작업할 함수.
	virtual void OnDestroy();
	// WM_LBUTTONDOWN 메시지가 발생할 때 작업할 함수.
	virtual void OnLButtonDown(int a_flag, POINT a_pos);
	// WM_LBUTTONUP 메시지가 발생할 때 작업할 함수.
	virtual void OnLButtonUp(int a_flag, POINT a_pos);
	// WM_MOUSEMOVE 메시지가 발생할 때 작업할 함수.
	virtual void OnMouseMove(int a_flag, POINT a_pos);
	// WM_LBUTTONDBLCLK 메시지가 발생할 때 작업할 함수.
	virtual void OnLButtonDblClk(int a_flag, POINT a_pos);
	// WM_RBUTTONDOWN 메시지가 발생할 때 작업할 함수.
	virtual void OnRButtonDown(int a_flag, POINT a_pos);
	// WM_RBUTTONUP 메시지가 발생할 때 작업할 함수.
	virtual void OnRButtonUp(int a_flag, POINT a_pos);
	// WM_MOUSEWHEEL 메시지가 발생할 때 작업할 함수.
	virtual int OnMouseWheel(unsigned short a_flag, short a_z_delta, POINT a_pos);
	// WM_DROPFILES 메시지가 발생할 때 작업할 함수.
	virtual void OnDropFiles(HANDLE ah_drop_info);
	// WM_CTLCOLORLISTBOX 메시지가 발생할 때 작업할 함수.
	virtual int OnCtrlColorListBox(HDC ah_dc, HWND ah_wnd);
	// WM_CTLCOLOREDIT 메시지가 발생할 때 작업할 함수.
	virtual int OnCtrlColorEdit(HDC ah_dc, HWND ah_wnd);
	// WM_DRAWITEM 메시지가 발생할 때 작업할 함수.
	virtual INT32 OnDrawItem(INT32 a_ctrl_id, DRAWITEMSTRUCT *ap_dis);
	// WM_MEASUREITEM 메시지가 발생할 때 작업할 함수.
	// a_is_ctrl : 0 -> Menu, 1 -> ListBox & ComboBox
	virtual INT32 OnMeasureItem(UINT32 a_is_ctrl, MEASUREITEMSTRUCT *ap_mis);
	// WM_COPYDATA 메시지가 발생할 때 작업할 함수.
	virtual void OnCopyData(HWND ah_send_wnd, COPYDATASTRUCT *ap_copy_data);
	// TW_Wnd 클래스가 정의하지 않은 윈도우 메시지를 처리하는 경우에 사용하는 함수
	virtual LRESULT UserMessageProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

class TW_WinApp
{
protected:
	TW_Wnd *mp_wnd;    // 메인 윈도우 객체의 주소를 저장
	HINSTANCE mh_instance;  // WinMain에 전달되는 첫 번째 인자값 저장
	wchar_t m_class_name[32]; // '윈도우 클래스' 이름을 저장할 변수
	// WinMain 함수의 최종 반환값을 WM_QUIT메시지의 wParam 값으로 사용하기
	// 위해서 이 값을 저장할 변수
	int m_exit_state;

public:
	TW_WinApp(HINSTANCE ah_instance, const wchar_t *ap_class_name, int a_use_ext_direct2d = 0);
	virtual ~TW_WinApp();

	int GetExitState() { return m_exit_state; }
	const wchar_t *GetWindowClassName() { return m_class_name; }
	TW_Wnd *GetMainWindow() { return mp_wnd; }
	HINSTANCE GetInstanceHandle() { return mh_instance; }
	
	virtual void InitApplication();
	virtual void InitInstance();
	virtual void Run();
	virtual void ExitInstance();
	virtual void ExitApplication();

	virtual int NormalProcess();
};

extern TW_WinApp *gp_app;
extern TW_Font_D2D *gp_default_font;
extern HFONT gh_default_gdi_font;

void TWAPI_CreateWIC();
int TWAPI_MakeD2D1_Bitmap(IWICBitmapFrameDecode *ap_image_frame, ID2D1DeviceContext *ap_context, ID2D1Bitmap **ap_bitmap);
int TWAPI_MakeD2D1_Bitmap(IWICBitmapFrameDecode *ap_image_frame, ID2D1HwndRenderTarget *ap_target, ID2D1Bitmap **ap_bitmap);
ID2D1Bitmap *TWAPI_CreateD2D1Bitmap(ID2D1RenderTarget *ap_target, HBITMAP ah_bitmap, D2D1_RECT_F *ap_rect, UINT8 a_remove_color_flag = 0, COLORREF a_remove_color = 0);
ID2D1Bitmap *TWAPI_CreateD2D1Bitmap(ID2D1RenderTarget *ap_target, UINT32 *ap_pattern_data, UINT32 a_pattern_width, UINT32 a_pattern_height, UINT8 a_remove_color_flag = 0, COLORREF a_remove_color = 0);
ID2D1Bitmap *TWAPI_CreateD2D1BitmapFromRC(ID2D1RenderTarget *ap_target, INT32 a_resource_id, D2D1_RECT_F *ap_rect, UINT8 a_remove_color_flag = 0, COLORREF a_remove_color = 0);
void TWAPI_MoveRect(D2D1_RECT_F *ap_rect, float a_move_x, float a_move_y);

int TWAPI_LoadImage(ID2D1DeviceContext *ap_context, ID2D1Bitmap **ap_bitmap, const wchar_t *ap_path);
int TWAPI_LoadImage(ID2D1HwndRenderTarget *ap_target, ID2D1Bitmap **ap_bitmap, const wchar_t *ap_path);
ID2D1Bitmap *TWAPI_LoadImageFromMemory(ID2D1RenderTarget *ap_render_target, void *ap_encoding_image_data, UINT32 a_image_size);
void TWAPI_SaveImage(const wchar_t *ap_file_path, HBITMAP ah_src_bitmap, INT32 a_image_type = TW_IMAGE_PNG);
INT32 TWAPI_SaveImageToMemory(UINT8 *ap_image_data, UINT32 a_image_size, HBITMAP ah_src_bitmap, INT32 a_image_type = TW_IMAGE_PNG);

void TWAPI_CreateExtD2D(HWND ah_wnd, ID2D1Device **ap_graphic_device, ID2D1DeviceContext **ap_device_context, ID2D1Bitmap1 **ap_render_bitmap, IDXGISwapChain1 **ap_swap_chain);
ID2D1RadialGradientBrush *CreateRadialGradientBrush(ID2D1RenderTarget *ap_target, D2D1_RECT_F *ap_rect, D2D1_GRADIENT_STOP *ap_stop_list, UINT32 a_stop_count, D2D1_GAMMA a_gamma = D2D1_GAMMA_2_2, D2D1_EXTEND_MODE a_mode = D2D1_EXTEND_MODE_CLAMP);

#endif