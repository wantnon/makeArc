
inline double calCircle(float _x1,float _y1,float _x2,float _y2,float _x3,float _y3,
		  float &x,float &y)
		  //输入三点(_x1,_y1),(_x2,y2),(_x3,_y3)，得到圆心坐标(x,y)，前端返回半径的平方
{		
	double r2;//外接圆半径的平方

	//求水平的边
	int EvenEdgeIndex=0;//记录第几个边水平
	int count=0;
	if(fabs(_y1-_y2)<0.01){//水平
		EvenEdgeIndex=1;
		count++;
	}
	if(fabs(_y2-_y3)<0.01){
		EvenEdgeIndex=2;
		count++;
	}
	if(fabs(_y1-_y3)<0.01){
		EvenEdgeIndex=3;
		count++;
	}
	if(count>1){
//		cout<<"error circle"<<endl;
		return -inf;

	}

	float x1,y1,x2,y2,x3,y3;
	switch(EvenEdgeIndex){
	case 1://如果(_x1,_y1)-(_x2,_y2)水平
		x1=_x2;
		y1=_y2;
		x2=_x3;
		y2=_y3;
		x3=_x1;
		y3=_y1;
		break;
	case 2://如果(_x2,_y2)-(_x3,_y3)水平
		x1=_x3;
		y1=_y3;
		x2=_x1;
		y2=_y1;
		x3=_x2;
		y3=_y2;
		break;
	case 3://如果(_x3,_y3)-(_x1,_y1)水平
		x1=_x1;
		y1=_y1;
		x2=_x2;
		y2=_y2;
		x3=_x3;
		y3=_y3;
		break;
	case 0:
		x1=_x1;
		y1=_y1;
		x2=_x2;
		y2=_y2;
		x3=_x3;
		y3=_y3;
		break;

	}	

	//求(x1,y1)-(x2,y2)的垂线斜率
	float k1;

	k1=-(x2-x1)/(y2-y1);//由于(x1,y1)-(x2,y2)不水平，所以y2必不等于y1
	//求(x2,y2)-(x3,y3)的垂线斜率
	float k2;
	k2=-(x3-x2)/(y3-y2);//由于(x2,y2)-(x3,y3)不水平，所以y3必不等于y2
	//求(x1,y1)-(x2,y2)中点(a,b)
	float a,b;
	a=(x1+x2)/2;
	b=(y1+y2)/2;
	//求(x2,y2)-(x3,y3)中点(c,d)
	float c,d;
	c=(x2+x3)/2;
	d=(y2+y3)/2;
	//求两条中垂线的交点（两条中垂线的方程是：y-b=k1x-ak1 和 y-d=k2x-ck2）
	x=(a*k1-c*k2+d-b)/(k1-k2);
	y=k1*x-a*k1+b;
	//求半径的平方
	r2=pow2(x-x1)+pow2(y-y1);
	return r2;

}





inline int getIndexOfp_IDtri1ToIDtri2(CIDTriangle*pIDtri1,CIDTriangle*pIDtri2)
//求*pIDtri1的第几个指针指向*pIDtri2
//如果*pIDtri1没有指向*pIDtri2的指针，则返回-1
{
	for(int i=0;i<=2;i++){
		if(pIDtri1->p[i]==pIDtri2){
			break;
		}
	}
	if(i<=2){//是break出来的，说明*pIDtri1有指针指向*pIDtri2，pIDtri1->p[i]即为此指针
		return i;
	}else{
		cout<<"error getIndexOfp_IDtri1ToIDtri2"<<endl;
		return -1;
	}
}
inline int getIndexOfVert_inIDtri1notinIDtri2(CIDTriangle*pIDtri1,CIDTriangle*pIDtri2)
//求*pIDtri1中第几个顶点不在*pIDtri2中，返回找到的第一个符合此条件的顶点号
{
//	int I[3];
//	int p=0;
	for(int i=0;i<=2;i++){
		int ID=pIDtri1->vID[i];
		//看ID是否在*pIDtri2中
		bool isin=false;
		for(int j=0;j<=2;j++){
			int _ID=pIDtri2->vID[j];
			if(ID==_ID){
				isin=true;
				break;
			}
		}//得到isin
		if(!isin){
//			I[p++]=i;
			return i;
		}
	}
	//没找到
	return -1;
//	if(p==1){
//		return I[0];
//	}else if(p==0){	
//		cout<<"errror"<<endl;
//		return -1;
//	}else{
//		cout<<"error"<<endl;
//		exit(0);
//	}

}

inline int pointInIDtri(float v[4],CIDTriangle*pIDtri,myvector<Cv4>&vlist,int&edgeID)
//判断ID号顶点是否在*pIDtri内或边界上
//输入的v的v[Y]必须是0
//返回值0,1,2,3分别代表：在外，在内，在边上，在顶点上
{
	float x=v[X];
	float y=v[Z];
	float x1=vlist[pIDtri->vID[0]].v[X];
	float y1=vlist[pIDtri->vID[0]].v[Z];
	float x2=vlist[pIDtri->vID[1]].v[X];
	float y2=vlist[pIDtri->vID[1]].v[Z];
	float x3=vlist[pIDtri->vID[2]].v[X];
	float y3=vlist[pIDtri->vID[2]].v[Z];
	int onEdgeCount=0;
	//看(x,y)是否在(x1,y1)-(x2,y2)左侧
	{
		//求(x1,y1)-(x2,y2)的右法向量
		float nx=y1-y2;//注意，在下面坐标系下，向量(x,y)的右法向量是(-y,x)
		               //        |---> x
		               //        |
		               //       \|/
		               //        z
		               //而在下面坐标系下，向量(x,y)的右法向量是(y,-x)
					   //        z
		               //       /|\
		               //        |
		               //        |
		               //        -----> x 
		float ny=x2-x1;
		//求与(x,y)-(x1,y1)同向的向量
		float vecx=x1-x;
		float vecy=y1-y;
		//看与(nx,ny)与(vecx,vecy)的点积是否为正
		float dot=nx*vecx+ny*vecy;
		if(dot<0)return 0;//不在内
		else if(dot==0){//在边上
			edgeID=0;
			onEdgeCount++;
		}
	}
	//看(x,y)是否在(x2,y2)-(x3,y3)左侧
	{
		//求(x2,y2)-(x3,y3)的右法向量
		float nx=y2-y3;
		float ny=x3-x2;
		//求与(x,y)-(x2,y2)同向的向量
		float vecx=x2-x;
		float vecy=y2-y;
		//看与(nx,ny)与(vecx,vecy)的点积是否为正
		float dot=nx*vecx+ny*vecy;
		if(dot<0)return 0;//不在内
		else if(dot==0){//在边上
			edgeID=1;
			onEdgeCount++;
		}
	}
	//看(x,y)是否在(x3,y3)-(x1,y1)左侧
	{
		//求(x3,y3)-(x1,y1)的右法向量
		float nx=y3-y1;
		float ny=x1-x3;
		//求与(x,y)-(x3,y3)同向的向量
		float vecx=x3-x;
		float vecy=y3-y;
		//看与(nx,ny)与(vecx,vecy)的点积是否为正
		float dot=nx*vecx+ny*vecy;
		if(dot<0)return 0;//不在内
		else if(dot==0){//在边上
			edgeID=2;
			onEdgeCount++;
		}
	}
	//不在外
	if(onEdgeCount==0){
		return 1;//在内
	}else if(onEdgeCount==1){
		return 2;//在边上
	}else{
		return 3;//在顶点上
	}

}
void flip(CIDTriangle*pAdjIDtri,CIDTriangle*pnewIDtri,const int ID,float v[4],myvector<Cv4>&vlist,myvector<CIDTriangle*>&pIDtriList,int&count){
	if(count>20){
		return;
	}
	//看pAdjIDtri的外接圆是否包含顶点ID（亦即v）
	if(!pAdjIDtri->circleAlreadyGet){//如果外接圆还没有求出过，求外接圆并保存
		//求pAdjIDtri的外接圆
		float x1,z1,x2,z2,x3,z3;//pAdjIDtri三个顶点的(x,z)坐标
		x1=vlist[pAdjIDtri->vID[0]].v[X];
		z1=vlist[pAdjIDtri->vID[0]].v[Z];
		x2=vlist[pAdjIDtri->vID[1]].v[X];
		z2=vlist[pAdjIDtri->vID[1]].v[Z];
		x3=vlist[pAdjIDtri->vID[2]].v[X];
		z3=vlist[pAdjIDtri->vID[2]].v[Z];
		pAdjIDtri->r2=calCircle(x1,z1,x2,z2,x3,z3,pAdjIDtri->x,pAdjIDtri->z);
		pAdjIDtri->circleAlreadyGet=true;
		
	}
	//看ID（亦即v）是否在pAdjIDtri的外接圆内
	if(pAdjIDtri->r2<0)return;
	if(_isnan(pAdjIDtri->r2))return;
	//看pAdjIDtri的外接圆是否包含v
	//求v到圆心(x,z)的距离平方
	double d2=pow2(v[X]-pAdjIDtri->x)+pow2(v[Z]-pAdjIDtri->z);
	if(d2<0)return;
	if(_isnan(d2))return;
	if(d2>=pAdjIDtri->r2)return;//ID不在pAdjIDtri的外接圆内



	//如果ID在pAdjIDtri的外接圆内
	//进行对角线更换
	//                  ID0                                   ID0
	//thirdID ----------/|                  thirdID-----------|
	//         \       / |                          \ - _1_   |       
	//          \     / ID                ==>        \      -ID   
	//           \   / /                              \  2  /  
	//            \ //                                 \  /   
	//            ID1                                  ID1
	int ID0=pnewIDtri->vID[1];
	int ID1=pnewIDtri->vID[2];
	//需要更换pAdjIDtri与pnewIDtri构成的四边形的对角线
	//求pAdjIDtri不与pIDtri公用的顶点ID
	int thirdindex=getIndexOfVert_inIDtri1notinIDtri2(pAdjIDtri,pnewIDtri);
	int thirdID=pAdjIDtri->vID[thirdindex];
	if(thirdindex==-1){
		cout<<"error thirdindex==-1"<<endl;
		return;
	}
	//用新的三角形pIDtri1,pIDtri2取代pnewIDtri和pAdjIDtri
	CIDTriangle*pIDtri1,*pIDtri2;
	pIDtri1=new CIDTriangle();
	pIDtri2=new CIDTriangle();
	//设顶点（均以ID为起点）
	pIDtri1->setvID(ID,ID0,thirdID);
	pIDtri2->setvID(ID,thirdID,ID1);
	//设指针
	pIDtri1->setp(pnewIDtri->p[0],pAdjIDtri->p[(thirdindex+2)%3],pIDtri2);
	pIDtri2->setp(pIDtri1,pAdjIDtri->p[thirdindex],pnewIDtri->p[2]);

	//外围三角形的指针修正
	//通过ID0thirdID相邻的三角形指针修正
	{
		CIDTriangle*_pAdjIDtri=pAdjIDtri->p[(thirdindex+2)%3];
		if(_pAdjIDtri!=NULL){
			//看_pAdjIDtri的哪个指针指向pAdjIDtri
			int index=getIndexOfp_IDtri1ToIDtri2(_pAdjIDtri,pAdjIDtri);
			if(index==-1)return;
			//将此指针改为指向pIDtri1
			_pAdjIDtri->p[index]=pIDtri1;
		}
		
	}
	//通过thirdIDID1相邻的三角形指针修正
	{
		CIDTriangle*_pAdjIDtri=pAdjIDtri->p[thirdindex];
		if(_pAdjIDtri!=NULL){
			//看_pAdjIDtri的哪个指针指向pAdjIDtri
			int index=getIndexOfp_IDtri1ToIDtri2(_pAdjIDtri,pAdjIDtri);
			if(index==-1)return;
			//将此指针改为指向pIDtri2
			_pAdjIDtri->p[index]=pIDtri2;
		}
		
	}
	//通过IDID0相邻的三角形指针修正
	{
		CIDTriangle*_pAdjIDtri=pnewIDtri->p[0];
		if(_pAdjIDtri!=NULL){
			//看_pAdjIDtri的哪个指针指向pnewIDtri
			int index=getIndexOfp_IDtri1ToIDtri2(_pAdjIDtri,pnewIDtri);
			if(index==-1)return;
			//将此指针改为指向pIDtri1
			_pAdjIDtri->p[index]=pIDtri1;
		}
		
	}
	//通过IDID1相邻的三角形指针修正
	{
		CIDTriangle*_pAdjIDtri=pnewIDtri->p[2];
		if(_pAdjIDtri!=NULL){
			//看_pAdjIDtri的哪个指针指向pnewIDtri
			int index=getIndexOfp_IDtri1ToIDtri2(_pAdjIDtri,pnewIDtri);
			if(index==-1)return;
			//将此指针改为指向pIDtri2
			_pAdjIDtri->p[index]=pIDtri2;	
		}
	}
	//用pIDtri1和pIDtri2替换pnewIDtri和pAdjIDtri
	//将pIDtri1加入pIDtriList
	pIDtriList.push_back(pIDtri1);
	//将pIDtri2加入pIDtriList
	pIDtriList.push_back(pIDtri2);

	//将pnewIDtri标记为removed
	pnewIDtri->removed=true;
	//将pIDtri1,pIDtri2加作pnewIDtri的孩子
	pnewIDtri->q.push_back(pIDtri1);
	pnewIDtri->q.push_back(pIDtri2);

	//将pAdjIDtri标记为removed
	pAdjIDtri->removed=true;
	pAdjIDtri->q.push_back(pIDtri1);
	pAdjIDtri->q.push_back(pIDtri2);


	//递归

	if(pIDtri1->p[1]!=NULL){
		count++;
		flip(pIDtri1->p[1],pIDtri1,ID,v,vlist,pIDtriList,count);
		count--;
	}

	

	if(pIDtri2->p[1]!=NULL){
		count++;
		flip(pIDtri2->p[1],pIDtri2,ID,v,vlist,pIDtriList,count);	
		count--;
	}

	

}

void locIDPoint(float v[4],CIDTriangle*pIDtri,myvector<Cv4>&vlist,CIDTriangle*&pTagIDtri,int&judge,int&edgeID)
//到三角树中去查找ID点位于哪个三角形，返回指向此三角形的指针pTagIDtri
//pTagIDtri最初传入必须为NULL，否则不能工作
//judge==1表示在内部，judge==2表示在边上，judge==3表示在顶点上
//注意，为了优化，这里对于落在顶点上的情况，不返回正确的pIDtri
{
	//看ID点是否位于pIDtri中
	judge=pointInIDtri(v,pIDtri,vlist,edgeID);
	if(judge==1||judge==2){//如果位于内部或边线上
		int nchildIDtri=(int)pIDtri->q.size();
		if(nchildIDtri==0){
			pTagIDtri=pIDtri;
			return;
		}else{
			//递归子节点
			for(int i=0;i<nchildIDtri;i++){
				locIDPoint(v,pIDtri->q[i],vlist,pTagIDtri,judge,edgeID);
				if(pTagIDtri!=NULL||judge==3)return;
			}
		}
	}else{
		return;
	}

}
void Delaunay(myvector<Cv4>&vlist,
			  float xmin,float xmax,float zmin,float zmax,
			  myvector<CIDTriangle*>&pIDtriList)
//Delaunay三角剖分
//输入点云mesh.vlist，和点云(x,z)范围，填充mesh.IDtriList
{
	//构造超级三角形
	//   a/2   a  a/2
	// v1_____________v3
	//   \ |   |   | /
 	//    \| b |   |/
	//     \---|---/
	//      \ b|  /
	//       \ | /
	//        \|/
	//         v2  
	//   其中a=xmax-xmin，b=zmax-zmin
	//
	//计算超级三角形的三个顶点
	float margin=10000;//margin取得足够大，以保证将来删除过含过长边的三角形时能将超级三角形删除
	float a=xmax-xmin;
	float b=zmax-zmin;
	float v1[4],v2[4],v3[4];
	init4(v1,xmin-a/2-margin,0,zmin-margin,1);
	init4(v2,(xmin+xmax)/2,0,zmin+b+b+margin,1);
	init4(v3,xmax+a/2+margin,0,zmin-margin,1);
	//将超级三角形的三个顶点v1,v2,v3加入到顶点列表
	vlist.grow();
	int ID0=(int)vlist.size()-1;
	veccopy(v1,vlist[ID0].v);

	vlist.grow();
	int ID1=ID0+1;
	veccopy(v2,vlist[ID1].v);

	vlist.grow();
	int ID2=ID1+1;
	veccopy(v3,vlist[ID2].v);

	//生成超级三角形
	CIDTriangle*pIDtri=new CIDTriangle();
	pIDtri->setvID(ID0,ID1,ID2);
	pIDtri->setp(NULL,NULL,NULL);

	//将pIDtri加入pIDtriList
	pIDtriList.push_back(pIDtri);





	//--------------------开始逐点插入
	int _nv=(int)vlist.size()-3;//最后三个顶点不再插入
	for(int ii=0;ii<_nv;ii++){
		const int ID=ii;//顶点索引号
		float v[4];
		veccopy(vlist[ID].v,v);//坐标
		v[Y]=0;
		//插入ID号顶点
		//看ID顶点位于哪个三角形
		CIDTriangle*pIDtri=NULL;//pIDtri包含ID顶点
		int judge=0;
		int edgeID=-1;
		locIDPoint(v,pIDtriList[0],vlist,pIDtri,judge,edgeID);
		if(pIDtri==NULL){
			continue;
		}

		if(judge==2){//如果落在三角形边上
			//         ID0
			//       ID/ \    
			// edgeID *   \
			//       /     \
			//      /_______\
			//    ID1       ID2 
			int ID0=pIDtri->vID[edgeID];
			int ID1=pIDtri->vID[(edgeID+1)%3];
			int ID2=pIDtri->vID[(edgeID+2)%3];
			//看边edgeID的邻居指针是否为空
			if(pIDtri->p[edgeID]==NULL){//如果edgeID的邻居指针为空
				//         ID0
				//       ID/ \    
				// edgeID /\ 0\
				//       /   \ \
				//      /__1___\\
				//    ID1       ID2 
				//pIDtri分裂为两个三角形
				//生成新三角形
				myvector<CIDTriangle*> pnewIDtri;
				pnewIDtri.resize(2);
				pnewIDtri[0]=new CIDTriangle();
				pnewIDtri[1]=new CIDTriangle();
				//为新三角形设定顶点(均以中心点ID作为起始点)
				pnewIDtri[0]->setvID(ID,ID2,ID0);
				pnewIDtri[1]->setvID(ID,ID1,ID2);
				//设定新增三角形的邻接指针
				pnewIDtri[0]->setp(pnewIDtri[1],pIDtri->p[(edgeID+2)%3],NULL);
				pnewIDtri[1]->setp(NULL,pIDtri->p[(edgeID+1)%3],pnewIDtri[0]);
				//更新pIDtri邻接三角形的指针
				//通过ID2ID0相邻的三角形指针修正
				{
					CIDTriangle*_pAdjIDtri=pIDtri->p[(edgeID+2)%3];
					if(_pAdjIDtri!=NULL){
						//看_pAdjIDtri的哪个指针指向pIDtri
						int index=getIndexOfp_IDtri1ToIDtri2(_pAdjIDtri,pIDtri);
						//将此指针改为指向pnewIDtri[0]
						if(index==-1)continue;
						_pAdjIDtri->p[index]=pnewIDtri[0];
					}

				}
				//通过ID1ID2相邻的三角形指针修正
				{
					CIDTriangle*_pAdjIDtri=pIDtri->p[(edgeID+1)%3];
					if(_pAdjIDtri!=NULL){
						//看_pAdjIDtri的哪个指针指向pIDtri
						int index=getIndexOfp_IDtri1ToIDtri2(_pAdjIDtri,pIDtri);
						if(index==-1)continue;
						//将此指针改为指向pnewIDtri[0]
						_pAdjIDtri->p[index]=pnewIDtri[1];
					}

				}
				//将新增三角形加入pIDtriList
				pIDtriList.push_back(pnewIDtri[0]);
				pIDtriList.push_back(pnewIDtri[1]);
				
				//将pIDtri标记为removed
				pIDtri->removed=true;
				//将pnewIDtri[0]~pnewIDtri[1]加作pIDtri的孩子
				pIDtri->q.push_back(pnewIDtri[0]);
				pIDtri->q.push_back(pnewIDtri[1]);
						int count=0;
				//依次检查各pnewIDtri的p[1]的两个邻边三角形，看其外接圆是否包含ID顶点，如果包含，则更换对角线
				for(int i=0;i<=1;i++){
					if(pnewIDtri[i]->p[1]!=NULL){
								count++;
						flip(pnewIDtri[i]->p[1],pnewIDtri[i],ID,v,vlist,pIDtriList,count);
								count--;
					}
					
				}


			}else{//如果edgeID的邻居指针不空
				// thirdID _________ID0
				//         \---2__ID/ \    
				//pAdjIDtri \edgeID/\ 0\  pIDtri
				//           \ 3  /   \ \
				//            \  /__1___\\
				//            ID1        ID2 
				CIDTriangle* pAdjIDtri=pIDtri->p[edgeID];
				int thirdindex=getIndexOfVert_inIDtri1notinIDtri2(pAdjIDtri,pIDtri);
				int thirdID=pAdjIDtri->vID[thirdindex];
				if(thirdindex==-1){
					cout<<"error thirdindex==-1"<<endl;
					continue;
				}
				//生成四个新三角形
				myvector<CIDTriangle*> pnewIDtri;
				pnewIDtri.resize(4);
				pnewIDtri[0]=new CIDTriangle();
				pnewIDtri[1]=new CIDTriangle();
				pnewIDtri[2]=new CIDTriangle();
				pnewIDtri[3]=new CIDTriangle();
				//为新三角形设定顶点(均以中心点ID作为起始点)
				pnewIDtri[0]->setvID(ID,ID2,ID0);
				pnewIDtri[1]->setvID(ID,ID1,ID2);
				pnewIDtri[2]->setvID(ID,ID0,thirdID);
				pnewIDtri[3]->setvID(ID,thirdID,ID1);
				//设定新增三角形的邻接指针
				pnewIDtri[0]->setp(pnewIDtri[1],pIDtri->p[(edgeID+2)%3],pnewIDtri[2]);
				pnewIDtri[1]->setp(pnewIDtri[3],pIDtri->p[(edgeID+1)%3],pnewIDtri[0]);
				pnewIDtri[2]->setp(pnewIDtri[0],pAdjIDtri->p[(thirdindex+2)%3],pnewIDtri[3]);
				pnewIDtri[3]->setp(pnewIDtri[2],pAdjIDtri->p[thirdindex],pnewIDtri[1]);
				//更新pIDtri邻接三角形的指针
				//通过ID2ID0相邻的三角形指针修正
				{
					CIDTriangle*_pAdjIDtri=pIDtri->p[(edgeID+2)%3];
					if(_pAdjIDtri!=NULL){
						//看_pAdjIDtri的哪个指针指向pIDtri
						int index=getIndexOfp_IDtri1ToIDtri2(_pAdjIDtri,pIDtri);
						if(index==-1)continue;
						//将此指针改为指向pnewIDtri[0]
						_pAdjIDtri->p[index]=pnewIDtri[0];
					}

				}
				//通过ID1ID2相邻的三角形指针修正
				{
					CIDTriangle*_pAdjIDtri=pIDtri->p[(edgeID+1)%3];
					if(_pAdjIDtri!=NULL){
						//看_pAdjIDtri的哪个指针指向pIDtri
						int index=getIndexOfp_IDtri1ToIDtri2(_pAdjIDtri,pIDtri);
						if(index==-1)continue;
						//将此指针改为指向pnewIDtri[0]
						_pAdjIDtri->p[index]=pnewIDtri[1];
					}

				}
				//通过ID0thirdID相邻的三角形指针修正
				{
					CIDTriangle*_pAdjIDtri=pAdjIDtri->p[(thirdindex+2)%3];
					if(_pAdjIDtri!=NULL){
						//看_pAdjIDtri的哪个指针指向pAdjIDtri
						int index=getIndexOfp_IDtri1ToIDtri2(_pAdjIDtri,pAdjIDtri);
						if(index==-1)continue;
						//将此指针改为指向pnewIDtri[2]
						_pAdjIDtri->p[index]=pnewIDtri[2];
					}

				}
				//通过thirdIDID1相邻的三角形指针修正
				{
					CIDTriangle*_pAdjIDtri=pAdjIDtri->p[thirdindex];
					if(_pAdjIDtri!=NULL){
						//看_pAdjIDtri的哪个指针指向pAdjIDtri
						int index=getIndexOfp_IDtri1ToIDtri2(_pAdjIDtri,pAdjIDtri);
						if(index==-1)continue;
						//将此指针改为指向pnewIDtri[0]
						_pAdjIDtri->p[index]=pnewIDtri[3];
					}
				
				}


				//将新增三角形加入pIDtriList
				pIDtriList.push_back(pnewIDtri[0]);
				pIDtriList.push_back(pnewIDtri[1]);
				pIDtriList.push_back(pnewIDtri[2]);
				pIDtriList.push_back(pnewIDtri[3]);
				
				//将pIDtri标记为removed
				pIDtri->removed=true;
				pAdjIDtri->removed=true;
				//将pnewIDtri[0]~pnewIDtri[1]加作pIDtri的孩子
				pIDtri->q.push_back(pnewIDtri[0]);
				pIDtri->q.push_back(pnewIDtri[1]);
				//将pnewIDtri[2]~pnewIDtri[3]加作pAdjIDtri的孩子
				pAdjIDtri->q.push_back(pnewIDtri[2]);
				pAdjIDtri->q.push_back(pnewIDtri[3]);
						int count=0;
				//依次检查各pnewIDtri的p[1]，看其外接圆是否包含ID顶点，如果包含，则更换对角线
				for(int i=0;i<=3;i++){
					
					if(pnewIDtri[i]->p[1]!=NULL){
								count++;
						flip(pnewIDtri[i]->p[1],pnewIDtri[i],ID,v,vlist,pIDtriList,count);
								count--;
					}
					
				}	
			}
		}else if(judge==1){//如果落在三角形内
			//将v插入到pIDtri内
			//直接将pIDtri分裂为三个三角形
			//           ID0      
			//           /|\
			//          / | \
			//         / ID  \
			//        / /   \ \
			//       //       \\
			//     ID1 ------- ID2
			//pIDtri的三个顶点
			int ID0=pIDtri->vID[0];
			int ID1=pIDtri->vID[1];
			int ID2=pIDtri->vID[2];
			//生成三个新三角形
			myvector<CIDTriangle*> pnewIDtri;
			pnewIDtri.resize(3);
			pnewIDtri[0]=new CIDTriangle();
			pnewIDtri[1]=new CIDTriangle();
			pnewIDtri[2]=new CIDTriangle();
			//为三个新三角形设定顶点(均以中心点ID作为起始点)
			pnewIDtri[0]->setvID(ID,ID0,ID1);
			pnewIDtri[1]->setvID(ID,ID1,ID2);
			pnewIDtri[2]->setvID(ID,ID2,ID0);
			//设定新增三角形的邻接指针
			pnewIDtri[0]->setp(pnewIDtri[2],pIDtri->p[0],pnewIDtri[1]);
			pnewIDtri[1]->setp(pnewIDtri[0],pIDtri->p[1],pnewIDtri[2]);
			pnewIDtri[2]->setp(pnewIDtri[1],pIDtri->p[2],pnewIDtri[0]);
			//更新pIDtri邻接三角形的指针
			bool error=false;
			for(int i=0;i<=2;i++){
				//更新pIDtri->p[i]的指针
				CIDTriangle*_pAdjIDtri=pIDtri->p[i];
				//更新_pAdjIDtri的指针
				if(_pAdjIDtri!=NULL){
					//看_pAdjIDtri的哪个指针指向pIDtri
					int index=getIndexOfp_IDtri1ToIDtri2(_pAdjIDtri,pIDtri);
					if(index==-1){
						error=true;
						break;
					}
					//将此指针指向pnewIDtri[i]
					_pAdjIDtri->p[index]=pnewIDtri[i];
				}
			}
			if(error)continue;
			//将新增三角形加入pIDtriList
			pIDtriList.push_back(pnewIDtri[0]);
			pIDtriList.push_back(pnewIDtri[1]);
			pIDtriList.push_back(pnewIDtri[2]);
			
			//将pIDtri标记为removed
			pIDtri->removed=true;
			//将pnewIDtri[0]~pnewIDtri[2]加作pIDtri的孩子
			pIDtri->q.push_back(pnewIDtri[0]);
			pIDtri->q.push_back(pnewIDtri[1]);
			pIDtri->q.push_back(pnewIDtri[2]);
					int count=0;
			//依次检查各pnewIDtri的p[1]的三个邻边三角形，看其外接圆是否包含ID顶点，如果包含，则更换对角线
			for(i=0;i<=2;i++){
				
				if(pnewIDtri[i]->p[1]!=NULL){
							count++;
					flip(pnewIDtri[i]->p[1],pnewIDtri[i],ID,v,vlist,pIDtriList,count);
							count--;
				}
				
			}
		}else{//judge==3，表示落在顶点上
			
			continue;
		}
	}
	//删除与超级三角形顶点相关的三角形
	//搜索pIDtriList，将含有ID0,ID1,ID2之一的三角形标为removed
	int nIDtri=(int)pIDtriList.size();
	for(int i=0;i<nIDtri;i++){
		CIDTriangle*pIDtri=pIDtriList[i];
		int _ID0,_ID1,_ID2;//pIDtri的三个顶点ID
		_ID0=pIDtri->vID[0];
		_ID1=pIDtri->vID[1];
		_ID2=pIDtri->vID[2];
		if(_ID0==ID0||_ID0==ID1||_ID0==ID2
			||_ID1==ID0||_ID1==ID1||_ID1==ID2
			||_ID2==ID0||_ID2==ID1||_ID2==ID2){
			pIDtri->removed=true;
		}
	}
	//删除_vlist中超级三角形三个顶点
	vlist.pop_back();
	vlist.pop_back();
	vlist.pop_back();
}
