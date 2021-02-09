//STLデバッグ機能をOFFにする
#define _SECURE_SCL (0)
#define _HAS_ITERATOR_DEBUGGING (0)

//システム系のヘッダーインクルード---------------
#include <stdio.h>
#include <Windows.h>
#include <D3D11.h>
#include <d3dx11.h>
#include <d3dCompiler.h>
#include <process.h>


//Gameシステム用ヘッダー（自作）インクルード
#include "WindowCreateClass.h"
#include "DeviceCreate.h"
#include "Draw2DPolygon.h"
#include "Input.h"
#include "Audio.h"
#include "TaskSystem.h"
#include "FontTex.h"
#include "Collision.h"
#include "Render3D.h"
#include "Math3D.h"

//デバック用オブジェクトヘッダ
#include "Hero.h"


//削除されていないメモリを出力にダンプする
#include <crtdbg.h>
#ifdef _DEBUG
#ifdef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK,__FILE__,__LINE__)

#define new DBG_NEW
#endif
#endif//_DEBUG

//LIBの登録---------------------
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dCompiler.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"XAudio2.lib")

//構造体------------------

//グローバル変数--------------------
/*IXAudio2* g_pXAudio2;//XAudio2オブジェクト
IXAudio2MasteringVoice* g_pMasteringVoice;//マスターボイス
atomic<bool> g_Is_game_end = false;*/

bool g_Is_game_end = false;
CMODEL* mod;


//プロトタイプ宣言
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParm, LPARAM lParam);

//マルチスレッドにする関数
//テクスチャ読み込み関数
unsigned __stdcall TextureLoadThread(void *p)
{
	//COM初期化
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	//イメージ読み込み
	CDraw2DPolygon::LoadImage(0, L"Texture1.png");

	CoUninitialize();//COM解除
	return 0;
}

//ミュージック読み込み関数
unsigned __stdcall MusicLoadThread(void *p)
{
	//COM初期化
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	//ミュージック読み込み
	CAudio::LoadBackMusic("Los!_Los!_Los!.ogg");

	CoUninitialize();//COM解除
	return 0;
}

//ゲームメイン関数
unsigned __stdcall GameMainThread(void *p)
{
	//カラーバッファクリア色
	float color[] = { 0.0f,0.25f,0.45f,1.0f };

	while (1)
	{
		//ゲームメイン
		TaskSystem::ListAction();
		//衝突判定実行
		Collision::CheckStart();

		//レンダリングターゲットセットとレンダリング画面クリア
		Dev::GetDeviceContext()->ClearRenderTargetView(Dev::GetRTV(), color);//画面をcolorでクリア
		Dev::GetDeviceContext()->ClearDepthStencilView(Dev::GetDSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		//ここからレンダリング開始
		//3D描画設定
		Dev::GetDeviceContext()->RSSetState(Dev::GetRS3D());
		Dev::GetDeviceContext()->OMSetRenderTargets(1, Dev::GetppRTV(), Dev::GetDSV());
		//3D描画

		//平行ライトをONにする
		Render::SetLightVec(-1.0f, -0.5f, 0.0f, true);
		//点ライトをONにする
		static int sin_r = 0;
		sin_r += 1;
		Render::SetLightPos(sin(3.14 / 180 * sin_r)*2.0f, 0.8f, 0.0f, 3.0f);

		
		float mat_w[16];//ワールド行列
		float mat_v[16];//ビュー(カメラ)行列
		float mat_p[16];//パースペクティヴ行列
		float mat_WVP[16];//ワールド×ビュー×パースペクティヴした行列

		//単位行列化
		Math3D::IdentityMatrix(mat_w);
		Math3D::IdentityMatrix(mat_v);
		Math3D::IdentityMatrix(mat_p);
		Math3D::IdentityMatrix(mat_WVP);

		//ビュー(カメラ)行列
		static float r = 0.0f;
		r += 0.01f;
		float eye[3] = { cos(r)*1000.0f,1000.0f,sin(r)*1000.0f };//カメラの位置
		float center[3] = { 0.0f,0.0f,0.0f };//カメラの注目点
		float up[3] = { 0.0f,1.0f,0.0f };//カメラのY軸ベクトル
		Math3D::LookAt(eye, center, up, mat_v);

		//パースペクティブ行列作成
		Math3D::Perspective(60.0f, 800.0f / 600.0f, 0.1f, 10000.0f, mat_p);

		//原点からX方向に2移動
		float pos[3] = { 2.0f,0.0f,0.0f };
		Math3D::IdentityMatrix(mat_w);//行列を初期化
		Math3D::Translate(mat_w, pos, mat_w);

		//テストのため一つ目の物体を回転させる。
		static float rr = 0.0f;
		rr * 0.01f;
		float f[3] = { 0.0f,1.0f,0.0f };
		Math3D::Rotate(mat_w, rr, f, mat_w);

		//三つの行列をmat_WVPに合成
		Math3D::Multiply(mat_w, mat_WVP, mat_WVP);
		Math3D::Multiply(mat_v, mat_WVP, mat_WVP);
		Math3D::Multiply(mat_p, mat_WVP, mat_WVP);

		//視野方向ベクトルを作成(視野方向ベクトル＝カメラの位置-注目点)
		float v_eye[3] = { eye[0] - center[0],eye[1] - center[1], eye[2] - center[2] };

		Render::Render(mod, mat_WVP,mat_w,v_eye);//一つ目

		//原点からX方向に2移動
		float pos_two[3] = { 1.0f,0.0f,0.0f };
		Math3D::IdentityMatrix(mat_w);//行列を初期化
		Math3D::Translate(mat_w, pos_two, mat_w);

		Math3D::IdentityMatrix(mat_WVP);//mat_WVP初期化
		//三つの行列をmat_WVPに合成
		Math3D::Multiply(mat_w, mat_WVP, mat_WVP);
		Math3D::Multiply(mat_v, mat_WVP, mat_WVP);
		Math3D::Multiply(mat_p, mat_WVP, mat_WVP);

		Render::Render(mod, mat_WVP,mat_w,v_eye);//二つ目

		//2D描画設定
		Dev::GetDeviceContext()->RSSetState(Dev::GetRS());
		Dev::GetDeviceContext()->OMSetRenderTargets(1, Dev::GetppRTV(), NULL);
		//2D描画


		TaskSystem::ListDraw();
		Collision::DrawDebug();

		//レンダリング終了
		Dev::GetSwapChain()->Present(1, 0);//60FPSでバックバッファとプライマリバッファの交換

		if (g_Is_game_end == true)
		{
			break;
		}
	}

	CoUninitialize();//COM解除
	return 0;
}


//Main関数
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR szCmdLine, int nCmdShow)
{
	//メモリダンプ開始
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF |_CRTDBG_LEAK_CHECK_DF);

	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	           
	wchar_t name[] = { L"GameEngine" };//ウィンドウ＆タイトルネーム
	MSG msg;                                   //メッセージハンドル
	
	//ウィンドウステータス
	WNDCLASSEX wcex = {
		sizeof(WNDCLASSEX),CS_HREDRAW | CS_VREDRAW,
		WndProc,0,0,hInstance,NULL,NULL,
		(HBRUSH)(COLOR_WINDOW + 1),NULL,name,NULL
	};
	//ウィンドウクラス作成
	RegisterClassEx(&wcex);

	//ゲーム各システム初期化-------
	CwindowCreate::NewWindow(800, 600, name, hInstance);
	CDeviceCreate::InitDevice(CwindowCreate::GethWnd(), 800, 600);
	Audio::InitAudio();
	Input::InitInput();
	Draw::InitPolygonRender();
	TaskSystem::InitTaskSystem();
	Font::InitFontTex();
	CCollision::InitHitoBox();//コリジョンの初期化
	CDeviceCreate::Init3DEnvironmet();
	CRender3D::Init();//3D描画環境設定

	//リソース読み込み--------
	thread* thread_i = new thread(TextureLoadThread, nullptr);
	thread* thread_m = new thread(MusicLoadThread, nullptr);
	//読み込みthread終了まで待機
	thread_i->join();
	thread_m->join();
	//メモリ開放
	delete thread_i;
	delete thread_m;

	//ミュージック情報取得
	/*Audio::LoadBackMusic("Los!_Los!_Los!.ogg");
	Audio::StartLoopMusic();*/

	/*//イメージ読み込み
	Draw::LoadImage(0, L"Texture1.png");
	//Draw::LoadImage(0, L"Texture.png");*/

	//デバック用オブジェクト作成
	/*CHero* hero = new CHero();
	hero->m_priority = 90;
	TaskSystem::InsertObj(hero);
	hero = new CHero();
	hero = new CHero();
	hero->m_priority = 80;
	TaskSystem::InsertObj(hero);
	hero = new CHero();
	hero->m_priority = 70;
	TaskSystem::InsertObj(hero);

	TaskSystem::SortPriority();
	
	*/
	/*//ウィンドウ作成
	CwindowCreate::NewWindow(800, 600, name, hInstance);

	//オーディオ作成
	Audio::InitAudio();

	//ミュージック情報取得
	Audio::LoadBackMusic("Los!_Los!_Los!.ogg");
	//Audio::LoadBackMusic(L"SETrigger.wav");
	Audio::LoadSEMusic(0,L"SEBom.wav");
	Audio::LoadSEMusic(1, L"SETrigger1.wav");

	Audio::StartLoopMusic();
	Audio::StartMusic(0);
	Audio::StartMusic(1);
	Audio::MasterVolume(1.0f);

	//DirectX Deviceの初期化
	CDeviceCreate::InitDevice(CwindowCreate::GethWnd(), 800, 600);

	//ポリゴン表示環境の初期化
	Draw::InitPolygonRender();
	Draw::LoadImage(0, L"Texture.png");//0番目に"Tecture.png"を読み込み
	Draw::LoadImage(1, L"Texture1.png");
	Draw::LoadImage(2, L"Texture2.png");
	Draw::LoadImage(3, L"Texture3.png");

	//入力用のクラス初期化
	Input::InitInput();

	//タスクシステム初期化
	TaskSystem::InitTaskSystem();*/

	//3Dモデル読み込み
	mod = new CMODEL();
	//mod->CreateSampleTriangularpyramid();
	mod->LoadComModel(L"test_sigure2.cmo.txt");

	//ゲームメインスレッド実行
	/*HANDLE game_handoru;
	game_handoru=(HACCEL) _beginthreadex(NULL,0,GameMainSled,)*/
	
	thread* thread_main = new thread(GameMainThread, nullptr);
	//メッセージループ
	do 
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

	} while (msg.message != WM_QUIT);
	thread_main->join();
	delete thread_main;

	//3Dモデル破棄
	delete mod;

	CDeviceCreate::Delete3DEnviroment();
	Collision::DeleteHitBox();//コリジョンの破棄
	Font::DeleteFontTex();
	TaskSystem::DeleteTaskSystem();//タスクシステムの破棄
	Draw::InitPolygonRender();////ポリゴン表示環境の破棄
	CDeviceCreate::ShutDown();//DirectXの環境破棄
	Audio::DeleteAudio();//オーディオ環境の破棄
	CRender3D::Delete();
	

	CoUninitialize();

	//この時点で解放されてないメモリ情報の表示
	_CrtDumpMemoryLeaks();
	return true;
}

//コールバック関数
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Input::SetMouPos(&uMsg, &lParam);

	switch (uMsg)
	{

		case WM_KEYDOWN:   //ESCキーで終了
			switch (wParam)
			{
				case VK_ESCAPE:
					g_Is_game_end = true;
					PostQuitMessage(0);
				break;
			}
			break;
			case WM_CLOSE:    //ウィンドウを閉じる場合
				g_Is_game_end = true;
				PostQuitMessage(0);
			case WM_DESTROY:
				return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}