//----------------------------------------------
//ȥֲ����
//----------------------------------------------
inline void main_remove_vegetation_point(myvector<Ccloud>&cloudList)
{
	//��ÿ��cloud����ȥֲ����
	{
		int ncloud=(int)cloudList.size();
		for(int __i=0;__i<ncloud;__i++){
			Ccloud&cloud=cloudList[__i];
			if(cloud.removed)continue;
			if(cloud.avrgH-cloud.minH>=maxHOfTree)continue;//���ߵ����žͲ���ȥֲ�����ˣ���Ϊ��ľ�ﲻ�����ָ߶�
			//��������
			CIDgrid IDgrid;//������ײ����
			float margin=2;
			IDgrid.makeIDgrid(cloud.xmin-margin,cloud.xmax+margin,cloud.zmin-margin,cloud.zmax+margin,//��Χ
				0.5,0.5,//����
				cloud.vlist//����
				);
    		//����ɢ��ȥ��ľ��
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
							//��vΪ���Ľ�����ɢ
							float r=1;
							while(1){
								//���ھ���vΪr����ķ�Χ�ڽ�Ծ��İٷֱ�
								//����ٷֱȳ���һ����ֵ���򽫴˷�Χ�����н�Ծ���ΪisTreePoint
								int count_edgePoint=0;//��vΪr����ķ�Χ��edgePoint�ĸ���
								int count_point=0;//��vΪr����ķ�Χ��point�ĸ���
								for(int _i=i-r;_i<=i+r;_i++){
									if(_i<0||_i>=m)continue;
									for(int _j=j-r;_j<=j+r;_j++){
										if(_j<0||_j>=n)continue;
										if(pow2(i-_i)+pow2(j-_j)<=pow2(r)){//���(_i,_j)��(i,j)�ľ���С�ڵ���r
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
									
								}//�õ�count_edgePoint��count_point
								//����edgePoint�İٱ�
								float rate=((float)count_edgePoint)/count_point;
								if(rate>=0.85){//�ٷֱȹ���
									//��r����1������ѭ��
									r++;
									if(r>=10/*25,18*/)break;//��ֹ��ѭ��
								}else{//�ٷֱȲ�����
									//����ѭ��
									break;
								}
							}
							//Ҫ��rֵ�жϵ�ǰ���ǵ���ɢ��Χ�Ƿ�Ϊһ��ֲ��Բ
							if(r<=5/*9*/){//̫С����Ϊ����ֲ��Բ
								continue;
							}else{//�ϴ���Ϊ��ֲ��Բ
								//���ݵ�ǰrֵ������v����Ϊr��Χ�����еĽ�Ծ���ΪisTreePoint
								for(int _i=i-r;_i<=i+r;_i++){
									if(_i<0||_i>=m)continue;
									for(int _j=j-r;_j<=j+r;_j++){
										if(_j<0||_j>=n)continue;
										if(pow2(i-_i)+pow2(j-_j)<=pow2(r)){//���(_i,_j)��(i,j)�ľ���С�ڵ���r
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
			
			//��cloud.vlist�б��Ϊ��isTreePoint=true�ĵ�ɾ��
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
	//ɾ����С������
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
