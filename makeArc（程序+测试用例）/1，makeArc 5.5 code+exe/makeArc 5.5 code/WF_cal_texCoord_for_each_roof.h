//------------------------
//计算各屋顶的纹理坐标
//------------------------
inline void main_cal_texCoord_for_each_roof(myvector<Ccloud>&cloudList,
											float xmin,float xmax,float zmin,float zmax,
											float offsetus,float offsetut){

	//为每个格中的顶点及每个多边形填充贴图坐标
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			cloud.grid.calTexCoord(xmin,xmax,zmin,zmax,offsetus,offsetut);
		}

	}
}