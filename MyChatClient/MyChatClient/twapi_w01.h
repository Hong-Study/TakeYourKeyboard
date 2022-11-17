#ifndef _TWAPI_W01_H_
#define _TWAPI_W01_H_

/*
  �� ���̺귯���� Win32�� Direct2D�� ����ؼ� ���α׷��� �ϴ� 
  �����ڵ��� ���� ���ϰ� ���α׷��� ������ �� �ֵ��� ���� ���̺귯�� �Դϴ�.
  ������ ���� ������� �������̽� ���̺귯���� �����ϰ� �������
  �ȵ���̵�� ���̺귯���� TWAPI_A01�� Ŭ������ �Լ��� ����ϰ� �����Ͽ�
  Win32 ���α׷��� �ȵ���̵� �۰��� �ҽ� ������ ���ϰ� �� �� �ֵ��� �߽��ϴ�.
  �� ���̺귯���� ���� �������� �߰��ϰų� �������� �ʿ��ϴٸ� �Ʒ��� �� ���Ϸ�
  �ǰ� �����ֽðų� ��α׿� ��۷� �����ּ���.
  �̸��� : tipsware@naver.com
  ��α� : http://blog.naver.com/tipsware
  
  ���̺귯�� ���� : 2017�� 11�� 2�� ( �輺�� )
  ���̺귯�� ���� ������Ʈ : 2017�� 12�� 14�� ( �輺�� )

  �׸��� �� ���̺귯���� ����ϴ� ������� ���ϰ� ����� �� �ֵ��� 
  �ʿ��� ���̺귯���� �߰��ϴ� ��ó���� �ڵ带 ����ϴ�.

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

// Direct2D�� ����ϱ� ���� ��� ���ϰ� ���̺귯�� ������ ���Խ�Ų��.
#include <d2d1.h>
#include <Wincodec.h>     // IWICImagingFactory�� ����ϱ� ���ؼ� �߰�

// Direct2D���� ���� ����� ���� �����̽��� ������ �� �ֵ��� �����Ѵ�.
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

// Direct2D�� �����ϴ� ���� ��ü�� �����ϴ� Factory ��ü
extern ID2D1Factory *gp_factory;
// Windows 8.1 �̻��� �ü������ ����ϱ� ���� Factory ��ü
extern ID2D1Factory1 *gp_factory_ext;
// WIC(Windows Imaging Component)���� ��ü�� �����ϱ� ���� Factory ��ü
extern IWICImagingFactory *gp_wic_factory;

// TW_String Ŭ������ ���ڿ��� ���ϰ� ������ �� �ֵ��� ���� Ŭ�����Դϴ�. �� Ŭ������ ���ڿ� ó���� ���� ������ �ϱ� 
// ���ؼ� ���ڿ��� ���̸� ���� ����ϴ� ��찡 ������ �� �� ����ϴ� ���ڿ��� ���̴� ���� ���ڿ��� ���̸� �ǹ��մϴ�. 
// ���� ���, �� ��ü�� "abc" ���ڿ��� ������ ������ ���ڿ��� ���̰� ����� m_length���� 3�� ���� �˴ϴ�. �׸��� ���ڿ�
// a_length�� ����Ҷ��� �� ��Ģ�� �����ϰ� ����˴ϴ�.
class TW_String
{
protected:
	wchar_t *mp_string;   // �� ��ü�� �����ϴ� ���ڿ��� �ּ�
	int m_length;         // �� ��ü�� �����ϴ� ���ڿ��� ���� ("abc"�̸� 3�� ����)

public:
	TW_String();  // �⺻ ������
	TW_String(const char *ap_string);
	TW_String(const wchar_t *ap_string);
	TW_String(const TW_String &ar_string);  // ���� ������

	virtual ~TW_String();

	void InitialObject(const wchar_t *ap_string);  // ��ü �ʱ�ȭ �Լ�
	inline int GetLength() { return m_length; }   // ���ڿ��� ���̸� �˷��ִ� �Լ�
	inline const wchar_t *GetString() { return mp_string; } // ���ڿ��� ������ �˷��ִ� �Լ�
	void RemoveString();                           // �� ��ü�� �����ϴ� ���ڿ��� �����Ѵ�.

	// ���޵� ���ڿ��� �����ؼ� �����Ѵ�.
	void SetString(const wchar_t *ap_string);
	// ���޵� ���ڿ��� �ּҸ� �״�� ����Ѵ�. (���縦 �����ʰ� ������ �״�� �����)
	void AttachString(wchar_t *ap_string, int a_length);
	// ����� ���ڿ��� �ٸ� ���ڿ��� �߰��ϴ� ��쿡 ����Ѵ�.
	// ���� ���, "abc"�� ����Ǿ� �ִµ� AddString("def", 3); �̶�� ����ϸ� "abcdef"�� �˴ϴ�.
	void AddString(const wchar_t *ap_string, int a_length);

	// ���޵� �ƽ�Ű ���ڿ��� �����ڵ�� �����Ͽ� Ŭ���� ���ο� �����Ѵ�. ��ȯ���� ��ȯ�� ������ �����̴� (wcslen ���� ��ġ�Ѵ� )
	int AsciiToUnicode(char *ap_src_string);
	// ���޵� �ƽ�Ű ���ڿ��� �����ڵ�� �����Ͽ� ap_dst_string�� ��ȯ�Ѵ�. ��ȯ���� ��ȯ�� ������ �����̴� (wcslen ���� ��ġ�Ѵ� )
	// ap_dest_string �� NULL�� ����ϸ� ��ȯ�������� ���ڿ� ���̸� �˾Ƴ��� �ִ�.
	static int AsciiToUnicode(char *ap_src_string, wchar_t *ap_dest_string);
	// Ŭ������ �����ϴ� ���ڿ��� UTF8 �������� �����Ͽ� ap_dest_string�� ��ȯ�Ѵ�. ��ȯ���� UTF8 ���ڿ��� ���� �޸� ũ���̴�.
	// ap_dest_string �� NULL�� ����ϸ� ��ȯ�������� ���ڿ� ���̸� �˾Ƴ��� �ִ�.
	int UnicodeToUTF8(char *ap_dest_string);
	// ap_src_string ���ڿ��� UTF8 �������� �����Ͽ� ap_dest_string�� ��ȯ�Ѵ�. ��ȯ���� UTF8 ���ڿ��� ���� �޸� ũ���̴�.
	// ap_dest_string �� NULL�� ����ϸ� ��ȯ�������� ���ڿ� ���̸� �˾Ƴ��� �ִ�.
	static int UnicodeToUTF8(wchar_t *ap_src_string, char *ap_dest_string);
	// Ŭ������ �����ϴ� ���ڿ��� �ƽ�Ű�ڵ�� �����Ͽ� ap_dst_string�� ��ȯ�Ѵ�. ��ȯ���� ��ȯ�� ������ �����̴� (strlen ���� ��ġ�Ѵ� )
	// ap_dest_string �� NULL�� ����ϸ� ��ȯ�������� ���ڿ� ���̸� �˾Ƴ��� �ִ�.
	int UnicodeToAscii(char *ap_dest_string);
	// ���޵� �����ڵ� ���ڿ��� �ƽ�Ű�ڵ�� �����Ͽ� ap_dst_string�� ��ȯ�Ѵ�. ��ȯ���� ��ȯ�� ������ �����̴� (strlen ���� ��ġ�Ѵ� )
	// ap_dest_string �� NULL�� ����ϸ� ��ȯ�������� ���ڿ� ���̸� �˾Ƴ��� �ִ�.
	static int UnicodeToAscii(wchar_t *ap_src_string, char *ap_dest_string);

	// printf�� ���� ������ ����ϰ� ���� �� ����ϴ� �Լ�. �� �Լ��� ����� �ش��ϴ� ���ڿ��� ȭ�鿡 ��µ��� �ʰ�
	// �� ��ü�� ���ڿ�(mp_string)�� ����˴ϴ�.
	int Format(const wchar_t *ap_format, ...);

	// ���� ������ �����ε� str = L"test";
	void operator=(const TW_String& ar_string);
	// ���ڿ� Ȯ��(append)�� ���� ������ �����ε�
	void operator+=(const TW_String& ar_str);
	// ���ڿ��� ���ϱ� ���� ������ �����ε�
	friend TW_String operator+(const TW_String& ar_str1, const TW_String& ar_str2)
	{
		TW_String str = ar_str1;
		str.AddString(ar_str2.mp_string, ar_str2.m_length);
		return str;
	}
	// ���ڿ� �񱳸� ���� ������ �����ε� (������ Ȯ���ϱ� ���� ������)
	friend char operator==(const TW_String& ar_str1, const TW_String& ar_str2)
	{
		if (ar_str1.m_length != ar_str2.m_length) return 0;
		return memcmp(ar_str1.mp_string, ar_str2.mp_string, ar_str1.m_length << 1) == 0;
	}
	// ���ڿ� �񱳸� ���� ������ �����ε� (�ٸ��� Ȯ���ϱ� ���� ������)
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

// ���� ����Ʈ�� �����ϰ� �� ����� ����
struct node_ext {
	// ����� ���� �����͸� �����ϱ� ���� ����� ������
	void *p_object;
	// ����� ���� ��带 ����Ű�� ������
	node_ext *p_next;
};

class TW_StockListExt;

class TW_ListExt
{
private:
	// ���� ����Ʈ�� ó���� ���� ����Ű�� ������.
	node_ext *mp_head, *mp_tail;
	// ���� ����Ʈ�� ��ϵ� ����� ������ ������ �ִ� ����.
	int m_node_ext_count;

	// ����ڰ� ����ϴ� �޸𸮸� �����Ҷ� TW_ListExt Ŭ������ ���� ȣ��� �Լ��� �ּҸ� ������ ������
	void(*mp_delete_user_data)(void *ap_object);

public:
	// ��ü ������. ����ڰ� ������ ����� ���� �����͸� ��Ȯ�ϰ� �����ϱ� ���ؼ� �� ��忡 �����
	// �����͸� �����Ҷ� ȣ���� �Լ��� ������ �������� �Ѱ� �޴´�.
	// ( Example ) 
	//
	//    void DeleteUserData(void *parm_object)
	//    {
	//        UserData *p = (UserData *)parm_object;  // �ڽ��� �Ҵ��� ������ ������ ��ȯ�Ѵ�.
	//
	//        delete[] p->p_name; // �ش� ��ü�� ����ü���� �߰������� �Ҵ��� �޸𸮰� �ִٸ� �����Ѵ�.
	//        delete p; // �ش� ��忡 �Ҵ�� �޸𸮸� �����Ѵ�.
	//    }
	//
	//    TW_ListExt my_list(DeleteUserData);
	//
	// ���� NULL�� ���ڷ� �ѱ�� ���, TW_ListExt Ŭ������ �ڽ��� �ı��ɶ� ��常 �����ϰ� ��忡
	// ����� �����ʹ� �״�� �д�. ������ �����ϴ� �������� ������ ���Ḯ��Ʈ�� �����Ҷ��� ������
	// �������� �ʵ��� NULL �� �Ѱܼ� ����ϸ� �ȴ�.
	TW_ListExt(void(*afp_delete_user_data)(void *ap_object) = NULL);
	// ��ü �ı���
	~TW_ListExt();

	// ����ڰ� ����� �޸𸮸� �ı��Ҷ� ȣ���� �Լ��� ����
	void ChangeDeleteUserData(void(*afp_delete_user_data)(void *ap_object));
	// ���� ����Ʈ�� �ʱ�ȭ�ϴ� �Լ�
	void InitListObject();

	// [ ���ο� ��带 �߰��ϴ� �Լ��� ]

	// AddObject, AddObjectByIndex �Լ��� ����ڰ� ������ �޸𸮸� �״�� ��忡 �����ؼ� ����ϱ� ������ 
	// �Ʒ��� ���� �ΰ��� ���·� ����ϴ°� ������ �߻��ϴ� �����ؾ� �մϴ�.
	// ����Ʈ�� ���� ���ο� ��带 �߰��ϴ� �Լ�. 
	//
	//  1. ���� ������ �ּҸ� �Ѱܼ��� �ȵ˴ϴ�.
	//
	//    UserData data;
	//    m_my_list.AddObject(&data);
	//    (����) data ������ ���������̱� ������ �Լ��� ����ÿ� �޸𸮿��� ������� �˴ϴ�. ������ �� �����
	//           m_my_list�� ���� ���ϱ� ������ �ش� �ּҸ� �״�� ����ϰ� �Ǿ� ������ �߻��ϰ� �˴ϴ�.
	//
	//  2. �����Ҵ�� �ּҶ�� �ص� TW_ListExt�� �Ѱ��� �ּҴ� TW_ListExt�� �����ϱ� ���� �����ϸ� ������ �߻��մϴ�.
	//
	//    UserData *p_data = new UserData;
	//    m_my_list.AddObject(p_data);
	//    delete p_data;  <-- (����) �̷��� ���� �����ع����� m_my_list �� ���޵� �ּҰ� ������ �޸𸮸� ����Ű�� �Ǿ�
	//                                ������ �߻��մϴ�.
	//

	// ����Ʈ�� ���� ���ο� ��带 �߰��ϴ� �Լ�. 
	void AddObject(void *parm_object);
	// ���ϴ� ��ġ�� ��带 �߰��ϴ� �Լ�, 0 �� ����ϸ� ����Ʈ�� ���� ù ��ġ�� ��尡 �߰��ȴ�.
	// �߰��� ��ġ�� �߸� �����ϸ� ��� �߰��� �����ϰ� -1 ���� ��ȯ�Ѵ�.
	char AddObjectByIndex(int parm_index, void *parm_object);
	node_ext *AddObjectAfterThisNode(node_ext *parm_this_node, void *parm_p_object);

	// [ ��带 �����ϴ� �Լ��� ]

	// DeleteNode, DeleteAllObject, DeleteNodeByIndex �Լ��� �ι�° ���ڷ� ���� parm_delete_object_flag ������
	// ��忡 ����� ����� �����͸� �����Ұ����� ���θ� �����ϳ���. 
	// ( 0 -> ��常 ����, 1 -> ���� ����� ������ ��� ����)

	// ����Ʈ�� ��ϵ� ���� �߿��� parm_node�� ������ ��带 ã�Ƽ� �ش� ��带 �����Ѵ�.
	char DeleteNode(node_ext *parm_node, char parm_delete_object_flag = 1);
	// ����Ʈ�� ��ϵ� ���� �߿��� parm_object�� ������ ������ ������ �ִ� ��带 ã�Ƽ� �ش� ��带 �����Ѵ�.
	char DeleteNode(void *parm_object, char parm_delete_object_flag = 1);
	// ����Ʈ�� ��ϵ� ��� ��带 �����Ѵ�.
	void DeleteAllNode(char parm_delete_object_flag = 1);
	// ����ڰ� ������ ��ġ�� �ִ� ��带 ã�Ƽ� �ش� ��带 �����Ѵ�.
	char DeleteNodeByIndex(int parm_index, char parm_delete_object_flag = 1);

	// [ ��带 �˻��ϴ� �Լ��� ]

	// ���Ḯ��Ʈ���� ������ ��ġ�� �ִ� ����� �ּҸ� ��´�.
	node_ext *GetNodeByIndex(int parm_index);
	// ���Ḯ��Ʈ���� ������ ��ġ�� �ִ� ��尡 ����Ű�� �����͸� ��´�.
	void *GetObjectByIndex(int parm_index);
	// ���Ḯ��Ʈ�� ��ϵ� �����߿��� parm_object�� ��õ� �ּҿ� ������ �ּҸ� ���� �����Ͱ� �ִ���
	// ã�Ƽ� �� ��ġ�� ��ȯ�Ѵ�. ( -1 �̸� �˻��� �����ߴٴ� ���̴�. )
	int FindNodeIndexByObject(void *parm_object);

	// ���� ó�� ��ġ�� �ִ� ����� �ּҸ� ��´�.
	node_ext *GetHeadNode();
	// ������ ����� ������ �ִ� ��带 ��´�.
	node_ext *GetNextNode(node_ext *parm_current_node);
	// ���� ������ ��ġ�� �ִ� ����� �ּҸ� ��´�.
	node_ext *GetTailNode();

	void CloneTipsListExt(TW_ListExt *parm_src_list, char parm_delete_src_list = 1);
	void CloneTipsListExt(TW_StockListExt *parm_src_list);

	// ���Ḯ��Ʈ�� ��ϵ� ����� ������ ��´�.
	int GetNodeCount();
};

class TW_StockListExt
{
private:
	// ���� ����Ʈ�� ó���� ���� ����Ű�� ������.
	node_ext *mp_head, *mp_tail;
	// ���� ����Ʈ�� ��ϵ� ����� ������ ������ �ִ� ����.
	int m_node_ext_count;

	// �����ؼ� �̸� �Ҵ��� ����� �ּ�, �Ҵ� ������ �ּ��� ��ġ
	node_ext *mp_fixed_node_memory, *mp_alloc_node_pos;
	// �̸� �Ҵ��� ����� �ִ� ����
	unsigned int m_max_fixed_node_count;

	// ����ڰ� ����ϴ� �޸𸮸� �����Ҷ� TW_StockListExt Ŭ������ ���� ȣ��� �Լ��� �ּҸ� ������ ������
	void(*mp_delete_user_data)(void *parm_object);

public:
	// �� ��ü�� ����� ����� �ִ� ������ ��� ������ ����� �Լ��� �����Ѵ�.
	// ( parm_delete_user_data �� ���ؼ��� TW_ListExt::TW_ListExt �� ���� )
	TW_StockListExt(unsigned int parm_max_fixed_node_count, void(*parm_delete_user_data)(void *parm_object) = NULL);
	// ��ü �ı���
	~TW_StockListExt();

	// ����ڰ� ����� �޸𸮸� �ı��Ҷ� ȣ���� �Լ��� ����
	void ChangeDeleteUserData(void(*parm_delete_user_data)(void *parm_object));
	// ���� ����Ʈ�� �ʱ�ȭ�ϴ� �Լ�
	void InitListObject();

	// ����Ʈ�� ���� ���ο� ��带 �߰��ϴ� �Լ�. ( TW_ListExt::AddObject ���ǻ��� ���� )
	void AddObject(void *parm_object);
	// ���ϴ� ��ġ�� ��带 �߰��ϴ� �Լ�, 0 �� ����ϸ� ����Ʈ�� ���� ù ��ġ�� ��尡 �߰��ȴ�.
	// �߰��� ��ġ�� �߸� �����ϸ� ��� �߰��� �����ϰ� -1 ���� ��ȯ�Ѵ�.
	char AddObjectByIndex(int parm_index, void *parm_object);
	node_ext *AddObjectAfterThisNode(node_ext *parm_this_node, void *parm_p_object);

	// [ ��带 �����ϴ� �Լ��� ]

	// DeleteNode, DeleteAllObject, DeleteNodeByIndex �Լ��� �ι�° ���ڷ� ���� parm_delete_object_flag ������
	// ��忡 ����� ����� �����͸� �����Ұ����� ���θ� �����ϳ���. 
	// ( 0 -> ��常 ����, 1 -> ���� ����� ������ ��� ����)

	// ����Ʈ�� ��ϵ� ���� �߿��� parm_node�� ������ ��带 ã�Ƽ� �ش� ��带 �����Ѵ�.
	char DeleteNode(node_ext *parm_node, char parm_delete_object_flag = 1);
	// ����Ʈ�� ��ϵ� ��� ��带 �����Ѵ�.
	void DeleteAllNode(char parm_delete_object_flag = 1);
	// ����ڰ� ������ ��ġ�� �ִ� ��带 ã�Ƽ� �ش� ��带 �����Ѵ�.
	char DeleteNodeByIndex(int parm_index, char parm_delete_object_flag = 1);

	// [ ��带 �˻��ϴ� �Լ��� ]

	// ���Ḯ��Ʈ���� ������ ��ġ�� �ִ� ����� �ּҸ� ��´�.
	node_ext *GetNodeByIndex(int parm_index);
	// ���Ḯ��Ʈ���� ������ ��ġ�� �ִ� ��尡 ����Ű�� �����͸� ��´�.
	void *GetObjectByIndex(int parm_index);
	// ���Ḯ��Ʈ�� ��ϵ� �����߿��� parm_object�� ��õ� �ּҿ� ������ �ּҸ� ���� �����Ͱ� �ִ���
	// ã�Ƽ� �� ��ġ�� ��ȯ�Ѵ�. ( -1 �̸� �˻��� �����ߴٴ� ���̴�. )
	int FindNodeIndexByObject(void *parm_object);

	// ���� ó�� ��ġ�� �ִ� ����� �ּҸ� ��´�.
	node_ext *GetHeadNode();
	// ������ ����� ������ �ִ� ��带 ��´�.
	node_ext *GetNextNode(node_ext *parm_current_node);
	// ���� ������ ��ġ�� �ִ� ����� �ּҸ� ��´�.
	node_ext *GetTailNode();

	// ���Ḯ��Ʈ�� ��ϵ� ����� ������ ��´�.
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
	char m_attached_flag;  // ��Ʈ�� ���� ��������
	HWND mh_wnd;           // ��Ʈ���� ������ �ڵ� ��
	COLORREF m_fill_color, m_text_color; // ��Ʈ���� ������ ���ڻ�
	COLORREF m_in_border_color, m_out_border_color; // ��Ʈ���� �׵θ� ��
	UINT m_control_id;  // ��Ʈ���� ID ��
	HFONT mh_font;  // ��Ʈ�ѿ� ����� �۲� �ڵ� ���� ������ ����

public:
	TW_CtrlWnd();           // ��ü ������
	virtual ~TW_CtrlWnd();  // ��ü �ı���

	inline HWND GetHandle() { return mh_wnd; }
	// �ܺο��� ������� ��Ʈ���� �� Ŭ������ �����ϴ� ��쿡 ���
	void Attach(HWND ah_attach_wnd);  
	// ��Ʈ���� ������ ������ �� ����ϴ� �Լ�
	void ChangeCtrlColor(COLORREF a_fill_color, COLORREF a_out_border_color, 
						 COLORREF a_in_border_color, COLORREF a_text_color);
	// ��Ʈ���� Owner Draw �Ӽ��� True�� ��쿡 ó���ϴ� �Լ�
	virtual void OnDrawItem(DRAWITEMSTRUCT *ap_dis);
	virtual INT32 OnCtrlColor(HDC ah_dc, HWND ah_wnd);

	// ��Ʈ���� �ٽ� �׸� �ʿ䰡 ������ ȣ���ϴ� �Լ�
	void Invalidate();
	// ��Ʈ�ѿ� �۲��� �����Ѵ�. a_auto_delete�� 1�� �Ѱ��ָ� �� �ڵ� ����
	// mh_font�� �����ߴٰ� ��Ʈ���� ���ŵɶ� �۲õ� ���� �����մϴ�.
	void SetFont(HFONT ah_font, UINT32 a_auto_delete = 1);
};

class TW_ButtonCtrl : public TW_CtrlWnd
{
public:
	TW_ButtonCtrl();   // ��ü ������
	~TW_ButtonCtrl();  // ��ü �ı���

	// ��Ʈ���� Owner Draw �Ӽ��� True�� ��쿡 ó���ϴ� �Լ�
	// ��ư�� �׸��� ���� ��������
	virtual void OnDrawItem(DRAWITEMSTRUCT *ap_dis);
};

#define EN_ENTER_KEYUP          1000

// Edit ��Ʈ�ѿ� ���Ӱ� ������ ������ ���ν���
LRESULT APIENTRY TW_EditProcForEnterKeyUp(HWND ah_wnd, UINT message_id, WPARAM wParam, LPARAM lParam);

class TW_EditWnd : public TW_CtrlWnd
{
protected:
	// Edit ��Ʈ���� ���� ����ϴ� ���ν����� �ּҸ� �����ϱ� ���� �Լ��� ������
	WNDPROC m_org_edit_proc;

public:
	TW_EditWnd();  // ��ü ������
	virtual ~TW_EditWnd(); // ��ü �ı���

    // �⺻ �Ӽ����� Edit ��Ʈ���� �����ϴ� �Լ�
	HWND Create(HWND ah_parent_wnd, INT32 a_sx, INT32 a_sy, INT32 a_ex, INT32 a_ey, INT32 a_ctrl_id, UINT32 a_style = 0);
	// ����Ŭ������ �ϴ� �Լ�. �⺻���� �����Ǵ� TW_EditProcForEnterKeyUp ���������� 
	// ����Ű�� üũ�ؼ� EN_ENTER_KEYUP �޽����� �����ϴ� ����� �߰��Ǿ� �ִ�.
	void SubclassWnd(WNDPROC a_new_proc = TW_EditProcForEnterKeyUp);
	// WM_CTRLCOLOREDIT �޽������� ����ϴ� �Լ�
	virtual INT32 OnCtrlColor(HDC ah_dc, HWND ah_wnd);
	// Edit ��Ʈ�ѿ��� ���ڿ��� ���̸� ��� �Լ�
	UINT32 GetTextLen();
	// Edit ��Ʈ�ѿ��� ���ڿ��� ��� �Լ�
	UINT32 GetText(TW_String &r_str);
	// Edit ��Ʈ�ѿ��� ���ڿ��� ��� �Լ�
	UINT32 GetText(wchar_t *ap_string, UINT32 a_max_length);
	// Edit ��Ʈ�ѿ� ���ڿ��� �����ϴ� �Լ�
	void SetText(const wchar_t *ap_string);
};

class TW_ListBoxWnd : public TW_CtrlWnd
{
public:
	TW_ListBoxWnd(); // ��ü ������
	virtual ~TW_ListBoxWnd(); // ��ü �ı���

	// �⺻ �Ӽ����� ListBox�� �����ϴ� �Լ�
	HWND Create(HWND ah_parent_wnd, INT32 a_sx, INT32 a_sy, INT32 a_ex, INT32 a_ey, INT32 a_ctrl_id, UINT32 a_style = LBS_STANDARD);
	// Ȯ�� �Ӽ��� ����ؼ� ListBox�� �����ϴ� �Լ�
	HWND CreateEx(HWND ah_parent_wnd, INT32 a_sx, INT32 a_sy, INT32 a_ex, INT32 a_ey, INT32 a_ctrl_id, UINT32 a_style = 0, UINT32 a_ex_style = WS_EX_STATICEDGE);
	// ���ڿ��� ListBox�� �߰��ϴ� �Լ�, LBS_SORT �ɼ��� ������ �ִٸ� ������������ �����ؼ� �߰��Ѵ�.
	INT32 AddString(const wchar_t *ap_string, UINT8 a_auto_select = 1);
	// ���ڿ��� ����ڰ� ������ ��ġ�� �߰��ϴ� �Լ�
	INT32 InsertString(INT32 a_index, const wchar_t *ap_string, UINT8 a_auto_select = 1);
	// ����ڰ� ������ �׸� �߰��� ���ڿ��� ���̸� ��´�.
	INT32 GetTextLength(INT32 a_index);
	// ����ڰ� ������ �׸񿡼� ���ڿ��� �����ؼ� �����´�.
	INT32 GetText(INT32 a_index, wchar_t *ap_string, int a_max_len);
	// ����ڰ� ������ �׸񿡼� ���ڿ��� �����ؼ� �����´�. 
	INT32 GetText(INT32 a_index, TW_String &r_str);
	// ListBox�� Ư�� �׸��� �����Ѵ�.
	void SetCurSel(INT32 a_index);
	// ListBox���� � �׸��� ���õǾ� �ִ��� ��ġ ���� ��´�.
	INT32 GetCurSel();
	// ListBox�� �߰��� �׸��� ������ ��´�.
	INT32 GetCount();
	// ListBox�� �߰��� ��� �׸��� �����Ѵ�.
	void ResetContent();
	// ������ �׸� �߰� 4����Ʈ ������ �����Ѵ�. (������ ����)
	void SetItemDataPtr(INT32 a_index, void *ap_data);
	// ������ ��� InsertString�� ����ϰ� SetItemDataPtr �Լ��� ����ؾ� ������
	// �� �Լ��� ����ϸ� �� �Լ��� ����� ���ÿ� �����մϴ�.  (������ ����)
	void SetItemDataPtrEx(INT32 a_index, const wchar_t *ap_string, void *ap_data, UINT8 a_auto_select = 1);
	// �׸� �߰��� 4����Ʈ �޸� ���� ��´�. (������ ����)
	void *GetItemDataPtr(INT32 a_index);
	// ������ �׸� �߰� 4����Ʈ ������ �����Ѵ�. (���� ����)
	void SetItemData(INT32 a_index, INT32 a_data);
	// ������ ��� InsertString�� ����ϰ� SetItemDataPtr �Լ��� ����ؾ� ������
	// �� �Լ��� ����ϸ� �� �Լ��� ����� ���ÿ� �����մϴ�.  (���� ����)
	void SetItemDataEx(INT32 a_index, const wchar_t *ap_string, INT32 a_data);
	// �׸� �߰��� 4����Ʈ �޸� ���� ��´�. (���� ����)
	INT32 GetItemData(INT32 a_index);
	// ListBox�� �߰��� ���ڿ����� ���ϴ� ���ڿ��� �˻��Ѵ�. �� �Լ��� ����ϸ�
	// �ڽ��� ã�� ���� ���ڿ��� 'abc'�� ��쿡 'abc'�Ӹ� �ƴ϶� 'abcdef'�� ����
	// �κ������� ��ġ�ϴ� ���ڿ��� ã�� �� �ֽ��ϴ�.
	INT32 FindString(INT32 a_index, const wchar_t *ap_string);
	// ListBox�� �߰��� ���ڿ����� ���ϴ� ���ڿ��� �˻��Ѵ�. �� �Լ��� ��Ȯ�ϰ�
	// ��ġ�ϴ� ���ڿ��� �˻��մϴ�.
	INT32 FindStringExact(INT32 a_index, const wchar_t *ap_string);

	virtual void OnDrawItem(DRAWITEMSTRUCT *ap_dis);
	virtual INT32 OnCtrlColor(HDC ah_dc, HWND ah_wnd);
};

class TW_Wnd
{
protected:
	// �� ��ü�� ������ �������� �ڵ�
	HWND mh_wnd;
	// Direct2D���� �������� Ŭ���̾�Ʈ ������ �׸��� �׸� ��ü
	ID2D1HwndRenderTarget *mp_target;

	// Windows 8.1 �̻󿡼� ��� ������ �׸� �׸��� ��ɵ�!
	ID2D1Device *mp_device;
	ID2D1DeviceContext *mp_dc;
	ID2D1Bitmap1 *mp_dc_bitmap;
	IDXGISwapChain1 *mp_sc;
	DXGI_PRESENT_PARAMETERS m_sc_parameters;
	TW_StockSolidBrushManager *mp_sb_man;

	// ���콺�� ���� ��ǥ�� �����ϴ� ����
	POINT m_prev_pos;
	// ���� ���콺 ��ư�� Ŭ�� ���ο� Ŭ���̾�Ʈ ����(Client Region)�� Ŭ���ؼ� �̵��� ������ ����.
	unsigned char m_is_clicked, m_is_cr_moving;

public:
	TW_Wnd();  // ��ü ������
	virtual ~TW_Wnd();  // ��ü �ı���

    // ��ü�� �����ϴ� mh_wnd ���� ��ų� ���ο� �ڵ� ���� �����ϴ� �Լ�
	HWND GetHandle() { return mh_wnd; }
	void SetHandle(HWND ah_wnd) { mh_wnd = ah_wnd; }
	// �����츦 ������ �� ����ϴ� �Լ�
	void Create(const wchar_t *ap_title_name, int a_x = 50, int a_y = 50,
		int a_width = 500, int a_height = 300, DWORD a_style = WS_OVERLAPPEDWINDOW);
	void CreateEx(const wchar_t *ap_title_name, int a_x = 50, int a_y = 50,
		int a_width = 500, int a_height = 300, DWORD a_style = WS_OVERLAPPEDWINDOW, 
		DWORD a_ex_style = WS_EX_TOPMOST | WS_EX_ACCEPTFILES);
	// �����츦 �ı��� �� ����ϴ� �Լ�
	void DestoryWindow();
	// �����츦 ������ ������� �ʰ� ��� �����ϵ��� �ϴ� �Լ�
	void UpdateWindow();
	// �����츦 ȭ�鿡 ��� ��½�ų �������� �����ϴ� �Լ�
	void ShowWindow(int a_show_type = SW_SHOW);
	// �������� Ŭ���̾�Ʈ ������ �������� ������ ũ�⸦ �����ϴ� �Լ�
	void ResizeWindow(int a_width, int a_height);
	// ������ ȭ���� ��ȿȭ ���Ѽ� WM_PAINT �޽����� �߻���Ű�� �Լ�
	void Invalidate();
	// Ÿ�̸Ӹ� ����ϴ� �Լ�
	void SetTimer(UINT a_timer_id, UINT a_elapse_time);
	// Ÿ�̸Ӹ� �����ϴ� �Լ�
	void KillTimer(UINT a_timer_id);
	void GetClientRect(D2D1_RECT_F *ap_rect);
	// �ڽ��� �����츦 Ÿ�������� �����մϴ�.
	void ChangeEllipticWnd(int a_x_radius, int a_y_radius);
	inline TW_StockSolidBrushManager *GetSbMan()
	{
		return mp_sb_man;
	}

	// WM_PAINT �޽����� �߻��� �� �۾��� �Լ�. �� �Լ��� �Ļ� Ŭ�������� ��κ� ������ ��
	virtual void OnPaint();
	// WM_CREATE �޽����� �߻��� �� �۾��� �Լ�.
	virtual int OnCreate(CREATESTRUCT *ap_create_info);
	// WM_TIMER �޽����� �߻��� �� �۾��� �Լ�.
	virtual void OnTimer(UINT a_timer_id);
	// WM_COMMAND �޽����� �߻��� �� �۾��� �Լ�.
	virtual void OnCommand(INT32 a_ctrl_id, INT32 a_notify_code, HWND ah_ctrl);
	// WM_DESTROY �޽����� �߻��� �� �۾��� �Լ�.
	virtual void OnDestroy();
	// WM_LBUTTONDOWN �޽����� �߻��� �� �۾��� �Լ�.
	virtual void OnLButtonDown(int a_flag, POINT a_pos);
	// WM_LBUTTONUP �޽����� �߻��� �� �۾��� �Լ�.
	virtual void OnLButtonUp(int a_flag, POINT a_pos);
	// WM_MOUSEMOVE �޽����� �߻��� �� �۾��� �Լ�.
	virtual void OnMouseMove(int a_flag, POINT a_pos);
	// WM_LBUTTONDBLCLK �޽����� �߻��� �� �۾��� �Լ�.
	virtual void OnLButtonDblClk(int a_flag, POINT a_pos);
	// WM_RBUTTONDOWN �޽����� �߻��� �� �۾��� �Լ�.
	virtual void OnRButtonDown(int a_flag, POINT a_pos);
	// WM_RBUTTONUP �޽����� �߻��� �� �۾��� �Լ�.
	virtual void OnRButtonUp(int a_flag, POINT a_pos);
	// WM_MOUSEWHEEL �޽����� �߻��� �� �۾��� �Լ�.
	virtual int OnMouseWheel(unsigned short a_flag, short a_z_delta, POINT a_pos);
	// WM_DROPFILES �޽����� �߻��� �� �۾��� �Լ�.
	virtual void OnDropFiles(HANDLE ah_drop_info);
	// WM_CTLCOLORLISTBOX �޽����� �߻��� �� �۾��� �Լ�.
	virtual int OnCtrlColorListBox(HDC ah_dc, HWND ah_wnd);
	// WM_CTLCOLOREDIT �޽����� �߻��� �� �۾��� �Լ�.
	virtual int OnCtrlColorEdit(HDC ah_dc, HWND ah_wnd);
	// WM_DRAWITEM �޽����� �߻��� �� �۾��� �Լ�.
	virtual INT32 OnDrawItem(INT32 a_ctrl_id, DRAWITEMSTRUCT *ap_dis);
	// WM_MEASUREITEM �޽����� �߻��� �� �۾��� �Լ�.
	// a_is_ctrl : 0 -> Menu, 1 -> ListBox & ComboBox
	virtual INT32 OnMeasureItem(UINT32 a_is_ctrl, MEASUREITEMSTRUCT *ap_mis);
	// WM_COPYDATA �޽����� �߻��� �� �۾��� �Լ�.
	virtual void OnCopyData(HWND ah_send_wnd, COPYDATASTRUCT *ap_copy_data);
	// TW_Wnd Ŭ������ �������� ���� ������ �޽����� ó���ϴ� ��쿡 ����ϴ� �Լ�
	virtual LRESULT UserMessageProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

class TW_WinApp
{
protected:
	TW_Wnd *mp_wnd;    // ���� ������ ��ü�� �ּҸ� ����
	HINSTANCE mh_instance;  // WinMain�� ���޵Ǵ� ù ��° ���ڰ� ����
	wchar_t m_class_name[32]; // '������ Ŭ����' �̸��� ������ ����
	// WinMain �Լ��� ���� ��ȯ���� WM_QUIT�޽����� wParam ������ ����ϱ�
	// ���ؼ� �� ���� ������ ����
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