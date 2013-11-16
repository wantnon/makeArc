
inline double calCircle(float _x1,float _y1,float _x2,float _y2,float _x3,float _y3,
		  float &x,float &y)
		  //��������(_x1,_y1),(_x2,y2),(_x3,_y3)���õ�Բ������(x,y)��ǰ�˷��ذ뾶��ƽ��
{		
	double r2;//���Բ�뾶��ƽ��

	//��ˮƽ�ı�
	int EvenEdgeIndex=0;//��¼�ڼ�����ˮƽ
	int count=0;
	if(fabs(_y1-_y2)<0.01){//ˮƽ
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
	case 1://���(_x1,_y1)-(_x2,_y2)ˮƽ
		x1=_x2;
		y1=_y2;
		x2=_x3;
		y2=_y3;
		x3=_x1;
		y3=_y1;
		break;
	case 2://���(_x2,_y2)-(_x3,_y3)ˮƽ
		x1=_x3;
		y1=_y3;
		x2=_x1;
		y2=_y1;
		x3=_x2;
		y3=_y2;
		break;
	case 3://���(_x3,_y3)-(_x1,_y1)ˮƽ
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

	//��(x1,y1)-(x2,y2)�Ĵ���б��
	float k1;

	k1=-(x2-x1)/(y2-y1);//����(x1,y1)-(x2,y2)��ˮƽ������y2�ز�����y1
	//��(x2,y2)-(x3,y3)�Ĵ���б��
	float k2;
	k2=-(x3-x2)/(y3-y2);//����(x2,y2)-(x3,y3)��ˮƽ������y3�ز�����y2
	//��(x1,y1)-(x2,y2)�е�(a,b)
	float a,b;
	a=(x1+x2)/2;
	b=(y1+y2)/2;
	//��(x2,y2)-(x3,y3)�е�(c,d)
	float c,d;
	c=(x2+x3)/2;
	d=(y2+y3)/2;
	//�������д��ߵĽ��㣨�����д��ߵķ����ǣ�y-b=k1x-ak1 �� y-d=k2x-ck2��
	x=(a*k1-c*k2+d-b)/(k1-k2);
	y=k1*x-a*k1+b;
	//��뾶��ƽ��
	r2=pow2(x-x1)+pow2(y-y1);
	return r2;

}





inline int getIndexOfp_IDtri1ToIDtri2(CIDTriangle*pIDtri1,CIDTriangle*pIDtri2)
//��*pIDtri1�ĵڼ���ָ��ָ��*pIDtri2
//���*pIDtri1û��ָ��*pIDtri2��ָ�룬�򷵻�-1
{
	for(int i=0;i<=2;i++){
		if(pIDtri1->p[i]==pIDtri2){
			break;
		}
	}
	if(i<=2){//��break�����ģ�˵��*pIDtri1��ָ��ָ��*pIDtri2��pIDtri1->p[i]��Ϊ��ָ��
		return i;
	}else{
		cout<<"error getIndexOfp_IDtri1ToIDtri2"<<endl;
		return -1;
	}
}
inline int getIndexOfVert_inIDtri1notinIDtri2(CIDTriangle*pIDtri1,CIDTriangle*pIDtri2)
//��*pIDtri1�еڼ������㲻��*pIDtri2�У������ҵ��ĵ�һ�����ϴ������Ķ����
{
//	int I[3];
//	int p=0;
	for(int i=0;i<=2;i++){
		int ID=pIDtri1->vID[i];
		//��ID�Ƿ���*pIDtri2��
		bool isin=false;
		for(int j=0;j<=2;j++){
			int _ID=pIDtri2->vID[j];
			if(ID==_ID){
				isin=true;
				break;
			}
		}//�õ�isin
		if(!isin){
//			I[p++]=i;
			return i;
		}
	}
	//û�ҵ�
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
//�ж�ID�Ŷ����Ƿ���*pIDtri�ڻ�߽���
//�����v��v[Y]������0
//����ֵ0,1,2,3�ֱ�������⣬���ڣ��ڱ��ϣ��ڶ�����
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
	//��(x,y)�Ƿ���(x1,y1)-(x2,y2)���
	{
		//��(x1,y1)-(x2,y2)���ҷ�����
		float nx=y1-y2;//ע�⣬����������ϵ�£�����(x,y)���ҷ�������(-y,x)
		               //        |---> x
		               //        |
		               //       \|/
		               //        z
		               //������������ϵ�£�����(x,y)���ҷ�������(y,-x)
					   //        z
		               //       /|\
		               //        |
		               //        |
		               //        -----> x 
		float ny=x2-x1;
		//����(x,y)-(x1,y1)ͬ�������
		float vecx=x1-x;
		float vecy=y1-y;
		//����(nx,ny)��(vecx,vecy)�ĵ���Ƿ�Ϊ��
		float dot=nx*vecx+ny*vecy;
		if(dot<0)return 0;//������
		else if(dot==0){//�ڱ���
			edgeID=0;
			onEdgeCount++;
		}
	}
	//��(x,y)�Ƿ���(x2,y2)-(x3,y3)���
	{
		//��(x2,y2)-(x3,y3)���ҷ�����
		float nx=y2-y3;
		float ny=x3-x2;
		//����(x,y)-(x2,y2)ͬ�������
		float vecx=x2-x;
		float vecy=y2-y;
		//����(nx,ny)��(vecx,vecy)�ĵ���Ƿ�Ϊ��
		float dot=nx*vecx+ny*vecy;
		if(dot<0)return 0;//������
		else if(dot==0){//�ڱ���
			edgeID=1;
			onEdgeCount++;
		}
	}
	//��(x,y)�Ƿ���(x3,y3)-(x1,y1)���
	{
		//��(x3,y3)-(x1,y1)���ҷ�����
		float nx=y3-y1;
		float ny=x1-x3;
		//����(x,y)-(x3,y3)ͬ�������
		float vecx=x3-x;
		float vecy=y3-y;
		//����(nx,ny)��(vecx,vecy)�ĵ���Ƿ�Ϊ��
		float dot=nx*vecx+ny*vecy;
		if(dot<0)return 0;//������
		else if(dot==0){//�ڱ���
			edgeID=2;
			onEdgeCount++;
		}
	}
	//������
	if(onEdgeCount==0){
		return 1;//����
	}else if(onEdgeCount==1){
		return 2;//�ڱ���
	}else{
		return 3;//�ڶ�����
	}

}
void flip(CIDTriangle*pAdjIDtri,CIDTriangle*pnewIDtri,const int ID,float v[4],myvector<Cv4>&vlist,myvector<CIDTriangle*>&pIDtriList,int&count){
	if(count>20){
		return;
	}
	//��pAdjIDtri�����Բ�Ƿ��������ID���༴v��
	if(!pAdjIDtri->circleAlreadyGet){//������Բ��û��������������Բ������
		//��pAdjIDtri�����Բ
		float x1,z1,x2,z2,x3,z3;//pAdjIDtri���������(x,z)����
		x1=vlist[pAdjIDtri->vID[0]].v[X];
		z1=vlist[pAdjIDtri->vID[0]].v[Z];
		x2=vlist[pAdjIDtri->vID[1]].v[X];
		z2=vlist[pAdjIDtri->vID[1]].v[Z];
		x3=vlist[pAdjIDtri->vID[2]].v[X];
		z3=vlist[pAdjIDtri->vID[2]].v[Z];
		pAdjIDtri->r2=calCircle(x1,z1,x2,z2,x3,z3,pAdjIDtri->x,pAdjIDtri->z);
		pAdjIDtri->circleAlreadyGet=true;
		
	}
	//��ID���༴v���Ƿ���pAdjIDtri�����Բ��
	if(pAdjIDtri->r2<0)return;
	if(_isnan(pAdjIDtri->r2))return;
	//��pAdjIDtri�����Բ�Ƿ����v
	//��v��Բ��(x,z)�ľ���ƽ��
	double d2=pow2(v[X]-pAdjIDtri->x)+pow2(v[Z]-pAdjIDtri->z);
	if(d2<0)return;
	if(_isnan(d2))return;
	if(d2>=pAdjIDtri->r2)return;//ID����pAdjIDtri�����Բ��



	//���ID��pAdjIDtri�����Բ��
	//���жԽ��߸���
	//                  ID0                                   ID0
	//thirdID ----------/|                  thirdID-----------|
	//         \       / |                          \ - _1_   |       
	//          \     / ID                ==>        \      -ID   
	//           \   / /                              \  2  /  
	//            \ //                                 \  /   
	//            ID1                                  ID1
	int ID0=pnewIDtri->vID[1];
	int ID1=pnewIDtri->vID[2];
	//��Ҫ����pAdjIDtri��pnewIDtri���ɵ��ı��εĶԽ���
	//��pAdjIDtri����pIDtri���õĶ���ID
	int thirdindex=getIndexOfVert_inIDtri1notinIDtri2(pAdjIDtri,pnewIDtri);
	int thirdID=pAdjIDtri->vID[thirdindex];
	if(thirdindex==-1){
		cout<<"error thirdindex==-1"<<endl;
		return;
	}
	//���µ�������pIDtri1,pIDtri2ȡ��pnewIDtri��pAdjIDtri
	CIDTriangle*pIDtri1,*pIDtri2;
	pIDtri1=new CIDTriangle();
	pIDtri2=new CIDTriangle();
	//�趥�㣨����IDΪ��㣩
	pIDtri1->setvID(ID,ID0,thirdID);
	pIDtri2->setvID(ID,thirdID,ID1);
	//��ָ��
	pIDtri1->setp(pnewIDtri->p[0],pAdjIDtri->p[(thirdindex+2)%3],pIDtri2);
	pIDtri2->setp(pIDtri1,pAdjIDtri->p[thirdindex],pnewIDtri->p[2]);

	//��Χ�����ε�ָ������
	//ͨ��ID0thirdID���ڵ�������ָ������
	{
		CIDTriangle*_pAdjIDtri=pAdjIDtri->p[(thirdindex+2)%3];
		if(_pAdjIDtri!=NULL){
			//��_pAdjIDtri���ĸ�ָ��ָ��pAdjIDtri
			int index=getIndexOfp_IDtri1ToIDtri2(_pAdjIDtri,pAdjIDtri);
			if(index==-1)return;
			//����ָ���Ϊָ��pIDtri1
			_pAdjIDtri->p[index]=pIDtri1;
		}
		
	}
	//ͨ��thirdIDID1���ڵ�������ָ������
	{
		CIDTriangle*_pAdjIDtri=pAdjIDtri->p[thirdindex];
		if(_pAdjIDtri!=NULL){
			//��_pAdjIDtri���ĸ�ָ��ָ��pAdjIDtri
			int index=getIndexOfp_IDtri1ToIDtri2(_pAdjIDtri,pAdjIDtri);
			if(index==-1)return;
			//����ָ���Ϊָ��pIDtri2
			_pAdjIDtri->p[index]=pIDtri2;
		}
		
	}
	//ͨ��IDID0���ڵ�������ָ������
	{
		CIDTriangle*_pAdjIDtri=pnewIDtri->p[0];
		if(_pAdjIDtri!=NULL){
			//��_pAdjIDtri���ĸ�ָ��ָ��pnewIDtri
			int index=getIndexOfp_IDtri1ToIDtri2(_pAdjIDtri,pnewIDtri);
			if(index==-1)return;
			//����ָ���Ϊָ��pIDtri1
			_pAdjIDtri->p[index]=pIDtri1;
		}
		
	}
	//ͨ��IDID1���ڵ�������ָ������
	{
		CIDTriangle*_pAdjIDtri=pnewIDtri->p[2];
		if(_pAdjIDtri!=NULL){
			//��_pAdjIDtri���ĸ�ָ��ָ��pnewIDtri
			int index=getIndexOfp_IDtri1ToIDtri2(_pAdjIDtri,pnewIDtri);
			if(index==-1)return;
			//����ָ���Ϊָ��pIDtri2
			_pAdjIDtri->p[index]=pIDtri2;	
		}
	}
	//��pIDtri1��pIDtri2�滻pnewIDtri��pAdjIDtri
	//��pIDtri1����pIDtriList
	pIDtriList.push_back(pIDtri1);
	//��pIDtri2����pIDtriList
	pIDtriList.push_back(pIDtri2);

	//��pnewIDtri���Ϊremoved
	pnewIDtri->removed=true;
	//��pIDtri1,pIDtri2����pnewIDtri�ĺ���
	pnewIDtri->q.push_back(pIDtri1);
	pnewIDtri->q.push_back(pIDtri2);

	//��pAdjIDtri���Ϊremoved
	pAdjIDtri->removed=true;
	pAdjIDtri->q.push_back(pIDtri1);
	pAdjIDtri->q.push_back(pIDtri2);


	//�ݹ�

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
//����������ȥ����ID��λ���ĸ������Σ�����ָ��������ε�ָ��pTagIDtri
//pTagIDtri����������ΪNULL�������ܹ���
//judge==1��ʾ���ڲ���judge==2��ʾ�ڱ��ϣ�judge==3��ʾ�ڶ�����
//ע�⣬Ϊ���Ż�������������ڶ����ϵ��������������ȷ��pIDtri
{
	//��ID���Ƿ�λ��pIDtri��
	judge=pointInIDtri(v,pIDtri,vlist,edgeID);
	if(judge==1||judge==2){//���λ���ڲ��������
		int nchildIDtri=(int)pIDtri->q.size();
		if(nchildIDtri==0){
			pTagIDtri=pIDtri;
			return;
		}else{
			//�ݹ��ӽڵ�
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
//Delaunay�����ʷ�
//�������mesh.vlist���͵���(x,z)��Χ�����mesh.IDtriList
{
	//���쳬��������
	//   a/2   a  a/2
	// v1_____________v3
	//   \ |   |   | /
 	//    \| b |   |/
	//     \---|---/
	//      \ b|  /
	//       \ | /
	//        \|/
	//         v2  
	//   ����a=xmax-xmin��b=zmax-zmin
	//
	//���㳬�������ε���������
	float margin=10000;//marginȡ���㹻���Ա�֤����ɾ�����������ߵ�������ʱ�ܽ�����������ɾ��
	float a=xmax-xmin;
	float b=zmax-zmin;
	float v1[4],v2[4],v3[4];
	init4(v1,xmin-a/2-margin,0,zmin-margin,1);
	init4(v2,(xmin+xmax)/2,0,zmin+b+b+margin,1);
	init4(v3,xmax+a/2+margin,0,zmin-margin,1);
	//�����������ε���������v1,v2,v3���뵽�����б�
	vlist.grow();
	int ID0=(int)vlist.size()-1;
	veccopy(v1,vlist[ID0].v);

	vlist.grow();
	int ID1=ID0+1;
	veccopy(v2,vlist[ID1].v);

	vlist.grow();
	int ID2=ID1+1;
	veccopy(v3,vlist[ID2].v);

	//���ɳ���������
	CIDTriangle*pIDtri=new CIDTriangle();
	pIDtri->setvID(ID0,ID1,ID2);
	pIDtri->setp(NULL,NULL,NULL);

	//��pIDtri����pIDtriList
	pIDtriList.push_back(pIDtri);





	//--------------------��ʼ������
	int _nv=(int)vlist.size()-3;//����������㲻�ٲ���
	for(int ii=0;ii<_nv;ii++){
		const int ID=ii;//����������
		float v[4];
		veccopy(vlist[ID].v,v);//����
		v[Y]=0;
		//����ID�Ŷ���
		//��ID����λ���ĸ�������
		CIDTriangle*pIDtri=NULL;//pIDtri����ID����
		int judge=0;
		int edgeID=-1;
		locIDPoint(v,pIDtriList[0],vlist,pIDtri,judge,edgeID);
		if(pIDtri==NULL){
			continue;
		}

		if(judge==2){//������������α���
			//         ID0
			//       ID/ \    
			// edgeID *   \
			//       /     \
			//      /_______\
			//    ID1       ID2 
			int ID0=pIDtri->vID[edgeID];
			int ID1=pIDtri->vID[(edgeID+1)%3];
			int ID2=pIDtri->vID[(edgeID+2)%3];
			//����edgeID���ھ�ָ���Ƿ�Ϊ��
			if(pIDtri->p[edgeID]==NULL){//���edgeID���ھ�ָ��Ϊ��
				//         ID0
				//       ID/ \    
				// edgeID /\ 0\
				//       /   \ \
				//      /__1___\\
				//    ID1       ID2 
				//pIDtri����Ϊ����������
				//������������
				myvector<CIDTriangle*> pnewIDtri;
				pnewIDtri.resize(2);
				pnewIDtri[0]=new CIDTriangle();
				pnewIDtri[1]=new CIDTriangle();
				//Ϊ���������趨����(�������ĵ�ID��Ϊ��ʼ��)
				pnewIDtri[0]->setvID(ID,ID2,ID0);
				pnewIDtri[1]->setvID(ID,ID1,ID2);
				//�趨���������ε��ڽ�ָ��
				pnewIDtri[0]->setp(pnewIDtri[1],pIDtri->p[(edgeID+2)%3],NULL);
				pnewIDtri[1]->setp(NULL,pIDtri->p[(edgeID+1)%3],pnewIDtri[0]);
				//����pIDtri�ڽ������ε�ָ��
				//ͨ��ID2ID0���ڵ�������ָ������
				{
					CIDTriangle*_pAdjIDtri=pIDtri->p[(edgeID+2)%3];
					if(_pAdjIDtri!=NULL){
						//��_pAdjIDtri���ĸ�ָ��ָ��pIDtri
						int index=getIndexOfp_IDtri1ToIDtri2(_pAdjIDtri,pIDtri);
						//����ָ���Ϊָ��pnewIDtri[0]
						if(index==-1)continue;
						_pAdjIDtri->p[index]=pnewIDtri[0];
					}

				}
				//ͨ��ID1ID2���ڵ�������ָ������
				{
					CIDTriangle*_pAdjIDtri=pIDtri->p[(edgeID+1)%3];
					if(_pAdjIDtri!=NULL){
						//��_pAdjIDtri���ĸ�ָ��ָ��pIDtri
						int index=getIndexOfp_IDtri1ToIDtri2(_pAdjIDtri,pIDtri);
						if(index==-1)continue;
						//����ָ���Ϊָ��pnewIDtri[0]
						_pAdjIDtri->p[index]=pnewIDtri[1];
					}

				}
				//�����������μ���pIDtriList
				pIDtriList.push_back(pnewIDtri[0]);
				pIDtriList.push_back(pnewIDtri[1]);
				
				//��pIDtri���Ϊremoved
				pIDtri->removed=true;
				//��pnewIDtri[0]~pnewIDtri[1]����pIDtri�ĺ���
				pIDtri->q.push_back(pnewIDtri[0]);
				pIDtri->q.push_back(pnewIDtri[1]);
						int count=0;
				//���μ���pnewIDtri��p[1]�������ڱ������Σ��������Բ�Ƿ����ID���㣬���������������Խ���
				for(int i=0;i<=1;i++){
					if(pnewIDtri[i]->p[1]!=NULL){
								count++;
						flip(pnewIDtri[i]->p[1],pnewIDtri[i],ID,v,vlist,pIDtriList,count);
								count--;
					}
					
				}


			}else{//���edgeID���ھ�ָ�벻��
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
				//�����ĸ���������
				myvector<CIDTriangle*> pnewIDtri;
				pnewIDtri.resize(4);
				pnewIDtri[0]=new CIDTriangle();
				pnewIDtri[1]=new CIDTriangle();
				pnewIDtri[2]=new CIDTriangle();
				pnewIDtri[3]=new CIDTriangle();
				//Ϊ���������趨����(�������ĵ�ID��Ϊ��ʼ��)
				pnewIDtri[0]->setvID(ID,ID2,ID0);
				pnewIDtri[1]->setvID(ID,ID1,ID2);
				pnewIDtri[2]->setvID(ID,ID0,thirdID);
				pnewIDtri[3]->setvID(ID,thirdID,ID1);
				//�趨���������ε��ڽ�ָ��
				pnewIDtri[0]->setp(pnewIDtri[1],pIDtri->p[(edgeID+2)%3],pnewIDtri[2]);
				pnewIDtri[1]->setp(pnewIDtri[3],pIDtri->p[(edgeID+1)%3],pnewIDtri[0]);
				pnewIDtri[2]->setp(pnewIDtri[0],pAdjIDtri->p[(thirdindex+2)%3],pnewIDtri[3]);
				pnewIDtri[3]->setp(pnewIDtri[2],pAdjIDtri->p[thirdindex],pnewIDtri[1]);
				//����pIDtri�ڽ������ε�ָ��
				//ͨ��ID2ID0���ڵ�������ָ������
				{
					CIDTriangle*_pAdjIDtri=pIDtri->p[(edgeID+2)%3];
					if(_pAdjIDtri!=NULL){
						//��_pAdjIDtri���ĸ�ָ��ָ��pIDtri
						int index=getIndexOfp_IDtri1ToIDtri2(_pAdjIDtri,pIDtri);
						if(index==-1)continue;
						//����ָ���Ϊָ��pnewIDtri[0]
						_pAdjIDtri->p[index]=pnewIDtri[0];
					}

				}
				//ͨ��ID1ID2���ڵ�������ָ������
				{
					CIDTriangle*_pAdjIDtri=pIDtri->p[(edgeID+1)%3];
					if(_pAdjIDtri!=NULL){
						//��_pAdjIDtri���ĸ�ָ��ָ��pIDtri
						int index=getIndexOfp_IDtri1ToIDtri2(_pAdjIDtri,pIDtri);
						if(index==-1)continue;
						//����ָ���Ϊָ��pnewIDtri[0]
						_pAdjIDtri->p[index]=pnewIDtri[1];
					}

				}
				//ͨ��ID0thirdID���ڵ�������ָ������
				{
					CIDTriangle*_pAdjIDtri=pAdjIDtri->p[(thirdindex+2)%3];
					if(_pAdjIDtri!=NULL){
						//��_pAdjIDtri���ĸ�ָ��ָ��pAdjIDtri
						int index=getIndexOfp_IDtri1ToIDtri2(_pAdjIDtri,pAdjIDtri);
						if(index==-1)continue;
						//����ָ���Ϊָ��pnewIDtri[2]
						_pAdjIDtri->p[index]=pnewIDtri[2];
					}

				}
				//ͨ��thirdIDID1���ڵ�������ָ������
				{
					CIDTriangle*_pAdjIDtri=pAdjIDtri->p[thirdindex];
					if(_pAdjIDtri!=NULL){
						//��_pAdjIDtri���ĸ�ָ��ָ��pAdjIDtri
						int index=getIndexOfp_IDtri1ToIDtri2(_pAdjIDtri,pAdjIDtri);
						if(index==-1)continue;
						//����ָ���Ϊָ��pnewIDtri[0]
						_pAdjIDtri->p[index]=pnewIDtri[3];
					}
				
				}


				//�����������μ���pIDtriList
				pIDtriList.push_back(pnewIDtri[0]);
				pIDtriList.push_back(pnewIDtri[1]);
				pIDtriList.push_back(pnewIDtri[2]);
				pIDtriList.push_back(pnewIDtri[3]);
				
				//��pIDtri���Ϊremoved
				pIDtri->removed=true;
				pAdjIDtri->removed=true;
				//��pnewIDtri[0]~pnewIDtri[1]����pIDtri�ĺ���
				pIDtri->q.push_back(pnewIDtri[0]);
				pIDtri->q.push_back(pnewIDtri[1]);
				//��pnewIDtri[2]~pnewIDtri[3]����pAdjIDtri�ĺ���
				pAdjIDtri->q.push_back(pnewIDtri[2]);
				pAdjIDtri->q.push_back(pnewIDtri[3]);
						int count=0;
				//���μ���pnewIDtri��p[1]���������Բ�Ƿ����ID���㣬���������������Խ���
				for(int i=0;i<=3;i++){
					
					if(pnewIDtri[i]->p[1]!=NULL){
								count++;
						flip(pnewIDtri[i]->p[1],pnewIDtri[i],ID,v,vlist,pIDtriList,count);
								count--;
					}
					
				}	
			}
		}else if(judge==1){//���������������
			//��v���뵽pIDtri��
			//ֱ�ӽ�pIDtri����Ϊ����������
			//           ID0      
			//           /|\
			//          / | \
			//         / ID  \
			//        / /   \ \
			//       //       \\
			//     ID1 ------- ID2
			//pIDtri����������
			int ID0=pIDtri->vID[0];
			int ID1=pIDtri->vID[1];
			int ID2=pIDtri->vID[2];
			//����������������
			myvector<CIDTriangle*> pnewIDtri;
			pnewIDtri.resize(3);
			pnewIDtri[0]=new CIDTriangle();
			pnewIDtri[1]=new CIDTriangle();
			pnewIDtri[2]=new CIDTriangle();
			//Ϊ�������������趨����(�������ĵ�ID��Ϊ��ʼ��)
			pnewIDtri[0]->setvID(ID,ID0,ID1);
			pnewIDtri[1]->setvID(ID,ID1,ID2);
			pnewIDtri[2]->setvID(ID,ID2,ID0);
			//�趨���������ε��ڽ�ָ��
			pnewIDtri[0]->setp(pnewIDtri[2],pIDtri->p[0],pnewIDtri[1]);
			pnewIDtri[1]->setp(pnewIDtri[0],pIDtri->p[1],pnewIDtri[2]);
			pnewIDtri[2]->setp(pnewIDtri[1],pIDtri->p[2],pnewIDtri[0]);
			//����pIDtri�ڽ������ε�ָ��
			bool error=false;
			for(int i=0;i<=2;i++){
				//����pIDtri->p[i]��ָ��
				CIDTriangle*_pAdjIDtri=pIDtri->p[i];
				//����_pAdjIDtri��ָ��
				if(_pAdjIDtri!=NULL){
					//��_pAdjIDtri���ĸ�ָ��ָ��pIDtri
					int index=getIndexOfp_IDtri1ToIDtri2(_pAdjIDtri,pIDtri);
					if(index==-1){
						error=true;
						break;
					}
					//����ָ��ָ��pnewIDtri[i]
					_pAdjIDtri->p[index]=pnewIDtri[i];
				}
			}
			if(error)continue;
			//�����������μ���pIDtriList
			pIDtriList.push_back(pnewIDtri[0]);
			pIDtriList.push_back(pnewIDtri[1]);
			pIDtriList.push_back(pnewIDtri[2]);
			
			//��pIDtri���Ϊremoved
			pIDtri->removed=true;
			//��pnewIDtri[0]~pnewIDtri[2]����pIDtri�ĺ���
			pIDtri->q.push_back(pnewIDtri[0]);
			pIDtri->q.push_back(pnewIDtri[1]);
			pIDtri->q.push_back(pnewIDtri[2]);
					int count=0;
			//���μ���pnewIDtri��p[1]�������ڱ������Σ��������Բ�Ƿ����ID���㣬���������������Խ���
			for(i=0;i<=2;i++){
				
				if(pnewIDtri[i]->p[1]!=NULL){
							count++;
					flip(pnewIDtri[i]->p[1],pnewIDtri[i],ID,v,vlist,pIDtriList,count);
							count--;
				}
				
			}
		}else{//judge==3����ʾ���ڶ�����
			
			continue;
		}
	}
	//ɾ���볬�������ζ�����ص�������
	//����pIDtriList��������ID0,ID1,ID2֮һ�������α�Ϊremoved
	int nIDtri=(int)pIDtriList.size();
	for(int i=0;i<nIDtri;i++){
		CIDTriangle*pIDtri=pIDtriList[i];
		int _ID0,_ID1,_ID2;//pIDtri����������ID
		_ID0=pIDtri->vID[0];
		_ID1=pIDtri->vID[1];
		_ID2=pIDtri->vID[2];
		if(_ID0==ID0||_ID0==ID1||_ID0==ID2
			||_ID1==ID0||_ID1==ID1||_ID1==ID2
			||_ID2==ID0||_ID2==ID1||_ID2==ID2){
			pIDtri->removed=true;
		}
	}
	//ɾ��_vlist�г�����������������
	vlist.pop_back();
	vlist.pop_back();
	vlist.pop_back();
}
