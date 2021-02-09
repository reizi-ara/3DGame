#pragma once

//�K�v�ȃw�b�_
#include <memory>
#include <list>

using namespace std;

//�I�u�W�F�N���X
class CObj
{
	public:
	CObj() 
	{
		is_delete = false;
		m_priority = 0;
	};
	virtual ~CObj() {};
		virtual void Action() = 0;
		virtual void Draw() = 0;

		bool is_delete;
		unsigned long m_priority;
};

//�^�X�N�V�X�e��
typedef class CTaskSystem
{
	public:
		CTaskSystem(){}
		~CTaskSystem(){}

		static void InsertObj(CObj* obj);//�ǉ�
		static void ListAction();
		static void ListDraw();

		static void InitTaskSystem();//������
		static void DeleteTaskSystem();//�j��

		static void SortPriority();

	private:
		//���X�g�@CObj�����I�u�W�F�N�g�̗v�f������
		static list<shared_ptr<CObj>>*m_task_list;

}TaskSystem;