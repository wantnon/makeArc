//------------------------------------------------------
//粗略计算各云团的离地高度
//------------------------------------------------------

inline void main_roughly_cal_minH_for_each_cloud(myvector<Ccloud>&cloudList,const Cgroundgrid&groundgrid){
	//求各云团的xmin,xmax,zmin,zmax中心点的离地高度，作为粗略的minH
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			float x=(cloud.xmin+cloud.xmax)/2;
			float z=(cloud.zmin+cloud.zmax)/2;
			//求(x,z)处的地面高度作为cloud.minH
			float v[4]={x,0,z,1};
			float H=groundgrid.getH(v);
			cloud.minH=H;

		}
	}



}