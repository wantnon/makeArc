//---------------------------------------------------
//点云分割
//---------------------------------------------------
inline void cloudcncTest(Cv4&,int,int,
					myvector<Cv4>&,
					myvector<myvector<myvector<int> > >&,
					myvector<Cv4>&);

inline void main_cloud_segmentation(myvector<Cv4>&vlist,
							 float xmin,float xmax,float ymin,float ymax,float zmin,float zmax,
							 myvector<Ccloud>&cloudList,bool ignoreRidTree_baseOnStepMark)
//操作：将点云按距离连通性分割为云团，顺便粗略去掉疑似为树木的云团
{
	//生成索引网格
	CIDgrid IDgrid;
	IDgrid.makeIDgrid(xmin,xmax,zmin,zmax,//范围
		5,5,//步长
		vlist//点云
		);
	//进行点云分割
	{
		int m=IDgrid.m_IDListmat;
		int n=IDgrid.n_IDListmat;
		for(int i=0;i<m;i++){
			for(int j=0;j<n;j++){
				int nv=(int)IDgrid.IDListmat[i][j].size();
				for(int k=0;k<nv;k++){
					Cv4&v=vlist[IDgrid.IDListmat[i][j][k]];
					if(v.added)continue;
					//新增cloud，并将v加入其中
					cloudList.grow();
					int n=(int)cloudList.size();
					cloudList[n-1].vlist.push_back(v);//加入到cloud的点added一定要保持为false
					v.added=true;
					//以v为起点进行连通性探测，所有探测到的点都加入当前cloud
					cloudcncTest(v,i,j,cloudList[n-1].vlist,IDgrid.IDListmat,vlist);
					//如果本次生成的云团过小，则放弃
					if((int)cloudList[n-1].vlist.size()<=20){
						cloudList.pop_back();
					}else{//不放弃的话，再做是否为树的判断
						if(ignoreRidTree_baseOnStepMark){
							//如果忽略了基于阶跃的去树，则此步跳过
						}else{
							//-----------插一步去树
							Ccloud&_cloud=cloudList[n-1];//本次生成的云团
							//如果本次生成的云团isEdgePoint的点比例过高，则放弃
							//求此云团中isEdgePoint的点的比例
							int count_edgePoint=0;
							int _nv=(int)_cloud.vlist.size();
							for(int u=0;u<_nv;u++){
								Cv4&_v=_cloud.vlist[u];
								if(_v.isEdgePoint){
									count_edgePoint++;
								}
							}//得到count_edgePoint
							float rate=((float)count_edgePoint)/_nv;
							if(rate>=0.6){
								cloudList.pop_back();
							}
						}
					}

				}
			}
		}//得到cloudList
	}
	//将vlist中所有点的added置回为false，这是为了保证vlist输入和输出时不发生变化
	{
		int nv=(int)vlist.size();
		for(int i=0;i<nv;i++){
			vlist[i].added=false;
		}
	}
	//求各云团的范围
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			//为cloud生成网格
			//求cloud的x,z范围
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
			}//得到_xmin,_xmax,_zmin,_zmax,_ymin,_ymax
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
	//为每个云团生成颜色
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			init4(cloud.color,rand()%10*0.1,rand()%10*0.1,rand()%10*0.1,1);
		}
	}

}


inline void cloudcncTest(Cv4&v,int I,int J,//v在IDListmat的(I,J)格
					myvector<Cv4>&cloud,
					myvector<myvector<myvector<int> > >&IDListmat,
					myvector<Cv4>&_vlist)
//点云连通测试（基于IDListmat，使用网格机制）
//以v为起点进行连通测试，将得到的连通分支存到cloud
{
	int m_IDListmat=(int)IDListmat.size();
	int n_IDListmat=(int)IDListmat[0].size();
	//计算v到
	//IDListmat[I-1][J-1]  IDListmat[I-1][J]  IDListmat[I-1][J+1]
	//IDListmat[I][J-1]    IDListmat[I][J]    IDListmat[I][J+1]
	//IDListmat[I+1][J-1]  IDListmat[I+1][J]  IDListmat[I+1][J+1]
	//中所有未加入任何云团的点的距离的平方
	int _I,_J;//_I取遍历I-1,I,I+1；_J取遍J-1,J,J+1
	for(_I=I-1;_I<=I+1;_I++){
		if(_I<0||_I>m_IDListmat-1)continue;
		for(_J=J-1;_J<=J+1;_J++){
			if(_J<0||_J>n_IDListmat-1)continue;
			//计算v到IDListmat[_I][_J]中所有点的距离的平方
			int nv=(int)IDListmat[_I][_J].size();
			for(int i=0;i<nv;i++){
				Cv4&_v=_vlist[IDListmat[_I][_J][i]];
				if(_v.added)continue;
				const float k=5;
				float d2=pow2(v.v[X]-_v.v[X])+pow2(v.v[Z]-_v.v[Z])+pow2(k*(v.v[Y]-_v.v[Y]));
				if(d2>0&&d2<=2.5*2.5){//d2<x*x中若x取得过大，则会使该分开的分不开，若x取得过小，则使不该分开的分开
					                      //但是由于点云质量差，使得相连点云的聚集程度不够，所以导致对x敏感，而如果点云质量好（密集）则不会出现些问题
					//认为_v与v邻接
					//将_v点加入到cloud
					cloud.push_back(_v);//加入到cloud中的点added一定要保持为false
					//将_v标记为added
					_v.added=true;
					//由_v向下继续探测
					cloudcncTest(_v,_I,_J,cloud,IDListmat,_vlist);
				
				}
			}
			
		}
	}
	

}
