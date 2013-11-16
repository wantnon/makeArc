//--------------------------------------
//为各云团计算最小包围矩形
//--------------------------------------
inline void calMinRect(myvector<Cv4>&,Crect&);

inline void main_cal_minRect_for_each_cloud(myvector<Ccloud>&cloudList)
{
	//求各云团的最小包围矩形
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			myvector<Cv4> vlist;//将cloud.IDoutline.polygenList中所有点加入到vlist
			int npolygen=(int)cloud.polygenList.size();
			for(int j=0;j<npolygen;j++){
				Cpolygen&polygen=cloud.polygenList[j];
				//将polygen中的所有点加入到vlist
				int nv=(int)polygen.vlist.size();
				for(int k=0;k<nv;k++){
					Cv4Ex&v=polygen.vlist[k];
					vlist.push_back(v);
				}
			}//得到vlist
			calMinRect(vlist,cloud.minRect);

		}

	}


}

inline void calMinRect(myvector<Cv4>&vlist,Crect&rect)
//用旋转法求点云的的最小包围矩形
{
	myvector<Crect> rectList;//保存各次旋转所得的rect结果
	myvector<float> sList;//面积列表
	int nv=(int)vlist.size();
	//求vlist重心
	float c[4]={0,0,0,0};//polygen重心
	for(int i=0;i<nv;i++){
		float*v=vlist[i].v;
		add(c,v,c);
	}
	c[X]/=nv;
	c[Y]=0;
	c[Z]/=nv;
	c[W]=1;//得到c	
	veccopy(c,rect.origin);
	//旋转dA度，最多旋转90度
	float dA=2*pi/180;
	float A=0;
	float halfpi=pi/2;
	for(A=0;A<=halfpi;A+=dA){
		//求xdir和zdir
		init4(rect.xdir,cos(A),0,sin(A),0);
		init4(rect.zdir,-sin(A),0,cos(A),0);
		//求vlist中各点在c-xdir-zdir坐标系下的坐标，存到_vlist
		myvector<Cv4>_vlist;
		_vlist.resize(nv);
		for(i=0;i<nv;i++){
			float*v=vlist[i].v;
			Cv4 _v;//点v在c-xdir-zdir系下的坐标
			float cv[4];
			sub(v,c,cv);
			_v.v[X]=dot(cv,rect.xdir);
			_v.v[Y]=0;
			_v.v[Z]=dot(cv,rect.zdir);
			_v.v[W]=1;
			_vlist[i]=_v;
		}//得到_vlist
		//求_vlist中坐标范围
		rect.xmin=inf;
		rect.xmax=-inf;
		rect.zmin=inf;
		rect.zmax=-inf;
		for(i=0;i<nv;i++){
			float*v=_vlist[i].v;
			if(v[X]<rect.xmin)rect.xmin=v[X];
			if(v[X]>rect.xmax)rect.xmax=v[X];
			if(v[Z]<rect.zmin)rect.zmin=v[Z];
			if(v[Z]>rect.zmax)rect.zmax=v[Z];
		}//得到rect.xmin,rect.xmax,rect.zmin,rect.zmax
	
		float s=(rect.xmax-rect.xmin)*(rect.zmax-rect.zmin);
		rectList.push_back(rect);
		sList.push_back(s);
	}//得到rectList和sList
	//求sList中的最小值
	int nRect=rectList.size();
	float minS=inf;
	int minI=-1;
	for(i=0;i<nRect;i++){
		if(sList[i]<minS){
			minS=sList[i];
			minI=i;
		}
	}//得到minS和minI
	//将rectList[minI]赋予rect作为结果
	rect=rectList[minI];//得到rect

}