//----------------------------------------
//��ȡ����
//----------------------------------------
inline void getLooseOutline(Cpatch&,CIDoutline&);
inline void IDloosePolygenToIDpolygen(CIDloosePolygen&,myvector<CIDpolygen>&);
inline void getLooseOutline_inn(CIDTriangle*,CIDloosePolygen&);

inline void main_outline_extraction(myvector<Ccloud>&cloudList)
{


	//����Ϊ�Ż��ڴ棬���д��������
	{
		int ncloud=(int)cloudList.size();
		for(int i_c=0;i_c<ncloud;i_c++){
			Ccloud&cloud=cloudList[i_c];
			if(cloud.removed)continue;
			Cpatch patch;
			CIDoutline IDoutline;
			//Ϊÿ�����Ž��������ʷ֣��õ��Ľ����ΪpatchList��һ��Ԫ��
			{
				//��cloudList[i]���������ʷ�
				//�����ʷֹ����в������볬�������ζ��㣬��Ϊ��Delaunay����֮���Ѿ�����ȥ�����������μ���������������
				Delaunay(cloud.vlist,cloud.xmin,cloud.xmax,cloud.zmin,cloud.zmax,patch.pIDtriList);
			}

				
			//ɾ��patch���й����ıߵ�������
			{
				for(int j=0;j<(int)patch.pIDtriList.size();j++){
					CIDTriangle*pIDtri=patch.pIDtriList[j];
					if(!pIDtri->removed){
						//���pIDtri���й����ıߣ���pIDtri��Ϊremoved
						float *v[3];
						v[0]=cloud.vlist[pIDtri->vID[0]].v;
						v[1]=cloud.vlist[pIDtri->vID[1]].v;
						v[2]=cloud.vlist[pIDtri->vID[2]].v;
						for(int k=0;k<=2;k++){
							//�����k���ߵĳ��ȵ�ƽ��
							int _k=(k+1)%3;
							float len2=pow2(v[k][X]-v[_k][X])+pow2(v[k][Z]-v[_k][Z]);
							if(len2>3*3){//3*3
								pIDtri->removed=true;
								break;
							}
						}
					}
					
				}
			}
	
			//�����Ϊremoved��������ɾ��
			//����ɾ��ǰ���뱣֤����δremoved�������ζ�������ָ��removed�������ε�ָ�루��������Ұָ�룩
			//�����Ҫ��ɾ��֮ǰ�ȱ���һ�θ������Σ������ǵ�ָ��removed�������ε�ָ���ΪNULL
			{
				//��patch��δremoved�������ε�ָ��removed��������ָ����ΪNULL
				int nIDtri=(int)patch.pIDtriList.size();
				for(int j=0;j<nIDtri;j++){
					CIDTriangle*pIDtri=patch.pIDtriList[j];
					if(pIDtri->removed)continue;
					//pIDtri��δremoved�������Σ�������������ھ�ָ�룬���Ƿ���ָ��removed=true�������εģ�����У�������ΪNULL
					if(pIDtri->p[0]!=NULL&&pIDtri->p[0]->removed)pIDtri->p[0]=NULL;
					if(pIDtri->p[1]!=NULL&&pIDtri->p[1]->removed)pIDtri->p[1]=NULL;
					if(pIDtri->p[2]!=NULL&&pIDtri->p[2]->removed)pIDtri->p[2]=NULL;
					
				}
				//��ʼɾ��removed������
				//ɾ��patch�б��Ϊremoved�������Σ�ͬʱ��ָ���б������������������Ա�����ģ�ڴ��ƶ�����ֱ����eraseЧ�ʸߣ�
				int count=0;//ͳ��removed�������θ���ͬʱҲ��Ϊ������������ǰ����ƫ����
				for(j=0;j<nIDtri;j++){
					CIDTriangle*pIDtri=patch.pIDtriList[j];
					//�����ǰ��������removed�ģ���delete����������
					//�����ǰ�������Ƿ�removed�ģ�����ǰ���
					if(pIDtri->removed){//��removed
						//delete������
						delete pIDtri;
						count++;
					}else{//δremoved
						//��ǰ���
						if(count!=0)patch.pIDtriList[j-count]=pIDtri;
					}
				}//��ʱremoved��������ɾ����ϣ���removed����������ǰ��䣨������Ҳ���
				//����ֻ�轫pIDtriList�к����һ������ָ��ɾ��
				//��ָ��ĸ������ǵ���removed==true�����εĸ����������ֵĿǰ�Ѵ浽��count��
				//��pIDtriList�е�ĩβcount��Ԫ��ɾ���������䳤������ΪnIDtri-count
				patch.pIDtriList.resize(nIDtri-count);
			}
		/*	//Ϊÿ������������ɫ
			{
				init4(cloud.color,rand()%10*0.1,rand()%10*0.1,rand()%10*0.1,1);
			}
		*/	
			//��patch��ȡ��ɢ����
			{
				getLooseOutline(patch,IDoutline);
			}
			//��ÿ��IDoutline��IDloosePolygenList������IDPolygenList��˳��ɾ���˰������������ٵ�IDpolygen�����ұ�֤ÿ��IDpolygen�ж��㻥�죩
			{
				int nIDloosePolygen=(int)IDoutline.IDloosePolygenList.size();
				for(int j=0;j<nIDloosePolygen;j++){
					CIDloosePolygen&IDloosePolygen=IDoutline.IDloosePolygenList[j];
					IDloosePolygenToIDpolygen(IDloosePolygen,IDoutline.IDpolygenList);
			
				}
			}
	
			//��IDpolygenת��Ϊpolygen����Y���궼��Ϊ0��
			{
				int nIDpolygen=(int)IDoutline.IDpolygenList.size();
				IDoutline.polygenList.resize(nIDpolygen);
				
				for(int j=0;j<nIDpolygen;j++){
					CIDpolygen&IDpolygen=IDoutline.IDpolygenList[j];
					Cpolygen&polygen=IDoutline.polygenList[j];
					
					
					//��IDpolygen�еĽǵ�������ӵ�polygen��
					int nvID=(int)IDpolygen.vIDList.size();
					for(int k=0;k<nvID;k++){
						Cv4 v=cloud.vlist[IDpolygen.vIDList[k]];
						v.v[Y]=0;
						Cv4Ex vEx;
						veccopy(v.v,vEx.v);
						polygen.vlist.push_back(vEx);
						//��cloud.vlist�д˵���ΪisOutlinePoint
						cloud.vlist[IDpolygen.vIDList[k]].isOutlinePoint=true;
						
					}
					
				}
			
				//���cloud��polygenListΪ�գ������Ϊremoved����֤���е�cloud��polygenList���ǿգ�
			
				if((int)IDoutline.polygenList.size()==0){
					cloud.removed=true;
					continue;
				}
				//��IDoutline.polygenList������cloud.polygenList
				cloud.polygenList=IDoutline.polygenList;
			}
			

		}
	
	}


}


inline void getLooseOutline(Cpatch&patch,CIDoutline&IDoutline)
//��ȡpatch����ɢ��������䵽IDoutline��IDloosePolygenList��
{
	//����patch�ĸ�����Σ�ÿ�ҵ�һ����removed�ҷ�visited2�ģ����������µݹ�
	int nIDtri=(int)patch.pIDtriList.size();
	for(int i=0;i<nIDtri;i++){
		CIDTriangle*pIDtri=patch.pIDtriList[i];
		if(!pIDtri->removed&&!pIDtri->visited2){
			//��patch�İ���pIDtri����ͨ��֧����ɢ���������
			IDoutline.IDloosePolygenList.grow();
			int n=(int)IDoutline.IDloosePolygenList.size();
			getLooseOutline_inn(pIDtri,IDoutline.IDloosePolygenList[n-1]);
			
		}
	}
}

inline void IDloosePolygenToIDpolygen(CIDloosePolygen&IDloosePolygen,myvector<CIDpolygen>&IDpolygenList)
//����IDloosePolygen��Ӧ��IDpolygen�������ж�����������Ǽ��뵽IDplygenList�У�˳����ɾ���������������ٵ�IDpolygen�������IDpolygenList��ֻ��һ��IDpolygen,��ɾ����
//���⻹Ҫ��֤ÿ��IDpolygen�в������ظ����㣨�������ŵ㣩

{
	int nIDedge=(int)IDloosePolygen.IDedgeList.size();
	if(nIDedge==0)return;
	//���IDloosePolygen��Ϊ��
	//������IDloosePolygen����ȡ�����
	while(1){
		//��IDloosePolygen����һ��δadded��δremoved�ı���Ϊ��ʼ��
		CIDedge*pIDedge=NULL;
		for(int i=0;i<nIDedge;i++){
			CIDedge&IDedge=IDloosePolygen.IDedgeList[i];
			if(!IDedge.added){
				pIDedge=&IDedge;
				break;
			}
		}//�õ�pIDedge
		if(pIDedge==NULL){//˵���Ѳ�����δadded��δremoved�ı�
			//��ȡ����
			break;
		}
		//���濪ʼ��ȡ����Σ���������б�����һ��Ԫ��
		IDpolygenList.grow();
		CIDpolygen&IDpolygen=IDpolygenList[(int)IDpolygenList.size()-1];//��ǰҪ��ȡ�Ķ����
		//��pIDedge��Ϊ��ʼ��
		int vID1=pIDedge->ID1;
		int vID2=pIDedge->ID2;
		IDpolygen.vIDList.push_back(vID1);
		IDpolygen.vIDList.push_back(vID2);
		pIDedge->added=true;
		int curLastvID=vID2;
		//��������ʣ����ߣ����ĸ��ߵ��������curLastvID����
		while(curLastvID!=vID1){
			bool find=false;
			for(int i=0;i<nIDedge;i++){
				CIDedge&IDedge=IDloosePolygen.IDedgeList[i];
				if(IDedge.added)continue;//�����Ѽ���ı�
				if(IDedge.ID1==curLastvID){
					//��IDedge���յ���뵽IDpolygen
					IDpolygen.vIDList.push_back(IDedge.ID2);
					IDedge.added=true;//���Ϊ�Ѽ���
					curLastvID=IDedge.ID2;
					find=true;
					break;
				}	
			}
			if(!find){//���û���ҵ�����curLastvID���ϵı�
				cout<<"error IDloosePolygenToIDpolygen"<<endl; 
				//��vID1����IDpolygen��Ȼ������ѭ��
				//��Ϊ������Ҫɾ�����һ��Ԫ�أ��������ｫvID1���룬��Ϊ�˵���ɾ��
				IDpolygen.vIDList.push_back(vID1);
				break;


			}
		}//�õ�IDpolygen,���������յ��ظ���
		IDpolygen.vIDList.pop_back();//�õ�IDpolygen
	

	}

	

	//ȥ��IDpolygenList�к��н��ٶ�������IDpolygen
	for(int i=0;i<(int)IDpolygenList.size();i++){
		CIDpolygen&IDpolygen=IDpolygenList[i];
		if((int)IDpolygen.vIDList.size()<15){//ֻ��ȡ15,ȡ30�������ɾ
			IDpolygenList.erase(&IDpolygenList[i]);
			i--;
		}
	}
	
}


inline void getLooseOutline_inn(CIDTriangle*pIDtri,CIDloosePolygen&IDloosePolygen)
//�����pIDtriһ����ΪNULL,Ҳһ����Ϊvisited2
{	
	pIDtri->visited2=true;
	//��pIDtri�ĸ��ڽ�ָ�룬���ĳ���ڽ�ָ��Ϊ�ջ���ָ���������Ϊremoved���򽫴˱߼��뵽IDloosePolygen
	for(int i=0;i<=2;i++){
		CIDTriangle*pAdjIDtri=pIDtri->p[i];
		//����pIDtri��i���ھ�pAdjIDtri
		if(pAdjIDtri==NULL||(pAdjIDtri!=NULL&&pAdjIDtri->removed)){//˵��pIDtri��i�ű��Ǳ߽��
			//��pIDtri�ĵ�i�ű߼��뵽IDloosePolygen
			CIDedge IDedge;
			IDedge.setvID(pIDtri->vID[i],pIDtri->vID[(i+1)%3]);
			IDloosePolygen.IDedgeList.push_back(IDedge);
	
		}else{//���pIDtri��i�ű߲��Ǳ߽��
			if(pAdjIDtri->visited2){//�������������visited2
				continue;
			}else{//�������������δvisited2
				//�ɴ��������������µݹ�
				getLooseOutline_inn(pAdjIDtri,IDloosePolygen);
			}

		
		}
	}
}
