//STL�f�o�b�O�@�\��OFF�ɂ���
#define _SECURE_SCL (0)
#define _HAS_ITERATOR_DEBUGGING (0)

//�V�X�e���n�̃w�b�_�[�C���N���[�h---------------
#include <stdio.h>
#include <Windows.h>
#include <D3D11.h>
#include <d3dx11.h>
#include <d3dCompiler.h>
#include <process.h>


//Game�V�X�e���p�w�b�_�[�i����j�C���N���[�h
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

//�f�o�b�N�p�I�u�W�F�N�g�w�b�_
#include "Hero.h"


//�폜����Ă��Ȃ����������o�͂Ƀ_���v����
#include <crtdbg.h>
#ifdef _DEBUG
#ifdef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK,__FILE__,__LINE__)

#define new DBG_NEW
#endif
#endif//_DEBUG

//LIB�̓o�^---------------------
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dCompiler.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"XAudio2.lib")

//�\����------------------

//�O���[�o���ϐ�--------------------
/*IXAudio2* g_pXAudio2;//XAudio2�I�u�W�F�N�g
IXAudio2MasteringVoice* g_pMasteringVoice;//�}�X�^�[�{�C�X
atomic<bool> g_Is_game_end = false;*/

bool g_Is_game_end = false;
CMODEL* mod;


//�v���g�^�C�v�錾
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParm, LPARAM lParam);

//�}���`�X���b�h�ɂ���֐�
//�e�N�X�`���ǂݍ��݊֐�
unsigned __stdcall TextureLoadThread(void *p)
{
	//COM������
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	//�C���[�W�ǂݍ���
	CDraw2DPolygon::LoadImage(0, L"Texture1.png");

	CoUninitialize();//COM����
	return 0;
}

//�~���[�W�b�N�ǂݍ��݊֐�
unsigned __stdcall MusicLoadThread(void *p)
{
	//COM������
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	//�~���[�W�b�N�ǂݍ���
	CAudio::LoadBackMusic("Los!_Los!_Los!.ogg");

	CoUninitialize();//COM����
	return 0;
}

//�Q�[�����C���֐�
unsigned __stdcall GameMainThread(void *p)
{
	//�J���[�o�b�t�@�N���A�F
	float color[] = { 0.0f,0.25f,0.45f,1.0f };

	while (1)
	{
		//�Q�[�����C��
		TaskSystem::ListAction();
		//�Փ˔�����s
		Collision::CheckStart();

		//�����_�����O�^�[�Q�b�g�Z�b�g�ƃ����_�����O��ʃN���A
		Dev::GetDeviceContext()->ClearRenderTargetView(Dev::GetRTV(), color);//��ʂ�color�ŃN���A
		Dev::GetDeviceContext()->ClearDepthStencilView(Dev::GetDSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		//�������烌���_�����O�J�n
		//3D�`��ݒ�
		Dev::GetDeviceContext()->RSSetState(Dev::GetRS3D());
		Dev::GetDeviceContext()->OMSetRenderTargets(1, Dev::GetppRTV(), Dev::GetDSV());
		//3D�`��

		//���s���C�g��ON�ɂ���
		Render::SetLightVec(-1.0f, -0.5f, 0.0f, true);
		//�_���C�g��ON�ɂ���
		static int sin_r = 0;
		sin_r += 1;
		Render::SetLightPos(sin(3.14 / 180 * sin_r)*2.0f, 0.8f, 0.0f, 3.0f);

		
		float mat_w[16];//���[���h�s��
		float mat_v[16];//�r���[(�J����)�s��
		float mat_p[16];//�p�[�X�y�N�e�B���s��
		float mat_WVP[16];//���[���h�~�r���[�~�p�[�X�y�N�e�B�������s��

		//�P�ʍs��
		Math3D::IdentityMatrix(mat_w);
		Math3D::IdentityMatrix(mat_v);
		Math3D::IdentityMatrix(mat_p);
		Math3D::IdentityMatrix(mat_WVP);

		//�r���[(�J����)�s��
		static float r = 0.0f;
		r += 0.01f;
		float eye[3] = { cos(r)*1000.0f,1000.0f,sin(r)*1000.0f };//�J�����̈ʒu
		float center[3] = { 0.0f,0.0f,0.0f };//�J�����̒��ړ_
		float up[3] = { 0.0f,1.0f,0.0f };//�J������Y���x�N�g��
		Math3D::LookAt(eye, center, up, mat_v);

		//�p�[�X�y�N�e�B�u�s��쐬
		Math3D::Perspective(60.0f, 800.0f / 600.0f, 0.1f, 10000.0f, mat_p);

		//���_����X������2�ړ�
		float pos[3] = { 2.0f,0.0f,0.0f };
		Math3D::IdentityMatrix(mat_w);//�s���������
		Math3D::Translate(mat_w, pos, mat_w);

		//�e�X�g�̂��߈�ڂ̕��̂���]������B
		static float rr = 0.0f;
		rr * 0.01f;
		float f[3] = { 0.0f,1.0f,0.0f };
		Math3D::Rotate(mat_w, rr, f, mat_w);

		//�O�̍s���mat_WVP�ɍ���
		Math3D::Multiply(mat_w, mat_WVP, mat_WVP);
		Math3D::Multiply(mat_v, mat_WVP, mat_WVP);
		Math3D::Multiply(mat_p, mat_WVP, mat_WVP);

		//��������x�N�g�����쐬(��������x�N�g�����J�����̈ʒu-���ړ_)
		float v_eye[3] = { eye[0] - center[0],eye[1] - center[1], eye[2] - center[2] };

		Render::Render(mod, mat_WVP,mat_w,v_eye);//���

		//���_����X������2�ړ�
		float pos_two[3] = { 1.0f,0.0f,0.0f };
		Math3D::IdentityMatrix(mat_w);//�s���������
		Math3D::Translate(mat_w, pos_two, mat_w);

		Math3D::IdentityMatrix(mat_WVP);//mat_WVP������
		//�O�̍s���mat_WVP�ɍ���
		Math3D::Multiply(mat_w, mat_WVP, mat_WVP);
		Math3D::Multiply(mat_v, mat_WVP, mat_WVP);
		Math3D::Multiply(mat_p, mat_WVP, mat_WVP);

		Render::Render(mod, mat_WVP,mat_w,v_eye);//���

		//2D�`��ݒ�
		Dev::GetDeviceContext()->RSSetState(Dev::GetRS());
		Dev::GetDeviceContext()->OMSetRenderTargets(1, Dev::GetppRTV(), NULL);
		//2D�`��


		TaskSystem::ListDraw();
		Collision::DrawDebug();

		//�����_�����O�I��
		Dev::GetSwapChain()->Present(1, 0);//60FPS�Ńo�b�N�o�b�t�@�ƃv���C�}���o�b�t�@�̌���

		if (g_Is_game_end == true)
		{
			break;
		}
	}

	CoUninitialize();//COM����
	return 0;
}


//Main�֐�
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR szCmdLine, int nCmdShow)
{
	//�������_���v�J�n
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF |_CRTDBG_LEAK_CHECK_DF);

	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	           
	wchar_t name[] = { L"GameEngine" };//�E�B���h�E���^�C�g���l�[��
	MSG msg;                                   //���b�Z�[�W�n���h��
	
	//�E�B���h�E�X�e�[�^�X
	WNDCLASSEX wcex = {
		sizeof(WNDCLASSEX),CS_HREDRAW | CS_VREDRAW,
		WndProc,0,0,hInstance,NULL,NULL,
		(HBRUSH)(COLOR_WINDOW + 1),NULL,name,NULL
	};
	//�E�B���h�E�N���X�쐬
	RegisterClassEx(&wcex);

	//�Q�[���e�V�X�e��������-------
	CwindowCreate::NewWindow(800, 600, name, hInstance);
	CDeviceCreate::InitDevice(CwindowCreate::GethWnd(), 800, 600);
	Audio::InitAudio();
	Input::InitInput();
	Draw::InitPolygonRender();
	TaskSystem::InitTaskSystem();
	Font::InitFontTex();
	CCollision::InitHitoBox();//�R���W�����̏�����
	CDeviceCreate::Init3DEnvironmet();
	CRender3D::Init();//3D�`����ݒ�

	//���\�[�X�ǂݍ���--------
	thread* thread_i = new thread(TextureLoadThread, nullptr);
	thread* thread_m = new thread(MusicLoadThread, nullptr);
	//�ǂݍ���thread�I���܂őҋ@
	thread_i->join();
	thread_m->join();
	//�������J��
	delete thread_i;
	delete thread_m;

	//�~���[�W�b�N���擾
	/*Audio::LoadBackMusic("Los!_Los!_Los!.ogg");
	Audio::StartLoopMusic();*/

	/*//�C���[�W�ǂݍ���
	Draw::LoadImage(0, L"Texture1.png");
	//Draw::LoadImage(0, L"Texture.png");*/

	//�f�o�b�N�p�I�u�W�F�N�g�쐬
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
	/*//�E�B���h�E�쐬
	CwindowCreate::NewWindow(800, 600, name, hInstance);

	//�I�[�f�B�I�쐬
	Audio::InitAudio();

	//�~���[�W�b�N���擾
	Audio::LoadBackMusic("Los!_Los!_Los!.ogg");
	//Audio::LoadBackMusic(L"SETrigger.wav");
	Audio::LoadSEMusic(0,L"SEBom.wav");
	Audio::LoadSEMusic(1, L"SETrigger1.wav");

	Audio::StartLoopMusic();
	Audio::StartMusic(0);
	Audio::StartMusic(1);
	Audio::MasterVolume(1.0f);

	//DirectX Device�̏�����
	CDeviceCreate::InitDevice(CwindowCreate::GethWnd(), 800, 600);

	//�|���S���\�����̏�����
	Draw::InitPolygonRender();
	Draw::LoadImage(0, L"Texture.png");//0�Ԗڂ�"Tecture.png"��ǂݍ���
	Draw::LoadImage(1, L"Texture1.png");
	Draw::LoadImage(2, L"Texture2.png");
	Draw::LoadImage(3, L"Texture3.png");

	//���͗p�̃N���X������
	Input::InitInput();

	//�^�X�N�V�X�e��������
	TaskSystem::InitTaskSystem();*/

	//3D���f���ǂݍ���
	mod = new CMODEL();
	//mod->CreateSampleTriangularpyramid();
	mod->LoadComModel(L"test_sigure2.cmo.txt");

	//�Q�[�����C���X���b�h���s
	/*HANDLE game_handoru;
	game_handoru=(HACCEL) _beginthreadex(NULL,0,GameMainSled,)*/
	
	thread* thread_main = new thread(GameMainThread, nullptr);
	//���b�Z�[�W���[�v
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

	//3D���f���j��
	delete mod;

	CDeviceCreate::Delete3DEnviroment();
	Collision::DeleteHitBox();//�R���W�����̔j��
	Font::DeleteFontTex();
	TaskSystem::DeleteTaskSystem();//�^�X�N�V�X�e���̔j��
	Draw::InitPolygonRender();////�|���S���\�����̔j��
	CDeviceCreate::ShutDown();//DirectX�̊��j��
	Audio::DeleteAudio();//�I�[�f�B�I���̔j��
	CRender3D::Delete();
	

	CoUninitialize();

	//���̎��_�ŉ������ĂȂ����������̕\��
	_CrtDumpMemoryLeaks();
	return true;
}

//�R�[���o�b�N�֐�
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Input::SetMouPos(&uMsg, &lParam);

	switch (uMsg)
	{

		case WM_KEYDOWN:   //ESC�L�[�ŏI��
			switch (wParam)
			{
				case VK_ESCAPE:
					g_Is_game_end = true;
					PostQuitMessage(0);
				break;
			}
			break;
			case WM_CLOSE:    //�E�B���h�E�����ꍇ
				g_Is_game_end = true;
				PostQuitMessage(0);
			case WM_DESTROY:
				return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}