//CPUから取得する頂点情報構造体
struct vertexIn
{
	float4 pos:POSITION;
	float4 col:COLOR;
	float2 uv:UV;
};

//VSからPSに送る情報
struct vertexOut
{
	//頂点情報はラスタライズで使用
	float4 pos:SV_POSITION;
	//以下、PSで使用する
	float4 col:COLOR;
	float2 uv:UV;
};

//グローバル
cbuffer global
{
	float4 color;
	float4 pos;
	float4 scale;
	float4 rotation;
	float4 texsize;
};

//テクスチャ情報
Texture2D txDiffuse:register(t0);//テクスチャのグラフィックス
SamplerState samLinear:register(s0);//テクスチャサンプラ

//頂点シェーダ
//引数　vertexIn：CPUから受け取る頂点情報
//戻り値　vertexOut：PSに送る情報
//頂点情報を座標変換させるが今回は変換させていない。
vertexOut vs(vertexIn IN)
{
	vertexOut OUT;
	float2 p;

	//原点にポリゴンの中心を移動させる
	p.x = IN.pos.x - 0.5f;
	p.y = IN.pos.y - 0.5f;

	//ポリゴンを拡大させる
	p.x = p.x*scale.x*texsize.x / 400.0f;
	p.y = p.y*scale.y*texsize.y / 300.0f;

	//ポリゴンを原点を中心に回転
	float r = 3.14f / 180.0f*rotation.x;
	OUT.pos.x = p.x*cos(r) - p.y*sin(r)*3.0f / 4.0f;
	OUT.pos.y = p.y*cos(r) + p.x*sin(r)*4.0f / 3.0f;
	/*//拡大率
	OUT.pos.x = IN.pos.x*scale.x;
	OUT.pos.y = IN.pos.y*scale.y;
	*/

	//ポりゴンを元の位置に移動させる（拡大分を考慮）
	OUT.pos.x += 0.5f*scale.x*texsize.x / 400.0f;
	OUT.pos.y += 0.5f*scale.y * texsize.y / 300.0f;

	//2D座標への変換
	OUT.pos.x = +(OUT.pos.x) - 1.0f;
	OUT.pos.y = -(OUT.pos.y) + 1.0f;

	//平行移動
	OUT.pos.x += (pos.x / 400.0f);
	OUT.pos.y += (-pos.y / 300.0f);

	//INからOUTへそのまま流す
	OUT.pos.zw = IN.pos.zw;
	OUT.col = IN.col;//色
	OUT.uv = IN.uv;	 //UV

	return OUT;
}

//ピクセルシェーダー
//引数　vertexOut：VSから送られて来た情報
//戻り値　float4：Color値
//引数の情報元に色を決める。今回は頂点が持つ色とグローバル
//から持ってきた色を乗算している
float4 ps(vertexOut IN) :SV_Target
{
	float4 col = IN.col * color;

	if (texsize.z == 1.0f)
	{
		//UVからテクスチャの色の値を取得
		float4 tex = txDiffuse.Sample(samLinear, IN.uv);
		col *= tex;
	}
	else
	{

	}

	return col;
}