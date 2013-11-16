//----------------------------------------------
//建屋顶和墙壁面
//----------------------------------------------
inline void makeOOBBgridForCloud(Ccloud&);
inline void markEmptyCellAndNeedcutCell(Ccloud&);
inline float cal_topH_useInnerLayerID(const Ccloud&);
inline void calUVForWallPoint(const float[4],const float[4],const float[4],const float,float[2]);

inline void main_make_roof_and_wall(myvector<Ccloud>&cloudList,const Cgroundgrid&GroundGrid,bool ignoreRidTree_baseOnCornerMark,bool useRealRoof){
	cout<<"::生成网格"<<endl;
	//对每个cloud生成网格
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			//为cloud建平行于主方向的网格
			{
				makeOOBBgridForCloud(cloud);
			}
			//补充缺失元素（进行多遍，填满为止）
			{
				while(1){
					int n=cloud.grid.makeUp_once();
					if(n==0)break;
				}
			}
			//让各格的边界点高度与中心点高度相同
			{
				cloud.grid.initOcH();
				           
			}
			//将各格边界点及中心点坐标变换回世界坐标系
			{
				cloud.grid.transformBack();
			}
		}	
	}
	//-------------------------------------------释放部分内存
	cout<<"::释放vlist"<<endl;
	//删除各cloud的vlist（由于removed的也要删除，所以此段要单列）
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			//if(cloud.removed)continue;//removed也要删除
			cloud.vlist.swap(myvector<Cv4>());
		}
	}
	//----------------------------------------------------------
	cout<<"::生成屋顶"<<endl;
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			//标记empty格（在轮廓外）和needcut格（轮廓穿过）
			{
				markEmptyCellAndNeedcutCell(cloud);
			}
			//去树
			if(ignoreRidTree_baseOnCornerMark){
				//跳过去树
			}else{
				//顶面抚平
				cloud.grid.roofsmooth(false);
				//计算法向量
				cloud.grid.calNorms();
				//判断角点cell
				cloud.grid.markCorner();
				//如果cloud的gird中角点cell的个数超过一定比例，则标记cloud为树
				cloud.removed=cloud.grid.isTree();
				//将所有角点cell的标记去掉
				cloud.grid.UnmarkCorner();
				//如果removed，则跳过（因为刚才cloud可能刚刚被判断为removed）
				if(cloud.removed)continue;
			}
			if(useRealRoof){
			/*	//保护性平滑
				{
					//进行平滑
					{
						//高度值平滑
						cloud.grid.Hsmooth(10);
						//重新填充各格的边界点
						//	cloud.grid.updateOcH();
						//顶面抚平
						cloud.grid.roofsmooth(false);	
					}
					//计算法向量
					{
						cloud.grid.calNorms();
						
					}
					//判断角点cell
					{
						cloud.grid.markCorner();
					}
					//再次进行平滑
					{
						//高度值平滑
						cloud.grid.Hsmooth(1);
						//重新填充各格的边界点
						//	cloud.grid.updateOcH();
						//顶面平滑
						cloud.grid.roofsmooth(false);	
					}
					//重新计算法向量
					{
						cloud.grid.calNorms();
					}
					
				}*/
				//简单平滑
				{
					//进行平滑
					{
						//高度值平滑
						cloud.grid.Hsmooth(2);
						//重新填充各格的边界点
						//	cloud.grid.updateOcH();
						//顶面平滑
						cloud.grid.roofsmooth(false);	
					}
					//重新计算法向量
					{
						cloud.grid.calNorms();
					}

				}
			}else{//!useRealRoof
				//计算各格的innerLayerID
				{
					cloud.grid.calInnerLayerID();
				}
				//计算cloud.topH（根据使用innerLayerID）
				{
					cloud.topH=cal_topH_useInnerLayerID(cloud);
					
				}
				//按innerLayerID填充高度
				{
					cloud.grid.fillH_innerLayerID(cloud.bottomH,cloud.topH);
					
				}
				//顶面抚平
				{
					//中为中心格高度发生了本质改变，所以要重新初始化边界格高度
					cloud.grid.initOcH();
					//	//标记阶跃和边缘
					//	cloud.grid.markJump();
					//顶面抚平
					cloud.grid.roofsmooth(false);
				}
				//计算各格法向量
				{
					cloud.grid.calNorms();
					
				}
			}

		}

	}
	


	//-------------------------------------------------------------------------------------------------
	cout<<"::制作墙壁"<<endl;
	//制作墙壁面（调整polygenList高度并生成polygenList_dn。此二者即描述了墙壁面）
	{

		//求地面矩形网格参数
		float zmin_g=GroundGrid.zmin;
		float xmin_g=GroundGrid.xmin;
		float zstepLen_g=GroundGrid.zstepLen;
		float xstepLen_g=GroundGrid.xstepLen;
		int m_mat_g=GroundGrid.m_mat;
		int n_mat_g=GroundGrid.n_mat;
		//开始制作墙壁面
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;

			//将cloud.polygenList中各点Y值设为矩形网格的插值高度		
			int npolygen=(int)cloud.polygenList.size();
			for(int j=0;j<npolygen;j++){
				Cpolygen&polygen=cloud.polygenList[j];
				//将polygen中各点Y值设为矩形网格的插值高度
				int nv=(int)polygen.vlist.size();
				for(int k=0;k<nv;k++){
					Cv4Ex&v=polygen.vlist[k];
					v.v[Y]=cloud.grid.getH_vIsWorldPoint(v.v);
				}
			}

			//将cloud.polygenList_dn中各点Y值设为地面矩形网格的插值高度
			cloud.polygenList_dn=cloud.polygenList;//拷贝polygenList为polygenList_dn
			int npolygen_dn=(int)cloud.polygenList_dn.size();
			for(j=0;j<npolygen_dn;j++){
				Cpolygen&polygen_dn=cloud.polygenList_dn[j];
				int nv=(int)polygen_dn.vlist.size();
				for(int k=0;k<nv;k++){
					Cv4Ex&v=polygen_dn.vlist[k];
					v.v[Y]=GroundGrid.getH(v.v);	
				}
			}

			//为polygenList和polygenList_dn的各polygen的各顶点计算贴图坐标
			{
				const float zoomk=0.05;
				const float O[4]={0,0,0,1};//原点
				for(j=0;j<npolygen_dn;j++){
					Cpolygen&polygen=cloud.polygenList[j];
					Cpolygen&polygen_dn=cloud.polygenList_dn[j];
					int nv=(int)polygen.vlist.size();
					for(int k=0;k<nv;k++){
						Cv4Ex&vup=polygen.vlist[k];
						Cv4Ex&vdn=polygen_dn.vlist[k];
						//求vup和vdn的前后贴图坐标
						{
							//求vup的前贴图坐标
							calUVForWallPoint(O,vup.v,vup.normf,zoomk,vup.uf);
							//求vup的后贴图坐标
							calUVForWallPoint(O,vup.v,vup.normn,zoomk,vup.un);
							//求vdn的前贴图坐标
							calUVForWallPoint(O,vdn.v,vdn.normf,zoomk,vdn.uf);
							//求vdn的后贴图坐标
							calUVForWallPoint(O,vdn.v,vdn.normn,zoomk,vdn.un);
						}
					}
				}
			}

		}
	}

	cout<<"::墙壁切割屋顶"<<endl;

	//将各cloud中的各cell转化为多边形集合
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			cloud.grid.polygenlize();
		}
	}

	//为每个三角多边形计算法向量（此过程只能在所有polygen尚为三角形时调用）
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			cloud.grid.calTriPolygenNorm();
		}

	}
	//用轮廓去切割needcut==true的cell中的每个多边形
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			myvector<Cpolygen>&polygenList=cloud.polygenList;
			int npolygen=(int)polygenList.size();
			for(int j=0;j<npolygen;j++){
				Cpolygen&polygen=polygenList[j];
				int nv=(int)polygen.vlist.size();
				for(int k=0;k<nv;k++){
					float *v1=polygen.vlist[k].v;
					float *v2=polygen.vlist[(k+1)%nv].v;
					//用v1v2去切割所有needcut==true的cell中的多边形
					cloud.grid.cutNeedcutCells(v1,v2);
				}
			}
		}
	}
	//对needcut==true的cell判断其每个多边形的重心是否落在轮廓内
	{
		int ncloud=(int)cloudList.size();
		for(int u=0;u<ncloud;u++){
			Ccloud&cloud=cloudList[u];
			if(cloud.removed)continue;
			Cgrid&grid=cloud.grid;
			myvector<Cpolygen>&polygenList=cloud.polygenList;
			int m=grid.m_mat;
			int n=grid.n_mat;
			for(int i=0;i<m;i++){
				for(int j=0;j<n;j++){
					Ccell&cell=*(grid.mat[i][j]);
					if(!cell.needcut)continue;
					int npolygen=(int)cell.polygenList.size();
					for(int k=0;k<npolygen;k++){
						Cpolygen&polygen=cell.polygenList[k];
						//求polygen的重心
						float c[4];
						polygen.cal_c(c);
						//判断c是否在polygenList中
						bool isin=isPointInPolygenList_reduced(c[X],c[Z],polygenList);
						if(!isin){
							polygen.removed=true;
						}


					}

					
				}
			}

		}

	}
	//--------------------------------------释放部分内存
	cout<<"::释放部分内存"<<endl;
	//删除各cloud的各cell的polygenList中标记为removed的多边形
	{
		int ncloud=(int)cloudList.size();
		for(int u=0;u<ncloud;u++){
			Ccloud&cloud=cloudList[u];
			if(cloud.removed)continue;
			Cgrid&grid=cloud.grid;
			myvector<Cpolygen>&polygenList=cloud.polygenList;
			int m=grid.m_mat;
			int n=grid.n_mat;
			for(int i=0;i<m;i++){
				for(int j=0;j<n;j++){
					Ccell&cell=*(grid.mat[i][j]);
					if(!cell.needcut)continue;
					int npolygen=(int)cell.polygenList.size();
					int count=0;
					for(int k=0;k<npolygen;k++){
						Cpolygen&polygen=cell.polygenList[k];
						if(polygen.removed){
							count++;
						}else{
							if(count!=0){
								cell.polygenList[k-count]=polygen;
							}
						}
					}
					cell.polygenList.resize(npolygen-count);
					//此时，虽然后count个元素的静态成员还占用空间，但其中的动态列表内容已清空，所以占用空间并不很大。
				}
			}

		}

	
	}

	//--------------------------------------------------------------------------------


	//删除empty的cell
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			cloud.grid.removeEmptyCell();
		}
	
	
		
	}
	//各cell的IDList
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			cloud.grid.clearIDListForEachCell();
		}
	
	
		
	}
	//将removed的cloud的grid.mat清除
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed){
				cloud.grid.clearMat();

			}
		}
	}

}




//----------------------------------------------------------------------------------
inline void makeOOBBgridForCloud(Ccloud&cloud)
//为cloud建立OOBB网格（与cloud的主方向平行的网格）
{
	//获得主方向坐标系原点
	float c[4]={0,0,0,1};
	//制作世界Y轴
	float Yvec[4]={0,1,0,0};
	//获得mainDir
	float mainDir[4];
	veccopy(cloud.mainDir.v,mainDir);
	//制作xz平面内与mainDir垂直的轴
	float mainT[4];
	cross(mainDir,Yvec,mainT);
	//制作由主方向系向世界坐标系转化的矩阵
	float m_locToWorld[16]={
		mainDir[X],mainDir[Y],mainDir[Z],0,//第一列
			Yvec[X],Yvec[Y],Yvec[Z],0,//第二列
			mainT[X],mainT[Y],mainT[Z],0,//第三列
			c[X],c[Y],c[Z],1//第四列
	};
	//存到cloud.grid
	matcopy(m_locToWorld,cloud.grid.m_locToWorld);
	//制作由世界坐标系向主方向坐标系转化的矩阵
	float m_worldToLoc[16];
	inv(m_locToWorld,m_worldToLoc);
	//存到cloud.grid
	matcopy(m_worldToLoc,cloud.grid.m_worldToLoc);
	//将cloud中所有点转化到其主方向坐标系
	myvector<Cv4> _vlist;//存转化后的结果
	{
		int nv=(int)cloud.vlist.size();
		for(int j=0;j<nv;j++){
			Cv4 v=cloud.vlist[j];
			//将v转化到主方向坐标系
			prodv(m_worldToLoc,v.v,v.v);
			//将v存到_vlist
			_vlist.push_back(v);
		}//得到_vlist
	}
	//求_vlist范围
	float xmin,xmax,zmin,zmax;
	{
		//求点云范围
		xmin=inf;
		xmax=-inf;
		zmin=inf;
		zmax=-inf;
		int nv=(int)_vlist.size();
		for(int i=0;i<nv;i++){
			Cv4 v=_vlist[i];
			if(!v.isOutlinePoint)continue;//只考虑isOutlinePoint
			float x=v.v[X];
			float z=v.v[Z];
			if(x<xmin)xmin=x;
			if(x>xmax)xmax=x;
			if(z<zmin)zmin=z;
			if(z>zmax)zmax=z;
		}//得到xmin,xmax,zmin,zmax
		const float margin=3;//4;//margin留大一点儿，为了保证这个范围也能覆盖住cloud.polygenList
		//（因为cloud.polygenList与cloud.vlist的范围可能因平滑和规则化而发生些许偏离）
		xmin-=margin;
		xmax+=margin;
		zmin-=margin;
		zmax+=margin;
	}
	
	
	//对_vlist建立网格
	//因为现在采用了屋顶与实际无关的方法，所以本不用再使用_vlist建网格了（因此计算_vlist也是多余）
	//但之所以这里还要计算_vlist并用它来建网格，只是为了后一步的去树
	float xstepLen=1;
	float zstepLen=1;
	float margin=max(xstepLen,zstepLen);
	cloud.grid.makeGrid(xmin-margin,xmax+margin,zmin-margin,zmax+margin,//范围
		xstepLen,zstepLen,//步长
		_vlist//点云
		);
	
	
}

inline void markEmptyCellAndNeedcutCell(Ccloud&cloud)
//标记empty格（在轮廓外）和needcut格（轮廓穿过）
{
	Cgrid&grid=cloud.grid;
	myvector<Cpolygen>&polygenList=cloud.polygenList;
	int m=grid.m_mat;
	int n=grid.n_mat;
	for(int i=0;i<m;i++){
		for(int j=0;j<n;j++){
			Ccell&cell=*(grid.mat[i][j]);
			//获得cell的四角顶点坐标
			//   v0------v3
			//   |        |
			//   |    v4  |
			//   |        |
			//   v1------v2     
			float *v0=cell.vlup;
			float *v1=cell.vldn;
			float *v2=cell.vrdn;
			float *v3=cell.vrup;
			float *v4=cell.v;
		//	float *v5=cell.vup;
		//	float *v6=cell.vdn;
		//	float *v7=cell.vlt;
		//	float *v8=cell.vrt;
			//判断v0,v1,v2,v3,v4是否在轮廓内
			bool isin0,isin1,isin2,isin3,isin4;
		//	bool isin5,isin6,isin7,isin8;
			isin0=isPointInPolygenList_reduced(v0[X],v0[Z],polygenList);
			isin1=isPointInPolygenList_reduced(v1[X],v1[Z],polygenList);
			isin2=isPointInPolygenList_reduced(v2[X],v2[Z],polygenList);
			isin3=isPointInPolygenList_reduced(v3[X],v3[Z],polygenList);
			isin4=isPointInPolygenList_reduced(v4[X],v4[Z],polygenList);
		//	isin5=isPointInPolygenList_reduced(v5[X],v5[Z],polygenList);
		//	isin6=isPointInPolygenList_reduced(v6[X],v6[Z],polygenList);
		//	isin7=isPointInPolygenList_reduced(v7[X],v7[Z],polygenList);
		//	isin8=isPointInPolygenList_reduced(v8[X],v8[Z],polygenList);
			//将四角顶点都落在轮廓外者标为empty
			//将四角顶点有落在轮廓外也有落在轮廓内者标为needcut
			if(!isin0&&!isin1&&!isin2&&!isin3&&!isin4
		//		&&!isin5&&!isin6&!isin7&&!isin8
				){
				//全都在外，标为empty
				cell.empty=true;
			}else{
				//到此支已说明已至少有一个在轮廓内，于是只要有一个不在内，就是内外都有
				if(!isin0||!isin1||!isin2||!isin3||!isin4
		//			||!isin5||!isin6||!isin7||!isin8
				){
					//内外都有，标为needcut
					cell.needcut=true;
				}
			}
			
		}
	}
}

inline float cal_topH_useInnerLayerID(const Ccloud&cloud)
//计算cloud的topH，在前端返回
{
	
	//求所有innerLayerID==maxInnerLayerID的cell的高度的平均值
	float topH;
	float maxH=-inf;
	int count=0;
	const Cgrid&grid=cloud.grid;
	int m=grid.m_mat;
	int n=grid.n_mat;
	for(int i=0;i<m;i++){
		for(int j=0;j<n;j++){
			Ccell&cell=*(grid.mat[i][j]);
			if(cell.innerLayerID==grid.maxInnerLayerID){
				if(cell.v[Y]>maxH){
					maxH=cell.v[Y];
					count++;
				}
			}
		}
	}//得到maxH
	if(count==0){//即maxH==-inf
		topH=cloud.bottomH;
	}else{
		topH=maxH;
	}
	return topH;
}
inline void calUVForWallPoint(const float O[4],const float v[4],const float norm[4],const float zoomk,float u[2])
//为墙壁点计算纹理坐标
//O为原点坐标，v为墙壁点坐标，norm为墙壁点法向量，zoomk为缩放因子，u为算得的纹理坐标
{
	//        O
	//        |
	//        |
	//        |    u0
	// line---*----<----* v
	//       _O   dir   |
	//                 \|/
	//                  norm
	//
	//求过v且与norm垂直的直线
	float dir[4]={-norm[Z],0,norm[X],0};//line的方向向量
	Cline line;
	line.makeLine(v,dir);
	//求原点在line上的投影
	float _O[4];//原点在line上的投影
	line.getProjOfPoint_xz(O,_O);
	//求v到_O的有向距离
	//_Ov=d*line.dir，所以要求d，只须用_Ov的某一个分离去除以line.dir的相应分量
	//不过为了减少误差，可以转用合比(_Ov[X]+_Ov[Z])/(line.dir[X]+line.dir[Z])
	float _Ov[4];
	sub(v,_O,_Ov);
	float d=(_Ov[X]+_Ov[Z])/(line.dir[X]+line.dir[Z]);
	//v的纹理坐标分量u0等于d的相反数
	float u0=-d;
	//v的纹理坐标分量u1等于v[Y]
	float u1=v[Y];
	//对(u0,u1)进行放缩
	u0*=zoomk;
	u1*=zoomk;
	//存结果
	u[0]=u0;
	u[1]=u1;


}











