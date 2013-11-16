//----------------------------------------------
//去植被点
//----------------------------------------------
inline void main_remove_vegetation_point(myvector<Ccloud>&cloudList)
{
	//对每个cloud进行去植被点
	{
		int ncloud=(int)cloudList.size();
		for(int __i=0;__i<ncloud;__i++){
			Ccloud&cloud=cloudList[__i];
			if(cloud.removed)continue;
			if(cloud.avrgH-cloud.minH>=maxHOfTree)continue;//过高的云团就不用去植被点了，因为树木达不到那种高度
			//生成网格
			CIDgrid IDgrid;//索引碰撞网格
			float margin=2;
			IDgrid.makeIDgrid(cloud.xmin-margin,cloud.xmax+margin,cloud.zmin-margin,cloud.zmax+margin,//范围
				0.5,0.5,//步长
				cloud.vlist//点云
				);
    		//用扩散法去树木点
			{
				int m=IDgrid.m_IDListmat;
				int n=IDgrid.n_IDListmat;
				for(int i=0;i<m;i++){
					for(int j=0;j<n;j++){
						myvector<int>&IDList=IDgrid.IDListmat[i][j];
						int nvID=(int)IDList.size();
						for(int k=0;k<nvID;k++){
							int ID=IDList[k];
							Cv4&v=cloud.vlist[ID];
							if(!v.isEdgePoint)continue;
							if(v.isTreePoint)continue;
							//以v为中心进行扩散
							float r=1;
							while(1){
								//看在距离v为r个格的范围内阶跃点的百分比
								//如果百分比超过一定阀值，则将此范围内所有阶跃点标为isTreePoint
								int count_edgePoint=0;//距v为r个格的范围内edgePoint的个数
								int count_point=0;//距v为r个格的范围内point的个数
								for(int _i=i-r;_i<=i+r;_i++){
									if(_i<0||_i>=m)continue;
									for(int _j=j-r;_j<=j+r;_j++){
										if(_j<0||_j>=n)continue;
										if(pow2(i-_i)+pow2(j-_j)<=pow2(r)){//如果(_i,_j)到(i,j)的距离小于等于r
											myvector<int>&_IDList=IDgrid.IDListmat[_i][_j];
											int _nvID=(int)_IDList.size();
											for(int _k=0;_k<_nvID;_k++){
												int _ID=_IDList[_k];
												Cv4&_v=cloud.vlist[_ID];
												if(_v.isEdgePoint){
													count_edgePoint++;
												}
												count_point++;
												
											}
											
											
											
										}
									}
									
								}//得到count_edgePoint和count_point
								//计算edgePoint的百比
								float rate=((float)count_edgePoint)/count_point;
								if(rate>=0.85){//百分比够高
									//将r增大1，继续循环
									r++;
									if(r>=10/*25,18*/)break;//防止死循环
								}else{//百分比不够高
									//跳出循环
									break;
								}
							}
							//要据r值判断当前点是的扩散范围是否为一个植被圆
							if(r<=5/*9*/){//太小，认为不是植被圆
								continue;
							}else{//较大，认为是植被圆
								//根据当前r值，将距v距离为r范围内所有的阶跃点标为isTreePoint
								for(int _i=i-r;_i<=i+r;_i++){
									if(_i<0||_i>=m)continue;
									for(int _j=j-r;_j<=j+r;_j++){
										if(_j<0||_j>=n)continue;
										if(pow2(i-_i)+pow2(j-_j)<=pow2(r)){//如果(_i,_j)到(i,j)的距离小于等于r
											myvector<int>&_IDList=IDgrid.IDListmat[_i][_j];
											int _nvID=(int)_IDList.size();
											for(int _k=0;_k<_nvID;_k++){
												int _ID=_IDList[_k];
												Cv4&_v=cloud.vlist[_ID];
												if(_v.isEdgePoint){
													_v.isTreePoint=true;
												}
											}
											
										}
									}
									
								}
								
							}
							
							
						}
						
					}
				}
				
			}
			
			//将cloud.vlist中标记为了isTreePoint=true的点删除
			{
				int count=0;
				int nv=(int)cloud.vlist.size();
				for(int i=0;i<nv;i++){
					Cv4&v=cloud.vlist[i];
					if(v.isTreePoint){
						count++;
					}else{
						if(count!=0)cloud.vlist[i-count]=v;
					}
				}
				cloud.vlist.resize(nv-count);
			}
		}
		
	}
	//删除过小的云团
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			if((int)cloud.vlist.size()<=20){
				cloud.removed=true;
			}
		}
	}





}
