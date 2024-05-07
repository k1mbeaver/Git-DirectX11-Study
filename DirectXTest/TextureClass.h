#pragma once

#ifndef _TEXTURECLASS_H_
#define _TEXTURECLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <stdio.h>

// TexureClass는 단일 텍스처 리소스의 로드, 언로드 및 액세스를 캡슐화합니다.
// 필요한 각 텍스처에 대해 이 클래스의 객체를 인스턴스화해야 합니다.

////////////////////////////////////////////////////////////////////////////////
// Class name: TextureClass
////////////////////////////////////////////////////////////////////////////////
class TextureClass
{
private:
    struct TargaHeader
    {
        // 여기서는 데이터를 더 쉽게 읽을 수 있도록 Targa 파일 헤더 구조를 정의합니다.
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
    // Targa 읽기 기능, 더 많은 형식을 지원하려면 여기에 읽기 기능을 추가
    bool LoadTarga32Bit(char*);

private:
    // 이 클래스에는 5개의 멤버 변수가 있습니다.
    unsigned char* m_targaData; // 파일에서 직접 읽은 원시 Targa 데이터를 보유
    ID3D11Texture2D* m_texture; // DirectX가 렌더링에 사용할 구조화 된 텍스처 데이터를 보유
    ID3D11ShaderResourceView* m_textureView; // 셰이더가 그림을 그릴 떄 텍스처 데이터에 액세스하는데 사용하는 리소스 뷰
    int m_width, m_height; // 텍스처의 크기
};

#endif
