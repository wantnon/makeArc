//-------------------------
//计算各云团的minH,并删除过低的云团
//-------------------------
main_cal_minH_and_rid_cloud_too_low(myvector<Ccloud>&cloudList,const Cgroundgrid&GroundGrid)
{
	//求当前轮廓条件下，各云团的cloud.minH
	//由于现在轮廓还没完全确定下来，在后面处理中还会起变化，所以此步估算出的cloud.minH将来还需要再更新
	{
		//求地面矩形网格参数
		float zmin_g=GroundGrid.zmin;
		float xmin_g=GroundGrid.xmin;
		float zstepLen_g=GroundGrid.zstepLen;
		float xstepLen_g=GroundGrid.xstepLen;
		int m_mat_g=GroundGrid.m_mat;
		int n_mat_g=GroundGrid.n_mat;
		int ncloud=(int)cloudList.size();
		for(int i=0;i<ncloud;i++){
			Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
		
			//求cloud.IDoutline.polygenList中的点在地形网格中对应的Y值的最小值，作为cloud.minH的值
			float minH=inf;
			int npolygen=cloud.polygenList.size();
			for(int j=0;j<npolygen;j++){
				Cpolygen&polygen=cloud.polygenList[j];
				int nv=(int)polygen.vlist.size();
				for(int k=0;k<nv;k++){
					Cv4&v=polygen.vlist[k];
					//求v处的地面Y值
					float vY;
					//求v在哪个格
					int I,J;//v属于(I,J)格
					I=(v.v[Z]-zmin_g)/zstepLen_g;
					J=(v.v[X]-xmin_g)/xstepLen_g;
					if(I<0||I>=GroundGrid.m_mat)continue;
					if(J<0||J>=GroundGrid.n_mat)continue;
					vY=GroundGrid.mat[I][J].h;
					//与minH比较
					if(vY<minH){
						minH=vY;
					}
				}
			}//得到minH
			cloud.minH=minH;
			if(cloud.avrgH-cloud.minH<=2.5){//过低删除
				cloud.removed=true;
			}
			
		}
	}

}