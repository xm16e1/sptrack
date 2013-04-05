// JPEG.cpp: JPEG �C���^�[�t�F�[�X�֘A�֐�
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XDIBSection.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

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
