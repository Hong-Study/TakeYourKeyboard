#ifndef _TWAPI_W01_GDI_H_
#define _TWAPI_W01_GDI_H_

// Brush 객체는 최대 100개까지만 관리한다.
#define MAX_STOCK_BRUSH          100

class TW_BrushManager
{
private:
	// 관리할 Brush 객체의 핸들 목록
	HBRUSH mh_brush[MAX_STOCK_BRUSH];
	// 관리할 Brush 객체의 색상 목록
	COLORREF m_brush_color[MAX_STOCK_BRUSH];
	// 현재 사용하고 있는 Brush 객체의 수
	int m_brush_count;

public:
	TW_BrushManager();
	~TW_BrushManager();

	// 관리하는 객체에서 a_color로 만들어진 Brush 객체가 있다면 해당 객체의 핸들을 
	// 반환하고 없으면 Brush 객체를 생성하고 생선된 Brush 객체의 핸들값을 반환한다.
	HBRUSH GetBrush(COLORREF a_color);
	// 관리하던 모든 Brush 객체를 제거한다.
	void DeleteAllBrush();
	// 현재 사용하고 있는 Brush 객체의 수를 반환한다.
	int GetCount() { return m_brush_count; }
};

// Pen 객체는 최대 100개까지만 관리한다.
#define MAX_STOCK_PEN   100

class TW_PenManager
{
private:
	// 관리할 Pen 객체의 핸들 목록
	HPEN mh_pen[MAX_STOCK_PEN];
	// 관리할 Pen 객체의 속성 목록
	COLORREF m_pen_color[MAX_STOCK_PEN];
	int m_pen_tick[MAX_STOCK_PEN];
	int m_pen_style[MAX_STOCK_PEN];
	// 현재 사용하고 있는 Pen 객체의 수
	int m_pen_count;

public:
	TW_PenManager();
	~TW_PenManager();

	// 관리하는 객체에서 a_color로 만들어진 Pen 객체가 있다면 해당 객체의 핸들을 
	// 반환하고 없으면 Pen 객체를 생성하고 생선된 Pen 객체의 핸들값을 반환한다.
	HPEN GetPen(int a_style, int a_tick, COLORREF a_color);
	// 관리하던 모든 Pen 객체를 제거한다.
	void DeleteAllPen();
	// 현재 사용하고 있는 Pen 객체의 수를 반환한다.
	int GetCount() { return m_pen_count; }
};

class TW_Image
{
protected:
	// Bitmap 객체의 핸들 값 저장
	HBITMAP mh_bitmap;
	// Bitmap 객체를 연결할 DC의 핸들 값 저장
	HDC mh_mem_dc;
	// Bitmap 객체의 폭과 높이. HBITMAP에서 얻을 수 있지만 
	// 매번 함수를 사용하는 것을 피하기 위해서 가지고 있음
	INT32 m_width, m_height;

public:
	TW_Image();           // 객체 생성자
	virtual ~TW_Image();  // 객체 파괴자

	// 사용자가 지정한 속성으로 비트맵을 생성한다.
	void CreateBitmap(INT32 a_width, INT32 a_height, UINT32 a_color_depth = 32, void *ap_pattern = NULL, INT8 a_create_dc_flag = 1);
	// 화면 속성으로 비트맵을 생성한다.
	void CreateScreenBitmap(INT8 a_create_dc_flag = 1);
	// 프로그램의 Resource 영역에 저장된 Bitmap을 가져와서 Bitmap 객체를 만든다.
	void LoadBitmapFromRC(INT32 a_resource_id, INT32 a_width, INT32 a_height, INT8 a_create_dc_flag = 1);
	// 프로그램의 Resource 영역에 저장된 Bitmap을 가져와서 Bitmap 객체를 만들고 핸들 값을 반환한다.
	static HBITMAP LoadBitmapFromRC(INT32 a_resource_id);
	// Bitmap 객체의 폭을 얻는다.
	inline INT32 GetWidth() { return m_width; }
	// Bitmap 객체의 높이를 얻는다.
	inline INT32 GetHeight() { return m_height; }
	// Bitmap 객체의 핸들 값을 얻는다.
	inline HBITMAP GetBitmap() { return mh_bitmap; }
	// Bitmap 객체와 연결된 DC 핸들 값을 얻는다.
	inline HDC GetDC() { return mh_mem_dc; }

	// 화면 이미지를 복사한다.
	void CaptureScreen();
	// TW_Image가 관리하는 Memory DC를 생성하는 함수. ah_base_dc에 NULL을 설정하면 
	// 현재 사용하는 출력장치와 호환되는 Memory DC를 생성한다.
	HDC CreateMemoryDC(HDC ah_base_dc = NULL);
	// TW_Image가 관리하고 있는 mh_mem_dc를 제거한다.
	void ReleaseDC();
	// 원하는 색상을 제거하고 Bitmap 객체를 출력하는 함수
	void DrawBitmap(HDC ah_dc, INT32 a_x, INT32 a_y, COLORREF a_remove_color);
	// Bitmap 객체를 그대로 출력하는 함수
	void DrawBitmap(HDC ah_dc, INT32 a_x, INT32 a_y);
};

extern TW_BrushManager *gp_stock_brush;
extern TW_PenManager *gp_stock_pen;


// 크기를 모르는 Bitmap의 패턴을 가져오는 경우에 사용함.
// 비트 패턴이 복사되어 반환된 메모리는 delete[]를 사용하여 반드시 직접 제거해야 합니다.
UINT32 *TWAPI_GetBitmapPattern(HBITMAP ah_bitmap, UINT32 *ap_width = NULL, UINT32 *ap_height = NULL,
							UINT16 *ap_color_bytes = NULL, UINT32 *ap_pattern_size = NULL);
// HFONT 형태의 글꼴을 생성하는 함수
HFONT TWAPI_CreateFont(const wchar_t *ap_font_name = L"굴림", INT32 ah_size = 12, INT32 ah_weight = FW_NORMAL);


#endif