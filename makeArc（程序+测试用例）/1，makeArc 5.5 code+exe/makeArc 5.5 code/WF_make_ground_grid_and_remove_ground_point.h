//----------------------------------------------
//���ɵ�������ɾ�������
//----------------------------------------------

inline void main_make_ground_grid_and_remove_ground_point(myvector<Cv4>&vlist,
												   float xmin,float xmax,float ymin,float ymax,float zmin,float zmax,
												   Cgroundgrid&GroundGrid)
{

	//������x,z�����ϵ������
	float maxHouseSizeX=50;//x�����ϵ������
	float maxHouseSizeZ=50;//z�����ϵ������
	CgroundgridEx _GroundGrid;
	//������������
	{
		float xstepLen=1;
		float zstepLen=1;
		float margin=1;//��margin=0���ܱ�֤��������ӳ����ȫ��ȷ���˴�Ϊ�˲����磬����margin��Ϊ0�������������ģ���Ϊmargin����ȡ��
		_GroundGrid.makeGrid(xmin-margin,xmax+margin,zmin-margin,zmax+margin,//��Χ
			xstepLen,zstepLen,//����
			vlist//����
			);
		//����ȱʧԪ��
		while(1){
			int n=_GroundGrid.makeUp_once();
			if(n==0)break;
		}
		//��ǽ�Ծ�ͱ�Ե
		_GroundGrid.markJump(0.5,0.5);
	}
	//������ӵ�
	//����ÿmaxHouseSizeX*maxHouseSizeZ�����򶼱ض��е����
	_GroundGrid.markSeed(maxHouseSizeX,maxHouseSizeZ);
	//��ǵ����
	//��GroundGrid�е�cell����ͨ�Խ��з���
	_GroundGrid.cncTest();
	//��GroundGrid.ijsetList�зǵ���ijsetȥ��
	_GroundGrid.getGroundijsetAndDestroyijsetList();
	//��vlist�еĵ��������
	_GroundGrid.markGround(vlist);
	//�ؽ�����
	//��GroundGrid�������
	while(1){
		int n=_GroundGrid.makeUpGround_once();
		if(n==0)break;
	}
	//��һ����Χ�ϴ�Χ�ڵĸ߶�ƽ��ֵ����˵�߶ȣ�������ǿ��ƽ��
	_GroundGrid.Hsmooth_strong();
	//���GroundGrid�и�cell��IDList
	_GroundGrid.clearIDListForEachCell();
	//���groundijset
	_GroundGrid.clearGroundijset();
	
	//------------------------------------ɾ���ȵ���߲���ĵ�
	{
		//���һ������ڵ���ľ���С��һ���߶ȣ���Ϊ�����
		//���������������
		float zmin_g=_GroundGrid.zmin;
		float xmin_g=_GroundGrid.xmin;
		float zstepLen_g=_GroundGrid.zstepLen;
		float xstepLen_g=_GroundGrid.xstepLen;
		int m_mat_g=_GroundGrid.m_mat;
		int n_mat_g=_GroundGrid.n_mat;
		int nv=vlist.size();
		for(int i=0;i<nv;i++){
			Cv4&v=vlist[i];
			if(v.isGround)continue;
			//��v��_GroundGrid���ĸ���
			int I,J;//v����(I,J)��
			I=(v.v[Z]-zmin_g)/zstepLen_g;
			J=(v.v[X]-xmin_g)/xstepLen_g;
			if(I<0||I>=m_mat_g)continue;
			if(J<0||J>=n_mat_g)continue;
			//��ô˸�߶�
			float H_g=_GroundGrid.mat[I][J].v[Y];
			//���v[Y]�߳�H_g��С����v���Ϊɾ��
			if(v.v[Y]-H_g<=2){
				v.isGround=true;
			}
		}

					
	}
	//------------------------------------ɾ������㣨����������
	//��vlist�б��Ϊ��isGround=true�ĵ�ɾ����ע�⣬��ʱvlist��GroundGrid�Ѿ���һ�£�
	{
		int count=0;
		int nv=(int)vlist.size();
		for(int i=0;i<nv;i++){
			Cv4&v=vlist[i];
			if(v.isGround){
				count++;
			}else{
				if(count!=0)vlist[i-count]=v;
			}
		}
		vlist.resize(nv-count);
	}
	//---------------��_GroundGrid��ʼ��GroundGrid
	GroundGrid.init(_GroundGrid);




}
