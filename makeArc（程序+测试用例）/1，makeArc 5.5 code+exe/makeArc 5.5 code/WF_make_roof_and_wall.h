//----------------------------------------------
//���ݶ���ǽ����
//----------------------------------------------
inline void makeOOBBgridForCloud(Ccloud&);
inline void markEmptyCellAndNeedcutCell(Ccloud&);
inline float cal_topH_useInnerLayerID(const Ccloud&);
inline void calUVForWallPoint(const float[4],const float[4],const float[4],const float,float[2]);

inline void main_make_roof_and_wall(myvector<Ccloud>&cloudList,const Cgroundgrid&GroundGrid,bool ignoreRidTree_baseOnCornerMark,bool useRealRoof){
	cout<<"::��������"<<endl;
	//��ÿ��cloud��������
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			//Ϊcloud��ƽ���������������
			{
				makeOOBBgridForCloud(cloud);
			}
			//����ȱʧԪ�أ����ж�飬����Ϊֹ��
			{
				while(1){
					int n=cloud.grid.makeUp_once();
					if(n==0)break;
				}
			}
			//�ø���ı߽��߶������ĵ�߶���ͬ
			{
				cloud.grid.initOcH();
				           
			}
			//������߽�㼰���ĵ�����任����������ϵ
			{
				cloud.grid.transformBack();
			}
		}	
	}
	//-------------------------------------------�ͷŲ����ڴ�
	cout<<"::�ͷ�vlist"<<endl;
	//ɾ����cloud��vlist������removed��ҲҪɾ�������Դ˶�Ҫ���У�
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			//if(cloud.removed)continue;//removedҲҪɾ��
			cloud.vlist.swap(myvector<Cv4>());
		}
	}
	//----------------------------------------------------------
	cout<<"::�����ݶ�"<<endl;
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			//���empty���������⣩��needcut������������
			{
				markEmptyCellAndNeedcutCell(cloud);
			}
			//ȥ��
			if(ignoreRidTree_baseOnCornerMark){
				//����ȥ��
			}else{
				//���渧ƽ
				cloud.grid.roofsmooth(false);
				//���㷨����
				cloud.grid.calNorms();
				//�жϽǵ�cell
				cloud.grid.markCorner();
				//���cloud��gird�нǵ�cell�ĸ�������һ������������cloudΪ��
				cloud.removed=cloud.grid.isTree();
				//�����нǵ�cell�ı��ȥ��
				cloud.grid.UnmarkCorner();
				//���removed������������Ϊ�ղ�cloud���ܸոձ��ж�Ϊremoved��
				if(cloud.removed)continue;
			}
			if(useRealRoof){
			/*	//������ƽ��
				{
					//����ƽ��
					{
						//�߶�ֵƽ��
						cloud.grid.Hsmooth(10);
						//����������ı߽��
						//	cloud.grid.updateOcH();
						//���渧ƽ
						cloud.grid.roofsmooth(false);	
					}
					//���㷨����
					{
						cloud.grid.calNorms();
						
					}
					//�жϽǵ�cell
					{
						cloud.grid.markCorner();
					}
					//�ٴν���ƽ��
					{
						//�߶�ֵƽ��
						cloud.grid.Hsmooth(1);
						//����������ı߽��
						//	cloud.grid.updateOcH();
						//����ƽ��
						cloud.grid.roofsmooth(false);	
					}
					//���¼��㷨����
					{
						cloud.grid.calNorms();
					}
					
				}*/
				//��ƽ��
				{
					//����ƽ��
					{
						//�߶�ֵƽ��
						cloud.grid.Hsmooth(2);
						//����������ı߽��
						//	cloud.grid.updateOcH();
						//����ƽ��
						cloud.grid.roofsmooth(false);	
					}
					//���¼��㷨����
					{
						cloud.grid.calNorms();
					}

				}
			}else{//!useRealRoof
				//��������innerLayerID
				{
					cloud.grid.calInnerLayerID();
				}
				//����cloud.topH������ʹ��innerLayerID��
				{
					cloud.topH=cal_topH_useInnerLayerID(cloud);
					
				}
				//��innerLayerID���߶�
				{
					cloud.grid.fillH_innerLayerID(cloud.bottomH,cloud.topH);
					
				}
				//���渧ƽ
				{
					//��Ϊ���ĸ�߶ȷ����˱��ʸı䣬����Ҫ���³�ʼ���߽��߶�
					cloud.grid.initOcH();
					//	//��ǽ�Ծ�ͱ�Ե
					//	cloud.grid.markJump();
					//���渧ƽ
					cloud.grid.roofsmooth(false);
				}
				//�����������
				{
					cloud.grid.calNorms();
					
				}
			}

		}

	}
	


	//-------------------------------------------------------------------------------------------------
	cout<<"::����ǽ��"<<endl;
	//����ǽ���棨����polygenList�߶Ȳ�����polygenList_dn���˶��߼�������ǽ���棩
	{

		//���������������
		float zmin_g=GroundGrid.zmin;
		float xmin_g=GroundGrid.xmin;
		float zstepLen_g=GroundGrid.zstepLen;
		float xstepLen_g=GroundGrid.xstepLen;
		int m_mat_g=GroundGrid.m_mat;
		int n_mat_g=GroundGrid.n_mat;
		//��ʼ����ǽ����
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;

			//��cloud.polygenList�и���Yֵ��Ϊ��������Ĳ�ֵ�߶�		
			int npolygen=(int)cloud.polygenList.size();
			for(int j=0;j<npolygen;j++){
				Cpolygen&polygen=cloud.polygenList[j];
				//��polygen�и���Yֵ��Ϊ��������Ĳ�ֵ�߶�
				int nv=(int)polygen.vlist.size();
				for(int k=0;k<nv;k++){
					Cv4Ex&v=polygen.vlist[k];
					v.v[Y]=cloud.grid.getH_vIsWorldPoint(v.v);
				}
			}

			//��cloud.polygenList_dn�и���Yֵ��Ϊ�����������Ĳ�ֵ�߶�
			cloud.polygenList_dn=cloud.polygenList;//����polygenListΪpolygenList_dn
			int npolygen_dn=(int)cloud.polygenList_dn.size();
			for(j=0;j<npolygen_dn;j++){
				Cpolygen&polygen_dn=cloud.polygenList_dn[j];
				int nv=(int)polygen_dn.vlist.size();
				for(int k=0;k<nv;k++){
					Cv4Ex&v=polygen_dn.vlist[k];
					v.v[Y]=GroundGrid.getH(v.v);	
				}
			}

			//ΪpolygenList��polygenList_dn�ĸ�polygen�ĸ����������ͼ����
			{
				const float zoomk=0.05;
				const float O[4]={0,0,0,1};//ԭ��
				for(j=0;j<npolygen_dn;j++){
					Cpolygen&polygen=cloud.polygenList[j];
					Cpolygen&polygen_dn=cloud.polygenList_dn[j];
					int nv=(int)polygen.vlist.size();
					for(int k=0;k<nv;k++){
						Cv4Ex&vup=polygen.vlist[k];
						Cv4Ex&vdn=polygen_dn.vlist[k];
						//��vup��vdn��ǰ����ͼ����
						{
							//��vup��ǰ��ͼ����
							calUVForWallPoint(O,vup.v,vup.normf,zoomk,vup.uf);
							//��vup�ĺ���ͼ����
							calUVForWallPoint(O,vup.v,vup.normn,zoomk,vup.un);
							//��vdn��ǰ��ͼ����
							calUVForWallPoint(O,vdn.v,vdn.normf,zoomk,vdn.uf);
							//��vdn�ĺ���ͼ����
							calUVForWallPoint(O,vdn.v,vdn.normn,zoomk,vdn.un);
						}
					}
				}
			}

		}
	}

	cout<<"::ǽ���и��ݶ�"<<endl;

	//����cloud�еĸ�cellת��Ϊ����μ���
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			cloud.grid.polygenlize();
		}
	}

	//Ϊÿ�����Ƕ���μ��㷨�������˹���ֻ��������polygen��Ϊ������ʱ���ã�
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			cloud.grid.calTriPolygenNorm();
		}

	}
	//������ȥ�и�needcut==true��cell�е�ÿ�������
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			myvector<Cpolygen>&polygenList=cloud.polygenList;
			int npolygen=(int)polygenList.size();
			for(int j=0;j<npolygen;j++){
				Cpolygen&polygen=polygenList[j];
				int nv=(int)polygen.vlist.size();
				for(int k=0;k<nv;k++){
					float *v1=polygen.vlist[k].v;
					float *v2=polygen.vlist[(k+1)%nv].v;
					//��v1v2ȥ�и�����needcut==true��cell�еĶ����
					cloud.grid.cutNeedcutCells(v1,v2);
				}
			}
		}
	}
	//��needcut==true��cell�ж���ÿ������ε������Ƿ�����������
	{
		int ncloud=(int)cloudList.size();
		for(int u=0;u<ncloud;u++){
			Ccloud&cloud=cloudList[u];
			if(cloud.removed)continue;
			Cgrid&grid=cloud.grid;
			myvector<Cpolygen>&polygenList=cloud.polygenList;
			int m=grid.m_mat;
			int n=grid.n_mat;
			for(int i=0;i<m;i++){
				for(int j=0;j<n;j++){
					Ccell&cell=*(grid.mat[i][j]);
					if(!cell.needcut)continue;
					int npolygen=(int)cell.polygenList.size();
					for(int k=0;k<npolygen;k++){
						Cpolygen&polygen=cell.polygenList[k];
						//��polygen������
						float c[4];
						polygen.cal_c(c);
						//�ж�c�Ƿ���polygenList��
						bool isin=isPointInPolygenList_reduced(c[X],c[Z],polygenList);
						if(!isin){
							polygen.removed=true;
						}


					}

					
				}
			}

		}

	}
	//--------------------------------------�ͷŲ����ڴ�
	cout<<"::�ͷŲ����ڴ�"<<endl;
	//ɾ����cloud�ĸ�cell��polygenList�б��Ϊremoved�Ķ����
	{
		int ncloud=(int)cloudList.size();
		for(int u=0;u<ncloud;u++){
			Ccloud&cloud=cloudList[u];
			if(cloud.removed)continue;
			Cgrid&grid=cloud.grid;
			myvector<Cpolygen>&polygenList=cloud.polygenList;
			int m=grid.m_mat;
			int n=grid.n_mat;
			for(int i=0;i<m;i++){
				for(int j=0;j<n;j++){
					Ccell&cell=*(grid.mat[i][j]);
					if(!cell.needcut)continue;
					int npolygen=(int)cell.polygenList.size();
					int count=0;
					for(int k=0;k<npolygen;k++){
						Cpolygen&polygen=cell.polygenList[k];
						if(polygen.removed){
							count++;
						}else{
							if(count!=0){
								cell.polygenList[k-count]=polygen;
							}
						}
					}
					cell.polygenList.resize(npolygen-count);
					//��ʱ����Ȼ��count��Ԫ�صľ�̬��Ա��ռ�ÿռ䣬�����еĶ�̬�б���������գ�����ռ�ÿռ䲢���ܴ�
				}
			}

		}

	
	}

	//--------------------------------------------------------------------------------


	//ɾ��empty��cell
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			cloud.grid.removeEmptyCell();
		}
	
	
		
	}
	//��cell��IDList
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			cloud.grid.clearIDListForEachCell();
		}
	
	
		
	}
	//��removed��cloud��grid.mat���
	{
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed){
				cloud.grid.clearMat();

			}
		}
	}

}




//----------------------------------------------------------------------------------
inline void makeOOBBgridForCloud(Ccloud&cloud)
//Ϊcloud����OOBB������cloud��������ƽ�е�����
{
	//�������������ϵԭ��
	float c[4]={0,0,0,1};
	//��������Y��
	float Yvec[4]={0,1,0,0};
	//���mainDir
	float mainDir[4];
	veccopy(cloud.mainDir.v,mainDir);
	//����xzƽ������mainDir��ֱ����
	float mainT[4];
	cross(mainDir,Yvec,mainT);
	//������������ϵ����������ϵת���ľ���
	float m_locToWorld[16]={
		mainDir[X],mainDir[Y],mainDir[Z],0,//��һ��
			Yvec[X],Yvec[Y],Yvec[Z],0,//�ڶ���
			mainT[X],mainT[Y],mainT[Z],0,//������
			c[X],c[Y],c[Z],1//������
	};
	//�浽cloud.grid
	matcopy(m_locToWorld,cloud.grid.m_locToWorld);
	//��������������ϵ������������ϵת���ľ���
	float m_worldToLoc[16];
	inv(m_locToWorld,m_worldToLoc);
	//�浽cloud.grid
	matcopy(m_worldToLoc,cloud.grid.m_worldToLoc);
	//��cloud�����е�ת����������������ϵ
	myvector<Cv4> _vlist;//��ת����Ľ��
	{
		int nv=(int)cloud.vlist.size();
		for(int j=0;j<nv;j++){
			Cv4 v=cloud.vlist[j];
			//��vת��������������ϵ
			prodv(m_worldToLoc,v.v,v.v);
			//��v�浽_vlist
			_vlist.push_back(v);
		}//�õ�_vlist
	}
	//��_vlist��Χ
	float xmin,xmax,zmin,zmax;
	{
		//����Ʒ�Χ
		xmin=inf;
		xmax=-inf;
		zmin=inf;
		zmax=-inf;
		int nv=(int)_vlist.size();
		for(int i=0;i<nv;i++){
			Cv4 v=_vlist[i];
			if(!v.isOutlinePoint)continue;//ֻ����isOutlinePoint
			float x=v.v[X];
			float z=v.v[Z];
			if(x<xmin)xmin=x;
			if(x>xmax)xmax=x;
			if(z<zmin)zmin=z;
			if(z>zmax)zmax=z;
		}//�õ�xmin,xmax,zmin,zmax
		const float margin=3;//4;//margin����һ�����Ϊ�˱�֤�����ΧҲ�ܸ���סcloud.polygenList
		//����Ϊcloud.polygenList��cloud.vlist�ķ�Χ������ƽ���͹��򻯶�����Щ��ƫ�룩
		xmin-=margin;
		xmax+=margin;
		zmin-=margin;
		zmax+=margin;
	}
	
	
	//��_vlist��������
	//��Ϊ���ڲ������ݶ���ʵ���޹صķ��������Ա�������ʹ��_vlist�������ˣ���˼���_vlistҲ�Ƕ��ࣩ
	//��֮�������ﻹҪ����_vlist��������������ֻ��Ϊ�˺�һ����ȥ��
	float xstepLen=1;
	float zstepLen=1;
	float margin=max(xstepLen,zstepLen);
	cloud.grid.makeGrid(xmin-margin,xmax+margin,zmin-margin,zmax+margin,//��Χ
		xstepLen,zstepLen,//����
		_vlist//����
		);
	
	
}

inline void markEmptyCellAndNeedcutCell(Ccloud&cloud)
//���empty���������⣩��needcut������������
{
	Cgrid&grid=cloud.grid;
	myvector<Cpolygen>&polygenList=cloud.polygenList;
	int m=grid.m_mat;
	int n=grid.n_mat;
	for(int i=0;i<m;i++){
		for(int j=0;j<n;j++){
			Ccell&cell=*(grid.mat[i][j]);
			//���cell���ĽǶ�������
			//   v0------v3
			//   |        |
			//   |    v4  |
			//   |        |
			//   v1------v2     
			float *v0=cell.vlup;
			float *v1=cell.vldn;
			float *v2=cell.vrdn;
			float *v3=cell.vrup;
			float *v4=cell.v;
		//	float *v5=cell.vup;
		//	float *v6=cell.vdn;
		//	float *v7=cell.vlt;
		//	float *v8=cell.vrt;
			//�ж�v0,v1,v2,v3,v4�Ƿ���������
			bool isin0,isin1,isin2,isin3,isin4;
		//	bool isin5,isin6,isin7,isin8;
			isin0=isPointInPolygenList_reduced(v0[X],v0[Z],polygenList);
			isin1=isPointInPolygenList_reduced(v1[X],v1[Z],polygenList);
			isin2=isPointInPolygenList_reduced(v2[X],v2[Z],polygenList);
			isin3=isPointInPolygenList_reduced(v3[X],v3[Z],polygenList);
			isin4=isPointInPolygenList_reduced(v4[X],v4[Z],polygenList);
		//	isin5=isPointInPolygenList_reduced(v5[X],v5[Z],polygenList);
		//	isin6=isPointInPolygenList_reduced(v6[X],v6[Z],polygenList);
		//	isin7=isPointInPolygenList_reduced(v7[X],v7[Z],polygenList);
		//	isin8=isPointInPolygenList_reduced(v8[X],v8[Z],polygenList);
			//���ĽǶ��㶼�����������߱�Ϊempty
			//���ĽǶ���������������Ҳ�������������߱�Ϊneedcut
			if(!isin0&&!isin1&&!isin2&&!isin3&&!isin4
		//		&&!isin5&&!isin6&!isin7&&!isin8
				){
				//ȫ�����⣬��Ϊempty
				cell.empty=true;
			}else{
				//����֧��˵����������һ���������ڣ�����ֻҪ��һ�������ڣ��������ⶼ��
				if(!isin0||!isin1||!isin2||!isin3||!isin4
		//			||!isin5||!isin6||!isin7||!isin8
				){
					//���ⶼ�У���Ϊneedcut
					cell.needcut=true;
				}
			}
			
		}
	}
}

inline float cal_topH_useInnerLayerID(const Ccloud&cloud)
//����cloud��topH����ǰ�˷���
{
	
	//������innerLayerID==maxInnerLayerID��cell�ĸ߶ȵ�ƽ��ֵ
	float topH;
	float maxH=-inf;
	int count=0;
	const Cgrid&grid=cloud.grid;
	int m=grid.m_mat;
	int n=grid.n_mat;
	for(int i=0;i<m;i++){
		for(int j=0;j<n;j++){
			Ccell&cell=*(grid.mat[i][j]);
			if(cell.innerLayerID==grid.maxInnerLayerID){
				if(cell.v[Y]>maxH){
					maxH=cell.v[Y];
					count++;
				}
			}
		}
	}//�õ�maxH
	if(count==0){//��maxH==-inf
		topH=cloud.bottomH;
	}else{
		topH=maxH;
	}
	return topH;
}
inline void calUVForWallPoint(const float O[4],const float v[4],const float norm[4],const float zoomk,float u[2])
//Ϊǽ�ڵ������������
//OΪԭ�����꣬vΪǽ�ڵ����꣬normΪǽ�ڵ㷨������zoomkΪ�������ӣ�uΪ��õ���������
{
	//        O
	//        |
	//        |
	//        |    u0
	// line---*----<----* v
	//       _O   dir   |
	//                 \|/
	//                  norm
	//
	//���v����norm��ֱ��ֱ��
	float dir[4]={-norm[Z],0,norm[X],0};//line�ķ�������
	Cline line;
	line.makeLine(v,dir);
	//��ԭ����line�ϵ�ͶӰ
	float _O[4];//ԭ����line�ϵ�ͶӰ
	line.getProjOfPoint_xz(O,_O);
	//��v��_O���������
	//_Ov=d*line.dir������Ҫ��d��ֻ����_Ov��ĳһ������ȥ����line.dir����Ӧ����
	//����Ϊ�˼���������ת�úϱ�(_Ov[X]+_Ov[Z])/(line.dir[X]+line.dir[Z])
	float _Ov[4];
	sub(v,_O,_Ov);
	float d=(_Ov[X]+_Ov[Z])/(line.dir[X]+line.dir[Z]);
	//v�������������u0����d���෴��
	float u0=-d;
	//v�������������u1����v[Y]
	float u1=v[Y];
	//��(u0,u1)���з���
	u0*=zoomk;
	u1*=zoomk;
	//����
	u[0]=u0;
	u[1]=u1;


}











