class Cgroundcell{
public:
    float h;
	float u[2];
};
class CgroundcellEx
{
public:
	float v[4];//中心点坐标
	float u[2];//中心点纹理坐标
	myvector<int> IDList;//落在此格中的云点的ID列表
	bool empty;//本格是否为空（即v[Y]是否已填充了有效值）
	           //最初时IDList为空则empty为true,但补充之后就不一定了，只能通过empty来判断，而不能通过IDList是否为空来判断
	bool ismadeup;//标记一个非empty格是否是由补充得来
	bool ltJump,rtJump,upJump,dnJump,lupJump,ldnJump,rupJump,rdnJump;//此格的上下左右，左上，左下，右上，右下邻格是否阶跃
	float ltdH,rtdH,updH,dndH,lupdH,ldndH,rupdH,rdndH;//此格与上下左右，左上，左下，右上，右下邻格的中心高度差（阶跃差）（本格减邻格）
	bool visited;//是否在连通测试过程中已访问过
	bool isGround;//是否为地面cell
	CgroundcellEx(){
		ismadeup=false;
		isGround=false;
		visited=false;
		ltdH=rtdH=updH=dndH=lupdH=ldndH=rupdH=rdndH=0;
		ltJump=rtJump=upJump=dnJump=lupJump=ldnJump=rupJump=rdnJump=false;
		empty=true;
	}
	clearIDList(){
		IDList.swap(myvector<int>());
	}



	
};


class Cij{
public:
	int i;
	int j;
	Cij(){
		i=0;
		j=0;
	}
	makeij(int _i,int _j){
		i=_i;
		j=_j;
	}

};
class Cijset{
public:
	myvector<Cij> ijset;
	float color[4];//颜色
	Cijset(){
		init4(color,1,1,1,1);
	}
};

class CgroundgridEx
{
public:
	int m_mat;//IDlistmat的行数
	int n_mat;//IDlistmat的列数
	float xstepLen;//x步长
	float zstepLen;//z步长
	float xmin,xmax,zmin,zmax;//xz范围
	myvector<myvector<CgroundcellEx> > mat;//cell矩阵
	//---------------------------------------------------------------------------------------------------------------
	myvector<Cijset> ijsetList;//ij集合列表
	Cijset*pgroundijset;//地面ij集
	//---------------------------------------------------------------------------------------------------
	CgroundgridEx(){
		pgroundijset=NULL;
		
	}
	~CgroundgridEx(){//析构销毁，只销毁动态部分即可
		clearGroundijset();
	
	}
	destroy(){//非析构销毁，为不影响下次使用，尽量销毁充分
		mat.resize(0);
		//mat.resize(0)后必面接下面两句
		m_mat=0;
		n_mat=0;
		xmin=xmax=zmin=zmax=0;
		ijsetList.resize(0);
		clearGroundijset();

		
	}
	clearGroundijset(){
		if(pgroundijset!=NULL){
			delete pgroundijset;
			pgroundijset=NULL;//不要忘了置为NULL
		}
	}
	float getH(const float v[4])const 
	{//求v处网格高度
		//求v在哪个格
		int I,J;//v属于(I,J)格
		I=(v[Z]-zmin)/zstepLen;
		J=(v[X]-xmin)/xstepLen;
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
		return mat[I][J].v[Y];
		
	
	}
	makeRoughGroundGrid(const CgroundgridEx&_GroundGrid,int di,int dj){//用_GroundGrid制作一个didj的粗糙版本
		//新网格参数
		m_mat=(int)(((float)_GroundGrid.m_mat)/di)+1;
		n_mat=(int)(((float)_GroundGrid.n_mat)/dj)+1;
		xstepLen=_GroundGrid.xstepLen*di;
		zstepLen=_GroundGrid.zstepLen+dj;
		xmin=_GroundGrid.xmin;//-xstepLen;
		xmax=_GroundGrid.xmax;//+xstepLen;
		zmin=_GroundGrid.zmin;//-zstepLen;
		zmax=_GroundGrid.zmax;//+zstepLen;
		//为新网格开辟空间
		{
			mat.resize(m_mat);
			for(int i=0;i<m_mat;i++){
				mat[i].resize(n_mat);
			}
		}
		//求mat各元素的v
		const myvector<myvector<CgroundcellEx> >&_mat=_GroundGrid.mat;
		float hfdz=zstepLen/2;
		float hfdx=xstepLen/2;
		for(int i=0;i<m_mat;i++){
			for(int j=0;j<n_mat;j++){
				CgroundcellEx&cell=mat[i][j];
				//----求cell.v[X]和cell.v[Z]
				cell.v[X]=xmin+xstepLen*j+hfdx;
				cell.v[Z]=zmin+zstepLen*i+hfdz;
				//----求cell.v[Y]
				
				//mat[i][j]（即cell）对应块
				//  _mat[i*di][j*dj]---------_mat[i*di][j*dj+dj-1]
				//     |                      |
				//  _mat[i*di+di-1][j*dj]----_mat[i*di+di-1][j*dj+dj-1]
				//求此块中各v[Y]的平均值作为cell.v[Y]
				float avrgH=0;
				int count=0;
				for(int _i=i*di;_i<i*di+di;_i++){
					for(int _j=j*dj;_j<j*dj+dj;_j++){
						if(_i>=_GroundGrid.m_mat||_j>=_GroundGrid.n_mat)continue;
						avrgH+=_mat[_i][_j].v[Y];
						count++;
					}
				}
				if(count!=0)avrgH/=count;
				cell.v[Y]=avrgH;
				cell.empty=false;
				cell.isGround=true;
			}
		}
	}
	caluOfEachCell(float offsetus,float offsetut)
	//为各cell中心v点计算纹理坐标u
	{
		for(int i=0;i<m_mat;i++){
			for(int j=0;j<n_mat;j++){
				if(mat[i][j].empty)continue;
				CgroundcellEx&cell=mat[i][j];
				//计算cell.u
				cell.u[0]=(cell.v[X]-xmin)/(xmax-xmin)+offsetus;
				cell.u[1]=(cell.v[Z]-zmin)/(zmax-zmin)+offsetut;

			}
		}

	}
	clearIDListForEachCell(){//清除各cell的IDList
		for(int i=0;i<m_mat;i++){
			for(int j=0;j<n_mat;j++){
				CgroundcellEx&cell=mat[i][j];
				cell.clearIDList();
			
			}
		}

	
	}
	int makeUpGround_once()
		//对缺失的地面元素进行修补
	{
		int count=0;
		for(int i=0;i<m_mat;i++){
			for(int j=0;j<n_mat;j++){
				CgroundcellEx&cell=mat[i][j];
				if(!cell.isGround){
					//看其八邻格，将八邻格中为isGround的H值取平均填入mat[i][j]
					//mat[i-1][j-1]  mat[i-1][j]  mat[i-1][j+1]
					//mat[i][j-1]                 mat[i][j+1]
					//mat[i+1][j-1]  mat[i+1][j]  mat[i+1][j+1]
					myvector<float> HList;//八邻格中有效H值列表
					for(int I=i-1;I<=i+1;I++){
						if(I<0||I>m_mat-1)continue;
						for(int J=j-1;J<=j+1;J++){
							if(J<0||J>n_mat-1)continue;
							if(I==i&&J==j)continue;
							if(mat[I][J].isGround){
								HList.push_back(mat[I][J].v[Y]);
							}
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
					cell.isGround=true;
					cell.ismadeup=true;
					count++;
				}
			}
		}
		return count;
	}
	markGround(myvector<Cv4>&_vlist){
		//将groundijset所指各cell的IDList所指各点（_vlist中的点）标记为isGround=true
		int nij=(int)pgroundijset->ijset.size();
		for(int i=0;i<nij;i++){
			Cij&ij=pgroundijset->ijset[i];
			int I=ij.i;
			int J=ij.j;
			CgroundcellEx&cell=mat[I][J];
			//将cell的IDList所指各点(_vlist中的点)标记为isGround=true
			int nID=(int)cell.IDList.size();
			for(int j=0;j<nID;j++){
				int ID=cell.IDList[j];
				Cv4&v=_vlist[ID];
				v.isGround=true;
			
			}

		}

	}
	getGroundijsetAndDestroyijsetList()
	//将ijsetList中的各ijsetList中的点都填入到pgroundijset
	{
		//生成pgroundijset
		if(pgroundijset==NULL)pgroundijset=new Cijset();
		//
		int nijset=(int)ijsetList.size();
		for(int i=0;i<nijset;i++){
			Cijset&ijset=ijsetList[i];
			int nij=(int)ijset.ijset.size();
			for(int j=0;j<nij;j++){
				Cij&ij=ijset.ijset[j];
				pgroundijset->ijset.push_back(ij);
			}
		}
		//删除ijsetList
		ijsetList.swap(myvector<Cijset>());


		
	}
	cncTest()
	//按连通性对grid中的cell进行分类，每个类保存为一个ijset，所有的类保存为ijsetList
	//以阶跃点作为连通性的分隔点
	{
		for(int i=0;i<m_mat;i++){
			for(int j=0;j<n_mat;j++){
				if(mat[i][j].empty)continue;
				CgroundcellEx&cell=mat[i][j];
				if(cell.visited)continue;
				if(!cell.isGround)continue;//从一个未访问过的地面点开始递归
				//由cell开始递归，得到一个连通分支ijset
				Cijset ijset;//连通分支
				cncTest_inn(i,j,ijset);
				//为得到的连通分支ijset指定一个颜色
				init4(ijset.color,rand()%10*0.1,rand()%10*0.1,rand()%10*0.1,1);
				//将得到的连通分支加入到连通分支列表ijsetList中
				ijsetList.push_back(ijset);
			}
		}
	}
	void cncTest_inn(int i,int j,Cijset&ijset){
		CgroundcellEx&cell=mat[i][j];
		if(cell.visited)return;
		cell.visited=true;
		cell.isGround=true;
		Cij ij;
		ij.makeij(i,j);
		ijset.ijset.push_back(ij);
		//找一个不阶跃的邻居向下深入
		if(j-1>=0&&!cell.ltJump){//有左且左不阶跃
			cncTest_inn(i,j-1,ijset);
		}
		if(j+1<=n_mat-1&&!cell.rtJump){//有右且右不阶跃
			cncTest_inn(i,j+1,ijset);
		}
		if(i-1>=0&&!cell.upJump){//有上且上不阶跃
			cncTest_inn(i-1,j,ijset);
			
		}
		if(i+1<=m_mat-1&&!cell.dnJump){//有下且下不阶跃
			cncTest_inn(i+1,j,ijset);
		}
		
		
	}


	markSeed(const float xL,const float zL)
	//每个xL*zL区域里取一个地面种子cell，即令此cell的isGround=true
	{
		int jL=xL/xstepLen;
		int iL=zL/zstepLen;
		//搜索每个xL*zL的区域
		for(int i=0;i<m_mat;i+=iL){
			for(int j=0;j<n_mat;j+=jL){
				//搜索以(i,j)为左上角的xL*zL区域，找最低点，标记为isGround=true
				float minH=inf;
				int minI=-1;
				int minJ=-1;
				int I,J;
				for(I=i;I<i+iL;I++){
					if(I<0||I>m_mat-1)continue;
					for(J=j;J<j+jL;J++){
						if(J<0||J>n_mat-1)continue;
						if(mat[I][J].empty)continue;
						if(mat[I][J].v[Y]<minH){
							minH=mat[I][J].v[Y];
							minI=I;
							minJ=J;
						}
					}
				}//得到minI,minJ
				mat[minI][minJ].isGround=true;
				
				
			}
		}
		
	}
	
	showGround(GLuint texture,bool enableTexture=true,float r=1,float g=1,float b=1,float a=1){
		if(enableTexture){
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, texture);
		}
		glColor4f(r,g,b,a);
		int m=m_mat;
		int n=n_mat;
		float dx=xstepLen;
		float dz=zstepLen;
		float hfdx=dx/2;
		float hfdz=dz/2;
		for(int i=0;i<=m-2;i++){//i取1到m-2是为了防止邻格越界
			for(int j=0;j<=n-2;j++){//j取1到n-2是为了防止邻格越界
				float*v0=mat[i][j].v;
				float*v1=mat[i+1][j].v;
				float*v2=mat[i+1][j+1].v;
				float*v3=mat[i][j+1].v;
				float*u0=mat[i][j].u;
				float*u1=mat[i+1][j].u;
				float*u2=mat[i+1][j+1].u;
				float*u3=mat[i][j+1].u;
				//求法向量
				float norm[4];
				float v0v1[4],v0v2[4];
				sub(v1,v0,v0v1);
				sub(v2,v0,v0v2);
				cross(v0v1,v0v2,norm);
				normalize(norm);
				//画v0v1v2v3
				glBegin(GL_QUADS);
				glNormal3fv(norm);
				glTexCoord2fv(u0);
				glVertex3fv(v0);

				glTexCoord2fv(u1);
				glVertex3fv(v1);

				glTexCoord2fv(u2);
				glVertex3fv(v2);

				glTexCoord2fv(u3);
				glVertex3fv(v3);
				glEnd();	
			}
		}
		glColor4f(1,1,1,1);
		if(enableTexture){
			glDisable(GL_TEXTURE_2D);
		}

	
	}
	
	Hsmooth_strong()
	//对值进行平滑
	{
		//用九个格的高度值平均值代替中间格高度值
		//计算各格的vlt[Y],vrt[Y],vup[Y],vdn[Y]
		for(int i=0;i<m_mat;i++){
			for(int j=0;j<n_mat;j++){
				int r=16;//以格为单位的平滑区域边心距
				int count=0;//计数周围以r为边心距（以格为单位）的距形区域内非madeUp点的个数
				float avrgH=0;
				for(int I=i-r;I<=i+r;I++){
					if(I<0||I>m_mat-1)continue;
					for(int J=j-r;J<=j+r;J++){
						if(J<0||J>n_mat-1)continue;
						if(I==i&&J==j)continue;
						if(mat[I][J].isGround/*&&mat[I][J].ismadeup==false*/){
							avrgH+=mat[I][J].v[Y];
							count++;
						}
					}
				}
				if(count==0){
					continue;
				}
				avrgH/=count;
				mat[i][j].v[Y]=avrgH;

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
				CgroundcellEx&cell=mat[i][j];
				
				if(j-1>=0){
					CgroundcellEx&ltcell=mat[i][j-1];
					cell.ltdH=cell.v[Y]-ltcell.v[Y];
					if(fabs(cell.ltdH)>=threshold_dH)cell.ltJump=true;
				}
				if(j+1<=n_mat-1){
					CgroundcellEx&rtcell=mat[i][j+1];
					cell.rtdH=cell.v[Y]-rtcell.v[Y];
					if(fabs(cell.rtdH)>=threshold_dH)cell.rtJump=true;
				}
				if(i-1>=0){
					CgroundcellEx&upcell=mat[i-1][j];
					cell.updH=cell.v[Y]-upcell.v[Y];
					if(fabs(cell.updH)>=threshold_dH)cell.upJump=true;
				
				}
				if(i+1<=m_mat-1){
					CgroundcellEx&dncell=mat[i+1][j];
					cell.dndH=cell.v[Y]-dncell.v[Y];
					if(fabs(cell.dndH)>=threshold_dH)cell.dnJump=true;
				
				}
				if(i-1>=0&&j-1>=0){
					CgroundcellEx&lupcell=mat[i-1][j-1];
					cell.lupdH=cell.v[Y]-lupcell.v[Y];
					if(fabs(cell.lupdH)>=threshold2_dH)cell.lupJump=true;
				
				}
				if(i-1>=0&&j+1<=n_mat-1){
					CgroundcellEx&rupcell=mat[i-1][j+1];
					cell.rupdH=cell.v[Y]-rupcell.v[Y];
					if(fabs(cell.rupdH)>=threshold2_dH)cell.rupJump=true;
				}
				if(i+1<=m_mat-1&&j-1>=0){
					CgroundcellEx&ldncell=mat[i+1][j-1];
					cell.ldndH=cell.v[Y]-ldncell.v[Y];
					if(fabs(cell.ldndH)>=threshold2_dH)cell.ldnJump=true;
				}
				if(i+1<=m_mat-1&&j+1<=n_mat-1){
					CgroundcellEx&rdncell=mat[i+1][j+1];
					cell.rdndH=cell.v[Y]-rdncell.v[Y];
					if(fabs(cell.rdndH)>=threshold2_dH)cell.rdnJump=true;
				
				}	
				
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
				CgroundcellEx&cell=mat[i][j];
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
							if(mat[I][J].empty)continue;
							HList.push_back(mat[I][J].v[Y]);
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
			mat[I][J].IDList.push_back(i);
		}//mat各元素的IDList域已填充好
		//求mat各元素的v
		float hfdz=zstepLen/2;
		float hfdx=xstepLen/2;
		for(i=0;i<m_mat;i++){
			for(int j=0;j<n_mat;j++){
				CgroundcellEx&cell=mat[i][j];
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
		
		
	}

};


class Cgroundgrid{
public:
	int m_mat;//IDlistmat的行数
	int n_mat;//IDlistmat的列数
	float xstepLen;//x步长
	float zstepLen;//z步长
	float xmin,xmax,zmin,zmax;//xz范围
	myvector<myvector<Cgroundcell> > mat;//cell矩阵
	
	destroy(){//非析构销毁，为不影响下次使用，尽量销毁充分
		mat.resize(0);
		//mat.resize(0)后必面接下面两句
		m_mat=0;
		n_mat=0;
		xmin=xmax=zmin=zmax=0;

		
	}
	float getH(const float v[4])const 
	{//求v处网格高度
		//求v在哪个格
		int I,J;//v属于(I,J)格
		I=(v[Z]-zmin)/zstepLen;
		J=(v[X]-xmin)/xstepLen;
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
		return mat[I][J].h;
		
	
	}


	caluOfEachCell(float offsetus,float offsetut)
	//为各cell中心v点计算纹理坐标u
	{

		float hfdz=zstepLen/2;
		float hfdx=xstepLen/2;
		for(int i=0;i<m_mat;i++){
			for(int j=0;j<n_mat;j++){
				Cgroundcell&cell=mat[i][j];
				//计算cell.u
				float x=xmin+xstepLen*j+hfdx;
				float z=zmin+zstepLen*i+hfdz;
				cell.u[0]=(x-xmin)/(xmax-xmin)+offsetus;
				cell.u[1]=(z-zmin)/(zmax-zmin)+offsetut;

			}
		}

	}
	init(const CgroundgridEx&_GroundGrid){
		m_mat=_GroundGrid.m_mat;
		n_mat=_GroundGrid.n_mat;
		xmin=_GroundGrid.xmin;
		xmax=_GroundGrid.xmax;
		zmin=_GroundGrid.zmin;
		zmax=_GroundGrid.zmax;
		xstepLen=_GroundGrid.xstepLen;
		zstepLen=_GroundGrid.zstepLen;
		//将_GroundGrid.mat拷贝给mat
		mat.resize(m_mat);
		for(int i=0;i<m_mat;i++){
			mat[i].resize(n_mat);
		}
		for(i=0;i<m_mat;i++){
			for(int j=0;j<n_mat;j++){
				mat[i][j].h=_GroundGrid.mat[i][j].v[Y];
				mat[i][j].u[0]=_GroundGrid.mat[i][j].u[0];
				mat[i][j].u[1]=_GroundGrid.mat[i][j].u[1];
			}
			
		}
	
	}
	
	makeRoughGroundGrid(const CgroundgridEx&_GroundGrid,int di,int dj){//用_GroundGrid制作一个didj的粗糙版本
		//新网格参数
		m_mat=(int)(((float)_GroundGrid.m_mat)/di)+1;
		n_mat=(int)(((float)_GroundGrid.n_mat)/dj)+1;
		
		xmin=_GroundGrid.xmin;
		xmax=_GroundGrid.xmax;
		zmin=_GroundGrid.zmin;
		zmax=_GroundGrid.zmax;
		
		xstepLen=(xmax-xmin)/n_mat;//_GroundGrid.xstepLen*di;
		zstepLen=(zmax-zmin)/m_mat;//_GroundGrid.zstepLen+dj;
		
		//为新网格开辟空间
		{
			mat.resize(m_mat);
			for(int i=0;i<m_mat;i++){
				mat[i].resize(n_mat);
			}
		}
		//求mat各元素的v
		const myvector<myvector<CgroundcellEx> >&_mat=_GroundGrid.mat;
		for(int i=0;i<m_mat;i++){
			for(int j=0;j<n_mat;j++){
				Cgroundcell&cell=mat[i][j];
				//----求cell.h
				//mat[i][j]（即cell）对应块
				//  _mat[i*di][j*dj]---------_mat[i*di][j*dj+dj-1]
				//     |                      |
				//  _mat[i*di+di-1][j*dj]----_mat[i*di+di-1][j*dj+dj-1]
				//求此块中各v[Y]的平均值作为cell.h
				float avrgH=0;
				int count=0;
				for(int _i=i*di;_i<i*di+di;_i++){
					for(int _j=j*dj;_j<j*dj+dj;_j++){
						if(_i>=_GroundGrid.m_mat||_j>=_GroundGrid.n_mat)continue;
						avrgH+=_mat[_i][_j].v[Y];
						count++;
					}
				}
				if(count!=0)avrgH/=count;
				cell.h=avrgH;
			}
		}
	}
	showGround(GLuint texture,bool enableTexture=true,float r=1,float g=1,float b=1,float a=1){
		if(enableTexture){
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, texture);
		}
		glColor4f(r,g,b,a);
		int m=m_mat;
		int n=n_mat;
		float dx=xstepLen;
		float dz=zstepLen;
		float hfdx=dx/2;
		float hfdz=dz/2;
		for(int i=0;i<=m-2;i++){//i取1到m-2是为了防止邻格越界
			for(int j=0;j<=n-2;j++){//j取1到n-2是为了防止邻格越界
				float x=xmin+xstepLen*j+hfdx;
				float z=zmin+zstepLen*i+hfdz;
				float v0[4]={x,mat[i][j].h,z,1};
				float v1[4]={x,mat[i+1][j].h,z+zstepLen,1};
				float v2[4]={x+xstepLen,mat[i+1][j+1].h,z+zstepLen,1};
				float v3[4]={x+xstepLen,mat[i][j+1].h,z,1};
				float*u0=mat[i][j].u;
				float*u1=mat[i+1][j].u;
				float*u2=mat[i+1][j+1].u;
				float*u3=mat[i][j+1].u;
				//求法向量
				float norm[4];
				float v0v1[4],v0v2[4];
				sub(v1,v0,v0v1);
				sub(v2,v0,v0v2);
				cross(v0v1,v0v2,norm);
				normalize(norm);
				//画v0v1v2v3
				glBegin(GL_QUADS);
				glNormal3fv(norm);
				glTexCoord2fv(u0);
				glVertex3fv(v0);

				glTexCoord2fv(u1);
				glVertex3fv(v1);

				glTexCoord2fv(u2);
				glVertex3fv(v2);

				glTexCoord2fv(u3);
				glVertex3fv(v3);
				glEnd();	
			}
		}
		glColor4f(1,1,1,1);
		if(enableTexture){
			glDisable(GL_TEXTURE_2D);
		}

	
	}

};