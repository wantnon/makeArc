//-----------------------------------------------
//��������ŵ�bottomH
//-----------------------------------------------
inline void main_cal_bottomH_for_each_cloud(myvector<Ccloud>&cloudList)
{
	//����ݶ�����͸߶ȣ�ͨ��cloud.vlist��isOutlinePoint==true�ĵ��ƽ���߶������㣬��õĽ��������cloud.bottomH��
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			//��cloud.vlist��isOutlinePoint==true�ĵ��ƽ���߶�
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