#include "Render3D.h"
#include "Draw2DPolygon.h"
#include "DeviceCreate.h"
#include "Math3D.h"

float CRender3D::m_light_vector[4];//平行光源(方向)
float CRender3D::m_light_pos[4];

ID3D11VertexShader* CRender3D::m_pVertexShader;//バーテックスシェーダー
ID3D11PixelShader* CRender3D::m_pPixelShader;//ピクセルシェーダー
ID3D11InputLayout* CRender3D::m_pVertexLayout;//頂点入力レイアウト
ID3D11Buffer* CRender3D::m_pConstantBuffer;//コンスタントバッファ

//HLSLソースコード(メモリ内登録)
const char* g_hlsl_sauce_code=
{
	//CPUから取得する頂点情報構造体
	"struct vertexIn                                                         \n"
	"{                                                                       \n"
	"  float4 pos : POSITION;                                                \n"
	"  float4 col : COLOR;                                                   \n"
	"  float4 Nor : NORMAL;                                                  \n"
	"  float2 uv  : UV;\n                                                    \n"
	"};                                                                      \n"
	//VSからPSに送る情報
	"struct vertexOut                                                        \n"
	"{                                                                       \n"
	"  float4  pos  : SV_POSITION;                                           \n"
	"  float4  col  : COLOR;                                                 \n"
	"  float2  uv   : UV;                                                    \n"
	"  float3  nor  : NORMAL;                                                \n"
	"  float3  pos_c: POSITION_COLOR;                                        \n"
	"};                                                                      \n"
	//コンスタントバッファ受取先
	"cbuffer global                                                          \n"
	"{                                                                       \n"
	"   float4x4 mat;                                                        \n"
	"   float4x4 w_mat;                                                      \n"
    "   float4   l_vec;                                                      \n"
    "   float4   l_pos;                                                      \n"
	"   float4   amb;                                                        \n"
	"   float4   diff;                                                       \n"
	"   float4   emi;                                                        \n"
	"   float4   sp;                                                         \n"
	"   float4   sp_p;                                                       \n"
	"   float4   eye;                                                        \n"
	"};                                                                      \n"
	//テクスチャ情報
	"Texture2D      txDiffuse  :  register(t0);                              \n"
	"SamplerState   samLinear  :  register(s0);                              \n"
	//頂点シェーダ
	"vertexOut  vs(vertexIn IN)                                              \n"
	"{                                                                       \n"
	"  vertexOut  OUT;                                                       \n"
	"  OUT.pos  =mul(IN.pos,transpose(mat));                                 \n"
	"  OUT.col  =IN.col;                                                     \n"
	"  OUT.uv   =IN.uv;                                                      \n"
	"  OUT.nor  =mul(IN.Nor,(float3x3)transpose(w_mat));                     \n"
	"  OUT.pos_c=mul(IN.pos,transpose(w_mat));                               \n"
	"  return OUT;                                                           \n"
	"}                                                                       \n"
	//ピクセルシェーダ
	"float4 ps(vertexOut IN) : SV_Target                                     \n"
	"{                                                                       \n"
	"  float4   col=IN.col;                                                  \n"
	"  float4 light_vec_sc=(float4)1.0f;                                     \n"
	"  float4 light_pos_sc=(float4)1.0f;                                     \n"
	"                                                                        \n"
	"  if(l_vec.w != 0.0f&&any(IN.nor)==true)                                \n"
	"  {                                                                     \n"
	"      light_vec_sc.rgb*=dot(normalize(IN.nor),normalize(-l_vec));       \n"
	"      light_vec_sc.rgb=saturate(light_vec_sc);                          \n"
	"  }                                                                     \n"
	"                                                                        \n"
	"  if(l_pos.w>0.0f && any(IN.nor)==true)                                 \n"
	"  {                                                                     \n"
	"     float3 lp_len=l_pos.xyz - IN.pos_c;                                \n"
	"     float  len=length(lp_len);                                         \n"
	"     float  w=saturate(len / l_pos.w);                                  \n"
	"     light_pos_sc.rgb=dot(normalize(IN.nor),normalize(lp_len));         \n"
	"     light_pos_sc.rgb=light_pos_sc.rgb*(float3)1.0-w;                   \n"
	"  }                                                                     \n"
	"                                                                        \n"
	"  float4 d=(float4)1.0f;                                                \n"
	"  if(any(diff)==true)                                                   \n"
	"  {                                                                     \n"
	"     d=max(light_pos_sc,light_vec_sc)*diff;                             \n"
	"  }                                                                     \n"
	"                                                                        \n"
	"  float4 a=(float4)0.0f;                                                \n"
	"  if(any(amb)==true)                                                    \n"
	"  {                                                                     \n"
	"     a=amb;                                                             \n"
	"  }                                                                     \n"
	"                                                                        \n"
	"  col=col*d+a;                                                          \n"
	"  col=saturate(col);                                                    \n"
	"                                                                        \n"
	"  float4 vec_sp=(float4)0.0f;vec_sp.a=1.0f;                             \n"
	"  float4 pos_sp=(float4)0.0f;pos_sp.a=1.0f;                             \n"
	"  if(any(sp)==true && any(eye)==true)                                   \n"
	"  {                                                                     \n"
	"     float3 e=normalize(eye.rgb);                                       \n"
	"     float3 n=normalize(IN.nor.rgb);                                    \n"
	"     float3 l=-normalize(l_vec.rgb);                                    \n"
	"     if(l_vec.w != 0.0f && any(IN.nor)==true)                           \n"
	"     {                                                                  \n"
	"        if(dot(n,l)>=0.0f)                                              \n"
	"        {                                                               \n"
	"           float3 rv=normalize(n*dot(n,l)*2.0-l);                       \n"
	"           vec_sp.rgb=pow(max(0,dot(rv,e)),sp_p.x);                     \n"
	"           float3 no=(float3)((sp_p.x+2)/(2*3.14));                     \n"
	"           vec_sp.rgb=sp.rgb*saturate(no*vec_sp.rgb);                   \n"
	"        }                                                               \n"
	"     }                                                                  \n"
	"                                                                        \n"
	"     if(l_pos.w>0.0f && any(IN.nor)==true)                              \n"
	"     {                                                                  \n"
	"        float3 lp_len=IN.pos_c-l_pos.xyz;                               \n"
	"        float  len=length(lp_len);                                      \n"
	"        float3 nor_lp=-normalize(lp_len);                               \n"
	"        float  w=saturate(len / l_pos.w);                               \n"
	"        if(dot(n,nor_lp)>=0.0f)                                         \n"
	"        {                                                               \n"
	"           float3 rv=normalize(n*dot(n,nor_lp)*2.0-nor_lp);             \n"
	"           pos_sp.rgb=pow(max(0,dot(rv,e)),sp_p.x);                     \n"
	"           float3 no=(float3)((sp_p.x+2)/(2*3.14));                     \n"
	"           pos_sp.rgb=saturate(no*pos_sp.rgb)*((float3)1.0-w);          \n"
	"        }                                                               \n"
	"     }                                                                  \n"
	"  }                                                                     \n"
	"                                                                        \n"
	"  col=col*d+vec_sp+a;                                                   \n"
	"  col=saturate(col);                                                    \n"
	"                                                                        \n"
	"  if(any(emi)==true)                                                    \n"
	"  {                                                                     \n"
	"     col=max(col,emi);                                                  \n"
	"  }                                                                     \n"
	"                                                                        \n"
	"  float4 tex=(float4)0.0f;                                              \n"
	"  tex=txDiffuse.Sample(samLinear,IN.uv);                                \n"
	"  float x,y;                                                            \n"
	"  txDiffuse.GetDimensions(x,y);                                         \n"
	"  if(x!=0.0f && y!=0.0f)                                                \n"
	"     col*=tex;                                                          \n"
	"  if(col.a<=0.0f)                                                       \n"
	"     discard;                                                          \n"
	"  return col;                                                           \n"
	"}                                                                       \n"

};

void CRender3D::Init()
{
	//平行ライト値の初期化
	memset(m_light_vector, 0x00, sizeof(m_light_vector));
	//点ライトの初期化
	memset(m_light_pos, 0x00, sizeof(m_light_pos));

	HRESULT hr = S_OK;
	//hlslファイル読み込みブロブ作成　ブロブとはシェーダーの塊みたいなもの
	//XXXシェーダーとして特徴を持たない。後で各シェーダーとなる
	ID3DBlob *pCompiledShader = NULL;
	ID3DBlob *pErrors = NULL;

	//メモリ内のHLSLのvs関数部分のコンパイル
	hr = D3DCompile(g_hlsl_sauce_code, strlen(g_hlsl_sauce_code), 0, 0, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"vs", "vs_4_0", 0, 0, &pCompiledShader, &pErrors);

	if (FAILED(hr))
	{
		//エラーがある場合、cがデバック情報を持つ
		char* c = (char*)pErrors->GetBufferPointer();
		MessageBox(0, L"3Dhlsl読み込み失敗1", NULL, MB_OK);
		SAFE_RELEASE(pErrors);
		return;
	}

	//コンパイルしたバーテックスシェーダーを元にインターフェースを作成
	hr = Dev::GetDevice()->CreateVertexShader(pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(),
		NULL, &m_pVertexShader);
	if (FAILED(hr))
	{
		SAFE_RELEASE(pCompiledShader);
		MessageBox(0, L"バーテックスシェーダー作成失敗", NULL, MB_OK);
		return;
	}

	//頂点インプットレイアウトを定義
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION"	,0,DXGI_FORMAT_R32G32B32_FLOAT		,0	,0	,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"NORMAL"	,0,DXGI_FORMAT_R32G32B32_FLOAT		,0	,12	,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"UV"		,0,DXGI_FORMAT_R32G32_FLOAT			,0	,24	,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR"	,0,DXGI_FORMAT_R32G32B32A32_FLOAT	,0	,32	,D3D11_INPUT_PER_VERTEX_DATA,0},
	};
	UINT numElements = sizeof(layout) / sizeof(layout[0]);

	//頂点インプットレイアウトを作成・レイアウトをセット
	hr = Dev::GetDevice()->CreateInputLayout(layout, numElements, pCompiledShader->GetBufferPointer(),
		pCompiledShader->GetBufferSize(), &m_pVertexLayout);
	if (FAILED(hr))
	{
		MessageBox(0, L"レイアウト作成失敗", NULL, MB_OK);
		return;
	}
	SAFE_RELEASE(pCompiledShader);

	//メモリ内のHLSLのps関数をコンパイル
	hr = D3DCompile(g_hlsl_sauce_code, strlen(g_hlsl_sauce_code), 0, 0, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, "ps", "ps_4_0", 0, 0, &pCompiledShader, &pErrors);
	if (FAILED(hr))
	{
		//エラーがある場合、cがデバック情報を持つ
		char* c = (char*)pErrors->GetBufferPointer();
		MessageBox(0, L"3Dhlsl読み込み失敗2", NULL, MB_OK);
		SAFE_RELEASE(pErrors);
		return;
	}

	//コンパイルしたピクセルシェーダでインターフェースを作成
	hr = Dev::GetDevice()->CreatePixelShader(pCompiledShader->GetBufferPointer(),
		pCompiledShader->GetBufferSize(), NULL, &m_pPixelShader);
	if (FAILED(hr))
	{
		SAFE_RELEASE(pCompiledShader);
		MessageBox(0, L"ピクセルシェーダー作成失敗", NULL, MB_OK);
		return;
	}
	SAFE_RELEASE(pCompiledShader);

	//バッファにコンスタントバッファ(シェーダにデータ受け渡し用)ステータスを設定
	D3D11_BUFFER_DESC cb;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.ByteWidth = sizeof(CMODEL3D_BUFFER);
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;
	cb.Usage = D3D11_USAGE_DYNAMIC;

	//ステータスを元にコンスタントバッファを作成
	hr = Dev::GetDevice()->CreateBuffer(&cb, NULL, &m_pConstantBuffer);
	if (FAILED(hr))
	{
		MessageBox(0, L"コンスタントバッファ作成失敗", NULL, MB_OK);
		return;
	}
}



void CRender3D::Delete()
{
	SAFE_RELEASE(m_pConstantBuffer);
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pVertexLayout);
	SAFE_RELEASE(m_pVertexShader);
}

//モデルをレンダリングする
void CRender3D::Render(CMODEL* model, float mat[16], float mat_w[16],float v_eye[3])
{
	//頂点レイアウト
	Dev::GetDeviceContext()->IASetInputLayout(m_pVertexLayout);

	//使用するシェーダーの登録
	Dev::GetDeviceContext()->VSSetShader(m_pVertexShader, NULL, 0);
	Dev::GetDeviceContext()->PSSetShader(m_pPixelShader, NULL, 0);

	//コンスタントバッファを使用するシェーダに登録
	Dev::GetDeviceContext()->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
	Dev::GetDeviceContext()->PSSetConstantBuffers(0, 1, &m_pConstantBuffer);

	//プリミティブ・トポロジーをセットD3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
	Dev::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (int i = 0; i < model->m_material_max; i++)
	{
		//バーテックスバッファ登録
		UINT stride = sizeof(CPOINT3D_LAUOUT);
		UINT offset = 0;
		Dev::GetDeviceContext()->IASetVertexBuffers(0, 1, &model->m_ppVertexBuffer[i], &stride, &offset);
		//インデックスバッファ登録
		Dev::GetDeviceContext()->IASetIndexBuffer(model->m_ppIndexBuffer[i], DXGI_FORMAT_R16_UINT, 0);

		//コンスタントバッファのデータ登録
		D3D11_MAPPED_SUBRESOURCE pData;
		if (SUCCEEDED(Dev::GetDeviceContext()->Map(m_pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
		{
			CMODEL3D_BUFFER data;

			//トランスフォーム行列情報コンスタントバッファに渡す
			if (mat == nullptr)
			{
				Math3D::IdentityMatrix(data.m_mat);//無い場合は単位行列を渡す
			}
			else
			{
				memcpy(data.m_mat, mat, sizeof(float) * 16);
			}

			//法線用のワールド行列を渡す
			if (mat_w == nullptr)
			{
				Math3D::IdentityMatrix(data.m_mat_w);//無い場合は単位行列を渡す
			}
			else
			{
				memcpy(data.m_mat_w, mat_w, sizeof(float) * 16);
			}

			//視野方向ベクトルを渡す
			if (v_eye == nullptr)
			{
				memset(data.m_eye, 0x00, sizeof(data.m_eye));
			}
			else
			{
				data.m_eye[0] = v_eye[0];
				data.m_eye[1] = v_eye[1];
				data.m_eye[2] = v_eye[2];
				data.m_eye[3] = 1.0f;
			}

			//平行ライトの値を渡す
			memcpy(data.m_light_vec, m_light_vector, sizeof(m_light_vector));
			//点ライトの値を渡す
			memcpy(data.m_light_pos, m_light_pos, sizeof(m_light_pos));

			//材質、アンビエントを渡す
			memcpy(data.m_ambient, model->m_Material[i].m_ambient, sizeof(data.m_ambient));
			//材質、デフィーズを渡す
			memcpy(data.m_diffuse, model->m_Material[i].m_diffuse, sizeof(data.m_diffuse));
			//材質、エミッシブを渡す
			memcpy(data.m_emissive, model->m_Material[i].m_emissive, sizeof(data.m_emissive));
			//材質、スぺキュラを渡す
			memcpy(data.m_specular, model->m_Material[i].m_specular, sizeof(data.m_specular));
			for (int j = 0; j < 4; j++)
				data.m_specular_power[j] = model->m_Material[i].m_specular_power;

			memcpy_s(pData.pData, pData.RowPitch, (void*)&data, sizeof(CMODEL3D_BUFFER));
			//コンスタントバッファをシェーダに転送
			Dev::GetDeviceContext()->Unmap(m_pConstantBuffer, 0);
		}
		//テクスチャーサンプラを登録
		Dev::GetDeviceContext()->PSSetSamplers(0, 1, Draw::GetSamplerState());
		//テクスチャを登録
		Dev::GetDeviceContext()->PSSetShaderResources(0, 1, &model->m_Material[i].m_pTexture);

		//登録した情報を元にポリゴンを描画
		Dev::GetDeviceContext()->DrawIndexed(model->m_pindex_size[0] * 3, 0, 0);
	}
}

//平行光源の向きを入れる
void CRender3D::SetLightVec(float x, float y, float z, bool light_on)
{
	//ライトの光線方向
	m_light_vector[0] = x;
	m_light_vector[1] = y;
	m_light_vector[2] = z;

	//light_onでライトの有無を決めるようにする
	if (light_on == true)
	{
		m_light_vector[3] = 1.0f;//有り
	}
	else 
	{
		m_light_vector[3] = 0.0f;//無し
	}
}

void CRender3D::SetLightPos(float x, float y, float z, float max)
{
	m_light_pos[0] = x;
	m_light_pos[1] = y;
	m_light_pos[2] = z;
	m_light_pos[3] = max;
}
