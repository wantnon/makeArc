//-------------------------------
//���Ƴ���
//-------------------------------
inline void main_cloud_settlement(myvector<Ccloud>&cloudList)
{
	//�����ư��߶�����
	myvector<float> HList;//�߶�ֵ�б�
	myvector<int> IDList;//�±��б�
	{
		//���IDList
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			IDList.push_back(i);
		}
		//���߶�ֵ�б�
		for(i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed){
				HList.push_back(-inf);
			}else{
				HList.push_back(cloud.avrgH);
			}

		}
		//��HList��������(����)��ͬ���䶯IDList
		for(i=ncloud-1;i>=1;i--){//iָ��Ҫ���̶�������λ��
			for(int j=0;j<i;j++){
				//HList[j]��HList[j+1]�Ƚ�
				if(HList[j]<HList[j+1]){
					//������С������ƣ�
					float tempH=HList[j];
					HList[j]=HList[j+1];
					HList[j+1]=tempH;
					//ͬ������IDList
					int tempID=IDList[j];
					IDList[j]=IDList[j+1];
					IDList[j+1]=tempID;

				}
			}
		}
		
	}

	//Ϊÿ��cloud����������
	myvector<CIDgrid> IDgridList;
	IDgridList.resize((int)cloudList.size());
	{
		
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			CIDgrid&IDgrid=IDgridList[i];
			float margin=6;
			IDgrid.makeIDgrid(cloud.xmin-margin,cloud.xmax+margin,cloud.zmin-margin,cloud.zmax+margin,//��Χ
				5,5,//����
				cloud.vlist//����
				);
		}

	
	}
	//��ÿ��cloud�����������������ǳ��ӽ���cloud���뵽��֮��
	{
		vector<bool> falledList;
		falledList.resize((int)cloudList.size());

		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			int ID=IDList[i];
			Ccloud&cloud=cloudList[ID];
			if(cloud.removed)continue;
			//���������������ţ����߶ȱ�cloud������cloud�зǳ��ӽ��ĵ��cloud���뵽cloud��
			for(int j=0;j<ncloud;j++){
				int _ID=IDList[j];
				Ccloud&_cloud=cloudList[_ID];
				bool&_falled=falledList[_ID];//���������ã���Ϊ�漰��ֵ
				CIDgrid&_IDgrid=IDgridList[_ID];
				if(_cloud.removed)continue;
				if(ID==_ID)continue;
				if(_cloud.avrgH<=cloud.avrgH)break;//��Ϊ���������Կ��Զ����������_cloud.avrgH�͸�С�ˣ�����ֱ������ѭ������
				if(_falled)continue;
				//��_cloud�ķ�Χ��cloud�ķ�Χ�Ƿ���
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

				//��_cloud���Ƿ���ĳ����cloud�е�ĳ��ܽӽ�
				int count=0;
				int nv=(int)cloud.vlist.size();
				for(int k=0;k<nv;k++){
					Cv4&v=cloud.vlist[k];
					//��v���ڵ�_cloud.IDgrid��
					int I,J;//v����_cloud.IDgrid��(I,J)��
					I=(v.v[Z]-_IDgrid.zmin)/_IDgrid.zstepLen;
					J=(v.v[X]-_IDgrid.xmin)/_IDgrid.xstepLen;
					if(I<0||J<0||I>_IDgrid.m_IDListmat-1||J>_IDgrid.n_IDListmat-1)continue;
					//��_IDgrid��I,J�������е�
					myvector<int> IDList=_IDgrid.IDListmat[I][J];//���е��ID�б�
					//��v�����еĵ���о����ж�
					int nvID=(int)IDList.size();
					for(int u=0;u<nvID;u++){
						int ID=IDList[u];
						Cv4&_v=_cloud.vlist[ID];
						float d2=pow2(_v.v[X]-v.v[X])+pow2(_v.v[Z]-v.v[Z]);//���������xzƽ���ϵľ���
						if(d2<1*1){
							count++;	
							if(count>=5)break;
						}
					}
					if(count>=5)break;
				}
				if(count>=5){
					//����
				}else{
					continue;
				}
				//��_cloud�����е����cloud
				int _nv=(int)_cloud.vlist.size();
				for(k=0;k<_nv;k++){
					Cv4 v=_cloud.vlist[k];
					v.v[Y]=cloud.avrgH;
					v.isoutcomer=true;
					cloud.vlist.push_back(v);
				}
				//��_cloud���Ϊfalled=true
				_falled=true;
				//����cloud�ķ�Χ
				cloud.xmin=min(cloud.xmin,_cloud.xmin);
				cloud.xmax=max(cloud.xmax,_cloud.xmax);
				cloud.zmin=min(cloud.zmin,_cloud.zmin);
				cloud.zmax=max(cloud.zmax,_cloud.zmax);

			

			}

		}
	}

}