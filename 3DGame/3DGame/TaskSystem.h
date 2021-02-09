#pragma once

//必要なヘッダ
#include <memory>
#include <list>

using namespace std;

//オブジェクラス
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

//タスクシステム
typedef class CTaskSystem
{
	public:
		CTaskSystem(){}
		~CTaskSystem(){}

		static void InsertObj(CObj* obj);//追加
		static void ListAction();
		static void ListDraw();

		static void InitTaskSystem();//初期化
		static void DeleteTaskSystem();//破棄

		static void SortPriority();

	private:
		//リスト　CObjを持つオブジェクトの要素を持つ
		static list<shared_ptr<CObj>>*m_task_list;

}TaskSystem;