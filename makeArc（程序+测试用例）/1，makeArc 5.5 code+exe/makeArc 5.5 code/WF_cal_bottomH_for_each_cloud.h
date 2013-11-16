//-----------------------------------------------
//计算各云团的bottomH
//-----------------------------------------------
inline void main_cal_bottomH_for_each_cloud(myvector<Ccloud>&cloudList)
{
	//求各屋顶的最低高度，通过cloud.vlist中isOutlinePoint==true的点的平均高度来估算，求得的结果保存在cloud.bottomH中
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			//求cloud.vlist中isOutlinePoint==true的点的平均高度
			float avrgH=0;
			int count=0;
			int nv=(int)cloud.vlist.size();
			for(int j=0;j<nv;j++){
				Cv4&v=cloud.vlist[j];
				if(!v.isOutlinePoint)continue;
				avrgH+=v.v[Y];
				count++;
			}
			avrgH/=count;
			cloud.bottomH=avrgH;

		}
	}
}