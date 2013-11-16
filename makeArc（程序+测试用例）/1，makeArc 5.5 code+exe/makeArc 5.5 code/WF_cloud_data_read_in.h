//------------------------------------
//点云数据的读取
//------------------------------------
inline bool main_cloud_data_read_in(const char *fileName,
							 myvector<Cv4>&vlist,
							 float&xmin,float&xmax,float&ymin,float&ymax,float&zmin,float&zmax)
//其中经历坐标调整调整
{
	//读取数据到vlist
	{
		//打开文件
		FILE  *infile;
		infile=fopen(fileName,"r");
		if(infile==NULL)
		{ 
			cout<<"Open "<<fileName<<" failed!"<<endl; 
			return false;
		}
		//读取数据
		while(1){
			float x,y,z;
			if(fscanf(infile,"%f",&x)==EOF)break;
			if(fscanf(infile,"%f",&y)==EOF)break;
			if(fscanf(infile,"%f",&z)==EOF)break;
			Cv4 v;
			init4(v.v,x,z,y,1);//注意，因为点云数据中z表示高度，所以将数据中的(x,y)作为世界空间的(x,z)，这样更符合opengl习惯
			vlist.push_back(v);
		}
		//关闭文件
		fclose(infile);
	}
	//求点云范围xmin,xmax,ymin,ymax,zmin,zmax
	{
		//求点云范围
		xmin=inf;
		xmax=-inf;
		zmin=inf;
		zmax=-inf;
		ymin=inf;
		ymax=-inf;
		int nv=(int)vlist.size();
		for(int i=0;i<nv;i++){
			float x=vlist[i].v[X];
			float y=vlist[i].v[Y];
			float z=vlist[i].v[Z];
			if(x<xmin)xmin=x;
			if(x>xmax)xmax=x;
			if(y<ymin)ymin=y;
			if(y>ymax)ymax=y;
			if(z<zmin)zmin=z;
			if(z>zmax)zmax=z;
		}//得到xmin,xmax,ymin,ymax,zmin,zmax
	}
	//对点云进行平移，使点云底面中心与世界原点重合
	{
		//求平移向量
		float bottomCenter[4]={(xmin+xmax)/2,ymin,(zmin+zmax)/2,1};//点云底面中心坐标
		float origin[4]={0,0,0,1};//世界原点坐标
		float movVec[4];//点云平移向量
		sub(origin,bottomCenter,movVec);//得到movVec
		//对点云中各点按movVec进行平移
		int nv=(int)vlist.size();
		for(int i=0;i<nv;i++){
			add(vlist[i].v,movVec,vlist[i].v);
		}
	}
	//更新xmin,xmax,ymin,ymax,zmin,zmax
	{
		//求点云区域的边心距
		float dx=(xmax-xmin)/2;//x方向边心距
		float dy=(ymax-ymin)/2;//y方向边心距
		float dz=(zmax-zmin)/2;//z方向边心距
		//更新xmin,xmax,ymin,ymax,zmin,zmax
		xmin=-dx;
		xmax=dx;
		ymin=0;
		ymax=2*dy;
		zmin=-dz;
		zmax=dz;
	}
	return true;


}
