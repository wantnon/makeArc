//----------------------------------------------
//生成地面网格并删除地面点
//----------------------------------------------

inline void main_make_ground_grid_and_remove_ground_point(myvector<Cv4>&vlist,
												   float xmin,float xmax,float ymin,float ymax,float zmin,float zmax,
												   Cgroundgrid&GroundGrid)
{

	//房屋在x,z方向上的最大跨度
	float maxHouseSizeX=50;//x方向上的最大跨度
	float maxHouseSizeZ=50;//z方向上的最大跨度
	CgroundgridEx _GroundGrid;
	//建立矩形网格
	{
		float xstepLen=1;
		float zstepLen=1;
		float margin=1;//此margin=0才能保证航拍纹理映射完全精确，此处为了不出界，允许margin不为0，所以是有误差的，因为margin不能取大
		_GroundGrid.makeGrid(xmin-margin,xmax+margin,zmin-margin,zmax+margin,//范围
			xstepLen,zstepLen,//步长
			vlist//点云
			);
		//补充缺失元素
		while(1){
			int n=_GroundGrid.makeUp_once();
			if(n==0)break;
		}
		//标记阶跃和边缘
		_GroundGrid.markJump(0.5,0.5);
	}
	//标记种子点
	//假设每maxHouseSizeX*maxHouseSizeZ的区域都必定有地面点
	_GroundGrid.markSeed(maxHouseSizeX,maxHouseSizeZ);
	//标记地面点
	//对GroundGrid中的cell按连通性进行分类
	_GroundGrid.cncTest();
	//将GroundGrid.ijsetList中非地面ijset去掉
	_GroundGrid.getGroundijsetAndDestroyijsetList();
	//对vlist中的地面作标记
	_GroundGrid.markGround(vlist);
	//重建地面
	//对GroundGrid补充地面
	while(1){
		int n=_GroundGrid.makeUpGround_once();
		if(n==0)break;
	}
	//用一点周围较大范围内的高度平均值代替此点高度，即进行强力平滑
	_GroundGrid.Hsmooth_strong();
	//清除GroundGrid中各cell的IDList
	_GroundGrid.clearIDListForEachCell();
	//清除groundijset
	_GroundGrid.clearGroundijset();
	
	//------------------------------------删除比地面高不多的点
	{
		//如果一个点高于地面的距离小于一定高度，标为地面点
		//求地面矩形网格参数
		float zmin_g=_GroundGrid.zmin;
		float xmin_g=_GroundGrid.xmin;
		float zstepLen_g=_GroundGrid.zstepLen;
		float xstepLen_g=_GroundGrid.xstepLen;
		int m_mat_g=_GroundGrid.m_mat;
		int n_mat_g=_GroundGrid.n_mat;
		int nv=vlist.size();
		for(int i=0;i<nv;i++){
			Cv4&v=vlist[i];
			if(v.isGround)continue;
			//求v在_GroundGrid的哪个格
			int I,J;//v属于(I,J)格
			I=(v.v[Z]-zmin_g)/zstepLen_g;
			J=(v.v[X]-xmin_g)/xstepLen_g;
			if(I<0||I>=m_mat_g)continue;
			if(J<0||J>=n_mat_g)continue;
			//获得此格高度
			float H_g=_GroundGrid.mat[I][J].v[Y];
			//如果v[Y]高出H_g过小，则将v标记为删除
			if(v.v[Y]-H_g<=2){
				v.isGround=true;
			}
		}

					
	}
	//------------------------------------删除地面点（必须放在最后）
	//将vlist中标记为了isGround=true的点删除（注意，此时vlist与GroundGrid已经不一致）
	{
		int count=0;
		int nv=(int)vlist.size();
		for(int i=0;i<nv;i++){
			Cv4&v=vlist[i];
			if(v.isGround){
				count++;
			}else{
				if(count!=0)vlist[i-count]=v;
			}
		}
		vlist.resize(nv-count);
	}
	//---------------用_GroundGrid初始化GroundGrid
	GroundGrid.init(_GroundGrid);




}
