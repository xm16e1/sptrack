// XDIBSection.h: CXDIBSection �N���X�̃C���^�[�t�F�C�X
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIBSECTION_H__E7EE5684_F12D_11D1_9CB2_006008B09019__INCLUDED_)
#define AFX_DIBSECTION_H__E7EE5684_F12D_11D1_9CB2_006008B09019__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif


#include "ADIBSection.h"


// Class: DIB Section �N���X�B24 �r�b�g�̂݁B
class CXDIBSection
{
public:
	CXDIBSection();
	virtual ~CXDIBSection();

	BOOL Create(CDC *pdc, CSize size);
	BOOL CreateFromJPEGFile(CDC *pdc, LPCSTR lpszFileName);
	BOOL CreateFromBMPFile(CDC *pdc, LPCSTR lpszFileName, const CSize *pSize = NULL);

	CSize GetSize() const { return m_sizeBmp; }
	HBITMAP	GetHBitmap() const { return m_hBmp; }
//	inline COLORREF GetPixel(int x, int y) const;
//	inline void SetPixel(int x, int y, COLORREF c);
	BOOL SaveToBitmapFile(LPCSTR lpszFileName) const;
//	BOOL SaveToBitmapFile8Gray(LPCSTR lpszFileName) const;
	BOOL SaveToJPEGFile(LPCSTR lpszFileName, int nQuality, const CRect *prect = NULL) const;
	void StretchDraw(CDC *pdc, const CRect *prectDraw) const;

protected:
	HBITMAP	m_hBmp;			// �r�b�g�}�b�v�n���h��
	BYTE	*m_pBits;		// �r�b�g�}�b�v�f�[�^
	CSize	m_sizeBmp;		// �T�C�Y
	int		m_nRowStride;	// �P�s�̃o�C�g��(4�o�C�g���E)

	BOOL ReadBmpFile(CDC *pdc, HANDLE hBmp, const CSize *pSize);
	BOOL CreateFromJPEGFile(CDC *pdc, FILE *fpIn);
	BOOL SaveToJPEGFile(FILE *fpOut, int nQuality, const CRect *prect) const;
	void GetLineBitsFromJPEGScanline(int y, const BYTE *pbyJPEGScanLine, int nComponents);
	void SetLineBitsToJPEGScanline(int y, BYTE *pbyJPEGScanLine, int nComponents) const;
	static int FitTo4Byte(int n);
	static int FitTo2Byte(int n);

	inline COLORREF GetPixel(int x, int y) const;
	inline void SetPixel(int x, int y, COLORREF c);

public:
	void CopyToCADIBSection(CADIBSection *pAdib) const;
	void CopyFromCADIBSection(const CADIBSection *pAdib);


};

/********************************************************
 * Func: �s�N�Z���̐F���擾
 ********************************************************/
inline COLORREF					// �_ (x, y) �̐F
CXDIBSection::GetPixel(
	int x,						// (I) �擾����s�N�Z���� x ���W
	int y						// (I) �擾����s�N�Z���� y ���W
) const
{
	ASSERT(m_pBits);
	ASSERT(0 <= y && y < m_sizeBmp.cy);
	ASSERT(0 <= x && x < m_sizeBmp.cx);

	y = m_sizeBmp.cy - y - 1;	// �㉺���]
	const BYTE *pbyPixel = m_pBits + m_nRowStride * y + x * 3;

	return RGB(pbyPixel[2], pbyPixel[1], pbyPixel[0]);
}

/********************************************************
 * Func: �s�N�Z���̐F��ݒ�
 ********************************************************/
inline void CXDIBSection::SetPixel(int x, int y, COLORREF c)
{
	ASSERT(m_pBits);
	ASSERT(0 <= y && y < m_sizeBmp.cy);
	ASSERT(0 <= x && x < m_sizeBmp.cx);

	y = m_sizeBmp.cy - y - 1;	// �㉺���]
	BYTE *pbyPixel = m_pBits + m_nRowStride * y + x * 3;

	pbyPixel[0] = GetBValue(c);
	pbyPixel[1] = GetGValue(c);
	pbyPixel[2] = GetRValue(c);
}


#endif // !defined(AFX_DIBSECTION_H__E7EE5684_F12D_11D1_9CB2_006008B09019__INCLUDED_)
