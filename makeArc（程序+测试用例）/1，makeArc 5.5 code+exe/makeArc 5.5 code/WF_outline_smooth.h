//----------------------------------------
//����ƽ��
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
			//��polygen�������ƽ��
			{
				const float wmid=16;
				polygenSmooth(polygen,wmid);
			}
			//����polygen�Ķ��㷨����
			{
				calNorm_vertsOfPolygen(polygen);
			}
			//��ǽǵ�isCornerPoint
			{
				markCornerPoint(polygen,6,0.93,true);//A>=20��
			}
			//��Ǻ��Ľǵ�isCoreCornerPoint
			{
				markCoreCornerPoint(polygen,5,0.34,true);//A>=70��
			}
			//�����ǵ�ƽ��
			{
				polygenSmooth_protectCorner(polygen,1.5,20);
			}
			//����������ǰ����������������ƽ��ʱҪ�ã�
			{
				calNormfAndNormn(polygen);
			}
			//���ӽ�ƽ������ֱ
			{
				pullStraight(polygen,0.95,50);

			}
			//���¼�������㷨��������Ϊǰ�涥�㷢�����ƶ������Է�������Ҫ���£�
			{
				calNorm_vertsOfPolygen(polygen);

			}
			//�����ǵ�ƽ��
			{
				polygenSmooth_protectCorner(polygen,1,20);

			}
		
			//���¼�������㷨��������Ϊǰ�涥�㷢�����ƶ������Է�������Ҫ���£�
			{
				calNorm_vertsOfPolygen(polygen);
			}
			//���±��isCornerPoint
			{
				markCornerPoint(polygen,6,0.984);//A>=10��

			}
			//���±��isCoreCornerPoint
			{
				markCoreCornerPoint(polygen,2,0.5);//A>=60��

			}
			//��polygen��û�б�ǵĶ���ɾ����Ȼ����в�ֵʹ����ָ�Ϊ�ȼ��
			{
				ridCornerAndInterp(polygen);
			}

		}
	}
	//��ƽ�����������polygenList_smoothRes����
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
//��polygen����ƽ�����м�Ȩ��Ϊwmid
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
//��ǽǵ�isCornerPoint
//AΪ�������߷���н���ֵ
{
	int nv=(int)polygen.vlist.size();
	for(int k=0;k<nv;k++){
		Cv4Ex&v=polygen.vlist[k];
		myvector<Cv4Ex> vlistf;//v��ǰ��nr������б�
		myvector<Cv4Ex> vlistn;//v�ĺ���nr������б�
		//����vlistf
		int index=k;
		for(int u=1;u<=nr;u++){
			index--;
			if(index<0)index=nv-1;
			vlistf.push_back(polygen.vlist[index]);//%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		}
		//����vlistn
		for(u=1;u<=nr;u++){
			vlistn.push_back(polygen.vlist[(k+u)%nv]);
		}
		//��vlistf�и����ƽ��������
		float normf[4]={0,0,0,0};//vlistf�и����ƽ��������
		for(u=0;u<nr;u++){
			add(normf,vlistf[u].norm,normf);
		}
		normalize(normf);//���µ�λ��
		//��vlistn�и����ƽ��������
		float normn[4]={0,0,0,0};//vlistn�и����ƽ��������
		for(u=0;u<nr;u++){
			add(normn,vlistn[u].norm,normn);
		}
		normalize(normn);//���µ�λ��
		//��normf��normn�ļнǣ�����ϴ󣬾ͱ�ΪisCornerPoint
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
//��Ǻ��Ľǵ�isCoreCornerPoint
//AΪ�������߷���н���ֵ
{
	
	
	int nv=(int)polygen.vlist.size();
	for(int k=0;k<nv;k++){
		Cv4Ex&v=polygen.vlist[k];
		myvector<Cv4Ex> vlistf;//v��ǰ��nr������б�
		myvector<Cv4Ex> vlistn;//v�ĺ���nr������б�
		//����vlistf
		int index=k;
		for(int u=1;u<=nr;u++){
			index--;
			if(index<0)index=nv-1;
			vlistf.push_back(polygen.vlist[index]);//%%%%%%%%%%%%%%%%%%%
		}
		//����vlistn
		for(u=1;u<=nr;u++){
			vlistn.push_back(polygen.vlist[(k+u)%nv]);
		}
		//��vlistf�и����ƽ��������
		float normf[4]={0,0,0,0};//vlistf�и����ƽ��������
		for(u=0;u<nr;u++){
			add(normf,vlistf[u].norm,normf);
		}
		normalize(normf);//���µ�λ��
		//��vlistn�и����ƽ��������
		float normn[4]={0,0,0,0};//vlistn�и����ƽ��������
		for(u=0;u<nr;u++){
			add(normn,vlistn[u].norm,normn);
		}
		normalize(normn);//���µ�λ��
		//��normf��normn�ļнǣ�����ϴ󣬾ͱ�ΪisCornerPoint
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
				v.isCornerPoint=false;//һ��Ҫ��isCoreCornerPoint==true�ĵ��isCornerPoint��Ϊfalse
			}
		}else{
			v.isCoreCornerPoint=false;
		}
		
	}
}

inline void polygenSmooth_protectCorner(Cpolygen&polygen,const float weight_corner,const float weight_coreCorner)
//�����ǵ�ƽ��
{
	//���ݱ����̶Ȳ�ͬ����ƽ��
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
						 int nv_notDeal//�������������nv_notDeal����������
						 )
//��polygen�й�ƽ�Ľ�����ƽ��
//AΪ�������ҷ������н���ֵ
{
	
	int nv=(int)polygen.vlist.size();
	if(nv<nv_notDeal)return;//����������̫���߲�����ֱ
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
			//��Ҫ�ж�v���Ƿ�Ϊ͹��
			float normVertical[4];
			cross(v.normf,v.normn,normVertical);
			if(normVertical[Y]>=0
				||(normVertical[Y]<0&&-normVertical[Y]<0.3)){//��͹�ǻ��С�İ���
				polygen.vlist.erase(&polygen.vlist[k]);
				k--;
			}
		}
		
	}
	//���в�ֵ
	Cpolygen _polygen;//����ֵ������浽_polygen
	//���в�ֵ
	int nv=(int)polygen.vlist.size();
	float mind=2;//��ֵ���
	for(k=0;k<nv;k++){
		Cv4Ex v=polygen.vlist[k];
		_polygen.vlist.push_back(v);
		Cv4Ex vn=polygen.vlist[(k+1)%nv];
		//�����ֵ�����
		float d=fabs(v.v[X]-vn.v[X])+fabs(v.v[Y]-vn.v[Y])+fabs(v.v[Z]-vn.v[Z]);//v��vn�������پ���
		int n=max(d/mind-1,0);//��ֵ�����
		//��n����ֵ��
		for(int u=1;u<=n;u++){
			//��u����ֵ�㽫�߶�v-vn�ֳ�u:(n+1-u)����
			float vec[4];
			sub(vn.v,v.v,vec);
			mul(((float)u)/(n+1),vec,vec);
			Cv4Ex v_insert;//��ֵ��
			add(v.v,vec,v_insert.v);
			//��v_insert���뵽_polygen
			_polygen.vlist.push_back(v_insert);
		}
		
	}//�õ�_polygen
	//��_polygen�滻polygen
	polygen=_polygen;
}

inline void calNorm_vertsOfPolygen(Cpolygen&polygen){
	int nv=(int)polygen.vlist.size();
	for(int k=0;k<nv;k++){
		Cv4Ex&v=polygen.vlist[k];
		float *vf=polygen.vlist[(k-1+nv)%nv].v;//%%%%%%%%%%%%%%%
		float *vn=polygen.vlist[(k+1)%nv].v;
		//��vfv���ҷ�����
		float vfv[4];
		sub(v.v,vf,vfv);
		float norm1[4]={-vfv[Z],0,vfv[X],0};
		normalize(norm1);
		//��vvn���ҷ�����
		float vvn[4];
		sub(vn,v.v,vvn);
		float norm2[4]={-vvn[Z],0,vvn[X],0};
		normalize(norm2);
		//��norm1,norm2��ƽ��������
		float norm[4]={0,0,0,0};//ƽ��������
		add(norm1,norm,norm);
		add(norm2,norm,norm);
		normalize(norm);//�����µ�λ��
		//��norm��Ϊ��v�ĵ㷨����
		veccopy(norm,v.norm);
		
	}

}
