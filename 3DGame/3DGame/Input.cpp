//STL�f�o�b�O�@�\��OFF�ɂ���
#define _SECURE_SCL (0)
#define _HAS_ITERATOR_DEBUGGING (0)

#include "Input.h"

int CInput::m_mou_x;//�}�E�X�̈ʒuX
int CInput::m_mou_y;//�}�E�X�̈ʒuY

void CInput::InitInput()
{
	m_mou_x = 0;
	m_mou_y = 0;
}

//�}�E�X�ʒu�擾�p�@�E�B���h�E�v���W�[�V���[���ɐ錾
void CInput::SetMouPos(UINT* uMsg, LPARAM* lParam)
{
	switch (*uMsg)
	{
		case WM_MOUSEMOVE:	//�}�E�X���삵���Ƃ�
		{
			POINT point = { LOWORD(*lParam),HIWORD((*lParam)) };

			m_mou_x = point.x;//�J�[�\����x���W
			m_mou_y = point.y;//�J�[�\����y���W
		}
		break;
	}
	
}

//�L�[�E�}�E�X�{�^���v�b�V���m�F
bool CInput::KeyPush(int key)
{
	if (GetAsyncKeyState(key) & 0x8000)
	{
		return true;
	}
	return false;
}