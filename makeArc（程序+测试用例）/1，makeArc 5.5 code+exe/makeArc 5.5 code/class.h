
class Ccell{
public:
	int innerLayerID;//empty的cell的innerLayerID=0，needcut的cell的innerLayerID=1，再里一层就是2,再里一层就是3
	myvector<int> IDList;//落在此格中的云点的ID列表
	bool empty;//本格是否为空（即v[Y]是否已填充了有效值）
	           //最初时IDList为空则empty为true,但补充之后就不一定了，只能通过empty来判断，而不能通过IDList是否为空来判断
	bool ismadeup;//标记一个非empty格是否是由补充得来
	float v[4];//中心点（其高度为IDList中各点的最大高度）
	float u[2];//中心点对应的贴图坐标
	float vlt[4],vrt[4],vup[4],vdn[4],vlup[4],vldn[4],vrup[4],vrdn[4];//本格的八个边界点
	float ult[2],urt[2],uup[2],udn[2],ulup[2],uldn[2],urup[2],urdn[2];//本格的八个边界点贴图坐标
	bool ltJump,rtJump,upJump,dnJump,lupJump,ldnJump,rupJump,rdnJump;//此格的上下左右，左上，左下，右上，右下邻格是否阶跃
	float ltdH,rtdH,updH,dndH,lupdH,ldndH,rupdH,rdndH;//此格与上下左右，左上，左下，右上，右下邻格的中心高度差（阶跃差）（本格减邻格）
	bool isCorner;//是角点
	bool isCorner_rough;//是粗角点
	float norm[4][4];//四个小面的法向量（法向量只用于去树）
	float avrgnorm[4];//四个小面的平均法向量（法向量只用于去树）
	bool needcut;//需要切割
	myvector<Cpolygen> polygenList;//多边形列表
	//  vlup--vup--vrup
	//   |  0  |  3 |
	//  vlt----v----vrt
	//   |  1  |  2 |
    //  vldn--vdn--vrdn
	bool asWhole4;//指明以此格左上角的4个格(2*2)是否作为一个整体绘制(对修补裂缝有用)
	Ccell(){
		innerLayerID=-1;
		asWhole4=false;
		ismadeup=false;
		//---------------------------------------------------------------
		needcut=false;
		isCorner=false;
		isCorner_rough=false;
		ltdH=rtdH=updH=dndH=lupdH=ldndH=rupdH=rdndH=0;
		ltJump=rtJump=upJump=dnJump=lupJump=ldnJump=rupJump=rdnJump=false;
		empty=true;
	}
	clearIDList(){
		IDList.swap(myvector<int>());
	}

	calTriPolygenNorm()
	//计算三角多边形的法向量（注意，此过程只能在所有polygen还都是三角形时调用）
	{
		//此时polygenList中各polygen尚均为三角形
		int npolygen=(int)polygenList.size();
		for(int i=0;i<npolygen;i++){
			Cpolygen&polygen=polygenList[i];
			float *v0=polygen.vlist[0].v;
			float *v1=polygen.vlist[1].v;
			float *v2=polygen.vlist[2].v;
			float v0v1[4];
			float v0v2[4];
			sub(v1,v0,v0v1);
			sub(v2,v0,v0v2);
			float norm[4];
			cross(v0v1,v0v2,norm);
			normalize(norm);
			veccopy(norm,polygen.norm);

		}
	}
	void polygenlize()
	//转化成多边形集合，仅对needcut作
	{
		if(!needcut)return;
		//    vlup-------vrup
		//     |           |  
		//     |           |
		//     |           |  
		//    vldn-------vrdn 
	
		Cpolygen polygen_up;
		Cpolygen polygen_dn;
		if(vlup[Y]+vrdn[Y]>=vldn[Y]+vrup[Y]){
			//     |\|
			//上
			polygen_up.addvert(vrup);
			polygen_up.addvert(vlup);
			polygen_up.addvert(vrdn);
			//下
			polygen_dn.addvert(vlup);
			polygen_dn.addvert(vldn);
			polygen_dn.addvert(vrdn);

		}else{
			//     |/|
			//上
			polygen_up.addvert(vrup);
			polygen_up.addvert(vlup);
			polygen_up.addvert(vldn);

			//下
			polygen_dn.addvert(vrup);
			polygen_dn.addvert(vldn);
			polygen_dn.addvert(vrdn);
		}
		polygenList.push_back(polygen_up);
		polygenList.push_back(polygen_dn);
		

	}

	calTexCoord(float xmin,float xmax,float zmin,float zmax,
		float offsetus,float offsetut)
	//为polygenList中的点计算贴图坐标
	//为v,vup,vdn,vlt,vrt,vlup,vldn,vrup,vrdn计算贴图坐标
	{
		//为polygenList中的点计算贴图坐标
		int npolygen=(int)polygenList.size();
		for(int i=0;i<npolygen;i++){
			Cpolygen&polygen=polygenList[i];
			if(polygen.removed)continue;
			int nv=(int)polygen.vlist.size();
			for(int j=0;j<nv;j++){
				Cv4Ex&v=polygen.vlist[j];
				v.u[0]=(v.v[X]-xmin)/(xmax-xmin)+offsetus;
				v.u[1]=(v.v[Z]-zmin)/(zmax-zmin)+offsetut;
			}
		}
	
		//为v,vup,vdn,vlt,vrt,vlup,vldn,vrup,vrdn计算贴图坐标
		//v
		u[0]=(v[X]-xmin)/(xmax-xmin)+offsetus;
		u[1]=(v[Z]-zmin)/(zmax-zmin)+offsetut;
		//vup
		uup[0]=(vup[X]-xmin)/(xmax-xmin)+offsetus;
		uup[1]=(vup[Z]-zmin)/(zmax-zmin)+offsetut;
		//udn
		udn[0]=(vdn[X]-xmin)/(xmax-xmin)+offsetus;
		udn[1]=(vdn[Z]-zmin)/(zmax-zmin)+offsetut;
		//vlt
		ult[0]=(vlt[X]-xmin)/(xmax-xmin)+offsetus;
		ult[1]=(vlt[Z]-zmin)/(zmax-zmin)+offsetut;
		//vrt
		urt[0]=(vrt[X]-xmin)/(xmax-xmin)+offsetus;
		urt[1]=(vrt[Z]-zmin)/(zmax-zmin)+offsetut;
		//vlup
		ulup[0]=(vlup[X]-xmin)/(xmax-xmin)+offsetus;
		ulup[1]=(vlup[Z]-zmin)/(zmax-zmin)+offsetut;
		//vldn
		uldn[0]=(vldn[X]-xmin)/(xmax-xmin)+offsetus;
		uldn[1]=(vldn[Z]-zmin)/(zmax-zmin)+offsetut;
		//vrup
		urup[0]=(vrup[X]-xmin)/(xmax-xmin)+offsetus;
		urup[1]=(vrup[Z]-zmin)/(zmax-zmin)+offsetut;
		//vrdn
		urdn[0]=(vrdn[X]-xmin)/(xmax-xmin)+offsetus;
		urdn[1]=(vrdn[Z]-zmin)/(zmax-zmin)+offsetut;

	
	}

	calNorms()
		//计算四个小面的法向量
		//计算四个小面的平均法向量
		//计算八个小三角面的法向量
	{
		//    vlup--vup--vrup
		//     |  0  |  3  |  
		//    vlt----v----vrt
		//     |  1  |  2  |  
		//    vldn--vdn--vrdn   
		//计算四个小面的法向量
		//面0
		init4(norm[0],0,0,0,0);
		float tnorm[4];
		calNorm(vup,vlup,vlt,tnorm);add(norm[0],tnorm,norm[0]);
		calNorm(vup,vlt,v,tnorm);add(norm[0],tnorm,norm[0]);
		calNorm(vup,vlup,v,tnorm);add(norm[0],tnorm,norm[0]);
		calNorm(vlup,vlt,v,tnorm);add(norm[0],tnorm,norm[0]);
		normalize(norm[0]);
		//面1
		init4(norm[1],0,0,0,0);
		calNorm(v,vlt,vldn,tnorm);add(norm[1],tnorm,norm[1]);
		calNorm(v,vldn,vdn,tnorm);add(norm[1],tnorm,norm[1]);
		calNorm(v,vlt,vdn,tnorm);add(norm[1],tnorm,norm[1]);
		calNorm(vlt,vldn,vdn,tnorm);add(norm[1],tnorm,norm[1]);
		normalize(norm[1]);
		//面2
		init4(norm[2],0,0,0,0);
		calNorm(vrt,v,vdn,tnorm);add(norm[2],tnorm,norm[2]);
		calNorm(vrt,vdn,vrdn,tnorm);add(norm[2],tnorm,norm[2]);
		calNorm(vrt,v,vrdn,tnorm);add(norm[2],tnorm,norm[2]);
		calNorm(v,vdn,vrdn,tnorm);add(norm[2],tnorm,norm[2]);
		normalize(norm[2]);
		//面3
		init4(norm[3],0,0,0,0);
		calNorm(vrup,vup,v,tnorm);add(norm[3],tnorm,norm[3]);
		calNorm(vrup,v,vrt,tnorm);add(norm[3],tnorm,norm[3]);
		calNorm(vrup,vup,vrt,tnorm);add(norm[3],tnorm,norm[3]);
		calNorm(vup,v,vrt,tnorm);add(norm[3],tnorm,norm[3]);
		normalize(norm[3]);

		//计算它们的平均法向量
		init4(avrgnorm,0,0,0,0);
		add(avrgnorm,norm[0],avrgnorm);
		add(avrgnorm,norm[1],avrgnorm);
		add(avrgnorm,norm[2],avrgnorm);
		add(avrgnorm,norm[3],avrgnorm);
		normalize(avrgnorm);//不要忘了单位化

	

	}

	show_rough0(float r=1,float g=1,float b=1,float a=1)
	//细致显示
	{

	/*  //此段调试显示innerLayerID用
 		r=(float)(innerLayerID*10)/255;
		g=r;
		b=r;
		glColor4f(r,g,b,a);*/


		glColor4f(r,g,b,a);
		if(needcut){
			//画多边形列表
			int npolygen=(int)polygenList.size();
			for(int i=0;i<npolygen;i++){
				Cpolygen&polygen=polygenList[i];
				if(polygen.removed)continue;
				//画polygen
				float*norm=polygen.norm;
				glBegin(GL_POLYGON);
				glNormal3fv(norm);
				int nv=(int)polygen.vlist.size();
				for(int j=0;j<nv;j++){
					float*v=polygen.vlist[j].v;
					float*u=polygen.vlist[j].u;
					
					glTexCoord2fv(u);
					glVertex3fv(v);
					
				}
				glEnd();
			}
			
			
		}else{//!needcut
			//画cell
			//    vlup----vrup
			//     |        |  
			//     |        |  
			//    vldn----vrdn   
			
			glBegin(GL_QUADS);
			glNormal3fv(avrgnorm);
			glTexCoord2fv(urup);glVertex3fv(vrup);
			glTexCoord2fv(ulup);glVertex3fv(vlup);
			glTexCoord2fv(uldn);glVertex3fv(vldn);
			glTexCoord2fv(urdn);glVertex3fv(vrdn);
			glEnd();
			
		}
		glColor4f(1,1,1,1);

	}

	
};


class Cgrid
{
private:
	float xmin,xmax,zmin,zmax;//xz范围（局部坐标系）
public:
	int maxInnerLayerID;//各格innerLayerID的最大值
	float m_locToWorld[16];//由主方向空间转到世界空间
	float m_worldToLoc[16];//由世界空间转到主方向空间
	int m_mat;//IDlistmat的行数
	int n_mat;//IDlistmat的列数
	float xstepLen;//x步长
	float zstepLen;//z步长
	myvector<myvector<Ccell*> > mat;//cell矩阵
	Cgrid(){
		maxInnerLayerID=-1;
		m_mat=0;
		n_mat=0;
		xstepLen=1;
		zstepLen=1;
		xmin=xmax=zmin=zmax=0;
	
	}
	void getRange(float&_xmin,float&_xmax,float&_zmin,float&_zmax)const{//获得范围
		_xmin=xmin;
		_xmax=xmax;
		_zmin=zmin;
		_zmax=zmax;
	}
	void setRange(float _xmin,float _xmax,float _zmin,float _zmax){//设定范围
		xmin=_xmin;
		xmax=_xmax;
		zmin=_zmin;
		zmax=_zmax;
	}
	~Cgrid(){
		for(int i=0;i<m_mat;i++){
			for(int j=0;j<n_mat;j++){
				if(mat[i][j]!=NULL){
					delete mat[i][j];
					mat[i][j]=NULL;
				}
			}
		}
		
	}

	float calS_notEmpty(){//计算本网格中非empty的cell的面积和
		float S=0;
		float dS=xstepLen*zstepLen;//一个格的面积
		float halfdS=dS/2;
		for(int i=0;i<m_mat;i++){
			for(int j=0;j<n_mat;j++){
				if(mat[i][j]==NULL)continue;
				Ccell&cell=*mat[i][j];
				if(cell.empty)continue;
				if(cell.needcut){
					S+=halfdS;
				}else{
					S+=dS;
				}
			}
		}
		return S;
	}
	clearMatElms(){//删除mat的所有元素
		for(int i=0;i<m_mat;i++){
			for(int j=0;j<n_mat;j++){
				if(mat[i][j]!=NULL){
					delete mat[i][j];
					mat[i][j]=NULL;//一定不要忘了置NULL
				}
			}
		}
		
	}
	clearMat(){//删除mat的所有元素，并删除mat框架
		clearMatElms();
		mat.resize(0);
		//由于执行了mat.resize(0)，所以必须执行下面两句
		m_mat=0;
		n_mat=0;
		xmin=xmax=zmin=zmax=0;
		
		
	}
	
	
	clearIDListForEachCell(){//清除各cell的IDList
		for(int i=0;i<m_mat;i++){
			for(int j=0;j<n_mat;j++){
				if(mat[i][j]==NULL)continue;
				Ccell&cell=*mat[i][j];
				cell.clearIDList();
			
			}
		}

	
	}
	removeEmptyCell(){//将empty的格删除
		for(int i=0;i<m_mat;i++){
			for(int j=0;j<n_mat;j++){
				if(mat[i][j]->empty){
					delete mat[i][j];
					mat[i][j]=NULL;//一定要赋为NULL
				}
			}
		}

	
	}
	bool vInCellIJ_vIsWorldPoint(const float v[4],int&I,int&J){//求世界坐标系中的v在哪个格中
		//将v变换到局部坐标系
		float v_loc[4];
		prodv(m_worldToLoc,v,v_loc);
		//求v_loc在哪个格
		I=(v_loc[Z]-zmin)/zstepLen;
		J=(v_loc[X]-xmin)/xstepLen;

		if(I<0||I>=m_mat)return false;
		if(J<0||J>=n_mat)return false;
		return true;

	
	}
	float getH_vIsWorldPoint(const float v[4]){//求世界坐标系中的点v在本网格中的插值高度
		//将v转到局部空间
		float _v[4];//v的局部坐标
		prodv(m_worldToLoc,v,_v);
		//求_v在哪个格
		int I,J;//_v属于(I,J)格
		I=(_v[Z]-zmin)/zstepLen;
		J=(_v[X]-xmin)/xstepLen;
		bool succ=true;
		if(I<0||I>=m_mat)succ=false;
		if(J<0||J>=n_mat)succ=false;
		if(!succ){
			if(showWarning)cout<<"error! 未落在网格内"<<endl;
			if(I<0)I=0;
			if(I>=m_mat)I=m_mat-1;
			if(J<0)J=0;
			if(J>=n_mat)J=n_mat-1;
		}
		Ccell&cell=*(mat[I][J]);
		float vlup[4],vldn[4],vrup[4],vrdn[4];
		veccopy(cell.vlup,vlup);
		veccopy(cell.vldn,vldn);
		veccopy(cell.vrup,vrup);
		veccopy(cell.vrdn,vrdn);
		//将vlup,vldn,vrup,vrdn转到局部空间
		prodv(m_worldToLoc,vlup,vlup);
		prodv(m_worldToLoc,vldn,vldn);
		prodv(m_worldToLoc,vrup,vrup);
		prodv(m_worldToLoc,vrdn,vrdn);
		//通过双线性插值求_v处的高度
		float H=calH_BilinearAABB(vlup,vldn,vrup,vrdn,_v);
		return H;

	
	}
	caluOfEachCell(float offsetus,float offsetut)
	//为各cell中心v点计算纹理坐标u
	{
		for(int i=0;i<m_mat;i++){
			for(int j=0;j<n_mat;j++){
				if(mat[i][j]->empty)continue;
				Ccell&cell=*mat[i][j];
				
				//计算cell.u
				cell.u[0]=(cell.v[X]-xmin)/(xmax-xmin)+offsetus;
				cell.u[1]=(cell.v[Z]-zmin)/(zmax-zmin)+offsetut;

			}
		}

	}


	
	
	
	
	//---------------------------------------------------------------------------------------------------------------
cutNeedcutCells(float _v1[4],float _v2[4])
	//用线段_v1_v2处的立面去切割所有needcut==true的cell的所有多边形
	{

		float v1[4],v2[4];
		veccopy(_v1,v1);
		v1[Y]=0;
		veccopy(_v2,v2);
		v2[Y]=0;
		//求v1v2的右法向量
		float v1v2[4];
		sub(v2,v1,v1v2);
		float norm[4]={-v1v2[Z],0,v1v2[X],0};
		normalize(norm);
		//制作v1v2处的立面
		Cplane plane;
		plane.makePlane(v1,norm);
		//求v1所在格
		int I1,J1;//v1属于(I1,J1)格
		vInCellIJ_vIsWorldPoint(v1,I1,J1);
		//求v2所在格
		int I2,J2;//v2属于(I2,J2)格
		vInCellIJ_vIsWorldPoint(v2,I2,J2);
		//求v1v2的矩形框范围
		int minI=min(I1,I2);
		int maxI=max(I1,I2);
		int minJ=min(J1,J2);
		int maxJ=max(J1,J2);
		//用plane去切割所有处在[minI,minJ]-[maxI,maxJ]范围内且needcut==true的cell中的所有多边形
		for(int i=minI;i<=maxI;i++){
			if(i<0||i>=m_mat)continue;
			for(int j=minJ;j<=maxJ;j++){
				if(j<0||j>=n_mat)continue;
				Ccell&cell=*mat[i][j];
				if(!cell.needcut)continue;
				//用plane去切割cell中所有多边形
				int npolygen=(int)cell.polygenList.size();
				for(int k=0;k<npolygen;k++){
					Cpolygen&polygen=cell.polygenList[k];
					//用plane去切割polygen
					float side1,side2;
					bool iscoplane;
					bool isdivided;
					Cpolygen polygen1,polygen2;
					bool isnormal=plane_div_polygen(plane,polygen,polygen1,side1,polygen2,side2,iscoplane,isdivided);
					if(iscoplane)continue;
					if(!isnormal)continue;
					if(isdivided){//如果切割成功
						//将polygen的法向量赋予polygen1和polygen2
						veccopy(polygen.norm,polygen1.norm);
						veccopy(polygen.norm,polygen2.norm);
						//用polygen1替换polygen
						polygen=polygen1;
						//将polygen2加入到polygenList
						cell.polygenList.push_back(polygen2);
					}else{//未成功，说明在某一边
						
					}
					
				}
				
			}
		}
		
	}

	polygenlize()
	//将各格polygenlize
	{
		for(int i=0;i<m_mat;i++){
			for(int j=0;j<n_mat;j++){
				if(mat[i][j]->empty)continue;
				Ccell&cell=*mat[i][j];
			
				cell.polygenlize();
			}
		}

	}
	calTriPolygenNorm()
	//求各格的polygenList中各三角polygen的法向量，注意，此过程只能在所有polygen还都是三角形时调用
	{
		for(int i=0;i<m_mat;i++){
			for(int j=0;j<n_mat;j++){
				if(mat[i][j]->empty)continue;
				Ccell&cell=*mat[i][j];
				
				cell.calTriPolygenNorm();
			}
		}

	}
	calTexCoord(float xmin,float xmax,float zmin,float zmax,
		float offsetus,float offsetut){
		for(int i=0;i<m_mat;i++){
			for(int j=0;j<n_mat;j++){
				if(mat[i][j]==NULL)continue;
				if(mat[i][j]->empty)continue;
				Ccell&cell=*mat[i][j];
				
				cell.calTexCoord(xmin,xmax,zmin,zmax,offsetus,offsetut);
			}
		}	
	}

	bool isTree()
	//判断此grid是否为tree
	{
		//如果grid中非empty的cell中isCorner_rough==true的cell超过一定比例，则认为此grid是tree
		int count_notEmpty=0;//统计非empty的cell的个数
		int count_isCorner_rough=0;//统计isCorner_rough==true的cell的个数
	    for(int i=0;i<m_mat;i++){
			for(int j=0;j<n_mat;j++){
				Ccell&cell=*mat[i][j];
				if(cell.empty||cell.needcut)continue;
				count_notEmpty++;
				if(cell.isCorner_rough||cell.isCorner){
					count_isCorner_rough++;
				}
			}
		}
		if(count_notEmpty==0){
			return true;
		}else{
			float rate=(float)count_isCorner_rough/count_notEmpty;
			if(count_notEmpty<=10){
				if(rate>=0.5)return true;
				else return false;
			}else{
				if(rate>=0.8)return true;
				else return false;
			}
		}
		
		
	
	}
	markCorner()
	//标记角点cell
	{
	
		for(int i=1;i<=m_mat-2;i++){//i取1到m_mat-2是为了防止邻格越界
			for(int j=1;j<=n_mat-2;j++){//j取1到n_mat-2是为了防止邻格越界
				if(mat[i][j]->empty)continue;
				Ccell&cell=*mat[i][j];
				
				Ccell&upcell=*mat[i-1][j];
				Ccell&dncell=*mat[i+1][j];
				Ccell&ltcell=*mat[i][j-1];
				Ccell&rtcell=*mat[i][j+1];
				if(upcell.empty||dncell.empty||ltcell.empty||rtcell.empty){//边界点不保护
					cell.isCorner=false;
					cell.isCorner_rough=false;
					continue;
				}
				
			
				//求本格的平均法向偏离量
				{
					float D=0;//夹角A的sin的标准差
					float tcrossRs[4];
					cross(cell.avrgnorm,cell.norm[0],tcrossRs);
					D+=pow2(tcrossRs[X])+pow2(tcrossRs[Y])+pow2(tcrossRs[Z]);
					cross(cell.avrgnorm,cell.norm[1],tcrossRs);
					D+=pow2(tcrossRs[X])+pow2(tcrossRs[Y])+pow2(tcrossRs[Z]);
					cross(cell.avrgnorm,cell.norm[2],tcrossRs);
					D+=pow2(tcrossRs[X])+pow2(tcrossRs[Y])+pow2(tcrossRs[Z]);
					cross(cell.avrgnorm,cell.norm[3],tcrossRs);
					D/=4;
	//				D=sqrt(D);
					//如果法向量的标准差超过一定值，则认为是角点cell
					if(D>=0.16*0.16){
						cell.isCorner=true;
					}else{
						cell.isCorner=false;
					}
				}
				//求邻格的平均法向偏离量
				{
					Ccell&upcell=*mat[i-1][j];
					Ccell&dncell=*mat[i+1][j];
					Ccell&ltcell=*mat[i][j-1];
					Ccell&rtcell=*mat[i][j+1];
					Ccell&lupcell=*mat[i-1][j-1];
					Ccell&ldncell=*mat[i+1][j-1];
					Ccell&rupcell=*mat[i-1][j+1];
					Ccell&rdncell=*mat[i+1][j+1];
					//求周围平均法向偏离量
					float D=0;//夹角A的sin的标准差
					float tcrossRs[4];
					cross(cell.avrgnorm,upcell.avrgnorm,tcrossRs);
					D+=pow2(tcrossRs[X])+pow2(tcrossRs[Y])+pow2(tcrossRs[Z]);
					cross(cell.avrgnorm,dncell.avrgnorm,tcrossRs);
					D+=pow2(tcrossRs[X])+pow2(tcrossRs[Y])+pow2(tcrossRs[Z]);
					cross(cell.avrgnorm,ltcell.avrgnorm,tcrossRs);
					D+=pow2(tcrossRs[X])+pow2(tcrossRs[Y])+pow2(tcrossRs[Z]);
					cross(cell.avrgnorm,rtcell.avrgnorm,tcrossRs);
					D+=pow2(tcrossRs[X])+pow2(tcrossRs[Y])+pow2(tcrossRs[Z]);
					cross(cell.avrgnorm,lupcell.avrgnorm,tcrossRs);
					D+=pow2(tcrossRs[X])+pow2(tcrossRs[Y])+pow2(tcrossRs[Z]);
					cross(cell.avrgnorm,ldncell.avrgnorm,tcrossRs);
					D+=pow2(tcrossRs[X])+pow2(tcrossRs[Y])+pow2(tcrossRs[Z]);
					cross(cell.avrgnorm,rupcell.avrgnorm,tcrossRs);
					D+=pow2(tcrossRs[X])+pow2(tcrossRs[Y])+pow2(tcrossRs[Z]);
					cross(cell.avrgnorm,rdncell.avrgnorm,tcrossRs);
					D+=pow2(tcrossRs[X])+pow2(tcrossRs[Y])+pow2(tcrossRs[Z]);
					D/=8;
		//			D=sqrt(D);
					if(D>=0.175*0.175){
						cell.isCorner_rough=true;
					}else{
						cell.isCorner_rough=false;
					
					}
					
				}

			}
		}


	
	}
	UnmarkCorner()
	//清空角点cell标记
	{
		for(int i=0;i<m_mat;i++){//i取1到m_mat-2是为了防止邻格越界
			for(int j=0;j<n_mat;j++){//j取1到n_mat-2是为了防止邻格越界
				if(mat[i][j]->empty)continue;
				Ccell&cell=*mat[i][j];
				
				cell.isCorner=false;
				cell.isCorner_rough=false;
			}
		}

	}
	calNorms()
	//计算各格的法向量
	{
		for(int i=0;i<m_mat;i++){
			for(int j=0;j<n_mat;j++){
				if(mat[i][j]->empty)continue;
				Ccell&cell=*mat[i][j];
				
				cell.calNorms();
			}
		}

	
	}
	emptyHsmooth(){//对empty==true的格进行平滑
		const int r=5;//以格为单位的平滑区域边心距
		for(int i=0;i<m_mat;i++){
			for(int j=0;j<n_mat;j++){
				if(!(*mat[i][j]).empty)continue;
				
				int count=0;//计数周围以r为边心距（以格为单位）的距形区域内非madeUp点的个数
				float avrgH=0;
				for(int I=i-r;I<=i+r;I++){
					if(I<0||I>m_mat-1)continue;
					for(int J=j-r;J<=j+r;J++){
						if(J<0||J>n_mat-1)continue;
						if(I==i&&J==j)continue;
							avrgH+=(*mat[I][J]).v[Y];
							count++;
					}
				}
				if(count==0){
					continue;
				}
				avrgH/=count;
				(*mat[i][j]).v[Y]=avrgH;

			}
		}


	
	}
	calInnerLayerID(){//为各cell计算innerLayerID值

		//首先将empty的cell的innerLayerID都置为0
		{
			for(int i=0;i<m_mat;i++){
				for(int j=0;j<n_mat;j++){
					if((*mat[i][j]).empty){
						(*mat[i][j]).innerLayerID=0;
					}
				}
			}
		}
		maxInnerLayerID=0;
		//将needcut的cell的innerLayerID都置为1
		{
			for(int i=0;i<m_mat;i++){
				for(int j=0;j<n_mat;j++){
					if(mat[i][j]->needcut){
						mat[i][j]->innerLayerID=1;
					}
				}
			}

		}
		maxInnerLayerID=1;
		//接下来求各内层的innerLayerID
		int curInnerLayerID=2;
		while(1){	
			//求innerLayerID为curInnerLayerID的cell，将其innerLayerID填充为curInnerLayerID
			//方法是找那些innerLayerID==-1(还没填充)的格，如果其相邻格中有innerLayerID==curInnerLayerID-1者，则其为所求
			bool alldone=true;//是否已全部填充完，开始假设是已经完全填充完
			for(int i=0;i<m_mat;i++){
				for(int j=0;j<n_mat;j++){
					if(mat[i][j]->innerLayerID==-1){
						alldone=false;//本次又找到了innerLayerID==-1者，不能断定已全部填充完
						//看其周围八个格中是否有innerLayerID等于curInnerLayerID-1者
						bool have=false;
						for(int I=i-1;I<=i+1;I++){
							if(I<0||I>m_mat-1)continue;
							for(int J=j-1;J<=j+1;J++){
								if(J<0||J>n_mat-1)continue;
								if(I==i&&J==j)continue;
								if(mat[I][J]->innerLayerID==curInnerLayerID-1){
									have=true;
									break;
								}
							}
							if(have==true)break;
						}//得到have
						if(have){
							mat[i][j]->innerLayerID=curInnerLayerID;
						}
					}
				}
			}
			if(alldone)break;//如果全部填充完，则退出
			curInnerLayerID++;
		}
		maxInnerLayerID=curInnerLayerID-1;//curInnerLayerID没找到填家，所以推出循环，说明最大的innerLayerID是curInnerLayerID-1
	

		
	
	}
	fillH_innerLayerID(float bottomH,float topH){//按innerLayerID填充高度

		float dH=(topH-bottomH)/maxInnerLayerID;
		for(int i=0;i<m_mat;i++){
			for(int j=0;j<n_mat;j++){
				mat[i][j]->v[Y]=bottomH+mat[i][j]->innerLayerID*dH;
				
			}
		}


	}
	
	HsmoothEx(){//按与needcut==true的cell的接近程度不同进行平滑
		int r=4;//以格为单位的平滑区域边心距
		for(int i=0;i<m_mat;i++){
			for(int j=0;j<n_mat;j++){
		//		if(mat[i][j].empty)continue;
				{
					float avrgH=0;
					float Sweight=0;
					for(int I=i-r;I<=i+r;I++){
						if(I<0||I>m_mat-1)continue;
						for(int J=j-r;J<=j+r;J++){
							if(J<0||J>n_mat-1)continue;
							if(i==I&&j==J){
								float weight=0.1;
								avrgH+=weight*mat[I][J]->v[Y];
								Sweight+=weight;
							}else if(mat[I][J]->needcut||mat[I][J]->empty){
								float MHD=fabs(I-i)+fabs(J-j);
								float weight=1.0/(MHD*MHD*MHD*MHD*MHD+0.001);
								avrgH+=weight*mat[I][J]->v[Y];
								Sweight+=weight;
							}
						}		
					}
		
					avrgH/=Sweight;
					mat[i][j]->v[Y]=avrgH;
					
					
				}

				

			}
		}

		
		
	}

	Hsmooth(float centerK)
	//对值进行平滑
	{
		//用九个格的高度值平均值代替中间格高度值
		//计算各格的vlt[Y],vrt[Y],vup[Y],vdn[Y]
		for(int i=1;i<=m_mat-2;i++){//i取1到m_mat-2是为了防止邻格越界
			for(int j=1;j<=n_mat-2;j++){//j取1到n_mat-2是为了防止邻格越界
				Ccell&cell=*mat[i][j];
				if(cell.empty)continue;
				Ccell&upcell=*mat[i-1][j];
				Ccell&dncell=*mat[i+1][j];
				Ccell&ltcell=*mat[i][j-1];
				Ccell&rtcell=*mat[i][j+1];
				Ccell&lupcell=*mat[i-1][j-1];
				Ccell&ldncell=*mat[i+1][j-1];
				Ccell&rupcell=*mat[i-1][j+1];
				Ccell&rdncell=*mat[i+1][j+1];
				double avrgH=0;
				double count=0;
				//outcomer的格对平滑无贡献，但是partoutcomer的格对平滑有贡献，因为partoutcomer的格的高度值计算的还是比较准确的
				//
				if(cell.empty){
					
				}else{
					if(!cell.isCorner&&!cell.isCorner_rough){//如果不是角点
						//按centerK规定的权计算
						avrgH+=centerK*cell.v[Y];	
						count+=centerK;
					}else{//如果是角点
						if(cell.isCorner){
							//按非常大的权来计算
							float k=500;
							avrgH+=k*cell.v[Y];
							count+=k;
						}else if(cell.isCorner_rough){
							float k=10;
							avrgH+=k*cell.v[Y];
							count+=k;
						}

					}
				}
				//
				if(upcell.empty){
					
				}else{
					avrgH+=upcell.v[Y];
					count++;
				}
				//
				if(dncell.empty){
					
				}else{
					avrgH+=dncell.v[Y];
					count++;
				}
				//
				if(ltcell.empty){
					
				}else{
					avrgH+=ltcell.v[Y];
					count++;
				}
				//
				if(rtcell.empty){
					
				}else{
					avrgH+=rtcell.v[Y];
					count++;
				}
				//
				if(lupcell.empty){
					
				}else{
					avrgH+=lupcell.v[Y];
					count++;
				}
				//
				if(ldncell.empty){
					
				}else{
					avrgH+=ldncell.v[Y];
					count++;
				}
				//
				if(rupcell.empty){
					
				}else{
					avrgH+=rupcell.v[Y];
					count++;
				}
				//
				if(rdncell.empty){
					
				}else{
					avrgH+=rdncell.v[Y];
					count++;
				}
				if(count==0)continue;
				avrgH/=count;
				//
				cell.v[Y]=avrgH;
				
			}
		}

		
	}

	roofsmooth(bool considerJump=true)
		//抚平
		//对每个格，根据本格及邻格中心坐标重新计算此格的Hlt,Hrt,Hup,Hdn,Hlup,Hldn,Hrup,Hrdn
	{
		
		//计算各格的vlt[Y],vrt[Y],vup[Y],vdn[Y]
		for(int i=1;i<=m_mat-2;i++){//i取1到m_mat-2是为了防止邻格越界
			for(int j=1;j<=n_mat-2;j++){//j取1到n_mat-2是为了防止邻格越界
				Ccell&cell=*mat[i][j];
				Ccell&upcell=*mat[i-1][j];
				Ccell&dncell=*mat[i+1][j];
				Ccell&ltcell=*mat[i][j-1];
				Ccell&rtcell=*mat[i][j+1];
				if(considerJump){
					if(!cell.ltJump&&!ltcell.empty)cell.vlt[Y]=(cell.v[Y]+ltcell.v[Y])/2;
					if(!cell.rtJump&&!rtcell.empty)cell.vrt[Y]=(cell.v[Y]+rtcell.v[Y])/2;
					if(!cell.upJump&&!upcell.empty)cell.vup[Y]=(cell.v[Y]+upcell.v[Y])/2;
					if(!cell.dnJump&&!dncell.empty)cell.vdn[Y]=(cell.v[Y]+dncell.v[Y])/2;
				}else{
					if(!ltcell.empty)cell.vlt[Y]=(cell.v[Y]+ltcell.v[Y])/2;
					if(!rtcell.empty)cell.vrt[Y]=(cell.v[Y]+rtcell.v[Y])/2;
					if(!upcell.empty)cell.vup[Y]=(cell.v[Y]+upcell.v[Y])/2;
					if(!dncell.empty)cell.vdn[Y]=(cell.v[Y]+dncell.v[Y])/2;
				
				}
			}
		}
		//计算各格的vlup[Y],vldn[Y],vrup[Y],vrdn[Y]（这一步必须在全部计算完vlt[Y],vrt[Y],vup[Y],vdn[Y]之后进行）
		for(i=1;i<=m_mat-2;i++){//i取1到m_mat-2是为了防止邻格越界
			for(int j=1;j<=n_mat-2;j++){//j取1到n_mat-2是为了防止邻格越界
				Ccell&cell=*mat[i][j];
				Ccell&upcell=*mat[i-1][j];
				Ccell&dncell=*mat[i+1][j];
				Ccell&ltcell=*mat[i][j-1];
				Ccell&rtcell=*mat[i][j+1];
				Ccell&lupcell=*mat[i-1][j-1];
				Ccell&ldncell=*mat[i+1][j-1];
				Ccell&rupcell=*mat[i-1][j+1];
				Ccell&rdncell=*mat[i+1][j+1];
				//看上、下、左、右、左上、左下、右上、右下邻格是否算数
				bool upcount;//上邻格是否算数
				bool dncount;//下邻格是否算数
				bool ltcount;//左邻格是否算数
				bool rtcount;//右邻格是否算数
				if(considerJump){
					upcount=(!mat[i][j]->upJump&&!upcell.empty);
					dncount=(!mat[i][j]->dnJump&&!dncell.empty);
					ltcount=(!mat[i][j]->ltJump&&!ltcell.empty);
					rtcount=(!mat[i][j]->rtJump&&!rtcell.empty);
				}else{
					upcount=(!upcell.empty);
					dncount=(!dncell.empty);
					ltcount=(!ltcell.empty);
					rtcount=(!rtcell.empty);

				}
				//计算cell.vlup[Y]
				//考查upcount,ltcount的状态
				if(!upcount&&!ltcount){//00
					//  无
					//无[]
					//cell.vlup[Y]保持不变
				}else if(!upcount&&ltcount){//01
					//  无
					//有[]
					//此时还要考虑左上格对左格是否算数
					if(ltcell.upJump){//左上格对左格不算数
						//则仅由左格计算cell.vlup[Y]
						cell.vlup[Y]=(ltcell.vup[Y]+cell.vup[Y])/2;
					}else{//左上格对左格算数
						//则应由左格和左上格共同计算cell.vlup[Y]
						cell.vlup[Y]=(ltcell.vup[Y]+cell.vup[Y]+lupcell.vrt[Y]+cell.vlt[Y])/4;
					}
					
				}else if(upcount&&!ltcount){//10
					//  有
					//无[]
					//此时还要考虑左上格对上格是否算数
					if(upcell.ltJump){//左上格对上格不算数
						//则仅由上格计算cell.vlup[Y]
						cell.vlup[Y]=(upcell.vlt[Y]+cell.vlt[Y])/2;
					}else{//左上格对上格算数
						//则应由上格和左上格共同计算cell.vlup[Y]
						cell.vlup[Y]=(upcell.vlt[Y]+cell.vlt[Y]+lupcell.vdn[Y]+cell.vup[Y])/4;

					}
				}else if(upcount&&ltcount){//11
					//  有
					//有[]
					cell.vlup[Y]=(ltcell.vup[Y]+ltcell.vrt[Y]+upcell.vlt[Y]+upcell.vdn[Y])/4;
					
				}
				//计算cell.vldn[Y]
				//考查dncount,ltcount的状态
				if(!dncount&&!ltcount){//00
					//无[]
					//  无
					//cell.vldn[Y]保持不变
				}else if(!dncount&&ltcount){//01
					//有[]
					//  无
					//此时还要考虑左下格对左格是否算数
					if(ltcell.dnJump){//左下格对左格不算数
						//则仅由左格计算cell.vldn[Y]
						cell.vldn[Y]=(ltcell.vdn[Y]+cell.vdn[Y])/2;
					}else{//左下格对左格算数
						//则应由左格和左下格共同计算cell.vldn[Y]
						cell.vldn[Y]=(ltcell.vdn[Y]+cell.vdn[Y]+ldncell.vrt[Y]+cell.vlt[Y])/4;
					}
				}else if(dncount&&!ltcount){//10
					//无[]
					//  有
					//此时还要考虑左下格对下格是否算数
					if(dncell.ltJump){//左下格对下格不算数
						//则仅由下格计算cell.vldn[Y]
						cell.vldn[Y]=(dncell.vlt[Y]+cell.vlt[Y])/2;
					}else{//左下格对下格算数
						//则由下格和左下格共同计算cell.vldn[Y]
						cell.vldn[Y]=(dncell.vlt[Y]+cell.vlt[Y]+ldncell.vup[Y]+cell.vdn[Y])/4;
					}
				}else if(dncount&&ltcount){//11
					//有[]
					//  有
					cell.vldn[Y]=(ltcell.vdn[Y]+ltcell.vrt[Y]+dncell.vlt[Y]+dncell.vup[Y])/4;
					
				}
				
				
				//计算cell.vrup[Y]
				//考查upcount,rtcount的状态
				if(!upcount&&!rtcount){//00
					//无
					//[]无
					//cell.vrup[Y]保持不变
				}else if(!upcount&&rtcount){//01
					//无
					//[]有
					//此时还要考虑右上格对右格是否算数
					if(rtcell.upJump){//右上格对右格不算数
						//则仅由右格计算cell.vrup[Y]
						cell.vrup[Y]=(cell.vup[Y]+rtcell.vup[Y])/2;
					}else{//右上格对右格算数
						//则应由右格和右上格共同计算cell.vrup[Y]
						cell.vrup[Y]=(cell.vup[Y]+rtcell.vup[Y]+rupcell.vlt[Y]+cell.vrt[Y])/4;
					}
				}else if(upcount&&!rtcount){//10
					//有
					//[]无
					//此时还要考虑右上格对上格是否算数
					if(upcell.rtJump){//右上格对上格不算数
						//则仅由上格计算cell.vrup[Y]
						cell.vrup[Y]=(upcell.vrt[Y]+cell.vrt[Y])/2;
					}else{//右上格对上格算数
						//则应由上格和右上格共同计算cell.vrup[Y]
						cell.vrup[Y]=(upcell.vrt[Y]+cell.vrt[Y]+rupcell.vdn[Y]+cell.vup[Y])/4;
					}
				}else if(upcount&&rtcount){//11
					//有
					//[]有
					cell.vrup[Y]=(upcell.vdn[Y]+upcell.vrt[Y]+rtcell.vlt[Y]+rtcell.vup[Y])/4;
					
				}
				
				//计算cell.vrdn[Y]
				//考查dncount,rtcount的状态
				if(!dncount&&!rtcount){//00
					//[]无
					//无
					//cell.vrdn[Y]保持不变
				}else if(!dncount&&rtcount){//01
					//[]有
					//无
					//此时还要考虑右下格对右格是否算数
					if(rtcell.dnJump){//右下格对右格不算数
						//则仅由右格计算cell.vrdn[Y]
						cell.vrdn[Y]=(cell.vdn[Y]+rtcell.vdn[Y])/2;
					}else{//右下格对右格算数
						//则应由右格和右下格共内计算cell.vrdn[Y]
						cell.vrdn[Y]=(cell.vdn[Y]+rtcell.vdn[Y]+rdncell.vlt[Y]+cell.vrt[Y])/4;
					}
				}else if(dncount&&!rtcount){//10
					//[]无
					//有
					//此时还要考虑右下格对下格是否算数
					if(dncell.rtJump){//右下格对下格不算数
						//则仅由下格计算cell.vrdn[Y]
						cell.vrdn[Y]=(cell.vrt[Y]+dncell.vrt[Y])/2;
					}else{//右下格对下格算数
						//则应由下格和右下格共同计算cell.vrdn[Y]
						cell.vrdn[Y]=(cell.vrt[Y]+dncell.vrt[Y]+rdncell.vup[Y]+cell.vdn[Y])/4;
					}
				}else if(dncount&&rtcount){//11
					//[]有
					//有
					cell.vrdn[Y]=(rtcell.vlt[Y]+rtcell.vdn[Y]+dncell.vup[Y]+dncell.vrt[Y])/4;
				}
				
			}
		}



	}
	markJump(float threshold_dH=1,float threshold2_dH=1.2)
		//标记各格的上下左右邻各的阶跃情况，即填充各格的ltJump,rtJump,upJump,dnJump
		//threshold_dH阶跃高度差阈值
		//threshold2_dH斜对角邻格高度差阈值
	{
		
		
		//计算各格的ltJump,rtJump,upJump,dnJump
		for(int i=0;i<=m_mat-1;i++){
			for(int j=0;j<=n_mat-1;j++){
				Ccell&cell=*mat[i][j];
				
				if(j-1>=0){
					Ccell&ltcell=*mat[i][j-1];
					cell.ltdH=cell.v[Y]-ltcell.v[Y];
					if(fabs(cell.ltdH)>=threshold_dH)cell.ltJump=true;
				}
				if(j+1<=n_mat-1){
					Ccell&rtcell=*mat[i][j+1];
					cell.rtdH=cell.v[Y]-rtcell.v[Y];
					if(fabs(cell.rtdH)>=threshold_dH)cell.rtJump=true;
				}
				if(i-1>=0){
					Ccell&upcell=*mat[i-1][j];
					cell.updH=cell.v[Y]-upcell.v[Y];
					if(fabs(cell.updH)>=threshold_dH)cell.upJump=true;
				
				}
				if(i+1<=m_mat-1){
					Ccell&dncell=*mat[i+1][j];
					cell.dndH=cell.v[Y]-dncell.v[Y];
					if(fabs(cell.dndH)>=threshold_dH)cell.dnJump=true;
				
				}
				if(i-1>=0&&j-1>=0){
					Ccell&lupcell=*mat[i-1][j-1];
					cell.lupdH=cell.v[Y]-lupcell.v[Y];
					if(fabs(cell.lupdH)>=threshold2_dH)cell.lupJump=true;
				
				}
				if(i-1>=0&&j+1<=n_mat-1){
					Ccell&rupcell=*mat[i-1][j+1];
					cell.rupdH=cell.v[Y]-rupcell.v[Y];
					if(fabs(cell.rupdH)>=threshold2_dH)cell.rupJump=true;
				}
				if(i+1<=m_mat-1&&j-1>=0){
					Ccell&ldncell=*mat[i+1][j-1];
					cell.ldndH=cell.v[Y]-ldncell.v[Y];
					if(fabs(cell.ldndH)>=threshold2_dH)cell.ldnJump=true;
				}
				if(i+1<=m_mat-1&&j+1<=n_mat-1){
					Ccell&rdncell=*mat[i+1][j+1];
					cell.rdndH=cell.v[Y]-rdncell.v[Y];
					if(fabs(cell.rdndH)>=threshold2_dH)cell.rdnJump=true;
				
				}	
				
			}
		}

	}
	transformBack(){//转换回世界空间
		for(int i=0;i<m_mat;i++){
			for(int j=0;j<n_mat;j++){
				Ccell&cell=*mat[i][j];
				//
				prodv(m_locToWorld,cell.v,cell.v);
				//
				prodv(m_locToWorld,cell.vup,cell.vup);
				//
				prodv(m_locToWorld,cell.vdn,cell.vdn);
				//
				prodv(m_locToWorld,cell.vlt,cell.vlt);
				//
				prodv(m_locToWorld,cell.vrt,cell.vrt);
				//
				prodv(m_locToWorld,cell.vlup,cell.vlup);
				//
				prodv(m_locToWorld,cell.vldn,cell.vldn);
				//
				prodv(m_locToWorld,cell.vrup,cell.vrup);
				//
				prodv(m_locToWorld,cell.vrdn,cell.vrdn);

			}
		}
		


	
	}
	initOcH()
		//更新周围八个边界点的高度值，让其与中心点高度值相同
		//仅当中心格高度发生本质变化才后才需要重新initOcH，如果只是对中心格高度进行一些平滑，则不要再initOcH
	{
			float hfdz=zstepLen/2;
		float hfdx=xstepLen/2;
		for(int i=0;i<m_mat;i++){
			for(int j=0;j<n_mat;j++){
				Ccell&cell=*mat[i][j];
				//
				cell.vup[Y]=cell.v[Y];
				//
				cell.vdn[Y]=cell.v[Y];
				//
				cell.vlt[Y]=cell.v[Y];
				//
				cell.vrt[Y]=cell.v[Y];
				//
				cell.vlup[Y]=cell.v[Y];
				//
				cell.vldn[Y]=cell.v[Y];
				//
				cell.vrup[Y]=cell.v[Y];
				//
				cell.vrdn[Y]=cell.v[Y];

			}
		}
		
	
	}


	int makeUp_once()
		//对缺失元素进行一趟修补（用均值进行修补）
		//返回去趟修补的cell个数
	{
		int count=0;
		for(int i=0;i<m_mat;i++){
			for(int j=0;j<n_mat;j++){
				Ccell&cell=*mat[i][j];
				if(cell.empty){//对partoutcomer和outcomer也进行修补
					//看其八邻格，将八邻格中非空的H值取平均填入mat[i][j]
					//mat[i-1][j-1]  mat[i-1][j]  mat[i-1][j+1]
					//mat[i][j-1]                 mat[i][j+1]
					//mat[i+1][j-1]  mat[i+1][j]  mat[i+1][j+1]
					myvector<float> HList;//八邻格中有效H值列表
					for(int I=i-1;I<=i+1;I++){
						if(I<0||I>m_mat-1)continue;
						for(int J=j-1;J<=j+1;J++){
							if(J<0||J>n_mat-1)continue;
							if(I==i&&J==j)continue;
							if(mat[I][J]->empty)continue;
							HList.push_back(mat[I][J]->v[Y]);
						}
					}//得到HList
					if((int)HList.size()==0){
						continue;
					}
					//求HList的平均高度
					float avrgH=0;
					int nH=(int)HList.size();
					for(int k=0;k<nH;k++){
						avrgH+=HList[k];
					}
					avrgH/=(int)HList.size();
					cell.v[Y]=avrgH;
					cell.empty=false;
					cell.ismadeup=true;
					count++;
				}
			}
		}
		return count;
	}
	makeUp_lowest()
		//对缺失元素进行修补（用最低点进行修补）
	{
		for(int i=0;i<m_mat;i++){
			for(int j=0;j<n_mat;j++){
				Ccell&cell=*mat[i][j];
				if(cell.empty){//对partoutcomer和outcomer也进行修补
					//看其八邻格，将八邻格中非空的H值取平均填入mat[i][j]
					//mat[i-1][j-1]  mat[i-1][j]  mat[i-1][j+1]
					//mat[i][j-1]                 mat[i][j+1]
					//mat[i+1][j-1]  mat[i+1][j]  mat[i+1][j+1]
					myvector<float> HList;//八邻格中有效H值列表
					for(int I=i-1;I<=i+1;I++){
						if(I<0||I>m_mat-1)continue;
						for(int J=j-1;J<=j+1;J++){
							if(J<0||J>n_mat-1)continue;
							if(I==i&&J==j)continue;
							if(mat[I][J]->empty)continue;
							HList.push_back(mat[I][J]->v[Y]);
						}
					}//得到HList
					if((int)HList.size()==0){
						continue;
					}
					//求HList的最低点
					float minH=inf;
					int nH=(int)HList.size();
					for(int k=0;k<nH;k++){
						if(HList[k]<minH){
							minH=HList[k];
						}
					}
					
					cell.v[Y]=minH;
					cell.empty=false;
					cell.ismadeup=true;
				}
			}
		}
	}

	makeGrid(float _xmin,float _xmax,float _zmin,float _zmax,//范围
		float _xstepLen,float _zstepLen,//步长
		myvector<Cv4>&_vlist//点云
		){//在_xmin,_xmax,_zmin,_zmax范围内为点云_vlist建立网格，网格步长约为_xstepLen,_zstepLen
		xstepLen=_xstepLen;
		zstepLen=_zstepLen;
		xmin=_xmin;
		xmax=_xmax;
		zmin=_zmin;
		zmax=_zmax;
		//设定网格尺寸
		m_mat=(_zmax-_zmin)/zstepLen+1;
		n_mat=(_xmax-_xmin)/xstepLen+1;
		//生成网格
		mat.resize(m_mat);
		for(int i=0;i<m_mat;i++){
			mat[i].resize(n_mat);
		}
		//生成网格元素
		{
			for(i=0;i<m_mat;i++){
				for(int j=0;j<n_mat;j++){
					mat[i][j]=new Ccell();
				}
			}

		}
		//将_vlist中的所有点分发到网格
		int _nv=(int)_vlist.size();
		for(i=0;i<_nv;i++){
			Cv4 v=_vlist[i];
			if(v.isoutcomer)continue;//跳过outcomer点
			//看v属于哪个格
			int I,J;//v属于(I,J)格
			I=(v.v[Z]-_zmin)/zstepLen;
			J=(v.v[X]-_xmin)/xstepLen;
			if(I<0||J<0||I>m_mat-1||J>n_mat-1)continue;
			//将v加入此格
			mat[I][J]->IDList.push_back(i);
		}//mat各元素的IDList域已填充好
		//求mat各元素的v
		float hfdz=zstepLen/2;
		float hfdx=xstepLen/2;
		for(i=0;i<m_mat;i++){
			for(int j=0;j<n_mat;j++){
				Ccell&cell=*mat[i][j];
				//----求cell.v[X]和cell.v[Z]
				cell.v[X]=xmin+xstepLen*j+hfdx;
				cell.v[Z]=zmin+zstepLen*i+hfdz;
				//----求cell.v[Y]
				//求cell.IDList中点的平均值
				float avrgH=0;
				int nID=(int)cell.IDList.size();
				if(nID==0){
					cell.v[Y]=-inf;
					cell.empty=true;
					continue;
				}
				for(int k=0;k<nID;k++){
					float*v=_vlist[cell.IDList[k]].v;
					avrgH+=v[Y];
				}
				avrgH/=nID;//得到avrgH
				//
				cell.v[Y]=avrgH;
				cell.empty=false;
	
				

			}
		}
		//填充H的八个边界点vlt,vrt,vup,vdn,vlup,vldn,vrup,vrdn
		{
			float hfdz=zstepLen/2;
			float hfdx=xstepLen/2;
			for(int i=0;i<m_mat;i++){
				for(int j=0;j<n_mat;j++){
					Ccell&cell=*mat[i][j];
					//
					cell.vup[X]=cell.v[X];
					cell.vup[Z]=cell.v[Z]-hfdz;
					cell.vup[Y]=cell.v[Y];
					//
					cell.vdn[X]=cell.v[X];
					cell.vdn[Z]=cell.v[Z]+hfdz;
					cell.vdn[Y]=cell.v[Y];
					//
					cell.vlt[X]=cell.v[X]-hfdx;
					cell.vlt[Z]=cell.v[Z];
					cell.vlt[Y]=cell.v[Y];
					//
					cell.vrt[X]=cell.v[X]+hfdx;
					cell.vrt[Z]=cell.v[Z];
					cell.vrt[Y]=cell.v[Y];
					//
					cell.vlup[X]=cell.v[X]-hfdx;
					cell.vlup[Z]=cell.v[Z]-hfdz;
					cell.vlup[Y]=cell.v[Y];
					//
					cell.vldn[X]=cell.v[X]-hfdx;
					cell.vldn[Z]=cell.v[Z]+hfdz;
					cell.vldn[Y]=cell.v[Y];
					//
					cell.vrup[X]=cell.v[X]+hfdx;
					cell.vrup[Z]=cell.v[Z]-hfdz;
					cell.vrup[Y]=cell.v[Y];
					//
					cell.vrdn[X]=cell.v[X]+hfdx;
					cell.vrdn[Z]=cell.v[Z]+hfdz;
					cell.vrdn[Y]=cell.v[Y];
					
				}
			}
			
			
		}
		
		
	}
	showMesh_rough3(GLuint texture){
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, texture);
		int m=m_mat;
		int n=n_mat;
		float dx=xstepLen;
		float dz=zstepLen;
		float hfdx=dx/2;
		float hfdz=dz/2;
		for(int i=0;i<=m-2;i+=2){//i取1到m-2是为了防止邻格越界
			for(int j=0;j<=n-2;j+=2){//j取1到n-2是为了防止邻格越界
				//cell0 cell3
				//cell1 cell2
				Ccell*pcell0=mat[i][j];
				Ccell*pcell1=mat[i+1][j];
				Ccell*pcell2=mat[i+1][j+1];
				Ccell*pcell3=mat[i][j+1];
				
				if(pcell0==NULL||pcell1==NULL||pcell2==NULL||pcell3==NULL
					||pcell0->empty||pcell1->empty||pcell2->empty||pcell3->empty
					||pcell0->needcut||pcell1->needcut||pcell2->needcut||pcell3->needcut){
					if(pcell0!=NULL)pcell0->asWhole4=false;
					//一个一个地画
					Ccell*pcell[4];
					pcell[0]=pcell0;
					pcell[1]=pcell1;
					pcell[2]=pcell2;
					pcell[3]=pcell3;
					for(int k=0;k<=3;k++){
						if(pcell[k]==NULL||pcell[k]->empty)continue;
						Ccell&cell=*pcell[k];
						
						//画多边形列表
						if(cell.needcut){
							int npolygen=(int)cell.polygenList.size();
							for(int _i=0;_i<npolygen;_i++){
								Cpolygen&polygen=cell.polygenList[_i];
								if(polygen.removed)continue;
								//画polygen
								float*norm=polygen.norm;
								glBegin(GL_POLYGON);
								glNormal3fv(norm);
								int nv=(int)polygen.vlist.size();
								for(int _j=0;_j<nv;_j++){
									float*v=polygen.vlist[_j].v;
									float*u=polygen.vlist[_j].u;
									
									glTexCoord2fv(u);
									glVertex3fv(v);
									
								}
								glEnd();
							}
						}else{
							glBegin(GL_QUADS);
							glNormal3fv(cell.avrgnorm);
							glTexCoord2fv(cell.urup);glVertex3fv(cell.vrup);
							glTexCoord2fv(cell.ulup);glVertex3fv(cell.vlup);
							glTexCoord2fv(cell.uldn);glVertex3fv(cell.vldn);
							glTexCoord2fv(cell.urdn);glVertex3fv(cell.vrdn);
							glEnd();

						
						}
					}
					
				}else{
					if(pcell0!=NULL)pcell0->asWhole4=true;
					//画这四个cell组成的面，法向取中心点的法向量
					//    cell0.vlup--cell3.vrup
					//     |                  |
					//     |			      |
					//     |                  |
					//    cell1.vldn--cell2.vrdn
					glBegin(GL_QUADS);
					glNormal3fv(pcell0->avrgnorm);//为了节约计算量，就用cell0的法向量代替整个法向量了
					glTexCoord2fv(pcell3->urup);glVertex3fv(pcell3->vrup);
					glTexCoord2fv(pcell0->ulup);glVertex3fv(pcell0->vlup);
					glTexCoord2fv(pcell1->uldn);glVertex3fv(pcell1->vldn);
					glTexCoord2fv(pcell2->urdn);glVertex3fv(pcell2->vrdn);
					glEnd();
				}


	
			}
		}
			//修补裂缝
		glBegin(GL_TRIANGLES);
		for(i=0;i<=m-2;i+=2){//i取1到m-2是为了防止邻格越界
			for(int j=0;j<=n-2;j+=2){//j取1到n-2是为了防止邻格越界
				//cell0 cell3
				//cell1 cell2
				Ccell*pcell0=mat[i][j];
				Ccell*pcell1=mat[i+1][j];
				Ccell*pcell2=mat[i+1][j+1];
				Ccell*pcell3=mat[i][j+1];
				//    cell0.vlup--cell0.vrup---cell3.vrup
				//     |   cell0      |    cell3     |
				//    cell0.vldn--cell0.vrdn---cell3.vrdn
				//     |   cell1      |    cell2     |
				//    cell1.vldn--cell1.vrdn---cell2.vrdn
				if(pcell0==NULL||!pcell0->asWhole4){//如果本2*2区域中各格是分着画的
					//看本2*2区域的上下左右2*2区域
					//看上2*2区域
					if(i-2>=0&&(mat[i-2][j]!=NULL&&mat[i-2][j]->asWhole4))
					{//上2*2区域是整体画的
						//    cellA.vldn--cellA.vrdn---cellB.vrdn
						//    cell0.vlup--cell0.vrup---cell3.vrup
						//     |   cell0      |    cell3     |
						//    cell0.vldn--cell0.vrdn---cell3.vrdn
						//     |   cell1      |    cell2     |
						//    cell1.vldn--cell1.vrdn---cell2.vrdn
						//则在上边界处要修补一个三角形裂缝
						Ccell*pcellA=mat[i-1][j];
						Ccell*pcellB=mat[i-1][j+1];
						//画三角形pcellB->vrdn,pcellA->vldn,pcellA->vrdn
						float norm[4];
						getNormalOfTri(pcellB->vrdn,pcellA->vldn,pcellA->vrdn,norm);
						glNormal3fv(norm);
						glTexCoord2fv(pcellB->urdn);glVertex3fv(pcellB->vrdn);
						glTexCoord2fv(pcellA->uldn);glVertex3fv(pcellA->vldn);
						glTexCoord2fv(pcellA->urdn);glVertex3fv(pcellA->vrdn);
					}
					//看下2*2区域
					if(i+2<m&&(mat[i+2][j]!=NULL&&mat[i+2][j]->asWhole4))
					{//下2*2区域是整体画的
						//    cell0.vlup--cell0.vrup---cell3.vrup
						//     |   cell0      |    cell3     |
						//    cell0.vldn--cell0.vrdn---cell3.vrdn
						//     |   cell1      |    cell2     |
						//    cell1.vldn--cell1.vrdn---cell2.vrdn
						//    cellA.vlup--cellA.vrup---cellB.vrup
						//则在下边界处要修补一个三角形裂缝
						Ccell*pcellA=mat[i+2][j];
						Ccell*pcellB=mat[i+2][j+1];
						//画三角形pcellB->vrup,pcellA->vrup,pcellA->vlup
						float norm[4];
						getNormalOfTri(pcellB->vrup,pcellA->vrup,pcellA->vlup,norm);
						glNormal3fv(norm);
						glTexCoord2fv(pcellB->urup);glVertex3fv(pcellB->vrup);
						glTexCoord2fv(pcellA->urup);glVertex3fv(pcellA->vrup);
						glTexCoord2fv(pcellA->ulup);glVertex3fv(pcellA->vlup);
					}
					//看左2*2区域
					if(j-2>=0&&(mat[i][j-2]!=NULL&&mat[i][j-2]->asWhole4))
					{//左2*2区域是整体画的
						//  cellA.vrup  cell0.vlup--cell0.vrup---cell3.vrup
						//		 |		  |   cell0      |    cell3     |
						//  cellA.vrdn  cell0.vldn--cell0.vrdn---cell3.vrdn
						//       |        |   cell1      |    cell2     |
						//  cellB.vrdn  cell1.vldn--cell1.vrdn---cell2.vrdn
						//则在左边界处要修补一个三角形裂缝
						Ccell*pcellA=mat[i][j-1];
						Ccell*pcellB=mat[i+1][j-1];
						//画三角形pcellB->vrdn,pcellA->vrdn,pcellA->vrup
						float norm[4];
						getNormalOfTri(pcellB->vrdn,pcellA->vrdn,pcellA->vrup,norm);
						glNormal3fv(norm);
						glTexCoord2fv(pcellB->urdn);glVertex3fv(pcellB->vrdn);
						glTexCoord2fv(pcellA->urdn);glVertex3fv(pcellA->vrdn);
						glTexCoord2fv(pcellA->urup);glVertex3fv(pcellA->vrup);
					}
					//看右2*2区域
					if(j+2<n&&(mat[i][j+2]!=NULL&&mat[i][j+2]->asWhole4))
					{//右2*2区域是整体画的
						//  cell0.vlup--cell0.vrup---cell3.vrup   cellA.vlup
						//	  |   cell0      |    cell3     |	       |
						//  cell0.vldn--cell0.vrdn---cell3.vrdn   cellA.vldn 
						//    |   cell1      |    cell2     |          |
						//  cell1.vldn--cell1.vrdn---cell2.vrdn   cellB.vldn 
						//则在右边界处要修补一个三角形裂缝
						Ccell*pcellA=mat[i][j+2];
						Ccell*pcellB=mat[i+1][j+2];
						//画三角形pcellA->vlup,pcellA->vldn,pcellB->vldn
						float norm[4];
						getNormalOfTri(pcellA->vlup,pcellA->vldn,pcellB->vldn,norm);
						glNormal3fv(norm);
						glTexCoord2fv(pcellA->ulup);glVertex3fv(pcellA->vlup);
						glTexCoord2fv(pcellA->uldn);glVertex3fv(pcellA->vldn);
						glTexCoord2fv(pcellB->uldn);glVertex3fv(pcellB->vldn);
					}
				}

	
			}
		}
		glEnd();
		glDisable(GL_TEXTURE_2D);

	}
	showMesh_rough4(GLuint texture){//与showMesh_rough3的区别是不修补裂缝
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, texture);
		int m=m_mat;
		int n=n_mat;
		float dx=xstepLen;
		float dz=zstepLen;
		float hfdx=dx/2;
		float hfdz=dz/2;
		for(int i=0;i<=m-2;i+=2){//i取1到m-2是为了防止邻格越界
			for(int j=0;j<=n-2;j+=2){//j取1到n-2是为了防止邻格越界
				//cell0 cell3
				//cell1 cell2
				Ccell*pcell0=mat[i][j];
				Ccell*pcell1=mat[i+1][j];
				Ccell*pcell2=mat[i+1][j+1];
				Ccell*pcell3=mat[i][j+1];
				
				if(pcell0==NULL||pcell1==NULL||pcell2==NULL||pcell3==NULL
					||pcell0->empty||pcell1->empty||pcell2->empty||pcell3->empty
					||pcell0->needcut||pcell1->needcut||pcell2->needcut||pcell3->needcut){
					if(pcell0!=NULL)pcell0->asWhole4=false;
					//一个一个地画
					Ccell*pcell[4];
					pcell[0]=pcell0;
					pcell[1]=pcell1;
					pcell[2]=pcell2;
					pcell[3]=pcell3;
					for(int k=0;k<=3;k++){
						if(pcell[k]==NULL||pcell[k]->empty)continue;
						Ccell&cell=*pcell[k];
						
						//画多边形列表
						if(cell.needcut){
							int npolygen=(int)cell.polygenList.size();
							for(int _i=0;_i<npolygen;_i++){
								Cpolygen&polygen=cell.polygenList[_i];
								if(polygen.removed)continue;
								//画polygen
								float*norm=polygen.norm;
								glBegin(GL_POLYGON);
								glNormal3fv(norm);
								int nv=(int)polygen.vlist.size();
								for(int _j=0;_j<nv;_j++){
									float*v=polygen.vlist[_j].v;
									float*u=polygen.vlist[_j].u;
									
									glTexCoord2fv(u);
									glVertex3fv(v);
									
								}
								glEnd();
							}
						}else{
							glBegin(GL_QUADS);
							glNormal3fv(cell.avrgnorm);
							glTexCoord2fv(cell.urup);glVertex3fv(cell.vrup);
							glTexCoord2fv(cell.ulup);glVertex3fv(cell.vlup);
							glTexCoord2fv(cell.uldn);glVertex3fv(cell.vldn);
							glTexCoord2fv(cell.urdn);glVertex3fv(cell.vrdn);
							glEnd();

						
						}
					}
					
				}else{
					if(pcell0!=NULL)pcell0->asWhole4=true;
					//画这四个cell组成的面，法向取中心点的法向量
					//    cell0.vlup--cell3.vrup
					//     |                  |
					//     |			      |
					//     |                  |
					//    cell1.vldn--cell2.vrdn
					glBegin(GL_QUADS);
					glNormal3fv(pcell0->avrgnorm);//为了节约计算量，就用cell0的法向量代替整个法向量了
					glTexCoord2fv(pcell3->urup);glVertex3fv(pcell3->vrup);
					glTexCoord2fv(pcell0->ulup);glVertex3fv(pcell0->vlup);
					glTexCoord2fv(pcell1->uldn);glVertex3fv(pcell1->vldn);
					glTexCoord2fv(pcell2->urdn);glVertex3fv(pcell2->vrdn);
					glEnd();
				}


	
			}
		}
		glEnd();
		glDisable(GL_TEXTURE_2D);

	}

	showMesh_rough5(GLuint texture,bool drawNeedCut=true){
			glEnable(GL_TEXTURE_2D);
					glBindTexture(GL_TEXTURE_2D, texture);
		int m=m_mat;
		int n=n_mat;
		float dx=xstepLen;
		float dz=zstepLen;
		float hfdx=dx/2;
		float hfdz=dz/2;
		glBegin(GL_QUADS);
		for(int i=0;i<=m-2;i+=2){//i取1到m-2是为了防止邻格越界
			for(int j=0;j<=n-2;j+=2){//j取1到n-2是为了防止邻格越界
				//cell0 cell3
				//cell1 cell2
				Ccell*pcell0=mat[i][j];
				Ccell*pcell1=mat[i+1][j];
				Ccell*pcell2=mat[i+1][j+1];
				Ccell*pcell3=mat[i][j+1];
				
				if(pcell0==NULL||pcell1==NULL||pcell2==NULL||pcell3==NULL
					||pcell0->empty||pcell1->empty||pcell2->empty||pcell3->empty
					||pcell0->needcut||pcell1->needcut||pcell2->needcut||pcell3->needcut){
					if(pcell0!=NULL)pcell0->asWhole4=false;
					//一个一个地画
					Ccell*pcell[4];
					pcell[0]=pcell0;
					pcell[1]=pcell1;
					pcell[2]=pcell2;
					pcell[3]=pcell3;
					for(int k=0;k<=3;k++){
						if(pcell[k]==NULL||pcell[k]->empty)continue;
						Ccell&cell=*pcell[k];
						if(!drawNeedCut&&cell.needcut)continue;//如果不画needcut格且此格恰为needcut，则跳过
						
						glNormal3fv(cell.avrgnorm);
						glTexCoord2fv(cell.urup);glVertex3fv(cell.vrup);
						glTexCoord2fv(cell.ulup);glVertex3fv(cell.vlup);
						glTexCoord2fv(cell.uldn);glVertex3fv(cell.vldn);
						glTexCoord2fv(cell.urdn);glVertex3fv(cell.vrdn);

					}
					
				}else{
					if(pcell0!=NULL)pcell0->asWhole4=true;
					//画这四个cell组成的面，法向取中心点的法向量
					//    cell0.vlup--cell3.vrup
					//     |                  |
					//     |			      |
					//     |                  |
					//    cell1.vldn--cell2.vrdn
					glNormal3fv(pcell0->avrgnorm);//为了节约计算量，就用cell0的法向量代替整个法向量了
					glTexCoord2fv(pcell3->urup);glVertex3fv(pcell3->vrup);
					glTexCoord2fv(pcell0->ulup);glVertex3fv(pcell0->vlup);
					glTexCoord2fv(pcell1->uldn);glVertex3fv(pcell1->vldn);
					glTexCoord2fv(pcell2->urdn);glVertex3fv(pcell2->vrdn);
				}


	
			}
		}
		
		glEnd();
		glDisable(GL_TEXTURE_2D);

	}
	showMesh_rough0(GLuint texture,bool enableTexture=true,float r=1,float g=1,float b=1,float a=1){
		if(enableTexture){
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, texture);
		}
		int m=m_mat;
		int n=n_mat;
		float dx=xstepLen;
		float dz=zstepLen;
		float hfdx=dx/2;
		float hfdz=dz/2;
		for(int i=0;i<m;i++){//i取1到m-2是为了防止邻格越界
			for(int j=0;j<n;j++){//j取1到n-2是为了防止邻格越界
				if(mat[i][j]==NULL)continue;
				Ccell&cell=*mat[i][j];//当前格
				if(cell.empty)continue;
				cell.show_rough0(r,g,b,a);
			}
		}
		if(enableTexture){
			glDisable(GL_TEXTURE_2D);
		}
	}



};

//-------------------------------------------------------
//模型基类
class Cmodel{
public:
	float RMmat[16];//旋转平移矩阵
	Cmodel(){
		makeE(RMmat);
	}
	reset(){//复位
		makeE(RMmat);
	
	}
	virtual void show(){
	
	}
	void setloc(float x,float y,float z){
		RMmat[12]=x;
		RMmat[13]=y;
		RMmat[14]=z;
	
	}

	void movloc(float dx,float dy,float dz){//相对于局部坐标系移动局部坐标
		//制作d所对应的平移矩阵
		float movmat[16]={
			1,0,0,0,//第一列
            0,1,0,0,//第二列
			0,0,1,0,//第三列
			dx,dy,dz,1//第四列
		};
		prodm(RMmat,movmat,RMmat);

	}
	void rotloc(float nx,float ny,float nz,float tha){//相对于自身旋转局部坐标
		//n必须是单位向量
		float cosa=cos(tha*pi/180);
		float sina=sin(tha*pi/180);
		float _cosa=1-cosa;
		if(nx==0&&nz==0){
			float rotmat[16]={
				cosa,0,-sina,0,//第一列
				0,1,0,0,//第二列
				sina,0,cosa,0,//第三列
				0,0,0,1//第四列
			};
			prodm(RMmat,rotmat,RMmat);
		}else{
			float rotmat[16]={
				nx*nx*_cosa+cosa,nx*ny*_cosa+nz*sina,nx*nz*_cosa-ny*sina,0,//col1
				nx*ny*_cosa-nz*sina,ny*ny*_cosa+cosa,ny*nz*_cosa+nx*sina,0,//col2
				nx*nz*_cosa+ny*sina,ny*nz*_cosa-nx*sina,nz*nz*_cosa+cosa,0,//col3
				0,0,0,1//col4
			};
			prodm(RMmat,rotmat,RMmat);
		}
		
	}
	void rotloc_inFather(float nx,float ny,float nz,float tha){//在父坐标系中旋转局部坐标系
		//n必须是单位向量
		float cosa=cos(tha*pi/180);
		float sina=sin(tha*pi/180);
		float _cosa=1-cosa;
		if(nx==0&&nz==0){
			float rotmat[16]={
				cosa,0,-sina,0,//第一列
				0,1,0,0,//第二列
				sina,0,cosa,0,//第三列
				0,0,0,1//第四列
			};
			prodm(rotmat,RMmat,RMmat);
		}else{
			float rotmat[16]={
				nx*nx*_cosa+cosa,nx*ny*_cosa+nz*sina,nx*nz*_cosa-ny*sina,0,//col1
				nx*ny*_cosa-nz*sina,ny*ny*_cosa+cosa,ny*nz*_cosa+nx*sina,0,//col2
				nx*nz*_cosa+ny*sina,ny*nz*_cosa-nx*sina,nz*nz*_cosa+cosa,0,//col3
				0,0,0,1//col4
			};
			prodm(rotmat,RMmat,RMmat);
		}
		
		
	}
	void movloc_inFather(float dx,float dy,float dz){//在父坐标系中移动局部坐标系
		//制作d所对应的平移矩阵
		float movmat[16]={
			1,0,0,0,//第一列
            0,1,0,0,//第二列
			0,0,1,0,//第三列
			dx,dy,dz,1//第四列
		};
		prodm(movmat,RMmat,RMmat);
		
		
	}
	toWorld(float x,float y,float z,float type,float v[4]){//将局部坐标(x,y,z,type)转化到世界坐标v
		//type:1表点0表向量
		float _v[4]={x,y,z,type};
		toWorld(_v,v);
	}
	toWorld(float _v[4],float v[4]){//将局部坐标_v转化到世界坐标v
		prodv(RMmat,_v,v);
	
	}

};
//观察者
class Cme:public Cmodel
{
public:
	void show(){
		glPushMatrix();
		glMultMatrixf(RMmat);

		
		//----画立方体
		//    vup1(-d,d,-d) ________  vup0(d,d,-d)
		//                 /|      /|
		//                / |     / |
		// vup2(-d,d,d)  |--+----|  | vup3(d,d,d) 
		//  vdn1(-d,0,-d)|  /----|--/ vdn0(d,0,-d)
		//               | /     | /
		//               |/______|/ 
		// vdn2(-d,0,d)           vdn3(d,0,d)
		float d=0.6;//边长一半
		float vup0[4],vup1[4],vup2[4],vup3[4],vdn0[4],vdn1[4],vdn2[4],vdn3[4];//八个顶点
		init4(vup0,d,d,-d,1);
		init4(vup1,-d,d,-d,1);
		init4(vup2,-d,d,d,1);
		init4(vup3,d,d,d,1);
		init4(vdn0,d,0,-d,1);
		init4(vdn1,-d,0,-d,1);
		init4(vdn2,-d,0,d,1);
		init4(vdn3,d,0,d,1);
		float norm_up[4],norm_dn[4],norm_lt[4],norm_rt[4],norm_ft[4],norm_bk[4];//六个面的法向量
		init4(norm_up,0,1,0,0);
		init4(norm_dn,0,-1,0,0);
		init4(norm_lt,-1,0,0,0);
		init4(norm_rt,1,0,0,0);
		init4(norm_ft,0,0,1,0);
		init4(norm_bk,0,0,-1,0);
		//画立方体
		//	glEnable(GL_TEXTURE_2D);
		//	glBindTexture(GL_TEXTURE_2D, texture[0]);
		glBegin(GL_QUADS);
		//上面
		glNormal3fv(norm_up);
		glTexCoord2f(1,1);glVertex3fv(vup0);
		glTexCoord2f(0,1);glVertex3fv(vup1);
		glTexCoord2f(0,0);glVertex3fv(vup2);
		glTexCoord2f(1,0);glVertex3fv(vup3);
		//下面
		glNormal3fv(norm_dn);
		glTexCoord2f(1,0);glVertex3fv(vdn3);
		glTexCoord2f(0,0);glVertex3fv(vdn2);
		glTexCoord2f(0,1);glVertex3fv(vdn1);
		glTexCoord2f(1,1);glVertex3fv(vdn0);
		//前面
		glNormal3fv(norm_ft);
		glTexCoord2f(1,1);glVertex3fv(vup3);
		glTexCoord2f(0,1);glVertex3fv(vup2);
		glTexCoord2f(0,0);glVertex3fv(vdn2);
		glTexCoord2f(1,0);glVertex3fv(vdn3);
		//后面
		glNormal3fv(norm_bk);
		glTexCoord2f(1,0);glVertex3fv(vdn0);
		glTexCoord2f(0,0);glVertex3fv(vdn1);
		glTexCoord2f(0,1);glVertex3fv(vup1);
		glTexCoord2f(1,1);glVertex3fv(vup0);
		//左面
		glNormal3fv(norm_lt);
		glTexCoord2f(0,0);glVertex3fv(vdn2);
		glTexCoord2f(0,1);glVertex3fv(vup2);
		glTexCoord2f(1,1);glVertex3fv(vup1);
		glTexCoord2f(1,0);glVertex3fv(vdn1);
		//右面
		glNormal3fv(norm_rt);
		glTexCoord2f(1,0);glVertex3fv(vdn0);
		glTexCoord2f(1,1);glVertex3fv(vup0);
		glTexCoord2f(0,1);glVertex3fv(vup3);
		glTexCoord2f(0,0);glVertex3fv(vdn3);
		glEnd();
		
		//	glDisable(GL_TEXTURE_2D);
		glPopMatrix();

	}


};





class Cpatch{
public:
	myvector<CIDTriangle*> pIDtriList;//索引三角形指针列表
	~Cpatch(){
		//将pIDtriList中动态申请的对象删除
		{
			int nIDtri=(int)pIDtriList.size();
			for(int j=0;j<nIDtri;j++){
				CIDTriangle*pIDtri=pIDtriList[j];
				//删除pIDtri
				delete pIDtri;
			}
		}
		
	}
};

class CIDpolygen{//索引多边形
public:
	myvector<int> vIDList;//顶点索引列表
};

class CIDedge{//索引边
public:
	int ID1;//起点索引
	int ID2;//终点索引
	bool added;
	CIDedge(){
		added=false;
	}
	setvID(int _ID1,int _ID2){
		ID1=_ID1;
		ID2=_ID2;
	}
};

class CIDloosePolygen{//索引松散多边形
public:
	myvector<CIDedge> IDedgeList;//一个松散多边形由多个索引边组成
};
class Crect{
public:
	float origin[4];//原点
	float xdir[4];//局部x方向向量
	float zdir[4];//局部z方向向量
	float xmin,xmax,zmin,zmax;
	Crect(){
		init4(origin,0,0,0,1);
		xmin=0;
		xmax=0;
		zmin=0;
		zmax=0;
		init4(xdir,0,0,0,0);
		init4(zdir,0,0,0,0);
	}
	show(float H){
		//画rect
		float v[4][4];//矩形4个顶点在rect坐标系下的坐标
		init4(v[0],xmin,0,zmin,1);
		init4(v[1],xmin,0,zmax,1);
		init4(v[2],xmax,0,zmax,1);
		init4(v[3],xmax,0,zmin,1);
		//将v转到世界坐标系
		//首先转到与世界坐标平行，但原点在origin的坐标系，称为“平行坐标系”
		//平行坐标系x轴在rect中的坐标为
		float xvec[4]={xdir[X],0,-xdir[Z],0};
		//平行坐标系z轴在rect中的坐标为
		float zvec[4]={xdir[Z],0,xdir[X],0};
		for(int k=0;k<=3;k++){
			float originvk[4]={v[k][X],0,v[k][Z],0};//由origin指向v[k]的向量（rect空间中）
			//将originv[k]转到平行坐标系
			//即将originv[k]向xvec和zvec投影
			float x=dot(originvk,xvec);//originv[k]在xvec上的投影，即v[k]在平行空间中的x坐标
			float z=dot(originvk,zvec);//originv[k]在zvec上的投影，即v[k]在平行空间中的z坐标
			//由平行空间转到世界空间
			x+=origin[X];
			z+=origin[Z];
			//改v[k]为世界空间坐标
			init4(v[k],x,H,z,1);
		}//得到v[4][4]的世界坐标
		//画v[4][4]所确定的矩形
		//画线
		glBegin(GL_LINES);
		glVertex3fv(v[0]);
		glVertex3fv(v[1]);
		
		glVertex3fv(v[1]);
		glVertex3fv(v[2]);
		
		glVertex3fv(v[2]);
		glVertex3fv(v[3]);
		
		glVertex3fv(v[3]);
		glVertex3fv(v[0]);
		glEnd();
		
		
	}
};



class CIDoutline{//一个IDoutline包括多个多边形
public:
	//----底面
	myvector<CIDloosePolygen> IDloosePolygenList;//松散多边形列表
	myvector<CIDpolygen> IDpolygenList;//索引多边形列表
	                                   //（注意，虽然多边形列表是由松散多边形列表生成的，
	                                   //但是二者所包含的多边形数量未必相同，可能一个松散
	                                   //多边形生成多个多边形。原因在于一个松散多边形对应
	                                   //patch的一个连通分支，而一个连通分支如果有洞的话
	                                   //则对应多个多边边界）
	myvector<Cpolygen> polygenList;//上轮廓多边形列表
	myvector<Cpolygen> polygenList_dn;//下轮廓多边形列表

};


class CIDgrid
//索引碰撞网格
{
public:
	int m_IDListmat;//IDlistmat的行数
	int n_IDListmat;//IDlistmat的列数
	float xstepLen;//x步长
	float zstepLen;//z步长
	float xmin,xmax,zmin,zmax;//xz范围
	myvector<myvector<myvector<int> > > IDListmat;//碰撞网格
	CIDgrid(){
		m_IDListmat=0;
		n_IDListmat=0;
		xstepLen=0;
		zstepLen=0;
		xmin=xmax=zmin=zmax=0;
	}
	destroy(){
		m_IDListmat=0;
		n_IDListmat=0;
		xstepLen=0;
		zstepLen=0;
		xmin=xmax=zmin=zmax=0;
		IDListmat.resize(0);

		
	}
	makeIDgrid(float _xmin,float _xmax,float _zmin,float _zmax,//范围
		float _xstepLen,float _zstepLen,//步长
		myvector<Cv4>&_vlist//点云
		)
		//在_xmin,_xmax,_zmin,_zmax范围内为点云_vlist建立网格，网格步长约为_xstepLen,_zstepLen
		//跳过墙面点
	{
		//首先清空IDListmat
		IDListmat.resize(0);
		//重设参数
		xstepLen=_xstepLen;
		zstepLen=_zstepLen;
		xmin=_xmin;
		xmax=_xmax;
		zmin=_zmin;
		zmax=_zmax;
		//设定网格尺寸
		m_IDListmat=(_zmax-_zmin)/zstepLen;
		n_IDListmat=(_xmax-_xmin)/xstepLen;
		//根据m_IDListmat和n_IDListmat校正步长
		zstepLen=(_zmax-_zmin)/m_IDListmat;
		xstepLen=(_xmax-_xmin)/n_IDListmat;
		//生成网格
		IDListmat.resize(m_IDListmat);
		for(int i=0;i<m_IDListmat;i++){
			IDListmat[i].resize(n_IDListmat);
		}
		//将_vlist中的所有点分发到网格
		int _nv=(int)_vlist.size();
		for(i=0;i<_nv;i++){
			Cv4&v=_vlist[i];
			if(v.wallpoint)continue;
			//看v属于哪个格
			int I,J;//v属于(I,J)格
			I=(v.v[Z]-_zmin)/zstepLen;
			J=(v.v[X]-_xmin)/xstepLen;
			if(I<0||J<0||I>m_IDListmat-1||J>n_IDListmat-1)continue;
			//将v加入此格
			IDListmat[I][J].push_back(i);
		}//网格已填充好
		
	}
	void getIDList_oc(float x,float z,myvector<int>&IDList)
		//获得本格及八邻格内所有顶点的ID列表
		//传入的IDList必须为空
	{
		//求(x,z)所在的格
		int I,J;//(x,z)属于(I,J)格
		I=(z-zmin)/zstepLen;
		J=(x-xmin)/xstepLen;
		if(I<0||J<0||I>m_IDListmat-1||J>n_IDListmat-1){
			return;
		}
		//将
		//IDListmat[I-1][J-1]  IDListmat[I-1][J]  IDListmat[I-1][J+1]
		//IDListmat[I][J-1]    IDListmat[I][J]    IDListmat[I][J+1]
		//IDListmat[I+1][J-1]  IDListmat[I+1][J]  IDListmat[I+1][J+1]
		//中内的点的ID存到IDList
		int _I,_J;//_I取遍历I-1,I,I+1；_J取遍J-1,J,J+1
		int Iadd1=I+1;
		int Isub1=I-1;
		int Jadd1=J+1;
		int Jsub1=J-1;
		for(_I=Isub1;_I<=Iadd1;_I++){
			if(_I<0||_I>=m_IDListmat)continue;
			for(_J=Jsub1;_J<=Jadd1;_J++){
				if(_J<0||_J>=n_IDListmat)continue;
				int nv=(int)IDListmat[_I][_J].size();
				for(int i=0;i<nv;i++){
					IDList.push_back(IDListmat[_I][_J][i]);
				}
				
			}
		}//得到IDList
		
	}
	

};
class Ccloud{
public:

	myvector<Cv4> vlist;//点列表
	myvector<Cpolygen> polygenList;//上轮廓多边形列表
	myvector<Cpolygen> polygenList_dn;//下轮廓多边形列表
	myvector<Cpolygen> polygenList_smoothRes;//平滑结果留底
	Cgrid grid;//矩形网格
	float avrgH;//屋顶平均高度
	float bottomH;//屋顶底盘高度
	float topH;//屋顶最高高度
	float minH;//房屋墙壁最低点高度
	bool removed;//是否已删除
	float color[4];//颜色
	float avrgnorm[4];//云团的平均法向量
	float xmin,xmax,zmin,zmax;//点云范围
	bool drawed;//是否已画过
	int hostID;//主人云团号
	bool selfHost;//是否为根主云团
	Crect minRect;//最小包围矩形
	Cv4 mainDir;//主方向
	bool mainDirGot;
	bool mainDirChanged;
	bool isRed;//是否以红色显示
	bool takenOut;//是否是polygen单独提出来而建成的
	Ccloud(){
		takenOut=false;
		isRed=false;
		mainDirGot=false;
		mainDirChanged=false;
		selfHost=false;
		hostID=-1;
		minH=0;
		bottomH=0;
		topH=0;
		drawed=false;
		xmin=xmax=zmin=zmax=0;
		init4(avrgnorm,0,0,0,0);
		init4(color,1,1,1,1);
		avrgH=0;
		removed=false;
	}
	~Ccloud(){
		//销毁动态部分
		grid.clearMat();

	
	}
	copy(Ccloud&cloud){//*this拷贝cloud
		//拷贝静态部分
		*this=cloud;
		//拷贝动态部分
		{
			int m=cloud.grid.m_mat;
			int n=cloud.grid.n_mat;
			for(int i=0;i<m;i++){
				for(int j=0;j<n;j++){
					if(cloud.grid.mat[i][j]==NULL){
						this->grid.mat[i][j]=NULL;
					}else{
						this->grid.mat[i][j]=new Ccell();
						*(this->grid.mat[i][j])=*(cloud.grid.mat[i][j]);
					}
				}
			}
		}
	}
	destroy(){
		grid.clearMat();
	}


};

