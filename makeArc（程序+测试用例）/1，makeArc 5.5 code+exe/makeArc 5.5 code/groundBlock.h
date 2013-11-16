
//地形四叉树
class CgroundBlock{
public:
	Cgroundgrid GroundGrid;//地形高度图
	int depth;
	float g_xmin,g_xmax,g_zmin,g_zmax,g_ymin,g_ymax;
	float width,height;//width=g_xmax-g_xmin,height=g_zmax-g_zmin
	float reso;//LOD分辨率
	myvector<myvector<bool> > markmat;//标记数组，用业标记各区域是否被分割
	myvector<Cij> MarkedijList;//指向markmat中被置1的元素，便于以后归零
	int top;//MarkedijList上的栈顶指针
	float campos[4];//相机位置
	GLuint texture;//纹理
	setTexture(GLuint _texture){
		texture=_texture;
	}
	CgroundBlock(){
		top=0;
	}
	destroy(){
		//删除GroundGrid
		GroundGrid.destroy();
		//删除markmat
		markmat.resize(0);
		//删除MarkedijList
		MarkedijList.resize(0);
		//清空MarkedijList，则必须写下面一句
		top=0;
	}
	updateCampos(float _campos[4]){
		veccopy(_campos,campos);
		
	}
	makeGroundBlock(float _xmin,float _xmax,float _zmin,float _zmax,float _ymin,float _ymax,
		int _depth,float _reso){
		g_xmin=_xmin;
		g_xmax=_xmax;
		g_zmin=_zmin;
		g_zmax=_zmax;
		g_ymin=_ymin;
		g_ymax=_ymax;
		depth=_depth;
		reso=_reso;
		width=g_xmax-g_xmin;
		height=g_zmax-g_zmin;
		int n_markmat=pow(2,depth-1)+1;//markmat的尺寸为n*n
		//为markmat开辟空间
		markmat.resize(n_markmat);
		for(int i=0;i<n_markmat;i++){
			markmat[i].resize(n_markmat);
			for(int j=0;j<n_markmat;j++){
				markmat[i][j]=false;
			}
		}
		//为MarkedijList开辟空间
		MarkedijList.resize(n_markmat*n_markmat);
		
	}

	void show(){
		
		//清除四叉树上的标记
		for(int i=0;i<top;i++){
			Cij&ij=MarkedijList[i];
			int I=ij.i;
			int J=ij.j;
			markmat[I][J]=false;
		}
		//top复位
		top=0;
		
		//---------------------求视锥
		//获得矩阵
		GLint viewport[4];
		GLdouble modelMatrix[16];
		GLdouble projMatrix[16];
		glGetIntegerv(GL_VIEWPORT,viewport);
		glGetDoublev(GL_MODELVIEW_MATRIX  ,modelMatrix);
		glGetDoublev(GL_PROJECTION_MATRIX ,projMatrix);
		//视口的视口空间坐标如下
		//   (0,winheight,0)---(winwidth,winheight,0)
		//      |              |
		//      |              |
		//    (0,0,0)--------(winwidth,0,0)
		float p[4][4];//视口四个顶点的视口空间坐标
		init4(p[0],0,0,0,1);
		init4(p[1],winwidth,0,0,1);
		init4(p[2],winwidth,winheight,0,1);
		init4(p[3],0,winheight,0,1);
		//将p转化到世界空间
		for(i=0;i<=3;i++){
			//将p[i]转化到世界空间
			GLdouble tp[3];
			gluUnProject(p[i][X],p[i][Y],p[i][Z],modelMatrix,projMatrix,viewport,&tp[X],&tp[Y],&tp[Z]);
			init4(p[i],tp[X],tp[Y],tp[Z],1);
		}//p已转化为世界坐标
		Cplane planeList[5];//视锥表示为五个平面（四个侧面加一个近截面）
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
		//绑定纹理
				glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture);
		//渲染四叉树并对节点的分割情况作标记
		int n_markmat=(int)markmat.size();
		showAndMark(g_xmin,g_xmax,g_zmin,g_zmax,
			0,n_markmat-1,0,n_markmat-1,
			planeList);
		makeUp(g_xmin,g_xmax,g_zmin,g_zmax,
			0,n_markmat-1,0,n_markmat-1);
		glDisable(GL_TEXTURE_2D);
	

	}
	
	void showAndMark(float xmin,float xmax,float zmin,float zmax,
		int jmin,int jmax,int imin,int imax,
		Cplane planeList[5])
	{
		//检查当前节点是否与视截体相交
		//求节点p所表示区域的保守包围盒
		//    上面
		//    p[0]--p[3]
		//     |     |
		//    p[1]--p[2]
		//    下面
		//    p[4]--p[7]
		//     |     |
		//    p[5]--p[6]
		float p[8][4];
		//上面
		init4(p[0],xmin,g_ymax,zmin,1);
		init4(p[1],xmin,g_ymax,zmax,1);
		init4(p[2],xmax,g_ymax,zmax,1);
		init4(p[3],xmax,g_ymax,zmin,1);
		//下面
		init4(p[4],xmin,g_ymin,zmin,1);
		init4(p[5],xmin,g_ymin,zmax,1);
		init4(p[6],xmax,g_ymin,zmax,1);
		init4(p[7],xmax,g_ymin,zmin,1);
		//看p[0]~p[7]是否都在planeList中某个面的反面，如果是则可剔除
		bool visible=true;
		for(int i=0;i<=4;i++){
			Cplane&plane=planeList[i];
			//看p[0]~p[7]是否都在plane的背面
			bool allback=true;//假设都在背面
			for(int j=0;j<=7;j++){
				float side=PND_point_plane(plane,p[j]);
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
			//判断是否再分
			//求四边形中心(y=0)到视点的距离
			float c[4];//四边形中心(y=0)
			init4(c,(xmax+xmin)/2,0,(zmax+zmin)/2,1);
		    float d=pow2((fabs(campos[X]-c[X])+fabs(campos[Y]-c[Y])+fabs(campos[Z]-c[Z])));//指数值越大，LOD效应越明显
			float e=xmax-xmin;//边长
			if(d<e*reso&&imin+1!=imax){//继续分
				int imid=(imin+imax)/2;
				int jmid=(jmin+jmax)/2;
				markmat[imid][jmid]=true;
				MarkedijList[top].makeij(imid,jmid);
				top++;	
				//对四个孩子继续递归
				showAndMark(xmin,(xmin+xmax)/2,zmin,(zmin+zmax)/2,
					jmin,(jmin+jmax)/2,imin,(imin+imax)/2,
					planeList);
				showAndMark(xmin,(xmin+xmax)/2,(zmin+zmax)/2,zmax,
					jmin,(jmin+jmax)/2,(imin+imax)/2,imax,
					planeList);
				showAndMark((xmin+xmax)/2,xmax,(zmin+zmax)/2,zmax,
					(jmin+jmax)/2,jmax,(imin+imax)/2,imax,
					planeList);
				showAndMark((xmin+xmax)/2,xmax,zmin,(zmin+zmax)/2,
					(jmin+jmax)/2,jmax,imin,(imin+imax)/2,
					planeList);
			}else{//不分，画
				float p[4][4];
				init4(p[0],xmin,getH(xmin,zmin),zmin,1);
				init4(p[1],xmin,getH(xmin,zmax),zmax,1);
				init4(p[2],xmax,getH(xmax,zmax),zmax,1);
				init4(p[3],xmax,getH(xmax,zmin),zmin,1);
				float u[4][2];
				for(int i=0;i<=3;i++){
					init2(u[i],(p[i][X]-g_xmin)/width,(p[i][Z]-g_zmin)/height);
				}
				//画p[0]p[1]p[2]p[3]
				float norm[4];
				getNormalOfTri(p[0],p[1],p[2],norm);
				glBegin(GL_QUADS);
				glNormal3fv(norm);
				glTexCoord2fv(u[0]);glVertex3fv(p[0]);
				glTexCoord2fv(u[1]);glVertex3fv(p[1]);
				glTexCoord2fv(u[2]);glVertex3fv(p[2]);
				glTexCoord2fv(u[3]);glVertex3fv(p[3]);
				glEnd();
			/*	//画线
				glColor3f(0,1,0);
				glBegin(GL_LINES);
				glVertex3fv(p[0]);glVertex3f(p[1][X],p[1][Y]+0.5,p[1][Z]);
				glVertex3fv(p[1]);glVertex3f(p[2][X],p[2][Y]+0.5,p[2][Z]);
				glVertex3fv(p[2]);glVertex3f(p[3][X],p[3][Y]+0.5,p[3][Z]);
				glVertex3fv(p[3]);glVertex3f(p[0][X],p[0][Y]+0.5,p[0][Z]);
				glEnd();
				glColor3f(1,1,1);
			*/

			}
		}
	}

	void makeUp(float xmin,float xmax,float zmin,float zmax,
		int jmin,int jmax,int imin,int imax)
	{
		if(imin+1==imax){//是原子格，不用修补
			return;
		}else{
			//本区域中心
			int imid=(imin+imax)/2;
			int jmid=(jmin+jmax)/2;
			if(markmat[imid][jmid]){//如果区域中心mark，即本区域要细分
				
				//向下走
				//对四个孩子继续递归
				makeUp(xmin,(xmin+xmax)/2,zmin,(zmin+zmax)/2,
					jmin,(jmin+jmax)/2,imin,(imin+imax)/2);
				makeUp(xmin,(xmin+xmax)/2,(zmin+zmax)/2,zmax,
					jmin,(jmin+jmax)/2,(imin+imax)/2,imax);
				makeUp((xmin+xmax)/2,xmax,(zmin+zmax)/2,zmax,
					(jmin+jmax)/2,jmax,(imin+imax)/2,imax);
				makeUp((xmin+xmax)/2,xmax,zmin,(zmin+zmax)/2,
					(jmin+jmax)/2,jmax,imin,(imin+imax)/2);
				
			}else{
				//本区域中心坐标
				float xmid=(xmin+xmax)/2;
				float zmid=(zmin+zmax)/2;
				
				//看上方同尺寸区域中心是否mark
				{
					int _imid=imid-(imax-imin);
					int _jmid=jmid;
					if(_imid>=0&&markmat[_imid][_jmid]){//上方同尺寸区域中心mark
					
						//对本区域上缘修补裂缝
						//          p2(xmid,zmin)
						//               /\      
						//              /  \
						// p0(xmin,zmin)----p1(xmax,zmin)
						float p0[4]={xmin,getH(xmin,zmin),zmin,1};
						float p1[4]={xmax,getH(xmax,zmin),zmin,1};
						float p2[4]={xmid,getH(xmid,zmin),zmin,1};
						float u0[2]={(p0[X]-g_xmin)/width,(p0[Z]-g_zmin)/height};
						float u1[2]={(p1[X]-g_xmin)/width,(p1[Z]-g_zmin)/height};
						float u2[2]={(p2[X]-g_xmin)/width,(p2[Z]-g_zmin)/height};
						//画三角面p0p1p2
						float norm[4];
						getNormalOfTri(p0,p1,p2,norm);
						glBegin(GL_TRIANGLES);
						glNormal3fv(norm);
						glTexCoord2fv(u0);glVertex3fv(p0);
						glTexCoord2fv(u1);glVertex3fv(p1);
						glTexCoord2fv(u2);glVertex3fv(p2);
						glEnd();
/*
						glLineWidth(5);
						glBegin(GL_LINES);
						glVertex3fv(p2);
						glVertex3fv(p1);
						glVertex3fv(p1);
						glVertex3fv(p2);
						glVertex3fv(p2);
						glVertex3fv(p2);
						glEnd();
						glLineWidth(1);
						*/
					}
				}
				//看下方同尺寸区域中心是否mark
				{
					int _imid=imid+(imax-imin);
					int _jmid=jmid;
					if(_imid<(int)markmat.size()&&markmat[_imid][_jmid]){//下方同尺寸区域中心mark
					
						//对本区域下缘修补裂缝
						// p1(xmin,zmax)----p0(xmax,zmax)
						//              \  /
						//               \/
						//          p2(xmid,zmax)
						float p0[4]={xmax,getH(xmax,zmax),zmax,1};
						float p1[4]={xmin,getH(xmin,zmax),zmax,1};
						float p2[4]={xmid,getH(xmid,zmax),zmax,1};
						float u0[2]={(p0[X]-g_xmin)/width,(p0[Z]-g_zmin)/height};
						float u1[2]={(p1[X]-g_xmin)/width,(p1[Z]-g_zmin)/height};
						float u2[2]={(p2[X]-g_xmin)/width,(p2[Z]-g_zmin)/height};
						//画三角面p0p1p2
						float norm[4];
						getNormalOfTri(p0,p1,p2,norm);
						glBegin(GL_TRIANGLES);
						glNormal3fv(norm);
						glTexCoord2fv(u0);glVertex3fv(p0);
						glTexCoord2fv(u1);glVertex3fv(p1);
						glTexCoord2fv(u2);glVertex3fv(p2);
						glEnd();
/*
						glLineWidth(5);
						glBegin(GL_LINES);
						glVertex3fv(p2);
						glVertex3fv(p1);
						glVertex3fv(p1);
						glVertex3fv(p2);
						glVertex3fv(p2);
						glVertex3fv(p2);
						glEnd();
						glLineWidth(1);
						*/
					}
				}
				//看左方同尺寸区域中心是否mark
				{
					int _imid=imid;
					int _jmid=jmid-(jmax-jmin);
					if(_imid<(int)markmat.size()&&markmat[_imid][_jmid]){//左方同尺寸区域中心mark
					
						//对本区域左缘修补裂缝
						//           p1 (xmin,zmin)
 						//                /|
						//               / | 
						// p2 (xmin,zmid)  |
						//               \ |
						//                \|
						//           p0 (xmin,zmax)
						float p0[4]={xmin,getH(xmin,zmax),zmax,1};
						float p1[4]={xmin,getH(xmin,zmin),zmin,1};
						float p2[4]={xmin,getH(xmin,zmid),zmid,1};
						float u0[2]={(p0[X]-g_xmin)/width,(p0[Z]-g_zmin)/height};
						float u1[2]={(p1[X]-g_xmin)/width,(p1[Z]-g_zmin)/height};
						float u2[2]={(p2[X]-g_xmin)/width,(p2[Z]-g_zmin)/height};
						//画三角面p0p1p2
						float norm[4];
						getNormalOfTri(p0,p1,p2,norm);
						glBegin(GL_TRIANGLES);
						glNormal3fv(norm);
						glTexCoord2fv(u0);glVertex3fv(p0);
						glTexCoord2fv(u1);glVertex3fv(p1);
						glTexCoord2fv(u2);glVertex3fv(p2);
						glEnd();

				/*		glLineWidth(5);
						glBegin(GL_LINES);
						glVertex3fv(p2);
						glVertex3fv(p1);
						glVertex3fv(p1);
						glVertex3fv(p2);
						glVertex3fv(p2);
						glVertex3fv(p2);
						glEnd();
						glLineWidth(1);
						*/
					}
				}
				//看右方同尺寸区域中心是否mark
				{
					int _imid=imid;
					int _jmid=jmid+(jmax-jmin);
					if(_imid<(int)markmat.size()&&markmat[_imid][_jmid]){//右方同尺寸区域中心mark
					
						//对本区域右缘修补裂缝
						//    p1 (xmax,zmin)
 						//                 |\
						//                 | \
						//                 |  p0 (xmax,zmid)
						//                 | /
						//                 |/
						//    p2 (xmax,zmax)
						float p0[4]={xmax,getH(xmax,zmid),zmid,1};
						float p1[4]={xmax,getH(xmax,zmin),zmin,1};
						float p2[4]={xmax,getH(xmax,zmax),zmax,1};
						float u0[2]={(p0[X]-g_xmin)/width,(p0[Z]-g_zmin)/height};
						float u1[2]={(p1[X]-g_xmin)/width,(p1[Z]-g_zmin)/height};
						float u2[2]={(p2[X]-g_xmin)/width,(p2[Z]-g_zmin)/height};
						//画三角面p0p1p2
						float norm[4];
						getNormalOfTri(p0,p1,p2,norm);
						glBegin(GL_TRIANGLES);
						glNormal3fv(norm);
						glTexCoord2fv(u0);glVertex3fv(p0);
						glTexCoord2fv(u1);glVertex3fv(p1);
						glTexCoord2fv(u2);glVertex3fv(p2);
						glEnd();

				/*		glLineWidth(5);
						glBegin(GL_LINES);
						glVertex3fv(p2);
						glVertex3fv(p1);
						glVertex3fv(p1);
						glVertex3fv(p2);
						glVertex3fv(p2);
						glVertex3fv(p2);
						glEnd();
						glLineWidth(1);
						*/
					}
				}

			}
		}
	
	}

	float getH(float x,float z){
		//求(x,z)所在的格
		int m=GroundGrid.m_mat;
		int n=GroundGrid.n_mat;
		float xmin,zmin;
		xmin=GroundGrid.xmin;
		zmin=GroundGrid.zmin;
		float xstepLen=GroundGrid.xstepLen;
		float zstepLen=GroundGrid.zstepLen;
		int I,J;
		I=(z-zmin)/zstepLen;
		J=(x-xmin)/xstepLen;
		if(I>=0&&I<m&&J>=0&&J<n){
			return GroundGrid.mat[I][J].h;
		}else{
			return 0;//inf;
		}

	}
	
};