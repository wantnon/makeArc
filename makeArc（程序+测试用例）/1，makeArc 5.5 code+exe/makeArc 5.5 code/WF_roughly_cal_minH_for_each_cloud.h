//------------------------------------------------------
//���Լ�������ŵ���ظ߶�
//------------------------------------------------------

inline void main_roughly_cal_minH_for_each_cloud(myvector<Ccloud>&cloudList,const Cgroundgrid&groundgrid){
	//������ŵ�xmin,xmax,zmin,zmax���ĵ����ظ߶ȣ���Ϊ���Ե�minH
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			float x=(cloud.xmin+cloud.xmax)/2;
			float z=(cloud.zmin+cloud.zmax)/2;
			//��(x,z)���ĵ���߶���Ϊcloud.minH
			float v[4]={x,0,z,1};
			float H=groundgrid.getH(v);
			cloud.minH=H;

		}
	}



}