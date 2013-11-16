//----------------------------------------
//轮廓平滑
//----------------------------------------
inline void polygenSmooth(Cpolygen&,const float);
inline void markCornerPoint(Cpolygen&,int,float,bool cosAShouldBigerThanZero=false);
inline void markCoreCornerPoint(Cpolygen&,int,float,bool cosAShouldBigerThanZero=false);
inline void polygenSmooth_protectCorner(Cpolygen&,const float,const float);
inline void pullStraight(Cpolygen&,float,int);
inline void ridCornerAndInterp(Cpolygen&);
inline void calNorm_vertsOfPolygen(Cpolygen&);

void main_outline_smooth(myvector<Ccloud>&cloudList)
{

		
	int ncloud=(int)cloudList.size();
	for(int i=0;i<ncloud;i++){
		Ccloud&cloud=cloudList[i];
		if(cloud.removed)continue;
		for(int j=0;j<(int)cloud.polygenList.size();j++){
			Cpolygen&polygen=cloud.polygenList[j];
			//对polygen进行轻度平滑
			{
				const float wmid=16;
				polygenSmooth(polygen,wmid);
			}
			//计算polygen的顶点法向量
			{
				calNorm_vertsOfPolygen(polygen);
			}
			//标记角点isCornerPoint
			{
				markCornerPoint(polygen,6,0.93,true);//A>=20度
			}
			//标记核心角点isCoreCornerPoint
			{
				markCoreCornerPoint(polygen,5,0.34,true);//A>=70度
			}
			//保护角点平滑
			{
				polygenSmooth_protectCorner(polygen,1.5,20);
			}
			//计算各顶点的前后法向量（下面拉成平角时要用）
			{
				calNormfAndNormn(polygen);
			}
			//将接近平角者拉直
			{
				pullStraight(polygen,0.95,50);

			}
			//重新计算各顶点法向量（因为前面顶点发生了移动，所以法向量需要更新）
			{
				calNorm_vertsOfPolygen(polygen);

			}
			//保护角点平滑
			{
				polygenSmooth_protectCorner(polygen,1,20);

			}
		
			//重新计算各顶点法向量（因为前面顶点发生了移动，所以法向量需要更新）
			{
				calNorm_vertsOfPolygen(polygen);
			}
			//重新标记isCornerPoint
			{
				markCornerPoint(polygen,6,0.984);//A>=10度

			}
			//重新标记isCoreCornerPoint
			{
				markCoreCornerPoint(polygen,2,0.5);//A>=60度

			}
			//将polygen中没有标记的顶点删除，然后进行插值使顶点恢复为等间距
			{
				ridCornerAndInterp(polygen);
			}

		}
	}
	//将平滑结果拷贝给polygenList_smoothRes留底
	if(saveSmooth==true)
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			cloud.polygenList_smoothRes=cloud.polygenList;
		}
	}




}

inline void polygenSmooth(Cpolygen&polygen,const float wmid)
//对polygen进行平滑，中间权重为wmid
{
	int nv=(int)polygen.vlist.size();
	for(int k=0;k<nv;k++){
		float *vf=polygen.vlist[(k-1+nv)%nv].v;//%%%%%%%%%%%%%%%%%%%%%%%%%
		Cv4Ex&v=polygen.vlist[k];
		float *vn=polygen.vlist[(k+1)%nv].v;
		v.v[X]=(vf[X]+wmid*v.v[X]+vn[X])/(wmid+2);
		v.v[Z]=(vf[Z]+wmid*v.v[Z]+vn[Z])/(wmid+2);
	}
}
inline void markCornerPoint(Cpolygen&polygen,int nr,float cosA_ref,bool cosAShouldBigerThanZero)
//标记角点isCornerPoint
//A为相邻两边法向夹角阈值
{
	int nv=(int)polygen.vlist.size();
	for(int k=0;k<nv;k++){
		Cv4Ex&v=polygen.vlist[k];
		myvector<Cv4Ex> vlistf;//v的前面nr个点的列表
		myvector<Cv4Ex> vlistn;//v的后面nr个点的列表
		//制作vlistf
		int index=k;
		for(int u=1;u<=nr;u++){
			index--;
			if(index<0)index=nv-1;
			vlistf.push_back(polygen.vlist[index]);//%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		}
		//制作vlistn
		for(u=1;u<=nr;u++){
			vlistn.push_back(polygen.vlist[(k+u)%nv]);
		}
		//求vlistf中各点的平均法向量
		float normf[4]={0,0,0,0};//vlistf中各点的平均法向量
		for(u=0;u<nr;u++){
			add(normf,vlistf[u].norm,normf);
		}
		normalize(normf);//重新单位化
		//求vlistn中各点的平均法向量
		float normn[4]={0,0,0,0};//vlistn中各点的平均法向量
		for(u=0;u<nr;u++){
			add(normn,vlistn[u].norm,normn);
		}
		normalize(normn);//重新单位化
		//看normf和normn的夹角，如果较大，就标为isCornerPoint
		float cosA=dot(normf,normn);
		bool condition;
		if(cosAShouldBigerThanZero){
			condition=(cosA<=cosA_ref&&cosA>=0);
		}else{
			condition=(cosA<=cosA_ref);	
		}
		if(condition){
			v.isCornerPoint=true;
		}else{
			v.isCornerPoint=false;
		}
		
	}
	
	
}

inline void markCoreCornerPoint(Cpolygen&polygen,int nr,float cosA_ref,bool cosAShouldBigerThanZero)
//标记核心角点isCoreCornerPoint
//A为相邻两边法向夹角阈值
{
	
	
	int nv=(int)polygen.vlist.size();
	for(int k=0;k<nv;k++){
		Cv4Ex&v=polygen.vlist[k];
		myvector<Cv4Ex> vlistf;//v的前面nr个点的列表
		myvector<Cv4Ex> vlistn;//v的后面nr个点的列表
		//制作vlistf
		int index=k;
		for(int u=1;u<=nr;u++){
			index--;
			if(index<0)index=nv-1;
			vlistf.push_back(polygen.vlist[index]);//%%%%%%%%%%%%%%%%%%%
		}
		//制作vlistn
		for(u=1;u<=nr;u++){
			vlistn.push_back(polygen.vlist[(k+u)%nv]);
		}
		//求vlistf中各点的平均法向量
		float normf[4]={0,0,0,0};//vlistf中各点的平均法向量
		for(u=0;u<nr;u++){
			add(normf,vlistf[u].norm,normf);
		}
		normalize(normf);//重新单位化
		//求vlistn中各点的平均法向量
		float normn[4]={0,0,0,0};//vlistn中各点的平均法向量
		for(u=0;u<nr;u++){
			add(normn,vlistn[u].norm,normn);
		}
		normalize(normn);//重新单位化
		//看normf和normn的夹角，如果较大，就标为isCornerPoint
		float cosA=dot(normf,normn);
		bool condition;
		if(cosAShouldBigerThanZero){
			condition=(cosA<=cosA_ref&&cosA>=0);
		}else{
			condition=(cosA<=cosA_ref);	
		}
		if(condition){
			v.isCoreCornerPoint=true;
			if(v.isCornerPoint==true){
				v.isCornerPoint=false;//一定要把isCoreCornerPoint==true的点的isCornerPoint标为false
			}
		}else{
			v.isCoreCornerPoint=false;
		}
		
	}
}

inline void polygenSmooth_protectCorner(Cpolygen&polygen,const float weight_corner,const float weight_coreCorner)
//保护角点平滑
{
	//根据保护程度不同进行平滑
	int nv=(int)polygen.vlist.size();
	for(int k=0;k<nv;k++){
		float *vf=polygen.vlist[(k-1+nv)%nv].v;//%%%%%%%%%%%%%%%%%
		Cv4Ex&v=polygen.vlist[k];
		float *vn=polygen.vlist[(k+1)%nv].v;
		float weight;
		if(v.isCoreCornerPoint){
			weight=weight_coreCorner;
		}else if(v.isCornerPoint){
			weight=weight_corner;
		}else{
			weight=1;
		}
		v.v[X]=(vf[X]+weight*v.v[X]+vn[X])/(weight+2);
		v.v[Z]=(vf[Z]+weight*v.v[Z]+vn[Z])/(weight+2);
		
		
	}
}

inline void pullStraight(Cpolygen&polygen,float cosA_ref,
						 int nv_notDeal//如果顶点数少于nv_notDeal，则不作处理
						 )
//将polygen中过平的角作成平角
//A为顶点左右法向量夹角阈值
{
	
	int nv=(int)polygen.vlist.size();
	if(nv<nv_notDeal)return;//对于轮廓线太短者不作拉直
	for(int k=0;k<nv;k++){
		float *vf=polygen.vlist[(k-1+nv)%nv].v;//%%%%%%%%%%%%%%%%%%%%%%%%%
		Cv4Ex&v=polygen.vlist[k];
		float *vn=polygen.vlist[(k+1)%nv].v;	
		float cosA=dot(v.normf,v.normn);
		if(cosA>=cosA_ref){
			add(vf,vn,v.v);
			mul(0.5,v.v,v.v);	
		}	
	}
}
inline void ridCornerAndInterp(Cpolygen&polygen){
	for(int k=0;k<(int)polygen.vlist.size();k++){
		Cv4Ex&v=polygen.vlist[k];
		if(!v.isCornerPoint&&!v.isCoreCornerPoint){
			//还要判断v处是否为凸角
			float normVertical[4];
			cross(v.normf,v.normn,normVertical);
			if(normVertical[Y]>=0
				||(normVertical[Y]<0&&-normVertical[Y]<0.3)){//是凸角或很小的凹角
				polygen.vlist.erase(&polygen.vlist[k]);
				k--;
			}
		}
		
	}
	//进行插值
	Cpolygen _polygen;//将插值结果保存到_polygen
	//进行插值
	int nv=(int)polygen.vlist.size();
	float mind=2;//差值间距
	for(k=0;k<nv;k++){
		Cv4Ex v=polygen.vlist[k];
		_polygen.vlist.push_back(v);
		Cv4Ex vn=polygen.vlist[(k+1)%nv];
		//计算插值点个数
		float d=fabs(v.v[X]-vn.v[X])+fabs(v.v[Y]-vn.v[Y])+fabs(v.v[Z]-vn.v[Z]);//v到vn的曼哈顿距离
		int n=max(d/mind-1,0);//插值点个数
		//求n个插值点
		for(int u=1;u<=n;u++){
			//第u个插值点将线段v-vn分成u:(n+1-u)两段
			float vec[4];
			sub(vn.v,v.v,vec);
			mul(((float)u)/(n+1),vec,vec);
			Cv4Ex v_insert;//插值点
			add(v.v,vec,v_insert.v);
			//将v_insert加入到_polygen
			_polygen.vlist.push_back(v_insert);
		}
		
	}//得到_polygen
	//用_polygen替换polygen
	polygen=_polygen;
}

inline void calNorm_vertsOfPolygen(Cpolygen&polygen){
	int nv=(int)polygen.vlist.size();
	for(int k=0;k<nv;k++){
		Cv4Ex&v=polygen.vlist[k];
		float *vf=polygen.vlist[(k-1+nv)%nv].v;//%%%%%%%%%%%%%%%
		float *vn=polygen.vlist[(k+1)%nv].v;
		//求vfv的右法向量
		float vfv[4];
		sub(v.v,vf,vfv);
		float norm1[4]={-vfv[Z],0,vfv[X],0};
		normalize(norm1);
		//求vvn的右法向量
		float vvn[4];
		sub(vn,v.v,vvn);
		float norm2[4]={-vvn[Z],0,vvn[X],0};
		normalize(norm2);
		//求norm1,norm2的平均法向量
		float norm[4]={0,0,0,0};//平均法向量
		add(norm1,norm,norm);
		add(norm2,norm,norm);
		normalize(norm);//需重新单位化
		//将norm作为点v的点法向量
		veccopy(norm,v.norm);
		
	}

}
