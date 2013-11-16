//--------------------------
//����ģ��
//--------------------------
inline int getNextValueStr(const string&,int,string&);
inline int getf(const string&,int,float&);
inline int geti(const string&,int,int&);
inline int getb(const string&,int,bool&);
inline int getPathName(const string&,int,string&);
inline bool load_cloud(const string&,Ccloud&);
inline void fscanPolygenList(FILE  *,myvector<Cpolygen>&);
inline void fscanGrid(FILE  *,Cgrid&);
inline void fscanCell(FILE  *,Ccell*&);
inline int get4fv(const string&,int,float[4]);
inline int get2fv(const string&,int,float[2]);
inline bool load_groundgrid(const string&,Cgroundgrid&);
inline bool load_texture(const string&,CTextureManager&);
inline void fscan4fv(FILE  *,float[4]);
inline void fscan2fv(FILE  *,float[2]);
//ע��fscanf(,,&x)һ��Ҫ������

bool main_load_model(string pathName,float&xmin,float&xmax,float&ymin,float&ymax,float&zmin,float&zmax,
				float&offsetus,float&offsetut,
				myvector<Ccloud>&cloudList,
				Cgroundgrid&groundgrid,
				CTextureManager&textureManager){

	//�����ļ����������ݶ���mainStr
	string mainStr;
	{
		FILE  *fp;
		fp=fopen(pathName.c_str(),"r");
		if(fp==NULL)
		{ 
			cout<<"Open "<<pathName<<" failed!"<<endl; 
			return false;
		}
		//���ļ����ݶ���mainStr
		{
			const int buffLen=1000;
			char buff[buffLen];//�л�����
			while(!feof(fp)){
				fgets(buff,buffLen,fp);
				mainStr+=buff;
			}//�õ�mainStr
		}
		//cout<<mainStr<<endl;
		//�ر��ļ�
		fclose(fp);	
	}
	//��pathName����pathName���ڵ��ļ���·��clipPathName
	string clipPathName;
	{
		clipPathName=pathName;
		//��clipPathName�����һ��ȥ������
		int n=(int)clipPathName.size();
		int count=0;//�������һ���ַ���
		for(int i=n-1;i>=0;i--){
			if(clipPathName[i]!='\\'){
				count++;
			}else{
				break;
			}
		}//�õ�count
		//ȥ�����һ��
		clipPathName.resize(n-count);
	}
	//��mainStr���н���
	{
		int I=0;
		//��ȡxmin,xmax,ymin,ymax,zmin,zmax
		I=getf(mainStr,I,xmin);if(I==-1)return false;
		I=getf(mainStr,I,xmax);if(I==-1)return false;
		I=getf(mainStr,I,ymin);if(I==-1)return false;
		I=getf(mainStr,I,ymax);if(I==-1)return false;
		I=getf(mainStr,I,zmin);if(I==-1)return false;
		I=getf(mainStr,I,zmax);if(I==-1)return false;
		//��ȡoffsetus,offsetut
		I=getf(mainStr,I,offsetus);if(I==-1)return false;
		I=getf(mainStr,I,offsetut);if(I==-1)return false;
		//��ȡncloud
		int ncloud=0;
		I=geti(mainStr,I,ncloud);if(I==-1)return false;
		//��ȡncloud���������·��
		myvector<string> rCPathList;//�������·�����б�
		{
			//��ȡncloud���������·����
			for(int i=0;i<ncloud;i++){
				string rCPath;
				I=getPathName(mainStr,I,rCPath);if(I==-1)return false;
				//��rCPath�浽rCPathList
				rCPathList.push_back(rCPath);
			}
		}
		//��ȡgroundgrid�����·��
		string rGPath;
		I=getPathName(mainStr,I,rGPath);if(I==-1)return false;
		//��ȡphoto,wall,sky�����·��
		string rTPath_photo,rTPath_wall,rTPath_sky;
		I=getPathName(mainStr,I,rTPath_photo);if(I==-1)return false;
		I=getPathName(mainStr,I,rTPath_wall);if(I==-1)return false;
		I=getPathName(mainStr,I,rTPath_sky);if(I==-1)return false;
		//����groundgrid�ļ�
		cout<<"load ground"<<endl;
		if(!load_groundgrid(clipPathName+rGPath,groundgrid)){
			return false;
		}
		//����rTPath_photo,rTPath_wall,rTPath_sky
		cout<<"load texture"<<endl;
		if(!load_texture(clipPathName+rTPath_photo,textureManager)){
			return false;
		}
		if(!load_texture(clipPathName+rTPath_wall,textureManager)){
			return false;
		}
		if(!load_texture(clipPathName+rTPath_sky,textureManager)){
			return false;
		}
		
		//���ν����������ļ�
		//ͬʱ�ڿ���̨������ȣ�ÿ10������Ϊһ�����
		cloudList.resize(ncloud);
		cout<<"load cloud: ";
		for(int i=0;i<ncloud;i++){
			int r=i%10;
			if(r==0){
				cout<<i<<"~";
			}else if(r==9){
				cout<<i<<" ";
			}
			Ccloud cloud;
			if(!load_cloud(clipPathName+rCPathList[i],cloud)){
				return false;			
			}
			//��cloud���뵽cloudList
			cloudList[i].copy(cloud);
		}
		//���ݴ�ʱ��i-1����10�����������Ƿ��β
		int r=(i-1)%10;
		if(r==9){//���÷�β
			cout<<endl;
		}else{//��Ҫ��β
			cout<<i-1<<endl;
		}
		
	}
	//��һЩ����
	{	
		//�����зǿ��ݶ����empty��Ϊfalse
		int ncloud=cloudList.size();
		for(int i=0;i<ncloud;i++){
			Cgrid&grid=cloudList[i].grid;
			int m=grid.m_mat;
			int n=grid.n_mat;
			for(int j=0;j<m;j++){
				for(int k=0;k<n;k++){
					Ccell*pcell=grid.mat[j][k];
					if(pcell!=NULL){
						pcell->empty=false;
					}
				}
			}	
		}	
	}
	return true;

}

inline int getNextValueStr(const string&str,int I,string&valueStr)
//��ȡ��һ��������ֵ
//��str��I����ʼ
//ǰ�˷�����ֵ����һ��λ�ã��ᵽ����ֵ�浽value��
//������ֵֻ�����Ը��Ż����ֿ�ͷ����������С���㿪ͷ�������⣬Ҳ�������ʺ�
//�ٶ�str������ֵ��β
{
	if(I==-1){
		cout<<"error I==-1!"<<endl;
		return -1;
	}
	//�Ƚ�valueStr���
	valueStr.resize(0);
	//����һ����ֵ����ʼ�ַ�
	int n=(int)str.size();
	for(int i=I;i<n;i++){
		char c=str[i];
		if((c>='0'&&c<='9')||c=='-'||c=='?'){//���c����ֵ�򸺺Ż�?
			break;
		}
	}
	if(i<n){//�ҵ���ֵ
		if(str[i]=='?'){//���iָ���ʺ�
			valueStr="?";
			return i+1;
		}
		//iָ���һ����ֵ�ַ��򸺺�
		//��i��ʼ��ȡ��������������ֵ�ַ���
		valueStr+=str[i];//��str[i]���뵽valueStr
		//������ȡ�������Ĳ���
		int count=0;//����С�������
		for(int j=i+1;j<n;j++){
			char c=str[j];
			bool isNumber=(c>='0'&&c<='9');
			bool isDot=(c=='.');
			if(isNumber||isDot){//�������ֵ��С����
				//���Խ�һ������
				if(isNumber){
					//��c���뵽valueStr
					valueStr+=c;
				}else{//isDot
					if(count==0){//�����û�����С����
						//��c���뵽valueStr
						valueStr+=c;
						count++;
					}else{//����Ѿ���С������
						//��ȡ��ֵ����
						break;
					}
				}	
			}else{//���������ֵ��С����
				//��ȡ����
				break;
			}
		}//�õ�valueStr
		if(j==n){//����break�����ģ�˵���ᵽ����ֵ����û�ж���
			cout<<"error �ı�����ֵ��β!"<<endl;
			return -1;
		}else{//��ȡ�ɹ�
			return j;
		}

	}else{//δ�ҵ���ֵ
		return -1;
	}

}

inline int getf(const string&str,int I,float&value)
//��ȡ��һ����ֵ������ֵ��
//��str��I����ʼ
//ǰ�˷�����ֵ����һ��λ�ã��ᵽ����ֵ�浽value��
//������ֵֻ�����Ը��Ż����ֿ�ͷ����������С���㿪ͷ�������⣬Ҳ�������ʺ�
//�ٶ�str������ֵ��β
{
	string valueStr;
	int In=getNextValueStr(str,I,valueStr);
	if(valueStr=="?"){
		value=0;
		return In; 
	}
	//��valueStrת���ɸ�����
	value=atof(valueStr.c_str());
//	cout<<"("<<valueStr<<" "<<value<<")"<<endl;
	return In;
	

}
inline int geti(const string&str,int I,int&value)
//��ȡ��һ����ֵ������ֵ��
//��str��I����ʼ
//ǰ�˷�����ֵ����һ��λ�ã��ᵽ����ֵ�浽value��
//������ֵֻ�����Ը��Ż����ֿ�ͷ����������С���㿪ͷ�������⣬Ҳ�������ʺ�
//�ٶ�str������ֵ��β
{
	string valueStr;
	int In=getNextValueStr(str,I,valueStr);
	if(valueStr=="?"){
		value=0;
		return In; 
	}
	//��valueStrת���ɸ�����
	value=atoi(valueStr.c_str());
	return In;
}

inline int getb(const string&str,int I,bool&value)
//��ȡ��һ����ֵ������ֵ��
//��str��I����ʼ
//ǰ�˷�����ֵ����һ��λ�ã��ᵽ����ֵ�浽value��
//������ֵֻ�����Ը��Ż����ֿ�ͷ����������С���㿪ͷ��
//�ٶ�str������ֵ��β
{
	string valueStr;
	int In=getNextValueStr(str,I,valueStr);
	//��valueStrת���ɸ�����
	value=atoi(valueStr.c_str());
	return In;
}
inline int getPathName(const string&str,int I,string&pathName)
//��ȡ��һ��·����
//·��������""�������
{
	if(I==-1){
		cout<<"error I==-1!"<<endl;
		return -1;
	}
	//�Ƚ�valueStr���
	pathName.resize(0);
	//����һ����ֵ����ʼ�ַ�
	int n=(int)str.size();
	for(int i=I;i<n;i++){
		char c=str[i];
		if(c=='\"'){//���c��"
			break;
		}
	}
	if(i<n){//�ҵ�"
		//iָ���һ��"
		//��i+1��ʼ��ȡ������������·��
		for(int j=i+1;j<n;j++){
			char c=str[j];
			if(c!='\"'){//�������"
				//��c���뵽pathName
				pathName+=c;
			}else{//�����"
				//������ȡ
				break;
			}
		}//�õ�pathName
		if(j==n){//�������break�����ģ�˵��û�ҵ���Ӧ��"
			cout<<"û�ҵ�ƥ���\""<<endl;
			return -1;
		}else{//��ȡ�ɹ�
			//jָ��"
			return j+1;
		}

	}else{//δ�ҵ�"
		return -1;
	}

}

inline bool load_cloud(const string&pathName,Ccloud&cloud)
//��������
{
	//��ȡ�ļ����ݵ�str
	FILE  *fp;
	fp=fopen(pathName.c_str(),"r");
	if(fp==NULL)
	{ 
		cout<<"load_cloud: Open "<<pathName<<" failed!"<<endl; 
		return false;
	}
	//��ʼ��ȡ����
	{
		//��ȡavrgH,bottomH,topH,minH,maxH
		fscanf(fp,"%f",&cloud.avrgH);
		fscanf(fp,"%f",&cloud.bottomH);
		fscanf(fp,"%f",&cloud.topH);
		fscanf(fp,"%f",&cloud.minH);
		//��ȡcolor
		fscan4fv(fp,cloud.color);
		//��ȡremoved
		fscanf(fp,"%d",&cloud.removed);
		//��ȡxmin,xmax,zmin,zmax
		fscanf(fp,"%f",&cloud.xmin);
		fscanf(fp,"%f",&cloud.xmax);
		fscanf(fp,"%f",&cloud.zmin);
		fscanf(fp,"%f",&cloud.zmax);
		//��ȡ����polygenList
		fscanPolygenList(fp,cloud.polygenList);
		fscanPolygenList(fp,cloud.polygenList_dn);
		//��ȡgrid
		fscanGrid(fp,cloud.grid);


	
	}
	
	//�ر��ļ�
	fclose(fp);	



	return true;


}
inline void fscanPolygenList(FILE  *fp,myvector<Cpolygen>&polygenList)
//��I��ʼ��ȡ��str��ȡpolygenList���ݲ���䵽polygenList
{

	int npolygen=0;
	fscanf(fp,"%d",&npolygen);
	//��ȡnpolygen��polygen
	polygenList.resize(npolygen);
	for(int i=0;i<npolygen;i++){
		Cpolygen&polygen=polygenList[i];
		//��ȡ��polygen
		//��ȡremoved
		fscanf(fp,"%d",&polygen.removed);
	
		//��ȡnv
		int nv=0;
		fscanf(fp,"%d",&nv);
	
		//��ȡnv����������
		polygen.vlist.resize(nv);

		for(int j=0;j<nv;j++){
			Cv4Ex&v=polygen.vlist[j];
			//��ȡv
			//��ȡisKeyPoint
			fscanf(fp,"%d",&v.isKeyPoint);
			//��ȡv��x,y,z,1��ע�⣬1Ҳ������������Ϊ��Ҳ����ֵ��
			fscan4fv(fp,v.v);
//			cout<<"v:";print4fv(v.v);
			//��ȡs,t
			fscan2fv(fp,v.u);
//			cout<<"u:";print2fv(v.u);
			//��ȡsf,tf
			fscan2fv(fp,v.uf);
//			cout<<"uf:";print2fv(v.uf);
			//��ȡsn,tn
			fscan2fv(fp,v.un);
//			cout<<"un:";print2fv(v.un);
		}//�����ȡ���
	}//��polygen��ȡ���
}

inline void fscanGrid(FILE  *fp,Cgrid&grid)
//��I��ʼ��ȡ��str��ȡgrid���ݲ���䵽grid
{
	//��ȡxmin,xmax,zmin,zmax
	float xmin,xmax,zmin,zmax;
	fscanf(fp,"%f",&xmin);
	fscanf(fp,"%f",&xmax);
	fscanf(fp,"%f",&zmin);
	fscanf(fp,"%f",&zmax);
	grid.setRange(xmin,xmax,zmin,zmax);
	//��ȡxstepLen,zstepLen
	fscanf(fp,"%f",&grid.xstepLen);
	fscanf(fp,"%f",&grid.zstepLen);
	//��ȡm_locToWorld
	for(int i=0;i<16;i++){
		fscanf(fp,"%f",&grid.m_locToWorld[i]);
	}
	//��ȡm_worldToLoc
	for(i=0;i<16;i++){
		fscanf(fp,"%f",&grid.m_worldToLoc[i]);
	}
	//��ȡm,n
	fscanf(fp,"%d",&grid.m_mat);
	fscanf(fp,"%d",&grid.n_mat);
	//����m,n����mat
	{
		grid.mat.resize(grid.m_mat);
		for(int i=0;i<grid.m_mat;i++){
			grid.mat[i].resize(grid.n_mat);
			for(int j=0;j<grid.n_mat;j++){
				grid.mat[i][j]=NULL;
			}
		}
	}
	//��ȡm*n��cell
	for(i=0;i<grid.m_mat;i++){
		for(int j=0;j<grid.n_mat;j++){
			fscanCell(fp,grid.mat[i][j]);
		
		}
	}

}

inline void fscanCell(FILE  *fp,Ccell*&pcell)
//��I��ʼ��ȡ��str��ȡcell���ݲ���䵽*pcell
//ע�������ָ�������
{
	//��ȡisNull
	bool isNull;
	fscanf(fp,"%d",&isNull);
	if(isNull){
		pcell=NULL;
		return;
	}
	//����cell
	pcell=new Ccell();
	//��ȡneedcut
	fscanf(fp,"%d",&pcell->needcut);
	//��ȡv��x,y,z,1��ע�⣬1ҲҪ�ᣬ������ֵ��λ��
	fscan4fv(fp,pcell->v);
//	cout<<"v:";print4fv(pcell->v);
	//��ȡu
	fscan2fv(fp,pcell->u);
//	cout<<"u:";print2fv(pcell->u);
	//��ȡvlt,vrt,vup,vdn,vlup,vldn,vrup,vrdn
	fscan4fv(fp,pcell->vlt);//cout<<"vlt:";print4fv(pcell->vlt);
	fscan4fv(fp,pcell->vrt);//cout<<"vrt:";print4fv(pcell->vrt);
	fscan4fv(fp,pcell->vup);//cout<<"vup:";print4fv(pcell->vup);
	fscan4fv(fp,pcell->vdn);//cout<<"vdn:";print4fv(pcell->vdn);
	fscan4fv(fp,pcell->vlup);//cout<<"vlup:";print4fv(pcell->vlup);
	fscan4fv(fp,pcell->vldn);//cout<<"vldn:";print4fv(pcell->vldn);
	fscan4fv(fp,pcell->vrup);//cout<<"vrup:";print4fv(pcell->vrup);
	fscan4fv(fp,pcell->vrdn);//cout<<"vrdn:";print4fv(pcell->vrdn);
	//��ȡult,urt,uup,udn,ulup,uldn,urup,urdn
	fscan2fv(fp,pcell->ult);//cout<<"ult:";print2fv(pcell->ult);
	fscan2fv(fp,pcell->urt);//cout<<"urt:";print2fv(pcell->urt);
	fscan2fv(fp,pcell->uup);//cout<<"uup:";print2fv(pcell->uup);
	fscan2fv(fp,pcell->udn);//cout<<"udn:";print2fv(pcell->udn);
	fscan2fv(fp,pcell->ulup);//cout<<"ulup:";print2fv(pcell->ulup);
	fscan2fv(fp,pcell->uldn);//cout<<"uldn:";print2fv(pcell->uldn);
	fscan2fv(fp,pcell->urup);//cout<<"urup:";print2fv(pcell->urup);
	fscan2fv(fp,pcell->urdn);//cout<<"urdn:";print2fv(pcell->urdn);
	//��ȡnorm0,norm1,norm2,norm3
	fscan4fv(fp,pcell->norm[0]);//cout<<"norm0:";print4fv(pcell->norm[0]);
	fscan4fv(fp,pcell->norm[1]);//cout<<"norm1:";print4fv(pcell->norm[1]);
	fscan4fv(fp,pcell->norm[2]);//cout<<"norm2:";print4fv(pcell->norm[2]);
	fscan4fv(fp,pcell->norm[3]);//cout<<"norm3:";print4fv(pcell->norm[3]);
	//��ȡavrgnorm
	fscan4fv(fp,pcell->avrgnorm);//cout<<"avrgnorm:";print4fv(pcell->avrgnorm);
	//��ȡpolygenList
	fscanPolygenList(fp,pcell->polygenList);

}
inline int get4fv(const string&str,int I,float vec[4]){
	I=getf(str,I,vec[X]);if(I==-1)return I;
	I=getf(str,I,vec[Y]);if(I==-1)return I;
	I=getf(str,I,vec[Z]);if(I==-1)return I;
	I=getf(str,I,vec[W]);if(I==-1)return I;
	return I;
}
inline int get2fv(const string&str,int I,float vec[2]){
	I=getf(str,I,vec[X]);if(I==-1)return I;
	I=getf(str,I,vec[Y]);if(I==-1)return I;
	return I;
}
inline void fscan4fv(FILE  *fp,float vec[4]){
	fscanf(fp,"%f",&vec[X]);
	fscanf(fp,"%f",&vec[Y]);
	fscanf(fp,"%f",&vec[Z]);
	fscanf(fp,"%f",&vec[W]);

}
inline void fscan2fv(FILE  *fp,float vec[2]){
	fscanf(fp,"%f",&vec[X]);
	fscanf(fp,"%f",&vec[Y]);
}
inline bool load_groundgrid(const string&pathName,Cgroundgrid&groundgrid)
//���ص���
{

	FILE  *fp;
	fp=fopen(pathName.c_str(),"r");
	if(fp==NULL)
	{ 
		cout<<"Open "<<pathName<<" failed!"<<endl; 
		return false;
	}
	//��ȡ��ֵ
	{
		//��ȡxmin,xmax,zmin,zmax
		fscanf(fp,"%f",&groundgrid.xmin);
		fscanf(fp,"%f",&groundgrid.xmax);
		fscanf(fp,"%f",&groundgrid.zmin);
		fscanf(fp,"%f",&groundgrid.zmax);
		//��ȡxstepLen,zstepLen
		fscanf(fp,"%f",&groundgrid.xstepLen);
		fscanf(fp,"%f",&groundgrid.zstepLen);
		
		//��ȡmat
		{
			//��ȡm,n
			fscanf(fp,"%d",&groundgrid.m_mat);
			fscanf(fp,"%d",&groundgrid.n_mat);
			//����mat
			{
				groundgrid.mat.resize(groundgrid.m_mat);
				for(int i=0;i<groundgrid.m_mat;i++){
					groundgrid.mat[i].resize(groundgrid.n_mat);
				}
			}
			//��ȡm*n��groundcell
			for(int i=0;i<groundgrid.m_mat;i++){
				for(int j=0;j<groundgrid.n_mat;j++){
					Cgroundcell&gcell=groundgrid.mat[i][j];
					//��ȡgcell
					//��ȡv
					float v[4];
					fscan4fv(fp,v);
					gcell.h=v[Y];
					//��ȡu
					fscan2fv(fp,gcell.u);
				}
			}
			
			
		}
	}
	//�ر��ļ�
	fclose(fp);	

	return true;

}
inline bool load_texture(const string&pathName,CTextureManager&textureManager)
//��������
{

	if(!textureManager.CreateTexture(pathName)){
		return false;
	}
	return true;

}