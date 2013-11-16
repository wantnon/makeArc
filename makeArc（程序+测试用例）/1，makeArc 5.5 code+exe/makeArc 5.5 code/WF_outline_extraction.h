//----------------------------------------
//提取轮廓
//----------------------------------------
inline void getLooseOutline(Cpatch&,CIDoutline&);
inline void IDloosePolygenToIDpolygen(CIDloosePolygen&,myvector<CIDpolygen>&);
inline void getLooseOutline_inn(CIDTriangle*,CIDloosePolygen&);

inline void main_outline_extraction(myvector<Ccloud>&cloudList)
{


	//下面为优化内存，串行处理各云团
	{
		int ncloud=(int)cloudList.size();
		for(int i_c=0;i_c<ncloud;i_c++){
			Ccloud&cloud=cloudList[i_c];
			if(cloud.removed)continue;
			Cpatch patch;
			CIDoutline IDoutline;
			//为每个云团进行三角剖分，得到的结果作为patchList的一个元素
			{
				//对cloudList[i]进行三角剖分
				//三角剖分过程中不会引入超级三角形顶点，因为在Delaunay函数之中已经加了去掉超级三角形及其三个顶点的语句
				Delaunay(cloud.vlist,cloud.xmin,cloud.xmax,cloud.zmin,cloud.zmax,patch.pIDtriList);
			}

				
			//删除patch中有过长的边的三角形
			{
				for(int j=0;j<(int)patch.pIDtriList.size();j++){
					CIDTriangle*pIDtri=patch.pIDtriList[j];
					if(!pIDtri->removed){
						//如果pIDtri含有过长的边，则将pIDtri标为removed
						float *v[3];
						v[0]=cloud.vlist[pIDtri->vID[0]].v;
						v[1]=cloud.vlist[pIDtri->vID[1]].v;
						v[2]=cloud.vlist[pIDtri->vID[2]].v;
						for(int k=0;k<=2;k++){
							//计算第k条边的长度的平方
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
	
			//将标记为removed的三角形删除
			//但是删除前必须保证所有未removed的三角形都不存在指向removed的三角形的指针（否则会产生野指针）
			//因此需要在删除之前先遍历一次各三角形，将它们的指向removed的三角形的指针改为NULL
			{
				//将patch中未removed的三角形的指向removed的三角形指针置为NULL
				int nIDtri=(int)patch.pIDtriList.size();
				for(int j=0;j<nIDtri;j++){
					CIDTriangle*pIDtri=patch.pIDtriList[j];
					if(pIDtri->removed)continue;
					//pIDtri是未removed的三角形，检查它的三个邻居指针，看是否有指向removed=true的三角形的，如果有，将其置为NULL
					if(pIDtri->p[0]!=NULL&&pIDtri->p[0]->removed)pIDtri->p[0]=NULL;
					if(pIDtri->p[1]!=NULL&&pIDtri->p[1]->removed)pIDtri->p[1]=NULL;
					if(pIDtri->p[2]!=NULL&&pIDtri->p[2]->removed)pIDtri->p[2]=NULL;
					
				}
				//开始删除removed三角形
				//删除patch中标记为removed的三角形，同时对指针列表作紧缩处理（这样可以避免大规模内存移动，比直接用erase效率高）
				int count=0;//统计removed的三角形个数同时也作为紧缩操作的向前填充的偏移量
				for(j=0;j<nIDtri;j++){
					CIDTriangle*pIDtri=patch.pIDtriList[j];
					//如果当前三角形是removed的，则delete掉，并计数
					//如果当前三角形是非removed的，则向前填充
					if(pIDtri->removed){//是removed
						//delete并计数
						delete pIDtri;
						count++;
					}else{//未removed
						//向前填充
						if(count!=0)patch.pIDtriList[j-count]=pIDtri;
					}
				}//此时removed的三角形删除完毕，非removed的三角形向前填充（紧缩）也完毕
				//下面只需将pIDtriList中后面的一连串空指针删掉
				//空指针的个数就是当初removed==true三角形的个数，这个数值目前已存到了count中
				//将pIDtriList中的末尾count个元素删除，即将其长度重设为nIDtri-count
				patch.pIDtriList.resize(nIDtri-count);
			}
		/*	//为每个云团生成颜色
			{
				init4(cloud.color,rand()%10*0.1,rand()%10*0.1,rand()%10*0.1,1);
			}
		*/	
			//对patch提取松散轮廓
			{
				getLooseOutline(patch,IDoutline);
			}
			//由每个IDoutline的IDloosePolygenList域生成IDPolygenList域（顺便删除了包含顶点数过少的IDpolygen，并且保证每个IDpolygen中顶点互异）
			{
				int nIDloosePolygen=(int)IDoutline.IDloosePolygenList.size();
				for(int j=0;j<nIDloosePolygen;j++){
					CIDloosePolygen&IDloosePolygen=IDoutline.IDloosePolygenList[j];
					IDloosePolygenToIDpolygen(IDloosePolygen,IDoutline.IDpolygenList);
			
				}
			}
	
			//将IDpolygen转化为polygen（将Y坐标都赋为0）
			{
				int nIDpolygen=(int)IDoutline.IDpolygenList.size();
				IDoutline.polygenList.resize(nIDpolygen);
				
				for(int j=0;j<nIDpolygen;j++){
					CIDpolygen&IDpolygen=IDoutline.IDpolygenList[j];
					Cpolygen&polygen=IDoutline.polygenList[j];
					
					
					//将IDpolygen中的角点坐标添加到polygen中
					int nvID=(int)IDpolygen.vIDList.size();
					for(int k=0;k<nvID;k++){
						Cv4 v=cloud.vlist[IDpolygen.vIDList[k]];
						v.v[Y]=0;
						Cv4Ex vEx;
						veccopy(v.v,vEx.v);
						polygen.vlist.push_back(vEx);
						//将cloud.vlist中此点标记为isOutlinePoint
						cloud.vlist[IDpolygen.vIDList[k]].isOutlinePoint=true;
						
					}
					
				}
			
				//如果cloud的polygenList为空，则将其标为removed（保证所有的cloud的polygenList都非空）
			
				if((int)IDoutline.polygenList.size()==0){
					cloud.removed=true;
					continue;
				}
				//将IDoutline.polygenList拷贝给cloud.polygenList
				cloud.polygenList=IDoutline.polygenList;
			}
			

		}
	
	}


}


inline void getLooseOutline(Cpatch&patch,CIDoutline&IDoutline)
//提取patch的松散轮廓，填充到IDoutline的IDloosePolygenList域
{
	//遍历patch的各多边形，每找到一个非removed且非visited2的，就由它向下递归
	int nIDtri=(int)patch.pIDtriList.size();
	for(int i=0;i<nIDtri;i++){
		CIDTriangle*pIDtri=patch.pIDtriList[i];
		if(!pIDtri->removed&&!pIDtri->visited2){
			//求patch的包含pIDtri的连通分支的松散多边形轮廓
			IDoutline.IDloosePolygenList.grow();
			int n=(int)IDoutline.IDloosePolygenList.size();
			getLooseOutline_inn(pIDtri,IDoutline.IDloosePolygenList[n-1]);
			
		}
	}
}

inline void IDloosePolygenToIDpolygen(CIDloosePolygen&IDloosePolygen,myvector<CIDpolygen>&IDpolygenList)
//计算IDloosePolygen对应的IDpolygen（可能有多个），将它们加入到IDplygenList中（顺便已删除包含顶点数过少的IDpolygen，但如果IDpolygenList中只含一个IDpolygen,则不删除）
//另外还要保证每个IDpolygen中不含有重复顶点（即不含桥点）

{
	int nIDedge=(int)IDloosePolygen.IDedgeList.size();
	if(nIDedge==0)return;
	//如果IDloosePolygen不为空
	//反复从IDloosePolygen中提取多边形
	while(1){
		//从IDloosePolygen中找一个未added且未removed的边作为初始边
		CIDedge*pIDedge=NULL;
		for(int i=0;i<nIDedge;i++){
			CIDedge&IDedge=IDloosePolygen.IDedgeList[i];
			if(!IDedge.added){
				pIDedge=&IDedge;
				break;
			}
		}//得到pIDedge
		if(pIDedge==NULL){//说明已不存在未added或未removed的边
			//提取结束
			break;
		}
		//下面开始提取多边形，将多边形列表新增一个元素
		IDpolygenList.grow();
		CIDpolygen&IDpolygen=IDpolygenList[(int)IDpolygenList.size()-1];//当前要提取的多边形
		//以pIDedge作为初始边
		int vID1=pIDedge->ID1;
		int vID2=pIDedge->ID2;
		IDpolygen.vIDList.push_back(vID1);
		IDpolygen.vIDList.push_back(vID2);
		pIDedge->added=true;
		int curLastvID=vID2;
		//反复搜索剩余各边，看哪个边的起点能与curLastvID接上
		while(curLastvID!=vID1){
			bool find=false;
			for(int i=0;i<nIDedge;i++){
				CIDedge&IDedge=IDloosePolygen.IDedgeList[i];
				if(IDedge.added)continue;//跳过已加入的边
				if(IDedge.ID1==curLastvID){
					//将IDedge的终点加入到IDpolygen
					IDpolygen.vIDList.push_back(IDedge.ID2);
					IDedge.added=true;//标记为已加入
					curLastvID=IDedge.ID2;
					find=true;
					break;
				}	
			}
			if(!find){//如果没能找到能与curLastvID接上的边
				cout<<"error IDloosePolygenToIDpolygen"<<endl; 
				//将vID1加入IDpolygen，然后跳出循环
				//因为跳出后要删除最后一个元素，所以这里将vID1加入，是为了抵消删除
				IDpolygen.vIDList.push_back(vID1);
				break;


			}
		}//得到IDpolygen,但是起点和终点重复了
		IDpolygen.vIDList.pop_back();//得到IDpolygen
	

	}

	

	//去掉IDpolygenList中含有较少顶点数的IDpolygen
	for(int i=0;i<(int)IDpolygenList.size();i++){
		CIDpolygen&IDpolygen=IDpolygenList[i];
		if((int)IDpolygen.vIDList.size()<15){//只能取15,取30会出现误删
			IDpolygenList.erase(&IDpolygenList[i]);
			i--;
		}
	}
	
}


inline void getLooseOutline_inn(CIDTriangle*pIDtri,CIDloosePolygen&IDloosePolygen)
//传入的pIDtri一定不为NULL,也一定不为visited2
{	
	pIDtri->visited2=true;
	//看pIDtri的各邻接指针，如果某个邻接指针为空或者指向的三角形为removed，则将此边加入到IDloosePolygen
	for(int i=0;i<=2;i++){
		CIDTriangle*pAdjIDtri=pIDtri->p[i];
		//考察pIDtri的i号邻居pAdjIDtri
		if(pAdjIDtri==NULL||(pAdjIDtri!=NULL&&pAdjIDtri->removed)){//说明pIDtri的i号边是边界边
			//将pIDtri的第i号边加入到IDloosePolygen
			CIDedge IDedge;
			IDedge.setvID(pIDtri->vID[i],pIDtri->vID[(i+1)%3]);
			IDloosePolygen.IDedgeList.push_back(IDedge);
	
		}else{//如果pIDtri的i号边不是边界边
			if(pAdjIDtri->visited2){//如果相邻三角形visited2
				continue;
			}else{//如果相邻三角形未visited2
				//由此相邻三角形向下递归
				getLooseOutline_inn(pAdjIDtri,IDloosePolygen);
			}

		
		}
	}
}
