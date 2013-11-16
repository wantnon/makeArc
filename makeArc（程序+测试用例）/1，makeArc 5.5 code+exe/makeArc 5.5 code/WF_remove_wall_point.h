//---------------------------------------------------------------
//去墙壁点
//---------------------------------------------------------------
inline void main_remove_wall_point(myvector<Cv4>&vlist,
							float xmin,float xmax,float ymin,float ymax,float zmin,float zmax)
{
	
	//生成网格索引网格
	CIDgrid IDgrid;//索引网格
	IDgrid.makeIDgrid(xmin,xmax,zmin,zmax,
		0.5,0.5,//步长
		vlist//点云
		);
	//判断vlist中哪些点是墙面点
	{
		int nv=(int)vlist.size();
		for(int u=0;u<nv;u++){
			Cv4&v=vlist[u];
			//求得v的邻近点的索引列表（由于v也是vlist中的点，所以包含v自身）
			myvector<int> IDList;
			IDgrid.getIDList_oc(v.v[X],v.v[Z],IDList);//v邻近点索引列表
			//求v在其中的下标
			int indexOfv=-1;
			for(int i=0;i<(int)IDList.size();i++){
				int ID=IDList[i];
				if(ID==u){
					indexOfv=i;
					break;
				}
			}//得到indexOfv
			//在IDList中寻找到v的距离小于ref_d的点，存到列表nearvList中(nearvList中不包含v)
			const float ref_d=0.2;
			myvector<Cv4> nearvList;
			int nvID=(int)IDList.size();
			for(i=0;i<nvID;i++){
				if(i==indexOfv)continue;
				Cv4&_v=vlist[IDList[i]];
				//计算_v到v的距离
				float d2=pow2(v.v[X]-_v.v[X])+pow2(v.v[Z]-_v.v[Z]);
				if(d2>0&&d2<=ref_d*ref_d){//如果_v到v的距离较小
					nearvList.push_back(_v);
				}
			}
			//根据nearvList判断v是否为墙壁点
			//求nearvList中最高点和最低点的高度
			float hmax=-inf;
			float hmin=inf;
			int nv=(int)nearvList.size();
			for(i=0;i<nv;i++){
				float *v=nearvList[i].v;
				if(v[Y]>hmax)hmax=v[Y];
				if(v[Y]<hmin)hmin=v[Y];
			}//得到hmax和hmin
			if(v.v[Y]>=hmax){//如果v比最高点不低
				//v不视作墙面点
			}else{
				//求将v和nearvList的并的最高和最低高度之差
				if(v.v[Y]<hmin)hmin=v.v[Y];
				float dh=hmax-hmin;
				if(dh>=0.25){//认为v是墙壁点
					v.wallpoint=true;
				}
			}
			
			
		}
	}
	//删除墙壁点
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