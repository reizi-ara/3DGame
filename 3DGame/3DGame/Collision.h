#pragma once
//必要なヘッダー必要なヘッダー
#include <windows.h>

//必要なヘッダ
#include <memory>
#include <list>

#include "TaskSystem.h"
using namespace std;
//衝突オブジェクトクラス
class HitBox
{
	//CColisionのみプライベート公開をす
	friend class CCollision;
	public:
		HitBox(){}
		~HitBox(){}
		//初期化
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
		//削除フラグ
		bool Is_delete;
		//位置
		float m_x;
		float m_y;
		//幅
		float m_w;
		float m_h;
		//この衝突オブジェクトを持つオブジェクトのポインタ
		CObj* m_obj;
		//属性
		int m_element;
		//無敵
		bool m_Is_invisible;
		//当たった相手のHitBox情報
		//同時に10オブジェクトまで情報をとる
		HitBox* m_hit[10];

};

//衝突クラス
typedef class CCollision
{
	public:
		CCollision() {}
		~CCollision() {}

		static void InitHitoBox();//初期化
		static void DeleteHitBox();//破棄

		static HitBox* HitBoxInsert(CObj* p);//当たり判定を作成しリストに登録
		static void CheckStart();
		static void DrawDebug();
	private:
		//個々の当たり判定
		static bool HitAB(float ax, float ay, float aw, float ah, float bx, float by, float bw, float bh);
		//リスト　HitBoxを持つオブジェクト要素を持つ
		static list<shared_ptr<HitBox>>* m_hit_box_list;

}Collision;
