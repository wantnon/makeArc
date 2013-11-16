//--------------------------------------
//Ϊ�����ż�����С��Χ����
//--------------------------------------
inline void calMinRect(myvector<Cv4>&,Crect&);

inline void main_cal_minRect_for_each_cloud(myvector<Ccloud>&cloudList)
{
	//������ŵ���С��Χ����
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			myvector<Cv4> vlist;//��cloud.IDoutline.polygenList�����е���뵽vlist
			int npolygen=(int)cloud.polygenList.size();
			for(int j=0;j<npolygen;j++){
				Cpolygen&polygen=cloud.polygenList[j];
				//��polygen�е����е���뵽vlist
				int nv=(int)polygen.vlist.size();
				for(int k=0;k<nv;k++){
					Cv4Ex&v=polygen.vlist[k];
					vlist.push_back(v);
				}
			}//�õ�vlist
			calMinRect(vlist,cloud.minRect);

		}

	}


}

inline void calMinRect(myvector<Cv4>&vlist,Crect&rect)
//����ת������Ƶĵ���С��Χ����
{
	myvector<Crect> rectList;//���������ת���õ�rect���
	myvector<float> sList;//����б�
	int nv=(int)vlist.size();
	//��vlist����
	float c[4]={0,0,0,0};//polygen����
	for(int i=0;i<nv;i++){
		float*v=vlist[i].v;
		add(c,v,c);
	}
	c[X]/=nv;
	c[Y]=0;
	c[Z]/=nv;
	c[W]=1;//�õ�c	
	veccopy(c,rect.origin);
	//��תdA�ȣ������ת90��
	float dA=2*pi/180;
	float A=0;
	float halfpi=pi/2;
	for(A=0;A<=halfpi;A+=dA){
		//��xdir��zdir
		init4(rect.xdir,cos(A),0,sin(A),0);
		init4(rect.zdir,-sin(A),0,cos(A),0);
		//��vlist�и�����c-xdir-zdir����ϵ�µ����꣬�浽_vlist
		myvector<Cv4>_vlist;
		_vlist.resize(nv);
		for(i=0;i<nv;i++){
			float*v=vlist[i].v;
			Cv4 _v;//��v��c-xdir-zdirϵ�µ�����
			float cv[4];
			sub(v,c,cv);
			_v.v[X]=dot(cv,rect.xdir);
			_v.v[Y]=0;
			_v.v[Z]=dot(cv,rect.zdir);
			_v.v[W]=1;
			_vlist[i]=_v;
		}//�õ�_vlist
		//��_vlist�����귶Χ
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
		}//�õ�rect.xmin,rect.xmax,rect.zmin,rect.zmax
	
		float s=(rect.xmax-rect.xmin)*(rect.zmax-rect.zmin);
		rectList.push_back(rect);
		sList.push_back(s);
	}//�õ�rectList��sList
	//��sList�е���Сֵ
	int nRect=rectList.size();
	float minS=inf;
	int minI=-1;
	for(i=0;i<nRect;i++){
		if(sList[i]<minS){
			minS=sList[i];
			minI=i;
		}
	}//�õ�minS��minI
	//��rectList[minI]����rect��Ϊ���
	rect=rectList[minI];//�õ�rect

}