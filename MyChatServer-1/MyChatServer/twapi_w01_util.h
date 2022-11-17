#ifndef _TWAPI_W01_UTIL_H_
#define _TWAPI_W01_UTIL_H_

#include <ShlDisp.h>

class TW_File
{
protected:
	HANDLE mh_file;

public:
	TW_File();
	~TW_File();

	char IsOpenFile();
	HANDLE GetHandle();
	__int64 GetFileSize();

	int OpenFile(const wchar_t *ap_path, unsigned char a_is_read = 1, unsigned char a_is_write = 0);

	void WriteStream(const void *ap_data, unsigned int a_data_size);
	int ReadStream(void *ap_data, unsigned int a_max_size);

	void CloseFile();
};

class TW_TextFile
{
protected:
	FILE *mp_file;
	wchar_t m_internal_buffer[2048];
public:
	TW_TextFile();
	~TW_TextFile();

	FILE *GetPointer();
	__int64 GetFileSize();

	int OpenFile(const wchar_t *ap_path, const wchar_t *ap_open_flag);
	void CloseFile();

	int WriteFormat(const wchar_t *ap_format, ...);
	void WriteString(const wchar_t *ap_string, int a_length = -1);
	void WriteString(TW_String *ap_string);

	int ReadString(wchar_t *ap_string, int a_max_length, int a_remove_line_peed = 1);
	int ReadString(TW_String *ap_string, int a_remove_line_peed = 1);
};

struct FileItemInfo {
	// 0th bit : 0 -> file, 1 -> directory, 
	// 7th bit(0x80, 0x7F) : 1 -> 미처리 폴더, 0 -> 처리 완료된 폴더, 6th bit(0x40, 0xBF) : 1 -> 확장자 필터 걸림, 0 -> 확장자 필터 안걸림
	UINT8 state;
	UINT8 depth_count;
	UINT8 ext_memory_size;     // .을 뺀 확장자의 길이가 메모리 크기로 저장되어 있다. "tipsware_name.txt" 이라면 6이 저장되어 있음 ("." 과 "\0"은 포함 안함)
	UINT8 ext_start_pos;
	UINT16 item_name_length;   // \0를 포함한 크기가 저장됨 "test" 이면 5가 저장되어 있음
	UINT16 item_full_length;   // \0를 포함한 크기가 저장됨 

	wchar_t *p_item_name;
	node_ext *p_parent;
};

typedef void(*T_DELETE_FILE_DATA)(void *ap_object);

class TDirectoryManager
{
private:
	UINT8 m_use_ext_info;
	INT32 m_base_path_length;  // m_base_path 문자열의 갯수가 들어있다 'c:\test' 이면 7이 저장되어 있다.

	TW_ListExt m_item_list;
	wchar_t m_base_path[MAX_PATH];

	void GetFilesFromDirectory(wchar_t *a_path, UINT16 a_length, INT32 a_depth, node_ext *ap_parent);

public:
	TDirectoryManager(INT8 a_use_ext_info = 1);
	~TDirectoryManager();

	// ad_data 의 경로를 구성합니다. ap_make_path는 MAX_PATH 크기이거나 ap_data->item_full_length + 1 크기여야 합니다.
	void GetFullPath(FileItemInfo *ap_data, wchar_t *ap_make_path);
	// ad_data 의 경로를 구성합니다. 하지만 m_base_path를 사용하지 않고 ap_dest_path 를 사용해서 구성합니다.
	// ap_make_path는 MAX_PATH 크기이거나 ap_data->item_full_length + a_dest_path_length - m_base_path_length + 1 크기여야 합니다.
	INT32 GetFullPath(wchar_t *ap_dest_path, INT32 a_dest_path_length, FileItemInfo *ap_data, wchar_t *ap_make_path);

	// 관리할 경로를 추가합니다. ap_item_path 에는 파일 또는 폴더 모두 가능하지만 폴더의 경우, 끝에 \가 포함되면 않된다
	int AddItem(wchar_t *ap_item_path, INT8 a_is_directory);
	// AddItem 함수로 지정한 폴더를 검색하여 폴더나 파일을 리스트에 추가합니다.
	void ScanDirectory();
	void ResetContent();

	// ap_dest_path로 TDirectoryManager가 가지고 있는 아이템들을 복사한다. ( 폴더와 파일 모두 복사한다 )
	// CopyItemToDestPath 전달되는 dest 경로는 MAX_PATH 크기로 할당된 메모리이여야 한다. 그리고 경로의 끝에는 \가 포함되어 있어야 한다
	// a_dest_path_length 에는 wcslen(ap_dest_path)의 값이 들어있다
	// a_use_ext_filter 를 사용하려면 CheckFileExt 함수를 먼저 호출해서 필터 결과를 만들어야 한다
	void CopyItemToDestPath(wchar_t *ap_dest_path, INT32 a_dest_path_length, INT8 a_use_ext_filter = 0);
	// 전달된 파일 확장자 목록과 일치하는 파일이 몇개나 있는지 체크하는 함수
	int CheckFileExt(INT32 a_ext_count, wchar_t **ap_ext_list, UINT8 *ap_ext_memory_size);
	// state 에 적용된 파일 확장자 필터 결과를 초기화한다
	void ResetFileExtFilter();
	TW_ListExt *GetItemList();
	wchar_t *GetBasePath();
	INT32 GetBasePathLength();
};



// 유니코드 파일을 생성한다.
int TWAPI_CreateUnicodeFile(const wchar_t *ap_file_path, char a_check_exist_file = 1);
// 사용자가 지정한 파일의 크기를 얻는다. 만약 파일이 없으면 0xFFFFFFFF 값을 반환한다.
unsigned int TWAPI_GetFileSize(wchar_t *ap_file_path);
// 내용이 비어있는 Zip 파일을 만든다.
void TWAPI_CreateZipFile(const wchar_t *ap_file_path, char a_is_overwrite = 0);
// 지정한 Zip 파일에 파일을 추가한다.
void TWAPI_AddFileToZip(wchar_t *ap_zip_path, wchar_t *ap_file_path);

// Zip 압축을 위한 엔진을 생성한다. (RST_TW_Zip.DLL, DSH_TW_Zip.DLL을 사용)
void TWAPI_CreateZipEngine();
// 자신이 구성한 파일 목록을 가지고 Zip 형식의 압축 파일을 만든다.
void TWAPI_AddFileToZipEngine(wchar_t *ap_zip_path, int a_file_count, char *(*afp_get_file_name)(int a_index, void *ap_data), void *ap_data);
// Zip 압축을 위해 생성했던 엔진을 제거한다.
void TWAPI_DestroyZipEngine();

// API 함수 사용시 발생한 오류 정보를 문자열로 얻을 때 사용
void TWAPI_GetLastErrorMessage(TW_String *ap_string);

// 지정한 경로를 생성한다. 예를 들어, 'c:\temp\tips\example'이라고 경로를 적으면
// C 드라이브에 temp 폴더를 생성하고 temp 폴더에 tips 폴더를 생성하고 tips 폴더에 example 폴더를 생성합니다.
// 물론 해당 폴더가 이미 존재한다면 해당 폴더의 생성은 무시됩니다.
void TWAPI_CreateDirectory(wchar_t *ap_path, int a_path_length);
// 지정한 경로가 존재하는지 체크한다. 존재하면 1, 존재하지 않으면 0을 반환한다.
int TWAPI_IsExistPath(const wchar_t *ap_path);
// "c:\temp\test.txt" 라고 지정하면 ".txt" 의 . 부분을 가르키는 주소를 반환한다. 
// 지정한 경로에 . 이 없다면 NULL을 반환한다.
const wchar_t *TWAPI_GetExtName(const wchar_t *ap_file_name);
// 파일명이 포함된 경로에서 경로명만 추출하여 새로운 메모리를 할당하여 복사해주는 함수
// "c:\temp\test.txt" 라고 지정하면 새로운 메모리를 생성하고 해당 메모리에 "c:\temp\"를 복사해준다. 
// 실패하면 NULL 을 반환한다. 그리고 new 연산자로 동적할당된 메모리 이기 때문에 반환된 주소는 반드시 삭제해야 한다.
// ap_create_path_length 에 변수를 넘기면 생성한 문자열의 길이를 넣어준다. "c:\temp\" 라면 8이 저장된다.
// 디렉토리로만 구성된 경로에서 마지막 디렉토리를 제외한 경로만을 구하고 싶다면 해당 경로의 끝에 \ 를 제거하고 넣으면 된다.
// 예를 들어, "c:\temp\tips"라고 경로를 넣으면 tips가 폴더라도 "c:\temp\"라고 경로를 만들어 줍니다.
wchar_t *TWAPI_GetPathExceptFileName(const wchar_t *ap_path_name, int *ap_create_path_length);
// 파일명이 포함된 경로에서 파일명이 시작하는 주소를 찾는 함수 
// "c:\temp\test.txt" 라고 지정하면 "test.txt" 이름의 시작위치에 해당주소를 반환한다.
// 디렉토리로만 구성된 경로에서 마지막 디렉토리의 이름을 구하고 싶다면 해당 경로의 끝에 \를 제거하고 넣으면 된다.
// 예를 들어, "c:\temp\tips"라고 경로를 넣으면 "tips"의 시작 주소를 반환한다.
const wchar_t *TWAPI_GetFileNameFromPath(const wchar_t *ap_path_name);
const char *TWAPI_GetFileNameFromPathA(const char *ap_path_name);
// 지정한 경로에 있는 모든 파일 및 폴더를 삭제한다.
// ap_path 로 전달되는 경로는 MAX_PATH 크기를 가진 메모리이여야 하며 \ 로 끝나야 한다. 
// a_path_length 에는 wcslen(ap_path) 값이 들어가야 한다 
int TWAPI_RemoveFiles(wchar_t *ap_path, unsigned int a_path_length);

// 현재 작업 경로를 기준으로 FindFirstFile 함수에서 사용할 경로 검색 문자열을 구성한다.
UINT32 TWAPI_MakeFindPath(wchar_t *ap_path, INT32 a_max_len, wchar_t *ap_filter = (wchar_t *)L"*.*", INT32 a_filter_len = 3);

// 이 함수는 FindFirstFile에서 사용할 검색 경로를 변경합니다. 예를 들어, ap_path에 'c:\tips\work\*.*' 형태의 
// 경로가 저장되어 있고 ap_dir_name에 '..'이라고 지정되면 'c:\tips\*.*'로 변경해주는 함수입니다.
// 그리고 인자에 사용된 문자열 길이는 모두 '\0'를 포함하지 않습니다. 또한 ap_path는 디렉토리를 추가했을 때
// 문제가 생기지 않도록 a_dir_len + 1만큼의 여유 공간이 있어야 합니다.
INT32 TWAPI_UpdateFilePath(wchar_t *ap_path, INT32 *ap_path_len, wchar_t *ap_dir_name, INT32 a_dir_len, 
							wchar_t *ap_filter = (wchar_t *)L"*.*", INT32 a_filter_len = 3);

#endif