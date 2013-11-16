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
#pragma comment(linker,"/STACK:0x800000") //目前栈保留8MB，如果点云分割（区域生长）失败，此值应调大
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


//------------------------------全局变量
//-----------显示开关
bool showWarning=false;
bool flg=false;
bool showGround=true;//是否显示地面
bool showSmooth=false;//是否显示平滑结果
bool saveSmooth=false;//是否保存平滑结果
bool showOutlineNorm=false;//是否显示轮廓法向量
bool showOutlineNorm_smooth=false;//是否显示平滑轮廓的法向量
bool showKeyPoint=false;//是否显示关键点
//是否忽略基于阶跃标记的去树过程
extern const bool ignoreRidTree_baseOnStepMark_default=false;
bool ignoreRidTree_baseOnStepMark=ignoreRidTree_baseOnStepMark_default;
//是否忽略基于角点标记的去树过程
extern const bool ignoreRidTree_baseOnCornerMark_default=false;
bool ignoreRidTree_baseOnCornerMark=ignoreRidTree_baseOnCornerMark_default;
//是否进行规则化
extern const bool doRegulation_default=true;
bool doRegulation=doRegulation_default;
//是否进行部分规则化
#define RGL_STRONG_WEAK 0//下部规则化强上部规则化弱，以maxHOfTree的分界
#define RGL_STRONG 1
#define RGL_WEAK 2
extern const int regulationDegree_default=RGL_STRONG_WEAK;
int regulationDegree=regulationDegree_default;
//是否使用真实屋顶
extern const bool useRealRoof_default=false;
bool useRealRoof=useRealRoof_default;
//-------最大树高
float maxHOfTree=28;//距地面高度大于此高度的云团就不用去植被点了
//-------最小不强力规则化高度
float minHNotRegulationStrong=maxHOfTree+4;//在regulationDegree=RGL_STRONG_WEAK时，高于minHNotRegulationStrong的云团不使用强力规则化，而使用弱规则化
//-------设置对话框
#include "setingDlg.h"
//设置对话框必须为全局量，因为它必须有记忆功能
CsetingDlg setingDlg;
//-------单步显示
bool singleStepMode=false;//是否为单步显示模式
int stage=0;//到哪个阶段
//-------宏和常数
const float inf=1000000000000;
const float pi=3.1415926;
//2D坐标风格
#define SCREEN_COOR 0
#define MATH_COOR 1
//坐标分量
#define X 0
#define Y 1
#define Z 2
#define W 3

//鼠标
int mosx=0;//鼠标x坐标
int mosy=0;//鼠标y坐标
int mosxf=0;//前一时刻鼠标x坐标
int mosyf=0;//前一时刻鼠标y坐标
bool leftDown=false;//是否鼠标左键按下
bool mosMovedWhileLeftDown=false;//是否在鼠标左键按下的情况下鼠标发生了移动
//键盘
bool upKeyDown=false;
bool dnKeyDown=false;
bool ltKeyDown=false;
bool rtKeyDown=false;
//窗口尺寸
int winwidth=768;//窗口宽度
int winheight=512;//窗口高度
//工作状态
#define WS_EMPTY 0//空
#define WS_RAW 1//原料
#define WS_DONE 2//建模完成
#define WS_MODEL 3//模型查看
int workState=WS_EMPTY;
//观察模式
#define VM_OUTLINE 0//轮廓
#define VM_OVERLOOK 1//鸟瞰
#define VM_WANDER 2//漫游
int viewMode=VM_OUTLINE;
//头文件
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

//流程 work flow (WF)
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
//备忘
#include"z_备忘.h"
#include"z_模型文件格式.h"
#include"z_整理原则.h"
#include"z_参数.h"
//光源参数
float light_pos[4]={500,100,-100};

//视角相关
float campos[4];//相机位置
float camtg[4];//目标点
const float A0=0;
const float B0=0;
float A=A0;//水平旋转角(与Z轴夹角，由Z转向X为正角)
float dA=10*pi/180;//A的增量
float B=B0;//竖直旋转角(与XZ平面夹角)
float dB=10*pi/180;//B的增量
float dis_cam_to_world_origin;//相机到世界原点的距离
const float max_dis_cam_to_world_origin=2000;//相机到世界原点的最大距离
const float min_dis_cam_to_world_origin=1;//相机到世界原点的最小距离


//纹理
CTextureManager textureManager;

//核心数据
myvector<Cv4> vlist;//点列表
float xmin,xmax,ymin,ymax,zmin,zmax;//vlist范围（这个范围必须是依据点原始点云精确求出的，否则会影响航片贴图效果）
float offsetus=0;//-0.003;//贴图横坐标修正值
float offsetut=0;//0.005;//贴图纵坐标修正值
myvector<Ccloud> cloudList;//云团列表（每处理一个cloud前，必须先判断其是否已removed，绝不能因为前面作了紧缩而省略此判断）
CqTree qTree;//四叉树
CgroundBlock groundBlock;//地形块四叉树
Cme me;//观察者

//起止时间
long beginTime=0;
long endTime=0;
//颜色表
float colorTable[256][3];
//当前文件路径
string curPathName;//当前打开的任务的文件路径
string picFileName_photo;//航空照片文件名
string picFileName_wall;//墙壁图片文件名
string picFileName_sky;//天空图片文件名
const string progName="makeArc";//程序名
//显示文字的位置
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
	
	
	//开始计时
	{
		cout<<"开始处理...  "<<endl;
		beginTime =clock();//获得开始时间，单位为毫秒
	}
	//去墙壁点
	cout<<"去墙壁点"<<endl;
	{
		main_remove_wall_point(vlist,xmin,xmax,ymin,ymax,zmin,zmax);
	}
	//标记阶跃点
	{
		if(ignoreRidTree_baseOnStepMark){
			cout<<"标记阶跃点（跳过）"<<endl;
		}else{
			cout<<"标记阶跃点"<<endl;
			main_mark_step_point(vlist,xmin,xmax,ymin,ymax,zmin,zmax);
		}
	
	}
	//生成地面网格并去地面点
	cout<<"去地面点及生成地面"<<endl;
	{
		main_make_ground_grid_and_remove_ground_point(vlist,xmin,xmax,ymin,ymax,zmin,zmax,groundBlock.GroundGrid);
	
	}
	//为GroundGrid计算纹理坐标
	cout<<"计算地面纹理坐标"<<endl;
	{
		main_cal_texCoord_for_groundgrid(groundBlock.GroundGrid,offsetus,offsetut);
	}
	//点云分割
	cout<<"点云分割"<<endl;
	{
		main_cloud_segmentation(vlist,xmin,xmax,ymin,ymax,zmin,zmax,cloudList,ignoreRidTree_baseOnStepMark);
	}
	//删除vlist
	cout<<"删除vlist"<<endl;
	{
		vlist.swap(myvector<Cv4>());
	}
	//计算各云团平均高度
	cout<<"计算avrgH"<<endl;
	{
		main_cal_avrgH_for_each_cloud(cloudList);
	}
	//计算各云团的近似minH
	cout<<"粗略计算minH"<<endl;
	{
		main_roughly_cal_minH_for_each_cloud(cloudList,groundBlock.GroundGrid);
	}
	//去植被点

	{
		if(ignoreRidTree_baseOnStepMark){
			cout<<"去植被点（跳过）"<<endl;
		}else{
			cout<<"去植被点"<<endl;
			main_remove_vegetation_point(cloudList);
		}
	
	}
	//重新计算各云团平均高度
	cout<<"重新计算avrgH"<<endl;
	{
		main_cal_avrgH_for_each_cloud(cloudList);
	}
	//点云沉降
	cout<<"点云沉降"<<endl;
	{
		main_cloud_settlement(cloudList);
	
	}
	//轮廓提取
	cout<<"轮廓提取"<<endl;
	{
		main_outline_extraction(cloudList);
	
	}
	//计算各云团的minH，并删除过低的云团
	cout<<"计算minH并删除过低云团"<<endl;
	{
		main_cal_minH_and_rid_cloud_too_low(cloudList,groundBlock.GroundGrid);
	}
	//计算各云团的bottomH
	cout<<"计算bottomH"<<endl;
	{
		main_cal_bottomH_for_each_cloud(cloudList);
	}
	//求最小包围矩形
	cout<<"求最小包围矩形"<<endl;
	{
		main_cal_minRect_for_each_cloud(cloudList);
	
	}
	//轮廓平滑
	cout<<"轮廓平滑"<<endl;
	{
		main_outline_smooth(cloudList);
	
	}
	//轮廓规则化
	cout<<"轮廓规则化"<<endl;
	{
		main_outline_regulation(cloudList,doRegulation);
	}
	//建屋顶和墙壁面
	cout<<"建屋顶和墙壁"<<endl;
	{
		main_make_roof_and_wall(cloudList,groundBlock.GroundGrid,ignoreRidTree_baseOnCornerMark,useRealRoof);
	}
	//生成屋顶纹理坐标
	cout<<"计算屋顶纹理坐标"<<endl;
	{
		main_cal_texCoord_for_each_roof(cloudList,xmin,xmax,zmin,zmax,offsetus,offsetut);

	}
	//场景管理
	cout<<"场景管理"<<endl;
	{
		main_scene_management(cloudList,qTree,groundBlock,xmin,xmax,zmin,zmax,ymin,ymax,textureManager);
	}


	//结束计时并计算处理用时
	{
		endTime=clock();//获得结束时间
		cout<<"完毕!  ";
		cout<<"处理用时:"<<endTime-beginTime<<"ms"<<endl;
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
	int nStepExcuted=0;//本次执行了几步
	switch(stage){

	case 0://去墙壁点
		{
			
			cout<<"去墙壁点...";
			main_remove_wall_point(vlist,xmin,xmax,ymin,ymax,zmin,zmax);
			cout<<"完成"<<endl;
			//
			nStepExcuted=1;
			
		}
		break;
	case 1://标记阶跃点
		{	
			if(ignoreRidTree_baseOnStepMark){
				cout<<"标记阶跃点（跳过）"<<endl;
			}else{
				cout<<"标记阶跃点...";
				main_mark_step_point(vlist,xmin,xmax,ymin,ymax,zmin,zmax);	
				cout<<"完成"<<endl;	
			}
			//
			nStepExcuted=1;
		}
		break;
	case 2://生成地面网格并去地面点 && 为GroundGrid计算纹理坐标
		{
			
			cout<<"去地面点及生成地面...";
			main_make_ground_grid_and_remove_ground_point(vlist,xmin,xmax,ymin,ymax,zmin,zmax,groundBlock.GroundGrid);
			cout<<"完成"<<endl;
			cout<<"计算地面纹理坐标...";
			main_cal_texCoord_for_groundgrid(groundBlock.GroundGrid,offsetus,offsetut);
			cout<<"完成"<<endl;
			//
			nStepExcuted=2;
		}
		break;
	case 3:
		{
			cout<<"error! 不应到此分支"<<endl;
			return;
		}
		break;
	case 4://点云分割 && 删除vlist && 计算各云团平均高度 && 计算各云团的近似minH
		{
			cout<<"点云分割...";
			main_cloud_segmentation(vlist,xmin,xmax,ymin,ymax,zmin,zmax,cloudList,ignoreRidTree_baseOnStepMark);
			cout<<"完成"<<endl;
			cout<<"删除vlist...";
			vlist.swap(myvector<Cv4>());
			cout<<"完成"<<endl;
			cout<<"计算avrgH...";
			main_cal_avrgH_for_each_cloud(cloudList);
			cout<<"完成"<<endl;
			cout<<"粗略计算minH...";
			main_roughly_cal_minH_for_each_cloud(cloudList,groundBlock.GroundGrid);
			cout<<"完成"<<endl;
			//
			nStepExcuted=4;
		}
		break;
	case 5:
	case 6:
	case 7:
		{
			cout<<"error! 不应到此分支"<<endl;
			return;
		}
		break;
	case 8://去植被点 && 重新计算各云团平均高度
		{
			if(ignoreRidTree_baseOnStepMark){
				cout<<"去植被点（跳过）"<<endl;
			}else{
				cout<<"去植被点...";
				main_remove_vegetation_point(cloudList);
				cout<<"完成"<<endl;	
			}
			cout<<"重新计算avrgH...";
			main_cal_avrgH_for_each_cloud(cloudList);
			cout<<"完成"<<endl;
			//
			nStepExcuted=2;
		}
		break;
	case 9:
		{
			cout<<"error! 不应到此分支"<<endl;
			return;
		}
		break;
	case 10://点云沉降
		{
			cout<<"点云沉降...";
			main_cloud_settlement(cloudList);
			cout<<"完成"<<endl;
			//
			nStepExcuted=1;
		}
		break;
	case 11://轮廓提取
		{
			cout<<"轮廓提取...";
			main_outline_extraction(cloudList);
			cout<<"完成"<<endl;
			//
			nStepExcuted=1;

		}
		break;
	case 12://计算各云团的minH,并删除过低的云团 && 计算各云团的bottomH && 求最小包围矩形
		{
			cout<<"计算minH并删除过低云团...";
			main_cal_minH_and_rid_cloud_too_low(cloudList,groundBlock.GroundGrid);
			cout<<"完成"<<endl;
			cout<<"计算bottomH...";
			main_cal_bottomH_for_each_cloud(cloudList);
			cout<<"完成"<<endl;
			cout<<"求最小包围矩形...";
			main_cal_minRect_for_each_cloud(cloudList);
			cout<<"完成"<<endl;
			//
			nStepExcuted=3;
		}
		break;
	case 13:
	case 14:
		{
			cout<<"error! 不应到此分支"<<endl;
			return;
		}
		break;
	case 15://轮廓平滑
		{
			cout<<"轮廓平滑...";
			main_outline_smooth(cloudList);
			cout<<"完成"<<endl;
			//
			nStepExcuted=1;
			
		}
		break;
	case 16://轮廓规则化
		{
			cout<<"轮廓规则化...";
			main_outline_regulation(cloudList,doRegulation);
			cout<<"完成"<<endl;
			//
			nStepExcuted=1;
		}
		break;
	case 17://建屋顶和墙壁面 && 生成屋顶纹理坐标 && 场景管理
		{
			cout<<"建屋顶和墙壁...";
			cout<<endl;
			main_make_roof_and_wall(cloudList,groundBlock.GroundGrid,ignoreRidTree_baseOnCornerMark,useRealRoof);
			cout<<"完成"<<endl;
			cout<<"计算屋顶纹理坐标...";
			main_cal_texCoord_for_each_roof(cloudList,xmin,xmax,zmin,zmax,offsetus,offsetut);
			cout<<"完成"<<endl;
			cout<<"场景管理...";
			main_scene_management(cloudList,qTree,groundBlock,xmin,xmax,zmin,zmax,ymin,ymax,textureManager);
			cout<<"完成"<<endl;
			//---------此时模型已建成
			//单步模式复位
			stage=0;
			singleStepMode=false;
			//将状态
			workState=WS_DONE;
			viewMode=VM_OVERLOOK;
			this->Invalidate(false);
			//更新菜单项禁用状态
			updateMenuItemEnableState();
			//返回
			return;

		}
		break;
	case 18:
	case 19:
		{
			cout<<"error! 不应到此分支"<<endl;
			return;
		}
		break;
	default:
		{
			cout<<"error! 不应到此分支"<<endl;
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
	//设置观察角度
	glMatrixMode(GL_MODELVIEW);//切换到 模型-视图 模式
	glLoadIdentity();//归一化
	//计算相机位置
	if(singleStepMode==true
		||workState==WS_RAW
		||(workState==WS_DONE&&(viewMode==VM_OUTLINE||viewMode==VM_OVERLOOK))
		||(workState==WS_MODEL&&(viewMode==VM_OUTLINE||viewMode==VM_OVERLOOK))
		){//浏览模式
		campos[X]=dis_cam_to_world_origin*cos(B)*sin(A);
		campos[Y]=dis_cam_to_world_origin*sin(B);
		campos[Z]=dis_cam_to_world_origin*cos(B)*cos(A);
		init4(camtg,0,0,0,1);
	}else if((workState==WS_DONE&&viewMode==VM_WANDER)
		||(workState==WS_MODEL&&viewMode==VM_WANDER)
		){//漫游模式
		me.toWorld(0,10,-15,1,camtg);
		float campos_inMe[4]={0,10,6,1};//主角坐标系下相机的位置
		me.toWorld(campos_inMe[X],
			campos_inMe[Y],
			campos_inMe[Z],
			campos_inMe[W],campos);//得到campos
	}
	gluLookAt(campos[X],campos[Y],campos[Z],//相机位置
		camtg[X],camtg[Y],camtg[Z],//关注点
		0,1,0);//相机向上方向
	glLightfv(GL_LIGHT0,GL_POSITION,light_pos);
	groundBlock.updateCampos(campos);
	//--------------------------根据workState不同状态显示不同的画面
	if(!singleStepMode){
		if(workState==WS_EMPTY){
			
		}else if(workState==WS_RAW){
			//画点云
			{
				int nv=(int)vlist.size();
				glBegin(GL_POINTS);
				for(int i=0;i<nv;i++){
					float Hrate=(vlist[i].v[Y]-ymin)/(ymax-ymin);
					float r,g,b;
					//	//简单计算颜色值
					//	{
					//		r=2*Hrate;
					//		g=Hrate<=0.5?4*Hrate:4*(1-Hrate);
					//		b=1-Hrate;
					//  }
					//使用颜色表计算颜色值
					{	
						//查颜色表获得颜色
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
			//在底面衬上航片
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
			//画建成结果
			if(viewMode==VM_OUTLINE){
				glDisable(GL_FOG);
				//画IDoutlineList各元素的polygenList_smoothRes
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
							//画polygen
							int nv=polygen.vlist.size();
							for(int k=0;k<nv;k++){
								Cv4Ex&v=polygen.vlist[k];
								float *v1=polygen.vlist[k].v;
								float *v2=polygen.vlist[(k+1)%nv].v;
								//画边
								glColor3f(1,1,1);
								glBegin(GL_LINES);
								glVertex3f(v1[X],bottomH,v1[Z]);
								glVertex3f(v2[X],bottomH,v2[Z]);
								glEnd();
								glColor3f(1,1,1);
								//画右法向量
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
				//画各polygenList
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
							//画polygen
							int nv=polygen.vlist.size();
							for(int k=0;k<nv;k++){
								Cv4Ex&v=polygen.vlist[k];
								float *v1=polygen.vlist[k].v;
								float *v2=polygen.vlist[(k+1)%nv].v;
								//画边
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
								//画isKeyPoint
								if(showKeyPoint){
									if(polygen.vlist[k].isKeyPoint){
										glPointSize(3);
										glBegin(GL_POINTS);
										glVertex3fv(v1);
										glEnd();
										glPointSize(1);	
									}
								}
								//画右法向量
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
				//----画天空盒
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, textureManager.getTex(2));
				double density_store;
				glGetDoublev(GL_FOG_DENSITY,&density_store);//获得当前雾浓度
				glFogf(GL_FOG_DENSITY, density_store*0.2);//减低雾浓度
				ball_seeback(12,//瓣数不能太低，否则会出缺口
					0,0,0,2*max((xmax-xmin)/2,(zmax-zmin)/2),campos,0.5);
				glFogf(GL_FOG_DENSITY, density_store);//恢复雾浓度
				glDisable(GL_TEXTURE_2D);
				glDisable(GL_FOG);
			}
		}else{
			cout<<"状态未定义!"<<endl;
			return;
		}
		//------------------------------显示文字
		const float h=20;//行高
		//左上角起始坐标
		const float x=10;
		const float y=50;
		//显示文字
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
			cout<<"状态未定义!"<<endl;
			return;
		}
		End2D();
		glEnable(GL_LIGHTING);
	}else{//singleStepMode
		switch(stage){
		case 1://显示去墙壁点结果
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
		case 2://显示标记阶跃点结果
			{
				int nv=(int)vlist.size();
				
				for(int i=0;i<nv;i++){
					Cv4&v=vlist[i];
					if(v.isEdgePoint){
						glColor3f(1,0,0);
					}else{
						glColor3f(0,0,1);
					}
					glBegin(GL_POINTS);//如果将glBegin放在循环外，则结果会不一样!!!!!!!
					glVertex3fv(vlist[i].v);
					glEnd();
				}
				
				glColor3f(1,1,1);
			}
			break;
		case 3://显示生成地面网格并去地面点结果
		case 4://显示为GroundGrid计算纹理坐标结果
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
		case 5://显示点云分割结果
		case 6://显示删除vlist结果
		case 7://显示计算各云团平均高度结果
		case 8://显示计算各云团的近似minH结果
			{
				//画各云团
				
				int ncloud=(int)cloudList.size();
				for(int i=0;i<ncloud;i++){
					Ccloud&cloud=cloudList[i];
					if(cloud.removed)continue;
					//画cloud
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
		case 9://显示去植被点结果
		case 10://显示重新计算各云团平均高度结果
		case 11://显示点云沉降结果
			{
				//画各云团
				
				int ncloud=(int)cloudList.size();
				for(int i=0;i<ncloud;i++){
					Ccloud&cloud=cloudList[i];
					if(cloud.removed)continue;
					//画cloud
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
		case 12://显示轮廓提取结果
		case 13://显示计算各云团的minH，并删除过低的云团结果
		case 14://显示计算各云团的bottomH结果
		case 15://显示求最小包围矩形结果
		case 16://显示轮廓平滑结果
			{
				//画各polygenList
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
						//画polygen
						int nv=polygen.vlist.size();
						for(int k=0;k<nv;k++){
							Cv4Ex&v=polygen.vlist[k];
							float *v1=polygen.vlist[k].v;
							float *v2=polygen.vlist[(k+1)%nv].v;
							//画边
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
		case 17://显示轮廓规则化结果
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
						//画polygen
						int nv=polygen.vlist.size();
						for(int k=0;k<nv;k++){
							Cv4Ex&v=polygen.vlist[k];
							float *v1=polygen.vlist[k].v;
							float *v2=polygen.vlist[(k+1)%nv].v;
							//画边
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
							//画isKeyPoint
							if(showKeyPoint){
								if(polygen.vlist[k].isKeyPoint){
									glPointSize(3);
									glBegin(GL_POINTS);
									glVertex3fv(v1);
									glEnd();
									glPointSize(1);	
								}
							}
							//画右法向量
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
		case 18://显示建屋顶和墙壁面结果
			{
				//画房屋
				int ncloud=(int)cloudList.size();
				for(int j=0;j<ncloud;j++){
					Ccloud&cloud=cloudList[j];
					if(cloud.removed)continue;
					//画屋顶面
					{
						glDisable(GL_LIGHTING);
						cloud.grid.showMesh_rough0(textureManager.getTex(0),false,1,0,0,1);
						glEnable(GL_LIGHTING);
						
					}
					//画墙壁
					{
						
						myvector<Cpolygen>&polygenList=cloud.polygenList;
						myvector<Cpolygen>&polygenList_dn=cloud.polygenList_dn;
						drawWall_polygenList_polygenList(polygenList,polygenList_dn,textureManager.getTex(1),1,false);
						
					}
					
				}
				//画地面
				groundBlock.GroundGrid.showGround(textureManager.getTex(0),false,0,1,0,1);

			}
		case 19://显示生成屋顶纹理坐标结果
			{

				
				
				//画房屋
				int ncloud=(int)cloudList.size();
				for(int j=0;j<ncloud;j++){
					Ccloud&cloud=cloudList[j];
					if(cloud.removed)continue;
					//画屋顶面
					{
						glDisable(GL_LIGHTING);
						if(cloud.isRed)glColor3f(1,0,0);
						cloud.grid.showMesh_rough0(textureManager.getTex(0));
						if(cloud.isRed)glColor3f(1,1,1);
						glEnable(GL_LIGHTING);
						
					}
					//画墙壁
					{
						
						myvector<Cpolygen>&polygenList=cloud.polygenList;
						myvector<Cpolygen>&polygenList_dn=cloud.polygenList_dn;
						drawWall_polygenList_polygenList(polygenList,polygenList_dn,textureManager.getTex(1));
						
					}
					
				}
				//画地面
				groundBlock.GroundGrid.showGround(textureManager.getTex(0));
			}
			break;
		case 20://显示场景管理结果
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


// 设置像素格式函数
int CmainDlg::MySetPixelFormat(HDC hdc)
{
	PIXELFORMATDESCRIPTOR pfd = { 
	    sizeof(PIXELFORMATDESCRIPTOR),    // pfd结构的大小 
	    1,                                // 版本号 
	    PFD_DRAW_TO_WINDOW |              // 支持在窗口中绘图 
	    PFD_SUPPORT_OPENGL |              // 支持 OpenGL 
	    PFD_DOUBLEBUFFER,                 // 双缓存模式 
	    PFD_TYPE_RGBA,                    // RGBA 颜色模式 
	    24,                               // 24 位颜色深度 
	    0, 0, 0, 0, 0, 0,                 // 忽略颜色位 
	    0,                                // 没有非透明度缓存 
	    0,                                // 忽略移位位 
	    0,                                // 无累加缓存 
	    0, 0, 0, 0,                       // 忽略累加位 
	    32,                               // 32 位深度缓存     
	    0,                                // 无模板缓存 
	    0,                                // 无辅助缓存 
	    PFD_MAIN_PLANE,                   // 主层 
	    0,                                // 保留 
	    0, 0, 0                           // 忽略层,可见性和损毁掩模 
	}; 
	
	int  iPixelFormat; 
 
	// 为设备描述表得到最匹配的像素格式 
	if((iPixelFormat = ChoosePixelFormat(hdc, &pfd)) == 0)
	{
		MessageBox("ChoosePixelFormat Failed", NULL, MB_OK);
		return 0;
	}
	 
	// 设置最匹配的像素格式为当前的像素格式 
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

	//设置窗口大小
	this->SetWindowPos(NULL,0,0,winwidth,winheight,SWP_NOMOVE);
	// 设置当前的绘图像素格式
	MySetPixelFormat(::GetDC(m_hWnd));
	// 获得绘图描述表
	hdc = ::GetDC(m_hWnd);
	// 创建渲染描述表
	hglrc = wglCreateContext(hdc);
	// 使绘图描述表为当前调用现程的当前绘图描述表 
	wglMakeCurrent(hdc, hglrc);	
	////-----------------------------------------------------------
	//select clearing(background) color
	glClearColor(0.0,0.0,0.0,0.0);
	//-------------------------------------------光照-------------------------------------------------------------------------- 
	//----选择明暗技术
	//可以用GL_FLAT或GL_SMOOTH
	glShadeModel(GL_SMOOTH);
	//----设置材质
	//可以对不同的面GL_FRONT,GL_BACK,GL_FRONT_AND_BACK设置如下项目：
	//GL_AMBIENT(环境颜色),GL_DIFFUSE(散射颜色),GL_AMBIENT_AND_DIFFUSE,GL_SPECULAR(镜面颜色),GL_SHININESS(镜面指数),
	//GL_EMISSION,GL_COLOR_INDEXES
	//这里只对GL_SPECULAR和GL_SHININESS作了设置，其余用默认值
	GLfloat mat_specular[]={0,0,0,1};
	glMaterialfv(GL_FRONT,GL_SPECULAR,mat_specular);//镜面颜色
	GLfloat mat_shininess[]={50};
	glMaterialfv(GL_FRONT,GL_SHININESS,mat_shininess);//镜面指数
	GLfloat mat_emission[]={0.3,0.3,0.3,0.0};
	glMaterialfv(GL_FRONT,GL_EMISSION,mat_emission);

	//----设置光源LIGHT0
	//共有8个光源LIGHT0~LIGHT7
	//设置项有GL_AMBIENT,GL_DIFFUSE,GL_SPECULAR,GL_POSITION,GL_SPOT_DIRECTION,GL_SPOT_EXPONENT,GL_SPOT_CUTOFF,
	//GL_CONSTANT_ATTENUATION,GL_LINEAR_ATTENUATION,GL_QUADRATIC_ATTENUATION
	//这里只对GL_DIFFUSE和GL_SPECULAR进行设置，其余用默认值
	GLfloat light0_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light0_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light0_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
    glLightfv(GL_LIGHT0,GL_DIFFUSE,light0_diffuse);//漫反射
	glLightfv(GL_LIGHT0,GL_SPECULAR,light0_specular);//镜面光
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
	//----设置光照模型
	//包括GL_LIGHT_MODEL_AMBIENT,GL_LIGHT_MODEL_LOCAL_VIEWR,GL_LIGHT_MODEL_TWO_SIDE,GL_LIGHT_MODEL_COLOR_CONTROL
	//这里只对GL_LIGHT_MODEL_AMBIENT进行一下设置，其余用默认值
	GLfloat lmodel_ambient[]={0.5,0.5,0.5,1.0};
   	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,lmodel_ambient);
	//----设置雾
	glFogi(GL_FOG_MODE,  GL_EXP2);
	float fogColor[4] = {80.0/255,80.0/255,100.0/255,0.5};    
	glFogfv(GL_FOG_COLOR, fogColor);                
	glFogf(GL_FOG_DENSITY, 0.003f);//雾的浓度，用于指数模式中    
	glHint(GL_FOG_HINT,GL_FASTEST);// 设置系统如何计算雾气
	glFogf(GL_FOG_START, 1.0f);// 雾气的开始位置


	

	//-----------------------------------启用-------------------------------
	glEnable(GL_DEPTH_TEST);//开启深度测试
	glEnable(GL_LIGHTING);//开启光照
	glEnable(GL_LIGHT0);//开启LIGHT0
	glEnable(GL_COLOR_MATERIAL);//开启颜色材质模式(使颜色与材质自动混合)
	//----------------------------获得视口大小-------------------------------
	GLint viewport[4];//4个元素依次为x,y,width,height
	glGetIntegerv(GL_VIEWPORT,viewport);
	int width=viewport[2];
	int height=viewport[3];
	
	//----------------------------设置视锥------------------------------------
	glMatrixMode(GL_PROJECTION);//切换到投影模式
	glLoadIdentity();//归一化
	float nearface=0.1;//若设为0则在投影模式下不能正常显示
	float farface=1.5*max_dis_cam_to_world_origin;//远截面
	gluPerspective(45,//视角
		(float)width/height, //窗口横纵比
		nearface,//近截面
		farface);//远截面


	//--------------------------生成颜色表
	makeColorTable(colorTable);

	//--------------------------更新菜单项禁用状态
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
	dlg.m_ofn.lpstrInitialDir   =  (LPCTSTR)moduleClipPathName.c_str();//设置默认路径
    if(dlg.DoModal() == IDOK){
		//获得文件路径
		string pathName=dlg.GetPathName();
		//读头文件，得到各文件名和路路径名
		string fileName_cloud,fileName_photo,fileName_wall,fileName_sky;
		string pathName_cloud,pathName_photo,pathName_wall,pathName_sky;
		if(!main_load_raw_head_file(pathName,
			fileName_cloud,fileName_photo,fileName_wall,fileName_sky,
			pathName_cloud,pathName_photo,pathName_wall,pathName_sky)){
			MessageBox("加载失败!");
			Onclose();
			return;
		}
		//卸载当前任务(如果头文件读成功了再卸载，这样好些)
		Onclose();
		//临时画面
		addText("loading data...",x_text_midup,y_text_midup);
		//加载各文件
		{
			
			//加载纹理
	
			if(!textureManager.CreateTexture(pathName_photo)){//0
				MessageBox("加载失败!");
				Onclose();
				return;
			}
			if(!textureManager.CreateTexture(pathName_wall)){//1
				MessageBox("加载失败!");
				Onclose();
				return;
			}
			if(!textureManager.CreateTexture(pathName_sky)){//2
				MessageBox("加载失败!");
				Onclose();
				return;
			}
			//加载点云数据
			if(!main_cloud_data_read_in(pathName_cloud.c_str(),vlist,xmin,xmax,ymin,ymax,zmin,zmax)){
				MessageBox("加载失败!");
				Onclose();
				return;
			}
			//输出范围
			cout<<"点云范围(xmin,xmax,ymin,ymax,zmin,zmax)："<<endl;
			cout<<xmin<<" "<<xmax<<" "<<ymin<<" "<<ymax<<" "<<zmin<<" "<<zmax<<endl;
		}	
		//更新工作状态	
		workState=WS_RAW;
		//重置相机
		reSetCam(xmin,xmax,zmin,zmax);
		//更新路径信息
		curPathName=pathName;
		picFileName_photo=fileName_photo;
		picFileName_wall=fileName_wall;
		picFileName_sky=fileName_sky;
		//更新窗口标题
		this->SetWindowText((progName+" | "+curPathName).c_str());
		//重画
		this->Invalidate(false);
		//更新菜单项禁用状态
		updateMenuItemEnableState();

	
	}
}
void CmainDlg::reSetCam(float xmin,float xmax,float zmin,float zmax)
//根据场景范围重置相机
{
	//设置相机角度
	A=A0;
	dA=10*pi/180;
	B=B0;
	dB=10*pi/180;
	//设置相机到世界原点的距离
	float dmax=max(max((xmax-xmin)/2,(ymax-ymin)/2),(zmax-zmin)/2);//场景范围各维边心距最大值
	float k=2;//取相机到世界原点的距离为dmax的k倍
	dis_cam_to_world_origin=k*dmax;
	//计算camtg
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
//在屏幕上追加文字
//bkDim指示是否背景变暗
{
	CPaintDC dc(this);
	
	SwapBuffers(hdc);//先交换缓冲，使当前缓冲变为后背缓冲
	//绘制文字
	{
		bool isLightOn=glIsEnabled(GL_LIGHTING);
		bool isTexOn=glIsEnabled(GL_TEXTURE_2D);
		bool isFogOn=glIsEnabled(GL_FOG);
		bool isDeepTestOn=glIsEnabled(GL_DEPTH_TEST);
		if(isLightOn)glDisable(GL_LIGHTING);
		if(isTexOn)glDisable(GL_TEXTURE_2D);
		if(isFogOn)glDisable(GL_FOG);
		if(isDeepTestOn)glDisable(GL_DEPTH_TEST);
		//进入2d模式
		Begin2D(SCREEN_COOR);//使用屏幕坐标
		//画半透明矩形
		if(bkDim){
			//开启混合
			bool isBlendOn=glIsEnabled(GL_BLEND);
			if(isBlendOn){
				//无动作
			}else{
				glEnable(GL_BLEND);
			}
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			//设置颜色和透明度
			glColor4f(0,0,0,0.5);
			//获得视口尺寸
			GLint viewport[4];
			glGetIntegerv(GL_VIEWPORT,viewport);
			int width=viewport[2];
			int height=viewport[3];
			//画矩形
			glBegin(GL_QUADS);
			glVertex2f(0,0);
			glVertex2f(0,height);
			glVertex2f(width,height);
			glVertex2f(width,0);
			glEnd();
			//恢复颜色
			glColor4f(1,1,1,1);
			//禁用混合
			if(isBlendOn){
				//无动作
			}else{
				glDisable(GL_BLEND);
			}
				
		}
		//画文字
		{
			Text(text,x,y);
		}
		//退出2d模式
		End2D();
		

		
		if(isLightOn)glEnable(GL_LIGHTING);
		if(isTexOn)glEnable(GL_TEXTURE_2D);
		if(isFogOn)glEnable(GL_FOG);
		if(isDeepTestOn)glEnable(GL_DEPTH_TEST);

	}

	
    SwapBuffers(hdc);//将后背缓冲换到前面

}

BOOL CmainDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	
		if (pMsg->message == WM_KEYDOWN)
	{
		UINT nChar = (UINT)pMsg->wParam;
		//测试键值
	//	CString str;str.Format("%d",nChar);MessageBox(str);
		switch (nChar)
		{
		case 86://v
			if(workState==WS_DONE||workState==WS_MODEL){
				viewMode++;
				if(viewMode>VM_WANDER)viewMode=VM_OUTLINE;
				if(viewMode==VM_WANDER){
					this->SetTimer( 1,100,NULL);//时钟
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
		case 38://上
			upKeyDown=true;
			break;
		case 40://下
			dnKeyDown=true;
			break;
		case 37://左
			ltKeyDown=true;
			break;
		case 39://右
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
	//注意，此处用为并列if，不能用if-elseif
	if(pMsg->message == WM_KEYUP){
		UINT nChar = (UINT)pMsg->wParam;
		switch (nChar)
		{
		case 68://d
			break;
		case 82://r
			break;	
		case 38://上
			upKeyDown=false;
			break;
		case 40://下
			dnKeyDown=false;
			break;
		case 37://左
			ltKeyDown=false;
			break;
		case 39://右
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
			//限制最小距离
			if(dis_cam_to_world_origin<min_dis_cam_to_world_origin){
				dis_cam_to_world_origin=min_dis_cam_to_world_origin;
			}
			this->Invalidate (false);
		}else if(zDelta<0){
			dis_cam_to_world_origin+=10;
			//限制最大距离
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
			//对B加限制，防止倒转
			const float maxB=pi/2-0.01*pi/180;
			const float minB=-maxB;
			if(B>maxB)B=maxB;
			else if(B<minB)B=minB;
			this->Invalidate (false);//重画
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
		//更新菜单项禁用状态
		updateMenuItemEnableState();
	}
	
}

void CmainDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default

	if((workState==WS_DONE||workState==WS_MODEL)&&viewMode==VM_WANDER){
		//移动
		{
			if(upKeyDown){
				me.movloc(0,0,-6);
			}
			if(dnKeyDown){
				me.movloc(0,0,5);
			}
			//移动后位置
			float pos[4];
			me.toWorld(0,0,0,1,pos);
			//判断是否出界
			{
				if(pos[X]<xmin||pos[X]>xmax||pos[Z]<zmin||pos[Z]>zmax){//出界
					//移回到界内
					const float margin=1;
					pos[X]=min(max(pos[X],xmin+margin),xmax-margin);
					pos[Z]=min(max(pos[Z],zmin+margin),zmax-margin);
					//将me的位置设为pos
					me.setloc(pos[X],pos[Y],pos[Z]);

					
				}
				

			}
			//让me落在地面上
			{
				bool outOfRange=false;//是否越界
				Cgroundgrid&GroundGrid=groundBlock.GroundGrid;
				float H;//me所在处地面高度
				//看pos在GroundGrid的哪个格
				//求地面矩形网格参数
				float zmin_g=GroundGrid.zmin;
				float xmin_g=GroundGrid.xmin;
				float zstepLen_g=GroundGrid.zstepLen;
				float xstepLen_g=GroundGrid.xstepLen;
				int m_mat_g=GroundGrid.m_mat;
				int n_mat_g=GroundGrid.n_mat;
				//求pos在哪个格
				int I,J;//pos属于(I,J)格
				I=(pos[Z]-zmin_g)/zstepLen_g;
				J=(pos[X]-xmin_g)/xstepLen_g;
				if(I<0||I>=m_mat_g)outOfRange=true;
				if(J<0||J>=n_mat_g)outOfRange=true;
				if(!outOfRange){//如果没出界
					H=GroundGrid.mat[I][J].h;
					//求高度平移量
					float dH=H-pos[Y];
					//按平移量进行移动
					me.movloc(0,dH,0);
				}
			}
		}
		//旋转
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

void CmainDlg::Onclose() //关闭文件（而非关闭窗口）
{
	// TODO: Add your command handler code here
	workState=WS_EMPTY;
	singleStepMode=false;
	stage=0;
	unload();
	this->SetWindowText((progName+curPathName).c_str());
	this->OnPaint();//必须用立即重画
	//更新菜单项禁用状态
	updateMenuItemEnableState();

	
	

	
}
void CmainDlg::unload(){//卸载数据
	//重置路径名和图片文件名和窗口标题
	{
		curPathName="";
		picFileName_photo="";
		picFileName_wall="";
		picFileName_sky="";
	}
	//重置对象
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
	//重置全局参数
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
	MessageBox(str.c_str(),"关于 makeArc");
}

void CmainDlg::Onsave() 
{
	// TODO: Add your command handler code here
	if(workState==WS_DONE){
		//在curPathName指向的文件所在目录中建一个model文件夹，将模型保存其中
		//求curPathName指向的文件所在的目录
		string pathName;//curPathName指向的文件所在目录（此pathName以\\结束）
		pathName=ridLastLevelOfPathName(curPathName);//去掉curPathName最后一级，得到pathName
		//临时画面
		{
			addText("saveing model...",x_text_midup,y_text_midup);
		}
		//开始保存
		{
			if(!main_save_model(pathName,"model",xmin,xmax,ymin,ymax,zmin,zmax,
				offsetus,offsetut,
				cloudList,
				groundBlock.GroundGrid,
				picFileName_photo,picFileName_wall,picFileName_sky)){
				MessageBox("保存失败!");
				this->Invalidate(false);
				return;
			}
			MessageBox("保存完成!");
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
	dlg.m_ofn.lpstrInitialDir   =  (LPCTSTR)moduleClipPathName.c_str();//设置默认路径
    if(dlg.DoModal() == IDOK){
		//获得文件路径
		string pathName=dlg.GetPathName();
		//关闭任务
		Onclose();
		//临时画面
		addText("loading model...",x_text_midup,y_text_midup);
		//加载模型
		bool succ=main_load_model(pathName,xmin,xmax,ymin,ymax,zmin,zmax,offsetus,offsetut,cloudList,groundBlock.GroundGrid,textureManager);
		if(!succ){
			MessageBox("模型加载失败!");
			Onclose();
			return;
		}
		//场景管理
		main_scene_management(cloudList,qTree,groundBlock,xmin,xmax,zmin,zmax,ymin,ymax,textureManager);
		//更新工作状态和观察模式
		workState=WS_MODEL;
		viewMode=VM_OUTLINE;
		//重置相机
		reSetCam(xmin,xmax,zmin,zmax);
		//更新路径信息
		curPathName=pathName;
		//更新窗口标题
		this->SetWindowText((progName+" | "+curPathName).c_str());
		//重画
		this->Invalidate(false);
		//更新菜单项禁用状态
		updateMenuItemEnableState();

	}

	
}

void CmainDlg::OnClose() //关闭窗口
{
	// TODO: Add your message handler code here and/or call default
	Onclose();

	CDialog::OnClose();
}
void CmainDlg::updateMenuItemEnableState(){//更新菜单项的禁用状态
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
			cout<<"error 工作状态未定义!"<<endl;
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
		//获得ignoreRidTree_baseOnStepMark和ignoreRidTree_baseOnCornerMark
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
			cout<<"error: setingDlg.m_thOfRad123无效值!"<<endl;
			return;
			break;
		}
		//获得doRegulation
		doRegulation=setingDlg.m_check1;
		if(doRegulation){
			//获得regulationDegree
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
				cout<<"error: setingDlg.m_thOfRad678无效值!"<<endl;
			}
		}
		//获得useRealRoof
		switch(setingDlg.m_thOfRad45){
		case 0:
			useRealRoof=false;
			break;
		case 1:
			useRealRoof=true;
			break;
		default:
			cout<<"error: setingDlg.m_thOfRad45无效值!"<<endl;
		}
	
	}
}
