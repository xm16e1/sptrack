// ADIBSection.cpp: CADIBSection NXﾌイve[V
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "ADIBSection.h"
//#include "System.h"

#include <math.h>
#define		SWAP(a,b)	{w=a;a=b;b=w;}
#define		PI	3.14159265358979


//#ifdef _DEBUG
//#undef THIS_FILE
//static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
//#endif

//////////////////////////////////////////////////////////////////////
// \z/V・ｽ
//////////////////////////////////////////////////////////////////////

CADIBSection::CADIBSection()	//空の画像セクションを作成する
{
	m_sizex=0;
	m_sizey=0;
	m_array=0;
}

CADIBSection::~CADIBSection()
{
	if(m_array!=0) {
		delete[] m_array;
	}
}

CADIBSection::CADIBSection(const int sizex, const int sizey)	//Createと同じ、
{
	m_sizex=sizex;
	m_sizey=sizey;
	m_array=new unsigned char[m_sizex*m_sizey*3];
//	ZeroMemory(m_array, sizeof(unsigned char)*m_sizex*m_sizey*3);
}

CADIBSection::CADIBSection(const CADIBSection *pAdib)	//DIBSection型を受け取り、新たにDIBSectionを複製する。
{
	m_sizex=pAdib->GetSizex();
	m_sizey=pAdib->GetSizey();
	m_array=new unsigned char[m_sizex*m_sizey*3];
	memcpy(m_array, pAdib->m_array, m_sizex*m_sizey*3);
}

void CADIBSection::Create(const int sizex, const int sizey)	//(sizex,sizey)で指定した大きさの画像セクションを作成する
{
	m_sizex=sizex;
	m_sizey=sizey;
	m_array=new unsigned char[m_sizex*m_sizey*3];
}

void CADIBSection::ChangeSize(const int sizex, const int sizey)		//クラスの保持する画像サイズを指定値に変更する、画像内容は失われる
{
	if(m_array!=0) {
		delete[] m_array;
	}
	m_sizex=sizex;
	m_sizey=sizey;
	m_array=new unsigned char[m_sizex*m_sizey*3];

}


void CADIBSection::CopyCADIB(const CADIBSection *pAdib)	//画像をコピーする
{
//	ASSERT(m_sizex==pAdib->GetSizex() && m_sizey==pAdib->GetSizey());
	memcpy(m_array, pAdib->m_array, m_sizex*m_sizey*3);
}

/*
void CADIBSection::ResizeCopyCADIB(const int sizex, const int sizey, const CADIBSection *pAdib)
{
//	if(sizex==pAdib->GetSizex() && sizey==pAdib->GetSizey()) {
//		CopyCADIB(pAdib);
//	} else {
		m_sizex=sizex;
		m_sizey=sizey;
		if(m_array!=0) {
			delete[] m_array;
		}
		m_array=new unsigned char[m_sizex*m_sizey*3];

		if(sizex==pAdib->GetSizex() && sizey==pAdib->GetSizey()) {
			CopyCADIB(pAdib);
		}else {

			for(int y=0; y<m_sizey; y++) {
				int ySrc= y*pAdib->GetSizey()/m_sizey;
				for(int x = 0; x<m_sizex; x++) {
					int xSrc = x*pAdib->GetSizex()/m_sizex;
					m_array[(y*m_sizex+x)*3+0]=pAdib->m_array[(ySrc*pAdib->GetSizex()+xSrc)*3+0];
					m_array[(y*m_sizex+x)*3+1]=pAdib->m_array[(ySrc*pAdib->GetSizex()+xSrc)*3+1];
					m_array[(y*m_sizex+x)*3+2]=pAdib->m_array[(ySrc*pAdib->GetSizex()+xSrc)*3+2];
				}
			}
		}
//	}
}
*/

void CADIBSection::ResizeCopyCADIB(const int sizex, const int sizey, const CADIBSection *pAdib, const int method)	//リサイズしてコピーする。methodは以下を参照
{
//pAdib(inSizex, inSizey)を (sizex,sizey) にresizeしてcopy
//method  0:nearest neighbor, 1:bi-linear, 2:bi-cubic, default:簡易手法（良くない）

	int inSizex=pAdib->GetSizex();
	int inSizey=pAdib->GetSizey();
	double dSizex=(double)inSizex;
	double dSizey=(double)inSizey;
	double ratiox=dSizex/sizex;
	double ratioy=dSizey/sizey;

	int x;
	int y;
	int x1, y1;
	int x2, y2;
	double dx1, dy1;
	double dx2, dy2;

	int px[4];
	int py[4];
	double wx[4];
	double wy[4];


	if(inSizex==m_sizex && inSizey==m_sizey && m_sizex==sizex && m_sizey==sizey && m_array!=0) {
		CopyCADIB(pAdib);
	} else {
		if(m_sizex!=sizex || m_sizey!=sizey || m_array==0) {
			m_sizex=sizex;
			m_sizey=sizey;
			if(m_array!=0) {
				delete[] m_array;
			}
			m_array=new unsigned char[m_sizex*m_sizey*3];
		}

		if(sizex==pAdib->GetSizex() && sizey==pAdib->GetSizey()) {
			CopyCADIB(pAdib);
		}else {
			switch(method) {
				
			case(0):	//nearest neighbor
				for(y=0; y<m_sizey; y++) {
					y1=(int)(y*ratioy);				//←オリジナル　■■■■■■■■■■大作改造■■■■■■■■■■■■■■■
						/*
					if(y==0){
						y1=0;
					}else{
						y1=(int)((y-1)*ratioy+0.5);
					}
					*/
					ASSERT(y1>=0);
					ASSERT(y1<inSizey);	
					for(x = 0; x<m_sizex; x++) {
						x1=(int)(x*ratiox);				//←オリジナル　■■■■■■■■■■大作改造■■■■■■■■■■■■■■■
							/*
						if(x==0){
							x1=0;
						}else{
							x1=(int)((x-1)*ratiox+0.5);
						}
						*/

						ASSERT(x1>=0);
						ASSERT(x1<inSizex);

						for(int p=0; p<3; p++) {
							int val=pAdib->m_array[(y1*inSizex+x1)*3+p];
							ASSERT(val>=0);
							ASSERT(val<=255);
							m_array[(y*m_sizex+x)*3+p]=val;
						}
					}
				}
				break;

			case(1):	//bi-linear
				//    x1    x       x2=x1+1
				// ---+-----*-------+-----
				//       dx1    dx2=1-dx1
				//
				//  f(x)=(f(x1)*dx2+f(x2)*dx1)/dx1+dx2)=f(x1)*dx2+f(x2)*dx1
				//  f(x,y)=(f(x1,y1)*dx2+f(x2,y1)*dx1)*dy2 + (f(x1,y2)*dx2+f(x2,y2)*dx1)*dy1
				//
				for(y=0; y<m_sizey; y++) {
					y1=(int)(y*ratioy);
					dy1=y*ratioy-y1;
					if(dy1>0 && y1<inSizey-1) {
						y2=y1+1;
					}else{
						y2=y1;
					}
					dy2=1-dy1;

					ASSERT(y1>=0);
					ASSERT(y1<inSizey);	
					ASSERT(y2>=0);
					ASSERT(y2<inSizey);	

					for(x = 0; x<m_sizex; x++) {
						x1=(int)(x*ratiox);
						dx1=x*ratiox-x1;
						if(dx1>0 && x1<inSizex-1) {
							x2=x1+1;
						}else{
							x2=x1;
						}			
						dx2=1-dx1;

						ASSERT(x1>=0);
						ASSERT(x1<inSizex);	
						ASSERT(x2>=0);
						ASSERT(x2<inSizex);	

						for(int p=0; p<3; p++) {
							int val	=(int) ( ((double)pAdib->m_array[(y1*inSizex+x1)*3+p] *dx2
										 +(double)pAdib->m_array[(y1*inSizex+x2)*3+p] *dx1)*dy2
									    +((double)pAdib->m_array[(y2*inSizex+x1)*3+p] *dx2
										 +(double)pAdib->m_array[(y2*inSizex+x2)*3+p] *dx1)*dy1 );
										 
							ASSERT(val>=0);
							ASSERT(val<=255);
							m_array[(y*m_sizex+x)*3+p]=val;
						}
					}
				}
				break;

			case(2):	//bi-cubic
				for(y=0; y<m_sizey; y++) {
					y1=(int)(y*ratioy);
					dy1=y*ratioy-y1;
					if(dy1>0 && y1<inSizey-1) {
						y2=y1+1;
					}else{
						y2=y1;
					}
					dy2=1-dy1;

					ASSERT(y1>=0);
					ASSERT(y1<inSizey);	
					ASSERT(y2>=0);
					ASSERT(y2<inSizey);	

					for(x = 0; x<m_sizex; x++) {
						x1=(int)(x*ratiox);
						dx1=x*ratiox-x1;
						if(dx1>0 && x1<inSizex-1) {
							x2=x1+1;
						}else{
							x2=x1;
						}			
						dx2=1-dx1;

						ASSERT(x1>=0);
						ASSERT(x1<inSizex);	
						ASSERT(x2>=0);
						ASSERT(x2<inSizex);	

					//以上はbi-linearと同じ
						if(y1<1 || y2>inSizey-2 || x1<1 || x2>inSizex-2){
							//縁，bi-linearにする
							for(int p=0; p<3; p++) {
								int val=(int) ( ((double)pAdib->m_array[(y1*inSizex+x1)*3+p] *dx2
												+(double)pAdib->m_array[(y1*inSizex+x2)*3+p] *dx1)*dy2
											   +((double)pAdib->m_array[(y2*inSizex+x1)*3+p] *dx2
												+(double)pAdib->m_array[(y2*inSizex+x2)*3+p] *dx1)*dy1 );
								ASSERT(val>=0);
								ASSERT(val<=255);
								m_array[(y*m_sizex+x)*3+p]=val;
							}
						}else {
							//bi-cubic
							px[0]=x1-1;
							px[1]=x1;
							px[2]=x2;
							px[3]=x2+1;
							py[0]=y1-1;
							py[1]=y1;
							py[2]=y2;
							py[3]=y2+1;

							//|t|<=1
							//|t|^3-2|t|^2+1 = |t|^2(|t|-2)+1		a=-1の場合の式
							wx[1]=dx1*dx1*(dx1-2)+1;
							wy[1]=dy1*dy1*(dy1-2)+1;
							wx[2]=dx2*dx2*(dx2-2)+1;
							wy[2]=dy2*dy2*(dy2-2)+1;
							//1<|t|<=2
							//-|t|^3+5|t|^2-8|t|+4 = |t|(|t|(-|t|+5)-8)+4
							double dxx=dx1+1;
							wx[0]=dxx*(dxx*(-dxx+5)-8)+4;
							dxx=dx2+1;
							wx[3]=dxx*(dxx*(-dxx+5)-8)+4;
							double dyy=dy1+1;
							wy[0]=dyy*(dyy*(-dyy+5)-8)+4;
							dyy=dy2+1;
							wy[3]=dyy*(dyy*(-dyy+5)-8)+4;
							

							for(int p=0; p<3; p++) {
								
								int val	=(int)( ((double)pAdib->m_array[(py[0]*inSizex+px[0])*3+p] *wx[0]
												+(double)pAdib->m_array[(py[0]*inSizex+px[1])*3+p] *wx[1]
												+(double)pAdib->m_array[(py[0]*inSizex+px[2])*3+p] *wx[2]
												+(double)pAdib->m_array[(py[0]*inSizex+px[3])*3+p] *wx[3])*wy[0]

											   +((double)pAdib->m_array[(py[1]*inSizex+px[0])*3+p] *wx[0]
												+(double)pAdib->m_array[(py[1]*inSizex+px[1])*3+p] *wx[1]
												+(double)pAdib->m_array[(py[1]*inSizex+px[2])*3+p] *wx[2]
												+(double)pAdib->m_array[(py[1]*inSizex+px[3])*3+p] *wx[3])*wy[1]

											   +((double)pAdib->m_array[(py[2]*inSizex+px[0])*3+p] *wx[0]
												+(double)pAdib->m_array[(py[2]*inSizex+px[1])*3+p] *wx[1]
												+(double)pAdib->m_array[(py[2]*inSizex+px[2])*3+p] *wx[2]
												+(double)pAdib->m_array[(py[2]*inSizex+px[3])*3+p] *wx[3])*wy[2]

											   +((double)pAdib->m_array[(py[3]*inSizex+px[0])*3+p] *wx[0]
												+(double)pAdib->m_array[(py[3]*inSizex+px[1])*3+p] *wx[1]
												+(double)pAdib->m_array[(py[3]*inSizex+px[2])*3+p] *wx[2]
												+(double)pAdib->m_array[(py[3]*inSizex+px[3])*3+p] *wx[3])*wy[3] );
						
								//範囲overがかなり発生する？ overshootのため．a=-1でなくa=-0.2くらいにすると改善されるらしい．
								if(val<0) {
									val=0;					
								}
								if(val>255) {
									val=255;
								}

								ASSERT(val>=0);
								ASSERT(val<=255);
								m_array[(y*m_sizex+x)*3+p]= val;
							}

							
						}


					}
				}	

				break;



			default:	//旧簡易手法　良くない
				for(y=0; y<m_sizey; y++) {
					y1= y*inSizey/m_sizey;
					for(x = 0; x<m_sizex; x++) {
						x1 = x*inSizex/m_sizex;
						m_array[(y*m_sizex+x)*3+0]=pAdib->m_array[(y1*inSizex+x1)*3+0];
						m_array[(y*m_sizex+x)*3+1]=pAdib->m_array[(y1*inSizex+x1)*3+1];
						m_array[(y*m_sizex+x)*3+2]=pAdib->m_array[(y1*inSizex+x1)*3+2];
					}
				}
				break;
			}
		}
	}
}


void CADIBSection::ExtractCopyCADIB(const int x1, const int y1, const int x2, const int y2, const CADIBSection *pAdib)
{
//[x1,y1][x2,y2]の領域を抽出してpAdibへ
//（*pAdibの指定領域を抽出してリサイズ、クラス内に組み込む）
	m_sizex=x2-x1;
	m_sizey=y2-y1;

	if(m_array!=0) {
		delete[] m_array;
	}
	m_array=new unsigned char[m_sizex*m_sizey*3];

	for(int y=0; y<m_sizey; y++) {
		int posy= (y+y1)*pAdib->GetSizex();
		for(int x = 0; x<m_sizex; x++) {
			int j=(y*m_sizex+x)*3;
			int k=(posy+x1+x)*3;
			m_array[j+0]=pAdib->m_array[k+0];
			m_array[j+1]=pAdib->m_array[k+1];
			m_array[j+2]=pAdib->m_array[k+2];
		}
	}
}

void CADIBSection::Trimming(const int x1, const int y1, const int x2, const int y2)
{
//[x1,y1][x2-1,y2-1]の領域を抽出して,現在の画像と置換える
//クラス内画像から指定領域を抽出する
	int sizex=x2-x1;
	int sizey=y2-y1;

	unsigned char *arrayTemp=new unsigned char[sizex*sizey*3];

	for(int y=0; y<sizey; y++) {
		for(int x=0; x<sizex; x++) {
			int j=(y*sizex+x)*3;
			int k=((y+y1)*m_sizex+(x+x1))*3;
			arrayTemp[j+0]=m_array[k+0];
			arrayTemp[j+1]=m_array[k+1];
			arrayTemp[j+2]=m_array[k+2];
		}
	}
	delete[] m_array;
	m_array=arrayTemp;
	m_sizex=sizex;
	m_sizey=sizey;
}



void CADIBSection::Clear()	//画像を黒色で埋める？。画像サイズは不変。
{
	memset(m_array, 0, sizeof(unsigned char)*m_sizex*m_sizey*3);
}

void CADIBSection::SetColor(const int col)	//画像に単色をつける。画像サイズは不変。
{
	memset(m_array, col, sizeof(unsigned char)*(m_sizex*m_sizey*3));
}

void CADIBSection::AbsDifference(const CADIBSection *pAdib1, const CADIBSection *pAdib2)
{
//２画像の輝度差分をとる、原２画像とクラス画像サイズは一致していなくてはいけない。

	if( (pAdib1->GetSizex() != pAdib2->GetSizex()) || (m_sizex != pAdib1->GetSizex()) ||
		(pAdib1->GetSizey() != pAdib2->GetSizey()) || (m_sizey != pAdib1->GetSizey()) ) {
		//size error
//		theSystem->MessageB("Error --- Different size!\n");
	}

	unsigned char *pArray1=pAdib1->GetArrayAddress();
	unsigned char *pArray2=pAdib2->GetArrayAddress();

	for(int i=0; i<m_sizex*m_sizey*3; i++) {
		if(pArray1[i]>pArray2[i]) {
			m_array[i]=pArray1[i]-pArray2[i];
		} else {
			m_array[i]=pArray2[i]-pArray1[i];

		}
	}

}

void CADIBSection::Superimpose(const CADIBSection *pAdib1, const CADIBSection *pAdib2, const double ratio)
{
//２画像を透過処理して重ねる。原２画像とクラス画像サイズは一致していなくてはいけない。比率をratioで指定する。
	if( (pAdib1->GetSizex() != pAdib2->GetSizex()) || (m_sizex != pAdib1->GetSizex()) ||
		(pAdib1->GetSizey() != pAdib2->GetSizey()) || (m_sizey != pAdib1->GetSizey()) ) {
		//size error
		AfxMessageBox("Error --- CADIBSection::Superimpose: Different size!\n");
	}

	unsigned char *pArray1=pAdib1->GetArrayAddress();
	unsigned char *pArray2=pAdib2->GetArrayAddress();

	for(int i=0; i<m_sizex*m_sizey*3; i++) {
		m_array[i]=pArray1[i]*(1-ratio)+pArray2[i]*ratio;

	}

}

void CADIBSection::Line(double x1, double y1, double x2, double y2,
		const double width, const unsigned int r, const unsigned int g, const unsigned int b)
{
	//widthは考慮していない．未完成（だそうな・・・）

	//clip
//	if(x1<0 || x1>m_sizex-1 || x2<0 || x2>m_sizex-1 ||
//	   y1<0 || y1>m_sizey-1 || y2<0 || y2>m_sizey-1) {
//		theSystem->Error("CADIBSection::Line --- range error!\n");
//	}


	if(y1<=3 && y2<=3 && r==255) {
		int check=0;
	}

	Clip(x1, m_sizex-1);
	Clip(x2, m_sizex-1);
	Clip(y1, m_sizey-1);
	Clip(y2, m_sizey-1);

	int x;
	int y;
	double yy;
	double xx;
	double w;

	if(fabs(x1-x2)>=fabs(y1-y2)) {
		//horizontal
		if(x1>x2) {
			SWAP(x1, x2);
			SWAP(y1, y2);
		}
		if(x1!=x2) {
			for(x=(int)(x1+0.5); x<=(int)(x2+0.5); x++) {
				//yy=y1+(y2-y1)* (x-x1)/(x2-x1)
				//           ={y1(x1-x)+y2(x-x1)}/(x2-x1)
				yy=(y1*(x2-x)+y2*(x-x1))/(x2-x1);
				y=(int)(yy+0.5);

				m_array[(y*m_sizex+x)*3+0]=r;
				m_array[(y*m_sizex+x)*3+1]=g;
				m_array[(y*m_sizex+x)*3+2]=b;
			}
		}else {
			//ここに来るのはx1==x2, y1==y2のときのみ．点を描画
			m_array[((int)y1*m_sizex+(int)x1)*3+0]=r;
			m_array[((int)y1*m_sizex+(int)x1)*3+1]=g;
			m_array[((int)y1*m_sizex+(int)x1)*3+2]=b;
		}
	} else {
		//vertical
		if(y1>y2) {
			SWAP(x1, x2);
			SWAP(y1, y2);
		}
		for(y=(int)(y1+0.5); y<=(int)(y2+0.5); y++) {
			//xx=x1+(x2-x1)* (y-y1)/(y2-y1)
			//           ={x1(y1-y)+x2(y-y1)}/(y2-y1)
			xx=(x1*(y2-y)+x2*(y-y1))/(y2-y1);
			x=(int)(xx+0.5);

			m_array[(y*m_sizex+x)*3+0]=r;
			m_array[(y*m_sizex+x)*3+1]=g;
			m_array[(y*m_sizex+x)*3+2]=b;
		}
	}

}

void CADIBSection::Rect(const int x, const int y, const int length, 
		const unsigned int r, const unsigned int g, const unsigned int b)
{
//Lineが未完成なので使わないほうがよい？
	Line((double)x-length, (double)y-length, (double)x+length, (double)y-length, 1, r,g,b); 
	Line((double)x+length, (double)y-length, (double)x+length, (double)y+length, 1, r,g,b); 
	Line((double)x+length, (double)y+length, (double)x-length, (double)y+length, 1, r,g,b); 
	Line((double)x-length, (double)y+length, (double)x-length, (double)y-length, 1, r,g,b); 
}


void CADIBSection::Clip(double &val, const double upperLimit)
{
	if(val<0) {
		val=0;
	} else if(val>upperLimit) {
		val=upperLimit;
	}

}
void CADIBSection::Text(const char *text, const double x0, const double y0, const double height,
		const unsigned int r, const unsigned int g, const unsigned int b)
{
//文字を書く　　未完成
//XDibSectionに変換して書いた方が良い

}


void CADIBSection::Grayscale()
{
//現在クラス内に保持している画像をグレイスケール変換する。
	int y;
	int x;
	int j3;
	double fIntensity;
	int iIntensity;

	for(y=0; y<m_sizey; y++) {
		for(x=0; x<m_sizex; x++) {
			j3=(y*m_sizex+x)*3;

			fIntensity=	(double)m_array[j3 +0]*0.299 +
						(double)m_array[j3 +1]*0.587 +
						(double)m_array[j3 +2]*0.114;
			iIntensity=(int)(fIntensity+0.5);

			if(iIntensity<0) {
				iIntensity=0;
			}else if(iIntensity>255) {
				iIntensity=255;
			}

			m_array[j3 +0]=(unsigned char)iIntensity;
			m_array[j3 +1]=(unsigned char)iIntensity;
			m_array[j3 +2]=(unsigned char)iIntensity;
		}
	}

}

void CADIBSection::FlipImage()
{
//画像の180'回転
	unsigned char *arrayTemp=new unsigned char[m_sizex*m_sizey*3];
	int y;
	int x;
	int j3;
	int ji;
	for(y=0; y<m_sizey; y++) {
		for(x=0; x<m_sizex; x++) {
			j3=(y*m_sizex+x)*3;
			ji=((m_sizey-y-1)*m_sizex+(m_sizex-x-1))*3;
			arrayTemp[j3+0]=m_array[ji+0];
			arrayTemp[j3+1]=m_array[ji+1];
			arrayTemp[j3+2]=m_array[ji+2];
		}
	}
	unsigned char *arrayDelete=m_array;
	m_array=arrayTemp;
	delete[] arrayDelete;
}

void CADIBSection::Average4Pixels()
{
//4pixel平滑化？画像サイズが変更されていくので複数回重ねるとエラーを吐くことがある
	if(m_sizex%2!=0 || m_sizey%2!=0) {
		//size error
		AfxMessageBox("Error --- CADIBSection::Average4Pixels: size!=2n\n");
	}

	m_sizex/=2;
	m_sizey/=2;
	unsigned char *arrayTemp=new unsigned char[m_sizex*m_sizey*3];

	int y;
	int x;
	int j3;
	int jo0;
	int jo1;
	int jo2;
	int jo3;

	for(y=0; y<m_sizey; y++) {
		for(x=0; x<m_sizex; x++) {
			j3=(y*m_sizex+x)*3;
			jo0=((y*2  )*(m_sizex*2)+x*2  )*3;
			jo1=((y*2  )*(m_sizex*2)+x*2+1)*3;
			jo2=((y*2+1)*(m_sizex*2)+x*2  )*3;
			jo3=((y*2+1)*(m_sizex*2)+x*2+1)*3;
			arrayTemp[j3+0]=(m_array[jo0+0]+m_array[jo1+0]+m_array[jo2+0]+m_array[jo3+0] +2)/4;
			arrayTemp[j3+1]=(m_array[jo0+1]+m_array[jo1+1]+m_array[jo2+1]+m_array[jo3+1] +2)/4;
			arrayTemp[j3+2]=(m_array[jo0+2]+m_array[jo1+2]+m_array[jo2+2]+m_array[jo3+2] +2)/4;
		}
	}
	unsigned char *arrayDelete=m_array;
	m_array=arrayTemp;
	delete[] arrayDelete;

}


void CADIBSection::GetValueHSV(const int x, const int y, double &H, double &S, double &V) const
{
//輝度情報をHSVに変換した値を返す
	int pos=(y*m_sizex+x)*3;
	int R=m_array[pos+0];
	int G=m_array[pos+1];
	int B=m_array[pos+2];

	double r,g,b;
	double Pi=3.1415926535;

	//int maxCol=max(r, max(g, b));
	int Imax=R;
	if(G>Imax) Imax=G;
	if(B>Imax) Imax=B;
	//int minCol=min(r, min(g, b));
	int Imin=R;
	if(G<Imin) Imin=G;
	if(B<Imin) Imin=B;

	V=(double)Imax/255;

	if(Imax==Imin){			//Imax==0の判定は不要
		S=0.0;
		H=99999;	//Hは不安、値はどうする？
	} else {
		S=(double)(Imax-Imin)/Imax;

		r=(double)(Imax-R)/(Imax-Imin);
		g=(double)(Imax-G)/(Imax-Imin);
		b=(double)(Imax-B)/(Imax-Imin);
		if(Imax==R){
			H=((b-g)*Pi)/3;
		}else{
			if(Imax==G){
				H=(2+r-b)*Pi/3;
			}else{
				H=(4+g-r)*Pi/3;
			}
		}
	}
	
	if(H<0) {
		H=(H+2*Pi);
	}
}



void CADIBSection::GetVHDEdge(const int x, const int y, int &edgeV, int &edgeH, int &edgeD) const
{
// Harr-like edge extraction
//  2x2 pixelのareaのVertical-edge, Horizontal-edge, Diagonal-edge
//   I00  I01
//   I10  I11
// V-edge=I00+I10-I10-I11
// H-edge=I00-I10+I10-I11
// V-edge=I00-I10-I10+I11
//1 pixelずつshiftしてcallする
//sizeをoverする場合は同じ値が続いているとする
//結果は　-510<=edge<=510 で返す

	int pos00=((y  )*m_sizex+x  )*3;
	int pos01=((y  )*m_sizex+x+1)*3;
	int pos10=((y+1)*m_sizex+x  )*3;
	int pos11=((y+1)*m_sizex+x+1)*3;

	double I00, I01, I10, I11;

	I00=(double)m_array[pos00+0]*0.299 +(double)m_array[pos00+1]*0.587 +(double)m_array[pos00+2]*0.114;

	if(x<m_sizex-1){
		I01=(double)m_array[pos01+0]*0.299 +(double)m_array[pos01+1]*0.587 +(double)m_array[pos01+2]*0.114;
	} else {
		I01=I00;
	}

	if(y<m_sizey-1) {
		I10=(double)m_array[pos10+0]*0.299 +(double)m_array[pos10+1]*0.587 +(double)m_array[pos10+2]*0.114;
	} else {
		I10=I00;
	}

	if(x<m_sizex-1 && y<m_sizey-1) {
		I11=(double)m_array[pos11+0]*0.299 +(double)m_array[pos11+1]*0.587 +(double)m_array[pos11+2]*0.114;
	} else {
		I11=I10;
	}

	edgeV=I00+I10-I10-I11;
	if(edgeV<-510) {
		edgeV=-510;
	} else if(edgeV>510) {
		edgeV=510;
	}

	edgeH=I00-I10+I10-I11;
	if(edgeH<-510) {
		edgeH=-510;
	} else if(edgeH>510) {
		edgeH=510;
	}

	edgeD=I00-I10-I10+I11;
	if(edgeD<-510) {
		edgeD=-510;
	} else if(edgeD>510) {
		edgeD=510;
	}
}


void CADIBSection::Gradient(const int x, const int y, double *val, double *angle) 
{
	//GraySclale画像のHOG生成に使用
	//R=G=BなのでRを用いる
	//atan()の変域は[-PI/2:PI/2]  0-180に直す．(atan()+PI/2)*180/PI=atan()*180/PI+90
	double dx=m_array[(y*m_sizex+(x+1))*3 +0]-m_array[(y*m_sizex+(x-1))*3 +0];
	double dy=m_array[((y+1)*m_sizex+x)*3 +0]-m_array[((y-1)*m_sizex+x)*3 +0];
	*val=sqrt(dx*dx+dy*dy);
	*angle=atan(dy/(dx+0.001))*180/PI+90;		

}



//2DGraphの描画
//Vrml3DGarphと同じinterface
//flag 0:普通に描画，1:sqrt()をとる
void CADIBSection::Draw2DGraph(const double *buff, const int sizex, const int sizey, 
			const int nLevel, const double *aValue, const COLORREF *aColor,
			const double scale, const int flag)
{
	unsigned char r,g,b;
	double val;

	for(int y=0; y<m_sizey; y++) {
		int ySrc= y*sizey/m_sizey;
		for(int x = 0; x<m_sizex; x++) {
			int xSrc = x*sizex/m_sizex;

			val=buff[ySrc*sizex+xSrc];

			//sqrt if flag==1
			if(flag==1) {
				val=sqrt(val);
			} 
			//scale
			if(scale!=1.0) {
				val*=scale;
			}

			Draw2DGraphColor(val, nLevel, aValue, aColor, r,g,b);
			m_array[(y*m_sizex+x)*3+0]=r;
			m_array[(y*m_sizex+x)*3+1]=g;
			m_array[(y*m_sizex+x)*3+2]=b;
		}
	}

}

void CADIBSection::Draw2DGraphColor(const double value, 
			const int nLevel, const double *aValue, const COLORREF *aColor,
			unsigned char &r, unsigned char &g, unsigned char &b)
{
	if(value <= aValue[0]) {
		r =GetRValue(aColor[0]);
		g =GetGValue(aColor[0]);
		b =GetBValue(aColor[0]);
		return;
	} 
	for(int i=1; i<nLevel-1; i++) {
		if(value <= aValue[i]) {
			r =GetRValue(aColor[i]);
			g =GetGValue(aColor[i]);
			b =GetBValue(aColor[i]);
			return;
		}
	}
	r =GetRValue(aColor[nLevel-1]);
	g =GetGValue(aColor[nLevel-1]);
	b =GetBValue(aColor[nLevel-1]);
}

