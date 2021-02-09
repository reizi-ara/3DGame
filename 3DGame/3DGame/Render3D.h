#pragma once
//3D���f���`��---
#include <Windows.h>
#include <D3D11.h>

#include <d3dcompiler.h>
//����}�N��---
#define SAFE_DELETE(p)	  {if(p){delete (p);		(p)=nullptr;}}
#define SAFE_DELETE_ARRAY {if(p){delete[](p);		(p)=nullptr;}}
#define SAFE_RELEASE(p)	  {if(p){(p)->Release();	(p)=nullptr;}}

//�R���X�^���g�o�b�t�@�N���X
class CMODEL3D_BUFFER 
{
	public:
		float m_mat[16];//�g�����X�t�H�[���p�s��
		float m_mat_w[16];//�@���p�̃��[���h�s��
		float m_light_vec[4];//���s���C�g����
		float m_light_pos[4];//���s���C�g����
		float m_ambient[4];//�A���r�G���g
		float m_diffuse[4];//�f�t�B�[�Y
		float m_emissive[4];//�G�~�b�V�u
		float m_specular[4];//�X�؃L����
		float m_specular_power[4];//�X�؃L�����p���[
		float m_eye[4];
};

//���_���C�A�E�g�N���X(���_�������)
class CPOINT3D_LAUOUT
{
	public:
		float m_pos[3];//���_
		float m_normal[3];//�@��
		float m_uv[2];//�e�N�X�`���ʒu
		float m_coloar[4];//�F
};

//�}�e���A���N���X(�ގ����)
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

//���f���N���X
class CMODEL
{
	public:
		~CMODEL();
		int* m_pvertex_size;//�ő咸�_��
		int* m_pindex_size;//�ő�ʐ�
		CMATERIAL* m_Material;//���f���̍ގ����
		ID3D11Buffer** m_ppVertexBuffer;//�o�[�e�b�N�X�o�b�t�@
		ID3D11Buffer** m_ppIndexBuffer;//�C���f�b�N�X�o�b�t�@
		int m_material_max;

		//�O�p��Model�쐬
		void CreateSampleTriangularpyramid();//�T���v���p�̎O�p��
		//com�t�@�C���ǂݍ���
		void LoadComModel(const wchar_t* name); //com�t�@�C���ǂݍ���
};

//3D�����_�����O�N���X
typedef class CRender3D
{
	public:
		CRender3D(){}
		~CRender3D(){}
		static void Init();
		static void Delete();

		static void Render(CMODEL* model, float mat[16], float mat_w[16],float v_eye[3]);
		static void SetLightVec(float x, float y, float z, bool light_on);//���s�����̌���
		static void SetLightPos(float x, float y, float z, float max);//���s�����̌���

	private:
		static float m_light_vector[4];//���s����(����)
		static float m_light_pos[4];//�_����
		static ID3D11VertexShader* m_pVertexShader;//�o�[�e�b�N�X�V�F�[�_�[
		static ID3D11PixelShader* m_pPixelShader;//�s�N�Z���V�F�[�_�[
		static ID3D11InputLayout* m_pVertexLayout;//���_���̓��C�A�E�g
		static ID3D11Buffer* m_pConstantBuffer;//�R���X�^���g�o�b�t�@


}Render;