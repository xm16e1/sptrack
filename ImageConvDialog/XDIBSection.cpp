// DIBSection.cpp: CXDIBSection �N���X�̃C���v�������e�[�V����
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XDIBSection.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// �\�z/����
//////////////////////////////////////////////////////////////////////

/********************************************************
 * Func: CXDIBSection �N���X�̃R���X�g���N�^
 * �����o�ϐ���������
 ********************************************************/
CXDIBSection::CXDIBSection()
{
	m_hBmp = NULL;
	m_pBits = NULL;
	m_sizeBmp = CSize(0, 0);
}

/********************************************************
 * Func: CXDIBSection �N���X�̃f�X�g���N�^
 * �r�b�g�}�b�v�����
 ********************************************************/
CXDIBSection::~CXDIBSection()
{
	if (m_hBmp)
		::DeleteObject(m_hBmp);		// ����� m_pBits ����������
}

/********************************************************
 * Func: BITMAPINFOHEADER �̓��e��ݒ�
 * ��� 24 �r�b�g�J���[
 ********************************************************/
static void
FillBitmapInfoHeader(
	LPBITMAPINFOHEADER pbmih,		// (B) ���e��ݒ肷��o�b�t�@
	CSize size						// (I) �r�b�g�}�b�v�̃T�C�Y
)
{
	pbmih->biSize = sizeof(BITMAPINFOHEADER);
	pbmih->biWidth = size.cx;
	pbmih->biHeight = size.cy;
	pbmih->biPlanes = 1;
	pbmih->biBitCount = 24;				// 24 �r�b�g�Œ�
	pbmih->biCompression = BI_RGB;
	pbmih->biSizeImage = 0;
	pbmih->biXPelsPerMeter = 0;
	pbmih->biYPelsPerMeter = 0;
	pbmih->biClrUsed = 0;
	pbmih->biClrImportant = 0;
}

/********************************************************
 * Func: �w�肳�ꂽ�T�C�Y�̃r�b�g�}�b�v���쐬
 ********************************************************/
BOOL					// TRUE->OK�AFALSE->�G���[
CXDIBSection::Create(
	CDC *pdc,			// (I) �r�b�g�}�b�v�쐬�Ɏg�� DC
	CSize size			// (I) �쐬����r�b�g�}�b�v�̃T�C�Y
)
{
	if (m_hBmp != NULL) {
		TRACE("CXDIBSection created twice\n");
		return FALSE;
	}

	BITMAPINFO bmi;

	m_sizeBmp = size;
	FillBitmapInfoHeader(&bmi.bmiHeader, size);
	m_hBmp = ::CreateDIBSection(pdc->GetSafeHdc(), &bmi, DIB_RGB_COLORS, (void **)&m_pBits, NULL, 0);

	m_nRowStride = FitTo4Byte(3 * size.cx);

	return m_hBmp != NULL;
}

/********************************************************
 * Func: 4�̔{���ɐ؂�グ��
 ********************************************************/
int				// 4�̔{���ɐ؂�グ������
CXDIBSection::FitTo4Byte(
	int n				// (I) ���̐�
)
{
	int nMod = n % 4;
	if (nMod != 0)
		n += (4 - nMod);

	return n;
}

/********************************************************
 * Func: 2�̔{���ɐ؂�グ��
 ********************************************************/
int				// 2�̔{���ɐ؂�グ������
CXDIBSection::FitTo2Byte(
	int n				// (I) ���̐�
)
{
	if (n % 2 != 0)
		++n;

	return n;
}
/********************************************************
 * Func: �r�b�g�}�b�v��L�k���ĕ`��
 ********************************************************/
void
CXDIBSection::StretchDraw(
	CDC *pdc,					// (O) �`�悷�� DC
	const CRect *prectDraw		// (I) �`���̋�`
) const
{
	CDC dcMem;
	VERIFY(dcMem.CreateCompatibleDC(pdc));

	HGDIOBJ hBmpOld = ::SelectObject(dcMem.GetSafeHdc(), GetHBitmap());
	pdc->StretchBlt(prectDraw->left, prectDraw->top, prectDraw->Width(), prectDraw->Height(), &dcMem, 0, 0, m_sizeBmp.cx, m_sizeBmp.cy, SRCCOPY);
	::SelectObject(dcMem.GetSafeHdc(), hBmpOld);
}


void CXDIBSection::CopyToCADIBSection(CADIBSection *pAdib) const
{
	int x;
	int y;
	unsigned int col;

	if(pAdib->GetSizex()!=m_sizeBmp.cx || pAdib->GetSizey()!=m_sizeBmp.cy){
		pAdib->ChangeSize(m_sizeBmp.cx, m_sizeBmp.cy);
	}

	for(y=0; y<m_sizeBmp.cy; y++) {
		for(x=0; x<m_sizeBmp.cx; x++) {
			col=(unsigned int)GetPixel(x,y);
			pAdib->SetPixel(x, y, GetRValue(col), GetGValue(col), GetBValue(col));

		}
	}
}


void CXDIBSection::CopyFromCADIBSection(const CADIBSection *pAdib)
{
	int x;
	int y;
	for(y=0; y<m_sizeBmp.cy; y++) {
		for(x=0; x<m_sizeBmp.cx; x++) {

			SetPixel(x, y, 
				RGB(pAdib->GetValueR(x,y), pAdib->GetValueG(x,y), pAdib->GetValueB(x,y)));

		}
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// CreateFromBMP.cpp: Bitmap �t�@�C������쐬
//
//////////////////////////////////////////////////////////////////////

/********************************************************
 * Func: BMP �t�@�C������摜��ǂݍ���
 ********************************************************/
BOOL						// TRUE->OK�AFALSE->�G���[
CXDIBSection::ReadBmpFile(
	CDC *pdc,				// (I) �r�b�g�}�b�v�쐬�Ɏg�� DC
	HANDLE hBmp,			// (I) �ǂݍ��݌��̃r�b�g�}�b�v�n���h��
	const CSize *pSize		// (I) ���̃T�C�Y�ɃX�P�[�����O���č쐬�BNULL�Ȃ�ABMP�t�@�C���Ɠ����T�C�Y
)
{
	BITMAP bm;
	if (GetObject(hBmp, sizeof(bm), &bm) == 0)
		return FALSE;

	CSize sizeCreate;	// �쐬����DIB�̃T�C�Y
	if (pSize != NULL)
		sizeCreate = *pSize;
	else
		sizeCreate = CSize(bm.bmWidth, bm.bmHeight);

	if (!Create(pdc, sizeCreate))
		return FALSE;

	BOOL bRet = FALSE;
	HDC hdcSrc	= ::CreateCompatibleDC(pdc->m_hDC);
	HDC hdcDest = ::CreateCompatibleDC(pdc->m_hDC);
	if (hdcSrc != NULL && hdcDest != NULL) {
		HGDIOBJ hbmpOrg1 = ::SelectObject(hdcSrc, hBmp);
		HGDIOBJ hbmpOrg2 = ::SelectObject(hdcDest, m_hBmp);
		if (pSize != NULL) {
			::SetStretchBltMode(hdcDest, HALFTONE);
			::StretchBlt(hdcDest, 0, 0, sizeCreate.cx, sizeCreate.cy, hdcSrc, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
		}
		else
			::BitBlt(hdcDest, 0, 0, sizeCreate.cx, sizeCreate.cy, hdcSrc, 0, 0, SRCCOPY);
		::SelectObject(hdcSrc,	hbmpOrg1);
		::SelectObject(hdcDest, hbmpOrg2);
		bRet = TRUE;
	}

	if (hdcSrc != NULL)
		DeleteDC(hdcSrc);
	if (hdcDest != NULL)
		DeleteDC(hdcDest);

	return bRet;
}

/********************************************************
 * Func: BMP �t�@�C������ DIB ���쐬����
 * �t���J���[ BMP �t�@�C���̂ݑΉ�
 ********************************************************/
BOOL						// TRUE->OK�AFALSE->�G���[
CXDIBSection::CreateFromBMPFile(
	CDC *pdc,				// (I) �r�b�g�}�b�v�쐬�Ɏg�� DC
	LPCSTR lpszFileName,	// (I) BMP �t�@�C����
	const CSize *pSize		// (I) ���̃T�C�Y�ɃX�P�[�����O���č쐬�BNULL�Ȃ�ABMP�t�@�C���Ɠ����T�C�Y
)
{
	HANDLE hBmp = LoadImage(NULL, lpszFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	if (hBmp == NULL)
		return FALSE;

	BOOL bRet = ReadBmpFile(pdc, hBmp, pSize);
	DeleteObject(hBmp);

	return bRet;
}

/********************************************************
 * Func: �r�b�g�}�b�v�t�@�C���ɃZ�[�u
 ********************************************************/
BOOL						// TRUE->OK�AFALSE->�G���[
CXDIBSection::SaveToBitmapFile(
	LPCSTR lpszFileName		// (I) �Z�[�u����t�@�C����
) const
{
	BOOL bRet = TRUE;
	try {
		CStdioFile f(lpszFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
		DIBSECTION dibs;
		GetObject(m_hBmp, sizeof(dibs), &dibs);

		DWORD dwImageSize = m_nRowStride * dibs.dsBmih.biHeight;
		ASSERT(dibs.dsBmih.biSizeImage == 0 || dibs.dsBmih.biSizeImage == dwImageSize);

		BITMAPFILEHEADER bmfh;
		ZeroMemory(&bmfh, sizeof(bmfh));
		bmfh.bfType = *(WORD *)"BM";
		bmfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwImageSize;
		bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		f.Write(&bmfh, sizeof(bmfh));

		f.Write(&dibs.dsBmih, sizeof(BITMAPINFOHEADER));

		f.Write(m_pBits, dwImageSize);

	}
	catch (CFileException *e) {
		bRet = FALSE;
		CHAR	szCause[256];
		e->GetErrorMessage(szCause, 256);
		TRACE("Error: %s", szCause);
		e->Delete();
	}

	return bRet;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// JPEG.cpp: JPEG �C���^�[�t�F�[�X�֘A�֐�
//
//////////////////////////////////////////////////////////////////////

#include <setjmp.h>

extern "C" {
	#include <jpeglib.h>
}

// C++ �� setjmp ���g���ƌx�����ł邪�AJPEG ���C�u�������g���ɂ� setjmp ��
// �K�v�Ȃ̂ŁA���̌x�����o���Ȃ��悤�ɂ���
#pragma warning(disable : 4611)

struct my_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */
  jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr *my_error_ptr;


METHODDEF(void)
CXDIBSection_JPEGErrorExit(j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = (my_error_ptr) cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
//	(*cinfo->err->output_message) (cinfo);	// �G���[���b�Z�[�W�͂����ł͕\�����Ȃ�

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}

/********************************************************
 * Func: JPEG ���C�u�����̑������o�b�t�@����A�P�s���̃f�[�^���擾
 ********************************************************/
void
CXDIBSection::GetLineBitsFromJPEGScanline(
	int y,							// (I) y ���W
	const BYTE *pbyJPEGScanLine,	// (I) JPEG ���C�u�����̑������o�b�t�@
	int nComponents					// (I) �F������
)
{
	ASSERT(m_pBits);
	ASSERT(0 <= y && y < m_sizeBmp.cy);
	ASSERT(pbyJPEGScanLine);
	ASSERT(nComponents == 3 || nComponents == 1);

	y = m_sizeBmp.cy - y - 1;	// �㉺���]
	BYTE *pbyBmpScanLine = m_pBits + m_nRowStride * y;	// 1 �s�N�Z�� 3 �o�C�g�Œ�
	if (nComponents == 3) {				// FullColor JPEG
		for (int x = 0; x < m_sizeBmp.cx; x++) {
			*pbyBmpScanLine++ = pbyJPEGScanLine[2];
			*pbyBmpScanLine++ = pbyJPEGScanLine[1];
			*pbyBmpScanLine++ = pbyJPEGScanLine[0];
			pbyJPEGScanLine += 3;
		}
	}
	else {								// GrayScale JPEG
		for (int x = 0; x < m_sizeBmp.cx; x++) {
			*pbyBmpScanLine++ = *pbyJPEGScanLine;
			*pbyBmpScanLine++ = *pbyJPEGScanLine;
			*pbyBmpScanLine++ = *pbyJPEGScanLine;
			pbyJPEGScanLine++;
		}
	}
}

/********************************************************
 * Func: JPEG ���C�u�����̑������o�b�t�@�ɁA�P�s���̃f�[�^��ݒ�
 ********************************************************/
void
CXDIBSection::SetLineBitsToJPEGScanline(
	int y,						// (I) y ���W
	BYTE *pbyJPEGScanLine,		// (B) JPEG ���C�u�����̑������o�b�t�@
	int nComponents				// (I) �F������
) const
{
	ASSERT(m_pBits);
	ASSERT(0 <= y && y < m_sizeBmp.cy);
	ASSERT(pbyJPEGScanLine);
	ASSERT(nComponents == 3 || nComponents == 1);

	y = m_sizeBmp.cy - y - 1;	// �㉺���]
	const BYTE *pbyBmpScanLine = m_pBits + m_nRowStride * y;	// 1 �s�N�Z�� 3 �o�C�g�Œ�
	if (nComponents == 3) {				// FullColor JPEG
		for (int x = 0; x < m_sizeBmp.cx; x++) {
			pbyJPEGScanLine[2] = *pbyBmpScanLine++;
			pbyJPEGScanLine[1] = *pbyBmpScanLine++;
			pbyJPEGScanLine[0] = *pbyBmpScanLine++;
			pbyJPEGScanLine += 3;
		}
	}
	else {								// GrayScale JPEG
		for (int x = 0; x < m_sizeBmp.cx; x++) {
			*pbyJPEGScanLine = *pbyBmpScanLine;
			pbyJPEGScanLine++;
			pbyBmpScanLine += 3;
		}
	}
}

/********************************************************
 * Func: JPEG �t�@�C������ DIB ���쐬
 ********************************************************/
BOOL						// TRUE->OK�AFALSE->�G���[
CXDIBSection::CreateFromJPEGFile(
	CDC *pdc,				// (I) �r�b�g�}�b�v�쐬�Ɏg�� DC
	LPCSTR lpszFileName		// (I) JPEG �t�@�C����
)
{
	FILE *fpIn = fopen(lpszFileName, "rb");
	if (fpIn == NULL)
		return FALSE;

	BOOL bResult = CreateFromJPEGFile(pdc, fpIn);
	fclose(fpIn);

	return bResult;
}

/********************************************************
 * Func: JPEG �t�@�C������ DIB ���쐬
 ********************************************************/
BOOL						// TRUE->OK�AFALSE->�G���[
CXDIBSection::CreateFromJPEGFile(
	CDC *pdc,				// (I) �r�b�g�}�b�v�쐬�Ɏg�� DC
	FILE *fpIn				// (I) JPEG �t�@�C���ǂݍ��ݗp�̃t�@�C���|�C���^
)
{
	struct jpeg_decompress_struct cinfo;
	struct my_error_mgr jerr;
	BYTE *pRowBuf = NULL;	// jpeg �f�[�^�P�s�ǂݍ��ݗp�o�b�t�@

	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = CXDIBSection_JPEGErrorExit;
	if (setjmp(jerr.setjmp_buffer)) {
		jpeg_destroy_decompress(&cinfo);
		if (pRowBuf) free(pRowBuf);
		return FALSE;
	}

	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, fpIn);
	jpeg_read_header(&cinfo, TRUE);
	if (Create(pdc, CSize(cinfo.image_width, cinfo.image_height)) == NULL) {
		jpeg_destroy_decompress(&cinfo);
		return FALSE;
	}

	jpeg_start_decompress(&cinfo);

	int nRowBufSize = cinfo.output_width * cinfo.output_components;
	if ((pRowBuf = (BYTE *)malloc(nRowBufSize)) == NULL) {
		jpeg_destroy_decompress(&cinfo);
		return FALSE;
	}
	BYTE *aRowArray[1];			// �������̔z��(�P�{����)
	aRowArray[0] = pRowBuf;
	while (cinfo.output_scanline < cinfo.output_height) {
		jpeg_read_scanlines(&cinfo, aRowArray, 1);		// JEPG ����P�s�ǂ�
		GetLineBitsFromJPEGScanline(cinfo.output_scanline - 1, pRowBuf, cinfo.output_components);
	}
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	free(pRowBuf);

	return TRUE;
}

/********************************************************
 * Func:  DIB �� JPEG �t�@�C���ɃZ�[�u
 ********************************************************/
BOOL						// TRUE->OK�AFALSE->�G���[
CXDIBSection::SaveToJPEGFile(
	LPCSTR lpszFileName,	// (I) �Z�[�u����t�@�C����
	int nQuality,			// (I) �i��(0�`100)
	const CRect *prect		// (I) �Z�[�u���钷���`(NULL �Ȃ�S��)
) const
{
	FILE *fpOut = fopen(lpszFileName, "wb");
	if (fpOut == NULL)
		return FALSE;

	BOOL bResult = SaveToJPEGFile(fpOut, nQuality, prect);
	fclose(fpOut);

	return bResult;
}

/********************************************************
 * Func:  DIB �� JPEG �t�@�C���ɃZ�[�u
 ********************************************************/
BOOL						// TRUE->OK�AFALSE->�G���[
CXDIBSection::SaveToJPEGFile(
	FILE *fpOut,			// (I) JPEG �t�@�C�����������ރt�@�C���|�C���^
	int nQuality,			// (I) �i��(0�`100)
	const CRect *prect		// (I) �Z�[�u���钷���`(top <= y <= bottom ���Z�[�u�Ax �����l�ANULL �Ȃ�S��)
) const
{
	const DWORD dwWidth = m_sizeBmp.cx;			// �r�b�g�}�b�v�̕�(Pixel)
	const DWORD dwHeight = m_sizeBmp.cy;		// �r�b�g�}�b�v�̍���(Pixel)
	const DWORD dwFileWidth	 = prect ? prect->Width()  + 1 : dwWidth;	// �Z�[�u����r�b�g�}�b�v�̕�(Pixel)
	const DWORD dwFileHeight = prect ? prect->Height() + 1 : dwHeight;	// �Z�[�u����r�b�g�}�b�v�̍���(Pixel)
	const DWORD dwFileStartX = prect ? prect->left : 0;				// �Z�[�u���鍶�[
	const DWORD dwFileStartY = prect ? prect->top  : 0;				// �Z�[�u�����

	if (dwFileStartX + dwFileWidth > dwWidth || dwFileStartY + dwFileHeight > dwHeight)
		return FALSE;

	BYTE *pbyScanLineBuf = (BYTE *)malloc(3 * dwWidth);
	if (pbyScanLineBuf == NULL) {
		return FALSE;
	}

	struct jpeg_compress_struct cinfo;
	struct my_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = CXDIBSection_JPEGErrorExit;
	if (setjmp(jerr.setjmp_buffer)) {
		jpeg_destroy_compress(&cinfo);
		if (pbyScanLineBuf) free(pbyScanLineBuf);
		return FALSE;
	}

	jpeg_create_compress(&cinfo);
	jpeg_stdio_dest(&cinfo, fpOut);
	cinfo.image_width = dwFileWidth;
	cinfo.image_height = dwFileHeight;
	cinfo.input_components = 3;		/* # of color components per pixel */
	cinfo.in_color_space = JCS_RGB;		/* colorspace of input image */
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, nQuality, TRUE /* limit to baseline-JPEG values */);
	jpeg_start_compress(&cinfo, TRUE);

	BYTE *aRowArray[1];			// �������̔z��(�P�{����)
	aRowArray[0] = pbyScanLineBuf + cinfo.input_components * dwFileStartX;
	while (cinfo.next_scanline < cinfo.image_height) {
		SetLineBitsToJPEGScanline(cinfo.next_scanline + dwFileStartY, pbyScanLineBuf, cinfo.input_components);
		jpeg_write_scanlines(&cinfo, aRowArray, 1);
	}
	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);

	free(pbyScanLineBuf);

	return TRUE;
}
