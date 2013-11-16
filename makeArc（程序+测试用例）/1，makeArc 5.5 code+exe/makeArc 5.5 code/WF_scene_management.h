//---------------------
//场景管理
//---------------------

inline void main_scene_management(myvector<Ccloud>&cloudList,CqTree&qTree,CgroundBlock&groundBlock,
								  float xmin,float xmax,float zmin,float zmax,float ymin,float ymax,
								  const CTextureManager&textureManager){
	//建立房屋四叉树
	{
		qTree.makeqTree(xmin,xmax,zmin,zmax,ymin,ymax,cloudList);
		qTree.setTextureRoof(textureManager.getTex(0));
		qTree.setTextureWall(textureManager.getTex(1));
	}

	//建立地形四叉树
	{
		//求groundBlock.GroundGrid的高度范围
		int m=groundBlock.GroundGrid.m_mat;
		int n=groundBlock.GroundGrid.n_mat;
		float __ymin=inf;
		float __ymax=-inf;
		for(int i=0;i<m;i++){
			for(int j=0;j<n;j++){
				Cgroundcell&cell=groundBlock.GroundGrid.mat[i][j];
				if(cell.h>__ymax)__ymax=cell.h;
				if(cell.h<__ymin)__ymin=cell.h;
			}
		}//得到__ymin,__ymax
		//建立地形四叉树
		groundBlock.makeGroundBlock(xmin,xmax,zmin,zmax,
			__ymin,__ymax,//这个用的是GroundGrid的高度范围，而不是用整个点云的高度范围
			8,10000);


		groundBlock.setTexture(textureManager.getTex(0));
	}
	



}