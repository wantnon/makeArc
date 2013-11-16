//-------------------------------------------------------
//标记阶跃点
//-------------------------------------------------------
inline void main_mark_step_point(myvector<Cv4>&vlist,float xmin,float xmax,float ymin,float ymax,float zmin,float zmax)
//标记阶跃点，即填充vlist各元素的isEdgePoint域
{
	//生成索引网格（局部作用域网格）
	CIDgrid IDgrid;//索引网格
	IDgrid.makeIDgrid(xmin,xmax,zmin,zmax,//范围
		0.5,0.5,//步长
		vlist//点云
		);
	//进行阶跃检测
	{
		int nv=(int)vlist.size();
		for(int i=0;i<nv;i++){
			Cv4&v=vlist[i];
			//求得v邻近点索引列表（由于v也是vlist中的点，所以包含v自身）
			myvector<int> IDList;//v邻近点索引列表
			IDgrid.getIDList_oc(v.v[X],v.v[Z],IDList);
			//IDList中各点最大和最小高度值
			float hmin=inf;
			float hmax=-inf;
			int nvID=(int)IDList.size();
			for(int j=0;j<nvID;j++){
				float h=vlist[IDList[j]].v[Y];
				if(h<hmin)hmin=h;
				if(h>hmax)hmax=h;
			}//得到hmin和hmax，均非inf或-inf
			if(hmax-hmin>0.8){
				v.isEdgePoint=true;
			}	
		}
	}


}