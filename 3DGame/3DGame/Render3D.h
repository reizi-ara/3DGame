#pragma once
//3Dモデル描画---
#include <Windows.h>
#include <D3D11.h>

#include <d3dcompiler.h>
//解放マクロ---
#define SAFE_DELETE(p)	  {if(p){delete (p);		(p)=nullptr;}}
#define SAFE_DELETE_ARRAY {if(p){delete[](p);		(p)=nullptr;}}
#define SAFE_RELEASE(p)	  {if(p){(p)->Release();	(p)=nullptr;}}

//コンスタントバッファクラス
class CMODEL3D_BUFFER 
{
	public:
		float m_mat[16];//トランスフォーム用行列
		float m_mat_w[16];//法線用のワールド行列
		float m_light_vec[4];//平行ライト方向
		float m_light_pos[4];//平行ライト方向
		float m_ambient[4];//アンビエント
		float m_diffuse[4];//デフィーズ
		float m_emissive[4];//エミッシブ
		float m_specular[4];//スぺキュラ
		float m_specular_power[4];//スぺキュラパワー
		float m_eye[4];
};

//頂点レイアウトクラス(頂点が持つ情報)
class CPOINT3D_LAUOUT
{
	public:
		float m_pos[3];//頂点
		float m_normal[3];//法線
		float m_uv[2];//テクスチャ位置
		float m_coloar[4];//色
};

//マテリアルクラス(材質情報)
class CMATERIAL
{
public:
	float m_diffuse[4];
	float m_specular[4];
	float m_specular_power;
	float m_ambient[4];
	float m_emissive[4];
	ID3D11ShaderResourceView* m_pTexture;
};

//モデルクラス
class CMODEL
{
	public:
		~CMODEL();
		int* m_pvertex_size;//最大頂点数
		int* m_pindex_size;//最大面数
		CMATERIAL* m_Material;//モデルの材質情報
		ID3D11Buffer** m_ppVertexBuffer;//バーテックスバッファ
		ID3D11Buffer** m_ppIndexBuffer;//インデックスバッファ
		int m_material_max;

		//三角錐Model作成
		void CreateSampleTriangularpyramid();//サンプル用の三角錐
		//comファイル読み込み
		void LoadComModel(const wchar_t* name); //comファイル読み込み
};

//3Dレンダリングクラス
typedef class CRender3D
{
	public:
		CRender3D(){}
		~CRender3D(){}
		static void Init();
		static void Delete();

		static void Render(CMODEL* model, float mat[16], float mat_w[16],float v_eye[3]);
		static void SetLightVec(float x, float y, float z, bool light_on);//平行光源の向き
		static void SetLightPos(float x, float y, float z, float max);//平行光源の向き

	private:
		static float m_light_vector[4];//平行光源(方向)
		static float m_light_pos[4];//点光源
		static ID3D11VertexShader* m_pVertexShader;//バーテックスシェーダー
		static ID3D11PixelShader* m_pPixelShader;//ピクセルシェーダー
		static ID3D11InputLayout* m_pVertexLayout;//頂点入力レイアウト
		static ID3D11Buffer* m_pConstantBuffer;//コンスタントバッファ


}Render;