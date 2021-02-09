//STLデバッグ機能をOFFにする
#define _SECURE_SCL (0)
#define _HAS_ITERATOR_DEBUGGING (0)

#include <Windows.h>
#include "WindowCreateClass.h"

HWND CwindowCreate::m_hWnd;	//ウィンドウハンドル
int CwindowCreate::m_width; //ウィンドウの横幅
int CwindowCreate::m_height;//ウィンドウの縦幅

//NewWindowメソッド
//引数 int :ウィンドウの横幅
//引数2int :ウィンドウの縦幅
//引数3wchar_t* :ウィンドウステータス・タイトル名
//引数4　HINSTANCE：インスタントハンドル
//戻り値　無し
//内容：ウィンドウを作成します。
void CwindowCreate::NewWindow(int w, int h, wchar_t* name, HINSTANCE hInstance)
{
	m_width = w;
	m_height = h;

	int width = m_width + GetSystemMetrics(SM_CXDLGFRAME) * 2;
	int height = m_height + GetSystemMetrics(SM_CYDLGFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION);

	//ウィンドウ作成
	if (!(m_hWnd = CreateWindow(name, name,
		WS_OVERLAPPEDWINDOW&(WS_MAXIMIZEBOX | WS_SIZEBOX),
		CW_USEDEFAULT, 0, width, height, 0, 0, hInstance, 0)))
	{
		return;
	}

	ShowWindow(m_hWnd, SW_SHOW);
}