
class Ccell{
public:
	int innerLayerID;//empty��cell��innerLayerID=0��needcut��cell��innerLayerID=1������һ�����2,����һ�����3
	myvector<int> IDList;//���ڴ˸��е��Ƶ��ID�б�
	bool empty;//�����Ƿ�Ϊ�գ���v[Y]�Ƿ����������Чֵ��
	           //���ʱIDListΪ����emptyΪtrue,������֮��Ͳ�һ���ˣ�ֻ��ͨ��empty���жϣ�������ͨ��IDList�Ƿ�Ϊ�����ж�
	bool ismadeup;//���һ����empty���Ƿ����ɲ������
	float v[4];//���ĵ㣨��߶�ΪIDList�и�������߶ȣ�
	float u[2];//���ĵ��Ӧ����ͼ����
	float vlt[4],vrt[4],vup[4],vdn[4],vlup[4],vldn[4],vrup[4],vrdn[4];//����İ˸��߽��
	float ult[2],urt[2],uup[2],udn[2],ulup[2],uldn[2],urup[2],urdn[2];//����İ˸��߽����ͼ����
	bool ltJump,rtJump,upJump,dnJump,lupJump,ldnJump,rupJump,rdnJump;//�˸���������ң����ϣ����£����ϣ������ڸ��Ƿ��Ծ
	float ltdH,rtdH,updH,dndH,lupdH,ldndH,rupdH,rdndH;//�˸����������ң����ϣ����£����ϣ������ڸ�����ĸ߶Ȳ��Ծ���������ڸ�
	bool isCorner;//�ǽǵ�
	bool isCorner_rough;//�Ǵֽǵ�
	float norm[4][4];//�ĸ�С��ķ�������������ֻ����ȥ����
	float avrgnorm[4];//�ĸ�С���ƽ����������������ֻ����ȥ����
	bool needcut;//��Ҫ�и�
	myvector<Cpolygen> polygenList;//������б�
	//  vlup--vup--vrup
	//   |  0  |  3 |
	//  vlt----v----vrt
	//   |  1  |  2 |
    //  vldn--vdn--vrdn
	bool asWhole4;//ָ���Դ˸����Ͻǵ�4����(2*2)�Ƿ���Ϊһ���������(���޲��ѷ�����)
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
	//�������Ƕ���εķ�������ע�⣬�˹���ֻ��������polygen������������ʱ���ã�
	{
		//��ʱpolygenList�и�polygen�о�Ϊ������
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
	//ת���ɶ���μ��ϣ�����needcut��
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
			//��
			polygen_up.addvert(vrup);
			polygen_up.addvert(vlup);
			polygen_up.addvert(vrdn);
			//��
			polygen_dn.addvert(vlup);
			polygen_dn.addvert(vldn);
			polygen_dn.addvert(vrdn);

		}else{
			//     |/|
			//��
			polygen_up.addvert(vrup);
			polygen_up.addvert(vlup);
			polygen_up.addvert(vldn);

			//��
			polygen_dn.addvert(vrup);
			polygen_dn.addvert(vldn);
			polygen_dn.addvert(vrdn);
		}
		polygenList.push_back(polygen_up);
		polygenList.push_back(polygen_dn);
		

	}

	calTexCoord(float xmin,float xmax,float zmin,float zmax,
		float offsetus,float offsetut)
	//ΪpolygenList�еĵ������ͼ����
	//Ϊv,vup,vdn,vlt,vrt,vlup,vldn,vrup,vrdn������ͼ����
	{
		//ΪpolygenList�еĵ������ͼ����
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
	
		//Ϊv,vup,vdn,vlt,vrt,vlup,vldn,vrup,vrdn������ͼ����
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
		//�����ĸ�С��ķ�����
		//�����ĸ�С���ƽ��������
		//����˸�С������ķ�����
	{
		//    vlup--vup--vrup
		//     |  0  |  3  |  
		//    vlt----v----vrt
		//     |  1  |  2  |  
		//    vldn--vdn--vrdn   
		//�����ĸ�С��ķ�����
		//��0
		init4(norm[0],0,0,0,0);
		float tnorm[4];
		calNorm(vup,vlup,vlt,tnorm);add(norm[0],tnorm,norm[0]);
		calNorm(vup,vlt,v,tnorm);add(norm[0],tnorm,norm[0]);
		calNorm(vup,vlup,v,tnorm);add(norm[0],tnorm,norm[0]);
		calNorm(vlup,vlt,v,tnorm);add(norm[0],tnorm,norm[0]);
		normalize(norm[0]);
		//��1
		init4(norm[1],0,0,0,0);
		calNorm(v,vlt,vldn,tnorm);add(norm[1],tnorm,norm[1]);
		calNorm(v,vldn,vdn,tnorm);add(norm[1],tnorm,norm[1]);
		calNorm(v,vlt,vdn,tnorm);add(norm[1],tnorm,norm[1]);
		calNorm(vlt,vldn,vdn,tnorm);add(norm[1],tnorm,norm[1]);
		normalize(norm[1]);
		//��2
		init4(norm[2],0,0,0,0);
		calNorm(vrt,v,vdn,tnorm);add(norm[2],tnorm,norm[2]);
		calNorm(vrt,vdn,vrdn,tnorm);add(norm[2],tnorm,norm[2]);
		calNorm(vrt,v,vrdn,tnorm);add(norm[2],tnorm,norm[2]);
		calNorm(v,vdn,vrdn,tnorm);add(norm[2],tnorm,norm[2]);
		normalize(norm[2]);
		//��3
		init4(norm[3],0,0,0,0);
		calNorm(vrup,vup,v,tnorm);add(norm[3],tnorm,norm[3]);
		calNorm(vrup,v,vrt,tnorm);add(norm[3],tnorm,norm[3]);
		calNorm(vrup,vup,vrt,tnorm);add(norm[3],tnorm,norm[3]);
		calNorm(vup,v,vrt,tnorm);add(norm[3],tnorm,norm[3]);
		normalize(norm[3]);

		//�������ǵ�ƽ��������
		init4(avrgnorm,0,0,0,0);
		add(avrgnorm,norm[0],avrgnorm);
		add(avrgnorm,norm[1],avrgnorm);
		add(avrgnorm,norm[2],avrgnorm);
		add(avrgnorm,norm[3],avrgnorm);
		normalize(avrgnorm);//��Ҫ���˵�λ��

	

	}

	show_rough0(float r=1,float g=1,float b=1,float a=1)
	//ϸ����ʾ
	{

	/*  //�˶ε�����ʾinnerLayerID��
 		r=(float)(innerLayerID*10)/255;
		g=r;
		b=r;
		glColor4f(r,g,b,a);*/


		glColor4f(r,g,b,a);
		if(needcut){
			//��������б�
			int npolygen=(int)polygenList.size();
			for(int i=0;i<npolygen;i++){
				Cpolygen&polygen=polygenList[i];
				if(polygen.removed)continue;
				//��polygen
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
			//��cell
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
	float xmin,xmax,zmin,zmax;//xz��Χ���ֲ�����ϵ��
public:
	int maxInnerLayerID;//����innerLayerID�����ֵ
	float m_locToWorld[16];//��������ռ�ת������ռ�
	float m_worldToLoc[16];//������ռ�ת��������ռ�
	int m_mat;//IDlistmat������
	int n_mat;//IDlistmat������
	float xstepLen;//x����
	float zstepLen;//z����
	myvector<myvector<Ccell*> > mat;//cell����
	Cgrid(){
		maxInnerLayerID=-1;
		m_mat=0;
		n_mat=0;
		xstepLen=1;
		zstepLen=1;
		xmin=xmax=zmin=zmax=0;
	
	}
	void getRange(float&_xmin,float&_xmax,float&_zmin,float&_zmax)const{//��÷�Χ
		_xmin=xmin;
		_xmax=xmax;
		_zmin=zmin;
		_zmax=zmax;
	}
	void setRange(float _xmin,float _xmax,float _zmin,float _zmax){//�趨��Χ
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

	float calS_notEmpty(){//���㱾�����з�empty��cell�������
		float S=0;
		float dS=xstepLen*zstepLen;//һ��������
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
	clearMatElms(){//ɾ��mat������Ԫ��
		for(int i=0;i<m_mat;i++){
			for(int j=0;j<n_mat;j++){
				if(mat[i][j]!=NULL){
					delete mat[i][j];
					mat[i][j]=NULL;//һ����Ҫ������NULL
				}
			}
		}
		
	}
	clearMat(){//ɾ��mat������Ԫ�أ���ɾ��mat���
		clearMatElms();
		mat.resize(0);
		//����ִ����mat.resize(0)�����Ա���ִ����������
		m_mat=0;
		n_mat=0;
		xmin=xmax=zmin=zmax=0;
		
		
	}
	
	
	clearIDListForEachCell(){//�����cell��IDList
		for(int i=0;i<m_mat;i++){
			for(int j=0;j<n_mat;j++){
				if(mat[i][j]==NULL)continue;
				Ccell&cell=*mat[i][j];
				cell.clearIDList();
			
			}
		}

	
	}
	removeEmptyCell(){//��empty�ĸ�ɾ��
		for(int i=0;i<m_mat;i++){
			for(int j=0;j<n_mat;j++){
				if(mat[i][j]->empty){
					delete mat[i][j];
					mat[i][j]=NULL;//һ��Ҫ��ΪNULL
				}
			}
		}

	
	}
	bool vInCellIJ_vIsWorldPoint(const float v[4],int&I,int&J){//����������ϵ�е�v���ĸ�����
		//��v�任���ֲ�����ϵ
		float v_loc[4];
		prodv(m_worldToLoc,v,v_loc);
		//��v_loc���ĸ���
		I=(v_loc[Z]-zmin)/zstepLen;
		J=(v_loc[X]-xmin)/xstepLen;

		if(I<0||I>=m_mat)return false;
		if(J<0||J>=n_mat)return false;
		return true;

	
	}
	float getH_vIsWorldPoint(const float v[4]){//����������ϵ�еĵ�v�ڱ������еĲ�ֵ�߶�
		//��vת���ֲ��ռ�
		float _v[4];//v�ľֲ�����
		prodv(m_worldToLoc,v,_v);
		//��_v���ĸ���
		int I,J;//_v����(I,J)��
		I=(_v[Z]-zmin)/zstepLen;
		J=(_v[X]-xmin)/xstepLen;
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
		Ccell&cell=*(mat[I][J]);
		float vlup[4],vldn[4],vrup[4],vrdn[4];
		veccopy(cell.vlup,vlup);
		veccopy(cell.vldn,vldn);
		veccopy(cell.vrup,vrup);
		veccopy(cell.vrdn,vrdn);
		//��vlup,vldn,vrup,vrdnת���ֲ��ռ�
		prodv(m_worldToLoc,vlup,vlup);
		prodv(m_worldToLoc,vldn,vldn);
		prodv(m_worldToLoc,vrup,vrup);
		prodv(m_worldToLoc,vrdn,vrdn);
		//ͨ��˫���Բ�ֵ��_v���ĸ߶�
		float H=calH_BilinearAABB(vlup,vldn,vrup,vrdn,_v);
		return H;

	
	}
	caluOfEachCell(float offsetus,float offsetut)
	//Ϊ��cell����v�������������u
	{
		for(int i=0;i<m_mat;i++){
			for(int j=0;j<n_mat;j++){
				if(mat[i][j]->empty)continue;
				Ccell&cell=*mat[i][j];
				
				//����cell.u
				cell.u[0]=(cell.v[X]-xmin)/(xmax-xmin)+offsetus;
				cell.u[1]=(cell.v[Z]-zmin)/(zmax-zmin)+offsetut;

			}
		}

	}


	
	
	
	
	//---------------------------------------------------------------------------------------------------------------
cutNeedcutCells(float _v1[4],float _v2[4])
	//���߶�_v1_v2��������ȥ�и�����needcut==true��cell�����ж����
	{

		float v1[4],v2[4];
		veccopy(_v1,v1);
		v1[Y]=0;
		veccopy(_v2,v2);
		v2[Y]=0;
		//��v1v2���ҷ�����
		float v1v2[4];
		sub(v2,v1,v1v2);
		float norm[4]={-v1v2[Z],0,v1v2[X],0};
		normalize(norm);
		//����v1v2��������
		Cplane plane;
		plane.makePlane(v1,norm);
		//��v1���ڸ�
		int I1,J1;//v1����(I1,J1)��
		vInCellIJ_vIsWorldPoint(v1,I1,J1);
		//��v2���ڸ�
		int I2,J2;//v2����(I2,J2)��
		vInCellIJ_vIsWorldPoint(v2,I2,J2);
		//��v1v2�ľ��ο�Χ
		int minI=min(I1,I2);
		int maxI=max(I1,I2);
		int minJ=min(J1,J2);
		int maxJ=max(J1,J2);
		//��planeȥ�и����д���[minI,minJ]-[maxI,maxJ]��Χ����needcut==true��cell�е����ж����
		for(int i=minI;i<=maxI;i++){
			if(i<0||i>=m_mat)continue;
			for(int j=minJ;j<=maxJ;j++){
				if(j<0||j>=n_mat)continue;
				Ccell&cell=*mat[i][j];
				if(!cell.needcut)continue;
				//��planeȥ�и�cell�����ж����
				int npolygen=(int)cell.polygenList.size();
				for(int k=0;k<npolygen;k++){
					Cpolygen&polygen=cell.polygenList[k];
					//��planeȥ�и�polygen
					float side1,side2;
					bool iscoplane;
					bool isdivided;
					Cpolygen polygen1,polygen2;
					bool isnormal=plane_div_polygen(plane,polygen,polygen1,side1,polygen2,side2,iscoplane,isdivided);
					if(iscoplane)continue;
					if(!isnormal)continue;
					if(isdivided){//����и�ɹ�
						//��polygen�ķ���������polygen1��polygen2
						veccopy(polygen.norm,polygen1.norm);
						veccopy(polygen.norm,polygen2.norm);
						//��polygen1�滻polygen
						polygen=polygen1;
						//��polygen2���뵽polygenList
						cell.polygenList.push_back(polygen2);
					}else{//δ�ɹ���˵����ĳһ��
						
					}
					
				}
				
			}
		}
		
	}

	polygenlize()
	//������polygenlize
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
	//������polygenList�и�����polygen�ķ�������ע�⣬�˹���ֻ��������polygen������������ʱ����
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
	//�жϴ�grid�Ƿ�Ϊtree
	{
		//���grid�з�empty��cell��isCorner_rough==true��cell����һ������������Ϊ��grid��tree
		int count_notEmpty=0;//ͳ�Ʒ�empty��cell�ĸ���
		int count_isCorner_rough=0;//ͳ��isCorner_rough==true��cell�ĸ���
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
	//��ǽǵ�cell
	{
	
		for(int i=1;i<=m_mat-2;i++){//iȡ1��m_mat-2��Ϊ�˷�ֹ�ڸ�Խ��
			for(int j=1;j<=n_mat-2;j++){//jȡ1��n_mat-2��Ϊ�˷�ֹ�ڸ�Խ��
				if(mat[i][j]->empty)continue;
				Ccell&cell=*mat[i][j];
				
				Ccell&upcell=*mat[i-1][j];
				Ccell&dncell=*mat[i+1][j];
				Ccell&ltcell=*mat[i][j-1];
				Ccell&rtcell=*mat[i][j+1];
				if(upcell.empty||dncell.empty||ltcell.empty||rtcell.empty){//�߽�㲻����
					cell.isCorner=false;
					cell.isCorner_rough=false;
					continue;
				}
				
			
				//�󱾸��ƽ������ƫ����
				{
					float D=0;//�н�A��sin�ı�׼��
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
					//����������ı�׼���һ��ֵ������Ϊ�ǽǵ�cell
					if(D>=0.16*0.16){
						cell.isCorner=true;
					}else{
						cell.isCorner=false;
					}
				}
				//���ڸ��ƽ������ƫ����
				{
					Ccell&upcell=*mat[i-1][j];
					Ccell&dncell=*mat[i+1][j];
					Ccell&ltcell=*mat[i][j-1];
					Ccell&rtcell=*mat[i][j+1];
					Ccell&lupcell=*mat[i-1][j-1];
					Ccell&ldncell=*mat[i+1][j-1];
					Ccell&rupcell=*mat[i-1][j+1];
					Ccell&rdncell=*mat[i+1][j+1];
					//����Χƽ������ƫ����
					float D=0;//�н�A��sin�ı�׼��
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
	//��սǵ�cell���
	{
		for(int i=0;i<m_mat;i++){//iȡ1��m_mat-2��Ϊ�˷�ֹ�ڸ�Խ��
			for(int j=0;j<n_mat;j++){//jȡ1��n_mat-2��Ϊ�˷�ֹ�ڸ�Խ��
				if(mat[i][j]->empty)continue;
				Ccell&cell=*mat[i][j];
				
				cell.isCorner=false;
				cell.isCorner_rough=false;
			}
		}

	}
	calNorms()
	//�������ķ�����
	{
		for(int i=0;i<m_mat;i++){
			for(int j=0;j<n_mat;j++){
				if(mat[i][j]->empty)continue;
				Ccell&cell=*mat[i][j];
				
				cell.calNorms();
			}
		}

	
	}
	emptyHsmooth(){//��empty==true�ĸ����ƽ��
		const int r=5;//�Ը�Ϊ��λ��ƽ��������ľ�
		for(int i=0;i<m_mat;i++){
			for(int j=0;j<n_mat;j++){
				if(!(*mat[i][j]).empty)continue;
				
				int count=0;//������Χ��rΪ���ľࣨ�Ը�Ϊ��λ���ľ��������ڷ�madeUp��ĸ���
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
	calInnerLayerID(){//Ϊ��cell����innerLayerIDֵ

		//���Ƚ�empty��cell��innerLayerID����Ϊ0
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
		//��needcut��cell��innerLayerID����Ϊ1
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
		//����������ڲ��innerLayerID
		int curInnerLayerID=2;
		while(1){	
			//��innerLayerIDΪcurInnerLayerID��cell������innerLayerID���ΪcurInnerLayerID
			//����������ЩinnerLayerID==-1(��û���)�ĸ���������ڸ�����innerLayerID==curInnerLayerID-1�ߣ�����Ϊ����
			bool alldone=true;//�Ƿ���ȫ������꣬��ʼ�������Ѿ���ȫ�����
			for(int i=0;i<m_mat;i++){
				for(int j=0;j<n_mat;j++){
					if(mat[i][j]->innerLayerID==-1){
						alldone=false;//�������ҵ���innerLayerID==-1�ߣ����ܶ϶���ȫ�������
						//������Χ�˸������Ƿ���innerLayerID����curInnerLayerID-1��
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
						}//�õ�have
						if(have){
							mat[i][j]->innerLayerID=curInnerLayerID;
						}
					}
				}
			}
			if(alldone)break;//���ȫ������꣬���˳�
			curInnerLayerID++;
		}
		maxInnerLayerID=curInnerLayerID-1;//curInnerLayerIDû�ҵ���ң������Ƴ�ѭ����˵������innerLayerID��curInnerLayerID-1
	

		
	
	}
	fillH_innerLayerID(float bottomH,float topH){//��innerLayerID���߶�

		float dH=(topH-bottomH)/maxInnerLayerID;
		for(int i=0;i<m_mat;i++){
			for(int j=0;j<n_mat;j++){
				mat[i][j]->v[Y]=bottomH+mat[i][j]->innerLayerID*dH;
				
			}
		}


	}
	
	HsmoothEx(){//����needcut==true��cell�Ľӽ��̶Ȳ�ͬ����ƽ��
		int r=4;//�Ը�Ϊ��λ��ƽ��������ľ�
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
	//��ֵ����ƽ��
	{
		//�þŸ���ĸ߶�ֵƽ��ֵ�����м��߶�ֵ
		//��������vlt[Y],vrt[Y],vup[Y],vdn[Y]
		for(int i=1;i<=m_mat-2;i++){//iȡ1��m_mat-2��Ϊ�˷�ֹ�ڸ�Խ��
			for(int j=1;j<=n_mat-2;j++){//jȡ1��n_mat-2��Ϊ�˷�ֹ�ڸ�Խ��
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
				//outcomer�ĸ��ƽ���޹��ף�����partoutcomer�ĸ��ƽ���й��ף���Ϊpartoutcomer�ĸ�ĸ߶�ֵ����Ļ��ǱȽ�׼ȷ��
				//
				if(cell.empty){
					
				}else{
					if(!cell.isCorner&&!cell.isCorner_rough){//������ǽǵ�
						//��centerK�涨��Ȩ����
						avrgH+=centerK*cell.v[Y];	
						count+=centerK;
					}else{//����ǽǵ�
						if(cell.isCorner){
							//���ǳ����Ȩ������
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
		//��ƽ
		//��ÿ���񣬸��ݱ����ڸ������������¼���˸��Hlt,Hrt,Hup,Hdn,Hlup,Hldn,Hrup,Hrdn
	{
		
		//��������vlt[Y],vrt[Y],vup[Y],vdn[Y]
		for(int i=1;i<=m_mat-2;i++){//iȡ1��m_mat-2��Ϊ�˷�ֹ�ڸ�Խ��
			for(int j=1;j<=n_mat-2;j++){//jȡ1��n_mat-2��Ϊ�˷�ֹ�ڸ�Խ��
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
		//��������vlup[Y],vldn[Y],vrup[Y],vrdn[Y]����һ��������ȫ��������vlt[Y],vrt[Y],vup[Y],vdn[Y]֮����У�
		for(i=1;i<=m_mat-2;i++){//iȡ1��m_mat-2��Ϊ�˷�ֹ�ڸ�Խ��
			for(int j=1;j<=n_mat-2;j++){//jȡ1��n_mat-2��Ϊ�˷�ֹ�ڸ�Խ��
				Ccell&cell=*mat[i][j];
				Ccell&upcell=*mat[i-1][j];
				Ccell&dncell=*mat[i+1][j];
				Ccell&ltcell=*mat[i][j-1];
				Ccell&rtcell=*mat[i][j+1];
				Ccell&lupcell=*mat[i-1][j-1];
				Ccell&ldncell=*mat[i+1][j-1];
				Ccell&rupcell=*mat[i-1][j+1];
				Ccell&rdncell=*mat[i+1][j+1];
				//���ϡ��¡����ҡ����ϡ����¡����ϡ������ڸ��Ƿ�����
				bool upcount;//���ڸ��Ƿ�����
				bool dncount;//���ڸ��Ƿ�����
				bool ltcount;//���ڸ��Ƿ�����
				bool rtcount;//���ڸ��Ƿ�����
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
				//����cell.vlup[Y]
				//����upcount,ltcount��״̬
				if(!upcount&&!ltcount){//00
					//  ��
					//��[]
					//cell.vlup[Y]���ֲ���
				}else if(!upcount&&ltcount){//01
					//  ��
					//��[]
					//��ʱ��Ҫ�������ϸ������Ƿ�����
					if(ltcell.upJump){//���ϸ���������
						//�����������cell.vlup[Y]
						cell.vlup[Y]=(ltcell.vup[Y]+cell.vup[Y])/2;
					}else{//���ϸ���������
						//��Ӧ���������ϸ�ͬ����cell.vlup[Y]
						cell.vlup[Y]=(ltcell.vup[Y]+cell.vup[Y]+lupcell.vrt[Y]+cell.vlt[Y])/4;
					}
					
				}else if(upcount&&!ltcount){//10
					//  ��
					//��[]
					//��ʱ��Ҫ�������ϸ���ϸ��Ƿ�����
					if(upcell.ltJump){//���ϸ���ϸ�����
						//������ϸ����cell.vlup[Y]
						cell.vlup[Y]=(upcell.vlt[Y]+cell.vlt[Y])/2;
					}else{//���ϸ���ϸ�����
						//��Ӧ���ϸ�����ϸ�ͬ����cell.vlup[Y]
						cell.vlup[Y]=(upcell.vlt[Y]+cell.vlt[Y]+lupcell.vdn[Y]+cell.vup[Y])/4;

					}
				}else if(upcount&&ltcount){//11
					//  ��
					//��[]
					cell.vlup[Y]=(ltcell.vup[Y]+ltcell.vrt[Y]+upcell.vlt[Y]+upcell.vdn[Y])/4;
					
				}
				//����cell.vldn[Y]
				//����dncount,ltcount��״̬
				if(!dncount&&!ltcount){//00
					//��[]
					//  ��
					//cell.vldn[Y]���ֲ���
				}else if(!dncount&&ltcount){//01
					//��[]
					//  ��
					//��ʱ��Ҫ�������¸������Ƿ�����
					if(ltcell.dnJump){//���¸���������
						//�����������cell.vldn[Y]
						cell.vldn[Y]=(ltcell.vdn[Y]+cell.vdn[Y])/2;
					}else{//���¸���������
						//��Ӧ���������¸�ͬ����cell.vldn[Y]
						cell.vldn[Y]=(ltcell.vdn[Y]+cell.vdn[Y]+ldncell.vrt[Y]+cell.vlt[Y])/4;
					}
				}else if(dncount&&!ltcount){//10
					//��[]
					//  ��
					//��ʱ��Ҫ�������¸���¸��Ƿ�����
					if(dncell.ltJump){//���¸���¸�����
						//������¸����cell.vldn[Y]
						cell.vldn[Y]=(dncell.vlt[Y]+cell.vlt[Y])/2;
					}else{//���¸���¸�����
						//�����¸�����¸�ͬ����cell.vldn[Y]
						cell.vldn[Y]=(dncell.vlt[Y]+cell.vlt[Y]+ldncell.vup[Y]+cell.vdn[Y])/4;
					}
				}else if(dncount&&ltcount){//11
					//��[]
					//  ��
					cell.vldn[Y]=(ltcell.vdn[Y]+ltcell.vrt[Y]+dncell.vlt[Y]+dncell.vup[Y])/4;
					
				}
				
				
				//����cell.vrup[Y]
				//����upcount,rtcount��״̬
				if(!upcount&&!rtcount){//00
					//��
					//[]��
					//cell.vrup[Y]���ֲ���
				}else if(!upcount&&rtcount){//01
					//��
					//[]��
					//��ʱ��Ҫ�������ϸ���Ҹ��Ƿ�����
					if(rtcell.upJump){//���ϸ���Ҹ�����
						//������Ҹ����cell.vrup[Y]
						cell.vrup[Y]=(cell.vup[Y]+rtcell.vup[Y])/2;
					}else{//���ϸ���Ҹ�����
						//��Ӧ���Ҹ�����ϸ�ͬ����cell.vrup[Y]
						cell.vrup[Y]=(cell.vup[Y]+rtcell.vup[Y]+rupcell.vlt[Y]+cell.vrt[Y])/4;
					}
				}else if(upcount&&!rtcount){//10
					//��
					//[]��
					//��ʱ��Ҫ�������ϸ���ϸ��Ƿ�����
					if(upcell.rtJump){//���ϸ���ϸ�����
						//������ϸ����cell.vrup[Y]
						cell.vrup[Y]=(upcell.vrt[Y]+cell.vrt[Y])/2;
					}else{//���ϸ���ϸ�����
						//��Ӧ���ϸ�����ϸ�ͬ����cell.vrup[Y]
						cell.vrup[Y]=(upcell.vrt[Y]+cell.vrt[Y]+rupcell.vdn[Y]+cell.vup[Y])/4;
					}
				}else if(upcount&&rtcount){//11
					//��
					//[]��
					cell.vrup[Y]=(upcell.vdn[Y]+upcell.vrt[Y]+rtcell.vlt[Y]+rtcell.vup[Y])/4;
					
				}
				
				//����cell.vrdn[Y]
				//����dncount,rtcount��״̬
				if(!dncount&&!rtcount){//00
					//[]��
					//��
					//cell.vrdn[Y]���ֲ���
				}else if(!dncount&&rtcount){//01
					//[]��
					//��
					//��ʱ��Ҫ�������¸���Ҹ��Ƿ�����
					if(rtcell.dnJump){//���¸���Ҹ�����
						//������Ҹ����cell.vrdn[Y]
						cell.vrdn[Y]=(cell.vdn[Y]+rtcell.vdn[Y])/2;
					}else{//���¸���Ҹ�����
						//��Ӧ���Ҹ�����¸��ڼ���cell.vrdn[Y]
						cell.vrdn[Y]=(cell.vdn[Y]+rtcell.vdn[Y]+rdncell.vlt[Y]+cell.vrt[Y])/4;
					}
				}else if(dncount&&!rtcount){//10
					//[]��
					//��
					//��ʱ��Ҫ�������¸���¸��Ƿ�����
					if(dncell.rtJump){//���¸���¸�����
						//������¸����cell.vrdn[Y]
						cell.vrdn[Y]=(cell.vrt[Y]+dncell.vrt[Y])/2;
					}else{//���¸���¸�����
						//��Ӧ���¸�����¸�ͬ����cell.vrdn[Y]
						cell.vrdn[Y]=(cell.vrt[Y]+dncell.vrt[Y]+rdncell.vup[Y]+cell.vdn[Y])/4;
					}
				}else if(dncount&&rtcount){//11
					//[]��
					//��
					cell.vrdn[Y]=(rtcell.vlt[Y]+rtcell.vdn[Y]+dncell.vup[Y]+dncell.vrt[Y])/4;
				}
				
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
	transformBack(){//ת��������ռ�
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
		//������Χ�˸��߽��ĸ߶�ֵ�����������ĵ�߶�ֵ��ͬ
		//�������ĸ�߶ȷ������ʱ仯�ź����Ҫ����initOcH�����ֻ�Ƕ����ĸ�߶Ƚ���һЩƽ������Ҫ��initOcH
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
		//��ȱʧԪ�ؽ���һ���޲����þ�ֵ�����޲���
		//����ȥ���޲���cell����
	{
		int count=0;
		for(int i=0;i<m_mat;i++){
			for(int j=0;j<n_mat;j++){
				Ccell&cell=*mat[i][j];
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
							if(mat[I][J]->empty)continue;
							HList.push_back(mat[I][J]->v[Y]);
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
	makeUp_lowest()
		//��ȱʧԪ�ؽ����޲�������͵�����޲���
	{
		for(int i=0;i<m_mat;i++){
			for(int j=0;j<n_mat;j++){
				Ccell&cell=*mat[i][j];
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
							if(mat[I][J]->empty)continue;
							HList.push_back(mat[I][J]->v[Y]);
						}
					}//�õ�HList
					if((int)HList.size()==0){
						continue;
					}
					//��HList����͵�
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
		//��������Ԫ��
		{
			for(i=0;i<m_mat;i++){
				for(int j=0;j<n_mat;j++){
					mat[i][j]=new Ccell();
				}
			}

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
			mat[I][J]->IDList.push_back(i);
		}//mat��Ԫ�ص�IDList��������
		//��mat��Ԫ�ص�v
		float hfdz=zstepLen/2;
		float hfdx=xstepLen/2;
		for(i=0;i<m_mat;i++){
			for(int j=0;j<n_mat;j++){
				Ccell&cell=*mat[i][j];
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
		//���H�İ˸��߽��vlt,vrt,vup,vdn,vlup,vldn,vrup,vrdn
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
		for(int i=0;i<=m-2;i+=2){//iȡ1��m-2��Ϊ�˷�ֹ�ڸ�Խ��
			for(int j=0;j<=n-2;j+=2){//jȡ1��n-2��Ϊ�˷�ֹ�ڸ�Խ��
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
					//һ��һ���ػ�
					Ccell*pcell[4];
					pcell[0]=pcell0;
					pcell[1]=pcell1;
					pcell[2]=pcell2;
					pcell[3]=pcell3;
					for(int k=0;k<=3;k++){
						if(pcell[k]==NULL||pcell[k]->empty)continue;
						Ccell&cell=*pcell[k];
						
						//��������б�
						if(cell.needcut){
							int npolygen=(int)cell.polygenList.size();
							for(int _i=0;_i<npolygen;_i++){
								Cpolygen&polygen=cell.polygenList[_i];
								if(polygen.removed)continue;
								//��polygen
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
					//�����ĸ�cell��ɵ��棬����ȡ���ĵ�ķ�����
					//    cell0.vlup--cell3.vrup
					//     |                  |
					//     |			      |
					//     |                  |
					//    cell1.vldn--cell2.vrdn
					glBegin(GL_QUADS);
					glNormal3fv(pcell0->avrgnorm);//Ϊ�˽�Լ������������cell0�ķ���������������������
					glTexCoord2fv(pcell3->urup);glVertex3fv(pcell3->vrup);
					glTexCoord2fv(pcell0->ulup);glVertex3fv(pcell0->vlup);
					glTexCoord2fv(pcell1->uldn);glVertex3fv(pcell1->vldn);
					glTexCoord2fv(pcell2->urdn);glVertex3fv(pcell2->vrdn);
					glEnd();
				}


	
			}
		}
			//�޲��ѷ�
		glBegin(GL_TRIANGLES);
		for(i=0;i<=m-2;i+=2){//iȡ1��m-2��Ϊ�˷�ֹ�ڸ�Խ��
			for(int j=0;j<=n-2;j+=2){//jȡ1��n-2��Ϊ�˷�ֹ�ڸ�Խ��
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
				if(pcell0==NULL||!pcell0->asWhole4){//�����2*2�����и����Ƿ��Ż���
					//����2*2�������������2*2����
					//����2*2����
					if(i-2>=0&&(mat[i-2][j]!=NULL&&mat[i-2][j]->asWhole4))
					{//��2*2���������廭��
						//    cellA.vldn--cellA.vrdn---cellB.vrdn
						//    cell0.vlup--cell0.vrup---cell3.vrup
						//     |   cell0      |    cell3     |
						//    cell0.vldn--cell0.vrdn---cell3.vrdn
						//     |   cell1      |    cell2     |
						//    cell1.vldn--cell1.vrdn---cell2.vrdn
						//�����ϱ߽紦Ҫ�޲�һ���������ѷ�
						Ccell*pcellA=mat[i-1][j];
						Ccell*pcellB=mat[i-1][j+1];
						//��������pcellB->vrdn,pcellA->vldn,pcellA->vrdn
						float norm[4];
						getNormalOfTri(pcellB->vrdn,pcellA->vldn,pcellA->vrdn,norm);
						glNormal3fv(norm);
						glTexCoord2fv(pcellB->urdn);glVertex3fv(pcellB->vrdn);
						glTexCoord2fv(pcellA->uldn);glVertex3fv(pcellA->vldn);
						glTexCoord2fv(pcellA->urdn);glVertex3fv(pcellA->vrdn);
					}
					//����2*2����
					if(i+2<m&&(mat[i+2][j]!=NULL&&mat[i+2][j]->asWhole4))
					{//��2*2���������廭��
						//    cell0.vlup--cell0.vrup---cell3.vrup
						//     |   cell0      |    cell3     |
						//    cell0.vldn--cell0.vrdn---cell3.vrdn
						//     |   cell1      |    cell2     |
						//    cell1.vldn--cell1.vrdn---cell2.vrdn
						//    cellA.vlup--cellA.vrup---cellB.vrup
						//�����±߽紦Ҫ�޲�һ���������ѷ�
						Ccell*pcellA=mat[i+2][j];
						Ccell*pcellB=mat[i+2][j+1];
						//��������pcellB->vrup,pcellA->vrup,pcellA->vlup
						float norm[4];
						getNormalOfTri(pcellB->vrup,pcellA->vrup,pcellA->vlup,norm);
						glNormal3fv(norm);
						glTexCoord2fv(pcellB->urup);glVertex3fv(pcellB->vrup);
						glTexCoord2fv(pcellA->urup);glVertex3fv(pcellA->vrup);
						glTexCoord2fv(pcellA->ulup);glVertex3fv(pcellA->vlup);
					}
					//����2*2����
					if(j-2>=0&&(mat[i][j-2]!=NULL&&mat[i][j-2]->asWhole4))
					{//��2*2���������廭��
						//  cellA.vrup  cell0.vlup--cell0.vrup---cell3.vrup
						//		 |		  |   cell0      |    cell3     |
						//  cellA.vrdn  cell0.vldn--cell0.vrdn---cell3.vrdn
						//       |        |   cell1      |    cell2     |
						//  cellB.vrdn  cell1.vldn--cell1.vrdn---cell2.vrdn
						//������߽紦Ҫ�޲�һ���������ѷ�
						Ccell*pcellA=mat[i][j-1];
						Ccell*pcellB=mat[i+1][j-1];
						//��������pcellB->vrdn,pcellA->vrdn,pcellA->vrup
						float norm[4];
						getNormalOfTri(pcellB->vrdn,pcellA->vrdn,pcellA->vrup,norm);
						glNormal3fv(norm);
						glTexCoord2fv(pcellB->urdn);glVertex3fv(pcellB->vrdn);
						glTexCoord2fv(pcellA->urdn);glVertex3fv(pcellA->vrdn);
						glTexCoord2fv(pcellA->urup);glVertex3fv(pcellA->vrup);
					}
					//����2*2����
					if(j+2<n&&(mat[i][j+2]!=NULL&&mat[i][j+2]->asWhole4))
					{//��2*2���������廭��
						//  cell0.vlup--cell0.vrup---cell3.vrup   cellA.vlup
						//	  |   cell0      |    cell3     |	       |
						//  cell0.vldn--cell0.vrdn---cell3.vrdn   cellA.vldn 
						//    |   cell1      |    cell2     |          |
						//  cell1.vldn--cell1.vrdn---cell2.vrdn   cellB.vldn 
						//�����ұ߽紦Ҫ�޲�һ���������ѷ�
						Ccell*pcellA=mat[i][j+2];
						Ccell*pcellB=mat[i+1][j+2];
						//��������pcellA->vlup,pcellA->vldn,pcellB->vldn
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
	showMesh_rough4(GLuint texture){//��showMesh_rough3�������ǲ��޲��ѷ�
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, texture);
		int m=m_mat;
		int n=n_mat;
		float dx=xstepLen;
		float dz=zstepLen;
		float hfdx=dx/2;
		float hfdz=dz/2;
		for(int i=0;i<=m-2;i+=2){//iȡ1��m-2��Ϊ�˷�ֹ�ڸ�Խ��
			for(int j=0;j<=n-2;j+=2){//jȡ1��n-2��Ϊ�˷�ֹ�ڸ�Խ��
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
					//һ��һ���ػ�
					Ccell*pcell[4];
					pcell[0]=pcell0;
					pcell[1]=pcell1;
					pcell[2]=pcell2;
					pcell[3]=pcell3;
					for(int k=0;k<=3;k++){
						if(pcell[k]==NULL||pcell[k]->empty)continue;
						Ccell&cell=*pcell[k];
						
						//��������б�
						if(cell.needcut){
							int npolygen=(int)cell.polygenList.size();
							for(int _i=0;_i<npolygen;_i++){
								Cpolygen&polygen=cell.polygenList[_i];
								if(polygen.removed)continue;
								//��polygen
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
					//�����ĸ�cell��ɵ��棬����ȡ���ĵ�ķ�����
					//    cell0.vlup--cell3.vrup
					//     |                  |
					//     |			      |
					//     |                  |
					//    cell1.vldn--cell2.vrdn
					glBegin(GL_QUADS);
					glNormal3fv(pcell0->avrgnorm);//Ϊ�˽�Լ������������cell0�ķ���������������������
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
		for(int i=0;i<=m-2;i+=2){//iȡ1��m-2��Ϊ�˷�ֹ�ڸ�Խ��
			for(int j=0;j<=n-2;j+=2){//jȡ1��n-2��Ϊ�˷�ֹ�ڸ�Խ��
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
					//һ��һ���ػ�
					Ccell*pcell[4];
					pcell[0]=pcell0;
					pcell[1]=pcell1;
					pcell[2]=pcell2;
					pcell[3]=pcell3;
					for(int k=0;k<=3;k++){
						if(pcell[k]==NULL||pcell[k]->empty)continue;
						Ccell&cell=*pcell[k];
						if(!drawNeedCut&&cell.needcut)continue;//�������needcut���Ҵ˸�ǡΪneedcut��������
						
						glNormal3fv(cell.avrgnorm);
						glTexCoord2fv(cell.urup);glVertex3fv(cell.vrup);
						glTexCoord2fv(cell.ulup);glVertex3fv(cell.vlup);
						glTexCoord2fv(cell.uldn);glVertex3fv(cell.vldn);
						glTexCoord2fv(cell.urdn);glVertex3fv(cell.vrdn);

					}
					
				}else{
					if(pcell0!=NULL)pcell0->asWhole4=true;
					//�����ĸ�cell��ɵ��棬����ȡ���ĵ�ķ�����
					//    cell0.vlup--cell3.vrup
					//     |                  |
					//     |			      |
					//     |                  |
					//    cell1.vldn--cell2.vrdn
					glNormal3fv(pcell0->avrgnorm);//Ϊ�˽�Լ������������cell0�ķ���������������������
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
		for(int i=0;i<m;i++){//iȡ1��m-2��Ϊ�˷�ֹ�ڸ�Խ��
			for(int j=0;j<n;j++){//jȡ1��n-2��Ϊ�˷�ֹ�ڸ�Խ��
				if(mat[i][j]==NULL)continue;
				Ccell&cell=*mat[i][j];//��ǰ��
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
//ģ�ͻ���
class Cmodel{
public:
	float RMmat[16];//��תƽ�ƾ���
	Cmodel(){
		makeE(RMmat);
	}
	reset(){//��λ
		makeE(RMmat);
	
	}
	virtual void show(){
	
	}
	void setloc(float x,float y,float z){
		RMmat[12]=x;
		RMmat[13]=y;
		RMmat[14]=z;
	
	}

	void movloc(float dx,float dy,float dz){//����ھֲ�����ϵ�ƶ��ֲ�����
		//����d����Ӧ��ƽ�ƾ���
		float movmat[16]={
			1,0,0,0,//��һ��
            0,1,0,0,//�ڶ���
			0,0,1,0,//������
			dx,dy,dz,1//������
		};
		prodm(RMmat,movmat,RMmat);

	}
	void rotloc(float nx,float ny,float nz,float tha){//�����������ת�ֲ�����
		//n�����ǵ�λ����
		float cosa=cos(tha*pi/180);
		float sina=sin(tha*pi/180);
		float _cosa=1-cosa;
		if(nx==0&&nz==0){
			float rotmat[16]={
				cosa,0,-sina,0,//��һ��
				0,1,0,0,//�ڶ���
				sina,0,cosa,0,//������
				0,0,0,1//������
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
	void rotloc_inFather(float nx,float ny,float nz,float tha){//�ڸ�����ϵ����ת�ֲ�����ϵ
		//n�����ǵ�λ����
		float cosa=cos(tha*pi/180);
		float sina=sin(tha*pi/180);
		float _cosa=1-cosa;
		if(nx==0&&nz==0){
			float rotmat[16]={
				cosa,0,-sina,0,//��һ��
				0,1,0,0,//�ڶ���
				sina,0,cosa,0,//������
				0,0,0,1//������
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
	void movloc_inFather(float dx,float dy,float dz){//�ڸ�����ϵ���ƶ��ֲ�����ϵ
		//����d����Ӧ��ƽ�ƾ���
		float movmat[16]={
			1,0,0,0,//��һ��
            0,1,0,0,//�ڶ���
			0,0,1,0,//������
			dx,dy,dz,1//������
		};
		prodm(movmat,RMmat,RMmat);
		
		
	}
	toWorld(float x,float y,float z,float type,float v[4]){//���ֲ�����(x,y,z,type)ת������������v
		//type:1���0������
		float _v[4]={x,y,z,type};
		toWorld(_v,v);
	}
	toWorld(float _v[4],float v[4]){//���ֲ�����_vת������������v
		prodv(RMmat,_v,v);
	
	}

};
//�۲���
class Cme:public Cmodel
{
public:
	void show(){
		glPushMatrix();
		glMultMatrixf(RMmat);

		
		//----��������
		//    vup1(-d,d,-d) ________  vup0(d,d,-d)
		//                 /|      /|
		//                / |     / |
		// vup2(-d,d,d)  |--+----|  | vup3(d,d,d) 
		//  vdn1(-d,0,-d)|  /----|--/ vdn0(d,0,-d)
		//               | /     | /
		//               |/______|/ 
		// vdn2(-d,0,d)           vdn3(d,0,d)
		float d=0.6;//�߳�һ��
		float vup0[4],vup1[4],vup2[4],vup3[4],vdn0[4],vdn1[4],vdn2[4],vdn3[4];//�˸�����
		init4(vup0,d,d,-d,1);
		init4(vup1,-d,d,-d,1);
		init4(vup2,-d,d,d,1);
		init4(vup3,d,d,d,1);
		init4(vdn0,d,0,-d,1);
		init4(vdn1,-d,0,-d,1);
		init4(vdn2,-d,0,d,1);
		init4(vdn3,d,0,d,1);
		float norm_up[4],norm_dn[4],norm_lt[4],norm_rt[4],norm_ft[4],norm_bk[4];//������ķ�����
		init4(norm_up,0,1,0,0);
		init4(norm_dn,0,-1,0,0);
		init4(norm_lt,-1,0,0,0);
		init4(norm_rt,1,0,0,0);
		init4(norm_ft,0,0,1,0);
		init4(norm_bk,0,0,-1,0);
		//��������
		//	glEnable(GL_TEXTURE_2D);
		//	glBindTexture(GL_TEXTURE_2D, texture[0]);
		glBegin(GL_QUADS);
		//����
		glNormal3fv(norm_up);
		glTexCoord2f(1,1);glVertex3fv(vup0);
		glTexCoord2f(0,1);glVertex3fv(vup1);
		glTexCoord2f(0,0);glVertex3fv(vup2);
		glTexCoord2f(1,0);glVertex3fv(vup3);
		//����
		glNormal3fv(norm_dn);
		glTexCoord2f(1,0);glVertex3fv(vdn3);
		glTexCoord2f(0,0);glVertex3fv(vdn2);
		glTexCoord2f(0,1);glVertex3fv(vdn1);
		glTexCoord2f(1,1);glVertex3fv(vdn0);
		//ǰ��
		glNormal3fv(norm_ft);
		glTexCoord2f(1,1);glVertex3fv(vup3);
		glTexCoord2f(0,1);glVertex3fv(vup2);
		glTexCoord2f(0,0);glVertex3fv(vdn2);
		glTexCoord2f(1,0);glVertex3fv(vdn3);
		//����
		glNormal3fv(norm_bk);
		glTexCoord2f(1,0);glVertex3fv(vdn0);
		glTexCoord2f(0,0);glVertex3fv(vdn1);
		glTexCoord2f(0,1);glVertex3fv(vup1);
		glTexCoord2f(1,1);glVertex3fv(vup0);
		//����
		glNormal3fv(norm_lt);
		glTexCoord2f(0,0);glVertex3fv(vdn2);
		glTexCoord2f(0,1);glVertex3fv(vup2);
		glTexCoord2f(1,1);glVertex3fv(vup1);
		glTexCoord2f(1,0);glVertex3fv(vdn1);
		//����
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
	myvector<CIDTriangle*> pIDtriList;//����������ָ���б�
	~Cpatch(){
		//��pIDtriList�ж�̬����Ķ���ɾ��
		{
			int nIDtri=(int)pIDtriList.size();
			for(int j=0;j<nIDtri;j++){
				CIDTriangle*pIDtri=pIDtriList[j];
				//ɾ��pIDtri
				delete pIDtri;
			}
		}
		
	}
};

class CIDpolygen{//���������
public:
	myvector<int> vIDList;//���������б�
};

class CIDedge{//������
public:
	int ID1;//�������
	int ID2;//�յ�����
	bool added;
	CIDedge(){
		added=false;
	}
	setvID(int _ID1,int _ID2){
		ID1=_ID1;
		ID2=_ID2;
	}
};

class CIDloosePolygen{//������ɢ�����
public:
	myvector<CIDedge> IDedgeList;//һ����ɢ������ɶ�����������
};
class Crect{
public:
	float origin[4];//ԭ��
	float xdir[4];//�ֲ�x��������
	float zdir[4];//�ֲ�z��������
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
		//��rect
		float v[4][4];//����4��������rect����ϵ�µ�����
		init4(v[0],xmin,0,zmin,1);
		init4(v[1],xmin,0,zmax,1);
		init4(v[2],xmax,0,zmax,1);
		init4(v[3],xmax,0,zmin,1);
		//��vת����������ϵ
		//����ת������������ƽ�У���ԭ����origin������ϵ����Ϊ��ƽ������ϵ��
		//ƽ������ϵx����rect�е�����Ϊ
		float xvec[4]={xdir[X],0,-xdir[Z],0};
		//ƽ������ϵz����rect�е�����Ϊ
		float zvec[4]={xdir[Z],0,xdir[X],0};
		for(int k=0;k<=3;k++){
			float originvk[4]={v[k][X],0,v[k][Z],0};//��originָ��v[k]��������rect�ռ��У�
			//��originv[k]ת��ƽ������ϵ
			//����originv[k]��xvec��zvecͶӰ
			float x=dot(originvk,xvec);//originv[k]��xvec�ϵ�ͶӰ����v[k]��ƽ�пռ��е�x����
			float z=dot(originvk,zvec);//originv[k]��zvec�ϵ�ͶӰ����v[k]��ƽ�пռ��е�z����
			//��ƽ�пռ�ת������ռ�
			x+=origin[X];
			z+=origin[Z];
			//��v[k]Ϊ����ռ�����
			init4(v[k],x,H,z,1);
		}//�õ�v[4][4]����������
		//��v[4][4]��ȷ���ľ���
		//����
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



class CIDoutline{//һ��IDoutline������������
public:
	//----����
	myvector<CIDloosePolygen> IDloosePolygenList;//��ɢ������б�
	myvector<CIDpolygen> IDpolygenList;//����������б�
	                                   //��ע�⣬��Ȼ������б�������ɢ������б����ɵģ�
	                                   //���Ƕ����������Ķ��������δ����ͬ������һ����ɢ
	                                   //��������ɶ������Ρ�ԭ������һ����ɢ����ζ�Ӧ
	                                   //patch��һ����ͨ��֧����һ����ͨ��֧����ж��Ļ�
	                                   //���Ӧ�����߽߱磩
	myvector<Cpolygen> polygenList;//������������б�
	myvector<Cpolygen> polygenList_dn;//������������б�

};


class CIDgrid
//������ײ����
{
public:
	int m_IDListmat;//IDlistmat������
	int n_IDListmat;//IDlistmat������
	float xstepLen;//x����
	float zstepLen;//z����
	float xmin,xmax,zmin,zmax;//xz��Χ
	myvector<myvector<myvector<int> > > IDListmat;//��ײ����
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
	makeIDgrid(float _xmin,float _xmax,float _zmin,float _zmax,//��Χ
		float _xstepLen,float _zstepLen,//����
		myvector<Cv4>&_vlist//����
		)
		//��_xmin,_xmax,_zmin,_zmax��Χ��Ϊ����_vlist�����������񲽳�ԼΪ_xstepLen,_zstepLen
		//����ǽ���
	{
		//�������IDListmat
		IDListmat.resize(0);
		//�������
		xstepLen=_xstepLen;
		zstepLen=_zstepLen;
		xmin=_xmin;
		xmax=_xmax;
		zmin=_zmin;
		zmax=_zmax;
		//�趨����ߴ�
		m_IDListmat=(_zmax-_zmin)/zstepLen;
		n_IDListmat=(_xmax-_xmin)/xstepLen;
		//����m_IDListmat��n_IDListmatУ������
		zstepLen=(_zmax-_zmin)/m_IDListmat;
		xstepLen=(_xmax-_xmin)/n_IDListmat;
		//��������
		IDListmat.resize(m_IDListmat);
		for(int i=0;i<m_IDListmat;i++){
			IDListmat[i].resize(n_IDListmat);
		}
		//��_vlist�е����е�ַ�������
		int _nv=(int)_vlist.size();
		for(i=0;i<_nv;i++){
			Cv4&v=_vlist[i];
			if(v.wallpoint)continue;
			//��v�����ĸ���
			int I,J;//v����(I,J)��
			I=(v.v[Z]-_zmin)/zstepLen;
			J=(v.v[X]-_xmin)/xstepLen;
			if(I<0||J<0||I>m_IDListmat-1||J>n_IDListmat-1)continue;
			//��v����˸�
			IDListmat[I][J].push_back(i);
		}//����������
		
	}
	void getIDList_oc(float x,float z,myvector<int>&IDList)
		//��ñ��񼰰��ڸ������ж����ID�б�
		//�����IDList����Ϊ��
	{
		//��(x,z)���ڵĸ�
		int I,J;//(x,z)����(I,J)��
		I=(z-zmin)/zstepLen;
		J=(x-xmin)/xstepLen;
		if(I<0||J<0||I>m_IDListmat-1||J>n_IDListmat-1){
			return;
		}
		//��
		//IDListmat[I-1][J-1]  IDListmat[I-1][J]  IDListmat[I-1][J+1]
		//IDListmat[I][J-1]    IDListmat[I][J]    IDListmat[I][J+1]
		//IDListmat[I+1][J-1]  IDListmat[I+1][J]  IDListmat[I+1][J+1]
		//���ڵĵ��ID�浽IDList
		int _I,_J;//_Iȡ����I-1,I,I+1��_Jȡ��J-1,J,J+1
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
		}//�õ�IDList
		
	}
	

};
class Ccloud{
public:

	myvector<Cv4> vlist;//���б�
	myvector<Cpolygen> polygenList;//������������б�
	myvector<Cpolygen> polygenList_dn;//������������б�
	myvector<Cpolygen> polygenList_smoothRes;//ƽ���������
	Cgrid grid;//��������
	float avrgH;//�ݶ�ƽ���߶�
	float bottomH;//�ݶ����̸߶�
	float topH;//�ݶ���߸߶�
	float minH;//����ǽ����͵�߶�
	bool removed;//�Ƿ���ɾ��
	float color[4];//��ɫ
	float avrgnorm[4];//���ŵ�ƽ��������
	float xmin,xmax,zmin,zmax;//���Ʒ�Χ
	bool drawed;//�Ƿ��ѻ���
	int hostID;//�������ź�
	bool selfHost;//�Ƿ�Ϊ��������
	Crect minRect;//��С��Χ����
	Cv4 mainDir;//������
	bool mainDirGot;
	bool mainDirChanged;
	bool isRed;//�Ƿ��Ժ�ɫ��ʾ
	bool takenOut;//�Ƿ���polygen��������������ɵ�
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
		//���ٶ�̬����
		grid.clearMat();

	
	}
	copy(Ccloud&cloud){//*this����cloud
		//������̬����
		*this=cloud;
		//������̬����
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

