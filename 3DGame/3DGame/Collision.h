#pragma once
//�K�v�ȃw�b�_�[�K�v�ȃw�b�_�[
#include <windows.h>

//�K�v�ȃw�b�_
#include <memory>
#include <list>

#include "TaskSystem.h"
using namespace std;
//�Փ˃I�u�W�F�N�g�N���X
class HitBox
{
	//CColision�̂݃v���C�x�[�g���J����
	friend class CCollision;
	public:
		HitBox(){}
		~HitBox(){}
		//������
		void Init()
		{
			Is_delete = false;
			m_x = m_y = -999.0f;
			m_w = m_h = 0.0f;
			m_obj = nullptr;
			m_element = -1;
			for (int i = 0; i < 10; i++)
				m_hit[i] = nullptr;
		}

		void SetDelete(bool d) { Is_delete = d; }
		void SetPos(float x, float y) { m_x = x; m_y = y; }
		void SetWH(float w, float h) { m_w = w; m_h = h; }
		void SetElement(int element) { m_element=element; }
		void SetInvisible(bool invisible) { m_Is_invisible=invisible; }
		HitBox** GetHitData() { return m_hit; }
	private:
		//�폜�t���O
		bool Is_delete;
		//�ʒu
		float m_x;
		float m_y;
		//��
		float m_w;
		float m_h;
		//���̏Փ˃I�u�W�F�N�g�����I�u�W�F�N�g�̃|�C���^
		CObj* m_obj;
		//����
		int m_element;
		//���G
		bool m_Is_invisible;
		//�������������HitBox���
		//������10�I�u�W�F�N�g�܂ŏ����Ƃ�
		HitBox* m_hit[10];

};

//�Փ˃N���X
typedef class CCollision
{
	public:
		CCollision() {}
		~CCollision() {}

		static void InitHitoBox();//������
		static void DeleteHitBox();//�j��

		static HitBox* HitBoxInsert(CObj* p);//�����蔻����쐬�����X�g�ɓo�^
		static void CheckStart();
		static void DrawDebug();
	private:
		//�X�̓����蔻��
		static bool HitAB(float ax, float ay, float aw, float ah, float bx, float by, float bw, float bh);
		//���X�g�@HitBox�����I�u�W�F�N�g�v�f������
		static list<shared_ptr<HitBox>>* m_hit_box_list;

}Collision;
