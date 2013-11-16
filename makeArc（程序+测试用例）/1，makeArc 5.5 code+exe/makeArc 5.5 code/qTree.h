
class CqNode{//�����Ĳ����ڵ�
public:
	float xmin,xmax,zmin,zmax;
	float ymin,ymax;
	myvector<int> cloudIDList;//����ID�б�
	CqNode*p[4];//�ĸ�����ָ��
	            //  0 3
	            //  1 2
	CqNode(){
		p[0]=p[1]=p[2]=p[3]=NULL;
		xmin=xmax=zmin=zmax=0;
		ymin=ymax=0;
	}

};
class CqTree{//�����Ĳ���
public:
	GLuint textureRoof;
	GLuint textureWall;
	CqNode*hd;//��ָ��
	CqTree(){
		hd=NULL;
	}
	destroy(){
		textureRoof=0;
		textureWall=0;
		//��hd������
		destroyTree(hd);
		hd=NULL;
	}
	void destroyTree(CqNode*p){
		if(p==NULL)return;
		//�����ĸ�����
		for(int i=0;i<4;i++){
			destroyTree(p->p[i]);
		}
		//�����Լ�
		delete p;
		p=NULL;
	}
	setTextureRoof(GLuint _textureRoof){
		textureRoof=_textureRoof;
	}
	setTextureWall(GLuint _textureWall){
		textureWall=_textureWall;
	
	}
	makeqTree(float xmin,float xmax,float zmin,float zmax,
		float ymin,float ymax,
		myvector<Ccloud>&cloudList){//�����Ĳ���
		//���ɸ��ڵ�
		hd=new CqNode();
		hd->xmin=xmin;
		hd->xmax=xmax;
		hd->zmin=zmin;
		hd->zmax=zmax;
		hd->ymin=ymin;
		hd->ymax=ymax;
		int nv=(int)cloudList.size();
		for(int i=0;i<nv;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			hd->cloudIDList.push_back(i);
		}
		//���ɺ���
		makeqTree_inn(hd,cloudList);

	
	}
	void makeqTree_inn(CqNode*&p,myvector<Ccloud>&cloudList){//�����Ĳ����ĵݹ��:�����ڵ�p���ĸ�����
		float xmin,xmax,zmin,zmax;
		xmin=p->xmin;
		xmax=p->xmax;
		zmin=p->zmin;
		zmax=p->zmax;
		float ymin,ymax;
		ymin=p->ymin;
		ymax=p->ymax;

		myvector<int>&cloudIDList=p->cloudIDList;
		
		if((xmax-xmin)*(zmax-zmin)<=5*5)
			return;
		//�����ĸ�����
		p->p[0]=new CqNode();
		p->p[1]=new CqNode();
		p->p[2]=new CqNode();
		p->p[3]=new CqNode();
		//����ĸ����ӵ�xz��Χ
		//����p->p[0]
		{
			float _xmin,_xmax,_zmin,_zmax;
			_xmin=xmin;
			_xmax=(xmin+xmax)/2;
			_zmin=zmin;
			_zmax=(zmin+zmax)/2;
			p->p[0]->xmin=_xmin;
			p->p[0]->xmax=_xmax;
			p->p[0]->zmin=_zmin;
			p->p[0]->zmax=_zmax;
		
		}
		//����p->p[1]
		{
			float _xmin,_xmax,_zmin,_zmax;
			_xmin=xmin;
			_xmax=(xmin+xmax)/2;
			_zmin=(zmin+zmax)/2;
			_zmax=zmax;
			p->p[1]->xmin=_xmin;
			p->p[1]->xmax=_xmax;
			p->p[1]->zmin=_zmin;
			p->p[1]->zmax=_zmax;
		}
		//����p->p[2]
		{
			float _xmin,_xmax,_zmin,_zmax;
			_xmin=(xmin+xmax)/2;
			_xmax=xmax;
			_zmin=(zmin+zmax)/2;
			_zmax=zmax;
			p->p[2]->xmin=_xmin;
			p->p[2]->xmax=_xmax;
			p->p[2]->zmin=_zmin;
			p->p[2]->zmax=_zmax;

		}
		//����p->p[3]
		{
			float _xmin,_xmax,_zmin,_zmax;
			_xmin=(xmin+xmax)/2;
			_xmax=xmax;
			_zmin=zmin;
			_zmax=(zmin+zmax)/2;
			p->p[3]->xmin=_xmin;
			p->p[3]->xmax=_xmax;
			p->p[3]->zmin=_zmin;
			p->p[3]->zmax=_zmax;
		}

		//��cloudIDList��ÿ�����ţ������䵽���䵽�ļ������ӵķ�Χ��
		int ncloudID=(int)cloudIDList.size();
		for(int i=0;i<ncloudID;i++){
			int ID=cloudIDList[i];
			Ccloud&cloud=cloudList[ID];
			if(cloud.removed)continue;
			float _xmin,_xmax,_zmin,_zmax;
			_xmin=cloud.xmin;
			_xmax=cloud.xmax;
			_zmin=cloud.zmin;
			_zmax=cloud.zmax;
			float _cx=(_xmin+_xmax)/2;
			float _cz=(_zmin+_zmax)/2;
			float _dx=(_xmax-_xmin)/2;
			float _dz=(_zmax-_zmin)/2;
			//��cloud�ķ�Χ_xmin,_xmax,_zmin,_zmax���ļ��������ཻ(һ��cloud�������������ཻ)
			for(int j=0;j<=3;j++){
				CqNode*q=p->p[j];
				//��cloud�Ƿ���q�ཻ
				float xmin,xmax,zmin,zmax;
				xmin=q->xmin;
				xmax=q->xmax;
				zmin=q->zmin;
				zmax=q->zmax;
				float cx=(xmin+xmax)/2;
				float cz=(zmin+zmax)/2;
				float dx=(xmax-xmin)/2;
				float dz=(zmax-zmin)/2;
				//��_xmin,_xmax,_zmin,_zmax����xmin,xmax,zmin,zmax�ཻ
				//ֻ�迴�������ľ����Ƿ񳬹����߱��ľ�֮��
				if(fabs(cx-_cx)>dx+_dx||fabs(cz-_cz)>dz+_dz){//���ཻ

				}else{//�ཻ
					//��cloud��ID���뵽q��cloudIDList��
					q->cloudIDList.push_back(ID);
				}
			}
		}
		//���p->cloudIDList
		p->cloudIDList.resize(0);
		//������ӵ�y��Χ
		for(i=0;i<=3;i++){
			CqNode*q=p->p[i];
			//��q��y��Χ
			//ͳ��q->cloudIDList�и����ŵ����ymax�����ymin
			float maxymax=-inf;
			float minymin=inf;
			int ncloudID=(int)q->cloudIDList.size();
			for(int j=0;j<ncloudID;j++){
				int ID=cloudIDList[j];
				Ccloud&cloud=cloudList[ID];
				if(cloud.removed)continue;
				if(cloud.topH>maxymax)maxymax=cloud.topH;
				if(cloud.minH<minymin)minymin=cloud.minH;
			}//�õ�maxymax,minymin
			q->ymax=maxymax;
			q->ymin=minymin;
		}
		
		//��ÿ�����ӽ��еݹ�
		for(i=0;i<=3;i++){
			makeqTree_inn(p->p[i],cloudList);
		
		}
		


	}
	cutAndDrawqTree(myvector<Ccloud>&cloudList,float campos[4]){//ʹ����׶���Ĳ������м��ã�������Ҷ�ڵ�
		
		//---------------------����׶
		//��þ���
		GLint viewport[4];
		GLdouble modelMatrix[16];
		GLdouble projMatrix[16];
		glGetIntegerv(GL_VIEWPORT,viewport);
		glGetDoublev(GL_MODELVIEW_MATRIX  ,modelMatrix);
		glGetDoublev(GL_PROJECTION_MATRIX ,projMatrix);
		float height=winheight;
		float width=winwidth;
		//�ӿڵ��ӿڿռ���������
		//   (0,height,0)---(width,height,0)
		//      |              |
		//      |              |
		//    (0,0,0)--------(width,0,0)
		float p[4][4];//�ӿ��ĸ�������ӿڿռ�����
		init4(p[0],0,0,0,1);
		init4(p[1],width,0,0,1);
		init4(p[2],width,height,0,1);
		init4(p[3],0,height,0,1);
		//��pת��������ռ�
		for(int i=0;i<=3;i++){
			//��p[i]ת��������ռ�
			GLdouble tp[3];
			gluUnProject(p[i][X],p[i][Y],p[i][Z],modelMatrix,projMatrix,viewport,&tp[X],&tp[Y],&tp[Z]);
			init4(p[i],tp[X],tp[Y],tp[Z],1);
		}//p��ת��Ϊ��������
		vector<Cplane> planeList;
		planeList.resize(5);//��׶��ʾΪ���ƽ�棨�ĸ������һ�������棩
		//�������ƽ�棨����ָ����׶�ڲ���
		float norm[5][4];//�����ķ�����
		getNormalOfTri(campos,p[1],p[0],norm[0]);
		getNormalOfTri(campos,p[2],p[1],norm[1]);
		getNormalOfTri(campos,p[3],p[2],norm[2]);
		getNormalOfTri(campos,p[0],p[3],norm[3]);
		getNormalOfTri(p[2],p[1],p[0],norm[4]);
		planeList[0].makePlane(campos,norm[0]);
		planeList[1].makePlane(campos,norm[1]);
		planeList[2].makePlane(campos,norm[2]);
		planeList[3].makePlane(campos,norm[3]);
		planeList[4].makePlane(p[0],norm[4]);//�õ���׶

		//������cloud��drawed����Ϊfalse
		int ncloud=(int)cloudList.size();
		for(i=0;i<ncloud;i++){
			cloudList[i].drawed=false;
		}
		//
		cutAndDrawqTree_inn(hd,planeList,cloudList,campos);

		
	}
	void cutAndDrawqTree_inn(CqNode*&p,vector<Cplane>&planeList,myvector<Ccloud>&cloudList,float campos[4]){
		if(p==NULL)return;
		//�жϽڵ�p�Ƿ�����׶�ཻ
		//��ڵ�p����ʾ����ı��ذ�Χ��
		//    ����
		//    v[0]--v[3]
		//     |     |
		//    v[1]--v[2]
		//    ����
		//    v[4]--v[7]
		//     |     |
		//    v[5]--v[6]
		float v[8][4];
		//����
		init4(v[0],p->xmin,p->ymax,p->zmin,1);
		init4(v[1],p->xmin,p->ymax,p->zmax,1);
		init4(v[2],p->xmax,p->ymax,p->zmax,1);
		init4(v[3],p->xmax,p->ymax,p->zmin,1);
		//����
		init4(v[4],p->xmin,p->ymin,p->zmin,1);
		init4(v[5],p->xmin,p->ymin,p->zmax,1);
		init4(v[6],p->xmax,p->ymin,p->zmax,1);
		init4(v[7],p->xmax,p->ymin,p->zmin,1);
		//��v[0]~v[7]�Ƿ���planeList��ĳ����ķ��棬���������޳�
		bool visible=true;
		for(int i=0;i<=4;i++){
			Cplane&plane=planeList[i];
			//��v[0]~v[7]�Ƿ���plane�ı���
			bool allback=true;//���趼�ڱ���
			for(int j=0;j<=7;j++){
				float side=PND_point_plane(plane,v[j]);
				if(side>0){
					allback=false;
					break;
				}
			}//�õ�allback
			if(allback){//���ȫ�ڱ���
				//�϶�Ϊ���ɼ��������ټ������
				visible=false;
				break;
			}
		}//�õ�visible
		if(visible){//����ɼ�
			//��p�Ƿ�ΪҶ�ڵ�
			if(p->p[0]==NULL){//�����Ҷ�ڵ�
				//�ڵ�����
				float c[4]={(p->xmin+p->xmax)/2,(p->ymin+p->ymax)/2,(p->zmin+p->zmax)/2,1};
				//�ӵ�൱ǰ�ڵ�ľ���
				float d=fabs(c[X]-campos[X])+fabs(c[Y]-campos[Y])+fabs(c[Z]-campos[Z]);
				//�����ݶ�LOD�ȼ�
				int thLOD_roof;//�ݶ�LOD�ȼ�
				{
					if(!singleStepMode){
						if(viewMode==VM_OVERLOOK){
							if(d<=200){
								thLOD_roof=1;
							}else if(d<=800){
								thLOD_roof=2;
							}else{
								thLOD_roof=3;
							}
						}else if(viewMode==VM_WANDER){
							if(d<=50){
								thLOD_roof=1;
							}else if(d<=700){
								thLOD_roof=2;
							}else{//����ģʽ����d�����򲻻�
								thLOD_roof=-1;//thLOD_roofΪ-1���򲻻�
							}
							
						}else{
							cout<<"error! ģʽδ����"<<endl;
							return;
						}
					}else{//singleStepMode
						thLOD_roof=1;
					}
					
				}
				//����ǽ�ڲ���
				int stepLen_wall;
				{
					if(!singleStepMode){
						if(viewMode==VM_OVERLOOK){
							if(d<=200){
								stepLen_wall=1;
							}else if(d<=500){
								stepLen_wall=3;
							}else if(d<=1200){
								stepLen_wall=6;
							}else if(d<=1800){
								stepLen_wall=10;
							}else{
								stepLen_wall=100;
								
							}
						}else if(viewMode==VM_WANDER){
							if(d<=100){
								stepLen_wall=1;
							}else if(d<=300){
								stepLen_wall=3;
							}else if(d<=500){
								stepLen_wall=10;
							}else if(d<=700){
								stepLen_wall=100;
							}else{//����ģʽ����d�����򲻻�
								stepLen_wall=-1;//stepLen_wallΪ-1���򲻻�
							}
							
						}else{
							cout<<"error! ģʽδ����"<<endl;
							return;
						}
						
					}else{//singleStepMode
						stepLen_wall=1;
					}
					
					
				}
				//��p������cloud
				{
					int ncloudID=(int)p->cloudIDList.size();
					for(int j=0;j<ncloudID;j++){
						int ID=p->cloudIDList[j];
						Ccloud&cloud=cloudList[ID];
						if(cloud.removed)continue;
						if(cloud.drawed)continue;
						//���ݶ���
						{
							glDisable(GL_LIGHTING);
							if(cloud.isRed)glColor3f(1,0,0);
							switch(thLOD_roof){
							case 1:
								cloud.grid.showMesh_rough0(textureRoof);
								break;
							case 2:
								cloud.grid.showMesh_rough3(textureRoof);
								break;
							case 3:
								cloud.grid.showMesh_rough4(textureRoof);
								break;
							case -1:
								//����
								break;
							}
							if(cloud.isRed)glColor3f(1,1,1);
							glEnable(GL_LIGHTING);
							
						}
						//��ǽ��
						{
							
							if(stepLen_wall==-1){
								//����
							}else{
								myvector<Cpolygen>&polygenList=cloud.polygenList;
								myvector<Cpolygen>&polygenList_dn=cloud.polygenList_dn;
								drawWall_polygenList_polygenList(polygenList,polygenList_dn,textureWall,stepLen_wall);
							}
							
						}
						
						cloud.drawed=true;
						
					}

				}
					
					
			}else{//�������Ҷ�ڵ�
				//��������p�ĺ���
				for(int j=0;j<=3;j++){
					cutAndDrawqTree_inn(p->p[j],planeList,cloudList,campos);
				}
			}

		}else{//������ɼ�
			return;
		}
		
	}


};
