#pragma once

#ifndef _TEXTURECLASS_H_
#define _TEXTURECLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <stdio.h>

// TexureClass�� ���� �ؽ�ó ���ҽ��� �ε�, ��ε� �� �׼����� ĸ��ȭ�մϴ�.
// �ʿ��� �� �ؽ�ó�� ���� �� Ŭ������ ��ü�� �ν��Ͻ�ȭ�ؾ� �մϴ�.

////////////////////////////////////////////////////////////////////////////////
// Class name: TextureClass
////////////////////////////////////////////////////////////////////////////////
class TextureClass
{
private:
    struct TargaHeader
    {
        // ���⼭�� �����͸� �� ���� ���� �� �ֵ��� Targa ���� ��� ������ �����մϴ�.
        unsigned char data1[12];
        unsigned short width;
        unsigned short height;
        unsigned char bpp;
        unsigned char data2;
    };

public:
    TextureClass();
    TextureClass(const TextureClass&);
    ~TextureClass();

    bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*);
    void Shutdown();

    ID3D11ShaderResourceView* GetTexture();

    int GetWidth();
    int GetHeight();

private:
    // Targa �б� ���, �� ���� ������ �����Ϸ��� ���⿡ �б� ����� �߰�
    bool LoadTarga32Bit(char*);

private:
    // �� Ŭ�������� 5���� ��� ������ �ֽ��ϴ�.
    unsigned char* m_targaData; // ���Ͽ��� ���� ���� ���� Targa �����͸� ����
    ID3D11Texture2D* m_texture; // DirectX�� �������� ����� ����ȭ �� �ؽ�ó �����͸� ����
    ID3D11ShaderResourceView* m_textureView; // ���̴��� �׸��� �׸� �� �ؽ�ó �����Ϳ� �׼����ϴµ� ����ϴ� ���ҽ� ��
    int m_width, m_height; // �ؽ�ó�� ũ��
};

#endif
