//STLデバッグ機能をOFFにする
#define _SECURE_SCL (0)
#define _HAS_ITERATOR_DEBUGGING (0)

#include "FontTex.h"

HFONT CFontTex::m_hFont;
HDC CFontTex::m_hdc;
HFONT CFontTex::m_oldFont;
TEXTMETRIC CFontTex::m_TM;
list<unique_ptr<CCherClass>>*CFontTex::list_char_tex;

//------CCherClass------

//文字テクスチャ作成メソッド
void CCherClass::CreateCharTex(const wchar_t c, HDC hdc, TEXTMETRIC TM)
{
	//識別文字
	UINT code = 0;//作成する文字コード

	//文字フォント描画用
	BYTE* ptr;//文字のグラフィック(ビットマップ)
	DWORD size;//文字を表記するのに必要なメモリの大きさ
	GLYPHMETRICS GM;//象形文字の情報が格納
	const MAT2 Mat = { {0,1},{0,0},{0,0},{0,1} };//フォントの書き込み

	//テクスチャ書き込み用ポインタ
	D3D11_MAPPED_SUBRESOURCE mapped;//リソースにアクセスするポインタ
	BYTE* pBits;	//テクスチャのピクセル情報を入れるポインタ

	//識別文字コード登録
	m_pc.reset(new wchar_t(c));
	code = (UINT)*m_pc.get();

	//フォント情報から文字のビットマップ取得
	//文字のビットマップの大きさを取得
	size = GetGlyphOutline(hdc, code, GGO_GRAY4_BITMAP, &GM, 0, NULL, &Mat);
	ptr = new BYTE[size];
	//文字のビットマップ情報をptrに入れる
	GetGlyphOutline(hdc, code, GGO_GRAY4_BITMAP, &GM, size, ptr, &Mat);

	//空テクスチャの設定
	D3D11_TEXTURE2D_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//テクスチャフォーマットR8G8B8の24bit
	desc.SampleDesc.Count = 1;//サンプリングは1ピクセルのみ
	desc.Usage = D3D11_USAGE_DYNAMIC;//CPU書き込み可能
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;//シェーダリソース
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;//CPUから書き込みアクセス可
	desc.Height = 32;//縦サイズ
	desc.Width = 32;//横サイズ

	//設定を元に空テクスチャを作成
	Dev::GetDevice()->CreateTexture2D(&desc, 0, &m_pTexture);

	//テクスチャ情報を取得する
	D3D11_TEXTURE2D_DESC texDesc;
	m_pTexture->GetDesc(&texDesc);

	//テクスチャにShaderResourceViewを接続
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	Dev::GetDevice()->CreateShaderResourceView(m_pTexture, &srvDesc, &m_pTexResView);

	//テクスチャロック(テクスチャの書き込みを行うときロックする)
	Dev::GetDeviceContext()->Map(m_pTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	pBits = (BYTE*)mapped.pData;

	//フォント情報の書き込み
	int iOfs_x = GM.gmptGlyphOrigin.x;
	int iOfs_y = TM.tmAscent - GM.gmptGlyphOrigin.y;//iOfs_x,iOfs_y:書き出し位置(左上)
	int iBmp_w = GM.gmBlackBoxX + (4 - (GM.gmBlackBoxX % 4)) % 4;
	int iBmp_h = GM.gmBlackBoxY;//iBmp_w,iBpm_h:フォントビットマップの幅高
	int Level = 17;//Level:α値の段階(GGO_GRAY4_BITMAPなので17段階)
	DWORD Alpha, Color;
	//1ピクセル単位にフォントの色情報(32bit)をテクスチャに書き込み
	memset(pBits,0x00, sizeof(pBits)*32*32);
	for (int y = iOfs_y; y < iOfs_y + iBmp_h; y++)
	{
		for (unsigned int x = iOfs_x; x < iOfs_x + GM.gmBlackBoxX; x++)
		{
			Alpha = (255 * ptr[x - iOfs_x + iBmp_w * (y - iOfs_y)]) / (Level - 1);
			Color = 0x00ffffff | (Alpha << 24);
			memcpy((BYTE*)pBits + (y << 7) + (x << 2), &Color, sizeof(DWORD));
		}
	}

	//アンロック(書き込みが終われば、アンロックする)
	Dev::GetDeviceContext()->Unmap(m_pTexture, D3D11CalcSubresource(0, 0, 1));

	//文字ビットマップデータ削除
	delete[] ptr;
}



//---CFontTex-----
//文字描画
void CFontTex::StrDraw(const wchar_t* str, float x, float y, float s, float r, float g, float b, float a)
{
	//文字列を登録
	CreateStrTex(str);

	//描画
	float str_pos = 0.0f;

	float c[] = { r,g,b,a };
	for (unsigned int i = 0; i < wcslen(str); i++)
	{
		for (auto itr = list_char_tex->begin(); itr != list_char_tex->end(); itr++)
		{
			if (*itr->get()->GetChar() == str[i])
			{
				Draw::Draw2DChar(itr->get()->GetTexResView(), x+(str_pos), y, s, c);
				//大文字か小文字で次の描画位置を変える
				char mc[3];
				int len;
				wctomb_s(&len, mc, sizeof(const wchar_t), *(*itr)->GetChar());
				if (len == 1)
				{
					str_pos += 16 * s;
				}
				else
				{
					str_pos += 32 * s;
				}
				break;
			}
		}
	}
}

//文字列を元に文字テクスチャを作成
void CFontTex::CreateStrTex(const wchar_t* str)
{
	//文字列を、文字をlistに登録済みかチェック
	for (unsigned int i = 0; i < wcslen(str); i++)
	{
		bool is_char_entry = false;

		//リストから検索
		for (auto itr = list_char_tex->begin(); itr != list_char_tex->end(); itr++)
		{
			//登録された文字とstrの文字を比較
			if (*itr->get()->GetChar()==str[i])
			{
				//登録されている
				is_char_entry = true;
			}
		}

		//なければ、CreateCharTexを作成する
		if (is_char_entry == false)
		{
			//文字テクスチャ作成
			unique_ptr<CCherClass>obj(new CCherClass());
			obj->CreateCharTex(str[i], m_hdc, m_TM);

			//リストに譲渡
			list_char_tex->push_back(move(obj));
		}
	}
}

//初期化メソッド
void CFontTex::InitFontTex()
{
	//リスト初期化
	list_char_tex=new list<unique_ptr<CCherClass>>;

	//第一引数のカテゴリに、第二引数の国別コードを影響与える
	//この場合、Unicodeの文字を日本コードするとなると
	setlocale(LC_CTYPE, "jpn");

	//論理フォント設定
	HFONT hFont = CreateFont(
		32,
		0, 0, 0, 0,
		FALSE, FALSE, FALSE,
		SHIFTJIS_CHARSET,
		OUT_TT_ONLY_PRECIS,
		CLIP_DEFAULT_PRECIS,
		PROOF_QUALITY,
		FIXED_PITCH | FF_MODERN,
		L"MS ゴシック"//使用フォント
		);

	//指定されたウィンドウクライアント領域または画面全体を表す
	//ディスプレイデバイスコンテキストのハンドルを取得
	m_hdc = GetDC(NULL);

	//Windowsは、SelectObjectでデバイスコンテキストにする。
	//これを設定すると論理フォントに最も近い物理フォントをセットする
	m_oldFont = (HFONT)SelectObject(m_hdc, hFont);

	//現在選択させれているフォントの情報をしていさえwタバッファに設定する
	GetTextMetrics(m_hdc, &m_TM);

	//論理フォントをメンバに渡す
	m_hFont = hFont;
}

void CFontTex::DeleteFontTex()
{
	//リスト破棄
	list_char_tex->clear();

	//これらのGDIオブジェクトを破壊
	DeleteObject(m_oldFont);
	DeleteObject(m_hFont);
	//ディスプレイデバイスコンテキストハンドル開放
	ReleaseDC(NULL, m_hdc);
}