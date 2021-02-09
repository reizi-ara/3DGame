//STLデバッグ機能をOFFにする
#define _SECURE_SCL (0)
#define _HAS_ITERATOR_DEBUGGING (0)

#include "Collision.h"
#include "Draw2DPolygon.h"

list<shared_ptr<HitBox>>* Collision::m_hit_box_list;//リスト　HitBox用

//デバック用の当たり判定描画
void CCollision::DrawDebug()
{
	float c[4] = { 1.0f,0.0f,0.0f,0.3f };
	//listのHitBoxを描画
	for (auto ip = m_hit_box_list->begin(); ip != m_hit_box_list->end(); ip++)
	{
		CDraw2DPolygon::Draw2DHitBox((*ip)->m_x, (*ip)->m_y, (*ip)->m_w, (*ip)->m_h, c);
	}
}

//当たり判定を作成しリストに登録
HitBox* CCollision::HitBoxInsert(CObj* p)
{
	//ヒットボックス作成
	shared_ptr <HitBox> sp(new HitBox());

	//ヒットボックスに情報を入れる(フレンドなので直接アクセスできる)
	sp->m_obj = p;//この当たり判定を持つオブジェクトのアドレス(オブジェクトアドレスは引数pから引っ張てくる)
	sp->m_x = -999.0f;//当たり判定のX位置
	sp->m_y = -999.0f;//当たり判定のY位置
	sp->m_w = 64.0f;//当たり判定の横幅
	sp->m_h = 64.0f;//当たり判定の縦幅
	sp->m_Is_invisible = false;//当たり判定の無敵モードOFF
	sp->m_element = 0;//当たり判定の属性

	//リストに登録
	m_hit_box_list->push_back(sp);

	return sp.get();
}

//初期化
void CCollision::InitHitoBox()
{
	m_hit_box_list = new list<shared_ptr<HitBox>>;
	m_hit_box_list->clear();
}

//破棄
void CCollision::DeleteHitBox()
{
	m_hit_box_list->clear();
	delete m_hit_box_list;
}

//list内の当たり判定全チェック開始
void CCollision::CheckStart()
{
	//リスト内のIs_deleteがtrueの要素を削除
	auto i = m_hit_box_list->begin();
	while (i != m_hit_box_list->end())
	{
		if (i->get()->Is_delete == true)
		{
			//イテレーターiの要素を削除
			i = m_hit_box_list -> erase(i);
		}
		else
		{
			i++;
		}
	}

	//複数との衝突情報制御用
	int hit_count = 0;

	//リスト内のヒットボックスで当たり判定を実施
	for (auto ip_a=m_hit_box_list->begin(); ip_a!=m_hit_box_list->end(); ip_a++)
	{
		hit_count = 0;
		//Aの情報の衝突情報初期化
		for (int i = 0; i < 10; i++)
		{
			(*ip_a)->m_hit[i] = nullptr;
		}
		//AのHitBoxの当たり判定無視チェック
		//無敵
		if ((*ip_a)->m_Is_invisible)
			continue;

		for (auto ip_b = m_hit_box_list->begin(); ip_b != m_hit_box_list->end(); ip_b++)
		{
			//Bの当たり判定無視チェック
			//AとBが同じヒットボックス
			if (ip_a == ip_b)
				continue;

			//無敵
			if ((*ip_b)->m_Is_invisible)
				continue;

			//同属性
			if ((*ip_a)->m_element== (*ip_b)->m_element)
				continue;

			//当たり判定を実施
			bool Is_hit = HitAB((*ip_a)->m_x, (*ip_a)->m_y, (*ip_a)->m_w, (*ip_a)->m_h, 
								(*ip_b)->m_x, (*ip_b)->m_y, (*ip_b)->m_w, (*ip_b)->m_h
								);
			//衝突している場合
			if (Is_hit == true)
			{
				//当たっている情報を加える
				int buffer_count = hit_count % 10;
				(*ip_a)->m_hit[buffer_count] = ip_b->get();
				hit_count++;
			}
		}
	}
}

//個々の当たり判定
bool CCollision::HitAB(float ax, float ay, float aw, float ah, float bx, float by, float bw, float bh)
{
	//当たり判定
	if (ax<bx + bw && ax + aw>bx&&ay<by + bh && ay + ah>by)
	{
		//衝突している
		return true;
	}
	//衝突していない
	return false;
}