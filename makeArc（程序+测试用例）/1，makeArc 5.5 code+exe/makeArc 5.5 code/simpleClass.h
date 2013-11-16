
template<class T> 
class myvector:public vector<T>
{
public:
	myvector():vector<T>(){};
	void grow(){
		this->resize((int)this->size()+1);
	}
};

class Cv4{
public:
	float v[4];//坐标
	bool added;//连通性测试时的访问标记
	bool wallpoint;//是否为墙壁点
	bool isGround;
	bool isCornerPoint;
	bool isCoreCornerPoint;
	bool isoutcomer;//是否为外来点
	bool isEdgePoint;//是否为边界点
	bool isTreePoint;//是否为植被点
	bool isOutlinePoint;//是否为轮廓点（轮廓点用于估算屋檐高度）
	Cv4(){
		isOutlinePoint=false;
		isTreePoint=false;
		isEdgePoint=false;
		isCoreCornerPoint=false;
		isoutcomer=false;
		isCornerPoint=false;
		isGround=false;
		added=false;
		wallpoint=false;
	}
};
class Cv4Ex:public Cv4 
{
public:
	float u[2];//贴图坐标
	float norm[4];
	float normf[4];
	float normn[4];
	float dir[4];//方向向量
	float uf[4];//前贴图坐标
	float un[4];//后贴图坐标
	bool isKeyPoint;//是否为关键点（轮廓规则化后生成的多边形全是关键点，后来经过插值生成的点不是关键点）
	Cv4Ex(){
		isKeyPoint=false;
	}
};
class Cplane//平面，用点法式描述
{
public:
	float p[4];
	float norm[4];
	makePlane(float _p[4],float _norm[4]){
		veccopy(_p,p);
		veccopy(_norm,norm);
	}
};
class CIDTriangle{
public:
	int vID[3];
	CIDTriangle*p[3];
	CIDTriangle*nxt;
	CIDTriangle*foe;
	bool removed;//标记是否已被替换下来了
	bool visited;//标记连通测试时是否已访问过
	bool visited2;//标记提取边缘的递归过程时是否已访问过
	double r2;//外接圆半径的平方
	float x,z;//外接圆圆心
	bool circleAlreadyGet;//指明此三角形的外接圆是否已求过
	myvector<CIDTriangle*>q;//Delaunay三角树孩子指针
	CIDTriangle(){
		circleAlreadyGet=false;
		visited=false;
		visited2=false;
		removed=false;
		nxt=NULL;
		foe=NULL;
		p[0]=NULL;
		p[1]=NULL;
		p[2]=NULL;
	}
	setp(CIDTriangle*p0,CIDTriangle*p1,CIDTriangle*p2){
		p[0]=p0;
		p[1]=p1;
		p[2]=p2;
	}
	setvID(int ID0,int ID1,int ID2){
		vID[0]=ID0;
		vID[1]=ID1;
		vID[2]=ID2;
	}

	
};
class Cquad{
public:
	float v0[4];
	float v1[4];
	float v2[4];
	float v3[4];
	float norm[4];
	bool removed;
	Cquad(){
		removed=false;
	}
	makequad(float _v0[4],float _v1[4],float _v2[4],float _v3[4]){
		veccopy(_v0,v0);
		veccopy(_v1,v1);
		veccopy(_v2,v2);
		veccopy(_v3,v3);
		//求norm
		float v0v1[4],v0v2[4];
		sub(v1,v0,v0v1);
		sub(v2,v0,v0v2);
		cross(v0v1,v0v2,norm);
		normalize(norm);
	}

};

class Cline{//点向式
public:
	float p[4];
	float dir[4];
	//进一步确定为线段时加入的属性
	float vbeg[4];//起点
	float vend[4];//终点
	Cline(){
		
	}
	int intersectWithLine_xz(Cline&line,float v[4])
		//求xz平面上两直线的交点
		//返回1表示有唯一交点，返回0表示平行或重合
	{
		//本直线参数
		float x1=p[X];
		float z1=p[Z];
		float dx1=dir[X];
		float dz1=dir[Z];
		//直线line的参数
		float x2=line.p[X];
		float z2=line.p[Z];
		float dx2=line.dir[X];
		float dz2=line.dir[Z];
		//交点公式为：
		//t1=(x2*dz2-z2*dx2-x1*dz2+z1*dx2)/(dx1*dz2-dz1*dx2)，分解为t1=A/D
		//t2=-(x1*dz1-z1*dx1-x2*dz1+z2*dx1)/(dx1*dz2-dz1*dx2)，分解为t2=B/D
		//x=x1+t1*dx1
		//z=z1+t1*dz1
		float A=x2*dz2-z2*dx2-x1*dz2+z1*dx2;
		float B=x1*dz1-z1*dx1-x2*dz1+z2*dx1;
		float D=dx1*dz2-dz1*dx2;
		if(D==0){//平行或重合
			return 0;
		}
		//求交点
		float t1=A/D;
		float t2=B/D;
		float x=x1+t1*dx1;
		float z=z1+t1*dz1;
		init4(v,x,0,z,1);
		return 1;	
			
	}
	show(float H){
		//起点
		float beg[4];
		veccopy(p,beg);
		beg[Y]=H;
		//终点
		float end[4];
		float vec[4];
		mul(5,dir,vec);
		add(p,vec,end);
		end[Y]=H;
		//画起点
		glPointSize(8);
		glBegin(GL_POINTS);
		glVertex3fv(beg);
		glEnd();
		glPointSize(1);
		//画线
		glLineWidth(4);
		glBegin(GL_LINES);
		glVertex3fv(beg);
		glVertex3fv(end);
		glEnd();
		glLineWidth(1);
		
		
	}
	show_beg_end(float H){
		//画vbeg和vend
	//	glPointSize(4);
		glBegin(GL_POINTS);
		glVertex3f(vbeg[X],H,vbeg[Z]);
		glVertex3f(vend[X],H,vend[Z]);
		glEnd();
	//	glPointSize(1);
		//画边
		glBegin(GL_LINES);
		glVertex3f(vbeg[X],H,vbeg[Z]);
		glVertex3f(vend[X],H,vend[Z]);
		glEnd();
	
	}
	
	void getProjOfPoint_xz(const float vert[4],float _v[4])
		//在xz平面上求点v在本直线上的投影坐标
		//返回在_v，_v[Y]==0
	{
		float v[4]={vert[X],0,vert[Z],1};
		float _p[4];
		veccopy(p,_p);
		float _dir[4];
		veccopy(dir,_dir);
		//新坐标为_v=_p+(_pv*_dir)*_dir
		//分解为K=(_pv*dir),vec=K*_dir,_v=p+vec
		float _pv[4];
		sub(v,_p,_pv);
		float K=dot(_pv,_dir);
		float vec[4];
		mul(K,_dir,vec);
		add(_p,vec,_v);//得到_v
		
	}

	pullPointsToLine(){//在xz平面上将本直线所关联的点都拉到本直线上
		
			//将vbeg拉到本直线上
		{
			float _v[4];//新坐标
			getProjOfPoint_xz(vbeg,_v);
			vbeg[X]=_v[X];
			vbeg[Z]=_v[Z];
		}
			//将vend拉到本直线上
		{
			float _v[4];//新坐标
			getProjOfPoint_xz(vend,_v);
			vend[X]=_v[X];
			vend[Z]=_v[Z];
		}
		


	}
	float getD2_xz()const{//求xz平面上线段长度的平方
		float d2=pow2(vbeg[X]-vend[X])+pow2(vbeg[Z]-vend[Z]);
		return d2;

	}
	makeLine(const float _p[4],const float _dir[4]){
		veccopy(_p,p);
		veccopy(_dir,dir);
	}

};

class Cpolygen{
public:
	myvector<Cv4Ex> vlist;//顶点列表
	myvector<Cline> lineList;
	bool removed;
	float norm[4];//法向量
	float color[4];
	bool isHole;
	bool isOutline;
	bool takeOut;
	Cpolygen(){
		isOutline=false;
		isHole=false;
		takeOut=false;
		init4(color,1,1,1,1);
		init4(norm,0,0,0,0);
		removed=false;
	}
	clearLineList(){
		lineList.swap(myvector<Cline>());
	
	}
	cal_c(float _c[4])const {//计算重心
		float c[4]={0,0,0,0};
		int nv=(int)vlist.size();
		for(int t=0;t<nv;t++){
			add(vlist[t].v,c,c);
		}
		c[X]/=nv;
		c[Y]/=nv;
		c[Z]/=nv;
		c[W]=1;
		//将c拷贝给_c
		veccopy(c,_c);

	}
	addvert(float p[4]){
		vlist.grow();
		veccopy(p,vlist[(int)vlist.size()-1].v);
	}
	ridSameNeighbour(){//删除相邻的重合点
		int nv=(int)vlist.size();
		for(int k=0;k<nv;k++){
			Cv4Ex&v=vlist[k];
			Cv4Ex&vn=vlist[(k+1)%nv];
			float d=fabs(v.v[X]-vn.v[X])+fabs(v.v[Y]-vn.v[Y])+fabs(v.v[Z]-vn.v[Z]);
			if(d<=0.001){
				vlist.erase(&vlist[k]);
				k--;
				nv--;
			}
		}	
		
	}
	calDirForEachVert_xz(){//在xz平面上求各点方向向量
		int nv=(int)vlist.size();
		for(int k=0;k<nv;k++){
			Cv4Ex&v=vlist[k];
			Cv4Ex&vn=vlist[(k+1)%nv];
			float dir[4];
			sub(vn.v,v.v,dir);
			dir[Y]=0;
			if(normalize(dir)==0){//说明k与(k+1)%nv重合
				init4(v.dir,0,0,0,0);
				cout<<"warning!"<<endl;
			}else{
				veccopy(dir,v.dir);
			}
		}				
	}
};
