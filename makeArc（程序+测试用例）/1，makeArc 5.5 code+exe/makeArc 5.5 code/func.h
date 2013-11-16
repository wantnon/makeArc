inline bool getBmp24Size(string pathName,int&bmpWidth,int&bmpHeight)
//���24λbmpͼƬ�ߴ�
//�������24λbmp�ļ����򷵻�false������true
{
	//���ļ�
	FILE* fp=fopen(pathName.c_str(),"rb");//�����ƶ�
	if(fp==NULL)
	{ 
		cout<<"Open file "<<pathName<<" failed!"<<endl; 
		return false;
	}
	//���ļ�ͷ
	BYTE  Head[54];
	fread(Head,54,1,fp); 
	//�ر��ļ�
	fclose(fp);
	//�����ļ�ͷ
	{
		//����Ƿ�Ϊbmp�ļ�
		char type[2]={Head[0],Head[1]};
		if(type[0]=='B'&&type[1]=='M'){
			//��bmp�ļ�
		}else{
			//����bmp�ļ�
			cout<<"����bmp�ļ�!"<<endl;
			return false;
		}
		//����Ƿ�Ϊ24λ
		int bitCount=Head[28]+Head[29]*256;
		if(bitCount!=24){
			//����24λ
			cout<<"����24λbmp�ļ�!"<<endl;
			return false;
		}
		//ͨ����飬��óߴ�
		bmpWidth =Head[18]+Head[19]*256;
		bmpHeight=Head[22]+Head[23]*256;
	}

	return true;
}
inline string ridLastLevelOfPathName(string pathName){//ȥ��·���������һ��������ĩβ��\\��
	int count=0;//�������һ�����ַ�����
	int n=(int)pathName.size();
	for(int i=n-1;i>=0;i--)
	{
		if(pathName[i]!='\\'){
			count++;
		}else{
			break;
		}
	}//�õ�count
	pathName.resize(n-count);
	return pathName;
}
inline void makeColorTable(float colorTable[256][3]){//������ɫ��
	//�Ƚ�colorTable����Ԫ�ظ�ֵΪ-1
	for(int i=0;i<256;i++){
		for(int j=0;j<3;j++){
			colorTable[i][j]=-1;
		}
	}
	//���峣��
	const int r=0;
	const int g=1;
	const int b=2;
	//������׼ֵ
	colorTable[0][r]=0;colorTable[0][g]=0;colorTable[0][b]=1;
	colorTable[63][r]=0;colorTable[63][g]=1;colorTable[63][b]=1;
	colorTable[127][r]=0;colorTable[127][g]=1;colorTable[127][b]=0;
	colorTable[191][r]=1;colorTable[191][g]=1;colorTable[191][b]=0;
	colorTable[255][r]=1;colorTable[255][g]=0;colorTable[255][b]=0;
	//��colorTable��û��ֵ�Ĳ��ֽ��в�ֵ
	for(i=0;i<3;i++){
		//��colorTable[][i]���в�ֵ
		int ibeg=0;
		int iend=ibeg+1;
		while(1){
			//iendȥ����һ����ֵ��Ԫ��
			for(;iend<256;iend++){
				if(colorTable[iend][i]!=-1)break;
			}
			if(iend==256){//û�ҵ�
				break;
			}else{//�ҵ�
				//��(ibeg,iend)�ڵ�Ԫ�ؽ��в�ֵ
				float dH=(colorTable[iend][i]-colorTable[ibeg][i])/(iend-ibeg);
				float H=colorTable[ibeg][i];
				for(int j=ibeg+1;j<iend;j++){
					H+=dH;
					colorTable[j][i]=H;
				}//�˶�������
				//ȥ����һ��
				ibeg=iend;
				iend=ibeg+1;
			}
		}
	
	}

}
inline bool LineSegOverLineSeg(float x1,float y1,
						float x2,float y2,
						float u1,float v1,
						float u2,float v2)
						//�ж��߶�(x1,y1)--(x2,y2)���߶�(u1,v1)--(u2,v2)�Ƿ��ཻ���������˵㣩
{
	//�ж�(u1,v1)��(u2,v2)�Ƿ���ֱ��(x1,y1)-(x2,y2)��ͬ��
	bool u1v1_separate_u2v2=false;
	double a=(v1-y1)*(x2-x1)-(y2-y1)*(u1-x1);
	double b=(v2-y1)*(x2-x1)-(y2-y1)*(u2-x1);
	if(a*b<0)u1v1_separate_u2v2=true;
	//�ж�(x1,y1)��(x2,y2)�Ƿ���ֱ��(u1,v1)-(u2,v2)��ͬ��
	bool x1y1_separate_x2y2=false;
	a=(y1-v1)*(u2-u1)-(v2-v1)*(x1-u1);
	b=(y2-v1)*(u2-u1)-(v2-v1)*(x2-u1);
	if(a*b<0)x1y1_separate_x2y2=true;
	return x1y1_separate_x2y2*u1v1_separate_u2v2;
}


inline void ridSelfInteract_polygen(Cpolygen&polygen)
//Ϊ�����polygenȥ���ཻ
//�ٶ�polygen���������ڵ���3
{
	for(int i=0;i<(int)polygen.vlist.size();i++){
		//i�űߵ����˵�����
		float*v1=polygen.vlist[i].v;
		int inext=(i+1)%(int)polygen.vlist.size();
		float*v2=polygen.vlist[inext].v;
		//��i�ű�֮��ı�������i�ű��ཻ��
		for(int j=i+2;j<(int)polygen.vlist.size();j++){//j��i+2��ʼ����Ϊi+1��i���ڱߣ��ز��ཻ
			if(i==0&&j==(int)polygen.vlist.size()-1)continue;//��ʱi�ű���j�ű����ڱߣ��������
			float*_v1=polygen.vlist[j].v;
			int jnext=(j+1)%(int)polygen.vlist.size();
			float*_v2=polygen.vlist[jnext].v;
			//��v1v2�Ƿ���_v1_v2�ཻ
			bool interact=LineSegOverLineSeg(v1[X],v1[Z],v2[X],v2[Z],_v1[X],_v1[Z],_v2[X],_v2[Z]);
			if(interact){//����ཻ
				//����v1v2��_v1_v2˭�̣�˭�̾�ɾ��˭�������˵�
				if(pow2(v1[X]-v2[X])+pow2(v1[Z]-v2[Z])<pow2(_v1[X]-_v2[X])+pow2(_v1[Z]-_v2[Z])){//v1v2��
					//ɾ��v1v2
					polygen.vlist.erase(&polygen.vlist[inext]);
					i--;//������һ��forѭ�����Զ���1

				}else{//_v1_v2��
	
					polygen.vlist.erase(&polygen.vlist[jnext]);
					j--;//������һ��forѭ�����Զ���1
					
					
				}
				
			}


		}
	}
}

inline bool isPointInPolygenList_reduced(float cx,float cz,myvector<Cpolygen>&_polygenList)
//�ж�(cx,cz)�Ƿ���polygenList�ڲ�(ʹ�ø�polygen�ļ򻯰汾�����ж�)
//ʹ�����߷�
//��Ϊ�˼򻯣����������н��㣬����ֻ���뷢�������Ľ��㣬�������ڴ˴��Ǵ��뻹�Ǵ�������Ȼ���˷��������ཻ��polygenList�����޷���ȷ����
{
	//����_polygenList�ļ򻯰汾polygenList
	myvector<Cpolygen> polygenList;
	{
		
		polygenList.resize((int)_polygenList.size());
		int npolygen=(int)polygenList.size();
		for(int i=0;i<npolygen;i++){
			Cpolygen&polygen=polygenList[i];
			Cpolygen&_polygen=_polygenList[i];
			//����polygen
			//��_polygenList�е�isKeyPoint��ӵ�polygen
			int nv=(int)_polygen.vlist.size();
			for(int j=0;j<nv;j++){
				Cv4Ex&v=_polygen.vlist[j];
				if(v.isKeyPoint){
					//��v��ӵ�polygen
					polygen.vlist.push_back(v);
				}
			}
		}//�õ�polygenList
	}
	//�ж�(cx,cz)�Ƿ���polygenList�ڲ�
	{
		
		//��(cx,cz)������ˮƽ����
		//��polygenList����Щ���������ཻ�������н����о���(cx,cz)����ģ���x��С�ģ�
		float xmin=inf;//����x����Сֵ
		int polygenID=-1;//ȡ��x��С�Ľ���Ķ����ID
		int edgeID=-1;//ȡ��x��С�Ľ���ıߵ�ID
		int npolygen=(int)polygenList.size();
		for(int i=0;i<npolygen;i++){
			Cpolygen&polygen=polygenList[i];
			int nv=(int)polygen.vlist.size();
			for(int j=0;j<nv;j++){
				float *v1=polygen.vlist[j].v;
				float *v2=polygen.vlist[(j+1)%nv].v;
				//�жϰ뿪����߶�[v1,v2)�Ƿ��������ཻ
				//�ȿ��Ƿ���(v1[Z]>=cz&&v2[Z]<cz)��(v1[Z]<=cz&&v2[Z]>cz)�����û�У���ض����������ཻ��ֱ������
				if((v1[Z]>=cz&&v2[Z]<cz)||(v1[Z]<=cz&&v2[Z]>cz)){//�����ཻ
					//����
				}else{//�������ཻ
					//����
					continue;
				}
				//�п����ཻ������ֱ��v1v2����������ֱ�ߵĽ����x����
				float x1=v1[X];
				float z1=v1[Z];
				float x2=v2[X];
				float z2=v2[Z];
				float x=(cz-z1)*(x2-x1)/(z2-z1)+x1;
				//����˽����X����>cx����˵���������ཻ
				if(x>cx){//v1v2�������ཻ
					if(x<xmin){
						xmin=x;
						polygenID=i;
						edgeID=j;
					}
				}
			}
			
		}//�õ�xmin,polygenID,edgeID
		if(polygenID==-1){//���û�н���
			//˵��(cx,cz)�ض���polygenList�ⲿ
			return false;
		}
		//�ж���(polygenID,edgeID)�Ľ����Ǵ��뻹�Ǵ���
		//���(polygenID,edgeID)�������˵�
		int nv=(int)polygenList[polygenID].vlist.size();
		float *v1=polygenList[polygenID].vlist[edgeID].v;
		float *v2=polygenList[polygenID].vlist[(edgeID+1)%nv].v;
		//�ж�������v1v2���Ǵ��뻹�Ǵ���
		//ֻ�轫���ߵķ�������������v1v2�����
		//��������YֵΪ������Ϊ���룻��������YֵΪ������Ϊ����
		bool penetrateOut;
		float ray_e[4]={1,0,0,0};//���ߵķ�������
		float v1v2[4];
		sub(v2,v1,v1v2);
		v1v2[Y]=0;
		float norm[4];
		cross(ray_e,v1v2,norm);
		if(norm[Y]>0){//����
			penetrateOut=true;
		}else{
			penetrateOut=false;
		}
		if(penetrateOut){//�������
			//˵��(cx,cy)��polygenList�ڲ�
			return true;
		}else{//����˵��(cx,cy)��polygenList�ⲿ
			return false;
		}
	}

}

inline bool isPointInPolygenList(float cx,float cz,myvector<Cpolygen>&polygenList)
//�ж�(cx,cz)�Ƿ���polygenList�ڲ�(ʹ�ø�polygen�ķǼ򻯰汾�����ж�)
//ʹ�����߷�
{
	//��(cx,cz)������ˮƽ����
	//��polygenList����Щ���������ཻ�������н����о���(cx,cz)����ģ���x��С�ģ�
	float xmin=inf;//����x����Сֵ
	int polygenID=-1;//ȡ��x��С�Ľ���Ķ����ID
	int edgeID=-1;//ȡ��x��С�Ľ���ıߵ�ID
	int npolygen=(int)polygenList.size();
	for(int i=0;i<npolygen;i++){
		Cpolygen&polygen=polygenList[i];
		int nv=(int)polygen.vlist.size();
		for(int j=0;j<nv;j++){
			float *v1=polygen.vlist[j].v;
			float *v2=polygen.vlist[(j+1)%nv].v;
			//�жϰ뿪����߶�[v1,v2)�Ƿ��������ཻ
			//�ȿ��Ƿ���(v1[Z]>=cz&&v2[Z]<cz)��(v1[Z]<=cz&&v2[Z]>cz)�����û�У���ض����������ཻ��ֱ������
			if((v1[Z]>=cz&&v2[Z]<cz)||(v1[Z]<=cz&&v2[Z]>cz)){//�����ཻ
				//����
			}else{//�������ཻ
				//����
				continue;
			}
			//�п����ཻ������ֱ��v1v2����������ֱ�ߵĽ����x����
			float x1=v1[X];
			float z1=v1[Z];
			float x2=v2[X];
			float z2=v2[Z];
			float x=(cz-z1)*(x2-x1)/(z2-z1)+x1;
			//����˽����X����>cx����˵���������ཻ
			if(x>cx){//v1v2�������ཻ
				if(x<xmin){
					xmin=x;
					polygenID=i;
					edgeID=j;
				}
			}
		}
		
	}//�õ�xmin,polygenID,edgeID
	if(polygenID==-1){//���û�н���
		//˵��(cx,cz)�ض���polygenList�ⲿ
		return false;
	}
	//�ж���(polygenID,edgeID)�Ľ����Ǵ��뻹�Ǵ���
	//���(polygenID,edgeID)�������˵�
	int nv=(int)polygenList[polygenID].vlist.size();
	float *v1=polygenList[polygenID].vlist[edgeID].v;
	float *v2=polygenList[polygenID].vlist[(edgeID+1)%nv].v;
	//�ж�������v1v2���Ǵ��뻹�Ǵ���
	//ֻ�轫���ߵķ�������������v1v2�����
	//��������YֵΪ������Ϊ���룻��������YֵΪ������Ϊ����
	bool penetrateOut;
	float ray_e[4]={1,0,0,0};//���ߵķ�������
	float v1v2[4];
	sub(v2,v1,v1v2);
	v1v2[Y]=0;
	float norm[4];
	cross(ray_e,v1v2,norm);
	if(norm[Y]>0){//����
		penetrateOut=true;
	}else{
		penetrateOut=false;
	}
	if(penetrateOut){//�������
		//˵��(cx,cy)��polygenList�ڲ�
		return true;
	}else{//����˵��(cx,cy)��polygenList�ⲿ
		return false;
	}

}



inline void calNormfAndNormn(Cpolygen&polygen){
	int nv=(int)polygen.vlist.size();
	for(int k=0;k<nv;k++){
		Cv4Ex&v=polygen.vlist[k];
		float *vf=polygen.vlist[(k-1+nv)%nv].v;//%%%%%%%%%%%%%%%%%%%%%%%%
		float *vn=polygen.vlist[(k+1)%nv].v;
		//��vfv���ҷ�����
		float vfv[4];
		sub(v.v,vf,vfv);
		float normf[4]={-vfv[Z],0,vfv[X],0};
		normalize(normf);
		//��vvn���ҷ�����
		float vvn[4];
		sub(vn,v.v,vvn);
		float normn[4]={-vvn[Z],0,vvn[X],0};
		normalize(normn);
		//��normf��normn����v
		veccopy(normf,v.normf);
		veccopy(normn,v.normn);	
	}

}

inline int getNearestEdgeOfPolygenList(myvector<Cpolygen>&polygenList,float v[4],int&I,int&J)
//��polygenList�о����v����ıߣ�����(I,J)��I��polygen��ţ�J�Ǳߵı��
//ǰ�˷���v������ߵ���̾����ںδ�ȡ�ã�����0�����ڴ���ȡ�ã�����1�����ڱߵ����ȡ�ã�����2�����ڱߵ��յ�ȡ��
{
	//��polygenList�о���v����ı�
	I=-1;
	J=-1;
	float mind2=inf;//��������ƽ��
	int where=-1;//��̾����ںδ�ȡ��
	int npolygen=(int)polygenList.size();
	for(int i=0;i<npolygen;i++){
		Cpolygen&polygen=polygenList[i];
		int nv=(int)polygen.vlist.size();
		for(int j=0;j<nv;j++){
			float*v1=polygen.vlist[j].v;
			float*v2=polygen.vlist[(j+1)%nv].v;
			//��v��ֱ��v1v2�ľ���(ֻ����xz���ϵ�ͶӰ)
			//��v1v2���ҷ�����
			float v1v2_e[4];//v1v2��xz���ϵ�ͶӰ�ķ�������
			sub(v2,v1,v1v2_e);
			v1v2_e[Y]=0;
			normalize(v1v2_e);
			float norm[4]={-v1v2_e[Z],0,v1v2_e[X],0};
			float v1v[4];
			sub(v,v1,v1v);
			v1v[Y]=0;
			float PND=dot(v1v,norm);//v��ֱ��v1v2���������
			//��v��ֱ��v1v2�ϵ�ͶӰ��
			//v_proj=v-PND*norm���ֽ�Ϊv_proj=v-tvec,tvec=PND*norm
			float v_proj[4];
			float tvec[4];
			mul(PND,norm,tvec);
			sub(v,tvec,v_proj);
			//�ж�v_proj�Ƿ���v1v2֮��
			float v1v_proj[4];
			sub(v_proj,v1,v1v_proj);
			v1v_proj[Y]=0;
			float v_projv2[4];
			sub(v2,v_proj,v_projv2);
			v_projv2[Y]=0;
			if(dot(v1v_proj,v_projv2)>=0){//v_proj��v1v2��
				//v���߶�v1v2�ľ��뼴fabs(PND)
				float d=fabs(PND);
				float d2=pow2(d);
				if(d2<mind2){
					mind2=d2;
					I=i;
					J=j;
					where=0;
				}
			}else{//v��v1v2��
				//v���߶�v1v2�ľ����ƽ��Ϊmin(d2_vv1,d2_vv2)
				//��xz����v��v1�ľ����ƽ��
				float d2_vv1=pow2(v[X]-v1[X])+pow2(v[Z]-v1[Z]);
				//��xz����v��v2�ľ����ƽ��
				float d2_vv2=pow2(v[X]-v2[X])+pow2(v[Z]-v2[Z]);
				//��v���߶�v1v2�ľ����ƽ��
				float d2=min(d2_vv1,d2_vv2);
				if(d2<mind2){
					mind2=d2;
					I=i;
					J=j;
					if(d2_vv1<=d2_vv2){
						where=1;
					}else{
						where=2;
					}
				}
			}
		}
	}//�õ�I,J,mind2,where
	return where;
}

inline float PND_p_line_xz(const float _v[4],Cline&line){//��xzƽ������㵽ֱ�ߵ��������
	float v[4];
	veccopy(_v,v);
	v[Y]=0;
	//��ֱ�ߵ��ҷ�����
	float v1[4];
	veccopy(line.p,v1);
	v1[Y]=0;
	float dir[4];
	veccopy(line.dir,dir);
	dir[Y]=0;
	float norm[4]={-dir[Z],0,dir[X],0};
	float v1v[4];
	sub(v,v1,v1v);
	return dot(v1v,norm);

}

inline void ball_seeback(int m,//����
				 float cx,float cy,float cz,//����λ��
				 float r,//��뾶
				 float campos[4],//���λ��
				 float k=1//Y��ѹ����
				 )//������?????????????������δ��λ��
{//rΪ��뾶,nΪ�������,mΪ����
	int n=m/2;
	glBegin(GL_QUADS);
	for(int i=0;i<=m-1;i++){
		for(int j=0;j<=2*n-1;j++){
			float A=i*2*pi/m;//��x��нǣ����ȣ�
			float B=-pi/2+j*pi/(2*n);//��XZƽ��нǣ����ȣ�
			float dA=2*pi/m;
			float dB=pi/(2*n);
			//���ı���(A,B)->(A,B+dB)->(A+dA,B+dB)->(A+dA,B)->(A,B)
			float s,t;//��ͼ����
			//���ĸ���������
			float v0[4],v1[4],v2[4],v3[4];
			float u0[2],u1[2],u2[2],u3[2];
			//   v2(A+dA,B+dB)---v1(A,B+dB)
			//       |            |
			//   v3(A+dA,B)------v0(A,B)
			//v0
			{
				float x=cx+r*cos(B)*cos(A);
				float y=cy+k*r*sin(B);
				float z=cz+r*cos(B)*sin(A);
				float s=(float)i/m;
				float t=(float)j/(2*n);
				init4(v0,x,y,z,1);
				init2(u0,s,t);
			}
			//v1
			{
				float x=cx+r*cos(B+dB)*cos(A);
				float y=cy+k*r*sin(B+dB);
				float z=cz+r*cos(B+dB)*sin(A);
				float s=(float)i/m;
				float t=(float)(j+1)/(2*n);
				init4(v1,x,y,z,1);
				init2(u1,s,t);

			}
			//v2
			{
				float x=cx+r*cos(B+dB)*cos(A+dA);
				float y=cy+k*r*sin(B+dB);
				float z=cz+r*cos(B+dB)*sin(A+dA);
				float s=(float)(i+1)/m;
				float t=(float)(j+1)/(2*n);
				init4(v2,x,y,z,1);
				init2(u2,s,t);

			}
			//v3
			{
				float x=cx+r*cos(B)*cos(A+dA);
				float y=cy+k*r*sin(B);
				float z=cz+r*cos(B)*sin(A+dA);
				float s=(float)(i+1)/m;
				float t=(float)j/(2*n);
				init4(v3,x,y,z,1);
				init2(u3,s,t);

			}
			//������
			float norm[4];
			float v0v1[4],v0v2[4];
			sub(v1,v0,v0v1);
			sub(v2,v0,v0v2);
			cross(v0v1,v0v2,norm);//����norm��λ���ˣ�������Ϊ�����Ҳ����������
			//�жϿ������Ƿ�Ϊ����
			float camposv0[4];//��camposָ��v������
			sub(v0,campos,camposv0);
			float cosA=dot(camposv0,norm);
			if(cosA<0){//������������
				continue;//��������
			}
	//		glNormal3fv(norm);//���ø�������
			glTexCoord2fv(u0);glVertex3fv(v0);
			glTexCoord2fv(u1);glVertex3fv(v1);
			glTexCoord2fv(u2);glVertex3fv(v2);
			glTexCoord2fv(u3);glVertex3fv(v3); 
		}
	}
	glEnd();
}	




inline calNorm(float v0[4],float v1[4],float v2[4],float norm[4]){
	float v0v1[4],v0v2[4];
	sub(v1,v0,v0v1);
	sub(v2,v0,v0v2);
	cross(v0v1,v0v2,norm);
	normalize(norm);
}



inline	float PND_point_plane(const Cplane&plane,const float point[4]){//�ǵ㵽ƽ����������
	const float *norm=plane.norm;
	const float *p=plane.p;
	float ppoint[4];
	sub(point,p,ppoint);
	float PNd=dot(norm,ppoint);
	return PNd;
}

inline int getPointOfIntersect_line_plane(const float p1[4],const float p2[4],const Cplane&plane,float p3[4])
//��p1p2��ȷ����ֱ����ƽ��plane�Ľ���p3��ע����ֱ�߶����߶Σ�
//ǰ�˷���0��ʾ�޽���
//����1��ʾǡ��һ������
//����2��ʾ������ཻ��
//������ཻ��ʱp3ֱ�ӵ���p1
{
	//��ֱ��p1p2��ƽ��Ľ���p3
	const float *norm=plane.norm;
	const float *p=plane.p;
	float v[4];//ֱ�ߵĵ�λ��������
	sub(p2,p1,v);
	normalize(v);
	float p1p[4];
	sub(p,p1,p1p);
	//p0��ƽ����������t
	double a=dot(p1p,norm);
	double b=dot(norm,v);
	if(a==0&&b==0){//����ཻ��
		veccopy(p1,p3);
		return 2;
	}else if(a!=0&&b==0){//�޽���
		return 0;
	}else{//ǡ��һ����
		float t=a/b;
		//�󽻵�p3
		float vt[4];
		mul(t,v,vt);
		add(p1,vt,p3);//�õ�p3
		return 1;
	}
}

inline float getWhoseFabsIsMax(const myvector<float>&list){//��list�о���ֵ����Ԫ��
	float who=list[0];
	int n=(int)list.size();
	for(int i=1;i<n;i++){
		if(fabs(list[i])>fabs(who)){
			who=list[i];
		}
	}//�õ�who
	return who;
}
inline bool plane_div_polygen(const Cplane&plane,const Cpolygen&polygen,
					   Cpolygen&polygen1,float&side1,
					   Cpolygen&polygen2,float&side2,
					   bool&iscoplane,bool&isdivided)
					   //��plane�и�����polygen
					   //ǰ�˷����Ƿ�������isdividedָʾ�Ƿ��г���������
					   //�涨��
					   //�����������
					   //	isdivided=false����������ֵ����Ч
					   //���������
					   //	iscoplane==true��isdivided=false��side1=side2=0��polygne1��polygen2��Ч
					   //	iscoplane==false��isdivided==true��polygen1,polygen2Ϊ�гɵ���������Σ�����֤������side1,side2ָʾpolygen1,polygen2��plane���棨Ҳ���ܵ�0��
					   //	iscoplane==false��isdivided==false��polygne1��polygen2��Ч��side1,side2ָʾpolygen��plane���棨Ҳ���ܵ�0��
{	
	//----��side�б�ͬʱ�õ�iscoplane
	int n=(int)polygen.vlist.size();
	myvector<float> sideList;//��¼polygen������λ��plane�ı�
	iscoplane=true;
	sideList.resize(n);
	for(int i=0;i<=n-1;i++){
		const float *v=polygen.vlist[i].v;
		float side=PND_point_plane(plane,v);
		sideList[i]=side;
		if(side!=0)iscoplane=false;//��һ������ƽ���Ͼ�Ϊfalse
	}//�õ�sideList��iscoplane
	if(iscoplane){
		side1=side2=0;
		isdivided=false;
		return true;//����
	}
	//----��plane��polygen�߽�Ľ���
	float ID[2];//��������ı�ţ�������������Ƕ���ζ��㣬��IDΪ�����ţ���������ڱ��α��ϣ����ڶ���ζ���i��i+1֮�䣬��IDΪi+0.5
	float p[2][4];//��¼ID��Ӧ�Ķ��������
	int top=0;//IDջջ��ָ��,ͬʱҲ��pջջ��ָ��,Ҳ�ǽ������
	//��ID,top
	//�����붥��Ľ���
	for(i=0;i<=n-1;i++){
		if(top==2)break;//��2������
		if(sideList[i]==0){//�ַ��ֽ���
			ID[top]=i;
			const float *v=polygen.vlist[i].v;
			veccopy(v,p[top]);
			top++;
		}
	}
	//����ߵĽ���
	for(i=0;i<=n-1;i++){
		if(top==2)break;//��2������
		float side1,side2;
		side1=sideList[i];
		side2=sideList[(i+1)%n];
		if(side1*side2<0){
			ID[top]=i+0.5;
			const float *p1=polygen.vlist[i].v;
			const float *p2=polygen.vlist[(i+1)%n].v;
			getPointOfIntersect_line_plane(p1,p2,plane,p[top]);
			top++;
		}
	}//�õ�ID,top
	//���˽�����<=2
	if(top==0||top==1){//���������<2
		//˵������polygen��plane��һ��
		//�����ıߣ�ֻ�뿴��Զ�����ı�
		float side=getWhoseFabsIsMax(sideList);
		side1=side2=side;
		isdivided=false;
		return true;//����
	}
	//���˽������==2
	if(ID[0]==(int)ID[0]&&ID[1]==(int)ID[1]&&(((int)ID[1]+1)%n==ID[0]||ID[0]+1==ID[1])){//plane��polygenĳ���غ�
		//˵������polygen��plane��һ��
		//�����ıߣ�ֻ�뿴��Զ�����ı�
		float side=getWhoseFabsIsMax(sideList);
		side1=side2=side;
		isdivided=false;
		return true;//����
	}
	//----����ID[0],ID[1]�����ָ�����������Σ�������onPside1��onPside2��ֵ
	//����Ϊ�����������򣬼�ʹID[0]<=ID[1]
	if(ID[0]>ID[1]){
		int temp=ID[0];
		ID[0]=ID[1];
		ID[1]=temp;
	}
	//����polygen1������side1
	myvector<float> sideList1;//polygen1��side�б�
	for(i=0;i<=ID[0];i++){
		polygen1.addvert((float*)polygen.vlist[i].v);
		sideList1.push_back(sideList[i]);
	}
	if(ID[0]!=(int)ID[0]){//���ID[0]������ֵ
		//���붥��
		polygen1.addvert(p[0]);
	}
	if(ID[1]!=(int)ID[1]){//���ID[1]������ֵ
		//���붥��
		polygen1.addvert(p[1]);
	}
	for(i=ceil(ID[1]);i<=n-1;i++){
		polygen1.addvert((float*)polygen.vlist[i].v);
		sideList1.push_back(sideList[i]);
	}//�õ�polygen1��sideList1
	//��side1
	side1=getWhoseFabsIsMax(sideList1);
	//����polygen2������side2
	myvector<float> sideList2;//polygen2��side�б�
	if(ID[0]!=(int)ID[0]){//���ID[0]������ֵ
		//���붥��
		polygen2.addvert(p[0]);
	}
	for(i=ceil(ID[0]);i<=floor(ID[1]);i++){
		polygen2.addvert((float*)polygen.vlist[i].v);
		sideList2.push_back(sideList[i]);
	}
	if(ID[1]!=(int)ID[1]){//���ID[1]������ֵ
		polygen2.addvert(p[1]);
		
	}//�õ�polygen2��sideList2
	//��side2
	side2=getWhoseFabsIsMax(sideList2);
	//��side1,side2�����ж�
	//��side1����
	int sgn1;//side1����
	if(side1==0){
		sgn1=0;
	}else if(side1>0){
		sgn1=1;
	}else{
		sgn1=-1;
	}
	//��side2����
	int sgn2;//side2����
	if(side2==0){
		sgn2=0;
	}else if(side2>0){
		sgn2=1;
	}else{
		sgn2=-1;
	}
	if(sgn1==sgn2){//�����и�
		isdivided=false;
		return true;//����
	}
	//�����и�(������֤side1��side2����0)
	isdivided=true;
	return true;//����
	
}

inline	bool getNormalOfTri(const float p0[4],const float p1[4],const float p2[4],float norm[4]){//���������ķ�������ǰ�˷����Ƿ񹹳�������
		float p01[4],p02[4];
		sub(p1,p0,p01);
		sub(p2,p0,p02);
		cross(p01,p02,norm);
		if(normalize(norm)==0){//���nromģΪ0��˵�����㹲��
			return false;
		}
		return true;
	}


inline void drawWall_polygenList_polygenList(myvector<Cpolygen>&polygenList,myvector<Cpolygen>&polygenList_dn,GLuint texture,int stepLen=1,bool enableTexture=true)
//����polygenList��polygenList_dn,��ǽ����
//stepLenָ��������ȡһ���㡣Ĭ����1������������
{
	if(stepLen<1){
		cout<<"error! stepLen����>=1"<<endl;
		return;
	}
	if(enableTexture){
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,texture);
	}
	glBegin(GL_QUADS);
	//����polygenList��polygenList_dn����ǽ������
	int npolygen=(int)polygenList.size();
	for(int i=0;i<npolygen;i++){		
		Cpolygen&polygenUp=polygenList[i];//�ϵ׶����
		Cpolygen&polygenDn=polygenList_dn[i];//�µ׶����
		//����ǽ����
		int nv=(int)polygenDn.vlist.size();
		int j,jn;//j,jn���ɵ�ǰ��
		j=0;
		while(1){
			//��jn
			jn=j+stepLen;
			if(jn<=nv-1){
				//��(j,jn)�ϵ�һ��isKeyPoint��
				int index=-1;//(j,jn)�ϵ�һ��isKeyPoint�������
				for(int k=j+1;k<jn;k++){
					Cv4Ex&v=polygenDn.vlist[k];
					if(v.isKeyPoint){
						index=k;
						break;
					}
				}
				if(index==-1){//û�ҵ�
					//��jn������ȷ��jn
					//�޶���
				}else{//�ҵ���
					//Ӧ��index��Ϊjn
					jn=index;
				}
			}else if(jn>=nv){
				//��(j,nv-1]�ϵ�һ��isKeyPoint��
				int index=-1;//(j,nv-1]�ϵ�һ��isKeyPoint�������
				for(int k=j+1;k<nv;k++){
					Cv4Ex&v=polygenDn.vlist[k];
					if(v.isKeyPoint){
						index=k;
						break;
					}
				}
				if(index==-1){//û�ҵ�
					//����0��Ϊjn
					jn=0;
				}else{//�ҵ���
					//Ӧ��index��Ϊjn
					jn=index;
				}
			}//�õ�jn
			//��j,jn��ǽ����
			//
			//   vup1--vup2
			//     |    |
			//   vdn1--vdn2
			float *vup1=polygenUp.vlist[j].v;
			float *vup2=polygenUp.vlist[jn].v;
			float *vdn1=polygenDn.vlist[j].v;
			float *vdn2=polygenDn.vlist[jn].v;

			float *uup1=polygenUp.vlist[j].un;
			float *uup2=polygenUp.vlist[jn].uf;
			float *udn1=polygenDn.vlist[j].un;
			float *udn2=polygenDn.vlist[jn].uf;
			float norm[4];
			getNormalOfTri(vup2,vup1,vdn1,norm);
			glNormal3fv(norm);
			glTexCoord2fv(uup2);glVertex3fv(vup2);
			glTexCoord2fv(uup1);glVertex3fv(vup1);
			glTexCoord2fv(udn1);glVertex3fv(vdn1);
			glTexCoord2fv(udn2);glVertex3fv(vdn2);
			//�ж���Ȧǽ���Ƿ�����
			if(jn==0){//���jn==0����˵����Ȧǽ��������
				//����ѭ��
				break;
			}
			//��jn��Ϊ�µ�j
			j=jn;
			
		}
	}
	glEnd();
	if(enableTexture){
		glDisable(GL_TEXTURE_2D);
	}
}

inline float calH_BilinearAABB(const float vlup[4],const float vldn[4],const float vrup[4],const float vrdn[4],const float _v[4])
//AABB���������Բ�ֵ���
{
	//ͨ��˫���Բ�ֵ��_v���ĸ߶ȣ����������AABB����ƽ�еģ���
	//  vlup----vrup
	//   |        |
	//   |_v(x,z) |
	//  vldn----vrdn
	float H;
	float x=_v[X];
	float z=_v[Z];
	//��x��vlup-vrup���ϵĸ߶�ֵ
	float Hup;
	{
		float x1=vlup[X];
		float H1=vlup[Y];
		float x2=vrup[X];
		float H2=vrup[Y];
		Hup=(x2-x)/(x2-x1)*(H1-H2)+H2;
		
	}
	//��x��vldn-vrdn���ϵĸ߶�ֵ
	float Hdn;
	{
		float x1=vldn[X];
		float H1=vldn[Y];
		float x2=vrdn[X];
		float H2=vrdn[Y];
		Hdn=(x2-x)/(x2-x1)*(H1-H2)+H2;
		
	}
	//��z��Hup-Hdn���ϵĸ߶�ֵ
	float Hxz;
	{
		float z1=vlup[Z];
		float H1=Hup;
		float z2=vldn[Z];
		float H2=Hdn;
		Hxz=(z2-z)/(z2-z1)*(H1-H2)+H2;
		
	}
	//��z��vlup-vldn���ϵĸ߶�ֵ
	float Hlt;
	{
		float z1=vlup[Z];
		float H1=vlup[Y];
		float z2=vldn[Z];
		float H2=vldn[Y];
		Hlt=(z2-z)/(z2-z1)*(H1-H2)+H2;
	}
	//��z��vrup-vrdn���ϵĸ߶�ֵ
	float Hrt;
	{
		float z1=vrup[Z];
		float H1=vrup[Y];
		float z2=vrdn[Z];
		float H2=vrdn[Y];
		Hrt=(z2-z)/(z2-z1)*(H1-H2)+H2;
	}
	//��x��Hlt-Hrt���ϵĸ߶�ֵ
	float Hzx;
	{
		float x1=vlup[X];
		float H1=Hlt;
		float x2=vrup[X];
		float H2=Hrt;
		Hzx=Hlt=(x2-x)/(x2-x1)*(H1-H2)+H2;
		
	}
	H=(Hxz+Hzx)/2;
	return H;


}

inline bool isHolePolygen(const Cpolygen&polygen)
//�ж�polygen�Ƿ�Ϊ��
//����polygen��xzƽ���ϵĶ����
{
	//���Ƿ����һ���ߣ��������ඥ�㶼�ڴ˱ߵ��Ҳ�
	int nv=(int)polygen.vlist.size();
	for(int i=0;i<nv;i++){
		const Cv4Ex&v=polygen.vlist[i];
		const Cv4Ex&vn=polygen.vlist[(i+1)%nv];
		//����polygen�ϳ�v��vn֮������ж����Ƿ���vvn���Ҳ�
		bool allAtRight=true;
		for(int j=0;j<nv;j++){
			if(j==i||j==(i+1)%nv)continue;
			const Cv4Ex&_v=polygen.vlist[j];
			//��_v�Ƿ���vvn���Ҳ�
			//����v_v��vvn���ҷ������ϵ�ͶӰ�Ƿ�>=0
			//��v_v
			float v_v[4];
			sub(_v.v,v.v,v_v);
			//��vvn���ҷ�����
			float dir[4];//vvn�ķ���������δ��λ��
			sub(vn.v,v.v,dir);
			float norm[4]={-dir[Z],0,dir[X],0};//vvn���ҷ�����
			normalize(norm);
			//��v_v��norm�ϵ�ͶӰ�Ƿ�>=0
			float proj=dot(v_v,norm);
			if(proj>=0){//_v��vvn�Ҳ�
				//����
			}else{//_v����vvn�Ҳ�
				allAtRight=false;
				break;
			}
		}//�õ�allAtRight
		if(allAtRight){
			return true;//ֻҪ�ҵ���һ�������ıߣ���˵���Ƕ�
			
		}
	}
	return false;

}

inline bool isOutlinePolygen(const Cpolygen&polygen)
//�ж�polygen�Ƿ�Ϊ���������
//����polygen��xzƽ���ϵĶ����
{
	//���Ƿ����һ���ߣ��������ඥ�㶼�ڴ˱ߵ����
	int nv=(int)polygen.vlist.size();
	for(int i=0;i<nv;i++){
		const Cv4Ex&v=polygen.vlist[i];
		const Cv4Ex&vn=polygen.vlist[(i+1)%nv];
		//����polygen�ϳ�v��vn֮������ж����Ƿ���vvn�����
		bool allAtLeft=true;
		for(int j=0;j<nv;j++){
			if(j==i||j==(i+1)%nv)continue;
			const Cv4Ex&_v=polygen.vlist[j];
			//��_v�Ƿ���vvn�����
			//����v_v��vvn���ҷ������ϵ�ͶӰ�Ƿ�<=0
			//��v_v
			float v_v[4];
			sub(_v.v,v.v,v_v);
			//��vvn���ҷ�����
			float dir[4];//vvn�ķ���������δ��λ��
			sub(vn.v,v.v,dir);
			float norm[4]={-dir[Z],0,dir[X],0};//vvn���ҷ�����
			normalize(norm);
			//��v_v��norm�ϵ�ͶӰ�Ƿ�<=0
			float proj=dot(v_v,norm);
			if(proj<=0){//_v��vvn���
				//����
			}else{//_v����vvn���
				allAtLeft=false;
				break;
			}
		}//�õ�allAtRight
		if(allAtLeft){
			return true;//ֻҪ�ҵ���һ�������ıߣ���˵�������������
			
		}
	}
	return false;

}

inline bool isSelfIntersect(const Cpolygen&polygen)
//�ж�polygen�Ƿ����ཻ
//����polygen��xzƽ���ϵĶ����
{
	
	int nv=(int)polygen.vlist.size();
	for(int i=0;i<nv;i++){
		const Cv4Ex&v=polygen.vlist[i];
		const Cv4Ex&vn=polygen.vlist[(i+1)%nv];
		//����������vvn�޹�������ı��Ƿ�����֮�ཻ��
		for(int j=0;j<nv;j++){
			const Cv4Ex&_v=polygen.vlist[j];
			const Cv4Ex&_vn=polygen.vlist[(j+1)%nv];
			if(j!=i&&j!=(i+1)%nv&&(j+1)%nv!=i&&(j+1)%nv!=(i+1)%nv){
				//��_v��_vn�Ƿ��ཻ
				bool intersect=LineSegOverLineSeg(v.v[X],v.v[Z],vn.v[X],vn.v[Z],_v.v[X],_v.v[Z],_vn.v[X],_vn.v[Z]);
				if(intersect){
					return true;
				}
				
			}
		
		}
	}
	return false;


}

inline void drawTrianFace(float p[3][4],float u[3][2]){//������Ϊp[0]~p[2]����������,u[i]Ϊ��Ӧ��ͼ����
		//   p[0]
		//  /   \
		// p[1]-p[2]
		//������
		float norm[4];
		float p01[4],p02[4];
		sub(p[1],p[0],p01);
		sub(p[2],p[0],p02);
		cross(p01,p02,norm);
		normalize(norm);
		//��������
		glBegin(GL_TRIANGLES);
		glNormal3fv(norm);

		glTexCoord2fv(u[0]);
		glVertex3fv(p[0]);
		
		glTexCoord2fv(u[1]);
		glVertex3fv(p[1]);
		
		glTexCoord2fv(u[2]);
		glVertex3fv(p[2]);
		glEnd();

		glLineWidth(5);
		glBegin(GL_LINES);
		glVertex3fv(p[0]);
		glVertex3fv(p[1]);
		
		glVertex3fv(p[1]);
		glVertex3fv(p[2]);
		
		glVertex3fv(p[2]);
		glVertex3fv(p[0]);
		
		glEnd();
		glLineWidth(1);
	}





	




	

