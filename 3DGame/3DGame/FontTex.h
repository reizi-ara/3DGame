#pragma once
//入力に必要なヘッダー
#include <Windows.h>
#include <locale.h>
#include <Windows.h>
#include <memory>
#include <list>

using namespace std;

#include "DeviceCreate.h"
#include "Draw2DPolygon.h"

//文字識別クラス
class CCherClass
{
	public:
		//コンストラクタ
		CCherClass()
		{
			m_pc.reset();
			m_pTexture = nullptr;
		}
		//デストラクタ
		~CCherClass()
		{
			m_pc.reset();
			SAFE_RELEASE(m_pTexResView);
			SAFE_RELEASE(m_pTexture);
		}

		const wchar_t* GetChar() { return m_pc.get(); }

		void CreateCharTex(const wchar_t c,HDC hdc,TEXTMETRIC TM);
		ID3D11ShaderResourceView* GetTexResView() { return m_pTexResView; }//文字のリソースビュー

		private:
			unique_ptr<const wchar_t> m_pc;
			ID3D11Texture2D* m_pTexture;
			ID3D11ShaderResourceView* m_pTexResView;
};

typedef class CFontTex
{
	public:
		CFontTex() {};
		~CFontTex() {};

		static void InitFontTex();
		static void DeleteFontTex();

		static void CreateStrTex(const wchar_t* str);
		static void StrDraw(const wchar_t* str, float x, float y, float s, float r, float g, float b, float a);//文字描画

	private:
		//フォントに必要な変数
		static HFONT m_hFont;
		static HDC m_hdc;
		static HFONT m_oldFont;
		static TEXTMETRIC m_TM;

		static list<unique_ptr<CCherClass>>*list_char_tex;

}Font;