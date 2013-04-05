// ImageConvDialogDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "ImageConvDialog.h"
#include "ImageConvDialogDlg.h"
#include "Filter.h"
#include "RegKeyName.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// アプリケーションのバージョン情報に使われる CAboutDlg ダイアログ

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ダイアログ データ
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

// 実装
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CImageConvDialogDlg ダイアログ




CImageConvDialogDlg::CImageConvDialogDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CImageConvDialogDlg::IDD, pParent)
	, m_CstrInputFile(_T(""))
	, m_sharp(0)
	, m_mosaicblocksize(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	CWinApp *pApp = AfxGetApp();	//load from registory
	GetRegistry(pApp);
}

void CImageConvDialogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PICT1, m_pict1);
	DDX_Control(pDX, IDC_PICT2, m_pict2);
	//	DDX_Text(pDX, IDC_EDIT_INPUTFILE, m_inputFileName);
	DDX_Text(pDX, IDC_EDIT_INPUTFILE, m_CstrInputFile);
	//	DDX_Text(pDX, IDC_EDIT1, m_threshold);
	DDX_Text(pDX, IDC_EDIT_SHARP, m_sharp);
	DDX_Text(pDX, IDC_EDIT2_MOSAIC, m_mosaicblocksize);
}

BEGIN_MESSAGE_MAP(CImageConvDialogDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_INPUTFILE, &CImageConvDialogDlg::OnBnClickedButtonInputfile)
	ON_BN_CLICKED(IDC_BUTTON_START, &CImageConvDialogDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_CHANGECOLOR, &CImageConvDialogDlg::OnBnClickedButtonChangecolor)
	ON_BN_CLICKED(IDC_BUTTON_HISTOGRAM, &CImageConvDialogDlg::OnBnClickedButtonHistogram)
	ON_BN_CLICKED(IDC_BUTTON_SAVEJPG, &CImageConvDialogDlg::OnBnClickedButtonSavejpg)
	ON_BN_CLICKED(IDC_BUTTON_SWAP, &CImageConvDialogDlg::OnBnClickedButtonSwap)

	ON_BN_CLICKED(IDC_BUTTON_FILTER, &CImageConvDialogDlg::OnBnClickedButtonFilter)
END_MESSAGE_MAP()


// CImageConvDialogDlg メッセージ ハンドラ

BOOL CImageConvDialogDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// "バージョン情報..." メニューをシステム メニューに追加します。

	// IDM_ABOUTBOX は、システム コマンドの範囲内になければなりません。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。

	m_pAdib1=new CADIBSection;
	m_pAdib2=new CADIBSection;
	m_dispFlag1=0;
	m_dispFlag2=0;


//	m_resizeFlag=1;
//	m_changeRGB=0;

	//registry

	CButton *check1=(CButton*) GetDlgItem(IDC_CHECK_RESIZE);
	check1->SetCheck(m_resizeFlag);


	CButton *radio1=(CButton*) GetDlgItem(IDC_RADIO_NOCHANGE);
	CButton *radio2=(CButton*) GetDlgItem(IDC_RADIO_CHANGERG);
	CButton *radio3=(CButton*) GetDlgItem(IDC_RADIO_CHANGEGB);
	CButton *radio4=(CButton*) GetDlgItem(IDC_RADIO_CHANGEBR);
	CButton *radio5=(CButton*) GetDlgItem(IDC_RADIO_GRAYSCALE);
	CButton *radio6=(CButton*) GetDlgItem(IDC_RADIO_BINARY);

	
	CButton *radio11=(CButton*) GetDlgItem(IDC_RADIO_AVERAGE);
	CButton *radio12=(CButton*) GetDlgItem(IDC_RADIO_GAUSS);
	CButton *radio13=(CButton*) GetDlgItem(IDC_RADIO_SHARP);
	CButton *radio14=(CButton*) GetDlgItem(IDC_RADIO_LAPLACE);
	CButton *radio15=(CButton*) GetDlgItem(IDC_RADIO_MOSAIC);

	

	switch(m_changeRGB){
	case(0): 
		radio1->SetCheck(1);
		break;
	case(1): 
		radio2->SetCheck(1);
		break;
	case(2): 
		radio3->SetCheck(1);
		break;
	case(3): 
		radio4->SetCheck(1);
		break;
	case(4):
		radio5->SetCheck(1);
		break;
	case(5):
		radio6->SetCheck(1);
		break;
	default:
		radio1->SetCheck(1);
		break;
	}

	switch(m_filter){
	case(1):
		radio11->SetCheck(1);
		break;
	case(2):
		radio12->SetCheck(1);
		break;
	case(3):
		radio13->SetCheck(1);
		break;
	case(4):
		radio14->SetCheck(1);
		break;
	case(5):
		radio15->SetCheck(1);
		break;
	default:
		radio11->SetCheck(1);
		break;
	}

	UpdateData(FALSE);

	SetDlgItemText(IDC_EDIT_INPUTFILE, m_inputFileName);
	SetDlgItemInt(IDC_EDIT_THRESHOLD, m_threshold);
	SetDlgItemInt(IDC_EDIT_SHARP,m_sharp);
	SetDlgItemInt(IDC_EDIT2_MOSAIC,m_mosaicblocksize);
	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

void CImageConvDialogDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CImageConvDialogDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();

		//picture boxにADIBを描画する

		if(m_dispFlag1==1)
			DrawPict(m_pict1,m_pAdib1);
		if(m_dispFlag2==1)
			DrawPict(m_pict2,m_pAdib2);

	}
}

//m_dibのbitmapをm_pictの領域に描画
void CImageConvDialogDlg::DrawPict(CStatic &m_pict, const CADIBSection *m_pAdib)
{
	CDC *pDC=m_pict.GetDC();			//dispya用device context
//	CDC *pDC=this->GetDC();				//フォームに直接描く場合
	//これに直接描画するのでなく別のDC(myDC)上にデータを作る
	//BitBlt()で描画領域にデータを移す

	CDC myDC;							
	myDC.CreateCompatibleDC(pDC);


	CSize sizeOriginal(m_pAdib->GetSizex(), m_pAdib->GetSizey());
	CRect wRect;
	m_pict.GetClientRect(wRect);
	int xSize=wRect.right-wRect.left;		
	int ySize=wRect.bottom-wRect.top;



	double ratio;

	//check box

	if(m_resizeFlag!=0){
		ratio=min((double)xSize/sizeOriginal.cx, (double)ySize/sizeOriginal.cy);
	} else {
		ratio=1;
	}

	CSize sizeCheck((int)sizeOriginal.cx*ratio, (int)sizeOriginal.cy*ratio);

	CADIBSection *pAdibTemp=new CADIBSection(sizeCheck.cx, sizeCheck.cy);

	if(m_resizeFlag!=0){
		pAdibTemp->ResizeCopyCADIB(sizeCheck.cx, sizeCheck.cy, m_pAdib);
	} else {
		pAdibTemp->CopyCADIB(m_pAdib);
	}
	//end

	CXDIBSection DibWork;
	DibWork.Create(pDC, sizeCheck);

	DibWork.CopyFromCADIBSection(pAdibTemp);

	HBITMAP hbmpOld = (HBITMAP)::SelectObject(myDC.m_hDC, DibWork.GetHBitmap());
	pDC->BitBlt(0,0,xSize,ySize,&myDC,0,0,SRCCOPY);

	myDC.SelectObject(hbmpOld);			//省略可能？
	m_pict.ReleaseDC(pDC);				//m_pict.GetDC()のrelease

	delete pAdibTemp;
}




// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR CImageConvDialogDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CImageConvDialogDlg::OnBnClickedButtonInputfile()	//ファイル名取得ボタンの動作
{
	CFileDialog dlg(TRUE, ".jpg", NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"all(*.*)|*.*|jpg(*.jpg)|*.jpg||");

	if (dlg.DoModal() == IDOK) 
		SetDlgItemText(IDC_EDIT_INPUTFILE, dlg.GetPathName());
	
	m_inputFileName=dlg.GetPathName();
}

void CImageConvDialogDlg::OnBnClickedButtonStart()	//Startボタンの動作
{
	SetParameters();

	CWindowDC dc(AfxGetMainWnd());
	CXDIBSection DibWork;

	if(!DibWork.CreateFromJPEGFile(&dc, m_inputFileName)) 
		AfxMessageBox("Jpeg file open error!\n");
	
	DibWork.CopyToCADIBSection(m_pAdib1);

	m_dispFlag1=1;

	Invalidate();
	
}

void CImageConvDialogDlg::OnBnClickedButtonChangecolor()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	SetParameters();

	int sizex=m_pAdib1->GetSizex();
	int sizey=m_pAdib1->GetSizey();
	m_pAdib2->ChangeSize(sizex,sizey);

	unsigned char r1,g1,b1;
	unsigned char r2,g2,b2;
	int x;
	int y;
	int intensity;


	unsigned char LUT[256]={0};		//look up table
	for(int i=m_threshold;i<256;i++)
		LUT[i]=255;
	

	//checkbox
	if(m_changeRGB==4){
		m_pAdib2->CopyCADIB(m_pAdib1);
		m_pAdib2->Grayscale();		//Grayscale

	
	}else{

		for(y=0;y<sizey;y++){
			for(x=0;x<sizex;x++){
				r1=m_pAdib1->GetValueR(x,y);
				g1=m_pAdib1->GetValueG(x,y);
				b1=m_pAdib1->GetValueB(x,y);


				//check box
				switch(m_changeRGB){
				case(0):
					r2=r1;
					g2=g1;
					b2=b1;
					break;
				case(1):
					r2=g1;
					g2=r1;
					b2=b1;
					break;
				case(2):
					r2=r1;
					g2=b1;
					b2=g1;
					break;
				case(3):
					r2=b1;
					g2=g1;
					b2=r1;
					break;
				case(5):	//binarize


					intensity=0.299*r1 + 0.587*g1 + 0.114*b1;

					r2=LUT[intensity];
					g2=LUT[intensity];
					b2=LUT[intensity];

					break;
				default:
					r2=0;
					g2=0;
					b2=0;
					break;	
				}

				if(r2<0){	//red check
					r2=0;
				} else if(r2>255) {
					r2=255;
				}
				if(g2<0){	//green check
					g2=0;
				}else if(g2>255){
					g2=255;
				}

				if(b2<0){ //blue check
					b2=0;
				}else if(b2>255){
					b2=255;
				}

				m_pAdib2->SetPixel(x,y,r2,g2,b2);

			
			}
		}
	}

	m_dispFlag2=1;
	Invalidate(TRUE);

}


void CImageConvDialogDlg::OnBnClickedButtonSavejpg()
{
	SetParameters();

	CString outputFileName;
	CFileDialog dlg(FALSE,".jpg",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"jpg(*.jpg)|*.jpg|all(*.*)|*.*|");

	if(dlg.DoModal()==IDOK)
		outputFileName=dlg.GetPathName();

	int jpegQuality=90;

	CWindowDC dc(AfxGetMainWnd());
	int sizex=m_pAdib2->GetSizex();
	int sizey=m_pAdib2->GetSizey();

	CXDIBSection DibWork;
	DibWork.Create(&dc,CSize(sizex,sizey));
	DibWork.CopyFromCADIBSection(m_pAdib2);

	DibWork.SaveToJPEGFile(outputFileName,jpegQuality);
}

void CImageConvDialogDlg::OnBnClickedButtonHistogram()
{
	SetParameters();


	int histogram[256];			//intensityは[0:255]に限定されるので固定値で良い．
	ZeroMemory(histogram, sizeof(int)*256);		//zero clear

	unsigned char r, g, b;
	int intensity;

	int x;
	int y;
	int sizex=m_pAdib1->GetSizex();
	int sizey=m_pAdib1->GetSizey();

	for(y=0; y<sizey; y++) {
		for(x=0; x<sizex; x++) {
			r=m_pAdib1->GetValueR(x,y);
			g=m_pAdib1->GetValueG(x,y);
			b=m_pAdib1->GetValueB(x,y);

			intensity=0.299*r + 0.587*g + 0.114*b;

			if(intensity<0) {				//[0:255]を必ずcheck
				intensity=0;				//外れていたら[0:255]にする
			} else if(intensity>255) {
				intensity=255;
			}

			histogram[intensity]++;
		}
	}

	//check max value		最大値が full scale になるようにする
	int max=0;
	int i;
	int j;

	for(i=0; i<=255; i++) {
		if(histogram[i]>max) {
			max=histogram[i];
		}
	}

	//m_pADib2の大きさを256x256にする
	m_pAdib2->ChangeSize(256, 256);		

	//draw histogram
	for(i=0; i<256; i++) {
		int height=histogram[i]*255/max;	//最大値を256にする
		for(j=0; j<=height; j++) {
			m_pAdib2->SetPixel(i, 255-j, 0, 0, 255); 
		}									//y座標は下向きが+なので反転させる
	}
	
	m_dispFlag2=1;

	Invalidate();

	// TODO: ここにコントロール通知ハンドラ コードを追加します。
}
void CImageConvDialogDlg::OnBnClickedButtonSwap()	//スワップボタン
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。

	SetParameters();

	m_pAdib1->CopyCADIB(m_pAdib2);	//memcpy使用、こちらのほうが効率がいい

/*
	unsigned char r2,g2,b2;
	int x,y;

	int sizex=m_pAdib2->GetSizex();
	int sizey=m_pAdib2->GetSizey();


	m_pAdib1->ChangeSize(sizex,sizey);

	for(y=0;y<sizey;y++){
		for(x=0;x<sizex;x++){

			r2=m_pAdib2->GetValueR(x,y);
			g2=m_pAdib2->GetValueG(x,y);
			b2=m_pAdib2->GetValueB(x,y);

			m_pAdib1->SetPixel(x,y,r2,g2,b2);

		}
	}
*/
	Invalidate();
}

void CImageConvDialogDlg::SetParameters()
{
	UpdateData(TRUE);

	CButton *radio1=(CButton*) GetDlgItem(IDC_RADIO_NOCHANGE);
	//radio button のon/offを読取る
	CButton *radio2=(CButton*) GetDlgItem(IDC_RADIO_CHANGERG);
	CButton *radio3=(CButton*) GetDlgItem(IDC_RADIO_CHANGEGB);
	CButton *radio4=(CButton*) GetDlgItem(IDC_RADIO_CHANGEBR);
	CButton *radio5=(CButton*) GetDlgItem(IDC_RADIO_GRAYSCALE);
	CButton *radio6=(CButton*) GetDlgItem(IDC_RADIO_BINARY);

	CButton *radio11=(CButton*) GetDlgItem(IDC_RADIO_AVERAGE);
	CButton *radio12=(CButton*) GetDlgItem(IDC_RADIO_GAUSS);
	CButton *radio13=(CButton*) GetDlgItem(IDC_RADIO_SHARP);
	CButton *radio14=(CButton*) GetDlgItem(IDC_RADIO_LAPLACE);
	CButton *radio15=(CButton*) GetDlgItem(IDC_RADIO_MOSAIC);

//	CEdit *edit2=(CEdit*) GetDlgItem(IDC_EDIT_THRESHOLD);

	m_threshold=GetDlgItemInt(IDC_EDIT_THRESHOLD);
	m_sharp=GetDlgItemInt(IDC_EDIT_SHARP);
	m_mosaicblocksize=GetDlgItemInt(IDC_EDIT2_MOSAIC);

	if(radio1->GetCheck()) m_changeRGB=0;					//m_changeRGBに値を設定
	if(radio2->GetCheck()) m_changeRGB=1;		
	if(radio3->GetCheck()) m_changeRGB=2;		
	if(radio4->GetCheck()) m_changeRGB=3;
	if(radio5->GetCheck()) m_changeRGB=4;
	if(radio6->GetCheck()) m_changeRGB=5;

	if(radio11->GetCheck()) m_filter=1;
	if(radio12->GetCheck()) m_filter=2;
	if(radio13->GetCheck()) m_filter=3;
	if(radio14->GetCheck()) m_filter=4;
	if(radio15->GetCheck()) m_filter=5;


	CButton *check1=(CButton*) GetDlgItem(IDC_CHECK_RESIZE);
		//check buttonのon/offを読取る
	check1->GetCheck() ? m_resizeFlag=1:m_resizeFlag=0;	
		//m_resizeFlagに値を設定

	CWinApp *pApp = AfxGetApp();
	WriteRegistry(pApp);					//save to registory

}

void CImageConvDialogDlg::GetRegistry(CWinApp *pApp)
{
	m_inputFileName=pApp->GetProfileString(c_pszRegKey, c_pszRegFileName, "");
	m_resizeFlag =pApp->GetProfileInt(c_pszRegKey, c_pszRegResige, 0);
	m_changeRGB =pApp->GetProfileInt(c_pszRegKey, c_pszRegImageConversion, 0);
	m_threshold=pApp->GetProfileInt(c_pszRegKey,c_pszRegThreshold,0);
	m_filter=pApp->GetProfileInt(c_pszRegKey,c_pszRegFilter,1);
	m_sharp=pApp->GetProfileInt(c_pszRegKey,c_pszRegSharp,0);
	m_mosaicblocksize=pApp->GetProfileInt(c_pszRegKey,c_pszRegMosaic,1);
}

void CImageConvDialogDlg::WriteRegistry(CWinApp *pApp)
{
	pApp->WriteProfileString(c_pszRegKey, c_pszRegFileName, m_inputFileName);
	pApp->WriteProfileInt(c_pszRegKey, c_pszRegResige, m_resizeFlag);
	pApp->WriteProfileInt(c_pszRegKey, c_pszRegImageConversion, m_changeRGB);
	pApp->WriteProfileInt(c_pszRegKey, c_pszRegThreshold,m_threshold);
	pApp->WriteProfileInt(c_pszRegKey, c_pszRegFilter, m_filter);
	pApp->WriteProfileInt(c_pszRegKey, c_pszRegSharp,m_sharp);
	pApp->WriteProfileInt(c_pszRegKey, c_pszRegMosaic,m_mosaicblocksize);
	Invalidate(FALSE);
}



void CImageConvDialogDlg::OnBnClickedButtonFilter()
{
	SetParameters();
	m_pAdib2->ChangeSize(m_pAdib1->GetSizex(),m_pAdib1->GetSizey());
	m_pAdib2->CopyCADIB(m_pAdib1);	

	CFilter filter(m_pAdib2);
	if(m_filter==3)
		filter.SetSharpPara(m_sharp);
	if(m_filter==5)
		filter.SetMosaicBlock(m_mosaicblocksize);

	filter.SetFilterType(m_filter);	//m_filterを引数に与えて分岐
	filter.DoFiltering(m_pAdib2);
	filter.WriteDIB(m_pAdib2);

	m_dispFlag2=1;
	
	Invalidate();
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
}
