
//�����Ĳ���
class CgroundBlock{
public:
	Cgroundgrid GroundGrid;//���θ߶�ͼ
	int depth;
	float g_xmin,g_xmax,g_zmin,g_zmax,g_ymin,g_ymax;
	float width,height;//width=g_xmax-g_xmin,height=g_zmax-g_zmin
	float reso;//LOD�ֱ���
	myvector<myvector<bool> > markmat;//������飬��ҵ��Ǹ������Ƿ񱻷ָ�
	myvector<Cij> MarkedijList;//ָ��markmat�б���1��Ԫ�أ������Ժ����
	int top;//MarkedijList�ϵ�ջ��ָ��
	float campos[4];//���λ��
	GLuint texture;//����
	setTexture(GLuint _texture){
		texture=_texture;
	}
	CgroundBlock(){
		top=0;
	}
	destroy(){
		//ɾ��GroundGrid
		GroundGrid.destroy();
		//ɾ��markmat
		markmat.resize(0);
		//ɾ��MarkedijList
		MarkedijList.resize(0);
		//���MarkedijList�������д����һ��
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
		int n_markmat=pow(2,depth-1)+1;//markmat�ĳߴ�Ϊn*n
		//Ϊmarkmat���ٿռ�
		markmat.resize(n_markmat);
		for(int i=0;i<n_markmat;i++){
			markmat[i].resize(n_markmat);
			for(int j=0;j<n_markmat;j++){
				markmat[i][j]=false;
			}
		}
		//ΪMarkedijList���ٿռ�
		MarkedijList.resize(n_markmat*n_markmat);
		
	}

	void show(){
		
		//����Ĳ����ϵı��
		for(int i=0;i<top;i++){
			Cij&ij=MarkedijList[i];
			int I=ij.i;
			int J=ij.j;
			markmat[I][J]=false;
		}
		//top��λ
		top=0;
		
		//---------------------����׶
		//��þ���
		GLint viewport[4];
		GLdouble modelMatrix[16];
		GLdouble projMatrix[16];
		glGetIntegerv(GL_VIEWPORT,viewport);
		glGetDoublev(GL_MODELVIEW_MATRIX  ,modelMatrix);
		glGetDoublev(GL_PROJECTION_MATRIX ,projMatrix);
		//�ӿڵ��ӿڿռ���������
		//   (0,winheight,0)---(winwidth,winheight,0)
		//      |              |
		//      |              |
		//    (0,0,0)--------(winwidth,0,0)
		float p[4][4];//�ӿ��ĸ�������ӿڿռ�����
		init4(p[0],0,0,0,1);
		init4(p[1],winwidth,0,0,1);
		init4(p[2],winwidth,winheight,0,1);
		init4(p[3],0,winheight,0,1);
		//��pת��������ռ�
		for(i=0;i<=3;i++){
			//��p[i]ת��������ռ�
			GLdouble tp[3];
			gluUnProject(p[i][X],p[i][Y],p[i][Z],modelMatrix,projMatrix,viewport,&tp[X],&tp[Y],&tp[Z]);
			init4(p[i],tp[X],tp[Y],tp[Z],1);
		}//p��ת��Ϊ��������
		Cplane planeList[5];//��׶��ʾΪ���ƽ�棨�ĸ������һ�������棩
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
		//������
				glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture);
		//��Ⱦ�Ĳ������Խڵ�ķָ���������
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
		//��鵱ǰ�ڵ��Ƿ����ӽ����ཻ
		//��ڵ�p����ʾ����ı��ذ�Χ��
		//    ����
		//    p[0]--p[3]
		//     |     |
		//    p[1]--p[2]
		//    ����
		//    p[4]--p[7]
		//     |     |
		//    p[5]--p[6]
		float p[8][4];
		//����
		init4(p[0],xmin,g_ymax,zmin,1);
		init4(p[1],xmin,g_ymax,zmax,1);
		init4(p[2],xmax,g_ymax,zmax,1);
		init4(p[3],xmax,g_ymax,zmin,1);
		//����
		init4(p[4],xmin,g_ymin,zmin,1);
		init4(p[5],xmin,g_ymin,zmax,1);
		init4(p[6],xmax,g_ymin,zmax,1);
		init4(p[7],xmax,g_ymin,zmin,1);
		//��p[0]~p[7]�Ƿ���planeList��ĳ����ķ��棬���������޳�
		bool visible=true;
		for(int i=0;i<=4;i++){
			Cplane&plane=planeList[i];
			//��p[0]~p[7]�Ƿ���plane�ı���
			bool allback=true;//���趼�ڱ���
			for(int j=0;j<=7;j++){
				float side=PND_point_plane(plane,p[j]);
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
			//�ж��Ƿ��ٷ�
			//���ı�������(y=0)���ӵ�ľ���
			float c[4];//�ı�������(y=0)
			init4(c,(xmax+xmin)/2,0,(zmax+zmin)/2,1);
		    float d=pow2((fabs(campos[X]-c[X])+fabs(campos[Y]-c[Y])+fabs(campos[Z]-c[Z])));//ָ��ֵԽ��LODЧӦԽ����
			float e=xmax-xmin;//�߳�
			if(d<e*reso&&imin+1!=imax){//������
				int imid=(imin+imax)/2;
				int jmid=(jmin+jmax)/2;
				markmat[imid][jmid]=true;
				MarkedijList[top].makeij(imid,jmid);
				top++;	
				//���ĸ����Ӽ����ݹ�
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
			}else{//���֣���
				float p[4][4];
				init4(p[0],xmin,getH(xmin,zmin),zmin,1);
				init4(p[1],xmin,getH(xmin,zmax),zmax,1);
				init4(p[2],xmax,getH(xmax,zmax),zmax,1);
				init4(p[3],xmax,getH(xmax,zmin),zmin,1);
				float u[4][2];
				for(int i=0;i<=3;i++){
					init2(u[i],(p[i][X]-g_xmin)/width,(p[i][Z]-g_zmin)/height);
				}
				//��p[0]p[1]p[2]p[3]
				float norm[4];
				getNormalOfTri(p[0],p[1],p[2],norm);
				glBegin(GL_QUADS);
				glNormal3fv(norm);
				glTexCoord2fv(u[0]);glVertex3fv(p[0]);
				glTexCoord2fv(u[1]);glVertex3fv(p[1]);
				glTexCoord2fv(u[2]);glVertex3fv(p[2]);
				glTexCoord2fv(u[3]);glVertex3fv(p[3]);
				glEnd();
			/*	//����
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
		if(imin+1==imax){//��ԭ�Ӹ񣬲����޲�
			return;
		}else{
			//����������
			int imid=(imin+imax)/2;
			int jmid=(jmin+jmax)/2;
			if(markmat[imid][jmid]){//�����������mark����������Ҫϸ��
				
				//������
				//���ĸ����Ӽ����ݹ�
				makeUp(xmin,(xmin+xmax)/2,zmin,(zmin+zmax)/2,
					jmin,(jmin+jmax)/2,imin,(imin+imax)/2);
				makeUp(xmin,(xmin+xmax)/2,(zmin+zmax)/2,zmax,
					jmin,(jmin+jmax)/2,(imin+imax)/2,imax);
				makeUp((xmin+xmax)/2,xmax,(zmin+zmax)/2,zmax,
					(jmin+jmax)/2,jmax,(imin+imax)/2,imax);
				makeUp((xmin+xmax)/2,xmax,zmin,(zmin+zmax)/2,
					(jmin+jmax)/2,jmax,imin,(imin+imax)/2);
				
			}else{
				//��������������
				float xmid=(xmin+xmax)/2;
				float zmid=(zmin+zmax)/2;
				
				//���Ϸ�ͬ�ߴ����������Ƿ�mark
				{
					int _imid=imid-(imax-imin);
					int _jmid=jmid;
					if(_imid>=0&&markmat[_imid][_jmid]){//�Ϸ�ͬ�ߴ���������mark
					
						//�Ա�������Ե�޲��ѷ�
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
						//��������p0p1p2
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
				//���·�ͬ�ߴ����������Ƿ�mark
				{
					int _imid=imid+(imax-imin);
					int _jmid=jmid;
					if(_imid<(int)markmat.size()&&markmat[_imid][_jmid]){//�·�ͬ�ߴ���������mark
					
						//�Ա�������Ե�޲��ѷ�
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
						//��������p0p1p2
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
				//����ͬ�ߴ����������Ƿ�mark
				{
					int _imid=imid;
					int _jmid=jmid-(jmax-jmin);
					if(_imid<(int)markmat.size()&&markmat[_imid][_jmid]){//��ͬ�ߴ���������mark
					
						//�Ա�������Ե�޲��ѷ�
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
						//��������p0p1p2
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
				//���ҷ�ͬ�ߴ����������Ƿ�mark
				{
					int _imid=imid;
					int _jmid=jmid+(jmax-jmin);
					if(_imid<(int)markmat.size()&&markmat[_imid][_jmid]){//�ҷ�ͬ�ߴ���������mark
					
						//�Ա�������Ե�޲��ѷ�
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
						//��������p0p1p2
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
		//��(x,z)���ڵĸ�
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