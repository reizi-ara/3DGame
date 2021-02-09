//STL�f�o�b�O�@�\��OFF�ɂ���
#define _SECURE_SCL (0)
#define _HAS_ITERATOR_DEBUGGING (0)

#include <Windows.h>
#include "WindowCreateClass.h"

HWND CwindowCreate::m_hWnd;	//�E�B���h�E�n���h��
int CwindowCreate::m_width; //�E�B���h�E�̉���
int CwindowCreate::m_height;//�E�B���h�E�̏c��

//NewWindow���\�b�h
//���� int :�E�B���h�E�̉���
//����2int :�E�B���h�E�̏c��
//����3wchar_t* :�E�B���h�E�X�e�[�^�X�E�^�C�g����
//����4�@HINSTANCE�F�C���X�^���g�n���h��
//�߂�l�@����
//���e�F�E�B���h�E���쐬���܂��B
void CwindowCreate::NewWindow(int w, int h, wchar_t* name, HINSTANCE hInstance)
{
	m_width = w;
	m_height = h;

	int width = m_width + GetSystemMetrics(SM_CXDLGFRAME) * 2;
	int height = m_height + GetSystemMetrics(SM_CYDLGFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION);

	//�E�B���h�E�쐬
	if (!(m_hWnd = CreateWindow(name, name,
		WS_OVERLAPPEDWINDOW&(WS_MAXIMIZEBOX | WS_SIZEBOX),
		CW_USEDEFAULT, 0, width, height, 0, 0, hInstance, 0)))
	{
		return;
	}

	ShowWindow(m_hWnd, SW_SHOW);
}