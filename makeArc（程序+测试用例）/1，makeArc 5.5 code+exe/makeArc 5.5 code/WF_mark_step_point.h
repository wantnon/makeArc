//-------------------------------------------------------
//��ǽ�Ծ��
//-------------------------------------------------------
inline void main_mark_step_point(myvector<Cv4>&vlist,float xmin,float xmax,float ymin,float ymax,float zmin,float zmax)
//��ǽ�Ծ�㣬�����vlist��Ԫ�ص�isEdgePoint��
{
	//�����������񣨾ֲ�����������
	CIDgrid IDgrid;//��������
	IDgrid.makeIDgrid(xmin,xmax,zmin,zmax,//��Χ
		0.5,0.5,//����
		vlist//����
		);
	//���н�Ծ���
	{
		int nv=(int)vlist.size();
		for(int i=0;i<nv;i++){
			Cv4&v=vlist[i];
			//���v�ڽ��������б�����vҲ��vlist�еĵ㣬���԰���v����
			myvector<int> IDList;//v�ڽ��������б�
			IDgrid.getIDList_oc(v.v[X],v.v[Z],IDList);
			//IDList�и���������С�߶�ֵ
			float hmin=inf;
			float hmax=-inf;
			int nvID=(int)IDList.size();
			for(int j=0;j<nvID;j++){
				float h=vlist[IDList[j]].v[Y];
				if(h<hmin)hmin=h;
				if(h>hmax)hmax=h;
			}//�õ�hmin��hmax������inf��-inf
			if(hmax-hmin>0.8){
				v.isEdgePoint=true;
			}	
		}
	}


}