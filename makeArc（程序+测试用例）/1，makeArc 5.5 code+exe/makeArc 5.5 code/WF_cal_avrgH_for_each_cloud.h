//-----------------------------------------------
//��������ŵ�avrgH
//-----------------------------------------------
inline void main_cal_avrgH_for_each_cloud(myvector<Ccloud>&cloudList)
{
	
	//������ص�ƽ���߶ȣ�ͨ��cloud�����㣬��õĽ��������cloud.avrgH��
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			//��cloud�еĵ��ƽ���߶�
			int count=0;
			double avrgH=0;//��double��Ϊ��ֹ�ۼ����
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