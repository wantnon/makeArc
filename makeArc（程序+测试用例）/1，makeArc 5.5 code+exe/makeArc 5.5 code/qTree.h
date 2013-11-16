
class CqNode{//房屋四叉树节点
public:
	float xmin,xmax,zmin,zmax;
	float ymin,ymax;
	myvector<int> cloudIDList;//云团ID列表
	CqNode*p[4];//四个孩子指针
	            //  0 3
	            //  1 2
	CqNode(){
		p[0]=p[1]=p[2]=p[3]=NULL;
		xmin=xmax=zmin=zmax=0;
		ymin=ymax=0;
	}

};
class CqTree{//房屋四叉树
public:
	GLuint textureRoof;
	GLuint textureWall;
	CqNode*hd;//根指针
	CqTree(){
		hd=NULL;
	}
	destroy(){
		textureRoof=0;
		textureWall=0;
		//将hd树销毁
		destroyTree(hd);
		hd=NULL;
	}
	void destroyTree(CqNode*p){
		if(p==NULL)return;
		//销毁四个孩子
		for(int i=0;i<4;i++){
			destroyTree(p->p[i]);
		}
		//销毁自己
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
		myvector<Ccloud>&cloudList){//创建四叉树
		//生成根节点
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
		//生成孩子
		makeqTree_inn(hd,cloudList);

	
	}
	void makeqTree_inn(CqNode*&p,myvector<Ccloud>&cloudList){//创建四叉树的递归核:创建节点p的四个孩子
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
		//创建四个孩子
		p->p[0]=new CqNode();
		p->p[1]=new CqNode();
		p->p[2]=new CqNode();
		p->p[3]=new CqNode();
		//填充四个孩子的xz范围
		//孩子p->p[0]
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
		//孩子p->p[1]
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
		//孩子p->p[2]
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
		//孩子p->p[3]
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

		//对cloudIDList中每个云团，看其落到哪落到哪几个孩子的范围内
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
			//看cloud的范围_xmin,_xmax,_zmin,_zmax与哪几个孩子相交(一个cloud可能与多个孩子相交)
			for(int j=0;j<=3;j++){
				CqNode*q=p->p[j];
				//看cloud是否与q相交
				float xmin,xmax,zmin,zmax;
				xmin=q->xmin;
				xmax=q->xmax;
				zmin=q->zmin;
				zmax=q->zmax;
				float cx=(xmin+xmax)/2;
				float cz=(zmin+zmax)/2;
				float dx=(xmax-xmin)/2;
				float dz=(zmax-zmin)/2;
				//看_xmin,_xmax,_zmin,_zmax是与xmin,xmax,zmin,zmax相交
				//只需看二者中心距离是否超过二者边心距之和
				if(fabs(cx-_cx)>dx+_dx||fabs(cz-_cz)>dz+_dz){//不相交

				}else{//相交
					//把cloud的ID加入到q的cloudIDList中
					q->cloudIDList.push_back(ID);
				}
			}
		}
		//清空p->cloudIDList
		p->cloudIDList.resize(0);
		//求各孩子的y范围
		for(i=0;i<=3;i++){
			CqNode*q=p->p[i];
			//求q的y范围
			//统计q->cloudIDList中各云团的最高ymax和最低ymin
			float maxymax=-inf;
			float minymin=inf;
			int ncloudID=(int)q->cloudIDList.size();
			for(int j=0;j<ncloudID;j++){
				int ID=cloudIDList[j];
				Ccloud&cloud=cloudList[ID];
				if(cloud.removed)continue;
				if(cloud.topH>maxymax)maxymax=cloud.topH;
				if(cloud.minH<minymin)minymin=cloud.minH;
			}//得到maxymax,minymin
			q->ymax=maxymax;
			q->ymin=minymin;
		}
		
		//对每个孩子进行递归
		for(i=0;i<=3;i++){
			makeqTree_inn(p->p[i],cloudList);
		
		}
		


	}
	cutAndDrawqTree(myvector<Ccloud>&cloudList,float campos[4]){//使用视锥对四叉树进行剪裁，并绘制叶节点
		
		//---------------------求视锥
		//获得矩阵
		GLint viewport[4];
		GLdouble modelMatrix[16];
		GLdouble projMatrix[16];
		glGetIntegerv(GL_VIEWPORT,viewport);
		glGetDoublev(GL_MODELVIEW_MATRIX  ,modelMatrix);
		glGetDoublev(GL_PROJECTION_MATRIX ,projMatrix);
		float height=winheight;
		float width=winwidth;
		//视口的视口空间坐标如下
		//   (0,height,0)---(width,height,0)
		//      |              |
		//      |              |
		//    (0,0,0)--------(width,0,0)
		float p[4][4];//视口四个顶点的视口空间坐标
		init4(p[0],0,0,0,1);
		init4(p[1],width,0,0,1);
		init4(p[2],width,height,0,1);
		init4(p[3],0,height,0,1);
		//将p转化到世界空间
		for(int i=0;i<=3;i++){
			//将p[i]转化到世界空间
			GLdouble tp[3];
			gluUnProject(p[i][X],p[i][Y],p[i][Z],modelMatrix,projMatrix,viewport,&tp[X],&tp[Y],&tp[Z]);
			init4(p[i],tp[X],tp[Y],tp[Z],1);
		}//p已转化为世界坐标
		vector<Cplane> planeList;
		planeList.resize(5);//视锥表示为五个平面（四个侧面加一个近截面）
		//求这五个平面（法向指向视锥内部）
		float norm[5][4];//五个面的法向量
		getNormalOfTri(campos,p[1],p[0],norm[0]);
		getNormalOfTri(campos,p[2],p[1],norm[1]);
		getNormalOfTri(campos,p[3],p[2],norm[2]);
		getNormalOfTri(campos,p[0],p[3],norm[3]);
		getNormalOfTri(p[2],p[1],p[0],norm[4]);
		planeList[0].makePlane(campos,norm[0]);
		planeList[1].makePlane(campos,norm[1]);
		planeList[2].makePlane(campos,norm[2]);
		planeList[3].makePlane(campos,norm[3]);
		planeList[4].makePlane(p[0],norm[4]);//得到视锥

		//将所有cloud的drawed域置为false
		int ncloud=(int)cloudList.size();
		for(i=0;i<ncloud;i++){
			cloudList[i].drawed=false;
		}
		//
		cutAndDrawqTree_inn(hd,planeList,cloudList,campos);

		
	}
	void cutAndDrawqTree_inn(CqNode*&p,vector<Cplane>&planeList,myvector<Ccloud>&cloudList,float campos[4]){
		if(p==NULL)return;
		//判断节点p是否与视锥相交
		//求节点p所表示区域的保守包围盒
		//    上面
		//    v[0]--v[3]
		//     |     |
		//    v[1]--v[2]
		//    下面
		//    v[4]--v[7]
		//     |     |
		//    v[5]--v[6]
		float v[8][4];
		//上面
		init4(v[0],p->xmin,p->ymax,p->zmin,1);
		init4(v[1],p->xmin,p->ymax,p->zmax,1);
		init4(v[2],p->xmax,p->ymax,p->zmax,1);
		init4(v[3],p->xmax,p->ymax,p->zmin,1);
		//下面
		init4(v[4],p->xmin,p->ymin,p->zmin,1);
		init4(v[5],p->xmin,p->ymin,p->zmax,1);
		init4(v[6],p->xmax,p->ymin,p->zmax,1);
		init4(v[7],p->xmax,p->ymin,p->zmin,1);
		//看v[0]~v[7]是否都在planeList中某个面的反面，如果是则可剔除
		bool visible=true;
		for(int i=0;i<=4;i++){
			Cplane&plane=planeList[i];
			//看v[0]~v[7]是否都在plane的背面
			bool allback=true;//假设都在背面
			for(int j=0;j<=7;j++){
				float side=PND_point_plane(plane,v[j]);
				if(side>0){
					allback=false;
					break;
				}
			}//得到allback
			if(allback){//如果全在背面
				//断定为不可见，不用再继续检测
				visible=false;
				break;
			}
		}//得到visible
		if(visible){//如果可见
			//看p是否为叶节点
			if(p->p[0]==NULL){//如果是叶节点
				//节点中心
				float c[4]={(p->xmin+p->xmax)/2,(p->ymin+p->ymax)/2,(p->zmin+p->zmax)/2,1};
				//视点距当前节点的距离
				float d=fabs(c[X]-campos[X])+fabs(c[Y]-campos[Y])+fabs(c[Z]-campos[Z]);
				//计算屋顶LOD等级
				int thLOD_roof;//屋顶LOD等级
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
							}else{//漫游模式下若d过大则不画
								thLOD_roof=-1;//thLOD_roof为-1，则不画
							}
							
						}else{
							cout<<"error! 模式未定义"<<endl;
							return;
						}
					}else{//singleStepMode
						thLOD_roof=1;
					}
					
				}
				//计算墙壁步长
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
							}else{//漫游模式下若d过大则不画
								stepLen_wall=-1;//stepLen_wall为-1，则不画
							}
							
						}else{
							cout<<"error! 模式未定义"<<endl;
							return;
						}
						
					}else{//singleStepMode
						stepLen_wall=1;
					}
					
					
				}
				//画p中所有cloud
				{
					int ncloudID=(int)p->cloudIDList.size();
					for(int j=0;j<ncloudID;j++){
						int ID=p->cloudIDList[j];
						Ccloud&cloud=cloudList[ID];
						if(cloud.removed)continue;
						if(cloud.drawed)continue;
						//画屋顶面
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
								//不画
								break;
							}
							if(cloud.isRed)glColor3f(1,1,1);
							glEnable(GL_LIGHTING);
							
						}
						//画墙壁
						{
							
							if(stepLen_wall==-1){
								//不画
							}else{
								myvector<Cpolygen>&polygenList=cloud.polygenList;
								myvector<Cpolygen>&polygenList_dn=cloud.polygenList_dn;
								drawWall_polygenList_polygenList(polygenList,polygenList_dn,textureWall,stepLen_wall);
							}
							
						}
						
						cloud.drawed=true;
						
					}

				}
					
					
			}else{//如果不是叶节点
				//继续遍历p的孩子
				for(int j=0;j<=3;j++){
					cutAndDrawqTree_inn(p->p[j],planeList,cloudList,campos);
				}
			}

		}else{//如果不可见
			return;
		}
		
	}


};
