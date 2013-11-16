//-----------------------------------------------
//计算各云团的avrgH
//-----------------------------------------------
inline void main_cal_avrgH_for_each_cloud(myvector<Ccloud>&cloudList)
{
	
	//求各屋沿的平均高度，通过cloud来估算，求得的结果保存在cloud.avrgH中
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			//求cloud中的点的平均高度
			int count=0;
			double avrgH=0;//用double是为防止累加溢出
			int nv=(int)cloud.vlist.size();
			for(int j=0;j<nv;j++){
				Cv4&v=cloud.vlist[j];
				avrgH+=v.v[Y];
				count++;
			}
			avrgH/=count;
			cloud.avrgH=avrgH;


		}
	}

}