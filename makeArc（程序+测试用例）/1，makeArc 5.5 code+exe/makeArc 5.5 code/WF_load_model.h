//--------------------------
//加载模型
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
//注：fscanf(,,&x)一定要用引用

bool main_load_model(string pathName,float&xmin,float&xmax,float&ymin,float&ymax,float&zmin,float&zmax,
				float&offsetus,float&offsetut,
				myvector<Ccloud>&cloudList,
				Cgroundgrid&groundgrid,
				CTextureManager&textureManager){

	//打开主文件，将其内容读到mainStr
	string mainStr;
	{
		FILE  *fp;
		fp=fopen(pathName.c_str(),"r");
		if(fp==NULL)
		{ 
			cout<<"Open "<<pathName<<" failed!"<<endl; 
			return false;
		}
		//将文件内容读到mainStr
		{
			const int buffLen=1000;
			char buff[buffLen];//行缓冲区
			while(!feof(fp)){
				fgets(buff,buffLen,fp);
				mainStr+=buff;
			}//得到mainStr
		}
		//cout<<mainStr<<endl;
		//关闭文件
		fclose(fp);	
	}
	//由pathName制作pathName所在的文件夹路径clipPathName
	string clipPathName;
	{
		clipPathName=pathName;
		//将clipPathName的最后一级去掉即得
		int n=(int)clipPathName.size();
		int count=0;//计数最后一节字符数
		for(int i=n-1;i>=0;i--){
			if(clipPathName[i]!='\\'){
				count++;
			}else{
				break;
			}
		}//得到count
		//去掉最后一级
		clipPathName.resize(n-count);
	}
	//对mainStr进行解析
	{
		int I=0;
		//提取xmin,xmax,ymin,ymax,zmin,zmax
		I=getf(mainStr,I,xmin);if(I==-1)return false;
		I=getf(mainStr,I,xmax);if(I==-1)return false;
		I=getf(mainStr,I,ymin);if(I==-1)return false;
		I=getf(mainStr,I,ymax);if(I==-1)return false;
		I=getf(mainStr,I,zmin);if(I==-1)return false;
		I=getf(mainStr,I,zmax);if(I==-1)return false;
		//提取offsetus,offsetut
		I=getf(mainStr,I,offsetus);if(I==-1)return false;
		I=getf(mainStr,I,offsetut);if(I==-1)return false;
		//提取ncloud
		int ncloud=0;
		I=geti(mainStr,I,ncloud);if(I==-1)return false;
		//提取ncloud个云团相对路径
		myvector<string> rCPathList;//云团相对路径名列表
		{
			//提取ncloud个云团相对路径名
			for(int i=0;i<ncloud;i++){
				string rCPath;
				I=getPathName(mainStr,I,rCPath);if(I==-1)return false;
				//将rCPath存到rCPathList
				rCPathList.push_back(rCPath);
			}
		}
		//提取groundgrid的相对路径
		string rGPath;
		I=getPathName(mainStr,I,rGPath);if(I==-1)return false;
		//提取photo,wall,sky的相对路径
		string rTPath_photo,rTPath_wall,rTPath_sky;
		I=getPathName(mainStr,I,rTPath_photo);if(I==-1)return false;
		I=getPathName(mainStr,I,rTPath_wall);if(I==-1)return false;
		I=getPathName(mainStr,I,rTPath_sky);if(I==-1)return false;
		//解析groundgrid文件
		cout<<"load ground"<<endl;
		if(!load_groundgrid(clipPathName+rGPath,groundgrid)){
			return false;
		}
		//解析rTPath_photo,rTPath_wall,rTPath_sky
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
		
		//依次解析各云团文件
		//同时在控制台输出进度，每10个云团为一组输出
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
			//将cloud加入到cloudList
			cloudList[i].copy(cloud);
		}
		//根据此时的i-1除以10的余数决定是否封尾
		int r=(i-1)%10;
		if(r==9){//不用封尾
			cout<<endl;
		}else{//需要封尾
			cout<<i-1<<endl;
		}
		
	}
	//作一些补充
	{	
		//将所有非空屋顶格的empty置为false
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
//提取下一个浮点数值
//从str的I处开始
//前端返回数值的下一个位置，提到的数值存到value里
//假设数值只可能以负号或数字开头（而不会以小数点开头），另外，也可能是问号
//假定str不以数值结尾
{
	if(I==-1){
		cout<<"error I==-1!"<<endl;
		return -1;
	}
	//先将valueStr清空
	valueStr.resize(0);
	//找下一个数值的起始字符
	int n=(int)str.size();
	for(int i=I;i<n;i++){
		char c=str[i];
		if((c>='0'&&c<='9')||c=='-'||c=='?'){//如果c是数值或负号或?
			break;
		}
	}
	if(i<n){//找到数值
		if(str[i]=='?'){//如果i指向问号
			valueStr="?";
			return i+1;
		}
		//i指向第一个数值字符或负号
		//从i开始提取接下来的整个数值字符串
		valueStr+=str[i];//将str[i]加入到valueStr
		//继续提取接下来的部分
		int count=0;//计数小数点个数
		for(int j=i+1;j<n;j++){
			char c=str[j];
			bool isNumber=(c>='0'&&c<='9');
			bool isDot=(c=='.');
			if(isNumber||isDot){//如果是数值或小数点
				//可以进一步考虑
				if(isNumber){
					//将c加入到valueStr
					valueStr+=c;
				}else{//isDot
					if(count==0){//如果还没加入过小数点
						//将c加入到valueStr
						valueStr+=c;
						count++;
					}else{//如果已经有小数点了
						//提取数值结束
						break;
					}
				}	
			}else{//如果不是数值或小数点
				//接取结束
				break;
			}
		}//得到valueStr
		if(j==n){//不是break出来的，说明提到的数值后面没有东西
			cout<<"error 文本以数值结尾!"<<endl;
			return -1;
		}else{//提取成功
			return j;
		}

	}else{//未找到数值
		return -1;
	}

}

inline int getf(const string&str,int I,float&value)
//提取下一个数值（浮点值）
//从str的I处开始
//前端返回数值的下一个位置，提到的数值存到value里
//假设数值只可能以负号或数字开头（而不会以小数点开头），另外，也可能是问号
//假定str不以数值结尾
{
	string valueStr;
	int In=getNextValueStr(str,I,valueStr);
	if(valueStr=="?"){
		value=0;
		return In; 
	}
	//将valueStr转化成浮点数
	value=atof(valueStr.c_str());
//	cout<<"("<<valueStr<<" "<<value<<")"<<endl;
	return In;
	

}
inline int geti(const string&str,int I,int&value)
//提取下一个数值（整数值）
//从str的I处开始
//前端返回数值的下一个位置，提到的数值存到value里
//假设数值只可能以负号或数字开头（而不会以小数点开头），另外，也可能是问号
//假定str不以数值结尾
{
	string valueStr;
	int In=getNextValueStr(str,I,valueStr);
	if(valueStr=="?"){
		value=0;
		return In; 
	}
	//将valueStr转化成浮点数
	value=atoi(valueStr.c_str());
	return In;
}

inline int getb(const string&str,int I,bool&value)
//提取下一个数值（布尔值）
//从str的I处开始
//前端返回数值的下一个位置，提到的数值存到value里
//假设数值只可能以负号或数字开头（而不会以小数点开头）
//假定str不以数值结尾
{
	string valueStr;
	int In=getNextValueStr(str,I,valueStr);
	//将valueStr转化成浮点数
	value=atoi(valueStr.c_str());
	return In;
}
inline int getPathName(const string&str,int I,string&pathName)
//提取下一个路径名
//路径名是以""号括起的
{
	if(I==-1){
		cout<<"error I==-1!"<<endl;
		return -1;
	}
	//先将valueStr清空
	pathName.resize(0);
	//找下一个数值的起始字符
	int n=(int)str.size();
	for(int i=I;i<n;i++){
		char c=str[i];
		if(c=='\"'){//如果c是"
			break;
		}
	}
	if(i<n){//找到"
		//i指向第一个"
		//从i+1开始提取接下来的整个路径
		for(int j=i+1;j<n;j++){
			char c=str[j];
			if(c!='\"'){//如果不是"
				//将c加入到pathName
				pathName+=c;
			}else{//如果是"
				//结束提取
				break;
			}
		}//得到pathName
		if(j==n){//如果不是break出来的，说明没找到对应的"
			cout<<"没找到匹配的\""<<endl;
			return -1;
		}else{//提取成功
			//j指向"
			return j+1;
		}

	}else{//未找到"
		return -1;
	}

}

inline bool load_cloud(const string&pathName,Ccloud&cloud)
//加载云团
{
	//读取文件内容到str
	FILE  *fp;
	fp=fopen(pathName.c_str(),"r");
	if(fp==NULL)
	{ 
		cout<<"load_cloud: Open "<<pathName<<" failed!"<<endl; 
		return false;
	}
	//开始提取数据
	{
		//提取avrgH,bottomH,topH,minH,maxH
		fscanf(fp,"%f",&cloud.avrgH);
		fscanf(fp,"%f",&cloud.bottomH);
		fscanf(fp,"%f",&cloud.topH);
		fscanf(fp,"%f",&cloud.minH);
		//提取color
		fscan4fv(fp,cloud.color);
		//提取removed
		fscanf(fp,"%d",&cloud.removed);
		//提取xmin,xmax,zmin,zmax
		fscanf(fp,"%f",&cloud.xmin);
		fscanf(fp,"%f",&cloud.xmax);
		fscanf(fp,"%f",&cloud.zmin);
		fscanf(fp,"%f",&cloud.zmax);
		//提取两个polygenList
		fscanPolygenList(fp,cloud.polygenList);
		fscanPolygenList(fp,cloud.polygenList_dn);
		//提取grid
		fscanGrid(fp,cloud.grid);


	
	}
	
	//关闭文件
	fclose(fp);	



	return true;


}
inline void fscanPolygenList(FILE  *fp,myvector<Cpolygen>&polygenList)
//从I开始提取从str提取polygenList数据并填充到polygenList
{

	int npolygen=0;
	fscanf(fp,"%d",&npolygen);
	//提取npolygen个polygen
	polygenList.resize(npolygen);
	for(int i=0;i<npolygen;i++){
		Cpolygen&polygen=polygenList[i];
		//提取第polygen
		//提取removed
		fscanf(fp,"%d",&polygen.removed);
	
		//提取nv
		int nv=0;
		fscanf(fp,"%d",&nv);
	
		//提取nv个顶点数据
		polygen.vlist.resize(nv);

		for(int j=0;j<nv;j++){
			Cv4Ex&v=polygen.vlist[j];
			//提取v
			//提取isKeyPoint
			fscanf(fp,"%d",&v.isKeyPoint);
			//提取v的x,y,z,1（注意，1也不能跳过，因为它也是数值）
			fscan4fv(fp,v.v);
//			cout<<"v:";print4fv(v.v);
			//提取s,t
			fscan2fv(fp,v.u);
//			cout<<"u:";print2fv(v.u);
			//提取sf,tf
			fscan2fv(fp,v.uf);
//			cout<<"uf:";print2fv(v.uf);
			//提取sn,tn
			fscan2fv(fp,v.un);
//			cout<<"un:";print2fv(v.un);
		}//诸顶点提取完成
	}//诸polygen提取完成
}

inline void fscanGrid(FILE  *fp,Cgrid&grid)
//从I开始提取从str提取grid数据并填充到grid
{
	//提取xmin,xmax,zmin,zmax
	float xmin,xmax,zmin,zmax;
	fscanf(fp,"%f",&xmin);
	fscanf(fp,"%f",&xmax);
	fscanf(fp,"%f",&zmin);
	fscanf(fp,"%f",&zmax);
	grid.setRange(xmin,xmax,zmin,zmax);
	//提取xstepLen,zstepLen
	fscanf(fp,"%f",&grid.xstepLen);
	fscanf(fp,"%f",&grid.zstepLen);
	//提取m_locToWorld
	for(int i=0;i<16;i++){
		fscanf(fp,"%f",&grid.m_locToWorld[i]);
	}
	//提取m_worldToLoc
	for(i=0;i<16;i++){
		fscanf(fp,"%f",&grid.m_worldToLoc[i]);
	}
	//提取m,n
	fscanf(fp,"%d",&grid.m_mat);
	fscanf(fp,"%d",&grid.n_mat);
	//根据m,n生成mat
	{
		grid.mat.resize(grid.m_mat);
		for(int i=0;i<grid.m_mat;i++){
			grid.mat[i].resize(grid.n_mat);
			for(int j=0;j<grid.n_mat;j++){
				grid.mat[i][j]=NULL;
			}
		}
	}
	//读取m*n个cell
	for(i=0;i<grid.m_mat;i++){
		for(int j=0;j<grid.n_mat;j++){
			fscanCell(fp,grid.mat[i][j]);
		
		}
	}

}

inline void fscanCell(FILE  *fp,Ccell*&pcell)
//从I开始提取从str提取cell数据并填充到*pcell
//注意必须用指针的引用
{
	//提取isNull
	bool isNull;
	fscanf(fp,"%d",&isNull);
	if(isNull){
		pcell=NULL;
		return;
	}
	//创建cell
	pcell=new Ccell();
	//提取needcut
	fscanf(fp,"%d",&pcell->needcut);
	//提取v的x,y,z,1（注意，1也要提，否则数值错位）
	fscan4fv(fp,pcell->v);
//	cout<<"v:";print4fv(pcell->v);
	//提取u
	fscan2fv(fp,pcell->u);
//	cout<<"u:";print2fv(pcell->u);
	//提取vlt,vrt,vup,vdn,vlup,vldn,vrup,vrdn
	fscan4fv(fp,pcell->vlt);//cout<<"vlt:";print4fv(pcell->vlt);
	fscan4fv(fp,pcell->vrt);//cout<<"vrt:";print4fv(pcell->vrt);
	fscan4fv(fp,pcell->vup);//cout<<"vup:";print4fv(pcell->vup);
	fscan4fv(fp,pcell->vdn);//cout<<"vdn:";print4fv(pcell->vdn);
	fscan4fv(fp,pcell->vlup);//cout<<"vlup:";print4fv(pcell->vlup);
	fscan4fv(fp,pcell->vldn);//cout<<"vldn:";print4fv(pcell->vldn);
	fscan4fv(fp,pcell->vrup);//cout<<"vrup:";print4fv(pcell->vrup);
	fscan4fv(fp,pcell->vrdn);//cout<<"vrdn:";print4fv(pcell->vrdn);
	//提取ult,urt,uup,udn,ulup,uldn,urup,urdn
	fscan2fv(fp,pcell->ult);//cout<<"ult:";print2fv(pcell->ult);
	fscan2fv(fp,pcell->urt);//cout<<"urt:";print2fv(pcell->urt);
	fscan2fv(fp,pcell->uup);//cout<<"uup:";print2fv(pcell->uup);
	fscan2fv(fp,pcell->udn);//cout<<"udn:";print2fv(pcell->udn);
	fscan2fv(fp,pcell->ulup);//cout<<"ulup:";print2fv(pcell->ulup);
	fscan2fv(fp,pcell->uldn);//cout<<"uldn:";print2fv(pcell->uldn);
	fscan2fv(fp,pcell->urup);//cout<<"urup:";print2fv(pcell->urup);
	fscan2fv(fp,pcell->urdn);//cout<<"urdn:";print2fv(pcell->urdn);
	//提取norm0,norm1,norm2,norm3
	fscan4fv(fp,pcell->norm[0]);//cout<<"norm0:";print4fv(pcell->norm[0]);
	fscan4fv(fp,pcell->norm[1]);//cout<<"norm1:";print4fv(pcell->norm[1]);
	fscan4fv(fp,pcell->norm[2]);//cout<<"norm2:";print4fv(pcell->norm[2]);
	fscan4fv(fp,pcell->norm[3]);//cout<<"norm3:";print4fv(pcell->norm[3]);
	//提取avrgnorm
	fscan4fv(fp,pcell->avrgnorm);//cout<<"avrgnorm:";print4fv(pcell->avrgnorm);
	//提取polygenList
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
//加载地形
{

	FILE  *fp;
	fp=fopen(pathName.c_str(),"r");
	if(fp==NULL)
	{ 
		cout<<"Open "<<pathName<<" failed!"<<endl; 
		return false;
	}
	//提取数值
	{
		//提取xmin,xmax,zmin,zmax
		fscanf(fp,"%f",&groundgrid.xmin);
		fscanf(fp,"%f",&groundgrid.xmax);
		fscanf(fp,"%f",&groundgrid.zmin);
		fscanf(fp,"%f",&groundgrid.zmax);
		//提取xstepLen,zstepLen
		fscanf(fp,"%f",&groundgrid.xstepLen);
		fscanf(fp,"%f",&groundgrid.zstepLen);
		
		//提取mat
		{
			//提取m,n
			fscanf(fp,"%d",&groundgrid.m_mat);
			fscanf(fp,"%d",&groundgrid.n_mat);
			//创建mat
			{
				groundgrid.mat.resize(groundgrid.m_mat);
				for(int i=0;i<groundgrid.m_mat;i++){
					groundgrid.mat[i].resize(groundgrid.n_mat);
				}
			}
			//读取m*n个groundcell
			for(int i=0;i<groundgrid.m_mat;i++){
				for(int j=0;j<groundgrid.n_mat;j++){
					Cgroundcell&gcell=groundgrid.mat[i][j];
					//提取gcell
					//提取v
					float v[4];
					fscan4fv(fp,v);
					gcell.h=v[Y];
					//提取u
					fscan2fv(fp,gcell.u);
				}
			}
			
			
		}
	}
	//关闭文件
	fclose(fp);	

	return true;

}
inline bool load_texture(const string&pathName,CTextureManager&textureManager)
//加载纹理
{

	if(!textureManager.CreateTexture(pathName)){
		return false;
	}
	return true;

}