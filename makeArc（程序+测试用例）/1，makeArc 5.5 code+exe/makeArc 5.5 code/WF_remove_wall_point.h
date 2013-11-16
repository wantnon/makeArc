//---------------------------------------------------------------
//ȥǽ�ڵ�
//---------------------------------------------------------------
inline void main_remove_wall_point(myvector<Cv4>&vlist,
							float xmin,float xmax,float ymin,float ymax,float zmin,float zmax)
{
	
	//����������������
	CIDgrid IDgrid;//��������
	IDgrid.makeIDgrid(xmin,xmax,zmin,zmax,
		0.5,0.5,//����
		vlist//����
		);
	//�ж�vlist����Щ����ǽ���
	{
		int nv=(int)vlist.size();
		for(int u=0;u<nv;u++){
			Cv4&v=vlist[u];
			//���v���ڽ���������б�����vҲ��vlist�еĵ㣬���԰���v����
			myvector<int> IDList;
			IDgrid.getIDList_oc(v.v[X],v.v[Z],IDList);//v�ڽ��������б�
			//��v�����е��±�
			int indexOfv=-1;
			for(int i=0;i<(int)IDList.size();i++){
				int ID=IDList[i];
				if(ID==u){
					indexOfv=i;
					break;
				}
			}//�õ�indexOfv
			//��IDList��Ѱ�ҵ�v�ľ���С��ref_d�ĵ㣬�浽�б�nearvList��(nearvList�в�����v)
			const float ref_d=0.2;
			myvector<Cv4> nearvList;
			int nvID=(int)IDList.size();
			for(i=0;i<nvID;i++){
				if(i==indexOfv)continue;
				Cv4&_v=vlist[IDList[i]];
				//����_v��v�ľ���
				float d2=pow2(v.v[X]-_v.v[X])+pow2(v.v[Z]-_v.v[Z]);
				if(d2>0&&d2<=ref_d*ref_d){//���_v��v�ľ����С
					nearvList.push_back(_v);
				}
			}
			//����nearvList�ж�v�Ƿ�Ϊǽ�ڵ�
			//��nearvList����ߵ����͵�ĸ߶�
			float hmax=-inf;
			float hmin=inf;
			int nv=(int)nearvList.size();
			for(i=0;i<nv;i++){
				float *v=nearvList[i].v;
				if(v[Y]>hmax)hmax=v[Y];
				if(v[Y]<hmin)hmin=v[Y];
			}//�õ�hmax��hmin
			if(v.v[Y]>=hmax){//���v����ߵ㲻��
				//v������ǽ���
			}else{
				//��v��nearvList�Ĳ�����ߺ���͸߶�֮��
				if(v.v[Y]<hmin)hmin=v.v[Y];
				float dh=hmax-hmin;
				if(dh>=0.25){//��Ϊv��ǽ�ڵ�
					v.wallpoint=true;
				}
			}
			
			
		}
	}
	//ɾ��ǽ�ڵ�
	{
		int count=0;
		int nv=(int)vlist.size();
		for(int i=0;i<nv;i++){
			Cv4&v=vlist[i];
			if(v.wallpoint){
				count++;
			}else{
				if(count!=0)vlist[i-count]=v;
			}
		}
		vlist.resize(nv-count);
	}
	
	
	
	
}