//-------------------------
//��������ŵ�minH,��ɾ�����͵�����
//-------------------------
main_cal_minH_and_rid_cloud_too_low(myvector<Ccloud>&cloudList,const Cgroundgrid&GroundGrid)
{
	//��ǰ���������£������ŵ�cloud.minH
	//��������������û��ȫȷ���������ں��洦���л�����仯�����Դ˲��������cloud.minH��������Ҫ�ٸ���
	{
		//���������������
		float zmin_g=GroundGrid.zmin;
		float xmin_g=GroundGrid.xmin;
		float zstepLen_g=GroundGrid.zstepLen;
		float xstepLen_g=GroundGrid.xstepLen;
		int m_mat_g=GroundGrid.m_mat;
		int n_mat_g=GroundGrid.n_mat;
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
		
			//��cloud.IDoutline.polygenList�еĵ��ڵ��������ж�Ӧ��Yֵ����Сֵ����Ϊcloud.minH��ֵ
			float minH=inf;
			int npolygen=cloud.polygenList.size();
			for(int j=0;j<npolygen;j++){
				Cpolygen&polygen=cloud.polygenList[j];
				int nv=(int)polygen.vlist.size();
				for(int k=0;k<nv;k++){
					Cv4&v=polygen.vlist[k];
					//��v���ĵ���Yֵ
					float vY;
					//��v���ĸ���
					int I,J;//v����(I,J)��
					I=(v.v[Z]-zmin_g)/zstepLen_g;
					J=(v.v[X]-xmin_g)/xstepLen_g;
					if(I<0||I>=GroundGrid.m_mat)continue;
					if(J<0||J>=GroundGrid.n_mat)continue;
					vY=GroundGrid.mat[I][J].h;
					//��minH�Ƚ�
					if(vY<minH){
						minH=vY;
					}
				}
			}//�õ�minH
			cloud.minH=minH;
			if(cloud.avrgH-cloud.minH<=2.5){//����ɾ��
				cloud.removed=true;
			}
			
		}
	}

}