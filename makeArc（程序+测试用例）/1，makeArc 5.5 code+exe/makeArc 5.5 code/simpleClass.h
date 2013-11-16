
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
	float v[4];//����
	bool added;//��ͨ�Բ���ʱ�ķ��ʱ��
	bool wallpoint;//�Ƿ�Ϊǽ�ڵ�
	bool isGround;
	bool isCornerPoint;
	bool isCoreCornerPoint;
	bool isoutcomer;//�Ƿ�Ϊ������
	bool isEdgePoint;//�Ƿ�Ϊ�߽��
	bool isTreePoint;//�Ƿ�Ϊֲ����
	bool isOutlinePoint;//�Ƿ�Ϊ�����㣨���������ڹ������ܸ߶ȣ�
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
	float u[2];//��ͼ����
	float norm[4];
	float normf[4];
	float normn[4];
	float dir[4];//��������
	float uf[4];//ǰ��ͼ����
	float un[4];//����ͼ����
	bool isKeyPoint;//�Ƿ�Ϊ�ؼ��㣨�������򻯺����ɵĶ����ȫ�ǹؼ��㣬����������ֵ���ɵĵ㲻�ǹؼ��㣩
	Cv4Ex(){
		isKeyPoint=false;
	}
};
class Cplane//ƽ�棬�õ㷨ʽ����
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
	bool removed;//����Ƿ��ѱ��滻������
	bool visited;//�����ͨ����ʱ�Ƿ��ѷ��ʹ�
	bool visited2;//�����ȡ��Ե�ĵݹ����ʱ�Ƿ��ѷ��ʹ�
	double r2;//���Բ�뾶��ƽ��
	float x,z;//���ԲԲ��
	bool circleAlreadyGet;//ָ���������ε����Բ�Ƿ������
	myvector<CIDTriangle*>q;//Delaunay����������ָ��
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
		//��norm
		float v0v1[4],v0v2[4];
		sub(v1,v0,v0v1);
		sub(v2,v0,v0v2);
		cross(v0v1,v0v2,norm);
		normalize(norm);
	}

};

class Cline{//����ʽ
public:
	float p[4];
	float dir[4];
	//��һ��ȷ��Ϊ�߶�ʱ���������
	float vbeg[4];//���
	float vend[4];//�յ�
	Cline(){
		
	}
	int intersectWithLine_xz(Cline&line,float v[4])
		//��xzƽ������ֱ�ߵĽ���
		//����1��ʾ��Ψһ���㣬����0��ʾƽ�л��غ�
	{
		//��ֱ�߲���
		float x1=p[X];
		float z1=p[Z];
		float dx1=dir[X];
		float dz1=dir[Z];
		//ֱ��line�Ĳ���
		float x2=line.p[X];
		float z2=line.p[Z];
		float dx2=line.dir[X];
		float dz2=line.dir[Z];
		//���㹫ʽΪ��
		//t1=(x2*dz2-z2*dx2-x1*dz2+z1*dx2)/(dx1*dz2-dz1*dx2)���ֽ�Ϊt1=A/D
		//t2=-(x1*dz1-z1*dx1-x2*dz1+z2*dx1)/(dx1*dz2-dz1*dx2)���ֽ�Ϊt2=B/D
		//x=x1+t1*dx1
		//z=z1+t1*dz1
		float A=x2*dz2-z2*dx2-x1*dz2+z1*dx2;
		float B=x1*dz1-z1*dx1-x2*dz1+z2*dx1;
		float D=dx1*dz2-dz1*dx2;
		if(D==0){//ƽ�л��غ�
			return 0;
		}
		//�󽻵�
		float t1=A/D;
		float t2=B/D;
		float x=x1+t1*dx1;
		float z=z1+t1*dz1;
		init4(v,x,0,z,1);
		return 1;	
			
	}
	show(float H){
		//���
		float beg[4];
		veccopy(p,beg);
		beg[Y]=H;
		//�յ�
		float end[4];
		float vec[4];
		mul(5,dir,vec);
		add(p,vec,end);
		end[Y]=H;
		//�����
		glPointSize(8);
		glBegin(GL_POINTS);
		glVertex3fv(beg);
		glEnd();
		glPointSize(1);
		//����
		glLineWidth(4);
		glBegin(GL_LINES);
		glVertex3fv(beg);
		glVertex3fv(end);
		glEnd();
		glLineWidth(1);
		
		
	}
	show_beg_end(float H){
		//��vbeg��vend
	//	glPointSize(4);
		glBegin(GL_POINTS);
		glVertex3f(vbeg[X],H,vbeg[Z]);
		glVertex3f(vend[X],H,vend[Z]);
		glEnd();
	//	glPointSize(1);
		//����
		glBegin(GL_LINES);
		glVertex3f(vbeg[X],H,vbeg[Z]);
		glVertex3f(vend[X],H,vend[Z]);
		glEnd();
	
	}
	
	void getProjOfPoint_xz(const float vert[4],float _v[4])
		//��xzƽ�������v�ڱ�ֱ���ϵ�ͶӰ����
		//������_v��_v[Y]==0
	{
		float v[4]={vert[X],0,vert[Z],1};
		float _p[4];
		veccopy(p,_p);
		float _dir[4];
		veccopy(dir,_dir);
		//������Ϊ_v=_p+(_pv*_dir)*_dir
		//�ֽ�ΪK=(_pv*dir),vec=K*_dir,_v=p+vec
		float _pv[4];
		sub(v,_p,_pv);
		float K=dot(_pv,_dir);
		float vec[4];
		mul(K,_dir,vec);
		add(_p,vec,_v);//�õ�_v
		
	}

	pullPointsToLine(){//��xzƽ���Ͻ���ֱ���������ĵ㶼������ֱ����
		
			//��vbeg������ֱ����
		{
			float _v[4];//������
			getProjOfPoint_xz(vbeg,_v);
			vbeg[X]=_v[X];
			vbeg[Z]=_v[Z];
		}
			//��vend������ֱ����
		{
			float _v[4];//������
			getProjOfPoint_xz(vend,_v);
			vend[X]=_v[X];
			vend[Z]=_v[Z];
		}
		


	}
	float getD2_xz()const{//��xzƽ�����߶γ��ȵ�ƽ��
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
	myvector<Cv4Ex> vlist;//�����б�
	myvector<Cline> lineList;
	bool removed;
	float norm[4];//������
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
	cal_c(float _c[4])const {//��������
		float c[4]={0,0,0,0};
		int nv=(int)vlist.size();
		for(int t=0;t<nv;t++){
			add(vlist[t].v,c,c);
		}
		c[X]/=nv;
		c[Y]/=nv;
		c[Z]/=nv;
		c[W]=1;
		//��c������_c
		veccopy(c,_c);

	}
	addvert(float p[4]){
		vlist.grow();
		veccopy(p,vlist[(int)vlist.size()-1].v);
	}
	ridSameNeighbour(){//ɾ�����ڵ��غϵ�
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
	calDirForEachVert_xz(){//��xzƽ��������㷽������
		int nv=(int)vlist.size();
		for(int k=0;k<nv;k++){
			Cv4Ex&v=vlist[k];
			Cv4Ex&vn=vlist[(k+1)%nv];
			float dir[4];
			sub(vn.v,v.v,dir);
			dir[Y]=0;
			if(normalize(dir)==0){//˵��k��(k+1)%nv�غ�
				init4(v.dir,0,0,0,0);
				cout<<"warning!"<<endl;
			}else{
				veccopy(dir,v.dir);
			}
		}				
	}
};
