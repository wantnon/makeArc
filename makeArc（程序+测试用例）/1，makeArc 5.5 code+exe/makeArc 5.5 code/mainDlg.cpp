// mainDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GLmfc.h"
#include "mainDlg.h"


#include<vector>
#include<string>
#include<time.h>
using namespace std;
#pragma comment (lib,"glaux.lib")
#pragma comment(linker,"/STACK:0x800000") //Ŀǰջ����8MB��������Ʒָ����������ʧ�ܣ���ֵӦ����
#include <stdlib.h>
#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glut.h>
#include <gl\glaux.h>
#include <math.h>
#include <float.h>
#include <direct.h>//_mkdir
#include <io.h>//_access


//------------------------------ȫ�ֱ���
//-----------��ʾ����
bool showWarning=false;
bool flg=false;
bool showGround=true;//�Ƿ���ʾ����
bool showSmooth=false;//�Ƿ���ʾƽ�����
bool saveSmooth=false;//�Ƿ񱣴�ƽ�����
bool showOutlineNorm=false;//�Ƿ���ʾ����������
bool showOutlineNorm_smooth=false;//�Ƿ���ʾƽ�������ķ�����
bool showKeyPoint=false;//�Ƿ���ʾ�ؼ���
//�Ƿ���Ի��ڽ�Ծ��ǵ�ȥ������
extern const bool ignoreRidTree_baseOnStepMark_default=false;
bool ignoreRidTree_baseOnStepMark=ignoreRidTree_baseOnStepMark_default;
//�Ƿ���Ի��ڽǵ��ǵ�ȥ������
extern const bool ignoreRidTree_baseOnCornerMark_default=false;
bool ignoreRidTree_baseOnCornerMark=ignoreRidTree_baseOnCornerMark_default;
//�Ƿ���й���
extern const bool doRegulation_default=true;
bool doRegulation=doRegulation_default;
//�Ƿ���в��ֹ���
#define RGL_STRONG_WEAK 0//�²�����ǿ�ϲ�����������maxHOfTree�ķֽ�
#define RGL_STRONG 1
#define RGL_WEAK 2
extern const int regulationDegree_default=RGL_STRONG_WEAK;
int regulationDegree=regulationDegree_default;
//�Ƿ�ʹ����ʵ�ݶ�
extern const bool useRealRoof_default=false;
bool useRealRoof=useRealRoof_default;
//-------�������
float maxHOfTree=28;//�����߶ȴ��ڴ˸߶ȵ����žͲ���ȥֲ������
//-------��С��ǿ�����򻯸߶�
float minHNotRegulationStrong=maxHOfTree+4;//��regulationDegree=RGL_STRONG_WEAKʱ������minHNotRegulationStrong�����Ų�ʹ��ǿ�����򻯣���ʹ��������
//-------���öԻ���
#include "setingDlg.h"
//���öԻ������Ϊȫ��������Ϊ�������м��书��
CsetingDlg setingDlg;
//-------������ʾ
bool singleStepMode=false;//�Ƿ�Ϊ������ʾģʽ
int stage=0;//���ĸ��׶�
//-------��ͳ���
const float inf=1000000000000;
const float pi=3.1415926;
//2D������
#define SCREEN_COOR 0
#define MATH_COOR 1
//�������
#define X 0
#define Y 1
#define Z 2
#define W 3

//���
int mosx=0;//���x����
int mosy=0;//���y����
int mosxf=0;//ǰһʱ�����x����
int mosyf=0;//ǰһʱ�����y����
bool leftDown=false;//�Ƿ�����������
bool mosMovedWhileLeftDown=false;//�Ƿ������������µ��������귢�����ƶ�
//����
bool upKeyDown=false;
bool dnKeyDown=false;
bool ltKeyDown=false;
bool rtKeyDown=false;
//���ڳߴ�
int winwidth=768;//���ڿ��
int winheight=512;//���ڸ߶�
//����״̬
#define WS_EMPTY 0//��
#define WS_RAW 1//ԭ��
#define WS_DONE 2//��ģ���
#define WS_MODEL 3//ģ�Ͳ鿴
int workState=WS_EMPTY;
//�۲�ģʽ
#define VM_OUTLINE 0//����
#define VM_OVERLOOK 1//���
#define VM_WANDER 2//����
int viewMode=VM_OUTLINE;
//ͷ�ļ�
#include"mathEx.h"
#include"simpleClass.h"
#include"func.h"
#include"texture.h"
#include"class.h"
#include"text.h"
#include"qTree.h"
#include"groundgrid.h"
#include"groundBlock.h"
#include"my_Delaunay.h"

//���� work flow (WF)
#include"WF_cloud_data_read_in.h"
#include"WF_remove_wall_point.h"
#include"WF_mark_step_point.h"
#include"WF_make_ground_grid_and_remove_ground_point.h"
#include"WF_cal_texCoord_for_groundgrid.h"
#include"WF_cloud_segmentation.h"
#include"WF_cal_avrgH_for_each_cloud.h"
#include"WF_roughly_cal_minH_for_each_cloud.h"
#include"WF_remove_vegetation_point.h"
#include"WF_cloud_settlement.h"
#include"WF_outline_extraction.h"
#include"WF_cal_minH_and_rid_cloud_too_low.h"
#include"WF_cal_bottomH_for_each_cloud.h"
#include"WF_cal_minRect_for_each_cloud.h"
#include"WF_outline_smooth.h"
#include"WF_outline_regulation.h"
#include"WF_make_roof_and_wall.h"
#include"WF_cal_texCoord_for_each_roof.h"
#include"WF_scene_management.h"
#include"WF_save_model.h"
#include"WF_load_model.h"
#include"WF_load_raw_head_file.h"
//����
#include"z_����.h"
#include"z_ģ���ļ���ʽ.h"
#include"z_����ԭ��.h"
#include"z_����.h"
//��Դ����
float light_pos[4]={500,100,-100};

//�ӽ����
float campos[4];//���λ��
float camtg[4];//Ŀ���
const float A0=0;
const float B0=0;
float A=A0;//ˮƽ��ת��(��Z��нǣ���Zת��XΪ����)
float dA=10*pi/180;//A������
float B=B0;//��ֱ��ת��(��XZƽ��н�)
float dB=10*pi/180;//B������
float dis_cam_to_world_origin;//���������ԭ��ľ���
const float max_dis_cam_to_world_origin=2000;//���������ԭ���������
const float min_dis_cam_to_world_origin=1;//���������ԭ�����С����


//����
CTextureManager textureManager;

//��������
myvector<Cv4> vlist;//���б�
float xmin,xmax,ymin,ymax,zmin,zmax;//vlist��Χ�������Χ���������ݵ�ԭʼ���ƾ�ȷ����ģ������Ӱ�캽Ƭ��ͼЧ����
float offsetus=0;//-0.003;//��ͼ����������ֵ
float offsetut=0;//0.005;//��ͼ����������ֵ
myvector<Ccloud> cloudList;//�����б�ÿ����һ��cloudǰ���������ж����Ƿ���removed����������Ϊǰ�����˽�����ʡ�Դ��жϣ�
CqTree qTree;//�Ĳ���
CgroundBlock groundBlock;//���ο��Ĳ���
Cme me;//�۲���

//��ֹʱ��
long beginTime=0;
long endTime=0;
//��ɫ��
float colorTable[256][3];
//��ǰ�ļ�·��
string curPathName;//��ǰ�򿪵�������ļ�·��
string picFileName_photo;//������Ƭ�ļ���
string picFileName_wall;//ǽ��ͼƬ�ļ���
string picFileName_sky;//���ͼƬ�ļ���
const string progName="makeArc";//������
//��ʾ���ֵ�λ��
const float x_text_midup=300;
const float y_text_midup=50;



//---------------------------------------------------------------
#include "stdafx.h"
#include "GLmfc.h"
#include "mainDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CmainDlg dialog


CmainDlg::CmainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CmainDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CmainDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CmainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CmainDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CmainDlg, CDialog)
	//{{AFX_MSG_MAP(CmainDlg)
	ON_WM_CREATE()
	ON_COMMAND(ID_open, Onopen)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_run, Onrun)
	ON_WM_TIMER()
	ON_COMMAND(ID_close, Onclose)
	ON_COMMAND(ID_about, Onabout)
	ON_COMMAND(ID_save, Onsave)
	ON_COMMAND(ID_load_model, Onloadmodel)
	ON_WM_CLOSE()
	ON_COMMAND(ID_runSingle, OnrunSingle)
	ON_COMMAND(ID_seting, Onseting)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CmainDlg message handlers

void CmainDlg::run(){
	
	
	//��ʼ��ʱ
	{
		cout<<"��ʼ����...  "<<endl;
		beginTime =clock();//��ÿ�ʼʱ�䣬��λΪ����
	}
	//ȥǽ�ڵ�
	cout<<"ȥǽ�ڵ�"<<endl;
	{
		main_remove_wall_point(vlist,xmin,xmax,ymin,ymax,zmin,zmax);
	}
	//��ǽ�Ծ��
	{
		if(ignoreRidTree_baseOnStepMark){
			cout<<"��ǽ�Ծ�㣨������"<<endl;
		}else{
			cout<<"��ǽ�Ծ��"<<endl;
			main_mark_step_point(vlist,xmin,xmax,ymin,ymax,zmin,zmax);
		}
	
	}
	//���ɵ�������ȥ�����
	cout<<"ȥ����㼰���ɵ���"<<endl;
	{
		main_make_ground_grid_and_remove_ground_point(vlist,xmin,xmax,ymin,ymax,zmin,zmax,groundBlock.GroundGrid);
	
	}
	//ΪGroundGrid������������
	cout<<"���������������"<<endl;
	{
		main_cal_texCoord_for_groundgrid(groundBlock.GroundGrid,offsetus,offsetut);
	}
	//���Ʒָ�
	cout<<"���Ʒָ�"<<endl;
	{
		main_cloud_segmentation(vlist,xmin,xmax,ymin,ymax,zmin,zmax,cloudList,ignoreRidTree_baseOnStepMark);
	}
	//ɾ��vlist
	cout<<"ɾ��vlist"<<endl;
	{
		vlist.swap(myvector<Cv4>());
	}
	//���������ƽ���߶�
	cout<<"����avrgH"<<endl;
	{
		main_cal_avrgH_for_each_cloud(cloudList);
	}
	//��������ŵĽ���minH
	cout<<"���Լ���minH"<<endl;
	{
		main_roughly_cal_minH_for_each_cloud(cloudList,groundBlock.GroundGrid);
	}
	//ȥֲ����

	{
		if(ignoreRidTree_baseOnStepMark){
			cout<<"ȥֲ���㣨������"<<endl;
		}else{
			cout<<"ȥֲ����"<<endl;
			main_remove_vegetation_point(cloudList);
		}
	
	}
	//���¼��������ƽ���߶�
	cout<<"���¼���avrgH"<<endl;
	{
		main_cal_avrgH_for_each_cloud(cloudList);
	}
	//���Ƴ���
	cout<<"���Ƴ���"<<endl;
	{
		main_cloud_settlement(cloudList);
	
	}
	//������ȡ
	cout<<"������ȡ"<<endl;
	{
		main_outline_extraction(cloudList);
	
	}
	//��������ŵ�minH����ɾ�����͵�����
	cout<<"����minH��ɾ����������"<<endl;
	{
		main_cal_minH_and_rid_cloud_too_low(cloudList,groundBlock.GroundGrid);
	}
	//��������ŵ�bottomH
	cout<<"����bottomH"<<endl;
	{
		main_cal_bottomH_for_each_cloud(cloudList);
	}
	//����С��Χ����
	cout<<"����С��Χ����"<<endl;
	{
		main_cal_minRect_for_each_cloud(cloudList);
	
	}
	//����ƽ��
	cout<<"����ƽ��"<<endl;
	{
		main_outline_smooth(cloudList);
	
	}
	//��������
	cout<<"��������"<<endl;
	{
		main_outline_regulation(cloudList,doRegulation);
	}
	//���ݶ���ǽ����
	cout<<"���ݶ���ǽ��"<<endl;
	{
		main_make_roof_and_wall(cloudList,groundBlock.GroundGrid,ignoreRidTree_baseOnCornerMark,useRealRoof);
	}
	//�����ݶ���������
	cout<<"�����ݶ���������"<<endl;
	{
		main_cal_texCoord_for_each_roof(cloudList,xmin,xmax,zmin,zmax,offsetus,offsetut);

	}
	//��������
	cout<<"��������"<<endl;
	{
		main_scene_management(cloudList,qTree,groundBlock,xmin,xmax,zmin,zmax,ymin,ymax,textureManager);
	}


	//������ʱ�����㴦����ʱ
	{
		endTime=clock();//��ý���ʱ��
		cout<<"���!  ";
		cout<<"������ʱ:"<<endTime-beginTime<<"ms"<<endl;
	}


}

void CmainDlg::OnrunSingle() 
{
	// TODO: Add your command handler code here
	addText("in processing...",x_text_midup,y_text_midup);
	if(stage==0){
		Onseting();
	}
	singleStepMode=true;
	updateMenuItemEnableState();
	int nStepExcuted=0;//����ִ���˼���
	switch(stage){

	case 0://ȥǽ�ڵ�
		{
			
			cout<<"ȥǽ�ڵ�...";
			main_remove_wall_point(vlist,xmin,xmax,ymin,ymax,zmin,zmax);
			cout<<"���"<<endl;
			//
			nStepExcuted=1;
			
		}
		break;
	case 1://��ǽ�Ծ��
		{	
			if(ignoreRidTree_baseOnStepMark){
				cout<<"��ǽ�Ծ�㣨������"<<endl;
			}else{
				cout<<"��ǽ�Ծ��...";
				main_mark_step_point(vlist,xmin,xmax,ymin,ymax,zmin,zmax);	
				cout<<"���"<<endl;	
			}
			//
			nStepExcuted=1;
		}
		break;
	case 2://���ɵ�������ȥ����� && ΪGroundGrid������������
		{
			
			cout<<"ȥ����㼰���ɵ���...";
			main_make_ground_grid_and_remove_ground_point(vlist,xmin,xmax,ymin,ymax,zmin,zmax,groundBlock.GroundGrid);
			cout<<"���"<<endl;
			cout<<"���������������...";
			main_cal_texCoord_for_groundgrid(groundBlock.GroundGrid,offsetus,offsetut);
			cout<<"���"<<endl;
			//
			nStepExcuted=2;
		}
		break;
	case 3:
		{
			cout<<"error! ��Ӧ���˷�֧"<<endl;
			return;
		}
		break;
	case 4://���Ʒָ� && ɾ��vlist && ���������ƽ���߶� && ��������ŵĽ���minH
		{
			cout<<"���Ʒָ�...";
			main_cloud_segmentation(vlist,xmin,xmax,ymin,ymax,zmin,zmax,cloudList,ignoreRidTree_baseOnStepMark);
			cout<<"���"<<endl;
			cout<<"ɾ��vlist...";
			vlist.swap(myvector<Cv4>());
			cout<<"���"<<endl;
			cout<<"����avrgH...";
			main_cal_avrgH_for_each_cloud(cloudList);
			cout<<"���"<<endl;
			cout<<"���Լ���minH...";
			main_roughly_cal_minH_for_each_cloud(cloudList,groundBlock.GroundGrid);
			cout<<"���"<<endl;
			//
			nStepExcuted=4;
		}
		break;
	case 5:
	case 6:
	case 7:
		{
			cout<<"error! ��Ӧ���˷�֧"<<endl;
			return;
		}
		break;
	case 8://ȥֲ���� && ���¼��������ƽ���߶�
		{
			if(ignoreRidTree_baseOnStepMark){
				cout<<"ȥֲ���㣨������"<<endl;
			}else{
				cout<<"ȥֲ����...";
				main_remove_vegetation_point(cloudList);
				cout<<"���"<<endl;	
			}
			cout<<"���¼���avrgH...";
			main_cal_avrgH_for_each_cloud(cloudList);
			cout<<"���"<<endl;
			//
			nStepExcuted=2;
		}
		break;
	case 9:
		{
			cout<<"error! ��Ӧ���˷�֧"<<endl;
			return;
		}
		break;
	case 10://���Ƴ���
		{
			cout<<"���Ƴ���...";
			main_cloud_settlement(cloudList);
			cout<<"���"<<endl;
			//
			nStepExcuted=1;
		}
		break;
	case 11://������ȡ
		{
			cout<<"������ȡ...";
			main_outline_extraction(cloudList);
			cout<<"���"<<endl;
			//
			nStepExcuted=1;

		}
		break;
	case 12://��������ŵ�minH,��ɾ�����͵����� && ��������ŵ�bottomH && ����С��Χ����
		{
			cout<<"����minH��ɾ����������...";
			main_cal_minH_and_rid_cloud_too_low(cloudList,groundBlock.GroundGrid);
			cout<<"���"<<endl;
			cout<<"����bottomH...";
			main_cal_bottomH_for_each_cloud(cloudList);
			cout<<"���"<<endl;
			cout<<"����С��Χ����...";
			main_cal_minRect_for_each_cloud(cloudList);
			cout<<"���"<<endl;
			//
			nStepExcuted=3;
		}
		break;
	case 13:
	case 14:
		{
			cout<<"error! ��Ӧ���˷�֧"<<endl;
			return;
		}
		break;
	case 15://����ƽ��
		{
			cout<<"����ƽ��...";
			main_outline_smooth(cloudList);
			cout<<"���"<<endl;
			//
			nStepExcuted=1;
			
		}
		break;
	case 16://��������
		{
			cout<<"��������...";
			main_outline_regulation(cloudList,doRegulation);
			cout<<"���"<<endl;
			//
			nStepExcuted=1;
		}
		break;
	case 17://���ݶ���ǽ���� && �����ݶ��������� && ��������
		{
			cout<<"���ݶ���ǽ��...";
			cout<<endl;
			main_make_roof_and_wall(cloudList,groundBlock.GroundGrid,ignoreRidTree_baseOnCornerMark,useRealRoof);
			cout<<"���"<<endl;
			cout<<"�����ݶ���������...";
			main_cal_texCoord_for_each_roof(cloudList,xmin,xmax,zmin,zmax,offsetus,offsetut);
			cout<<"���"<<endl;
			cout<<"��������...";
			main_scene_management(cloudList,qTree,groundBlock,xmin,xmax,zmin,zmax,ymin,ymax,textureManager);
			cout<<"���"<<endl;
			//---------��ʱģ���ѽ���
			//����ģʽ��λ
			stage=0;
			singleStepMode=false;
			//��״̬
			workState=WS_DONE;
			viewMode=VM_OVERLOOK;
			this->Invalidate(false);
			//���²˵������״̬
			updateMenuItemEnableState();
			//����
			return;

		}
		break;
	case 18:
	case 19:
		{
			cout<<"error! ��Ӧ���˷�֧"<<endl;
			return;
		}
		break;
	default:
		{
			cout<<"error! ��Ӧ���˷�֧"<<endl;
			return;
		}
		break;
	}
		
	if(nStepExcuted==0){
		cout<<"error: nStepExcuted==0!"<<endl;
		return;
	}
	stage+=nStepExcuted;

	OnPaint();
		
		
		
		
		
		
}

void CmainDlg::draw(){
	//clear all pixels
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);	
	//���ù۲�Ƕ�
	glMatrixMode(GL_MODELVIEW);//�л��� ģ��-��ͼ ģʽ
	glLoadIdentity();//��һ��
	//�������λ��
	if(singleStepMode==true
		||workState==WS_RAW
		||(workState==WS_DONE&&(viewMode==VM_OUTLINE||viewMode==VM_OVERLOOK))
		||(workState==WS_MODEL&&(viewMode==VM_OUTLINE||viewMode==VM_OVERLOOK))
		){//���ģʽ
		campos[X]=dis_cam_to_world_origin*cos(B)*sin(A);
		campos[Y]=dis_cam_to_world_origin*sin(B);
		campos[Z]=dis_cam_to_world_origin*cos(B)*cos(A);
		init4(camtg,0,0,0,1);
	}else if((workState==WS_DONE&&viewMode==VM_WANDER)
		||(workState==WS_MODEL&&viewMode==VM_WANDER)
		){//����ģʽ
		me.toWorld(0,10,-15,1,camtg);
		float campos_inMe[4]={0,10,6,1};//��������ϵ�������λ��
		me.toWorld(campos_inMe[X],
			campos_inMe[Y],
			campos_inMe[Z],
			campos_inMe[W],campos);//�õ�campos
	}
	gluLookAt(campos[X],campos[Y],campos[Z],//���λ��
		camtg[X],camtg[Y],camtg[Z],//��ע��
		0,1,0);//������Ϸ���
	glLightfv(GL_LIGHT0,GL_POSITION,light_pos);
	groundBlock.updateCampos(campos);
	//--------------------------����workState��ͬ״̬��ʾ��ͬ�Ļ���
	if(!singleStepMode){
		if(workState==WS_EMPTY){
			
		}else if(workState==WS_RAW){
			//������
			{
				int nv=(int)vlist.size();
				glBegin(GL_POINTS);
				for(int i=0;i<nv;i++){
					float Hrate=(vlist[i].v[Y]-ymin)/(ymax-ymin);
					float r,g,b;
					//	//�򵥼�����ɫֵ
					//	{
					//		r=2*Hrate;
					//		g=Hrate<=0.5?4*Hrate:4*(1-Hrate);
					//		b=1-Hrate;
					//  }
					//ʹ����ɫ�������ɫֵ
					{	
						//����ɫ������ɫ
						int colorIndex=Hrate*255;
						r=colorTable[colorIndex][0];
						g=colorTable[colorIndex][1];
						b=colorTable[colorIndex][2];	
					}
					glColor3f(r,g,b);
					glVertex3fv(vlist[i].v);
				}
				glEnd();
				glColor3f(1,1,1);
			}
			//�ڵ�����Ϻ�Ƭ
			{
				//     
				// v0(xmin,zmin)--v3(xmax,zmin)
				//     |               |
				//     |               |
				// v1(xmin,zmax)--v2(xmax,zmax)
				//
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, textureManager.getTex(0));
				glBegin(GL_QUADS);
				glTexCoord2f(0,0);glVertex3f(xmin,0,zmin);
				glTexCoord2f(0,1);glVertex3f(xmin,0,zmax);
				glTexCoord2f(1,1);glVertex3f(xmax,0,zmax);
				glTexCoord2f(1,0);glVertex3f(xmax,0,zmin);
				glEnd();
				glDisable(GL_TEXTURE_2D);	
			}
		}else if(workState==WS_DONE||workState==WS_MODEL){
			//�����ɽ��
			if(viewMode==VM_OUTLINE){
				glDisable(GL_FOG);
				//��IDoutlineList��Ԫ�ص�polygenList_smoothRes
				if(showSmooth)
				{
					glLineWidth(1);	
					int ncloud=(int)cloudList.size();
					for(int i=0;i<ncloud;i++){
						Ccloud&cloud=cloudList[i];
						if(cloud.removed)continue;
						float bottomH=cloud.bottomH;
						int npolygen=(int)cloud.polygenList_smoothRes.size();
						for(int j=0;j<npolygen;j++){
							Cpolygen&polygen=cloud.polygenList_smoothRes[j];
							//��polygen
							int nv=polygen.vlist.size();
							for(int k=0;k<nv;k++){
								Cv4Ex&v=polygen.vlist[k];
								float *v1=polygen.vlist[k].v;
								float *v2=polygen.vlist[(k+1)%nv].v;
								//����
								glColor3f(1,1,1);
								glBegin(GL_LINES);
								glVertex3f(v1[X],bottomH,v1[Z]);
								glVertex3f(v2[X],bottomH,v2[Z]);
								glEnd();
								glColor3f(1,1,1);
								//���ҷ�����
								if(showOutlineNorm_smooth){
									float norm[4];
									float dir[4];
									sub(v2,v1,dir);
									init4(norm,-dir[Z],dir[Y],dir[X],0);
									normalize(norm);
									float beg[4]={(v1[X]+v2[X])/2,bottomH,(v1[Z]+v2[Z])/2,1};
									float end[4];
									add(beg,norm,end);
									glBegin(GL_LINES);
									glVertex3f(beg[X],bottomH,beg[Z]);
									glVertex3f(end[X],bottomH,end[Z]);
									glEnd();	
								}
							}
						}
					}
					glLineWidth(1);
				}
				//����polygenList
				{
					int ncloud=(int)cloudList.size();
					for(int i=0;i<ncloud;i++){
						Ccloud&cloud=cloudList[i];
						if(cloud.removed)continue;
						//	bool selfHost=cloud.selfHost;
						bool takenOut=cloud.takenOut;
						float bottomH=cloud.bottomH;
						int npolygen=(int)cloud.polygenList.size();
						for(int j=0;j<npolygen;j++){
							Cpolygen&polygen=cloud.polygenList[j];
							Cv4 color;
							veccopy(cloud.polygenList[j].color,color.v);	
							//��polygen
							int nv=polygen.vlist.size();
							for(int k=0;k<nv;k++){
								Cv4Ex&v=polygen.vlist[k];
								float *v1=polygen.vlist[k].v;
								float *v2=polygen.vlist[(k+1)%nv].v;
								//����
								if(/*selfHost*/takenOut){
									glLineWidth(2);
								}else{
									glLineWidth(1);
								}
								glColor3fv(cloud.color);
								glBegin(GL_LINES);
								glVertex3f(v1[X],bottomH,v1[Z]);
								glVertex3f(v2[X],bottomH,v2[Z]);
								glEnd();
								glColor3f(1,1,1);
								glLineWidth(1);
								//��isKeyPoint
								if(showKeyPoint){
									if(polygen.vlist[k].isKeyPoint){
										glPointSize(3);
										glBegin(GL_POINTS);
										glVertex3fv(v1);
										glEnd();
										glPointSize(1);	
									}
								}
								//���ҷ�����
								if(showOutlineNorm)
								{
									float norm[4];
									float dir[4];
									sub(v2,v1,dir);
									init4(norm,-dir[Z],dir[Y],dir[X],1);
									normalize(norm);
									float beg[4]={(v1[X]+v2[X])/2,bottomH,(v1[Z]+v2[Z])/2,1};
									float end[4];
									add(beg,norm,end);
									glBegin(GL_LINES);
									glVertex3f(beg[X],bottomH,beg[Z]);
									glVertex3f(end[X],bottomH,end[Z]);
									glEnd();	
								}	
							}			
						}	
					}		
				}
			}else if(viewMode==VM_OVERLOOK){
				if(showGround)groundBlock.show();
				qTree.cutAndDrawqTree(cloudList,campos);
			}else if(viewMode==VM_WANDER){
				glEnable(GL_FOG);
				if(showGround)groundBlock.show();
				qTree.cutAndDrawqTree(cloudList,campos);
				me.show();	
				//----����պ�
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, textureManager.getTex(2));
				double density_store;
				glGetDoublev(GL_FOG_DENSITY,&density_store);//��õ�ǰ��Ũ��
				glFogf(GL_FOG_DENSITY, density_store*0.2);//������Ũ��
				ball_seeback(12,//��������̫�ͣ�������ȱ��
					0,0,0,2*max((xmax-xmin)/2,(zmax-zmin)/2),campos,0.5);
				glFogf(GL_FOG_DENSITY, density_store);//�ָ���Ũ��
				glDisable(GL_TEXTURE_2D);
				glDisable(GL_FOG);
			}
		}else{
			cout<<"״̬δ����!"<<endl;
			return;
		}
		//------------------------------��ʾ����
		const float h=20;//�и�
		//���Ͻ���ʼ����
		const float x=10;
		const float y=50;
		//��ʾ����
		glDisable(GL_LIGHTING);
		Begin2D(SCREEN_COOR);
		if(workState==WS_EMPTY){
			Text("empty",x,y);	
		}else if(workState==WS_RAW){
			Text("preview",x,y);
		}else if(workState==WS_DONE){
			Text("done",x,y);
			Text("use V to switch view mode",x,y+h);
			if(viewMode==VM_OUTLINE){
				
			}else if(viewMode==VM_OVERLOOK){
					
			}else if(viewMode==VM_WANDER){
				Text("use Arrow Keys to move",x,y+2*h);
			}
		}else if(workState==WS_MODEL){
			Text("model",x,y);
			Text("use V to switch view mode",x,y+h);
			if(viewMode==VM_OUTLINE){
				
			}else if(viewMode==VM_OVERLOOK){
					
			}else if(viewMode==VM_WANDER){
				Text("use Arrow Keys to move",x,y+2*h);
			}
		}else{
			cout<<"״̬δ����!"<<endl;
			return;
		}
		End2D();
		glEnable(GL_LIGHTING);
	}else{//singleStepMode
		switch(stage){
		case 1://��ʾȥǽ�ڵ���
			{
				int nv=(int)vlist.size();
				glColor3f(0,0,1);
				glBegin(GL_POINTS);
				for(int i=0;i<nv;i++){
					
					glVertex3fv(vlist[i].v);
				}
				glEnd();
				glColor3f(1,1,1);
			}
			break;
		case 2://��ʾ��ǽ�Ծ����
			{
				int nv=(int)vlist.size();
				
				for(int i=0;i<nv;i++){
					Cv4&v=vlist[i];
					if(v.isEdgePoint){
						glColor3f(1,0,0);
					}else{
						glColor3f(0,0,1);
					}
					glBegin(GL_POINTS);//�����glBegin����ѭ���⣬�����᲻һ��!!!!!!!
					glVertex3fv(vlist[i].v);
					glEnd();
				}
				
				glColor3f(1,1,1);
			}
			break;
		case 3://��ʾ���ɵ�������ȥ�������
		case 4://��ʾΪGroundGrid��������������
			{
				int nv=(int)vlist.size();
				
				for(int i=0;i<nv;i++){
					Cv4&v=vlist[i];
					if(v.isEdgePoint){
						glColor3f(1,0,0);
					}else{
						glColor3f(0,0,1);
					}
					glBegin(GL_POINTS);
					glVertex3fv(vlist[i].v);
					glEnd();
				}
				
				glColor3f(1,1,1);	
				
			}
			break;
		case 5://��ʾ���Ʒָ���
		case 6://��ʾɾ��vlist���
		case 7://��ʾ���������ƽ���߶Ƚ��
		case 8://��ʾ��������ŵĽ���minH���
			{
				//��������
				
				int ncloud=(int)cloudList.size();
				for(int i=0;i<ncloud;i++){
					Ccloud&cloud=cloudList[i];
					if(cloud.removed)continue;
					//��cloud
					glColor3fv(cloud.color);
					glBegin(GL_POINTS);
					int nv=(int)cloud.vlist.size();
					for(int j=0;j<nv;j++){
						Cv4&v=cloud.vlist[j];
						glVertex3fv(v.v);
					}
					glEnd();
				
				}
				
				glColor3f(1,1,1);
				
			}
			break;
		case 9://��ʾȥֲ������
		case 10://��ʾ���¼��������ƽ���߶Ƚ��
		case 11://��ʾ���Ƴ������
			{
				//��������
				
				int ncloud=(int)cloudList.size();
				for(int i=0;i<ncloud;i++){
					Ccloud&cloud=cloudList[i];
					if(cloud.removed)continue;
					//��cloud
					glColor3fv(cloud.color);
					glBegin(GL_POINTS);
					int nv=(int)cloud.vlist.size();
					for(int j=0;j<nv;j++){
						Cv4&v=cloud.vlist[j];
						glVertex3fv(v.v);
					}
					glEnd();
				
				}
				
				glColor3f(1,1,1);
				
			}
			break;
		case 12://��ʾ������ȡ���
		case 13://��ʾ��������ŵ�minH����ɾ�����͵����Ž��
		case 14://��ʾ��������ŵ�bottomH���
		case 15://��ʾ����С��Χ���ν��
		case 16://��ʾ����ƽ�����
			{
				//����polygenList
				int ncloud=(int)cloudList.size();
				for(int i=0;i<ncloud;i++){
					Ccloud&cloud=cloudList[i];
					if(cloud.removed)continue;
					float bottomH=cloud.bottomH;
					int npolygen=(int)cloud.polygenList.size();
					for(int j=0;j<npolygen;j++){
						Cpolygen&polygen=cloud.polygenList[j];
						Cv4 color;
						veccopy(cloud.polygenList[j].color,color.v);	
						//��polygen
						int nv=polygen.vlist.size();
						for(int k=0;k<nv;k++){
							Cv4Ex&v=polygen.vlist[k];
							float *v1=polygen.vlist[k].v;
							float *v2=polygen.vlist[(k+1)%nv].v;
							//����
							glColor3fv(cloud.color);
							glBegin(GL_LINES);
							glVertex3f(v1[X],bottomH,v1[Z]);
							glVertex3f(v2[X],bottomH,v2[Z]);
							glEnd();
							glColor3f(1,1,1);
							glLineWidth(1);
						}			
					}	
				}
				
			}
			break;
		case 17://��ʾ�������򻯽��
			{
				int ncloud=(int)cloudList.size();
				for(int i=0;i<ncloud;i++){
					Ccloud&cloud=cloudList[i];
					if(cloud.removed)continue;
					//	bool selfHost=cloud.selfHost;
					bool takenOut=cloud.takenOut;
					float bottomH=cloud.bottomH;
					int npolygen=(int)cloud.polygenList.size();
					for(int j=0;j<npolygen;j++){
						Cpolygen&polygen=cloud.polygenList[j];
						Cv4 color;
						veccopy(cloud.polygenList[j].color,color.v);	
						//��polygen
						int nv=polygen.vlist.size();
						for(int k=0;k<nv;k++){
							Cv4Ex&v=polygen.vlist[k];
							float *v1=polygen.vlist[k].v;
							float *v2=polygen.vlist[(k+1)%nv].v;
							//����
							if(/*selfHost*/takenOut){
								glLineWidth(2);
							}else{
								glLineWidth(1);
							}
							glColor3fv(cloud.color);
							glBegin(GL_LINES);
							glVertex3f(v1[X],bottomH,v1[Z]);
							glVertex3f(v2[X],bottomH,v2[Z]);
							glEnd();
							glColor3f(1,1,1);
							glLineWidth(1);
							//��isKeyPoint
							if(showKeyPoint){
								if(polygen.vlist[k].isKeyPoint){
									glPointSize(3);
									glBegin(GL_POINTS);
									glVertex3fv(v1);
									glEnd();
									glPointSize(1);	
								}
							}
							//���ҷ�����
							if(showOutlineNorm)
							{
								float norm[4];
								float dir[4];
								sub(v2,v1,dir);
								init4(norm,-dir[Z],dir[Y],dir[X],1);
								normalize(norm);
								float beg[4]={(v1[X]+v2[X])/2,bottomH,(v1[Z]+v2[Z])/2,1};
								float end[4];
								add(beg,norm,end);
								glBegin(GL_LINES);
								glVertex3f(beg[X],bottomH,beg[Z]);
								glVertex3f(end[X],bottomH,end[Z]);
								glEnd();	
							}	
						}			
					}	
				}	
				
			}
			break;
		case 18://��ʾ���ݶ���ǽ������
			{
				//������
				int ncloud=(int)cloudList.size();
				for(int j=0;j<ncloud;j++){
					Ccloud&cloud=cloudList[j];
					if(cloud.removed)continue;
					//���ݶ���
					{
						glDisable(GL_LIGHTING);
						cloud.grid.showMesh_rough0(textureManager.getTex(0),false,1,0,0,1);
						glEnable(GL_LIGHTING);
						
					}
					//��ǽ��
					{
						
						myvector<Cpolygen>&polygenList=cloud.polygenList;
						myvector<Cpolygen>&polygenList_dn=cloud.polygenList_dn;
						drawWall_polygenList_polygenList(polygenList,polygenList_dn,textureManager.getTex(1),1,false);
						
					}
					
				}
				//������
				groundBlock.GroundGrid.showGround(textureManager.getTex(0),false,0,1,0,1);

			}
		case 19://��ʾ�����ݶ�����������
			{

				
				
				//������
				int ncloud=(int)cloudList.size();
				for(int j=0;j<ncloud;j++){
					Ccloud&cloud=cloudList[j];
					if(cloud.removed)continue;
					//���ݶ���
					{
						glDisable(GL_LIGHTING);
						if(cloud.isRed)glColor3f(1,0,0);
						cloud.grid.showMesh_rough0(textureManager.getTex(0));
						if(cloud.isRed)glColor3f(1,1,1);
						glEnable(GL_LIGHTING);
						
					}
					//��ǽ��
					{
						
						myvector<Cpolygen>&polygenList=cloud.polygenList;
						myvector<Cpolygen>&polygenList_dn=cloud.polygenList_dn;
						drawWall_polygenList_polygenList(polygenList,polygenList_dn,textureManager.getTex(1));
						
					}
					
				}
				//������
				groundBlock.GroundGrid.showGround(textureManager.getTex(0));
			}
			break;
		case 20://��ʾ����������
			{
				if(showGround)groundBlock.show();
				qTree.cutAndDrawqTree(cloudList,campos);
				
			}
			break;
		default:
			{
				
			}
			break;
		}
	
	
	}
	
	
	
	
	
	
	
}


// �������ظ�ʽ����
int CmainDlg::MySetPixelFormat(HDC hdc)
{
	PIXELFORMATDESCRIPTOR pfd = { 
	    sizeof(PIXELFORMATDESCRIPTOR),    // pfd�ṹ�Ĵ�С 
	    1,                                // �汾�� 
	    PFD_DRAW_TO_WINDOW |              // ֧���ڴ����л�ͼ 
	    PFD_SUPPORT_OPENGL |              // ֧�� OpenGL 
	    PFD_DOUBLEBUFFER,                 // ˫����ģʽ 
	    PFD_TYPE_RGBA,                    // RGBA ��ɫģʽ 
	    24,                               // 24 λ��ɫ��� 
	    0, 0, 0, 0, 0, 0,                 // ������ɫλ 
	    0,                                // û�з�͸���Ȼ��� 
	    0,                                // ������λλ 
	    0,                                // ���ۼӻ��� 
	    0, 0, 0, 0,                       // �����ۼ�λ 
	    32,                               // 32 λ��Ȼ���     
	    0,                                // ��ģ�建�� 
	    0,                                // �޸������� 
	    PFD_MAIN_PLANE,                   // ���� 
	    0,                                // ���� 
	    0, 0, 0                           // ���Բ�,�ɼ��Ժ������ģ 
	}; 
	
	int  iPixelFormat; 
 
	// Ϊ�豸������õ���ƥ������ظ�ʽ 
	if((iPixelFormat = ChoosePixelFormat(hdc, &pfd)) == 0)
	{
		MessageBox("ChoosePixelFormat Failed", NULL, MB_OK);
		return 0;
	}
	 
	// ������ƥ������ظ�ʽΪ��ǰ�����ظ�ʽ 
	if(SetPixelFormat(hdc, iPixelFormat, &pfd) == FALSE)
	{
		MessageBox("SetPixelFormat Failed", NULL, MB_OK);
		return 0;
	}

	return 1;
}

int CmainDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here

	//���ô��ڴ�С
	this->SetWindowPos(NULL,0,0,winwidth,winheight,SWP_NOMOVE);
	// ���õ�ǰ�Ļ�ͼ���ظ�ʽ
	MySetPixelFormat(::GetDC(m_hWnd));
	// ��û�ͼ������
	hdc = ::GetDC(m_hWnd);
	// ������Ⱦ������
	hglrc = wglCreateContext(hdc);
	// ʹ��ͼ������Ϊ��ǰ�����̵ֳĵ�ǰ��ͼ������ 
	wglMakeCurrent(hdc, hglrc);	
	////-----------------------------------------------------------
	//select clearing(background) color
	glClearColor(0.0,0.0,0.0,0.0);
	//-------------------------------------------����-------------------------------------------------------------------------- 
	//----ѡ����������
	//������GL_FLAT��GL_SMOOTH
	glShadeModel(GL_SMOOTH);
	//----���ò���
	//���ԶԲ�ͬ����GL_FRONT,GL_BACK,GL_FRONT_AND_BACK����������Ŀ��
	//GL_AMBIENT(������ɫ),GL_DIFFUSE(ɢ����ɫ),GL_AMBIENT_AND_DIFFUSE,GL_SPECULAR(������ɫ),GL_SHININESS(����ָ��),
	//GL_EMISSION,GL_COLOR_INDEXES
	//����ֻ��GL_SPECULAR��GL_SHININESS�������ã�������Ĭ��ֵ
	GLfloat mat_specular[]={0,0,0,1};
	glMaterialfv(GL_FRONT,GL_SPECULAR,mat_specular);//������ɫ
	GLfloat mat_shininess[]={50};
	glMaterialfv(GL_FRONT,GL_SHININESS,mat_shininess);//����ָ��
	GLfloat mat_emission[]={0.3,0.3,0.3,0.0};
	glMaterialfv(GL_FRONT,GL_EMISSION,mat_emission);

	//----���ù�ԴLIGHT0
	//����8����ԴLIGHT0~LIGHT7
	//��������GL_AMBIENT,GL_DIFFUSE,GL_SPECULAR,GL_POSITION,GL_SPOT_DIRECTION,GL_SPOT_EXPONENT,GL_SPOT_CUTOFF,
	//GL_CONSTANT_ATTENUATION,GL_LINEAR_ATTENUATION,GL_QUADRATIC_ATTENUATION
	//����ֻ��GL_DIFFUSE��GL_SPECULAR�������ã�������Ĭ��ֵ
	GLfloat light0_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light0_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light0_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
    glLightfv(GL_LIGHT0,GL_DIFFUSE,light0_diffuse);//������
	glLightfv(GL_LIGHT0,GL_SPECULAR,light0_specular);//�����
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
	//----���ù���ģ��
	//����GL_LIGHT_MODEL_AMBIENT,GL_LIGHT_MODEL_LOCAL_VIEWR,GL_LIGHT_MODEL_TWO_SIDE,GL_LIGHT_MODEL_COLOR_CONTROL
	//����ֻ��GL_LIGHT_MODEL_AMBIENT����һ�����ã�������Ĭ��ֵ
	GLfloat lmodel_ambient[]={0.5,0.5,0.5,1.0};
   	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,lmodel_ambient);
	//----������
	glFogi(GL_FOG_MODE,  GL_EXP2);
	float fogColor[4] = {80.0/255,80.0/255,100.0/255,0.5};    
	glFogfv(GL_FOG_COLOR, fogColor);                
	glFogf(GL_FOG_DENSITY, 0.003f);//���Ũ�ȣ�����ָ��ģʽ��    
	glHint(GL_FOG_HINT,GL_FASTEST);// ����ϵͳ��μ�������
	glFogf(GL_FOG_START, 1.0f);// �����Ŀ�ʼλ��


	

	//-----------------------------------����-------------------------------
	glEnable(GL_DEPTH_TEST);//������Ȳ���
	glEnable(GL_LIGHTING);//��������
	glEnable(GL_LIGHT0);//����LIGHT0
	glEnable(GL_COLOR_MATERIAL);//������ɫ����ģʽ(ʹ��ɫ������Զ����)
	//----------------------------����ӿڴ�С-------------------------------
	GLint viewport[4];//4��Ԫ������Ϊx,y,width,height
	glGetIntegerv(GL_VIEWPORT,viewport);
	int width=viewport[2];
	int height=viewport[3];
	
	//----------------------------������׶------------------------------------
	glMatrixMode(GL_PROJECTION);//�л���ͶӰģʽ
	glLoadIdentity();//��һ��
	float nearface=0.1;//����Ϊ0����ͶӰģʽ�²���������ʾ
	float farface=1.5*max_dis_cam_to_world_origin;//Զ����
	gluPerspective(45,//�ӽ�
		(float)width/height, //���ں��ݱ�
		nearface,//������
		farface);//Զ����


	//--------------------------������ɫ��
	makeColorTable(colorTable);

	//--------------------------���²˵������״̬
	updateMenuItemEnableState();


	return 0;
}

void CmainDlg::Onopen() 
{
	// TODO: Add your command handler code here
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY,"(*.myraw)|*.myraw|");
	char modulePathName[MAX_PATH];
	GetModuleFileName(NULL,modulePathName,MAX_PATH);
	string moduleClipPathName=ridLastLevelOfPathName(modulePathName);
	dlg.m_ofn.lpstrInitialDir   =  (LPCTSTR)moduleClipPathName.c_str();//����Ĭ��·��
    if(dlg.DoModal() == IDOK){
		//����ļ�·��
		string pathName=dlg.GetPathName();
		//��ͷ�ļ����õ����ļ�����··����
		string fileName_cloud,fileName_photo,fileName_wall,fileName_sky;
		string pathName_cloud,pathName_photo,pathName_wall,pathName_sky;
		if(!main_load_raw_head_file(pathName,
			fileName_cloud,fileName_photo,fileName_wall,fileName_sky,
			pathName_cloud,pathName_photo,pathName_wall,pathName_sky)){
			MessageBox("����ʧ��!");
			Onclose();
			return;
		}
		//ж�ص�ǰ����(���ͷ�ļ����ɹ�����ж�أ�������Щ)
		Onclose();
		//��ʱ����
		addText("loading data...",x_text_midup,y_text_midup);
		//���ظ��ļ�
		{
			
			//��������
	
			if(!textureManager.CreateTexture(pathName_photo)){//0
				MessageBox("����ʧ��!");
				Onclose();
				return;
			}
			if(!textureManager.CreateTexture(pathName_wall)){//1
				MessageBox("����ʧ��!");
				Onclose();
				return;
			}
			if(!textureManager.CreateTexture(pathName_sky)){//2
				MessageBox("����ʧ��!");
				Onclose();
				return;
			}
			//���ص�������
			if(!main_cloud_data_read_in(pathName_cloud.c_str(),vlist,xmin,xmax,ymin,ymax,zmin,zmax)){
				MessageBox("����ʧ��!");
				Onclose();
				return;
			}
			//�����Χ
			cout<<"���Ʒ�Χ(xmin,xmax,ymin,ymax,zmin,zmax)��"<<endl;
			cout<<xmin<<" "<<xmax<<" "<<ymin<<" "<<ymax<<" "<<zmin<<" "<<zmax<<endl;
		}	
		//���¹���״̬	
		workState=WS_RAW;
		//�������
		reSetCam(xmin,xmax,zmin,zmax);
		//����·����Ϣ
		curPathName=pathName;
		picFileName_photo=fileName_photo;
		picFileName_wall=fileName_wall;
		picFileName_sky=fileName_sky;
		//���´��ڱ���
		this->SetWindowText((progName+" | "+curPathName).c_str());
		//�ػ�
		this->Invalidate(false);
		//���²˵������״̬
		updateMenuItemEnableState();

	
	}
}
void CmainDlg::reSetCam(float xmin,float xmax,float zmin,float zmax)
//���ݳ�����Χ�������
{
	//��������Ƕ�
	A=A0;
	dA=10*pi/180;
	B=B0;
	dB=10*pi/180;
	//�������������ԭ��ľ���
	float dmax=max(max((xmax-xmin)/2,(ymax-ymin)/2),(zmax-zmin)/2);//������Χ��ά���ľ����ֵ
	float k=2;//ȡ���������ԭ��ľ���Ϊdmax��k��
	dis_cam_to_world_origin=k*dmax;
	//����camtg
	init4(camtg,(xmin+xmax)/2,0,(zmin+zmax)/2,1);

}
void CmainDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	draw();
	
    SwapBuffers(hdc);
	
	// Do not call CDialog::OnPaint() for painting messages

}
void CmainDlg::addText(const char* text, float x, float y,bool bkDim)
//����Ļ��׷������
//bkDimָʾ�Ƿ񱳾��䰵
{
	CPaintDC dc(this);
	
	SwapBuffers(hdc);//�Ƚ������壬ʹ��ǰ�����Ϊ�󱳻���
	//��������
	{
		bool isLightOn=glIsEnabled(GL_LIGHTING);
		bool isTexOn=glIsEnabled(GL_TEXTURE_2D);
		bool isFogOn=glIsEnabled(GL_FOG);
		bool isDeepTestOn=glIsEnabled(GL_DEPTH_TEST);
		if(isLightOn)glDisable(GL_LIGHTING);
		if(isTexOn)glDisable(GL_TEXTURE_2D);
		if(isFogOn)glDisable(GL_FOG);
		if(isDeepTestOn)glDisable(GL_DEPTH_TEST);
		//����2dģʽ
		Begin2D(SCREEN_COOR);//ʹ����Ļ����
		//����͸������
		if(bkDim){
			//�������
			bool isBlendOn=glIsEnabled(GL_BLEND);
			if(isBlendOn){
				//�޶���
			}else{
				glEnable(GL_BLEND);
			}
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			//������ɫ��͸����
			glColor4f(0,0,0,0.5);
			//����ӿڳߴ�
			GLint viewport[4];
			glGetIntegerv(GL_VIEWPORT,viewport);
			int width=viewport[2];
			int height=viewport[3];
			//������
			glBegin(GL_QUADS);
			glVertex2f(0,0);
			glVertex2f(0,height);
			glVertex2f(width,height);
			glVertex2f(width,0);
			glEnd();
			//�ָ���ɫ
			glColor4f(1,1,1,1);
			//���û��
			if(isBlendOn){
				//�޶���
			}else{
				glDisable(GL_BLEND);
			}
				
		}
		//������
		{
			Text(text,x,y);
		}
		//�˳�2dģʽ
		End2D();
		

		
		if(isLightOn)glEnable(GL_LIGHTING);
		if(isTexOn)glEnable(GL_TEXTURE_2D);
		if(isFogOn)glEnable(GL_FOG);
		if(isDeepTestOn)glEnable(GL_DEPTH_TEST);

	}

	
    SwapBuffers(hdc);//���󱳻��廻��ǰ��

}

BOOL CmainDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	
		if (pMsg->message == WM_KEYDOWN)
	{
		UINT nChar = (UINT)pMsg->wParam;
		//���Լ�ֵ
	//	CString str;str.Format("%d",nChar);MessageBox(str);
		switch (nChar)
		{
		case 86://v
			if(workState==WS_DONE||workState==WS_MODEL){
				viewMode++;
				if(viewMode>VM_WANDER)viewMode=VM_OUTLINE;
				if(viewMode==VM_WANDER){
					this->SetTimer( 1,100,NULL);//ʱ��
				}else{
					this->KillTimer(1);
				}
				
				this->Invalidate(false);
			}
			break;
		case 68://d
		
			break;
		case 32://space
		
			break;
		case 90://z
		
			break;
		case 88://x
		//	flg=!flg;
		//	this->Invalidate(false);
			break;
		case 38://��
			upKeyDown=true;
			break;
		case 40://��
			dnKeyDown=true;
			break;
		case 37://��
			ltKeyDown=true;
			break;
		case 39://��
			rtKeyDown=true;
			break;
		case 71://g
		//	showGround=!showGround;
		//	this->Invalidate(false);
			break;
		case 70://f
			break;
		}
	}
	//ע�⣬�˴���Ϊ����if��������if-elseif
	if(pMsg->message == WM_KEYUP){
		UINT nChar = (UINT)pMsg->wParam;
		switch (nChar)
		{
		case 68://d
			break;
		case 82://r
			break;	
		case 38://��
			upKeyDown=false;
			break;
		case 40://��
			dnKeyDown=false;
			break;
		case 37://��
			ltKeyDown=false;
			break;
		case 39://��
			rtKeyDown=false;
			break;


		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CmainDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	leftDown=true;
	mosx=point.x;
	mosy=point.y;
	mosxf=point.x;
	mosyf=point.y;
	
	
	CDialog::OnLButtonDown(nFlags, point);
}

void CmainDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	leftDown=false;
	mosMovedWhileLeftDown=false;

	
	CDialog::OnLButtonUp(nFlags, point);
}

BOOL CmainDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	// TODO: Add your message handler code here and/or call default
	if(workState==WS_RAW||workState==WS_DONE||workState==WS_MODEL){
		if(zDelta>0){
			dis_cam_to_world_origin-=10;
			//������С����
			if(dis_cam_to_world_origin<min_dis_cam_to_world_origin){
				dis_cam_to_world_origin=min_dis_cam_to_world_origin;
			}
			this->Invalidate (false);
		}else if(zDelta<0){
			dis_cam_to_world_origin+=10;
			//����������
			if(dis_cam_to_world_origin>max_dis_cam_to_world_origin){
				dis_cam_to_world_origin=max_dis_cam_to_world_origin;
			}
			this->Invalidate (false);
		}
	}
	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}

void CmainDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(workState==WS_RAW||workState==WS_DONE||workState==WS_MODEL){
		if(leftDown){
			mosxf=mosx;
			mosyf=mosy;
			mosx=point.x;
			mosy=point.y;
			A-=(mosx-mosxf)*0.01;
			B+=(mosy-mosyf)*0.01;
			//��B�����ƣ���ֹ��ת
			const float maxB=pi/2-0.01*pi/180;
			const float minB=-maxB;
			if(B>maxB)B=maxB;
			else if(B<minB)B=minB;
			this->Invalidate (false);//�ػ�
		}
	}
	CDialog::OnMouseMove(nFlags, point);
}

void CmainDlg::Onrun() 
{
	// TODO: Add your command handler code here
	if(workState==WS_RAW){
		Onseting();
		addText("in processing...",x_text_midup,y_text_midup);
		run();
		workState=WS_DONE;
		viewMode=VM_OUTLINE;
		this->Invalidate(false);
		//���²˵������״̬
		updateMenuItemEnableState();
	}
	
}

void CmainDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default

	if((workState==WS_DONE||workState==WS_MODEL)&&viewMode==VM_WANDER){
		//�ƶ�
		{
			if(upKeyDown){
				me.movloc(0,0,-6);
			}
			if(dnKeyDown){
				me.movloc(0,0,5);
			}
			//�ƶ���λ��
			float pos[4];
			me.toWorld(0,0,0,1,pos);
			//�ж��Ƿ����
			{
				if(pos[X]<xmin||pos[X]>xmax||pos[Z]<zmin||pos[Z]>zmax){//����
					//�ƻص�����
					const float margin=1;
					pos[X]=min(max(pos[X],xmin+margin),xmax-margin);
					pos[Z]=min(max(pos[Z],zmin+margin),zmax-margin);
					//��me��λ����Ϊpos
					me.setloc(pos[X],pos[Y],pos[Z]);

					
				}
				

			}
			//��me���ڵ�����
			{
				bool outOfRange=false;//�Ƿ�Խ��
				Cgroundgrid&GroundGrid=groundBlock.GroundGrid;
				float H;//me���ڴ�����߶�
				//��pos��GroundGrid���ĸ���
				//���������������
				float zmin_g=GroundGrid.zmin;
				float xmin_g=GroundGrid.xmin;
				float zstepLen_g=GroundGrid.zstepLen;
				float xstepLen_g=GroundGrid.xstepLen;
				int m_mat_g=GroundGrid.m_mat;
				int n_mat_g=GroundGrid.n_mat;
				//��pos���ĸ���
				int I,J;//pos����(I,J)��
				I=(pos[Z]-zmin_g)/zstepLen_g;
				J=(pos[X]-xmin_g)/xstepLen_g;
				if(I<0||I>=m_mat_g)outOfRange=true;
				if(J<0||J>=n_mat_g)outOfRange=true;
				if(!outOfRange){//���û����
					H=GroundGrid.mat[I][J].h;
					//��߶�ƽ����
					float dH=H-pos[Y];
					//��ƽ���������ƶ�
					me.movloc(0,dH,0);
				}
			}
		}
		//��ת
		{
			if(ltKeyDown){
				me.rotloc(0,1,0,8);
				
			}
			if(rtKeyDown){
				me.rotloc(0,1,0,-8);
				
			}
		}
		this->Invalidate(false);
		
	}else{
		KillTimer(1);
	}
	CDialog::OnTimer(nIDEvent);
}

void CmainDlg::Onclose() //�ر��ļ������ǹرմ��ڣ�
{
	// TODO: Add your command handler code here
	workState=WS_EMPTY;
	singleStepMode=false;
	stage=0;
	unload();
	this->SetWindowText((progName+curPathName).c_str());
	this->OnPaint();//�����������ػ�
	//���²˵������״̬
	updateMenuItemEnableState();

	
	

	
}
void CmainDlg::unload(){//ж������
	//����·������ͼƬ�ļ����ʹ��ڱ���
	{
		curPathName="";
		picFileName_photo="";
		picFileName_wall="";
		picFileName_sky="";
	}
	//���ö���
	{
		textureManager.destroy();
		vlist.resize(0);
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			cloudList[i].destroy();	
		}
		cloudList.resize(0);
		qTree.destroy();
		groundBlock.destroy();
		me.reset();
	}
	//����ȫ�ֲ���
	{
		xmin=xmax=ymin=ymax=zmin=zmax=0;
		offsetus=0;
		offsetut=0;
		beginTime=0;
		endTime=0;
	}
	
	
	
	
}

void CmainDlg::Onabout() 
{
	// TODO: Add your command handler code here
	string str;
	str+="    makeArc 5.5     \r\n";
    str+="    by wantnon      \r\n";
	str+="     2012-4-21      \r\n";
	MessageBox(str.c_str(),"���� makeArc");
}

void CmainDlg::Onsave() 
{
	// TODO: Add your command handler code here
	if(workState==WS_DONE){
		//��curPathNameָ����ļ�����Ŀ¼�н�һ��model�ļ��У���ģ�ͱ�������
		//��curPathNameָ����ļ����ڵ�Ŀ¼
		string pathName;//curPathNameָ����ļ�����Ŀ¼����pathName��\\������
		pathName=ridLastLevelOfPathName(curPathName);//ȥ��curPathName���һ�����õ�pathName
		//��ʱ����
		{
			addText("saveing model...",x_text_midup,y_text_midup);
		}
		//��ʼ����
		{
			if(!main_save_model(pathName,"model",xmin,xmax,ymin,ymax,zmin,zmax,
				offsetus,offsetut,
				cloudList,
				groundBlock.GroundGrid,
				picFileName_photo,picFileName_wall,picFileName_sky)){
				MessageBox("����ʧ��!");
				this->Invalidate(false);
				return;
			}
			MessageBox("�������!");
			this->Invalidate(false);
		}
	}
	
}

void CmainDlg::Onloadmodel() 
{
	// TODO: Add your command handler code here
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY,"(*.model)|*.model|");
	char modulePathName[MAX_PATH];
	GetModuleFileName(NULL,modulePathName,MAX_PATH);
	string moduleClipPathName=ridLastLevelOfPathName(modulePathName);
	dlg.m_ofn.lpstrInitialDir   =  (LPCTSTR)moduleClipPathName.c_str();//����Ĭ��·��
    if(dlg.DoModal() == IDOK){
		//����ļ�·��
		string pathName=dlg.GetPathName();
		//�ر�����
		Onclose();
		//��ʱ����
		addText("loading model...",x_text_midup,y_text_midup);
		//����ģ��
		bool succ=main_load_model(pathName,xmin,xmax,ymin,ymax,zmin,zmax,offsetus,offsetut,cloudList,groundBlock.GroundGrid,textureManager);
		if(!succ){
			MessageBox("ģ�ͼ���ʧ��!");
			Onclose();
			return;
		}
		//��������
		main_scene_management(cloudList,qTree,groundBlock,xmin,xmax,zmin,zmax,ymin,ymax,textureManager);
		//���¹���״̬�͹۲�ģʽ
		workState=WS_MODEL;
		viewMode=VM_OUTLINE;
		//�������
		reSetCam(xmin,xmax,zmin,zmax);
		//����·����Ϣ
		curPathName=pathName;
		//���´��ڱ���
		this->SetWindowText((progName+" | "+curPathName).c_str());
		//�ػ�
		this->Invalidate(false);
		//���²˵������״̬
		updateMenuItemEnableState();

	}

	
}

void CmainDlg::OnClose() //�رմ���
{
	// TODO: Add your message handler code here and/or call default
	Onclose();

	CDialog::OnClose();
}
void CmainDlg::updateMenuItemEnableState(){//���²˵���Ľ���״̬
	const UINT MF_DG=(MF_DISABLED | MF_GRAYED);
	const UINT MF_E=MF_ENABLED;
	if(!singleStepMode){
		switch(workState){
		case WS_EMPTY:
			this->GetMenu()->EnableMenuItem( ID_open,  MF_E | MF_BYCOMMAND);
			this->GetMenu()->EnableMenuItem( ID_run,  MF_DG | MF_BYCOMMAND);
			this->GetMenu()->EnableMenuItem( ID_close,  MF_DG | MF_BYCOMMAND);
			this->GetMenu()->EnableMenuItem( ID_save,  MF_DG | MF_BYCOMMAND);
			this->GetMenu()->EnableMenuItem( ID_load_model,  MF_E | MF_BYCOMMAND);
			this->GetMenu()->EnableMenuItem( ID_runSingle,  MF_DG | MF_BYCOMMAND);
			this->GetMenu()->EnableMenuItem( ID_seting,  MF_DG | MF_BYCOMMAND);
			break;
		case WS_RAW:
			this->GetMenu()->EnableMenuItem( ID_open,  MF_E | MF_BYCOMMAND);
			this->GetMenu()->EnableMenuItem( ID_run,  MF_E | MF_BYCOMMAND);
			this->GetMenu()->EnableMenuItem( ID_close,  MF_E | MF_BYCOMMAND);
			this->GetMenu()->EnableMenuItem( ID_save,  MF_DG | MF_BYCOMMAND);
			this->GetMenu()->EnableMenuItem( ID_load_model,  MF_E | MF_BYCOMMAND);
			this->GetMenu()->EnableMenuItem( ID_runSingle,  MF_E | MF_BYCOMMAND);
			this->GetMenu()->EnableMenuItem( ID_seting,  MF_E | MF_BYCOMMAND);
			break;
		case WS_DONE:
			this->GetMenu()->EnableMenuItem( ID_open,  MF_E | MF_BYCOMMAND);
			this->GetMenu()->EnableMenuItem( ID_run,  MF_DG | MF_BYCOMMAND);
			this->GetMenu()->EnableMenuItem( ID_close,  MF_E | MF_BYCOMMAND);
			this->GetMenu()->EnableMenuItem( ID_save,  MF_E | MF_BYCOMMAND);
			this->GetMenu()->EnableMenuItem( ID_load_model,  MF_E | MF_BYCOMMAND);
			this->GetMenu()->EnableMenuItem( ID_runSingle,  MF_DG | MF_BYCOMMAND);
			this->GetMenu()->EnableMenuItem( ID_seting,  MF_DG | MF_BYCOMMAND);
			break;
		case WS_MODEL:
			this->GetMenu()->EnableMenuItem( ID_open,  MF_E | MF_BYCOMMAND);
			this->GetMenu()->EnableMenuItem( ID_run,  MF_DG | MF_BYCOMMAND);
			this->GetMenu()->EnableMenuItem( ID_close,  MF_E | MF_BYCOMMAND);
			this->GetMenu()->EnableMenuItem( ID_save,  MF_DG | MF_BYCOMMAND);
			this->GetMenu()->EnableMenuItem( ID_load_model,  MF_E | MF_BYCOMMAND);
			this->GetMenu()->EnableMenuItem( ID_runSingle,  MF_DG | MF_BYCOMMAND);
			this->GetMenu()->EnableMenuItem( ID_seting,  MF_DG | MF_BYCOMMAND);
			break;
		default:
			cout<<"error ����״̬δ����!"<<endl;
			break;
			
		}
	}else{//singleStepMode
		this->GetMenu()->EnableMenuItem( ID_open,  MF_E | MF_BYCOMMAND);
		this->GetMenu()->EnableMenuItem( ID_run,  MF_DG | MF_BYCOMMAND);
		this->GetMenu()->EnableMenuItem( ID_close,  MF_E | MF_BYCOMMAND);
		this->GetMenu()->EnableMenuItem( ID_save,  MF_DG | MF_BYCOMMAND);
		this->GetMenu()->EnableMenuItem( ID_load_model,  MF_E | MF_BYCOMMAND);
		this->GetMenu()->EnableMenuItem( ID_runSingle,  MF_E | MF_BYCOMMAND);
		this->GetMenu()->EnableMenuItem( ID_seting,  MF_DG | MF_BYCOMMAND);

	
	
	}

}

void CmainDlg::Onseting() 
{
	// TODO: Add your command handler code here
	if(setingDlg.DoModal()==IDOK){
		//���ignoreRidTree_baseOnStepMark��ignoreRidTree_baseOnCornerMark
		switch(setingDlg.m_thOfRad123){
		case 0:
			ignoreRidTree_baseOnStepMark=false;
			ignoreRidTree_baseOnCornerMark=false;
			break;
		case 1:
			ignoreRidTree_baseOnStepMark=true;
			ignoreRidTree_baseOnCornerMark=false;
			break;
		case 2:
			ignoreRidTree_baseOnStepMark=true;
			ignoreRidTree_baseOnCornerMark=true;
			break;
		default:
			cout<<"error: setingDlg.m_thOfRad123��Чֵ!"<<endl;
			return;
			break;
		}
		//���doRegulation
		doRegulation=setingDlg.m_check1;
		if(doRegulation){
			//���regulationDegree
			switch(setingDlg.m_thOfRad678){
			case 0:
				regulationDegree=RGL_STRONG_WEAK;
				break;
			case 1:
				regulationDegree=RGL_STRONG;
				break;
			case 2:
				regulationDegree=RGL_WEAK;
				break;
			default:
				cout<<"error: setingDlg.m_thOfRad678��Чֵ!"<<endl;
			}
		}
		//���useRealRoof
		switch(setingDlg.m_thOfRad45){
		case 0:
			useRealRoof=false;
			break;
		case 1:
			useRealRoof=true;
			break;
		default:
			cout<<"error: setingDlg.m_thOfRad45��Чֵ!"<<endl;
		}
	
	}
}
