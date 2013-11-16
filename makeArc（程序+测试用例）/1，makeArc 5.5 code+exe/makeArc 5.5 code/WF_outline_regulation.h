//--------------------------------------------
//轮廓规则化
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
//-----------------------------------主函数
inline void main_outline_regulation(myvector<Ccloud>&cloudList,bool doRegulation)
{
	//注意，即使doRegulation==false，也不能跳过求主方向一步，因为主方向在后面建oobb网格时要用到。

	//提直线
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			int npolygen=(int)cloud.polygenList.size();
			for(int j=0;j<npolygen;j++){
				Cpolygen&polygen=cloud.polygenList[j];
				myvector<Cline>&lineList=polygen.lineList;
				//删除重合的相邻点（为下一步求各点方向向量作准备）
				{
					polygen.ridSameNeighbour();
					
				}
				//求各点的方向向量
				{
					polygen.calDirForEachVert_xz();
				}
				//开始提取直线
				//此步有优化余地：line上所有点的均方差可以增量式求得，不必每次都重算
				{
					extractLine(polygen,lineList);
					
				}
				//把点拉到直线上
				{
					
					pullPointsToLine(lineList);
					
				}
				
			}
		}
		
		
	}
	
	//求主云团
	{
		find_dominant_cloud_cluster(cloudList);
	}
	//求主方向
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			Cv4&mainDir=cloud.mainDir;
			bool&mainDirGot=cloud.mainDirGot;
			//求cloud的主方向
			Ccloud&_cloud=cloudList[cloud.hostID];
			Cv4&_mainDir=_cloud.mainDir;
			bool&_mainDirGot=_cloud.mainDirGot;
			if(_mainDirGot){//如果_cloud.mainDir已得到，则直接拷贝给cloud
				veccopy(_mainDir.v,mainDir.v);
				mainDirGot=true;
				continue;
			}//否则求_lineListList中最长边的方向，作为cloud的主方向
			float dir[4];
			cal_mainDir(_cloud,dir);
			//将dir赋给_mainDir
			veccopy(dir,_mainDir.v);
			_mainDirGot=true;
			//将_mainDir赋给mainDir
			veccopy(_mainDir.v,mainDir.v);
			mainDirGot=true;
			
			
			
		}
		
		
	}
	//参照邻近建筑重新调整主方向
	{
		//		adjustMainDirRefBigNeighbour(cloudList);
		
	}
	//删除各cloud的hostID域
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			cloud.hostID=-1;
		}
		
	}
	if(!doRegulation){
		cout<<"（规则化跳过）"<<endl;
	}else{
		
		//规则化
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
					//主方向规则化+局部规则化
					//主方向规则化（简单）
					{
						regulate_mainDir_simple(lineList,mainDir);
						
					}
					//遍历lineList中各line,其中的所有顶点都拉到line上
					{
						pullPointsToLine(lineList);
					}
					//平行直线合并
					{
						mergeParallelLines(lineList);
						pullPointsToLine(lineList);
					}
					//主方向规则化（复杂）
					{
						switch(regulationDegree){
						case RGL_STRONG_WEAK:
							{
								if(cloud.avrgH-cloud.minH>=minHNotRegulationStrong){//云团高
									//弱规则化
									const float a=30;
									const float b=12;
									const float da=3.5;
									const float db=10;
									regulate_mainDir(lineList,mainDir,a,b,da,db);
								}else{//云团低
									//强规则化
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
							cout<<"error! 无效的regulationDegree值!"<<endl;
							
						}
						
					}
					//把点拉到直线上
					{
						
						pullPointsToLine(lineList);
						
					}
					//平行直线合并
					{
						mergeParallelLines(lineList);
						//mergeParallelLines后面必须立刻跟一个pullPointsToLine，否则会变歪
						pullPointsToLine(lineList);
					}
					//处理相邻直线反向平行的情况
					{
						deal_neighbour_rev_parallel(lineList);
					}
					//将不与主方向平行或垂直的边作成与主方向平行或垂直
					switch(regulationDegree){
					case RGL_STRONG_WEAK:
						{
							//判断云团高度，决定是否进行regulate_mainDir_second
							if(cloud.avrgH-cloud.minH>=minHNotRegulationStrong){//云团高
								//不进行regulate_mainDir_second
							}else{//云团低
								//进行regulate_mainDir_second
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
						cout<<"error! 无效的regulationDegree值!"<<endl;
						
					}
					//平行直线合并
					{
						mergeParallelLines(lineList);
						pullPointsToLine(lineList);
						
					}
					//通过相邻直线求交得到顶点
					{
						cal_vertex_via_neighbour_intersect(lineList);
					}
					
				}
			}
			
			
		}
		
		
		
		
		
		//用lineList重新生成polygen
		{
			int ncloud=(int)cloudList.size();
			for(int i=0;i<ncloud;i++){
				Ccloud&cloud=cloudList[i];
				if(cloud.removed)continue;
				int npolygen=(int)cloud.polygenList.size();
				for(int j=0;j<npolygen;j++){
					Cpolygen&polygen=cloud.polygenList[j];
					myvector<Cline>&lineList=polygen.lineList;
					//将polygen销毁
					polygen.vlist.resize(0);
					//依据lineList重新生成polygen
					make_polygen_use_lineList(lineList,polygen);
					
				}
			}
			
		}
		//多边形插值
		{
			int ncloud=(int)cloudList.size();
			for(int i=0;i<ncloud;i++){
				Ccloud&cloud=cloudList[i];
				if(cloud.removed)continue;
				myvector<Cpolygen>&polygenList=cloud.polygenList;
				//对各polygen插值使顶点恢复为等间距
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
	//去掉polygen中的重合点
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
						//删除vn
						polygen.vlist.erase(&polygen.vlist[(k+1)%nv]);
						nv--;
						k--;
					
					}

				}
				
				
			}
		}

	}
    //边形进行合法化
	//多边形合法化中的去自相交必须对插值后的多边形进行
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			myvector<Cpolygen>&polygenList=cloud.polygenList;
			//将polygenList中顶点数少于3的polygen去掉（这一步非常重要!!使后继步骤能安全地进行）
			{
				rid_polygen_nv_less_than_3(polygenList);
			}
			//为各polygen去自相交
			{
				ridSelfInteractForEachPolygen(polygenList);
			}
			//为polygenList内的polygen之间去自相交
			{
				ridSelfInteract_polygenList_Ex(polygenList);
			}
			//再次将polygenList中顶点数少于3的polygen去掉
			{
				rid_polygen_nv_less_than_3(polygenList);
			}
			//自相交检测
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
	//去掉polygen中的重合点
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
						//删除vn
						polygen.vlist.erase(&polygen.vlist[(k+1)%nv]);
						nv--;
						k--;
					
					}

				}
				
				
			}
		}

	}
	//标记isKeyPoint（必须放在合法化之后，因为此时多边形方才定型）
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			int npolygen=(int)cloud.polygenList.size();
			for(int j=0;j<npolygen;j++){
				Cpolygen&polygen=cloud.polygenList[j];
				//求各点方向向量
				{
					polygen.calDirForEachVert_xz();
				}
				//标记各点的isKeyPoint域
				{
					//如果某点的前一个点的方向向量与本点方向向量不平行，则标记为isKeyPoint点
					int nv=(int)polygen.vlist.size();
					for(int k=0;k<nv;k++){
						Cv4Ex&v=polygen.vlist[k];
						Cv4Ex&vf=polygen.vlist[(k-1+nv)%nv];
						float crossRs[4];
						cross(vf.dir,v.dir,crossRs);
						//如果crossRs[Y]非常接近0，则说明vf.dir和v.dir平行，不标记
						if(fabs(crossRs[Y])<0.001){//vf.dir与v.dir平行
							//不标记
						}else{//vf.dir与v.dir不平行
							//标记
							v.isKeyPoint=true;
						}
					}
				}
			}
		}

	
	}
	//处理洞多边形以及对含有多个轮廓多边形的polygenList进行分裂
	//****注意：此过程增加云团****
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			myvector<Cpolygen>&polygenList=cloud.polygenList;
			//将polygenList中顶点数少于3的polygen去掉（这一步非常重要!!使后继步骤能安全地进行）
			{
				rid_polygen_nv_less_than_3(polygenList);
			}
			//为各polygen去自相交
			{
				ridSelfInteractForEachPolygen(polygenList);
			}
			//为polygenList内的polygen之间去自相交
			{
				ridSelfInteract_polygenList_Ex(polygenList);
			}
			//再次将polygenList中顶点数少于3的polygen去掉
			{
				rid_polygen_nv_less_than_3(polygenList);
			}
			//自相交检测
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
			//标记洞多边形
			{
				int npolygen=(int)cloud.polygenList.size();
				for(int j=0;j<npolygen;j++){
					Cpolygen&polygen=cloud.polygenList[j];
					polygen.isHole=isHolePolygen(polygen);
				}
			}
			//删除cloud.polygenList中所有较小的洞多边形
			{
				int npolygen=(int)cloud.polygenList.size();
				for(int j=0;j<npolygen;j++){
					Cpolygen&polygen=cloud.polygenList[j];
					if(!polygen.isHole)continue;//只考虑isHole为true者
					//求polygen周长
					float L=0;
					{
						int nv=(int)polygen.vlist.size();
						for(int k=0;k<nv;k++){
							Cv4Ex&v=polygen.vlist[k];
							Cv4Ex&vn=polygen.vlist[(k+1)%nv];
							float dL=CarmSqrt(pow2(v.v[X]-vn.v[X])+pow2(v.v[Z]-vn.v[Z]));
							L+=dL;
						}//得到L
					}
					if(L<20){
						cloud.polygenList.erase(&cloud.polygenList[j]);
		//				cout<<"--"<<endl;
						j--;
						npolygen--;
					}
				}
				
			}
			//标记轮廓多边形
			{
				int npolygen=(int)cloud.polygenList.size();
				for(int j=0;j<npolygen;j++){
					Cpolygen&polygen=cloud.polygenList[j];
					polygen.isOutline=isOutlinePolygen(polygen);
				}
			}
			
			
			
			//如果polygenList中有两个以上轮廓多边形，则将其中多余的轮廓多边形拿出来单独成一个cloud
			{
				//统计polygenList中轮廓多边形个数
				int count=0;//polygenList中轮廓多边形个数
				{
					int npolygen=(int)cloud.polygenList.size();
					for(int j=0;j<npolygen;j++){
						Cpolygen&polygen=cloud.polygenList[j];
						if(polygen.isOutline)count++;
					}//得到count
				}
				if(count==0){
					cloud.removed=true;
					continue;
				}else if(count>1){
					//求周长最大的轮廓多边形
					float maxL=-inf;
					int I=-1;
					int npolygen=(int)cloud.polygenList.size();
					for(int j=0;j<npolygen;j++){
						Cpolygen&polygen=cloud.polygenList[j];
						if(!polygen.isOutline)continue;//只考虑轮廓多边形
						//求polygen周长
						float L=0;
						{
							int nv=(int)polygen.vlist.size();
							for(int k=0;k<nv;k++){
								Cv4Ex&v=polygen.vlist[k];
								Cv4Ex&vn=polygen.vlist[(k+1)%nv];
								float dL=CarmSqrt(pow2(v.v[X]-vn.v[X])+pow2(v.v[Z]-vn.v[Z]));
								L+=dL;
							}//得到L
						}
						if(L>maxL){
							maxL=L;
							I=j;
						}
					}//得到maxL和I
					//将polygenList中除I以外的轮廓多边形标记为takeOut
					{	
						int npolygen=(int)cloud.polygenList.size();
						for(int j=0;j<npolygen;j++){
							Cpolygen&polygen=cloud.polygenList[j];
							if(j!=I&&polygen.isOutline)polygen.takeOut=true;
						}
					}
					//将标记为takeOut的polygen单独作成polygenList加入到cloudList
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
								//删除polygen
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

	//结合轮廓重新计算cloud的范围
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			float xmin,xmax,zmin,zmax;
			calxzRangeForPolygenList(cloud.polygenList,2,
						  xmin,xmax,zmin,zmax);
			//与原来的范围比较，要保证不缩小（必须保证不缩小，否则出错）
			if(xmax>cloud.xmax)cloud.xmax=xmax;
			if(zmin<cloud.zmin)cloud.zmin=zmin;
			if(zmax>cloud.zmax)cloud.zmax=zmax;

		}

	}
	//重新计算polygen各顶点前、后、平均法向量
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
	//为每个polygen生成一个颜色
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
	//删除各polygen的lineList
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







//--------------------------------------辅助函数


inline void extractLine(const Cpolygen&polygen,myvector<Cline>&lineList){
	
	//对polygen提取直线
	int nv=(int)polygen.vlist.size();
	int k=0;//当前种子点
	while(1){
		if(k>=nv)break;
		const Cv4Ex&v=polygen.vlist[k];
		//以v为种子点提取一条直线
		float sumDir[4]={0,0,0,0};//方向向量累加值
		float sumV[4]={0,0,0,0};//坐标累加值
		//将v加入
		add(sumDir,v.dir,sumDir);
		sumDir[Y]=0;
		add(sumV,v.v,sumV);
		sumV[Y]=0;
		//依次尝试将k后面的点加入到直线
		for(int u=k+1;u<nv;u++){
			const Cv4Ex&_v=polygen.vlist[u];
			//看_v是否可以加入到直线
			//求将_v加入后的sumDir和sumV
			float _sumDir[4];
			add(sumDir,_v.dir,_sumDir);
			_sumDir[Y]=0;
			float _sumV[4];
			add(sumV,_v.v,_sumV);
			_sumV[Y]=0;
			//求加入后的平均方向向量和平均位置向量
			int count=u-k+1;//加入u后已加入的总点数
			float _avrgDir[4]={_sumDir[X],0,_sumDir[Z],0};
			normalize(_avrgDir);
			float _avrgV[4]={_sumV[X]/count,0,_sumV[Z]/count,1};
			//临时直线
			Cline _line;
			_line.makeLine(_avrgV,_avrgDir);
			//求所有加入点到直线距离的均方差
			float avrgD2=0;
			for(int t=k;t<=u;t++){
				const	Cv4Ex&__v=polygen.vlist[t];
				float PND=PND_p_line_xz(__v.v,_line);
				avrgD2+=fabs(PND*PND*PND*PND);
			}
			avrgD2/=count;
			const float threshold_d=0.15;

			if(avrgD2>=threshold_d*threshold_d*threshold_d*threshold_d){
				//直线结束
				break;
			}else{
				//将此点真正加入
				add(sumDir,_v.dir,sumDir);
				sumDir[Y]=0;
				add(sumV,_v.v,sumV);
				sumV[Y]=0;
			}
		}//得到直线
		//生成此直线
		Cline line;//xz面上的直线
		int count=(u-1)-k+1;
		float avrgDir[4]={sumDir[X],0,sumDir[Z],0};
		normalize(avrgDir);
		float avrgV[4]={sumV[X]/count,0,sumV[Z]/count,1};
		line.makeLine(avrgV,avrgDir);
		const		float *vbeg=polygen.vlist[k].v;

		const		float *vend=(u-1==k)?polygen.vlist[(k+1)%nv].v:polygen.vlist[u-1].v;//避免vbeg和vend相同，如果相同，则用下一个点作为vend
		veccopy(vbeg,line.vbeg);
		veccopy(vend,line.vend);
		//将line加入polygen.lineList
		lineList.push_back(line);
		//进行下一条直线提取
		k=u;
	}//对polygen的直线提取完毕	

}

inline void cal_mainDir(Ccloud&_cloud,float mainDir[4])
//求_cloud的主方向
{
	
	int maxpolygenID=-1;//包含最长边的多边形的ID
	int maxlineID=-1;//支持长度最长的边的ID
	float maxd=-inf;//支持长度
	int npolygen=_cloud.polygenList.size();
	for(int j=0;j<npolygen;j++){
		const myvector<Cline>&_lineList=_cloud.polygenList[j].lineList;
		int nline=(int)_lineList.size();
		for(int k=0;k<nline;k++){
			const Cline&line=_lineList[k];
			//求line的支持长度
			//即所有与line接近平行或垂直的直线的长度和
			float d=0;//支持长度
			for(int u=0;u<nline;u++){
				const Cline&_line=_lineList[u];
				//看_line与line是否接近平行或垂直
				float cosA=dot(line.dir,_line.dir);
			   if(cosA>=0.996||cosA<=-0.996||(cosA<=0.087&&cosA>=-0.087)){//A<=5||A>=175||(A>=85&&A<=95)
			//	if(cosA>=0.9986||cosA<=-0.9986||(cosA<=0.0523&&cosA>=-0.0523)){//A<=3||A>=177||(A>=87&&A<=93)
					//	if(cosA>=0.99939||cosA<=-0.99939||(cosA<=0.034899&&cosA>=-0.034899)){//A<=2||A>=178||(A>=88&&A<=92)
					//平行或垂直
					//求_line长度
					float _d=sqrt(_line.getD2_xz());
					d+=_d;
				}
			}//得到d
			if(d>maxd){
				maxd=d;
				maxlineID=k;
				maxpolygenID=j;
			}
		}
	}//得到maxpolygenID,maxlineID和maxd
	if(maxpolygenID==-1){
		cout<<"warning! 1"<<endl;
		_cloud.isRed=true;
		maxlineID=0;
		maxpolygenID=0;
	}
	//支持长度最长的边
	const Cline&maxline=_cloud.polygenList[maxpolygenID].lineList[maxlineID];//主方向边
	//对maxline的支持边方向向量取平均值得到最终的支持方向
	//而且此平均值为带权平均值，权值为各边长度
	float dir[4]={0,0,0,0};//所有支持dir的平均带权方向向量
	for(j=0;j<npolygen;j++){
		const myvector<Cline>&_lineList=_cloud.polygenList[j].lineList;
		int nline=(int)_lineList.size();
		for(int k=0;k<nline;k++){
			const Cline&line=_lineList[k];
			//看line是否支持maxline
			float cosA=dot(line.dir,maxline.dir);
			if(cosA>=0.996||cosA<=-0.996||(cosA<=0.087&&cosA>=-0.087)){//A<=5||A>=175||(A>=85&&A<=95)
	//		if(cosA>=0.9986||cosA<=-0.9986||(cosA<=0.0523&&cosA>=-0.0523)){//A<=3||A>=177||(A>=87&&A<=93)
				//		if(cosA>=0.99939||cosA<=-0.99939||(cosA<=0.034899&&cosA>=-0.034899)){//A<=2||A>=178||(A>=88&&A<=92)
				//line支持maxline
				//求line长度
				float _d=sqrt(line.getD2_xz());
				//将line转为与maxline近似同向平行
				float _dir[4];//line转为与maxline近似同向平行后的方向向量
				veccopy(line.dir,_dir);
				if(cosA>=0.996){//本来就是同向平行
					//_dir不用变
				}else if(cosA<=-0.996){//反向平行
					//将_dir反向
					mul(-1,_dir,_dir);
				}else if(cosA<=0.087&&cosA>=-0.087){//垂直
					//将_dir变成与自己垂直向量
					init4(_dir,-_dir[Z],_dir[Y],_dir[X],0);
					//看_dir与maxline是同向还是反向
					float _cosA=dot(_dir,maxline.dir);
					if(_cosA>=0){//_dir与maxline同向
						//_dir不用变
					}else{//_dir与maxline反向
						//将_dir反向
						mul(-1,_dir,_dir);
					}
				}
				//将_dir的模作成_d
				mul(_d,_dir,_dir);//此时_dir已是带权方向向量
				//将_dir加到dir
				add(_dir,dir,dir);
			}
			
		}
	}
	//将dir单位化得到dir
	normalize(dir);
	//将dir赋给mainDir
	veccopy(dir,mainDir);

}
inline void regulate_mainDir_simple(myvector<Cline>&lineList,const Cv4&mainDir){
	//将lineList中直线进行规则化
	int nline=lineList.size();
	for(int k=0;k<nline;k++){
		Cline&line=lineList[k];
		//求此边的长度平方
		float d2=line.getD2_xz();
	//	if(d2<=5*5){
			
	//	}else
		{//如果此边是长边
			//用主方向边作为参考
			float cosA=dot(line.dir,mainDir.v);
			if(cosA>=0.984||cosA<=-0.984){//A<=10||A>=170
				//平行
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
	//将polygen.lineList中直线进行规则化
	int nline=(int)lineList.size();
	//开始规则化各直线
	for(int k=0;k<nline;k++){
		Cline&line=lineList[k];
		//用主方向边作为参考
		float d2=line.getD2_xz();
		//根据line长度不同，规则化策略不同
		//如果line长度较短，则规则化力度大
		//如果line长度较大，则规则化力度小
		//使用连续阈值
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
			//平行
			veccopy(mainDir.v,line.dir);
			if(cosA<0){
				mul(-1,line.dir,line.dir);
			}
		}else if(-cos_halfpi_sub_A_threshold<=cosA&&cosA<=cos_halfpi_sub_A_threshold){//90+A_threshold>=A>=90-A_threshold
			//垂直
			float cross_rs[4];
			cross(mainDir.v,line.dir,cross_rs);
			if(cross_rs[Y]>0){//凸角
				//求左法向量
				float norm[4]={mainDir.v[Z],0,-mainDir.v[X],0};
				//将line.dir置为norm
				veccopy(norm,line.dir);
			}
			else{//凹角
				//求右法向量
				float norm[4]={-mainDir.v[Z],0,mainDir.v[X],0};
				//将line.dir置为norm
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
	//平行直线的合并
	//对相邻直线进行合并
	int nline=(int)lineList.size();
	int k=0;//当前直线
	while(1){	
		if(k==nline)break;
		Cline&line=lineList[k];
		Cline&linen=lineList[(k+1)%nline];
		//看line和linen能否合并
		float*dir=line.dir;
		float*dirn=linen.dir;
		float cosA=dot(dir,dirn);
		if(cosA>=0.984){//A<=10
			//认为平行
			//看两直线距离是否足够近
			//用line.vID2到linen的距离作为两直线间的距离
			float*v=line.vend;//polygen.vlist[line.vID2].v;
			float PND=PND_p_line_xz(v,linen);
			if(fabs(PND)<=2/*1.5*/){//对line和linen进行合并
				Cline tline;//暂存合并结果
				float line_D2=line.getD2_xz();
				float linen_D2=linen.getD2_xz();
				//求合并后的p
				//用较长者的p
				float p[4];
				if(line_D2>=linen_D2){
					veccopy(line.p,p);
				}else{
					veccopy(linen.p,p);
				}
			/*	float p[4];//合并后的p
				//p=(line.p*line.D2+linen.p*linen.D2)/(line.D2+linen.D2)
				//分解为p=vec/K,K=line.D2+linen.D2,vec=vec1+vec2,vec1=line.p*line.D2,vec2=linen.p*linen.D2
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
				p[W]=1;//得到p*/
				//求合并后的dir
				//用较长者的dir
				float dir[4];
				if(line_D2>=linen_D2){
					veccopy(line.dir,dir);
				}else{
					veccopy(linen.dir,dir);
				}
				//制作tline
				tline.makeLine(p,dir);
				veccopy(line.vbeg,tline.vbeg);
				veccopy(linen.vend,tline.vend);
				//用tline替换line
				line=tline;
				//将linen删除
				lineList.erase(&lineList[(k+1)%nline]);
				nline--;
				//k不动
				
			}else{
				k++;
			}
			
		}else{
			k++;
		}
	}
				
}

inline void deal_neighbour_rev_parallel(myvector<Cline>&lineList)
//处理相邻直线反向平行的情况
{
	int nline=(int)lineList.size();
	for(int k=0;k<nline;k++){
		
		Cline&line=lineList[k];
		Cline&linen=lineList[(k+1)%nline];
		//如果line与linen反向平行
		float cosA=dot(line.dir,linen.dir);
		
		if(cosA<=-0.9848){//A>=170
			//反向平行
			//认为linen与line反向平行
			//看两直线距离是否足够近
			//用line.vID2到linen的距离作为两直线间的距离
			float*v=line.vend;//polygen.vlist[line.vID2].v;
			float PND=PND_p_line_xz(v,linen);
			if(fabs(PND)<=1){//距离足够近
				//判断是凸角还是凹角
				float cross_rs[4];
				cross(line.dir,linen.dir,cross_rs);
				if(cross_rs[Y]>=0){//凸角
					float d2=line.getD2_xz();
					float d2n=linen.getD2_xz();
					if(d2>=d2n){//line长
						//删除linen
						lineList.erase(&lineList[(k+1)%nline]);
						k--;
						nline--;		
					}else{//linen长
						//删除line
						lineList.erase(&lineList[k]);
						k--;
						nline--;
					}
					
				}else{//凹角
					//凹角可以不管，因为看不到
				}
				
			}else{//距离较远
				//求line.vend在linen上的投影v_proj,令linen.vbeg=v_proj
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
		//将line与linen求交
		float v[4];//交点
		int judge=line.intersectWithLine_xz(linen,v);
		if(judge==1){//相交
			//看交点是否落在线段line或linen上
			bool onlineOronlinen=false;
			//看交点是否落在line上
			bool online=false;
			float *v1=line.vbeg;//polygen.vlist[line.vID1].v;
			float *v2=line.vend;//polygen.vlist[line.vID2].v;
			float v1v[4],vv2[4];
			sub(v,v1,v1v);
			sub(v2,v,vv2);
			if(dot(v1v,vv2)>=0){//在line上
				onlineOronlinen=true;
			}else{
				//看交点是否落在linen上
				bool onlinen=false;
				float *v1n=linen.vbeg;//polygen.vlist[linen.vID1].v;
				float *v2n=linen.vend;//polygen.vlist[linen.vID2].v;
				float v1nv[4],vv2n[4];
				sub(v,v1n,v1nv);
				sub(v2n,v,vv2n);
				if(dot(v1nv,vv2n)>=0){//在linen上
					onlineOronlinen=true;
				}
			}//得到onlineOronlinen
			//如果onlineOronlinen==true，则用v作为线段端点
			if(onlineOronlinen){
				//用交点去填充line.vend和linen.vbeg
				veccopy(v,line.vend);
				veccopy(v,linen.vbeg);
			}else{
				
				//求交点到line.vID2的距离平方和交点到linen.vID1的距离平方
				float *_v=line.vend;//polygen.vlist[line.vID2].v;
				float *_vn=linen.vbeg;//polygen.vlist[linen.vID1].v;
				float _d2=pow2(v[X]-_v[X])+pow2(v[Z]-_v[Z]);
				float _d2n=pow2(v[X]-_vn[X])+pow2(v[Z]-_vn[Z]);
				if(min(_d2,_d2n)>=2*2){//如果交点到line.vID2和linen.vID1的距离都较远
					//不用v作为线段端点
				}else{//如果交点到line.vID2和linen.vID1的距离不都远
					//还要看line和linen的夹角，如果夹角不太尖锐才用交点作为端点
					float cosA=dot(line.dir,linen.dir);
					if(cosA>=-0.08715){//A<=95
						//不尖锐
						//用交点去填充line.vend和linen.vbeg
						veccopy(v,line.vend);
						veccopy(v,linen.vbeg);
					}else{
						//太尖锐
						//不用v作为线段端点
					}
					
					
					
				}
			}
			
			
			
			
		}else{//平行或重合
			
			//将line.vID2与linen.vID1对齐
			float*v2=line.vend;//polygen.vlist[line.vID2].v;//line.vID2坐标
			float*v1n=linen.vbeg;//polygen.vlist[linen.vID1].v;//linen.vID1坐标
			//求v2,v1n中点vmid
			float vmid[4];
			add(v2,v1n,vmid);
			mul(0.5,vmid,vmid);
			//求vmid在line和linen上的投影
			float vmid_proj[4];//vmid在line上的投影
			float vmid_projn[4];//vmid在linen上的投影
			line.getProjOfPoint_xz(vmid,vmid_proj);
			linen.getProjOfPoint_xz(vmid,vmid_projn);
			//用vmid_proj填充line.vend，用vmid_projn填充linen.vbeg
			veccopy(vmid_proj,line.vend);
			veccopy(vmid_projn,linen.vbeg);
			
			
		}
	}
}

inline void make_polygen_use_lineList(myvector<Cline>&lineList,Cpolygen&polygen){
	//用polygen.lineList重新生成polygen
	Cline*plinef=NULL;//指向前一个line的指针
	int nline=(int)lineList.size();
	for(int k=0;k<nline;k++){
		Cline&line=lineList[k];
		if(plinef==NULL){//没有前一个line
			//将line的两个端点都加入到polygen
			Cv4 vbeg,vend;
			veccopy(line.vbeg,vbeg.v);
			veccopy(line.vend,vend.v);
			Cv4Ex vbegEx,vendEx;
			veccopy(vbeg.v,vbegEx.v);
			veccopy(vend.v,vendEx.v);
			polygen.vlist.push_back(vbegEx);
			polygen.vlist.push_back(vendEx);
		}else{//有前一个line
			//判断line.vbeg与linef.vend是否重合
			//如果重合就只将line.vend加入polygen
			//如果不重合则将line.vbeg和line.vend都加入polygen
			float d2=pow2(line.vbeg[X]-plinef->vend[X])+pow2(line.vbeg[Z]-plinef->vend[Z]);
			if(d2<=0.01*0.01){//足够近
				//重合
				//只将Line.vend加入polygen
				Cv4Ex v;
				veccopy(line.vend,v.v);
				polygen.vlist.push_back(v);
			}else{//不重合
				//将line.vbeg和line.vend都加入polygen
				Cv4Ex vbeg,vend;
				veccopy(line.vbeg,vbeg.v);
				veccopy(line.vend,vend.v);
				polygen.vlist.push_back(vbeg);
				polygen.vlist.push_back(vend);
				
			}
			
		}
		//为下一次循环存储plinef
		plinef=&line;
		
	}
	//看得到的polygen的最后一个顶点是否与第0个顶点重合，如果重合则将最后一个顶点删除
	if((int)polygen.vlist.size()>=2){//注意，必须加此判断，否则导致重置后再构建崩溃
		Cv4Ex&v0=polygen.vlist[0];
		Cv4Ex&vlast=polygen.vlist[(int)polygen.vlist.size()-1];
		float d2=pow2(v0.v[X]-vlast.v[X])+pow2(v0.v[Z]-vlast.v[Z]);
		if(d2<=0.01*0.01){//重合
			polygen.vlist.pop_back();
		}
	}
				
				
}
inline void rid_polygen_nv_less_than_3(myvector<Cpolygen>&polygenList)
//去掉polygenList中顶点数小于3的polygen
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
		//求vfv的右法向量
		float vfv[4];
		sub(v.v,vf,vfv);
		float norm1[4]={-vfv[Z],0,vfv[X]};
		normalize(norm1);
		veccopy(norm1,v.normf);
		//求vvn的右法向量
		float vvn[4];
		sub(vn,v.v,vvn);
		float norm2[4]={-vvn[Z],0,vvn[X]};
		normalize(norm2);
		veccopy(norm2,v.normn);
		//求norm1,norm2的平均法向量
		float norm[4]={0,0,0,0};//平均法向量
		add(norm1,norm,norm);
		add(norm2,norm,norm);
		norm[X]/=2;
		norm[Y]/=2;
		norm[Z]/=2;
		norm[W]=0;
		normalize(norm);//需重新单位化
		//将norm作为点v的点法向量
		veccopy(norm,v.norm);
		
		
	}
}

inline void insertVertsForPolygen(const Cpolygen&polygen,Cpolygen&_polygen)
//对polygen进行插值，得到_polygen
{
	//进行插值
	int nv=(int)polygen.vlist.size();
	float mind=1;//差值间距
	for(int k=0;k<nv;k++){
		Cv4Ex v=polygen.vlist[k];
		_polygen.vlist.push_back(v);
		Cv4Ex vn=polygen.vlist[(k+1)%nv];
		//计算插值点个数
		float d=fabs(v.v[X]-vn.v[X])+fabs(v.v[Y]-vn.v[Y])+fabs(v.v[Z]-vn.v[Z]);//v到vn的曼哈顿距离
		int n=max(d/mind-1,0);//插值点个数
		//求n个插值点
		for(int u=1;u<=n;u++){
			//第u个插值点将线段v-vn分成u:(n+1-u)两段
			float vec[4];
			sub(vn.v,v.v,vec);
			mul(((float)u)/(n+1),vec,vec);
			Cv4Ex v_insert;//插值点
			add(v.v,vec,v_insert.v);
			//将v_insert加入到_polygen
			_polygen.vlist.push_back(v_insert);
		}
		//将vn加入到_polygen
		_polygen.vlist.push_back(vn);
	
		
	}//得到_polygen
				
}

inline void calxzRangeForPolygenList(const myvector<Cpolygen>&polygenList,const float margin,
						  float&xmin,float&xmax,float&zmin,float&zmax)
//求polygenList的xz范围
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
	zmax+=margin;//得到xmin,xmax,zmin,zmax

}
inline void find_dominant_cloud_cluster(myvector<Ccloud>&cloudList){
		
  //为每个cloud建索引网格
	myvector<CIDgrid> IDgridList;
	IDgridList.resize((int)cloudList.size());
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			CIDgrid&IDgrid=IDgridList[i];
			if(cloud.removed)continue;
			float margin=6;
			IDgrid.makeIDgrid(cloud.xmin-margin,cloud.xmax+margin,cloud.zmin-margin,cloud.zmax+margin,//范围
				5,5,//步长
				cloud.vlist//点云
				);
		}

	
	}
	//对每个云团，找它的主人云团
	//云团cloud的主人云团为：与cloud相交或紧邻(至少要有交点)，尺寸比cloud大，距离地面较高(不一定比cloud高，但要足够高)，而且要是满足以上条件中的最好的一个
	//如果对cloud找不到主人，则取cloud自身为主人云团
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			//找cloud的主人云团
			myvector<int> cloudIDList;//候选主人列表
			for(int j=0;j<ncloud;j++){
				if(i==j)continue;
				Ccloud&_cloud=cloudList[j];	
				CIDgrid&_IDgrid=IDgridList[j];
				if(_cloud.removed)continue;
				//看_cloud范围是否比cloud范围大
				float _S=(_cloud.minRect.xmax-_cloud.minRect.xmin)*(_cloud.minRect.zmax-_cloud.minRect.zmin);//_cloud范围
				float S=(cloud.minRect.xmax-cloud.minRect.xmin)*(cloud.minRect.zmax-cloud.minRect.zmin);//cloud范围
				if(_S>S//如果_cloud范围大，可以放行（必须是严格大于，为了防止互为主云团现象发生）
					||
					(_S==S&&_cloud.avrgH<cloud.avrgH)//如果_cloud与cloud范围一样大，但_cloud较低，也放行（同样必须是严格低于，为了防止互为主云团现象发生）
					){
					//放行
				}else{
					continue;
				}

				//看_cloud是否可以作为cloud的主人
				//看_cloud与cloud范围是否搭界
				float cx,cz;
				cx=(cloud.xmin+cloud.xmax)/2;
				cz=(cloud.zmin+cloud.zmax)/2;
				float _cx,_cz;
				_cx=(_cloud.xmin+_cloud.xmax)/2;
				_cz=(_cloud.zmin+_cloud.zmax)/2;
				if(fabs(cx-_cx)<=0.5*(cloud.xmax-cloud.xmin)+0.5*(_cloud.xmax-_cloud.xmin)+1.5
					&&fabs(cz-_cz)<=0.5*(cloud.zmax-cloud.zmin)+0.5*(_cloud.zmax-_cloud.zmin)+1.5){//搭界
					//放行
				}else{
					continue;//跳过
				}
		/*		//看_cloud是否足够高
				//之所以不能过低，是因为离地面过近的建筑边缘提不准，其确定的主方向没有参考价值
				if(_cloud.avrgH-_cloud.minH>=1.5){//??????????由于去低面不准确，导致低地处把地面突起当成建筑，从而引起主方向判断错误
					                            //这里体现出去地面的准确性对建模的影响
					                            //就是由于去地面不充分
					
					//放行
				}else{
				
					continue;
				}*/
				//看_cloud中是否有某点与cloud中的某点很接近
				int count=0;
				int nv=(int)cloud.vlist.size();
				for(int k=0;k<nv;k++){
					Cv4&v=cloud.vlist[k];
					//求v所在的_cloud.IDgrid格
					int I,J;//v属于_cloud.IDgrid的(I,J)格
					I=(v.v[Z]-_IDgrid.zmin)/_IDgrid.zstepLen;
					J=(v.v[X]-_IDgrid.xmin)/_IDgrid.xstepLen;
					if(I<0||J<0||I>_IDgrid.m_IDListmat-1||J>_IDgrid.n_IDListmat-1)continue;
					//求_cloud.IDgrid的I,J格中所有点
					myvector<int> IDList=_IDgrid.IDListmat[I][J];//格中点的ID列表
					//用v与其中的点进行距离判断
					int nvID=(int)IDList.size();
					for(int u=0;u<nvID;u++){
						int ID=IDList[u];
						Cv4&_v=_cloud.vlist[ID];
						float d2=pow2(_v.v[X]-v.v[X])+pow2(_v.v[Z]-v.v[Z]);//这里必须用xz平面上的距离
						if(d2<1*1){
							count++;	
							if(count>=5)break;
						}
					}
					if(count>=5)break;
				}
				if(count>=5){
					//放行
				}else{
					continue;
				}
				//通过检验，将_cloud的ID号加入cloudIDList
				cloudIDList.push_back(j);
				
			}//得到cloudIDList
			int nID=(int)cloudIDList.size();
			if(nID==0){//如果cloudIDList空，即没有候选主人
				//将自己作为候选主人
				cloud.hostID=i;
				cloud.selfHost=true;
			
			}else{//cloudIDList不为空
				//在cloudIDList中找面积最大者
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
				}//得到maxS和ID
				cloud.hostID=ID;
				if(ID==-1){
					cout<<"ID==-1"<<endl;
					char c;
					cin>>c;
				}
			}//cloud的主人云团号cloud.hostID已得到

			
		}


	}

	//以上得到的各cloud的hostID只是一步hostID，接下来要考虑通过传递，对每个cloud找到它的根主人的ID作为其hostID
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			//求cloud的根host
			int hostID=cloud.hostID;//根主人的hostID
			while(1){
				Ccloud&_cloud=cloudList[hostID];
				if(_cloud.hostID!=hostID){
					hostID=_cloud.hostID;
				}else{
					break;
				}
			}//得到hostID;
			cloud.hostID=hostID;

			
		}


	}

}

inline void ridSelfInteract_polygenList_Ex(myvector<Cpolygen>&polygenList)
//去除polygenList中各多边形之间的相交
{
	for(int i=0;i<(int)polygenList.size();i++){
		Cpolygen&polygen=polygenList[i];
		bool polygenRemoved=false;
		
		//检测polygen与其之后的polygen是否相交
		for(int j=i+1;j<(int)polygenList.size();j++){
			Cpolygen&_polygen=polygenList[j];
			bool _polygenRemoved=false;
			//检测polygen与_polygen是否有相交边
			for(int k=0;k<(int)polygen.vlist.size();k++){
				float *v1=polygen.vlist[k].v;
				int knext=(k+1)%(int)polygen.vlist.size();
				float *v2=polygen.vlist[knext].v;	
				//看v1v2是否与_polygen某个边相交
				for(int u=0;u<(int)_polygen.vlist.size();u++){//由于要对_polygen.vlist进行删除操作，所以不能提前固化长度
					float *_v1=_polygen.vlist[u].v;
					int unext=(u+1)%(int)_polygen.vlist.size();
					float *_v2=_polygen.vlist[unext].v;
					//看_v1_v2是否与v1v2相交
					if(LineSegOverLineSeg(v1[X],v1[Z],v2[X],v2[Z],_v1[X],_v1[Z],_v2[X],_v2[Z])){//如果v1v2与_v1_v2相交
						//看polygen与_polygen谁短
						//求polygen的周长
						float SL=0;
						{	
							int nv=(int)polygen.vlist.size();
							for(int i=0;i<nv;i++){
								float *v=polygen.vlist[i].v;
								float *vn=polygen.vlist[(i+1)%nv].v;
								float d2=pow2(v[X]-vn[X])+pow2(v[Z]-vn[Z]);
								float d=sqrt(d2);
								SL+=d;
							}//得到SL
						}
						//求_polygen的周长
						float _SL=0;
						{	
							int nv=(int)_polygen.vlist.size();
							for(int i=0;i<nv;i++){
								float *v=_polygen.vlist[i].v;
								float *vn=_polygen.vlist[(i+1)%nv].v;
								float d2=pow2(v[X]-vn[X])+pow2(v[Z]-vn[Z]);
								float d=sqrt(d2);
								_SL+=d;
							}//得到_SL
						}
						if(SL>_SL){
							//删除_polygen
							_polygenRemoved=true;
							polygenList.erase(&polygenList[j]);
							j--;
							break;
						}else{
							//删除polygen
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
	//计算最小包围矩形面积
	const float S=(cloud.minRect.xmax-cloud.minRect.xmin)*(cloud.minRect.zmax-cloud.minRect.zmin);
	//将polygen.lineList中直线进行规则化
	int nline=(int)lineList.size();
	//开始规则化各直线
	for(int k=0;k<nline;k++){
		Cline&line=lineList[k];
		//用主方向边作为参考
		float d2=line.getD2_xz();
		//根据line长度不同，规则化策略不同
		//如果line长度较短，则规则化力度大
		//如果line长度较大，则规则化力度小
		if(d2<=5*5||S<10*10){//很短3.5*3.5
			float cosA=dot(line.dir,mainDir.v);
			if(cosA>=0.707||cosA<=-0.707){//A<=45||A>=135
				//平行
				veccopy(mainDir.v,line.dir);
				if(cosA<0){
					mul(-1,line.dir,line.dir);
				}
			}else if(-0.707<=cosA&&cosA<=0.707){//135>=A>=45
				//垂直
				float cross_rs[4];
				cross(mainDir.v,line.dir,cross_rs);
				if(cross_rs[Y]>0){//凸角
					//求左法向量
					float norm[4]={mainDir.v[Z],0,-mainDir.v[X],0};
					//将line.dir置为norm
					veccopy(norm,line.dir);
				}else{//凹角
					//求右法向量
					float norm[4]={-mainDir.v[Z],0,mainDir.v[X],0};
					//将line.dir置为norm
					veccopy(norm,line.dir);
				}	
			}
		}
		
		
	}
				
//*/

}

inline void adjustMainDirRefBigNeighbour(myvector<Ccloud>&cloudList){
	//对每个较小根主云团，如果有与之搭界的面积比它大得多的云团x，并且二者的主方向非常接近平行或反向平行或垂直，则将此根主云团的主方向设为与x的主云团一致
	//并且所有与此根主云团为主云团的云团的主方向也要相应修改
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			if(!cloud.selfHost)continue;
			bool&mainDirChanged=cloud.mainDirChanged;//必须用引用
			//cloud是根主云团
			//求cloud的面积
			float S=(cloud.minRect.xmax-cloud.minRect.xmin)*(cloud.minRect.zmax-cloud.minRect.zmin);
			if(S>80*80)continue;
			float maxEdgeLen=max(cloud.minRect.xmax-cloud.minRect.xmin,cloud.minRect.zmax-cloud.minRect.zmin);
			//cloud面积较小
			//为cloud找候选主人
			myvector<int> cloudIDList;//候选主人列表
			myvector<float> SList;//cloudIDList对应的S值列表
			for(int j=0;j<ncloud;j++){
				if(i==j)continue;
				Ccloud&_cloud=cloudList[j];	
				if(_cloud.removed)continue;
				//看_cloud范围是否比cloud范围大
				float _S=(_cloud.minRect.xmax-_cloud.minRect.xmin)*(_cloud.minRect.zmax-_cloud.minRect.zmin);//_cloud范围
				float _maxEdgeLen=max(_cloud.minRect.xmax-_cloud.minRect.xmin,_cloud.minRect.zmax-_cloud.minRect.zmin);
				
				if(_S>2*S||_maxEdgeLen>1.5*maxEdgeLen&&_S>S){//如果_cloud范围大，可以放行（必须是严格大于，为了防止互为主云团现象发生）
					//放行
				}else{
					continue;
				}
				//看_cloud与cloud范围是否搭界
				float cx,cz;
				cx=(cloud.xmin+cloud.xmax)/2;
				cz=(cloud.zmin+cloud.zmax)/2;
				float _cx,_cz;
				_cx=(_cloud.xmin+_cloud.xmax)/2;
				_cz=(_cloud.zmin+_cloud.zmax)/2;
				const float affectMargin=5;//30;//此值，可取主干道宽度，我们假设主干道将不同的楼群分离
				if(fabs(cx-_cx)<=0.5*(cloud.xmax-cloud.xmin)+0.5*(_cloud.xmax-_cloud.xmin)+affectMargin
					&&fabs(cz-_cz)<=0.5*(cloud.zmax-cloud.zmin)+0.5*(_cloud.zmax-_cloud.zmin)+affectMargin
					){//搭界
					//放行
				}else{
					continue;//跳过
				}
				//通过检验，将_cloud的ID号加入cloudIDList
				cloudIDList.push_back(j);
				SList.push_back(_S);
				
			}//得到cloudIDList
			int nID=(int)cloudIDList.size();
			if(nID==0){//如果cloudIDList空，即没有候选主人
				//cloud的主人保持不变
			
			}else{//cloudIDList不为空
				//对cloudIDList指示的云团进行筛选，筛选出那些主方向与cloud接近平行或反向平行或垂直的云团
				Cv4&mainDir=cloud.mainDir;
				float maxS=-inf;
				int index=-1;
				for(int j=0;j<nID;j++){
					Ccloud&_cloud=cloudList[cloudIDList[j]];
					Cv4&_mainDir=_cloud.mainDir;//_cloud的主方向
					//看_cloud的主方向是否与cloud的主方向接近平行或反向平行或垂直
					float cosA=dot(mainDir.v,_mainDir.v);
					if(cosA>=0.9848||cosA<=-0.9848||(cosA>=-0.1736&&cosA<=0.1736)){//10度
						float _S=SList[j];
						if(_S>maxS){
							maxS=_S;
							index=j;
						}
						
					
					}
				}//得到maxJ和index
				if(index==-1){
					//cloud的主人保持不变
					
				}else{
					Cv4&_mainDir=cloudList[cloudIDList[index]].mainDir;
					mainDir=_mainDir;
					mainDirChanged=true;
				}

			}


		}
	}
	//将如果某云团的主云团mainDirChanged，则其主方向要重新更新为与主云团一致
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			Cv4&mainDir=cloud.mainDir;
			//求cloud的主云团的mainDirChanged
			bool _mainDirChanged=cloudList[cloud.hostID].mainDirChanged;//cloud的主云团的mainDirChanged
			//如果_mainDirChanged，则cloud的mainDir要改为与cloud的主云团一致
			if(_mainDirChanged){
				//求host的主云团的mainDir
				Cv4&_mainDir=cloudList[cloud.hostID].mainDir;
				//让cloud的mainDir等于_mainDir
				mainDir=_mainDir;
			}

		}
		
	}
	



}