//STL�f�o�b�O�@�\��OFF�ɂ���
#define _SECURE_SCL (0)
#define _HAS_ITERATOR_DEBUGGING (0)

#include "Collision.h"
#include "Draw2DPolygon.h"

list<shared_ptr<HitBox>>* Collision::m_hit_box_list;//���X�g�@HitBox�p

//�f�o�b�N�p�̓����蔻��`��
void CCollision::DrawDebug()
{
	float c[4] = { 1.0f,0.0f,0.0f,0.3f };
	//list��HitBox��`��
	for (auto ip = m_hit_box_list->begin(); ip != m_hit_box_list->end(); ip++)
	{
		CDraw2DPolygon::Draw2DHitBox((*ip)->m_x, (*ip)->m_y, (*ip)->m_w, (*ip)->m_h, c);
	}
}

//�����蔻����쐬�����X�g�ɓo�^
HitBox* CCollision::HitBoxInsert(CObj* p)
{
	//�q�b�g�{�b�N�X�쐬
	shared_ptr <HitBox> sp(new HitBox());

	//�q�b�g�{�b�N�X�ɏ�������(�t�����h�Ȃ̂Œ��ڃA�N�Z�X�ł���)
	sp->m_obj = p;//���̓����蔻������I�u�W�F�N�g�̃A�h���X(�I�u�W�F�N�g�A�h���X�͈���p����������Ă���)
	sp->m_x = -999.0f;//�����蔻���X�ʒu
	sp->m_y = -999.0f;//�����蔻���Y�ʒu
	sp->m_w = 64.0f;//�����蔻��̉���
	sp->m_h = 64.0f;//�����蔻��̏c��
	sp->m_Is_invisible = false;//�����蔻��̖��G���[�hOFF
	sp->m_element = 0;//�����蔻��̑���

	//���X�g�ɓo�^
	m_hit_box_list->push_back(sp);

	return sp.get();
}

//������
void CCollision::InitHitoBox()
{
	m_hit_box_list = new list<shared_ptr<HitBox>>;
	m_hit_box_list->clear();
}

//�j��
void CCollision::DeleteHitBox()
{
	m_hit_box_list->clear();
	delete m_hit_box_list;
}

//list���̓����蔻��S�`�F�b�N�J�n
void CCollision::CheckStart()
{
	//���X�g����Is_delete��true�̗v�f���폜
	auto i = m_hit_box_list->begin();
	while (i != m_hit_box_list->end())
	{
		if (i->get()->Is_delete == true)
		{
			//�C�e���[�^�[i�̗v�f���폜
			i = m_hit_box_list -> erase(i);
		}
		else
		{
			i++;
		}
	}

	//�����Ƃ̏Փˏ�񐧌�p
	int hit_count = 0;

	//���X�g���̃q�b�g�{�b�N�X�œ����蔻������{
	for (auto ip_a=m_hit_box_list->begin(); ip_a!=m_hit_box_list->end(); ip_a++)
	{
		hit_count = 0;
		//A�̏��̏Փˏ�񏉊���
		for (int i = 0; i < 10; i++)
		{
			(*ip_a)->m_hit[i] = nullptr;
		}
		//A��HitBox�̓����蔻�薳���`�F�b�N
		//���G
		if ((*ip_a)->m_Is_invisible)
			continue;

		for (auto ip_b = m_hit_box_list->begin(); ip_b != m_hit_box_list->end(); ip_b++)
		{
			//B�̓����蔻�薳���`�F�b�N
			//A��B�������q�b�g�{�b�N�X
			if (ip_a == ip_b)
				continue;

			//���G
			if ((*ip_b)->m_Is_invisible)
				continue;

			//������
			if ((*ip_a)->m_element== (*ip_b)->m_element)
				continue;

			//�����蔻������{
			bool Is_hit = HitAB((*ip_a)->m_x, (*ip_a)->m_y, (*ip_a)->m_w, (*ip_a)->m_h, 
								(*ip_b)->m_x, (*ip_b)->m_y, (*ip_b)->m_w, (*ip_b)->m_h
								);
			//�Փ˂��Ă���ꍇ
			if (Is_hit == true)
			{
				//�������Ă������������
				int buffer_count = hit_count % 10;
				(*ip_a)->m_hit[buffer_count] = ip_b->get();
				hit_count++;
			}
		}
	}
}

//�X�̓����蔻��
bool CCollision::HitAB(float ax, float ay, float aw, float ah, float bx, float by, float bw, float bh)
{
	//�����蔻��
	if (ax<bx + bw && ax + aw>bx&&ay<by + bh && ay + ah>by)
	{
		//�Փ˂��Ă���
		return true;
	}
	//�Փ˂��Ă��Ȃ�
	return false;
}