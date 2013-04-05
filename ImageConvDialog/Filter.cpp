
#include "StdAfx.h"
#include "Filter.h"


#define kernelsize 9	//�J�[�l���T�C�Y���Œ肵�Ă���

CFilter::CFilter(void)
{
	SetFilterType(0);
	m_mosaicblock=1;		//���U�C�N�u���b�N������

}

CFilter::~CFilter(void)
{
	if(m_ptmp!=0)
		delete m_ptmp;

	if(del!=0)
		delete del;

	if(m_filtertype==5)
		delete[] mosaic;

}

CFilter::CFilter(CADIBSection *pAdib)
{
	m_ptmp=new CADIBSection(pAdib);	//�o�͗p�o�b�t�@
	del=new CADIBSection(pAdib->GetSizex(),pAdib->GetSizey());			//��Ɨp�o�b�t�@
	m_mosaicblock=1;	//���U�C�N�u���b�N������
	
	SetFilterType(0);	//�f�t�H���g�t�B���^

}



void CFilter::SetFilterType(int ftype)	//�t�B���^�^�C�v��ݒ肷��
{
	int i;
	switch(ftype){
	case 1:				//average
		for(i=0;i<kernelsize;i++)
//			m_kernel[i]=1.0/9.0;
			m_kernel[i]=1.0;
		m_divoption=9;
		m_filtertype=1;
		break;
	case 2:				//Gaussian
		m_kernel[0]=1.0;
		m_kernel[1]=2.0;
		m_kernel[2]=1.0;
		m_kernel[3]=2.0;
		m_kernel[4]=4.0;
		m_kernel[5]=2.0;
		m_kernel[6]=1.0;
		m_kernel[7]=2.0;
		m_kernel[8]=1.0;
		m_divoption=16;
		m_filtertype=2;
		break;

	case 3:				//sharpening
		for(i=0;i<kernelsize;i++)
			m_kernel[i]=-(double)m_sp/10.0;
		m_kernel[(kernelsize-1)/2]=(double)m_sp/10.0*8+1;
		m_divoption=1;
		m_filtertype=3;
		break;

	case 4:				//Laplacian
		m_kernel[0]=0;
		m_kernel[1]=-1;
		m_kernel[2]=0;
		m_kernel[3]=-1;
		m_kernel[4]=4;
		m_kernel[5]=-1;
		m_kernel[6]=0;
		m_kernel[7]=-1;
		m_kernel[8]=0;
		m_divoption=1;
		m_filtertype=4;
		break;
	case 5:				//���U�C�N
		mosaic=new double[m_mosaicblock];
		m_filtertype=5;


	default:
		for(i=0;i<kernelsize;i++)
			m_kernel[i]=0;
		m_kernel[4]=1;	//�������Ȃ��t�B���^
		m_divoption=1;
		break;
	}
}

void CFilter::SetSharpPara(int param)
{
	m_sp=param;
}

void CFilter::SetMosaicBlock(int block)
{
	m_mosaicblock=block;
}


void CFilter::DoFiltering(CADIBSection *pAdib)	//pAdib�ɑ΂��ăt�B���^�����O�����s����
{
	int sizex = m_ptmp->GetSizex();
	int sizey = m_ptmp->GetSizey();	//�c���T�C�Y���擾

	int x;
	int y;
	unsigned char r,g,b;


	int i;
	int j;

	if(m_mosaicblock==1){				//���U�C�N�u���b�N1x1(=�������Ȃ�)

		int sr,sg,sb;	//sum

		for(y=1;y<sizey-1;y++){
			for(x=1;x<sizex-1;x++){		//�ŊO����f�𖳎�����悤�ɃX�L����
				sr=0,sg=0,sb=0;

				for(i=0;i<3;i++){
					for(j=0;j<3;j++){				

						pAdib->GetValue(x+j-1,y+i-1,r,g,b);	//�F�擾
						sr+=m_kernel[j+i*3]*r;
						sg+=m_kernel[j+i*3]*g;
						sb+=m_kernel[j+i*3]*b;		//�������ݒl�̐ݒ�

					}
				}

				sr/=m_divoption;
				sg/=m_divoption;
				sb/=m_divoption;

				if(m_filtertype==4){	//���v���V�A���I�t�Z�b�g
					sr+=128;
					sg+=128;
					sb+=128;
				}


				if(sr<0){	//red check
					sr=0;
				} else if(sr>255) {
					sr=255;
				}
				if(sg<0){	//green check
					sg=0;
				}else if(sg>255){
					sg=255;
				}

				if(sb<0){ //blue check
					sb=0;
				}else if(sb>255){
					sb=255;
				}

				del->SetPixel(x,y,sr,sg,sb);	//�o�b�t�@�ɏ�������

			}
		}
		
		m_ptmp->CopyCADIB(del);	//del��m_ptmp�ɃR�s�[

	} else if(m_mosaicblock>1) {	//���U�C�N�u���b�N(m_mosaicblock)x(m_mosaicblock)�̏ꍇ

		double sr,sg,sb;	//sum
		int xendflag;//=m_mosaicblock;
		int yendflag;//=m_mosaicblock;


		for(y=0;y<sizey;y+=m_mosaicblock){	
			xendflag=m_mosaicblock;
			yendflag=m_mosaicblock;

			for(x=0;x<sizex;x+=m_mosaicblock){
		
				if(x==sizex-(sizex%m_mosaicblock))	//�E�[���O�ɂȂ�����
					xendflag=sizex%m_mosaicblock;	//�t���O�𗧂Ă�

				if(y==sizey-(sizey%m_mosaicblock))	//���[�[���O�ɂȂ�����
					yendflag=sizey%m_mosaicblock;	//�t���O�𗧂Ă�

				//���U�C�N�u���b�N�̕��ϒl���o��
				sr=0,sg=0,sb=0;

				for(i=0;i<yendflag;i++){
					for(j=0;j<xendflag;j++){
						
						pAdib->GetValue(x+j,y+i,r,g,b);
						sr+=r;
						sg+=g;
						sb+=b;		//�u���b�N�P�x�a
					}
				}
				
				sr/=xendflag*yendflag;
				sg/=xendflag*yendflag;
				sb/=xendflag*yendflag;	//�u���b�N����

				if(sr<0){	//red check
					sr=0;
				} else if(sr>255) {
					sr=255;
				}
				if(sg<0){	//green check
					sg=0;
				}else if(sg>255){
					sg=255;
				}

				if(sb<0){ //blue check
					sb=0;
				}else if(sb>255){
					sb=255;
				}

				for(i=0;i<yendflag;i++){
					for(j=0;j<xendflag;j++){
						del->SetPixel(x+j,y+i,(unsigned char)sr,(unsigned char)sg,(unsigned char)sb);	//�u���b�N���ɐF��񏑂�����
					}
				}

			}
		}
		m_ptmp->CopyCADIB(del);	//del��m_ptmp�ɃR�s�[

	
	} else {
	
		//error����
	
	}

}





void CFilter::WriteDIB(CADIBSection *pAdib)	//�Ώۂɏ����o��
{
	pAdib->CopyCADIB(m_ptmp);
}
