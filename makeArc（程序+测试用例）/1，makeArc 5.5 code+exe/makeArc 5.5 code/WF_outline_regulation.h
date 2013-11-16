//--------------------------------------------
//��������
//--------------------------------------------
inline void extractLine(const Cpolygen&,myvector<Cline>&);
inline void cal_mainDir(Ccloud&,float[4]);
inline void regulate_mainDir_simple(myvector<Cline>&,const Cv4&);
inline void pullPointsToLine(myvector<Cline>&);
inline void mergeParallelLines(myvector<Cline>&);
inline void regulate_mainDir(myvector<Cline>&,const Cv4&,const float a,const float b,const float da,const float db);
inline void regulate_mainDir_second(myvector<Cline>&,const Cv4&,const Ccloud&);
inline void deal_neighbour_rev_parallel(myvector<Cline>&);
inline void cal_vertex_via_neighbour_intersect(myvector<Cline>&);
inline void make_polygen_use_lineList(myvector<Cline>&,Cpolygen&);
inline void rid_polygen_nv_less_than_3(myvector<Cpolygen>&);
inline void ridSelfInteractForEachPolygen(myvector<Cpolygen>&);
inline void cal_rightnorm_and_avrgnorm_for_each_vert_of_polygen(Cpolygen&);
inline void insertVertsForPolygen(const Cpolygen&,Cpolygen&);
inline void calxzRangeForPolygenList(const myvector<Cpolygen>&,const float,
						  float&,float&,float&,float&);
inline void find_dominant_cloud_cluster(myvector<Ccloud>&);
inline void ridSelfInteract_polygenList_Ex(myvector<Cpolygen>&);
inline void adjustMainDirRefBigNeighbour(myvector<Ccloud>&);
//-----------------------------------������
inline void main_outline_regulation(myvector<Ccloud>&cloudList,bool doRegulation)
{
	//ע�⣬��ʹdoRegulation==false��Ҳ����������������һ������Ϊ�������ں��潨oobb����ʱҪ�õ���

	//��ֱ��
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			int npolygen=(int)cloud.polygenList.size();
			for(int j=0;j<npolygen;j++){
				Cpolygen&polygen=cloud.polygenList[j];
				myvector<Cline>&lineList=polygen.lineList;
				//ɾ���غϵ����ڵ㣨Ϊ��һ������㷽��������׼����
				{
					polygen.ridSameNeighbour();
					
				}
				//�����ķ�������
				{
					polygen.calDirForEachVert_xz();
				}
				//��ʼ��ȡֱ��
				//�˲����Ż���أ�line�����е�ľ������������ʽ��ã�����ÿ�ζ�����
				{
					extractLine(polygen,lineList);
					
				}
				//�ѵ�����ֱ����
				{
					
					pullPointsToLine(lineList);
					
				}
				
			}
		}
		
		
	}
	
	//��������
	{
		find_dominant_cloud_cluster(cloudList);
	}
	//��������
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			Cv4&mainDir=cloud.mainDir;
			bool&mainDirGot=cloud.mainDirGot;
			//��cloud��������
			Ccloud&_cloud=cloudList[cloud.hostID];
			Cv4&_mainDir=_cloud.mainDir;
			bool&_mainDirGot=_cloud.mainDirGot;
			if(_mainDirGot){//���_cloud.mainDir�ѵõ�����ֱ�ӿ�����cloud
				veccopy(_mainDir.v,mainDir.v);
				mainDirGot=true;
				continue;
			}//������_lineListList����ߵķ�����Ϊcloud��������
			float dir[4];
			cal_mainDir(_cloud,dir);
			//��dir����_mainDir
			veccopy(dir,_mainDir.v);
			_mainDirGot=true;
			//��_mainDir����mainDir
			veccopy(_mainDir.v,mainDir.v);
			mainDirGot=true;
			
			
			
		}
		
		
	}
	//�����ڽ��������µ���������
	{
		//		adjustMainDirRefBigNeighbour(cloudList);
		
	}
	//ɾ����cloud��hostID��
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			cloud.hostID=-1;
		}
		
	}
	if(!doRegulation){
		cout<<"������������"<<endl;
	}else{
		
		//����
		{
			int ncloud=(int)cloudList.size();
			for(int i=0;i<ncloud;i++){
				Ccloud&cloud=cloudList[i];
				Cv4&mainDir=cloud.mainDir;
				if(cloud.removed)continue;
				int npolygen=(int)cloud.polygenList.size();
				for(int j=0;j<npolygen;j++){
					myvector<Cline>&lineList=cloud.polygenList[j].lineList;
					Cpolygen&polygen=cloud.polygenList[j];
					//���������+�ֲ�����
					//��������򻯣��򵥣�
					{
						regulate_mainDir_simple(lineList,mainDir);
						
					}
					//����lineList�и�line,���е����ж��㶼����line��
					{
						pullPointsToLine(lineList);
					}
					//ƽ��ֱ�ߺϲ�
					{
						mergeParallelLines(lineList);
						pullPointsToLine(lineList);
					}
					//��������򻯣����ӣ�
					{
						switch(regulationDegree){
						case RGL_STRONG_WEAK:
							{
								if(cloud.avrgH-cloud.minH>=minHNotRegulationStrong){//���Ÿ�
									//������
									const float a=30;
									const float b=12;
									const float da=3.5;
									const float db=10;
									regulate_mainDir(lineList,mainDir,a,b,da,db);
								}else{//���ŵ�
									//ǿ����
									const float a=45;
									const float b=12;//20;
									const float da=3.5;
									const float db=10;
									regulate_mainDir(lineList,mainDir,a,b,da,db);
								}
								
							}
							break;
						case RGL_STRONG:
							{
								const float a=45;
								const float b=20;
								const float da=3.5;
								const float db=10;
								regulate_mainDir(lineList,mainDir,a,b,da,db);
								
							}
							break;
						case RGL_WEAK:
							{
								float a=30;
								float b=12;
								float da=3.5;
								float db=10;
								regulate_mainDir(lineList,mainDir,a,b,da,db);
							}
							break;
						default:
							cout<<"error! ��Ч��regulationDegreeֵ!"<<endl;
							
						}
						
					}
					//�ѵ�����ֱ����
					{
						
						pullPointsToLine(lineList);
						
					}
					//ƽ��ֱ�ߺϲ�
					{
						mergeParallelLines(lineList);
						//mergeParallelLines����������̸�һ��pullPointsToLine����������
						pullPointsToLine(lineList);
					}
					//��������ֱ�߷���ƽ�е����
					{
						deal_neighbour_rev_parallel(lineList);
					}
					//������������ƽ�л�ֱ�ı�������������ƽ�л�ֱ
					switch(regulationDegree){
					case RGL_STRONG_WEAK:
						{
							//�ж����Ÿ߶ȣ������Ƿ����regulate_mainDir_second
							if(cloud.avrgH-cloud.minH>=minHNotRegulationStrong){//���Ÿ�
								//������regulate_mainDir_second
							}else{//���ŵ�
								//����regulate_mainDir_second
								regulate_mainDir_second(lineList,mainDir,cloud);
								pullPointsToLine(lineList);	
							}
						}
						break;
					case RGL_STRONG:
						{
							regulate_mainDir_second(lineList,mainDir,cloud);
							pullPointsToLine(lineList);
						}
						break;
					case RGL_WEAK:
						break;
					default:
						cout<<"error! ��Ч��regulationDegreeֵ!"<<endl;
						
					}
					//ƽ��ֱ�ߺϲ�
					{
						mergeParallelLines(lineList);
						pullPointsToLine(lineList);
						
					}
					//ͨ������ֱ���󽻵õ�����
					{
						cal_vertex_via_neighbour_intersect(lineList);
					}
					
				}
			}
			
			
		}
		
		
		
		
		
		//��lineList��������polygen
		{
			int ncloud=(int)cloudList.size();
			for(int i=0;i<ncloud;i++){
				Ccloud&cloud=cloudList[i];
				if(cloud.removed)continue;
				int npolygen=(int)cloud.polygenList.size();
				for(int j=0;j<npolygen;j++){
					Cpolygen&polygen=cloud.polygenList[j];
					myvector<Cline>&lineList=polygen.lineList;
					//��polygen����
					polygen.vlist.resize(0);
					//����lineList��������polygen
					make_polygen_use_lineList(lineList,polygen);
					
				}
			}
			
		}
		//����β�ֵ
		{
			int ncloud=(int)cloudList.size();
			for(int i=0;i<ncloud;i++){
				Ccloud&cloud=cloudList[i];
				if(cloud.removed)continue;
				myvector<Cpolygen>&polygenList=cloud.polygenList;
				//�Ը�polygen��ֵʹ����ָ�Ϊ�ȼ��
				{
					int npolygen=(int)polygenList.size();
					for(int i=0;i<npolygen;i++){
						Cpolygen&polygen=polygenList[i];
						Cpolygen _polygen;
						insertVertsForPolygen(polygen,_polygen);
						polygen=_polygen;
					}
				}
			}
			
		}
	}
	//ȥ��polygen�е��غϵ�
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			int npolygen=(int)cloud.polygenList.size();
			for(int j=0;j<npolygen;j++){
				Cpolygen&polygen=cloud.polygenList[j];
				int nv=(int)polygen.vlist.size();
				for(int k=0;k<nv;k++){
					Cv4Ex&v=polygen.vlist[k];
					Cv4Ex&vn=polygen.vlist[(k+1)%nv];
					float Dmh=fabs(v.v[X]-vn.v[X])+fabs(v.v[Y]-vn.v[Y])+fabs(v.v[Z]-vn.v[Z]);
					if(Dmh<0.01){
						//ɾ��vn
						polygen.vlist.erase(&polygen.vlist[(k+1)%nv]);
						nv--;
						k--;
					
					}

				}
				
				
			}
		}

	}
    //���ν��кϷ���
	//����κϷ����е�ȥ���ཻ����Բ�ֵ��Ķ���ν���
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			myvector<Cpolygen>&polygenList=cloud.polygenList;
			//��polygenList�ж���������3��polygenȥ������һ���ǳ���Ҫ!!ʹ��̲����ܰ�ȫ�ؽ��У�
			{
				rid_polygen_nv_less_than_3(polygenList);
			}
			//Ϊ��polygenȥ���ཻ
			{
				ridSelfInteractForEachPolygen(polygenList);
			}
			//ΪpolygenList�ڵ�polygen֮��ȥ���ཻ
			{
				ridSelfInteract_polygenList_Ex(polygenList);
			}
			//�ٴν�polygenList�ж���������3��polygenȥ��
			{
				rid_polygen_nv_less_than_3(polygenList);
			}
			//���ཻ���
			{
				int npolygen=(int)cloud.polygenList.size();
				for(int j=0;j<npolygen;j++){
					Cpolygen&polygen=cloud.polygenList[j];
					if(isSelfIntersect(polygen)){
						cloud.polygenList.erase(&cloud.polygenList[j]);
						npolygen--;
						j--;
					}
				}


			}
		
		}

	}
	//ȥ��polygen�е��غϵ�
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			int npolygen=(int)cloud.polygenList.size();
			for(int j=0;j<npolygen;j++){
				Cpolygen&polygen=cloud.polygenList[j];
				int nv=(int)polygen.vlist.size();
				for(int k=0;k<nv;k++){
					Cv4Ex&v=polygen.vlist[k];
					Cv4Ex&vn=polygen.vlist[(k+1)%nv];
					float Dmh=fabs(v.v[X]-vn.v[X])+fabs(v.v[Y]-vn.v[Y])+fabs(v.v[Z]-vn.v[Z]);
					if(Dmh<0.01){
						//ɾ��vn
						polygen.vlist.erase(&polygen.vlist[(k+1)%nv]);
						nv--;
						k--;
					
					}

				}
				
				
			}
		}

	}
	//���isKeyPoint��������ںϷ���֮����Ϊ��ʱ����η��Ŷ��ͣ�
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			int npolygen=(int)cloud.polygenList.size();
			for(int j=0;j<npolygen;j++){
				Cpolygen&polygen=cloud.polygenList[j];
				//����㷽������
				{
					polygen.calDirForEachVert_xz();
				}
				//��Ǹ����isKeyPoint��
				{
					//���ĳ���ǰһ����ķ��������뱾�㷽��������ƽ�У�����ΪisKeyPoint��
					int nv=(int)polygen.vlist.size();
					for(int k=0;k<nv;k++){
						Cv4Ex&v=polygen.vlist[k];
						Cv4Ex&vf=polygen.vlist[(k-1+nv)%nv];
						float crossRs[4];
						cross(vf.dir,v.dir,crossRs);
						//���crossRs[Y]�ǳ��ӽ�0����˵��vf.dir��v.dirƽ�У������
						if(fabs(crossRs[Y])<0.001){//vf.dir��v.dirƽ��
							//�����
						}else{//vf.dir��v.dir��ƽ��
							//���
							v.isKeyPoint=true;
						}
					}
				}
			}
		}

	
	}
	//����������Լ��Ժ��ж����������ε�polygenList���з���
	//****ע�⣺�˹�����������****
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			myvector<Cpolygen>&polygenList=cloud.polygenList;
			//��polygenList�ж���������3��polygenȥ������һ���ǳ���Ҫ!!ʹ��̲����ܰ�ȫ�ؽ��У�
			{
				rid_polygen_nv_less_than_3(polygenList);
			}
			//Ϊ��polygenȥ���ཻ
			{
				ridSelfInteractForEachPolygen(polygenList);
			}
			//ΪpolygenList�ڵ�polygen֮��ȥ���ཻ
			{
				ridSelfInteract_polygenList_Ex(polygenList);
			}
			//�ٴν�polygenList�ж���������3��polygenȥ��
			{
				rid_polygen_nv_less_than_3(polygenList);
			}
			//���ཻ���
			{
				int npolygen=(int)cloud.polygenList.size();
				for(int j=0;j<npolygen;j++){
					Cpolygen&polygen=cloud.polygenList[j];
					if(isSelfIntersect(polygen)){
						cloud.polygenList.erase(&cloud.polygenList[j]);
						npolygen--;
						j--;
					}
				}


			}
			//-----------------------------------------------------------------------------------------------
			//��Ƕ������
			{
				int npolygen=(int)cloud.polygenList.size();
				for(int j=0;j<npolygen;j++){
					Cpolygen&polygen=cloud.polygenList[j];
					polygen.isHole=isHolePolygen(polygen);
				}
			}
			//ɾ��cloud.polygenList�����н�С�Ķ������
			{
				int npolygen=(int)cloud.polygenList.size();
				for(int j=0;j<npolygen;j++){
					Cpolygen&polygen=cloud.polygenList[j];
					if(!polygen.isHole)continue;//ֻ����isHoleΪtrue��
					//��polygen�ܳ�
					float L=0;
					{
						int nv=(int)polygen.vlist.size();
						for(int k=0;k<nv;k++){
							Cv4Ex&v=polygen.vlist[k];
							Cv4Ex&vn=polygen.vlist[(k+1)%nv];
							float dL=CarmSqrt(pow2(v.v[X]-vn.v[X])+pow2(v.v[Z]-vn.v[Z]));
							L+=dL;
						}//�õ�L
					}
					if(L<20){
						cloud.polygenList.erase(&cloud.polygenList[j]);
		//				cout<<"--"<<endl;
						j--;
						npolygen--;
					}
				}
				
			}
			//������������
			{
				int npolygen=(int)cloud.polygenList.size();
				for(int j=0;j<npolygen;j++){
					Cpolygen&polygen=cloud.polygenList[j];
					polygen.isOutline=isOutlinePolygen(polygen);
				}
			}
			
			
			
			//���polygenList��������������������Σ������ж��������������ó���������һ��cloud
			{
				//ͳ��polygenList����������θ���
				int count=0;//polygenList����������θ���
				{
					int npolygen=(int)cloud.polygenList.size();
					for(int j=0;j<npolygen;j++){
						Cpolygen&polygen=cloud.polygenList[j];
						if(polygen.isOutline)count++;
					}//�õ�count
				}
				if(count==0){
					cloud.removed=true;
					continue;
				}else if(count>1){
					//���ܳ��������������
					float maxL=-inf;
					int I=-1;
					int npolygen=(int)cloud.polygenList.size();
					for(int j=0;j<npolygen;j++){
						Cpolygen&polygen=cloud.polygenList[j];
						if(!polygen.isOutline)continue;//ֻ�������������
						//��polygen�ܳ�
						float L=0;
						{
							int nv=(int)polygen.vlist.size();
							for(int k=0;k<nv;k++){
								Cv4Ex&v=polygen.vlist[k];
								Cv4Ex&vn=polygen.vlist[(k+1)%nv];
								float dL=CarmSqrt(pow2(v.v[X]-vn.v[X])+pow2(v.v[Z]-vn.v[Z]));
								L+=dL;
							}//�õ�L
						}
						if(L>maxL){
							maxL=L;
							I=j;
						}
					}//�õ�maxL��I
					//��polygenList�г�I�������������α��ΪtakeOut
					{	
						int npolygen=(int)cloud.polygenList.size();
						for(int j=0;j<npolygen;j++){
							Cpolygen&polygen=cloud.polygenList[j];
							if(j!=I&&polygen.isOutline)polygen.takeOut=true;
						}
					}
					//�����ΪtakeOut��polygen��������polygenList���뵽cloudList
					{
						int npolygen=(int)cloud.polygenList.size();
						for(int j=0;j<npolygen;j++){
							Cpolygen&polygen=cloud.polygenList[j];
							if(polygen.takeOut){
						//		cout<<"---"<<endl;
								cloudList.grow();
								Ccloud&cloud_add=cloudList[(int)cloudList.size()-1];
								cloud_add=cloud;
								cloud_add.takenOut=true;
								cloud_add.polygenList.resize(0);
								cloud_add.polygenList_smoothRes.resize(0);
								cloud_add.polygenList.push_back(polygen);
								cloud_add.polygenList[0].takeOut=false;
								calxzRangeForPolygenList(cloud_add.polygenList,2,
								  cloud_add.xmin,cloud_add.xmax,cloud_add.zmin,cloud_add.zmax);
								//ɾ��polygen
								cloud.polygenList.erase(&cloud.polygenList[j]);
								j--;
								npolygen--;

							}
						}
						
						
					}
					
				
				}

			}
		}

	}

	//����������¼���cloud�ķ�Χ
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			float xmin,xmax,zmin,zmax;
			calxzRangeForPolygenList(cloud.polygenList,2,
						  xmin,xmax,zmin,zmax);
			//��ԭ���ķ�Χ�Ƚϣ�Ҫ��֤����С�����뱣֤����С���������
			if(xmax>cloud.xmax)cloud.xmax=xmax;
			if(zmin<cloud.zmin)cloud.zmin=zmin;
			if(zmax>cloud.zmax)cloud.zmax=zmax;

		}

	}
	//���¼���polygen������ǰ����ƽ��������
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			for(int j=0;j<(int)cloud.polygenList.size();j++){
				Cpolygen&polygen=cloud.polygenList[j];
				cal_rightnorm_and_avrgnorm_for_each_vert_of_polygen(polygen);
			}
		}

	}
	//Ϊÿ��polygen����һ����ɫ
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){	
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			int npolygen=(int)cloud.polygenList.size();
			for(int j=0;j<npolygen;j++){
				init4(cloud.polygenList[j].color,rand()%10*0.1,rand()%10*0.1,rand()%10*0.1,1);
			}
		}


	}
	//ɾ����polygen��lineList
	int ncloud=(int)cloudList.size();
	for(int i=0;i<ncloud;i++){	
		Ccloud&cloud=cloudList[i];
		if(cloud.removed)continue;
		int npolygen=(int)cloud.polygenList.size();
		for(int j=0;j<npolygen;j++){
			Cpolygen&polygen=cloud.polygenList[j];
			polygen.clearLineList();
		}
	}

}







//--------------------------------------��������


inline void extractLine(const Cpolygen&polygen,myvector<Cline>&lineList){
	
	//��polygen��ȡֱ��
	int nv=(int)polygen.vlist.size();
	int k=0;//��ǰ���ӵ�
	while(1){
		if(k>=nv)break;
		const Cv4Ex&v=polygen.vlist[k];
		//��vΪ���ӵ���ȡһ��ֱ��
		float sumDir[4]={0,0,0,0};//���������ۼ�ֵ
		float sumV[4]={0,0,0,0};//�����ۼ�ֵ
		//��v����
		add(sumDir,v.dir,sumDir);
		sumDir[Y]=0;
		add(sumV,v.v,sumV);
		sumV[Y]=0;
		//���γ��Խ�k����ĵ���뵽ֱ��
		for(int u=k+1;u<nv;u++){
			const Cv4Ex&_v=polygen.vlist[u];
			//��_v�Ƿ���Լ��뵽ֱ��
			//��_v������sumDir��sumV
			float _sumDir[4];
			add(sumDir,_v.dir,_sumDir);
			_sumDir[Y]=0;
			float _sumV[4];
			add(sumV,_v.v,_sumV);
			_sumV[Y]=0;
			//�������ƽ������������ƽ��λ������
			int count=u-k+1;//����u���Ѽ�����ܵ���
			float _avrgDir[4]={_sumDir[X],0,_sumDir[Z],0};
			normalize(_avrgDir);
			float _avrgV[4]={_sumV[X]/count,0,_sumV[Z]/count,1};
			//��ʱֱ��
			Cline _line;
			_line.makeLine(_avrgV,_avrgDir);
			//�����м���㵽ֱ�߾���ľ�����
			float avrgD2=0;
			for(int t=k;t<=u;t++){
				const	Cv4Ex&__v=polygen.vlist[t];
				float PND=PND_p_line_xz(__v.v,_line);
				avrgD2+=fabs(PND*PND*PND*PND);
			}
			avrgD2/=count;
			const float threshold_d=0.15;

			if(avrgD2>=threshold_d*threshold_d*threshold_d*threshold_d){
				//ֱ�߽���
				break;
			}else{
				//���˵���������
				add(sumDir,_v.dir,sumDir);
				sumDir[Y]=0;
				add(sumV,_v.v,sumV);
				sumV[Y]=0;
			}
		}//�õ�ֱ��
		//���ɴ�ֱ��
		Cline line;//xz���ϵ�ֱ��
		int count=(u-1)-k+1;
		float avrgDir[4]={sumDir[X],0,sumDir[Z],0};
		normalize(avrgDir);
		float avrgV[4]={sumV[X]/count,0,sumV[Z]/count,1};
		line.makeLine(avrgV,avrgDir);
		const		float *vbeg=polygen.vlist[k].v;

		const		float *vend=(u-1==k)?polygen.vlist[(k+1)%nv].v:polygen.vlist[u-1].v;//����vbeg��vend��ͬ�������ͬ��������һ������Ϊvend
		veccopy(vbeg,line.vbeg);
		veccopy(vend,line.vend);
		//��line����polygen.lineList
		lineList.push_back(line);
		//������һ��ֱ����ȡ
		k=u;
	}//��polygen��ֱ����ȡ���	

}

inline void cal_mainDir(Ccloud&_cloud,float mainDir[4])
//��_cloud��������
{
	
	int maxpolygenID=-1;//������ߵĶ���ε�ID
	int maxlineID=-1;//֧�ֳ�����ıߵ�ID
	float maxd=-inf;//֧�ֳ���
	int npolygen=_cloud.polygenList.size();
	for(int j=0;j<npolygen;j++){
		const myvector<Cline>&_lineList=_cloud.polygenList[j].lineList;
		int nline=(int)_lineList.size();
		for(int k=0;k<nline;k++){
			const Cline&line=_lineList[k];
			//��line��֧�ֳ���
			//��������line�ӽ�ƽ�л�ֱ��ֱ�ߵĳ��Ⱥ�
			float d=0;//֧�ֳ���
			for(int u=0;u<nline;u++){
				const Cline&_line=_lineList[u];
				//��_line��line�Ƿ�ӽ�ƽ�л�ֱ
				float cosA=dot(line.dir,_line.dir);
			   if(cosA>=0.996||cosA<=-0.996||(cosA<=0.087&&cosA>=-0.087)){//A<=5||A>=175||(A>=85&&A<=95)
			//	if(cosA>=0.9986||cosA<=-0.9986||(cosA<=0.0523&&cosA>=-0.0523)){//A<=3||A>=177||(A>=87&&A<=93)
					//	if(cosA>=0.99939||cosA<=-0.99939||(cosA<=0.034899&&cosA>=-0.034899)){//A<=2||A>=178||(A>=88&&A<=92)
					//ƽ�л�ֱ
					//��_line����
					float _d=sqrt(_line.getD2_xz());
					d+=_d;
				}
			}//�õ�d
			if(d>maxd){
				maxd=d;
				maxlineID=k;
				maxpolygenID=j;
			}
		}
	}//�õ�maxpolygenID,maxlineID��maxd
	if(maxpolygenID==-1){
		cout<<"warning! 1"<<endl;
		_cloud.isRed=true;
		maxlineID=0;
		maxpolygenID=0;
	}
	//֧�ֳ�����ı�
	const Cline&maxline=_cloud.polygenList[maxpolygenID].lineList[maxlineID];//�������
	//��maxline��֧�ֱ߷�������ȡƽ��ֵ�õ����յ�֧�ַ���
	//���Ҵ�ƽ��ֵΪ��Ȩƽ��ֵ��ȨֵΪ���߳���
	float dir[4]={0,0,0,0};//����֧��dir��ƽ����Ȩ��������
	for(j=0;j<npolygen;j++){
		const myvector<Cline>&_lineList=_cloud.polygenList[j].lineList;
		int nline=(int)_lineList.size();
		for(int k=0;k<nline;k++){
			const Cline&line=_lineList[k];
			//��line�Ƿ�֧��maxline
			float cosA=dot(line.dir,maxline.dir);
			if(cosA>=0.996||cosA<=-0.996||(cosA<=0.087&&cosA>=-0.087)){//A<=5||A>=175||(A>=85&&A<=95)
	//		if(cosA>=0.9986||cosA<=-0.9986||(cosA<=0.0523&&cosA>=-0.0523)){//A<=3||A>=177||(A>=87&&A<=93)
				//		if(cosA>=0.99939||cosA<=-0.99939||(cosA<=0.034899&&cosA>=-0.034899)){//A<=2||A>=178||(A>=88&&A<=92)
				//line֧��maxline
				//��line����
				float _d=sqrt(line.getD2_xz());
				//��lineתΪ��maxline����ͬ��ƽ��
				float _dir[4];//lineתΪ��maxline����ͬ��ƽ�к�ķ�������
				veccopy(line.dir,_dir);
				if(cosA>=0.996){//��������ͬ��ƽ��
					//_dir���ñ�
				}else if(cosA<=-0.996){//����ƽ��
					//��_dir����
					mul(-1,_dir,_dir);
				}else if(cosA<=0.087&&cosA>=-0.087){//��ֱ
					//��_dir������Լ���ֱ����
					init4(_dir,-_dir[Z],_dir[Y],_dir[X],0);
					//��_dir��maxline��ͬ���Ƿ���
					float _cosA=dot(_dir,maxline.dir);
					if(_cosA>=0){//_dir��maxlineͬ��
						//_dir���ñ�
					}else{//_dir��maxline����
						//��_dir����
						mul(-1,_dir,_dir);
					}
				}
				//��_dir��ģ����_d
				mul(_d,_dir,_dir);//��ʱ_dir���Ǵ�Ȩ��������
				//��_dir�ӵ�dir
				add(_dir,dir,dir);
			}
			
		}
	}
	//��dir��λ���õ�dir
	normalize(dir);
	//��dir����mainDir
	veccopy(dir,mainDir);

}
inline void regulate_mainDir_simple(myvector<Cline>&lineList,const Cv4&mainDir){
	//��lineList��ֱ�߽��й���
	int nline=lineList.size();
	for(int k=0;k<nline;k++){
		Cline&line=lineList[k];
		//��˱ߵĳ���ƽ��
		float d2=line.getD2_xz();
	//	if(d2<=5*5){
			
	//	}else
		{//����˱��ǳ���
			//�����������Ϊ�ο�
			float cosA=dot(line.dir,mainDir.v);
			if(cosA>=0.984||cosA<=-0.984){//A<=10||A>=170
				//ƽ��
				veccopy(mainDir.v,line.dir);
				if(cosA<0){
					mul(-1,line.dir,line.dir);
				}
			}
		}
	}
				
}
inline void regulate_mainDir(myvector<Cline>&lineList,const Cv4&mainDir,
							 const float a,const float b,const float da,const float db){
	//��polygen.lineList��ֱ�߽��й���
	int nline=(int)lineList.size();
	//��ʼ���򻯸�ֱ��
	for(int k=0;k<nline;k++){
		Cline&line=lineList[k];
		//�����������Ϊ�ο�
		float d2=line.getD2_xz();
		//����line���Ȳ�ͬ�����򻯲��Բ�ͬ
		//���line���Ƚ϶̣���������ȴ�
		//���line���Ƚϴ����������С
		//ʹ��������ֵ
		float A_threshold;
		if(d2<=da*da){
			A_threshold=a;
		}else if(d2>=db*db){
			A_threshold=b;
		}else{
			float d=sqrt(d2);
			A_threshold=a+(b-a)/(db-da)*(d-da);
		}
		float cosA=dot(line.dir,mainDir.v);
		float cos_A_threshold=cos(A_threshold*pi/180);
		float cos_halfpi_sub_A_threshold=cos((90-A_threshold)*pi/180);
		if(cosA>=cos_A_threshold||cosA<=-cos_A_threshold){//A<=A_threshold||A>=180-A_threshold
			//ƽ��
			veccopy(mainDir.v,line.dir);
			if(cosA<0){
				mul(-1,line.dir,line.dir);
			}
		}else if(-cos_halfpi_sub_A_threshold<=cosA&&cosA<=cos_halfpi_sub_A_threshold){//90+A_threshold>=A>=90-A_threshold
			//��ֱ
			float cross_rs[4];
			cross(mainDir.v,line.dir,cross_rs);
			if(cross_rs[Y]>0){//͹��
				//��������
				float norm[4]={mainDir.v[Z],0,-mainDir.v[X],0};
				//��line.dir��Ϊnorm
				veccopy(norm,line.dir);
			}
			else{//����
				//���ҷ�����
				float norm[4]={-mainDir.v[Z],0,mainDir.v[X],0};
				//��line.dir��Ϊnorm
				veccopy(norm,line.dir);
			}	
		}

		
		
	}
				
	
	
}


inline void pullPointsToLine(myvector<Cline>&lineList){
	int nline=(int)lineList.size();
	for(int k=0;k<nline;k++){
		Cline&line=lineList[k];
		line.pullPointsToLine();
	}

}

inline void mergeParallelLines(myvector<Cline>&lineList){
	//ƽ��ֱ�ߵĺϲ�
	//������ֱ�߽��кϲ�
	int nline=(int)lineList.size();
	int k=0;//��ǰֱ��
	while(1){	
		if(k==nline)break;
		Cline&line=lineList[k];
		Cline&linen=lineList[(k+1)%nline];
		//��line��linen�ܷ�ϲ�
		float*dir=line.dir;
		float*dirn=linen.dir;
		float cosA=dot(dir,dirn);
		if(cosA>=0.984){//A<=10
			//��Ϊƽ��
			//����ֱ�߾����Ƿ��㹻��
			//��line.vID2��linen�ľ�����Ϊ��ֱ�߼�ľ���
			float*v=line.vend;//polygen.vlist[line.vID2].v;
			float PND=PND_p_line_xz(v,linen);
			if(fabs(PND)<=2/*1.5*/){//��line��linen���кϲ�
				Cline tline;//�ݴ�ϲ����
				float line_D2=line.getD2_xz();
				float linen_D2=linen.getD2_xz();
				//��ϲ����p
				//�ýϳ��ߵ�p
				float p[4];
				if(line_D2>=linen_D2){
					veccopy(line.p,p);
				}else{
					veccopy(linen.p,p);
				}
			/*	float p[4];//�ϲ����p
				//p=(line.p*line.D2+linen.p*linen.D2)/(line.D2+linen.D2)
				//�ֽ�Ϊp=vec/K,K=line.D2+linen.D2,vec=vec1+vec2,vec1=line.p*line.D2,vec2=linen.p*linen.D2
				float line_D2=line.getD2_xz();
				float linen_D2=linen.getD2_xz();
				float K=line_D2+linen_D2;
				float vec[4],vec1[4],vec2[4];
				mul(line_D2,line.p,vec1);
				mul(linen_D2,linen.p,vec2);
				add(vec1,vec2,vec);
				p[X]=vec[X]/K;
				p[Y]=0;
				p[Z]=vec[Z]/K;
				p[W]=1;//�õ�p*/
				//��ϲ����dir
				//�ýϳ��ߵ�dir
				float dir[4];
				if(line_D2>=linen_D2){
					veccopy(line.dir,dir);
				}else{
					veccopy(linen.dir,dir);
				}
				//����tline
				tline.makeLine(p,dir);
				veccopy(line.vbeg,tline.vbeg);
				veccopy(linen.vend,tline.vend);
				//��tline�滻line
				line=tline;
				//��linenɾ��
				lineList.erase(&lineList[(k+1)%nline]);
				nline--;
				//k����
				
			}else{
				k++;
			}
			
		}else{
			k++;
		}
	}
				
}

inline void deal_neighbour_rev_parallel(myvector<Cline>&lineList)
//��������ֱ�߷���ƽ�е����
{
	int nline=(int)lineList.size();
	for(int k=0;k<nline;k++){
		
		Cline&line=lineList[k];
		Cline&linen=lineList[(k+1)%nline];
		//���line��linen����ƽ��
		float cosA=dot(line.dir,linen.dir);
		
		if(cosA<=-0.9848){//A>=170
			//����ƽ��
			//��Ϊlinen��line����ƽ��
			//����ֱ�߾����Ƿ��㹻��
			//��line.vID2��linen�ľ�����Ϊ��ֱ�߼�ľ���
			float*v=line.vend;//polygen.vlist[line.vID2].v;
			float PND=PND_p_line_xz(v,linen);
			if(fabs(PND)<=1){//�����㹻��
				//�ж���͹�ǻ��ǰ���
				float cross_rs[4];
				cross(line.dir,linen.dir,cross_rs);
				if(cross_rs[Y]>=0){//͹��
					float d2=line.getD2_xz();
					float d2n=linen.getD2_xz();
					if(d2>=d2n){//line��
						//ɾ��linen
						lineList.erase(&lineList[(k+1)%nline]);
						k--;
						nline--;		
					}else{//linen��
						//ɾ��line
						lineList.erase(&lineList[k]);
						k--;
						nline--;
					}
					
				}else{//����
					//���ǿ��Բ��ܣ���Ϊ������
				}
				
			}else{//�����Զ
				//��line.vend��linen�ϵ�ͶӰv_proj,��linen.vbeg=v_proj
				float v_proj[4];
				line.getProjOfPoint_xz(line.vend,v_proj);
				veccopy(v_proj,linen.vbeg);

			
			}
		}
		
		
		
		
	}
				
}
inline void cal_vertex_via_neighbour_intersect(myvector<Cline>&lineList){
	int nline=(int)lineList.size();
	for(int k=0;k<nline;k++){
		Cline&line=lineList[k];
		Cline&linen=lineList[(k+1)%nline];
		//��line��linen��
		float v[4];//����
		int judge=line.intersectWithLine_xz(linen,v);
		if(judge==1){//�ཻ
			//�������Ƿ������߶�line��linen��
			bool onlineOronlinen=false;
			//�������Ƿ�����line��
			bool online=false;
			float *v1=line.vbeg;//polygen.vlist[line.vID1].v;
			float *v2=line.vend;//polygen.vlist[line.vID2].v;
			float v1v[4],vv2[4];
			sub(v,v1,v1v);
			sub(v2,v,vv2);
			if(dot(v1v,vv2)>=0){//��line��
				onlineOronlinen=true;
			}else{
				//�������Ƿ�����linen��
				bool onlinen=false;
				float *v1n=linen.vbeg;//polygen.vlist[linen.vID1].v;
				float *v2n=linen.vend;//polygen.vlist[linen.vID2].v;
				float v1nv[4],vv2n[4];
				sub(v,v1n,v1nv);
				sub(v2n,v,vv2n);
				if(dot(v1nv,vv2n)>=0){//��linen��
					onlineOronlinen=true;
				}
			}//�õ�onlineOronlinen
			//���onlineOronlinen==true������v��Ϊ�߶ζ˵�
			if(onlineOronlinen){
				//�ý���ȥ���line.vend��linen.vbeg
				veccopy(v,line.vend);
				veccopy(v,linen.vbeg);
			}else{
				
				//�󽻵㵽line.vID2�ľ���ƽ���ͽ��㵽linen.vID1�ľ���ƽ��
				float *_v=line.vend;//polygen.vlist[line.vID2].v;
				float *_vn=linen.vbeg;//polygen.vlist[linen.vID1].v;
				float _d2=pow2(v[X]-_v[X])+pow2(v[Z]-_v[Z]);
				float _d2n=pow2(v[X]-_vn[X])+pow2(v[Z]-_vn[Z]);
				if(min(_d2,_d2n)>=2*2){//������㵽line.vID2��linen.vID1�ľ��붼��Զ
					//����v��Ϊ�߶ζ˵�
				}else{//������㵽line.vID2��linen.vID1�ľ��벻��Զ
					//��Ҫ��line��linen�ļнǣ�����нǲ�̫������ý�����Ϊ�˵�
					float cosA=dot(line.dir,linen.dir);
					if(cosA>=-0.08715){//A<=95
						//������
						//�ý���ȥ���line.vend��linen.vbeg
						veccopy(v,line.vend);
						veccopy(v,linen.vbeg);
					}else{
						//̫����
						//����v��Ϊ�߶ζ˵�
					}
					
					
					
				}
			}
			
			
			
			
		}else{//ƽ�л��غ�
			
			//��line.vID2��linen.vID1����
			float*v2=line.vend;//polygen.vlist[line.vID2].v;//line.vID2����
			float*v1n=linen.vbeg;//polygen.vlist[linen.vID1].v;//linen.vID1����
			//��v2,v1n�е�vmid
			float vmid[4];
			add(v2,v1n,vmid);
			mul(0.5,vmid,vmid);
			//��vmid��line��linen�ϵ�ͶӰ
			float vmid_proj[4];//vmid��line�ϵ�ͶӰ
			float vmid_projn[4];//vmid��linen�ϵ�ͶӰ
			line.getProjOfPoint_xz(vmid,vmid_proj);
			linen.getProjOfPoint_xz(vmid,vmid_projn);
			//��vmid_proj���line.vend����vmid_projn���linen.vbeg
			veccopy(vmid_proj,line.vend);
			veccopy(vmid_projn,linen.vbeg);
			
			
		}
	}
}

inline void make_polygen_use_lineList(myvector<Cline>&lineList,Cpolygen&polygen){
	//��polygen.lineList��������polygen
	Cline*plinef=NULL;//ָ��ǰһ��line��ָ��
	int nline=(int)lineList.size();
	for(int k=0;k<nline;k++){
		Cline&line=lineList[k];
		if(plinef==NULL){//û��ǰһ��line
			//��line�������˵㶼���뵽polygen
			Cv4 vbeg,vend;
			veccopy(line.vbeg,vbeg.v);
			veccopy(line.vend,vend.v);
			Cv4Ex vbegEx,vendEx;
			veccopy(vbeg.v,vbegEx.v);
			veccopy(vend.v,vendEx.v);
			polygen.vlist.push_back(vbegEx);
			polygen.vlist.push_back(vendEx);
		}else{//��ǰһ��line
			//�ж�line.vbeg��linef.vend�Ƿ��غ�
			//����غϾ�ֻ��line.vend����polygen
			//������غ���line.vbeg��line.vend������polygen
			float d2=pow2(line.vbeg[X]-plinef->vend[X])+pow2(line.vbeg[Z]-plinef->vend[Z]);
			if(d2<=0.01*0.01){//�㹻��
				//�غ�
				//ֻ��Line.vend����polygen
				Cv4Ex v;
				veccopy(line.vend,v.v);
				polygen.vlist.push_back(v);
			}else{//���غ�
				//��line.vbeg��line.vend������polygen
				Cv4Ex vbeg,vend;
				veccopy(line.vbeg,vbeg.v);
				veccopy(line.vend,vend.v);
				polygen.vlist.push_back(vbeg);
				polygen.vlist.push_back(vend);
				
			}
			
		}
		//Ϊ��һ��ѭ���洢plinef
		plinef=&line;
		
	}
	//���õ���polygen�����һ�������Ƿ����0�������غϣ�����غ������һ������ɾ��
	if((int)polygen.vlist.size()>=2){//ע�⣬����Ӵ��жϣ����������ú��ٹ�������
		Cv4Ex&v0=polygen.vlist[0];
		Cv4Ex&vlast=polygen.vlist[(int)polygen.vlist.size()-1];
		float d2=pow2(v0.v[X]-vlast.v[X])+pow2(v0.v[Z]-vlast.v[Z]);
		if(d2<=0.01*0.01){//�غ�
			polygen.vlist.pop_back();
		}
	}
				
				
}
inline void rid_polygen_nv_less_than_3(myvector<Cpolygen>&polygenList)
//ȥ��polygenList�ж�����С��3��polygen
{
	int count=0;
	int npolygen=(int)polygenList.size();
	for(int j=0;j<npolygen;j++){
		Cpolygen&polygen=polygenList[j];
		if((int)polygen.vlist.size()<3){
			count++;
		}else{
			if(count!=0)polygenList[j-count]=polygen;
			
		}
	}
	polygenList.resize(npolygen-count);
	
	
}
inline void ridSelfInteractForEachPolygen(myvector<Cpolygen>&polygenList){
	int npolygen=(int)polygenList.size();
	for(int j=0;j<npolygen;j++){
		Cpolygen&polygen=polygenList[j];
		ridSelfInteract_polygen(polygen);
	}
}

inline void cal_rightnorm_and_avrgnorm_for_each_vert_of_polygen(Cpolygen&polygen){
	int nv=(int)polygen.vlist.size();
	for(int k=0;k<nv;k++){
		Cv4Ex&v=polygen.vlist[k];
		float *vf=polygen.vlist[(k-1+nv)%nv].v;//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		float *vn=polygen.vlist[(k+1)%nv].v;
		//��vfv���ҷ�����
		float vfv[4];
		sub(v.v,vf,vfv);
		float norm1[4]={-vfv[Z],0,vfv[X]};
		normalize(norm1);
		veccopy(norm1,v.normf);
		//��vvn���ҷ�����
		float vvn[4];
		sub(vn,v.v,vvn);
		float norm2[4]={-vvn[Z],0,vvn[X]};
		normalize(norm2);
		veccopy(norm2,v.normn);
		//��norm1,norm2��ƽ��������
		float norm[4]={0,0,0,0};//ƽ��������
		add(norm1,norm,norm);
		add(norm2,norm,norm);
		norm[X]/=2;
		norm[Y]/=2;
		norm[Z]/=2;
		norm[W]=0;
		normalize(norm);//�����µ�λ��
		//��norm��Ϊ��v�ĵ㷨����
		veccopy(norm,v.norm);
		
		
	}
}

inline void insertVertsForPolygen(const Cpolygen&polygen,Cpolygen&_polygen)
//��polygen���в�ֵ���õ�_polygen
{
	//���в�ֵ
	int nv=(int)polygen.vlist.size();
	float mind=1;//��ֵ���
	for(int k=0;k<nv;k++){
		Cv4Ex v=polygen.vlist[k];
		_polygen.vlist.push_back(v);
		Cv4Ex vn=polygen.vlist[(k+1)%nv];
		//�����ֵ�����
		float d=fabs(v.v[X]-vn.v[X])+fabs(v.v[Y]-vn.v[Y])+fabs(v.v[Z]-vn.v[Z]);//v��vn�������پ���
		int n=max(d/mind-1,0);//��ֵ�����
		//��n����ֵ��
		for(int u=1;u<=n;u++){
			//��u����ֵ�㽫�߶�v-vn�ֳ�u:(n+1-u)����
			float vec[4];
			sub(vn.v,v.v,vec);
			mul(((float)u)/(n+1),vec,vec);
			Cv4Ex v_insert;//��ֵ��
			add(v.v,vec,v_insert.v);
			//��v_insert���뵽_polygen
			_polygen.vlist.push_back(v_insert);
		}
		//��vn���뵽_polygen
		_polygen.vlist.push_back(vn);
	
		
	}//�õ�_polygen
				
}

inline void calxzRangeForPolygenList(const myvector<Cpolygen>&polygenList,const float margin,
						  float&xmin,float&xmax,float&zmin,float&zmax)
//��polygenList��xz��Χ
{
	xmin=inf;
	xmax=-inf;
	zmin=inf;
	zmax=-inf;
	int npolygen=(int)polygenList.size();
	for(int j=0;j<npolygen;j++){
		const Cpolygen&polygen=polygenList[j];
		int nv=polygen.vlist.size();
		for(int k=0;k<nv;k++){
			const Cv4Ex&v=polygen.vlist[k];
			if(v.v[X]<xmin)xmin=v.v[X];
			if(v.v[X]>xmax)xmax=v.v[X];
			if(v.v[Z]<zmin)zmin=v.v[Z];
			if(v.v[Z]>zmax)zmax=v.v[Z];
		}
	}
	xmin-=margin;
	xmax+=margin;
	zmin-=margin;
	zmax+=margin;//�õ�xmin,xmax,zmin,zmax

}
inline void find_dominant_cloud_cluster(myvector<Ccloud>&cloudList){
		
  //Ϊÿ��cloud����������
	myvector<CIDgrid> IDgridList;
	IDgridList.resize((int)cloudList.size());
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			CIDgrid&IDgrid=IDgridList[i];
			if(cloud.removed)continue;
			float margin=6;
			IDgrid.makeIDgrid(cloud.xmin-margin,cloud.xmax+margin,cloud.zmin-margin,cloud.zmax+margin,//��Χ
				5,5,//����
				cloud.vlist//����
				);
		}

	
	}
	//��ÿ�����ţ���������������
	//����cloud����������Ϊ����cloud�ཻ�����(����Ҫ�н���)���ߴ��cloud�󣬾������ϸ�(��һ����cloud�ߣ���Ҫ�㹻��)������Ҫ���������������е���õ�һ��
	//�����cloud�Ҳ������ˣ���ȡcloud����Ϊ��������
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			//��cloud����������
			myvector<int> cloudIDList;//��ѡ�����б�
			for(int j=0;j<ncloud;j++){
				if(i==j)continue;
				Ccloud&_cloud=cloudList[j];	
				CIDgrid&_IDgrid=IDgridList[j];
				if(_cloud.removed)continue;
				//��_cloud��Χ�Ƿ��cloud��Χ��
				float _S=(_cloud.minRect.xmax-_cloud.minRect.xmin)*(_cloud.minRect.zmax-_cloud.minRect.zmin);//_cloud��Χ
				float S=(cloud.minRect.xmax-cloud.minRect.xmin)*(cloud.minRect.zmax-cloud.minRect.zmin);//cloud��Χ
				if(_S>S//���_cloud��Χ�󣬿��Է��У��������ϸ���ڣ�Ϊ�˷�ֹ��Ϊ��������������
					||
					(_S==S&&_cloud.avrgH<cloud.avrgH)//���_cloud��cloud��Χһ���󣬵�_cloud�ϵͣ�Ҳ���У�ͬ���������ϸ���ڣ�Ϊ�˷�ֹ��Ϊ��������������
					){
					//����
				}else{
					continue;
				}

				//��_cloud�Ƿ������Ϊcloud������
				//��_cloud��cloud��Χ�Ƿ���
				float cx,cz;
				cx=(cloud.xmin+cloud.xmax)/2;
				cz=(cloud.zmin+cloud.zmax)/2;
				float _cx,_cz;
				_cx=(_cloud.xmin+_cloud.xmax)/2;
				_cz=(_cloud.zmin+_cloud.zmax)/2;
				if(fabs(cx-_cx)<=0.5*(cloud.xmax-cloud.xmin)+0.5*(_cloud.xmax-_cloud.xmin)+1.5
					&&fabs(cz-_cz)<=0.5*(cloud.zmax-cloud.zmin)+0.5*(_cloud.zmax-_cloud.zmin)+1.5){//���
					//����
				}else{
					continue;//����
				}
		/*		//��_cloud�Ƿ��㹻��
				//֮���Բ��ܹ��ͣ�����Ϊ���������Ľ�����Ե�᲻׼����ȷ����������û�вο���ֵ
				if(_cloud.avrgH-_cloud.minH>=1.5){//??????????����ȥ���治׼ȷ�����µ͵ش��ѵ���ͻ�𵱳ɽ������Ӷ������������жϴ���
					                            //�������ֳ�ȥ�����׼ȷ�ԶԽ�ģ��Ӱ��
					                            //��������ȥ���治���
					
					//����
				}else{
				
					continue;
				}*/
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
					//��_cloud.IDgrid��I,J�������е�
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
				//ͨ�����飬��_cloud��ID�ż���cloudIDList
				cloudIDList.push_back(j);
				
			}//�õ�cloudIDList
			int nID=(int)cloudIDList.size();
			if(nID==0){//���cloudIDList�գ���û�к�ѡ����
				//���Լ���Ϊ��ѡ����
				cloud.hostID=i;
				cloud.selfHost=true;
			
			}else{//cloudIDList��Ϊ��
				//��cloudIDList������������
				float maxS=-inf;
				int ID=-1;
				for(j=0;j<nID;j++){
					int _ID=cloudIDList[j];
					Ccloud&_cloud=cloudList[_ID];
					float S=(_cloud.minRect.xmax-_cloud.minRect.xmin)*(_cloud.minRect.zmax-_cloud.minRect.zmin);
					if(S>maxS){
						maxS=S;
						ID=_ID;
					}
				}//�õ�maxS��ID
				cloud.hostID=ID;
				if(ID==-1){
					cout<<"ID==-1"<<endl;
					char c;
					cin>>c;
				}
			}//cloud���������ź�cloud.hostID�ѵõ�

			
		}


	}

	//���ϵõ��ĸ�cloud��hostIDֻ��һ��hostID��������Ҫ����ͨ�����ݣ���ÿ��cloud�ҵ����ĸ����˵�ID��Ϊ��hostID
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			//��cloud�ĸ�host
			int hostID=cloud.hostID;//�����˵�hostID
			while(1){
				Ccloud&_cloud=cloudList[hostID];
				if(_cloud.hostID!=hostID){
					hostID=_cloud.hostID;
				}else{
					break;
				}
			}//�õ�hostID;
			cloud.hostID=hostID;

			
		}


	}

}

inline void ridSelfInteract_polygenList_Ex(myvector<Cpolygen>&polygenList)
//ȥ��polygenList�и������֮����ཻ
{
	for(int i=0;i<(int)polygenList.size();i++){
		Cpolygen&polygen=polygenList[i];
		bool polygenRemoved=false;
		
		//���polygen����֮���polygen�Ƿ��ཻ
		for(int j=i+1;j<(int)polygenList.size();j++){
			Cpolygen&_polygen=polygenList[j];
			bool _polygenRemoved=false;
			//���polygen��_polygen�Ƿ����ཻ��
			for(int k=0;k<(int)polygen.vlist.size();k++){
				float *v1=polygen.vlist[k].v;
				int knext=(k+1)%(int)polygen.vlist.size();
				float *v2=polygen.vlist[knext].v;	
				//��v1v2�Ƿ���_polygenĳ�����ཻ
				for(int u=0;u<(int)_polygen.vlist.size();u++){//����Ҫ��_polygen.vlist����ɾ�����������Բ�����ǰ�̻�����
					float *_v1=_polygen.vlist[u].v;
					int unext=(u+1)%(int)_polygen.vlist.size();
					float *_v2=_polygen.vlist[unext].v;
					//��_v1_v2�Ƿ���v1v2�ཻ
					if(LineSegOverLineSeg(v1[X],v1[Z],v2[X],v2[Z],_v1[X],_v1[Z],_v2[X],_v2[Z])){//���v1v2��_v1_v2�ཻ
						//��polygen��_polygen˭��
						//��polygen���ܳ�
						float SL=0;
						{	
							int nv=(int)polygen.vlist.size();
							for(int i=0;i<nv;i++){
								float *v=polygen.vlist[i].v;
								float *vn=polygen.vlist[(i+1)%nv].v;
								float d2=pow2(v[X]-vn[X])+pow2(v[Z]-vn[Z]);
								float d=sqrt(d2);
								SL+=d;
							}//�õ�SL
						}
						//��_polygen���ܳ�
						float _SL=0;
						{	
							int nv=(int)_polygen.vlist.size();
							for(int i=0;i<nv;i++){
								float *v=_polygen.vlist[i].v;
								float *vn=_polygen.vlist[(i+1)%nv].v;
								float d2=pow2(v[X]-vn[X])+pow2(v[Z]-vn[Z]);
								float d=sqrt(d2);
								_SL+=d;
							}//�õ�_SL
						}
						if(SL>_SL){
							//ɾ��_polygen
							_polygenRemoved=true;
							polygenList.erase(&polygenList[j]);
							j--;
							break;
						}else{
							//ɾ��polygen
							polygenRemoved=true;
							polygenList.erase(&polygenList[i]);
							i--;
							break;
						
						}	
					
					}
				}
				if(polygenRemoved)break;
				if(_polygenRemoved)break;
				
			}
			if(polygenRemoved)break;
			if(_polygenRemoved)continue;;


		}
		if(polygenRemoved)continue;
	}

}
inline void regulate_mainDir_second(myvector<Cline>&lineList,const Cv4&mainDir,const Ccloud&cloud){
	//������С��Χ�������
	const float S=(cloud.minRect.xmax-cloud.minRect.xmin)*(cloud.minRect.zmax-cloud.minRect.zmin);
	//��polygen.lineList��ֱ�߽��й���
	int nline=(int)lineList.size();
	//��ʼ���򻯸�ֱ��
	for(int k=0;k<nline;k++){
		Cline&line=lineList[k];
		//�����������Ϊ�ο�
		float d2=line.getD2_xz();
		//����line���Ȳ�ͬ�����򻯲��Բ�ͬ
		//���line���Ƚ϶̣���������ȴ�
		//���line���Ƚϴ����������С
		if(d2<=5*5||S<10*10){//�ܶ�3.5*3.5
			float cosA=dot(line.dir,mainDir.v);
			if(cosA>=0.707||cosA<=-0.707){//A<=45||A>=135
				//ƽ��
				veccopy(mainDir.v,line.dir);
				if(cosA<0){
					mul(-1,line.dir,line.dir);
				}
			}else if(-0.707<=cosA&&cosA<=0.707){//135>=A>=45
				//��ֱ
				float cross_rs[4];
				cross(mainDir.v,line.dir,cross_rs);
				if(cross_rs[Y]>0){//͹��
					//��������
					float norm[4]={mainDir.v[Z],0,-mainDir.v[X],0};
					//��line.dir��Ϊnorm
					veccopy(norm,line.dir);
				}else{//����
					//���ҷ�����
					float norm[4]={-mainDir.v[Z],0,mainDir.v[X],0};
					//��line.dir��Ϊnorm
					veccopy(norm,line.dir);
				}	
			}
		}
		
		
	}
				
//*/

}

inline void adjustMainDirRefBigNeighbour(myvector<Ccloud>&cloudList){
	//��ÿ����С�������ţ��������֮�������������ö������x�����Ҷ��ߵ�������ǳ��ӽ�ƽ�л���ƽ�л�ֱ���򽫴˸������ŵ���������Ϊ��x��������һ��
	//����������˸�������Ϊ�����ŵ����ŵ�������ҲҪ��Ӧ�޸�
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			if(!cloud.selfHost)continue;
			bool&mainDirChanged=cloud.mainDirChanged;//����������
			//cloud�Ǹ�������
			//��cloud�����
			float S=(cloud.minRect.xmax-cloud.minRect.xmin)*(cloud.minRect.zmax-cloud.minRect.zmin);
			if(S>80*80)continue;
			float maxEdgeLen=max(cloud.minRect.xmax-cloud.minRect.xmin,cloud.minRect.zmax-cloud.minRect.zmin);
			//cloud�����С
			//Ϊcloud�Һ�ѡ����
			myvector<int> cloudIDList;//��ѡ�����б�
			myvector<float> SList;//cloudIDList��Ӧ��Sֵ�б�
			for(int j=0;j<ncloud;j++){
				if(i==j)continue;
				Ccloud&_cloud=cloudList[j];	
				if(_cloud.removed)continue;
				//��_cloud��Χ�Ƿ��cloud��Χ��
				float _S=(_cloud.minRect.xmax-_cloud.minRect.xmin)*(_cloud.minRect.zmax-_cloud.minRect.zmin);//_cloud��Χ
				float _maxEdgeLen=max(_cloud.minRect.xmax-_cloud.minRect.xmin,_cloud.minRect.zmax-_cloud.minRect.zmin);
				
				if(_S>2*S||_maxEdgeLen>1.5*maxEdgeLen&&_S>S){//���_cloud��Χ�󣬿��Է��У��������ϸ���ڣ�Ϊ�˷�ֹ��Ϊ��������������
					//����
				}else{
					continue;
				}
				//��_cloud��cloud��Χ�Ƿ���
				float cx,cz;
				cx=(cloud.xmin+cloud.xmax)/2;
				cz=(cloud.zmin+cloud.zmax)/2;
				float _cx,_cz;
				_cx=(_cloud.xmin+_cloud.xmax)/2;
				_cz=(_cloud.zmin+_cloud.zmax)/2;
				const float affectMargin=5;//30;//��ֵ����ȡ���ɵ���ȣ����Ǽ������ɵ�����ͬ��¥Ⱥ����
				if(fabs(cx-_cx)<=0.5*(cloud.xmax-cloud.xmin)+0.5*(_cloud.xmax-_cloud.xmin)+affectMargin
					&&fabs(cz-_cz)<=0.5*(cloud.zmax-cloud.zmin)+0.5*(_cloud.zmax-_cloud.zmin)+affectMargin
					){//���
					//����
				}else{
					continue;//����
				}
				//ͨ�����飬��_cloud��ID�ż���cloudIDList
				cloudIDList.push_back(j);
				SList.push_back(_S);
				
			}//�õ�cloudIDList
			int nID=(int)cloudIDList.size();
			if(nID==0){//���cloudIDList�գ���û�к�ѡ����
				//cloud�����˱��ֲ���
			
			}else{//cloudIDList��Ϊ��
				//��cloudIDListָʾ�����Ž���ɸѡ��ɸѡ����Щ��������cloud�ӽ�ƽ�л���ƽ�л�ֱ������
				Cv4&mainDir=cloud.mainDir;
				float maxS=-inf;
				int index=-1;
				for(int j=0;j<nID;j++){
					Ccloud&_cloud=cloudList[cloudIDList[j]];
					Cv4&_mainDir=_cloud.mainDir;//_cloud��������
					//��_cloud���������Ƿ���cloud��������ӽ�ƽ�л���ƽ�л�ֱ
					float cosA=dot(mainDir.v,_mainDir.v);
					if(cosA>=0.9848||cosA<=-0.9848||(cosA>=-0.1736&&cosA<=0.1736)){//10��
						float _S=SList[j];
						if(_S>maxS){
							maxS=_S;
							index=j;
						}
						
					
					}
				}//�õ�maxJ��index
				if(index==-1){
					//cloud�����˱��ֲ���
					
				}else{
					Cv4&_mainDir=cloudList[cloudIDList[index]].mainDir;
					mainDir=_mainDir;
					mainDirChanged=true;
				}

			}


		}
	}
	//�����ĳ���ŵ�������mainDirChanged������������Ҫ���¸���Ϊ��������һ��
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			Cv4&mainDir=cloud.mainDir;
			//��cloud�������ŵ�mainDirChanged
			bool _mainDirChanged=cloudList[cloud.hostID].mainDirChanged;//cloud�������ŵ�mainDirChanged
			//���_mainDirChanged����cloud��mainDirҪ��Ϊ��cloud��������һ��
			if(_mainDirChanged){
				//��host�������ŵ�mainDir
				Cv4&_mainDir=cloudList[cloud.hostID].mainDir;
				//��cloud��mainDir����_mainDir
				mainDir=_mainDir;
			}

		}
		
	}
	



}