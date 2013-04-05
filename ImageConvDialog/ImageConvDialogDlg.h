// ImageConvDialogDlg.h : ヘッダー ファイル
//

#pragma once
#include "afxwin.h"
#include "XDIBSection.h"
#include "ADIBSection.h"


// CImageConvDialogDlg ダイアログ
class CImageConvDialogDlg : public CDialog
{
// コンストラクション
public:
	CImageConvDialogDlg(CWnd* pParent = NULL);	// 標準コンストラクタ

// ダイアログ データ
	enum { IDD = IDD_IMAGECONVDIALOG_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート


// 実装
protected:
	HICON m_hIcon;

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CStatic m_pict1;
	CStatic m_pict2;
	CString m_CstrInputFile;
	afx_msg void OnBnClickedButtonInputfile();
	afx_msg void OnBnClickedButtonStart();


private:
	CADIBSection *m_pAdib1;
	CADIBSection *m_pAdib2;
	int m_dispFlag1;
	int m_dispFlag2;
	CString m_inputFileName;
	void CImageConvDialogDlg::DrawPict(CStatic &m_pict, const CADIBSection *m_pAdib);
public:
	afx_msg void OnBnClickedButtonChangecolor();
	afx_msg void OnBnClickedButtonHistogram();
	afx_msg void OnBnClickedButtonSavejpg();
	afx_msg void OnBnClickedButtonSwap();

private:
	void SetParameters();

	void GetRegistry(CWinApp *pApp);
	void WriteRegistry(CWinApp *pApp);

private:
	int m_resizeFlag;
	int m_changeRGB;	//0:No Change
						//1:R<-->G
						//2:G<-->B
						//3:B<-->R
						//4:GrayScale
						//5:Binarize

	int m_threshold;
	int m_filter;		//1:Average
						//2:Gaussian
						//3:Sharp
						//4:laplace

public:
	afx_msg void OnBnClickedButtonFilter();
	int m_sharp;
	int m_mosaicblocksize;
};
