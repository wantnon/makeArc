//------------------------------------
//�������ݵĶ�ȡ
//------------------------------------
inline bool main_cloud_data_read_in(const char *fileName,
							 myvector<Cv4>&vlist,
							 float&xmin,float&xmax,float&ymin,float&ymax,float&zmin,float&zmax)
//���о��������������
{
	//��ȡ���ݵ�vlist
	{
		//���ļ�
		FILE  *infile;
		infile=fopen(fileName,"r");
		if(infile==NULL)
		{ 
			cout<<"Open "<<fileName<<" failed!"<<endl; 
			return false;
		}
		//��ȡ����
		while(1){
			float x,y,z;
			if(fscanf(infile,"%f",&x)==EOF)break;
			if(fscanf(infile,"%f",&y)==EOF)break;
			if(fscanf(infile,"%f",&z)==EOF)break;
			Cv4 v;
			init4(v.v,x,z,y,1);//ע�⣬��Ϊ����������z��ʾ�߶ȣ����Խ������е�(x,y)��Ϊ����ռ��(x,z)������������openglϰ��
			vlist.push_back(v);
		}
		//�ر��ļ�
		fclose(infile);
	}
	//����Ʒ�Χxmin,xmax,ymin,ymax,zmin,zmax
	{
		//����Ʒ�Χ
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
		}//�õ�xmin,xmax,ymin,ymax,zmin,zmax
	}
	//�Ե��ƽ���ƽ�ƣ�ʹ���Ƶ�������������ԭ���غ�
	{
		//��ƽ������
		float bottomCenter[4]={(xmin+xmax)/2,ymin,(zmin+zmax)/2,1};//���Ƶ�����������
		float origin[4]={0,0,0,1};//����ԭ������
		float movVec[4];//����ƽ������
		sub(origin,bottomCenter,movVec);//�õ�movVec
		//�Ե����и��㰴movVec����ƽ��
		int nv=(int)vlist.size();
		for(int i=0;i<nv;i++){
			add(vlist[i].v,movVec,vlist[i].v);
		}
	}
	//����xmin,xmax,ymin,ymax,zmin,zmax
	{
		//���������ı��ľ�
		float dx=(xmax-xmin)/2;//x������ľ�
		float dy=(ymax-ymin)/2;//y������ľ�
		float dz=(zmax-zmin)/2;//z������ľ�
		//����xmin,xmax,ymin,ymax,zmin,zmax
		xmin=-dx;
		xmax=dx;
		ymin=0;
		ymax=2*dy;
		zmin=-dz;
		zmax=dz;
	}
	return true;


}
