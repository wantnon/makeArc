//-------------------------------
//点云沉降
//-------------------------------
inline void main_cloud_settlement(myvector<Ccloud>&cloudList)
{
	//将点云按高度排序
	myvector<float> HList;//高度值列表
	myvector<int> IDList;//下标列表
	{
		//填充IDList
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			IDList.push_back(i);
		}
		//填充高度值列表
		for(i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed){
				HList.push_back(-inf);
			}else{
				HList.push_back(cloud.avrgH);
			}

		}
		//对HList进行排序(降序)，同步变动IDList
		for(i=ncloud-1;i>=1;i--){//i指向将要被固定下来的位置
			for(int j=0;j<i;j++){
				//HList[j]与HList[j+1]比较
				if(HList[j]<HList[j+1]){
					//交换（小者向后移）
					float tempH=HList[j];
					HList[j]=HList[j+1];
					HList[j+1]=tempH;
					//同步交换IDList
					int tempID=IDList[j];
					IDList[j]=IDList[j+1];
					IDList[j+1]=tempID;

				}
			}
		}
		
	}

	//为每个cloud建索引网格
	myvector<CIDgrid> IDgridList;
	IDgridList.resize((int)cloudList.size());
	{
		
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			CIDgrid&IDgrid=IDgridList[i];
			float margin=6;
			IDgrid.makeIDgrid(cloud.xmin-margin,cloud.xmax+margin,cloud.zmin-margin,cloud.zmax+margin,//范围
				5,5,//步长
				cloud.vlist//点云
				);
		}

	
	}
	//对每个cloud，将比它高且与它非常接近的cloud加入到它之中
	{
		vector<bool> falledList;
		falledList.resize((int)cloudList.size());

		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			int ID=IDList[i];
			Ccloud&cloud=cloudList[ID];
			if(cloud.removed)continue;
			//搜索其余所有云团，将高度比cloud高且与cloud有非常接近的点的cloud加入到cloud中
			for(int j=0;j<ncloud;j++){
				int _ID=IDList[j];
				Ccloud&_cloud=cloudList[_ID];
				bool&_falled=falledList[_ID];//必须是引用，因为涉及赋值
				CIDgrid&_IDgrid=IDgridList[_ID];
				if(_cloud.removed)continue;
				if(ID==_ID)continue;
				if(_cloud.avrgH<=cloud.avrgH)break;//因为已排序，所以可以断言再往后的_cloud.avrgH就更小了，所以直接跳出循环即可
				if(_falled)continue;
				//看_cloud的范围与cloud的范围是否搭边
				float cx,cz;
				cx=(cloud.xmin+cloud.xmax)/2;
				cz=(cloud.zmin+cloud.zmax)/2;
				float _cx,_cz;
				_cx=(_cloud.xmin+_cloud.xmax)/2;
				_cz=(_cloud.zmin+_cloud.zmax)/2;
				if(fabs(cx-_cx)<=0.5*(cloud.xmax-cloud.xmin)+0.5*(_cloud.xmax-_cloud.xmin)+1.5
					&&fabs(cz-_cz)<=0.5*(cloud.zmax-cloud.zmin)+0.5*(_cloud.zmax-_cloud.zmin)+1.5){
					
				}else{
					continue;
				}

				//看_cloud中是否有某点与cloud中的某点很接近
				int count=0;
				int nv=(int)cloud.vlist.size();
				for(int k=0;k<nv;k++){
					Cv4&v=cloud.vlist[k];
					//求v所在的_cloud.IDgrid格
					int I,J;//v属于_cloud.IDgrid的(I,J)格
					I=(v.v[Z]-_IDgrid.zmin)/_IDgrid.zstepLen;
					J=(v.v[X]-_IDgrid.xmin)/_IDgrid.xstepLen;
					if(I<0||J<0||I>_IDgrid.m_IDListmat-1||J>_IDgrid.n_IDListmat-1)continue;
					//求_IDgrid的I,J格中所有点
					myvector<int> IDList=_IDgrid.IDListmat[I][J];//格中点的ID列表
					//用v与其中的点进行距离判断
					int nvID=(int)IDList.size();
					for(int u=0;u<nvID;u++){
						int ID=IDList[u];
						Cv4&_v=_cloud.vlist[ID];
						float d2=pow2(_v.v[X]-v.v[X])+pow2(_v.v[Z]-v.v[Z]);//这里必须用xz平面上的距离
						if(d2<1*1){
							count++;	
							if(count>=5)break;
						}
					}
					if(count>=5)break;
				}
				if(count>=5){
					//放行
				}else{
					continue;
				}
				//将_cloud中所有点加入cloud
				int _nv=(int)_cloud.vlist.size();
				for(k=0;k<_nv;k++){
					Cv4 v=_cloud.vlist[k];
					v.v[Y]=cloud.avrgH;
					v.isoutcomer=true;
					cloud.vlist.push_back(v);
				}
				//将_cloud标记为falled=true
				_falled=true;
				//更新cloud的范围
				cloud.xmin=min(cloud.xmin,_cloud.xmin);
				cloud.xmax=max(cloud.xmax,_cloud.xmax);
				cloud.zmin=min(cloud.zmin,_cloud.zmin);
				cloud.zmax=max(cloud.zmax,_cloud.zmax);

			

			}

		}
	}

}