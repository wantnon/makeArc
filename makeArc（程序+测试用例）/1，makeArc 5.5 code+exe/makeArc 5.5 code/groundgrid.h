class Cgroundcell{
public:
    float h;
	float u[2];
};
class CgroundcellEx
{
public:
	float v[4];//���ĵ�����
	float u[2];//���ĵ���������
	myvector<int> IDList;//���ڴ˸��е��Ƶ��ID�б�
	bool empty;//�����Ƿ�Ϊ�գ���v[Y]�Ƿ����������Чֵ��
	           //���ʱIDListΪ����emptyΪtrue,������֮��Ͳ�һ���ˣ�ֻ��ͨ��empty���жϣ�������ͨ��IDList�Ƿ�Ϊ�����ж�
	bool ismadeup;//���һ����empty���Ƿ����ɲ������
	bool ltJump,rtJump,upJump,dnJump,lupJump,ldnJump,rupJump,rdnJump;//�˸���������ң����ϣ����£����ϣ������ڸ��Ƿ��Ծ
	float ltdH,rtdH,updH,dndH,lupdH,ldndH,rupdH,rdndH;//�˸����������ң����ϣ����£����ϣ������ڸ�����ĸ߶Ȳ��Ծ���������ڸ�
	bool visited;//�Ƿ�����ͨ���Թ������ѷ��ʹ�
	bool isGround;//�Ƿ�Ϊ����cell
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
	float color[4];//��ɫ
	Cijset(){
		init4(color,1,1,1,1);
	}
};

class CgroundgridEx
{
public:
	int m_mat;//IDlistmat������
	int n_mat;//IDlistmat������
	float xstepLen;//x����
	float zstepLen;//z����
	float xmin,xmax,zmin,zmax;//xz��Χ
	myvector<myvector<CgroundcellEx> > mat;//cell����
	//---------------------------------------------------------------------------------------------------------------
	myvector<Cijset> ijsetList;//ij�����б�
	Cijset*pgroundijset;//����ij��
	//---------------------------------------------------------------------------------------------------
	CgroundgridEx(){
		pgroundijset=NULL;
		
	}
	~CgroundgridEx(){//�������٣�ֻ���ٶ�̬���ּ���
		clearGroundijset();
	
	}
	destroy(){//���������٣�Ϊ��Ӱ���´�ʹ�ã��������ٳ��
		mat.resize(0);
		//mat.resize(0)��������������
		m_mat=0;
		n_mat=0;
		xmin=xmax=zmin=zmax=0;
		ijsetList.resize(0);
		clearGroundijset();

		
	}
	clearGroundijset(){
		if(pgroundijset!=NULL){
			delete pgroundijset;
			pgroundijset=NULL;//��Ҫ������ΪNULL
		}
	}
	float getH(const float v[4])const 
	{//��v������߶�
		//��v���ĸ���
		int I,J;//v����(I,J)��
		I=(v[Z]-zmin)/zstepLen;
		J=(v[X]-xmin)/xstepLen;
		bool succ=true;
		if(I<0||I>=m_mat)succ=false;
		if(J<0||J>=n_mat)succ=false;
		if(!succ){
			if(showWarning)cout<<"error! δ����������"<<endl;
			if(I<0)I=0;
			if(I>=m_mat)I=m_mat-1;
			if(J<0)J=0;
			if(J>=n_mat)J=n_mat-1;
		}
		return mat[I][J].v[Y];
		
	
	}
	makeRoughGroundGrid(const CgroundgridEx&_GroundGrid,int di,int dj){//��_GroundGrid����һ��didj�Ĵֲڰ汾
		//���������
		m_mat=(int)(((float)_GroundGrid.m_mat)/di)+1;
		n_mat=(int)(((float)_GroundGrid.n_mat)/dj)+1;
		xstepLen=_GroundGrid.xstepLen*di;
		zstepLen=_GroundGrid.zstepLen+dj;
		xmin=_GroundGrid.xmin;//-xstepLen;
		xmax=_GroundGrid.xmax;//+xstepLen;
		zmin=_GroundGrid.zmin;//-zstepLen;
		zmax=_GroundGrid.zmax;//+zstepLen;
		//Ϊ�����񿪱ٿռ�
		{
			mat.resize(m_mat);
			for(int i=0;i<m_mat;i++){
				mat[i].resize(n_mat);
			}
		}
		//��mat��Ԫ�ص�v
		const myvector<myvector<CgroundcellEx> >&_mat=_GroundGrid.mat;
		float hfdz=zstepLen/2;
		float hfdx=xstepLen/2;
		for(int i=0;i<m_mat;i++){
			for(int j=0;j<n_mat;j++){
				CgroundcellEx&cell=mat[i][j];
				//----��cell.v[X]��cell.v[Z]
				cell.v[X]=xmin+xstepLen*j+hfdx;
				cell.v[Z]=zmin+zstepLen*i+hfdz;
				//----��cell.v[Y]
				
				//mat[i][j]����cell����Ӧ��
				//  _mat[i*di][j*dj]---------_mat[i*di][j*dj+dj-1]
				//     |                      |
				//  _mat[i*di+di-1][j*dj]----_mat[i*di+di-1][j*dj+dj-1]
				//��˿��и�v[Y]��ƽ��ֵ��Ϊcell.v[Y]
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
	//Ϊ��cell����v�������������u
	{
		for(int i=0;i<m_mat;i++){
			for(int j=0;j<n_mat;j++){
				if(mat[i][j].empty)continue;
				CgroundcellEx&cell=mat[i][j];
				//����cell.u
				cell.u[0]=(cell.v[X]-xmin)/(xmax-xmin)+offsetus;
				cell.u[1]=(cell.v[Z]-zmin)/(zmax-zmin)+offsetut;

			}
		}

	}
	clearIDListForEachCell(){//�����cell��IDList
		for(int i=0;i<m_mat;i++){
			for(int j=0;j<n_mat;j++){
				CgroundcellEx&cell=mat[i][j];
				cell.clearIDList();
			
			}
		}

	
	}
	int makeUpGround_once()
		//��ȱʧ�ĵ���Ԫ�ؽ����޲�
	{
		int count=0;
		for(int i=0;i<m_mat;i++){
			for(int j=0;j<n_mat;j++){
				CgroundcellEx&cell=mat[i][j];
				if(!cell.isGround){
					//������ڸ񣬽����ڸ���ΪisGround��Hֵȡƽ������mat[i][j]
					//mat[i-1][j-1]  mat[i-1][j]  mat[i-1][j+1]
					//mat[i][j-1]                 mat[i][j+1]
					//mat[i+1][j-1]  mat[i+1][j]  mat[i+1][j+1]
					myvector<float> HList;//���ڸ�����ЧHֵ�б�
					for(int I=i-1;I<=i+1;I++){
						if(I<0||I>m_mat-1)continue;
						for(int J=j-1;J<=j+1;J++){
							if(J<0||J>n_mat-1)continue;
							if(I==i&&J==j)continue;
							if(mat[I][J].isGround){
								HList.push_back(mat[I][J].v[Y]);
							}
						}
					}//�õ�HList
					if((int)HList.size()==0){
						continue;
					}
					//��HList��ƽ���߶�
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
		//��groundijset��ָ��cell��IDList��ָ���㣨_vlist�еĵ㣩���ΪisGround=true
		int nij=(int)pgroundijset->ijset.size();
		for(int i=0;i<nij;i++){
			Cij&ij=pgroundijset->ijset[i];
			int I=ij.i;
			int J=ij.j;
			CgroundcellEx&cell=mat[I][J];
			//��cell��IDList��ָ����(_vlist�еĵ�)���ΪisGround=true
			int nID=(int)cell.IDList.size();
			for(int j=0;j<nID;j++){
				int ID=cell.IDList[j];
				Cv4&v=_vlist[ID];
				v.isGround=true;
			
			}

		}

	}
	getGroundijsetAndDestroyijsetList()
	//��ijsetList�еĸ�ijsetList�еĵ㶼���뵽pgroundijset
	{
		//����pgroundijset
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
		//ɾ��ijsetList
		ijsetList.swap(myvector<Cijset>());


		
	}
	cncTest()
	//����ͨ�Զ�grid�е�cell���з��࣬ÿ���ౣ��Ϊһ��ijset�����е��ౣ��ΪijsetList
	//�Խ�Ծ����Ϊ��ͨ�Եķָ���
	{
		for(int i=0;i<m_mat;i++){
			for(int j=0;j<n_mat;j++){
				if(mat[i][j].empty)continue;
				CgroundcellEx&cell=mat[i][j];
				if(cell.visited)continue;
				if(!cell.isGround)continue;//��һ��δ���ʹ��ĵ���㿪ʼ�ݹ�
				//��cell��ʼ�ݹ飬�õ�һ����ͨ��֧ijset
				Cijset ijset;//��ͨ��֧
				cncTest_inn(i,j,ijset);
				//Ϊ�õ�����ͨ��֧ijsetָ��һ����ɫ
				init4(ijset.color,rand()%10*0.1,rand()%10*0.1,rand()%10*0.1,1);
				//���õ�����ͨ��֧���뵽��ͨ��֧�б�ijsetList��
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
		//��һ������Ծ���ھ���������
		if(j-1>=0&&!cell.ltJump){//�������󲻽�Ծ
			cncTest_inn(i,j-1,ijset);
		}
		if(j+1<=n_mat-1&&!cell.rtJump){//�������Ҳ���Ծ
			cncTest_inn(i,j+1,ijset);
		}
		if(i-1>=0&&!cell.upJump){//�������ϲ���Ծ
			cncTest_inn(i-1,j,ijset);
			
		}
		if(i+1<=m_mat-1&&!cell.dnJump){//�������²���Ծ
			cncTest_inn(i+1,j,ijset);
		}
		
		
	}


	markSeed(const float xL,const float zL)
	//ÿ��xL*zL������ȡһ����������cell�������cell��isGround=true
	{
		int jL=xL/xstepLen;
		int iL=zL/zstepLen;
		//����ÿ��xL*zL������
		for(int i=0;i<m_mat;i+=iL){
			for(int j=0;j<n_mat;j+=jL){
				//������(i,j)Ϊ���Ͻǵ�xL*zL��������͵㣬���ΪisGround=true
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
				}//�õ�minI,minJ
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
		for(int i=0;i<=m-2;i++){//iȡ1��m-2��Ϊ�˷�ֹ�ڸ�Խ��
			for(int j=0;j<=n-2;j++){//jȡ1��n-2��Ϊ�˷�ֹ�ڸ�Խ��
				float*v0=mat[i][j].v;
				float*v1=mat[i+1][j].v;
				float*v2=mat[i+1][j+1].v;
				float*v3=mat[i][j+1].v;
				float*u0=mat[i][j].u;
				float*u1=mat[i+1][j].u;
				float*u2=mat[i+1][j+1].u;
				float*u3=mat[i][j+1].u;
				//������
				float norm[4];
				float v0v1[4],v0v2[4];
				sub(v1,v0,v0v1);
				sub(v2,v0,v0v2);
				cross(v0v1,v0v2,norm);
				normalize(norm);
				//��v0v1v2v3
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
	//��ֵ����ƽ��
	{
		//�þŸ���ĸ߶�ֵƽ��ֵ�����м��߶�ֵ
		//��������vlt[Y],vrt[Y],vup[Y],vdn[Y]
		for(int i=0;i<m_mat;i++){
			for(int j=0;j<n_mat;j++){
				int r=16;//�Ը�Ϊ��λ��ƽ��������ľ�
				int count=0;//������Χ��rΪ���ľࣨ�Ը�Ϊ��λ���ľ��������ڷ�madeUp��ĸ���
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
		//��Ǹ�������������ڸ��Ľ�Ծ��������������ltJump,rtJump,upJump,dnJump
		//threshold_dH��Ծ�߶Ȳ���ֵ
		//threshold2_dHб�Խ��ڸ�߶Ȳ���ֵ
	{
		
		
		//��������ltJump,rtJump,upJump,dnJump
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
		//��ȱʧԪ�ؽ���һ���޲����þ�ֵ�����޲���
		//����ȥ���޲���cell����
	{
		int count=0;
		for(int i=0;i<m_mat;i++){
			for(int j=0;j<n_mat;j++){
				CgroundcellEx&cell=mat[i][j];
				if(cell.empty){//��partoutcomer��outcomerҲ�����޲�
					//������ڸ񣬽����ڸ��зǿյ�Hֵȡƽ������mat[i][j]
					//mat[i-1][j-1]  mat[i-1][j]  mat[i-1][j+1]
					//mat[i][j-1]                 mat[i][j+1]
					//mat[i+1][j-1]  mat[i+1][j]  mat[i+1][j+1]
					myvector<float> HList;//���ڸ�����ЧHֵ�б�
					for(int I=i-1;I<=i+1;I++){
						if(I<0||I>m_mat-1)continue;
						for(int J=j-1;J<=j+1;J++){
							if(J<0||J>n_mat-1)continue;
							if(I==i&&J==j)continue;
							if(mat[I][J].empty)continue;
							HList.push_back(mat[I][J].v[Y]);
						}
					}//�õ�HList
					if((int)HList.size()==0){
						continue;
					}
					//��HList��ƽ���߶�
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

	makeGrid(float _xmin,float _xmax,float _zmin,float _zmax,//��Χ
		float _xstepLen,float _zstepLen,//����
		myvector<Cv4>&_vlist//����
		){//��_xmin,_xmax,_zmin,_zmax��Χ��Ϊ����_vlist�����������񲽳�ԼΪ_xstepLen,_zstepLen
		xstepLen=_xstepLen;
		zstepLen=_zstepLen;
		xmin=_xmin;
		xmax=_xmax;
		zmin=_zmin;
		zmax=_zmax;
		//�趨����ߴ�
		m_mat=(_zmax-_zmin)/zstepLen+1;
		n_mat=(_xmax-_xmin)/xstepLen+1;
		//��������
		mat.resize(m_mat);
		for(int i=0;i<m_mat;i++){
			mat[i].resize(n_mat);
		}
		//��_vlist�е����е�ַ�������
		int _nv=(int)_vlist.size();
		for(i=0;i<_nv;i++){
			Cv4 v=_vlist[i];
			if(v.isoutcomer)continue;//����outcomer��
			//��v�����ĸ���
			int I,J;//v����(I,J)��
			I=(v.v[Z]-_zmin)/zstepLen;
			J=(v.v[X]-_xmin)/xstepLen;
			if(I<0||J<0||I>m_mat-1||J>n_mat-1)continue;
			//��v����˸�
			mat[I][J].IDList.push_back(i);
		}//mat��Ԫ�ص�IDList��������
		//��mat��Ԫ�ص�v
		float hfdz=zstepLen/2;
		float hfdx=xstepLen/2;
		for(i=0;i<m_mat;i++){
			for(int j=0;j<n_mat;j++){
				CgroundcellEx&cell=mat[i][j];
				//----��cell.v[X]��cell.v[Z]
				cell.v[X]=xmin+xstepLen*j+hfdx;
				cell.v[Z]=zmin+zstepLen*i+hfdz;
				//----��cell.v[Y]
				//��cell.IDList�е��ƽ��ֵ
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
				avrgH/=nID;//�õ�avrgH
				//
				cell.v[Y]=avrgH;
				cell.empty=false;
	
				

			}
		}
		
		
	}

};


class Cgroundgrid{
public:
	int m_mat;//IDlistmat������
	int n_mat;//IDlistmat������
	float xstepLen;//x����
	float zstepLen;//z����
	float xmin,xmax,zmin,zmax;//xz��Χ
	myvector<myvector<Cgroundcell> > mat;//cell����
	
	destroy(){//���������٣�Ϊ��Ӱ���´�ʹ�ã��������ٳ��
		mat.resize(0);
		//mat.resize(0)��������������
		m_mat=0;
		n_mat=0;
		xmin=xmax=zmin=zmax=0;

		
	}
	float getH(const float v[4])const 
	{//��v������߶�
		//��v���ĸ���
		int I,J;//v����(I,J)��
		I=(v[Z]-zmin)/zstepLen;
		J=(v[X]-xmin)/xstepLen;
		bool succ=true;
		if(I<0||I>=m_mat)succ=false;
		if(J<0||J>=n_mat)succ=false;
		if(!succ){
			if(showWarning)cout<<"error! δ����������"<<endl;
			if(I<0)I=0;
			if(I>=m_mat)I=m_mat-1;
			if(J<0)J=0;
			if(J>=n_mat)J=n_mat-1;
		}
		return mat[I][J].h;
		
	
	}


	caluOfEachCell(float offsetus,float offsetut)
	//Ϊ��cell����v�������������u
	{

		float hfdz=zstepLen/2;
		float hfdx=xstepLen/2;
		for(int i=0;i<m_mat;i++){
			for(int j=0;j<n_mat;j++){
				Cgroundcell&cell=mat[i][j];
				//����cell.u
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
		//��_GroundGrid.mat������mat
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
	
	makeRoughGroundGrid(const CgroundgridEx&_GroundGrid,int di,int dj){//��_GroundGrid����һ��didj�Ĵֲڰ汾
		//���������
		m_mat=(int)(((float)_GroundGrid.m_mat)/di)+1;
		n_mat=(int)(((float)_GroundGrid.n_mat)/dj)+1;
		
		xmin=_GroundGrid.xmin;
		xmax=_GroundGrid.xmax;
		zmin=_GroundGrid.zmin;
		zmax=_GroundGrid.zmax;
		
		xstepLen=(xmax-xmin)/n_mat;//_GroundGrid.xstepLen*di;
		zstepLen=(zmax-zmin)/m_mat;//_GroundGrid.zstepLen+dj;
		
		//Ϊ�����񿪱ٿռ�
		{
			mat.resize(m_mat);
			for(int i=0;i<m_mat;i++){
				mat[i].resize(n_mat);
			}
		}
		//��mat��Ԫ�ص�v
		const myvector<myvector<CgroundcellEx> >&_mat=_GroundGrid.mat;
		for(int i=0;i<m_mat;i++){
			for(int j=0;j<n_mat;j++){
				Cgroundcell&cell=mat[i][j];
				//----��cell.h
				//mat[i][j]����cell����Ӧ��
				//  _mat[i*di][j*dj]---------_mat[i*di][j*dj+dj-1]
				//     |                      |
				//  _mat[i*di+di-1][j*dj]----_mat[i*di+di-1][j*dj+dj-1]
				//��˿��и�v[Y]��ƽ��ֵ��Ϊcell.h
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
		for(int i=0;i<=m-2;i++){//iȡ1��m-2��Ϊ�˷�ֹ�ڸ�Խ��
			for(int j=0;j<=n-2;j++){//jȡ1��n-2��Ϊ�˷�ֹ�ڸ�Խ��
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
				//������
				float norm[4];
				float v0v1[4],v0v2[4];
				sub(v1,v0,v0v1);
				sub(v2,v0,v0v2);
				cross(v0v1,v0v2,norm);
				normalize(norm);
				//��v0v1v2v3
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