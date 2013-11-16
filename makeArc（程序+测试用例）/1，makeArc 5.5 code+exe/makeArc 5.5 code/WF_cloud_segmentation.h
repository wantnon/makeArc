//---------------------------------------------------
//���Ʒָ�
//---------------------------------------------------
inline void cloudcncTest(Cv4&,int,int,
					myvector<Cv4>&,
					myvector<myvector<myvector<int> > >&,
					myvector<Cv4>&);

inline void main_cloud_segmentation(myvector<Cv4>&vlist,
							 float xmin,float xmax,float ymin,float ymax,float zmin,float zmax,
							 myvector<Ccloud>&cloudList,bool ignoreRidTree_baseOnStepMark)
//�����������ư�������ͨ�Էָ�Ϊ���ţ�˳�����ȥ������Ϊ��ľ������
{
	//������������
	CIDgrid IDgrid;
	IDgrid.makeIDgrid(xmin,xmax,zmin,zmax,//��Χ
		5,5,//����
		vlist//����
		);
	//���е��Ʒָ�
	{
		int m=IDgrid.m_IDListmat;
		int n=IDgrid.n_IDListmat;
		for(int i=0;i<m;i++){
			for(int j=0;j<n;j++){
				int nv=(int)IDgrid.IDListmat[i][j].size();
				for(int k=0;k<nv;k++){
					Cv4&v=vlist[IDgrid.IDListmat[i][j][k]];
					if(v.added)continue;
					//����cloud������v��������
					cloudList.grow();
					int n=(int)cloudList.size();
					cloudList[n-1].vlist.push_back(v);//���뵽cloud�ĵ�addedһ��Ҫ����Ϊfalse
					v.added=true;
					//��vΪ��������ͨ��̽�⣬����̽�⵽�ĵ㶼���뵱ǰcloud
					cloudcncTest(v,i,j,cloudList[n-1].vlist,IDgrid.IDListmat,vlist);
					//����������ɵ����Ź�С�������
					if((int)cloudList[n-1].vlist.size()<=20){
						cloudList.pop_back();
					}else{//�������Ļ��������Ƿ�Ϊ�����ж�
						if(ignoreRidTree_baseOnStepMark){
							//��������˻��ڽ�Ծ��ȥ������˲�����
						}else{
							//-----------��һ��ȥ��
							Ccloud&_cloud=cloudList[n-1];//�������ɵ�����
							//����������ɵ�����isEdgePoint�ĵ�������ߣ������
							//���������isEdgePoint�ĵ�ı���
							int count_edgePoint=0;
							int _nv=(int)_cloud.vlist.size();
							for(int u=0;u<_nv;u++){
								Cv4&_v=_cloud.vlist[u];
								if(_v.isEdgePoint){
									count_edgePoint++;
								}
							}//�õ�count_edgePoint
							float rate=((float)count_edgePoint)/_nv;
							if(rate>=0.6){
								cloudList.pop_back();
							}
						}
					}

				}
			}
		}//�õ�cloudList
	}
	//��vlist�����е��added�û�Ϊfalse������Ϊ�˱�֤vlist��������ʱ�������仯
	{
		int nv=(int)vlist.size();
		for(int i=0;i<nv;i++){
			vlist[i].added=false;
		}
	}
	//������ŵķ�Χ
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			//Ϊcloud��������
			//��cloud��x,z��Χ
			float margin=1;
			float _xmin,_xmax,_zmin,_zmax;
			float _ymax;
			_xmin=inf;
			_xmax=-inf;
			_zmin=inf;
			_zmax=-inf;
			_ymax=-inf;
			int nv=(int)cloud.vlist.size();
			for(int j=0;j<nv;j++){
				float x=cloud.vlist[j].v[X];
				float z=cloud.vlist[j].v[Z];
				float y=cloud.vlist[j].v[Y];
				if(x<_xmin)_xmin=x;
				if(x>_xmax)_xmax=x;
				if(z<_zmin)_zmin=z;
				if(z>_zmax)_zmax=z;
				if(y>_ymax)_ymax=y;
			}//�õ�_xmin,_xmax,_zmin,_zmax,_ymin,_ymax
			_xmin-=margin;
			_xmax+=margin;
			_zmin-=margin;
			_zmax+=margin;
			_ymax+=margin;
			//
			cloud.xmin=_xmin;
			cloud.xmax=_xmax;
			cloud.zmin=_zmin;
			cloud.zmax=_zmax;
			cloud.topH=_ymax;
		}	
	}
	//Ϊÿ������������ɫ
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			init4(cloud.color,rand()%10*0.1,rand()%10*0.1,rand()%10*0.1,1);
		}
	}

}


inline void cloudcncTest(Cv4&v,int I,int J,//v��IDListmat��(I,J)��
					myvector<Cv4>&cloud,
					myvector<myvector<myvector<int> > >&IDListmat,
					myvector<Cv4>&_vlist)
//������ͨ���ԣ�����IDListmat��ʹ��������ƣ�
//��vΪ��������ͨ���ԣ����õ�����ͨ��֧�浽cloud
{
	int m_IDListmat=(int)IDListmat.size();
	int n_IDListmat=(int)IDListmat[0].size();
	//����v��
	//IDListmat[I-1][J-1]  IDListmat[I-1][J]  IDListmat[I-1][J+1]
	//IDListmat[I][J-1]    IDListmat[I][J]    IDListmat[I][J+1]
	//IDListmat[I+1][J-1]  IDListmat[I+1][J]  IDListmat[I+1][J+1]
	//������δ�����κ����ŵĵ�ľ����ƽ��
	int _I,_J;//_Iȡ����I-1,I,I+1��_Jȡ��J-1,J,J+1
	for(_I=I-1;_I<=I+1;_I++){
		if(_I<0||_I>m_IDListmat-1)continue;
		for(_J=J-1;_J<=J+1;_J++){
			if(_J<0||_J>n_IDListmat-1)continue;
			//����v��IDListmat[_I][_J]�����е�ľ����ƽ��
			int nv=(int)IDListmat[_I][_J].size();
			for(int i=0;i<nv;i++){
				Cv4&_v=_vlist[IDListmat[_I][_J][i]];
				if(_v.added)continue;
				const float k=5;
				float d2=pow2(v.v[X]-_v.v[X])+pow2(v.v[Z]-_v.v[Z])+pow2(k*(v.v[Y]-_v.v[Y]));
				if(d2>0&&d2<=2.5*2.5){//d2<x*x����xȡ�ù������ʹ�÷ֿ��ķֲ�������xȡ�ù�С����ʹ���÷ֿ��ķֿ�
					                      //�������ڵ��������ʹ���������Ƶľۼ��̶Ȳ��������Ե��¶�x���У���������������ã��ܼ����򲻻����Щ����
					//��Ϊ_v��v�ڽ�
					//��_v����뵽cloud
					cloud.push_back(_v);//���뵽cloud�еĵ�addedһ��Ҫ����Ϊfalse
					//��_v���Ϊadded
					_v.added=true;
					//��_v���¼���̽��
					cloudcncTest(_v,_I,_J,cloud,IDListmat,_vlist);
				
				}
			}
			
		}
	}
	

}
