inline bool getBmp24Size(string pathName,int&bmpWidth,int&bmpHeight)
//获得24位bmp图片尺寸
//如果不是24位bmp文件，则返回false，否则true
{
	//打开文件
	FILE* fp=fopen(pathName.c_str(),"rb");//二进制读
	if(fp==NULL)
	{ 
		cout<<"Open file "<<pathName<<" failed!"<<endl; 
		return false;
	}
	//读文件头
	BYTE  Head[54];
	fread(Head,54,1,fp); 
	//关闭文件
	fclose(fp);
	//分析文件头
	{
		//检查是否为bmp文件
		char type[2]={Head[0],Head[1]};
		if(type[0]=='B'&&type[1]=='M'){
			//是bmp文件
		}else{
			//不是bmp文件
			cout<<"不是bmp文件!"<<endl;
			return false;
		}
		//检查是否为24位
		int bitCount=Head[28]+Head[29]*256;
		if(bitCount!=24){
			//不是24位
			cout<<"不是24位bmp文件!"<<endl;
			return false;
		}
		//通过检查，获得尺寸
		bmpWidth =Head[18]+Head[19]*256;
		bmpHeight=Head[22]+Head[23]*256;
	}

	return true;
}
inline string ridLastLevelOfPathName(string pathName){//去掉路径名的最后一级（保留末尾的\\）
	int count=0;//计数最后一级的字符个数
	int n=(int)pathName.size();
	for(int i=n-1;i>=0;i--)
	{
		if(pathName[i]!='\\'){
			count++;
		}else{
			break;
		}
	}//得到count
	pathName.resize(n-count);
	return pathName;
}
inline void makeColorTable(float colorTable[256][3]){//生成颜色表
	//先将colorTable所有元素复值为-1
	for(int i=0;i<256;i++){
		for(int j=0;j<3;j++){
			colorTable[i][j]=-1;
		}
	}
	//定义常量
	const int r=0;
	const int g=1;
	const int b=2;
	//制作基准值
	colorTable[0][r]=0;colorTable[0][g]=0;colorTable[0][b]=1;
	colorTable[63][r]=0;colorTable[63][g]=1;colorTable[63][b]=1;
	colorTable[127][r]=0;colorTable[127][g]=1;colorTable[127][b]=0;
	colorTable[191][r]=1;colorTable[191][g]=1;colorTable[191][b]=0;
	colorTable[255][r]=1;colorTable[255][g]=0;colorTable[255][b]=0;
	//对colorTable中没有值的部分进行插值
	for(i=0;i<3;i++){
		//对colorTable[][i]进行插值
		int ibeg=0;
		int iend=ibeg+1;
		while(1){
			//iend去找下一个有值的元素
			for(;iend<256;iend++){
				if(colorTable[iend][i]!=-1)break;
			}
			if(iend==256){//没找到
				break;
			}else{//找到
				//对(ibeg,iend)内的元素进行插值
				float dH=(colorTable[iend][i]-colorTable[ibeg][i])/(iend-ibeg);
				float H=colorTable[ibeg][i];
				for(int j=ibeg+1;j<iend;j++){
					H+=dH;
					colorTable[j][i]=H;
				}//此段填充完成
				//去找下一段
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
						//判断线段(x1,y1)--(x2,y2)和线段(u1,v1)--(u2,v2)是否相交（不包括端点）
{
	//判断(u1,v1)和(u2,v2)是否在直线(x1,y1)-(x2,y2)异同侧
	bool u1v1_separate_u2v2=false;
	double a=(v1-y1)*(x2-x1)-(y2-y1)*(u1-x1);
	double b=(v2-y1)*(x2-x1)-(y2-y1)*(u2-x1);
	if(a*b<0)u1v1_separate_u2v2=true;
	//判断(x1,y1)和(x2,y2)是否在直线(u1,v1)-(u2,v2)异同侧
	bool x1y1_separate_x2y2=false;
	a=(y1-v1)*(u2-u1)-(v2-v1)*(x1-u1);
	b=(y2-v1)*(u2-u1)-(v2-v1)*(x2-u1);
	if(a*b<0)x1y1_separate_x2y2=true;
	return x1y1_separate_x2y2*u1v1_separate_u2v2;
}


inline void ridSelfInteract_polygen(Cpolygen&polygen)
//为多边形polygen去自相交
//假定polygen顶点数大于等于3
{
	for(int i=0;i<(int)polygen.vlist.size();i++){
		//i号边的两端点坐标
		float*v1=polygen.vlist[i].v;
		int inext=(i+1)%(int)polygen.vlist.size();
		float*v2=polygen.vlist[inext].v;
		//看i号边之后的边有无与i号边相交者
		for(int j=i+2;j<(int)polygen.vlist.size();j++){//j从i+2开始是因为i+1与i是邻边，必不相交
			if(i==0&&j==(int)polygen.vlist.size()-1)continue;//此时i号边与j号边是邻边，不作检测
			float*_v1=polygen.vlist[j].v;
			int jnext=(j+1)%(int)polygen.vlist.size();
			float*_v2=polygen.vlist[jnext].v;
			//看v1v2是否与_v1_v2相交
			bool interact=LineSegOverLineSeg(v1[X],v1[Z],v2[X],v2[Z],_v1[X],_v1[Z],_v2[X],_v2[Z]);
			if(interact){//如果相交
				//看是v1v2和_v1_v2谁短，谁短就删除谁的两个端点
				if(pow2(v1[X]-v2[X])+pow2(v1[Z]-v2[Z])<pow2(_v1[X]-_v2[X])+pow2(_v1[Z]-_v2[Z])){//v1v2短
					//删除v1v2
					polygen.vlist.erase(&polygen.vlist[inext]);
					i--;//抵消下一次for循环的自动加1

				}else{//_v1_v2短
	
					polygen.vlist.erase(&polygen.vlist[jnext]);
					j--;//抵消下一次for循环的自动加1
					
					
				}
				
			}


		}
	}
}

inline bool isPointInPolygenList_reduced(float cx,float cz,myvector<Cpolygen>&_polygenList)
//判断(cx,cz)是否在polygenList内部(使用各polygen的简化版本进行判断)
//使用射线法
//但为了简化，不是求所有交点，而是只求与发射点最近的交点，看射线在此处是穿入还是穿出。显然，此法对于自相交的polygenList可能无法正确处理
{
	//制作_polygenList的简化版本polygenList
	myvector<Cpolygen> polygenList;
	{
		
		polygenList.resize((int)_polygenList.size());
		int npolygen=(int)polygenList.size();
		for(int i=0;i<npolygen;i++){
			Cpolygen&polygen=polygenList[i];
			Cpolygen&_polygen=_polygenList[i];
			//制作polygen
			//将_polygenList中的isKeyPoint添加到polygen
			int nv=(int)_polygen.vlist.size();
			for(int j=0;j<nv;j++){
				Cv4Ex&v=_polygen.vlist[j];
				if(v.isKeyPoint){
					//将v添加到polygen
					polygen.vlist.push_back(v);
				}
			}
		}//得到polygenList
	}
	//判断(cx,cz)是否在polygenList内部
	{
		
		//由(cx,cz)向右作水平射线
		//求polygenList中哪些边与射线相交，求所有交点中距离(cx,cz)最近的（即x最小的）
		float xmin=inf;//交点x的最小值
		int polygenID=-1;//取得x最小的交点的多边形ID
		int edgeID=-1;//取得x最小的交点的边的ID
		int npolygen=(int)polygenList.size();
		for(int i=0;i<npolygen;i++){
			Cpolygen&polygen=polygenList[i];
			int nv=(int)polygen.vlist.size();
			for(int j=0;j<nv;j++){
				float *v1=polygen.vlist[j].v;
				float *v2=polygen.vlist[(j+1)%nv].v;
				//判断半开半闭线段[v1,v2)是否与射线相交
				//先看是否有(v1[Z]>=cz&&v2[Z]<cz)或(v1[Z]<=cz&&v2[Z]>cz)，如果没有，则必定不与射线相交，直接跳过
				if((v1[Z]>=cz&&v2[Z]<cz)||(v1[Z]<=cz&&v2[Z]>cz)){//可能相交
					//放行
				}else{//不可能相交
					//跳过
					continue;
				}
				//有可能相交，则求直线v1v2与射线所在直线的交点的x坐标
				float x1=v1[X];
				float z1=v1[Z];
				float x2=v2[X];
				float z2=v2[Z];
				float x=(cz-z1)*(x2-x1)/(z2-z1)+x1;
				//如果此交点的X坐标>cx，则说明与射线相交
				if(x>cx){//v1v2与射线相交
					if(x<xmin){
						xmin=x;
						polygenID=i;
						edgeID=j;
					}
				}
			}
			
		}//得到xmin,polygenID,edgeID
		if(polygenID==-1){//如果没有交点
			//说明(cx,cz)必定在polygenList外部
			return false;
		}
		//判断与(polygenID,edgeID)的交点是穿入还是穿出
		//求边(polygenID,edgeID)的两个端点
		int nv=(int)polygenList[polygenID].vlist.size();
		float *v1=polygenList[polygenID].vlist[edgeID].v;
		float *v2=polygenList[polygenID].vlist[(edgeID+1)%nv].v;
		//判断射线在v1v2处是穿入还是穿出
		//只需将射线的方向向量与向量v1v2作叉积
		//如果叉积的Y值为负，则为穿入；如果叉积的Y值为正，则为穿出
		bool penetrateOut;
		float ray_e[4]={1,0,0,0};//射线的方向向量
		float v1v2[4];
		sub(v2,v1,v1v2);
		v1v2[Y]=0;
		float norm[4];
		cross(ray_e,v1v2,norm);
		if(norm[Y]>0){//穿出
			penetrateOut=true;
		}else{
			penetrateOut=false;
		}
		if(penetrateOut){//如果穿出
			//说明(cx,cy)在polygenList内部
			return true;
		}else{//否则说明(cx,cy)在polygenList外部
			return false;
		}
	}

}

inline bool isPointInPolygenList(float cx,float cz,myvector<Cpolygen>&polygenList)
//判断(cx,cz)是否在polygenList内部(使用各polygen的非简化版本进行判断)
//使用射线法
{
	//由(cx,cz)向右作水平射线
	//求polygenList中哪些边与射线相交，求所有交点中距离(cx,cz)最近的（即x最小的）
	float xmin=inf;//交点x的最小值
	int polygenID=-1;//取得x最小的交点的多边形ID
	int edgeID=-1;//取得x最小的交点的边的ID
	int npolygen=(int)polygenList.size();
	for(int i=0;i<npolygen;i++){
		Cpolygen&polygen=polygenList[i];
		int nv=(int)polygen.vlist.size();
		for(int j=0;j<nv;j++){
			float *v1=polygen.vlist[j].v;
			float *v2=polygen.vlist[(j+1)%nv].v;
			//判断半开半闭线段[v1,v2)是否与射线相交
			//先看是否有(v1[Z]>=cz&&v2[Z]<cz)或(v1[Z]<=cz&&v2[Z]>cz)，如果没有，则必定不与射线相交，直接跳过
			if((v1[Z]>=cz&&v2[Z]<cz)||(v1[Z]<=cz&&v2[Z]>cz)){//可能相交
				//放行
			}else{//不可能相交
				//跳过
				continue;
			}
			//有可能相交，则求直线v1v2与射线所在直线的交点的x坐标
			float x1=v1[X];
			float z1=v1[Z];
			float x2=v2[X];
			float z2=v2[Z];
			float x=(cz-z1)*(x2-x1)/(z2-z1)+x1;
			//如果此交点的X坐标>cx，则说明与射线相交
			if(x>cx){//v1v2与射线相交
				if(x<xmin){
					xmin=x;
					polygenID=i;
					edgeID=j;
				}
			}
		}
		
	}//得到xmin,polygenID,edgeID
	if(polygenID==-1){//如果没有交点
		//说明(cx,cz)必定在polygenList外部
		return false;
	}
	//判断与(polygenID,edgeID)的交点是穿入还是穿出
	//求边(polygenID,edgeID)的两个端点
	int nv=(int)polygenList[polygenID].vlist.size();
	float *v1=polygenList[polygenID].vlist[edgeID].v;
	float *v2=polygenList[polygenID].vlist[(edgeID+1)%nv].v;
	//判断射线在v1v2处是穿入还是穿出
	//只需将射线的方向向量与向量v1v2作叉积
	//如果叉积的Y值为负，则为穿入；如果叉积的Y值为正，则为穿出
	bool penetrateOut;
	float ray_e[4]={1,0,0,0};//射线的方向向量
	float v1v2[4];
	sub(v2,v1,v1v2);
	v1v2[Y]=0;
	float norm[4];
	cross(ray_e,v1v2,norm);
	if(norm[Y]>0){//穿出
		penetrateOut=true;
	}else{
		penetrateOut=false;
	}
	if(penetrateOut){//如果穿出
		//说明(cx,cy)在polygenList内部
		return true;
	}else{//否则说明(cx,cy)在polygenList外部
		return false;
	}

}



inline void calNormfAndNormn(Cpolygen&polygen){
	int nv=(int)polygen.vlist.size();
	for(int k=0;k<nv;k++){
		Cv4Ex&v=polygen.vlist[k];
		float *vf=polygen.vlist[(k-1+nv)%nv].v;//%%%%%%%%%%%%%%%%%%%%%%%%
		float *vn=polygen.vlist[(k+1)%nv].v;
		//求vfv的右法向量
		float vfv[4];
		sub(v.v,vf,vfv);
		float normf[4]={-vfv[Z],0,vfv[X],0};
		normalize(normf);
		//求vvn的右法向量
		float vvn[4];
		sub(vn,v.v,vvn);
		float normn[4]={-vvn[Z],0,vvn[X],0};
		normalize(normn);
		//将normf和normn填充给v
		veccopy(normf,v.normf);
		veccopy(normn,v.normn);	
	}

}

inline int getNearestEdgeOfPolygenList(myvector<Cpolygen>&polygenList,float v[4],int&I,int&J)
//求polygenList中距离点v最近的边，返回(I,J)，I是polygen标号，J是边的标号
//前端返回v到最近边的最短距离在何处取得，返回0表明在垂足取得，返回1表明在边的起点取得，返回2表明在边的终点取得
{
	//找polygenList中距离v最近的边
	I=-1;
	J=-1;
	float mind2=inf;//最近距离的平方
	int where=-1;//最短距离在何处取得
	int npolygen=(int)polygenList.size();
	for(int i=0;i<npolygen;i++){
		Cpolygen&polygen=polygenList[i];
		int nv=(int)polygen.vlist.size();
		for(int j=0;j<nv;j++){
			float*v1=polygen.vlist[j].v;
			float*v2=polygen.vlist[(j+1)%nv].v;
			//求v到直线v1v2的距离(只考虑xz面上的投影)
			//求v1v2的右法向量
			float v1v2_e[4];//v1v2在xz面上的投影的方向向量
			sub(v2,v1,v1v2_e);
			v1v2_e[Y]=0;
			normalize(v1v2_e);
			float norm[4]={-v1v2_e[Z],0,v1v2_e[X],0};
			float v1v[4];
			sub(v,v1,v1v);
			v1v[Y]=0;
			float PND=dot(v1v,norm);//v到直线v1v2的有向距离
			//求v在直线v1v2上的投影点
			//v_proj=v-PND*norm，分解为v_proj=v-tvec,tvec=PND*norm
			float v_proj[4];
			float tvec[4];
			mul(PND,norm,tvec);
			sub(v,tvec,v_proj);
			//判断v_proj是否在v1v2之间
			float v1v_proj[4];
			sub(v_proj,v1,v1v_proj);
			v1v_proj[Y]=0;
			float v_projv2[4];
			sub(v2,v_proj,v_projv2);
			v_projv2[Y]=0;
			if(dot(v1v_proj,v_projv2)>=0){//v_proj在v1v2上
				//v到线段v1v2的距离即fabs(PND)
				float d=fabs(PND);
				float d2=pow2(d);
				if(d2<mind2){
					mind2=d2;
					I=i;
					J=j;
					where=0;
				}
			}else{//v在v1v2外
				//v到线段v1v2的距离的平方为min(d2_vv1,d2_vv2)
				//求xz面上v到v1的距离的平方
				float d2_vv1=pow2(v[X]-v1[X])+pow2(v[Z]-v1[Z]);
				//求xz面上v到v2的距离的平方
				float d2_vv2=pow2(v[X]-v2[X])+pow2(v[Z]-v2[Z]);
				//求v到线段v1v2的距离的平方
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
	}//得到I,J,mind2,where
	return where;
}

inline float PND_p_line_xz(const float _v[4],Cline&line){//在xz平面上求点到直线的有向距离
	float v[4];
	veccopy(_v,v);
	v[Y]=0;
	//求直线的右法向量
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

inline void ball_seeback(int m,//瓣数
				 float cx,float cy,float cz,//球心位置
				 float r,//球半径
				 float campos[4],//相机位置
				 float k=1//Y轴压扁率
				 )//画球体?????????????法向量未单位化
{//r为球半径,n为半球层数,m为瓣数
	int n=m/2;
	glBegin(GL_QUADS);
	for(int i=0;i<=m-1;i++){
		for(int j=0;j<=2*n-1;j++){
			float A=i*2*pi/m;//与x轴夹角（弧度）
			float B=-pi/2+j*pi/(2*n);//与XZ平面夹角（弧度）
			float dA=2*pi/m;
			float dB=pi/(2*n);
			//画四边形(A,B)->(A,B+dB)->(A+dA,B+dB)->(A+dA,B)->(A,B)
			float s,t;//贴图坐标
			//求四个顶点坐标
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
			//求法向量
			float norm[4];
			float v0v1[4],v0v2[4];
			sub(v1,v0,v0v1);
			sub(v2,v0,v0v2);
			cross(v0v1,v0v2,norm);//不将norm单位化了，反正作为天空球也不用有明暗
			//判断看到的是否为背面
			float camposv0[4];//由campos指向v的向量
			sub(v0,campos,camposv0);
			float cosA=dot(camposv0,norm);
			if(cosA<0){//看到的是正面
				continue;//跳过不画
			}
	//		glNormal3fv(norm);//不用给法向量
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



inline	float PND_point_plane(const Cplane&plane,const float point[4]){//是点到平面的有向距离
	const float *norm=plane.norm;
	const float *p=plane.p;
	float ppoint[4];
	sub(point,p,ppoint);
	float PNd=dot(norm,ppoint);
	return PNd;
}

inline int getPointOfIntersect_line_plane(const float p1[4],const float p2[4],const Cplane&plane,float p3[4])
//求p1p2所确定的直线与平面plane的交点p3（注意是直线而非线段）
//前端返回0表示无交点
//返回1表示恰有一个交点
//返回2表示有无穷多交点
//有无穷多交点时p3直接等于p1
{
	//求直线p1p2与平面的交点p3
	const float *norm=plane.norm;
	const float *p=plane.p;
	float v[4];//直线的单位方向向量
	sub(p2,p1,v);
	normalize(v);
	float p1p[4];
	sub(p,p1,p1p);
	//p0到平面的有向距离t
	double a=dot(p1p,norm);
	double b=dot(norm,v);
	if(a==0&&b==0){//无穷多交点
		veccopy(p1,p3);
		return 2;
	}else if(a!=0&&b==0){//无交点
		return 0;
	}else{//恰有一交点
		float t=a/b;
		//求交点p3
		float vt[4];
		mul(t,v,vt);
		add(p1,vt,p3);//得到p3
		return 1;
	}
}

inline float getWhoseFabsIsMax(const myvector<float>&list){//求list中绝对值最大的元素
	float who=list[0];
	int n=(int)list.size();
	for(int i=1;i<n;i++){
		if(fabs(list[i])>fabs(who)){
			who=list[i];
		}
	}//得到who
	return who;
}
inline bool plane_div_polygen(const Cplane&plane,const Cpolygen&polygen,
					   Cpolygen&polygen1,float&side1,
					   Cpolygen&polygen2,float&side2,
					   bool&iscoplane,bool&isdivided)
					   //用plane切割多边形polygen
					   //前端返回是否正常，isdivided指示是否切成了两部分
					   //规定：
					   //如果不正常：
					   //	isdivided=false，其它返回值均无效
					   //如果正常：
					   //	iscoplane==true：isdivided=false，side1=side2=0，polygne1和polygen2无效
					   //	iscoplane==false且isdivided==true：polygen1,polygen2为切成的两个多边形（不保证合理），side1,side2指示polygen1,polygen2在plane哪面（也可能得0）
					   //	iscoplane==false且isdivided==false：polygne1和polygen2无效，side1,side2指示polygen在plane哪面（也可能得0）
{	
	//----求side列表，同时得到iscoplane
	int n=(int)polygen.vlist.size();
	myvector<float> sideList;//记录polygen各顶点位于plane哪边
	iscoplane=true;
	sideList.resize(n);
	for(int i=0;i<=n-1;i++){
		const float *v=polygen.vlist[i].v;
		float side=PND_point_plane(plane,v);
		sideList[i]=side;
		if(side!=0)iscoplane=false;//有一个不在平面上就为false
	}//得到sideList和iscoplane
	if(iscoplane){
		side1=side2=0;
		isdivided=false;
		return true;//正常
	}
	//----求plane与polygen边界的交点
	float ID[2];//两个交点的编号，如果交点正好是多边形顶点，则ID为顶点编号，如果交点在边形边上，如在多边形顶点i和i+1之间，则ID为i+0.5
	float p[2][4];//记录ID对应的顶点的坐标
	int top=0;//ID栈栈顶指针,同时也是p栈栈顶指针,也是交点个数
	//求ID,top
	//先求与顶点的交点
	for(i=0;i<=n-1;i++){
		if(top==2)break;//够2个即可
		if(sideList[i]==0){//又发现交点
			ID[top]=i;
			const float *v=polygen.vlist[i].v;
			veccopy(v,p[top]);
			top++;
		}
	}
	//求与边的交点
	for(i=0;i<=n-1;i++){
		if(top==2)break;//够2个即可
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
	}//得到ID,top
	//至此交点数<=2
	if(top==0||top==1){//如果交点数<2
		//说明整个polygen在plane的一边
		//求在哪边，只须看最远点在哪边
		float side=getWhoseFabsIsMax(sideList);
		side1=side2=side;
		isdivided=false;
		return true;//正常
	}
	//至此交点个数==2
	if(ID[0]==(int)ID[0]&&ID[1]==(int)ID[1]&&(((int)ID[1]+1)%n==ID[0]||ID[0]+1==ID[1])){//plane与polygen某边重合
		//说明整个polygen在plane的一边
		//求在哪边，只须看最远点在哪边
		float side=getWhoseFabsIsMax(sideList);
		side1=side2=side;
		isdivided=false;
		return true;//正常
	}
	//----根据ID[0],ID[1]制作分割后的两个多边形，并计算onPside1和onPside2的值
	//首先为两个交点排序，即使ID[0]<=ID[1]
	if(ID[0]>ID[1]){
		int temp=ID[0];
		ID[0]=ID[1];
		ID[1]=temp;
	}
	//制作polygen1并计算side1
	myvector<float> sideList1;//polygen1的side列表
	for(i=0;i<=ID[0];i++){
		polygen1.addvert((float*)polygen.vlist[i].v);
		sideList1.push_back(sideList[i]);
	}
	if(ID[0]!=(int)ID[0]){//如果ID[0]不是整值
		//插入顶点
		polygen1.addvert(p[0]);
	}
	if(ID[1]!=(int)ID[1]){//如果ID[1]不是整值
		//插入顶点
		polygen1.addvert(p[1]);
	}
	for(i=ceil(ID[1]);i<=n-1;i++){
		polygen1.addvert((float*)polygen.vlist[i].v);
		sideList1.push_back(sideList[i]);
	}//得到polygen1和sideList1
	//求side1
	side1=getWhoseFabsIsMax(sideList1);
	//制作polygen2并计算side2
	myvector<float> sideList2;//polygen2的side列表
	if(ID[0]!=(int)ID[0]){//如果ID[0]不是整值
		//插入顶点
		polygen2.addvert(p[0]);
	}
	for(i=ceil(ID[0]);i<=floor(ID[1]);i++){
		polygen2.addvert((float*)polygen.vlist[i].v);
		sideList2.push_back(sideList[i]);
	}
	if(ID[1]!=(int)ID[1]){//如果ID[1]不是整值
		polygen2.addvert(p[1]);
		
	}//得到polygen2和sideList2
	//求side2
	side2=getWhoseFabsIsMax(sideList2);
	//对side1,side2进行判断
	//求side1符号
	int sgn1;//side1符号
	if(side1==0){
		sgn1=0;
	}else if(side1>0){
		sgn1=1;
	}else{
		sgn1=-1;
	}
	//求side2符号
	int sgn2;//side2符号
	if(side2==0){
		sgn2=0;
	}else if(side2>0){
		sgn2=1;
	}else{
		sgn2=-1;
	}
	if(sgn1==sgn2){//放弃切割
		isdivided=false;
		return true;//正常
	}
	//接受切割(但不保证side1和side2均非0)
	isdivided=true;
	return true;//正常
	
}

inline	bool getNormalOfTri(const float p0[4],const float p1[4],const float p2[4],float norm[4]){//获得三角面的法向量，前端返回是否构成三角形
		float p01[4],p02[4];
		sub(p1,p0,p01);
		sub(p2,p0,p02);
		cross(p01,p02,norm);
		if(normalize(norm)==0){//如果nrom模为0，说明三点共线
			return false;
		}
		return true;
	}


inline void drawWall_polygenList_polygenList(myvector<Cpolygen>&polygenList,myvector<Cpolygen>&polygenList_dn,GLuint texture,int stepLen=1,bool enableTexture=true)
//输入polygenList和polygenList_dn,画墙壁面
//stepLen指明几个点取一个点。默认是1，即不跳过点
{
	if(stepLen<1){
		cout<<"error! stepLen必须>=1"<<endl;
		return;
	}
	if(enableTexture){
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,texture);
	}
	glBegin(GL_QUADS);
	//根据polygenList和polygenList_dn构造墙壁网格
	int npolygen=(int)polygenList.size();
	for(int i=0;i<npolygen;i++){		
		Cpolygen&polygenUp=polygenList[i];//上底多边形
		Cpolygen&polygenDn=polygenList_dn[i];//下底多边形
		//画各墙壁面
		int nv=(int)polygenDn.vlist.size();
		int j,jn;//j,jn构成当前边
		j=0;
		while(1){
			//求jn
			jn=j+stepLen;
			if(jn<=nv-1){
				//找(j,jn)上第一个isKeyPoint点
				int index=-1;//(j,jn)上第一个isKeyPoint点的索引
				for(int k=j+1;k<jn;k++){
					Cv4Ex&v=polygenDn.vlist[k];
					if(v.isKeyPoint){
						index=k;
						break;
					}
				}
				if(index==-1){//没找到
					//则jn就是正确的jn
					//无动作
				}else{//找到了
					//应以index作为jn
					jn=index;
				}
			}else if(jn>=nv){
				//找(j,nv-1]上第一个isKeyPoint点
				int index=-1;//(j,nv-1]上第一个isKeyPoint点的索引
				for(int k=j+1;k<nv;k++){
					Cv4Ex&v=polygenDn.vlist[k];
					if(v.isKeyPoint){
						index=k;
						break;
					}
				}
				if(index==-1){//没找到
					//则以0作为jn
					jn=0;
				}else{//找到了
					//应以index作为jn
					jn=index;
				}
			}//得到jn
			//以j,jn作墙壁面
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
			//判断整圈墙壁是否作完
			if(jn==0){//如果jn==0，则说明整圈墙壁已作完
				//跳出循环
				break;
			}
			//用jn作为新的j
			j=jn;
			
		}
	}
	glEnd();
	if(enableTexture){
		glDisable(GL_TEXTURE_2D);
	}
}

inline float calH_BilinearAABB(const float vlup[4],const float vldn[4],const float vrup[4],const float vrdn[4],const float _v[4])
//AABB网格又线性插值求高
{
	//通过双线性插值求_v处的高度（假设格子是AABB（轴平行的））
	//  vlup----vrup
	//   |        |
	//   |_v(x,z) |
	//  vldn----vrdn
	float H;
	float x=_v[X];
	float z=_v[Z];
	//求x在vlup-vrup边上的高度值
	float Hup;
	{
		float x1=vlup[X];
		float H1=vlup[Y];
		float x2=vrup[X];
		float H2=vrup[Y];
		Hup=(x2-x)/(x2-x1)*(H1-H2)+H2;
		
	}
	//求x在vldn-vrdn边上的高度值
	float Hdn;
	{
		float x1=vldn[X];
		float H1=vldn[Y];
		float x2=vrdn[X];
		float H2=vrdn[Y];
		Hdn=(x2-x)/(x2-x1)*(H1-H2)+H2;
		
	}
	//求z在Hup-Hdn边上的高度值
	float Hxz;
	{
		float z1=vlup[Z];
		float H1=Hup;
		float z2=vldn[Z];
		float H2=Hdn;
		Hxz=(z2-z)/(z2-z1)*(H1-H2)+H2;
		
	}
	//求z在vlup-vldn边上的高度值
	float Hlt;
	{
		float z1=vlup[Z];
		float H1=vlup[Y];
		float z2=vldn[Z];
		float H2=vldn[Y];
		Hlt=(z2-z)/(z2-z1)*(H1-H2)+H2;
	}
	//求z在vrup-vrdn边上的高度值
	float Hrt;
	{
		float z1=vrup[Z];
		float H1=vrup[Y];
		float z2=vrdn[Z];
		float H2=vrdn[Y];
		Hrt=(z2-z)/(z2-z1)*(H1-H2)+H2;
	}
	//求x在Hlt-Hrt边上的高度值
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
//判断polygen是否为洞
//假设polygen是xz平面上的多边形
{
	//看是否存在一条边，所有其余顶点都在此边的右侧
	int nv=(int)polygen.vlist.size();
	for(int i=0;i<nv;i++){
		const Cv4Ex&v=polygen.vlist[i];
		const Cv4Ex&vn=polygen.vlist[(i+1)%nv];
		//看是polygen上除v和vn之外的所有顶点是否都在vvn的右侧
		bool allAtRight=true;
		for(int j=0;j<nv;j++){
			if(j==i||j==(i+1)%nv)continue;
			const Cv4Ex&_v=polygen.vlist[j];
			//看_v是否在vvn的右侧
			//即看v_v在vvn的右法向量上的投影是否>=0
			//求v_v
			float v_v[4];
			sub(_v.v,v.v,v_v);
			//求vvn的右法向量
			float dir[4];//vvn的方向向量，未单位化
			sub(vn.v,v.v,dir);
			float norm[4]={-dir[Z],0,dir[X],0};//vvn的右法向量
			normalize(norm);
			//看v_v在norm上的投影是否>=0
			float proj=dot(v_v,norm);
			if(proj>=0){//_v在vvn右侧
				//放行
			}else{//_v不在vvn右侧
				allAtRight=false;
				break;
			}
		}//得到allAtRight
		if(allAtRight){
			return true;//只要找到了一个这样的边，就说明是洞
			
		}
	}
	return false;

}

inline bool isOutlinePolygen(const Cpolygen&polygen)
//判断polygen是否为轮廓多边形
//假设polygen是xz平面上的多边形
{
	//看是否存在一条边，所有其余顶点都在此边的左侧
	int nv=(int)polygen.vlist.size();
	for(int i=0;i<nv;i++){
		const Cv4Ex&v=polygen.vlist[i];
		const Cv4Ex&vn=polygen.vlist[(i+1)%nv];
		//看是polygen上除v和vn之外的所有顶点是否都在vvn的左侧
		bool allAtLeft=true;
		for(int j=0;j<nv;j++){
			if(j==i||j==(i+1)%nv)continue;
			const Cv4Ex&_v=polygen.vlist[j];
			//看_v是否在vvn的左侧
			//即看v_v在vvn的右法向量上的投影是否<=0
			//求v_v
			float v_v[4];
			sub(_v.v,v.v,v_v);
			//求vvn的右法向量
			float dir[4];//vvn的方向向量，未单位化
			sub(vn.v,v.v,dir);
			float norm[4]={-dir[Z],0,dir[X],0};//vvn的右法向量
			normalize(norm);
			//看v_v在norm上的投影是否<=0
			float proj=dot(v_v,norm);
			if(proj<=0){//_v在vvn左侧
				//放行
			}else{//_v不在vvn左侧
				allAtLeft=false;
				break;
			}
		}//得到allAtRight
		if(allAtLeft){
			return true;//只要找到了一个这样的边，就说明是轮廓多边形
			
		}
	}
	return false;

}

inline bool isSelfIntersect(const Cpolygen&polygen)
//判断polygen是否自相交
//假设polygen是xz平面上的多边形
{
	
	int nv=(int)polygen.vlist.size();
	for(int i=0;i<nv;i++){
		const Cv4Ex&v=polygen.vlist[i];
		const Cv4Ex&vn=polygen.vlist[(i+1)%nv];
		//看是其余与vvn无公共顶点的边是否有与之相交者
		for(int j=0;j<nv;j++){
			const Cv4Ex&_v=polygen.vlist[j];
			const Cv4Ex&_vn=polygen.vlist[(j+1)%nv];
			if(j!=i&&j!=(i+1)%nv&&(j+1)%nv!=i&&(j+1)%nv!=(i+1)%nv){
				//看_v与_vn是否相交
				bool intersect=LineSegOverLineSeg(v.v[X],v.v[Z],vn.v[X],vn.v[Z],_v.v[X],_v.v[Z],_vn.v[X],_vn.v[Z]);
				if(intersect){
					return true;
				}
				
			}
		
		}
	}
	return false;


}

inline void drawTrianFace(float p[3][4],float u[3][2]){//画顶点为p[0]~p[2]的三角形面,u[i]为相应贴图坐标
		//   p[0]
		//  /   \
		// p[1]-p[2]
		//求法向量
		float norm[4];
		float p01[4],p02[4];
		sub(p[1],p[0],p01);
		sub(p[2],p[0],p02);
		cross(p01,p02,norm);
		normalize(norm);
		//画三角面
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





	




	

