//-----------------------------------------
//����ģ��
//-----------------------------------------
inline string f2str(float);
inline string i2str(int);
inline string toS(string);
inline void fprintPolygenList(FILE   *,const myvector<Cpolygen>&);
inline void fprintGrid(FILE   *,const Cgrid&);
inline void fprintPolygen(FILE   *,const Cpolygen&);
inline void fprintMat(FILE   *,const myvector<myvector<Ccell*> >&,int,int);
inline void fprintCell(FILE   *,const Ccell*);
inline void fprint4fv(FILE   *,const float[4]);
inline void fprint2fv(FILE   *,const float[2]);
inline void fprint16fv(FILE   *,const float[16]);
inline void fprint4f(FILE   *,float x,float y,float z,float w);
inline void fprint2f(FILE   *,float x,float y);
inline void fprint2d(FILE   *,int m,int n);
inline void fprint1f(FILE   *,float);
inline void fprint1d(FILE   *,int);

inline bool main_save_model(string pathName,//·����
					 string clipName,//��pathName·����Ҫ�������ļ�����
	float xmin,float xmax,float ymin,float ymax,float zmin,float zmax,
    float offsetus,float offsetut,
	const myvector<Ccloud>&cloudList,
	const Cgroundgrid&groundgrid,
	string picFileName_photo,string picFileName_wall,string picFileName_sky){
	//��cloudList��δremoved��cloud����
	int ncloud=0;//��removed��cloud����
	{
		int nc=(int)cloudList.size();
		for(int i=0;i<nc;i++){
			if(!cloudList[i].removed){
				ncloud++;
			}
		}//�õ�ncloud
	}
	//�½��ļ���
	//�ж��ļ����Ƿ����
	if(_access((pathName+clipName).c_str(),0) == 0 ){
		AfxMessageBox(("����ʧ��!\n\nԭ��"+clipName+"�ļ����Ѵ���\n\nλ�ã�"+pathName+clipName+"\n\n��Ҫ���±��棬�����Ƴ�model�ļ���").c_str());
		return false;	
	}
	if (_mkdir((pathName+clipName).c_str()) != 0)
	{
		cout<<"error �����ļ���ʧ��!"<<endl;
		return false;	
	}
	//���ɷ�removed�����ļ����б�
	myvector<string> CFNameList;//��removed�����ļ����б�
	{
		for(int i=0;i<ncloud;i++){
			//���ɵ�i�������ļ���
			string _pathName;
			_pathName+="cloud_"+i2str(i)+".cloud";
			CFNameList.push_back(_pathName);
		}
	}
	//�������п������ļ�
	{
		int nCF=(int)CFNameList.size();
		for(int i=0;i<nCF;i++){
			string&CFName=CFNameList[i];
			//�����ļ�
			FILE   *fp; 	
			string CFpathName=pathName+clipName+"\\"+CFName.c_str();
			fp=fopen(CFpathName.c_str(),"w+");
			if(fp==NULL){
				cout<<"error �����ļ�ʧ��!"<<endl;
				return false;
			}
			fclose(fp);
		}
	}
	//�����յ����ļ�
	string GFName="groundgrid.gg";//�����ļ���
	{
		//�����ļ�
		FILE   *fp; 	
		string GFpathName=pathName+clipName+"\\"+GFName.c_str();
		fp=fopen(GFpathName.c_str(),"w+");
		if(fp==NULL){
			cout<<"error �����ļ�ʧ��!"<<endl;
			return false;
		}
		fclose(fp);
	}
	//�������ļ�����
	string mainStr;//���ļ�����
	{
		//���ļ���ʽ��
		//model{
	    //    xmin,xmax,ymin,ymax,zmin,zmax;
        //    offsetus,offsetut;
        //    cloudList{
        //        ncloud;
        //        cloud{"����ڱ��ļ���·��"}
        //        cloud{"����ڱ��ļ���·��"}
        //        ....
        //    }
        //    groundgrid{"����ڱ��ļ���·��"}
		//    texture{
		//        photo{"����ڱ��ļ���·��"}
		//        wall{"����ڱ��ļ���·��"}
		//        sky{"����ڱ��ļ���·��"}
		//    }
        //}
		//
		mainStr+=toS("model{")+"\n";
		mainStr+=f2str(xmin)+","+f2str(xmax)+","+f2str(ymin)+","+f2str(ymax)+","+f2str(zmin)+","+f2str(zmax)+";"+"\n";
		mainStr+=f2str(offsetus)+","+f2str(offsetut)+";"+"\n";
		mainStr+=toS("cloudList{")+"\n";
		mainStr+=i2str(ncloud)+";"+"\n";
		//��CFNameListд��str
		{
			int nCF=(int)CFNameList.size();
			for(int i=0;i<nCF;i++){
				mainStr+=toS("cloud{\"")+CFNameList[i]+toS("\"}")+"\n";
			}
		}
		mainStr+=toS("}")+"\n";
		mainStr+="groundgrid{\""+GFName+"\"}"+"\n";
		cout<<"save texture"<<endl;//��һ����ʾ
		mainStr+="texture{\n";
		mainStr+="photo{\"..\\"+picFileName_photo+"\"}\n";
		mainStr+="wall{\"..\\"+picFileName_wall+"\"}\n";
		mainStr+="sky{\"..\\"+picFileName_sky+"\"}\n";
		mainStr+=toS("}")+"\n";
		mainStr+=toS("}")+"\n";	
	}
	//��mainStrд�����ļ�
	{
		string MFpathName=pathName+clipName+"\\model.model";//���ļ�·����
		FILE*fp;
		fp=fopen(MFpathName.c_str(),"w");
		if(fp==NULL){
			cout<<"error �����ļ�ʧ��!"<<endl;
			return false;
		}
		fwrite(mainStr.c_str(),(int)mainStr.size()*sizeof(char),1,fp);
		fclose(fp);
	}
	//����������ݵ������ļ�
	{
		cout<<"save ground"<<endl;
		string GFpathName=pathName+clipName+"\\groundgrid.gg";//�����ļ�·����
		FILE*fp;
		fp=fopen(GFpathName.c_str(),"a");//׷��
		if(fp==NULL){
			cout<<"error ���ļ�ʧ��!"<<endl;
			return false;
		}
		//�������
		{
			//groundgrid�ļ���ʽ��
			//groundgrid{
			//    xmin,xmax,zmin,zmax;
			//    xstepLen,zstepLen;
			//    mat{
			//        m,n;
			//        groundcell{
			//            v{x,y,z,1}
			//            u{s,t}
			//        }
			//        groundcell{
			//            ....
			//        }
			//        ....
			//    }
			//}
			//
			//�����ı�
			fprint4f(fp,groundgrid.xmin,groundgrid.xmax,groundgrid.zmin,groundgrid.zmax);
			fprint2f(fp,groundgrid.xstepLen,groundgrid.zstepLen);
			int m=groundgrid.m_mat;
			int n=groundgrid.n_mat;
			fprint2d(fp,m,n);
			//����m*n��groundcell������
			{
				float hfdz=groundgrid.zstepLen/2;
				float hfdx=groundgrid.xstepLen/2;
				for(int i=0;i<m;i++){
					for(int j=0;j<n;j++){
						const Cgroundcell&gcell=groundgrid.mat[i][j];
						//�����ı�
						float v[4]={groundgrid.xmin+groundgrid.xstepLen*j+hfdx,groundgrid.mat[i][j].h,groundgrid.zmin+groundgrid.zstepLen*i+hfdz,1};
						fprint4fv(fp,v);
						fprint2fv(fp,groundgrid.mat[i][j].u);
					}
					
				}
			}
		}
		//�ر�
		fclose(fp);
	}
	//д�������ļ�
	{
		//ͬʱ�ڿ���̨������ȣ�ÿ10������Ϊһ�����
		cout<<"save cloud: ";
		int nc=(int)cloudList.size();
		int curID=0;
		for(int i=0;i<nc;i++){
			const Ccloud&cloud=cloudList[i];
			if(cloud.removed)continue;
			int r=curID%10;
			if(r==0){
				cout<<curID<<"~";
			}else if(r==9){
				cout<<curID<<" ";
			}
			string CFName=CFNameList[curID];
			//�����ļ���ʽ
			//cloud{
			//    avrgH;
			//	  bottomH;
	        //    topH;
	        //    minH;
	        //    maxH;
			//    color;
			//	  removed;
			// 	  xmin,xmax,zmin,zmax;
		   	//    polygenList{....}
			//    polygenList{....}
			//    grid{....}
			//}
			string CFpathName=pathName+clipName+"\\"+CFName;//�����ļ�·����
			FILE*fp;
			fp=fopen(CFpathName.c_str(),"a");//׷��
			if(fp==NULL){
				cout<<"error ���ļ�ʧ��!"<<endl;
				return false;
			}
			//�������
			{
				fprint1f(fp,cloud.avrgH);
				fprint1f(fp,cloud.bottomH);
				fprint1f(fp,cloud.topH);
				fprint1f(fp,cloud.minH);
				fprint4fv(fp,cloud.color);
				fprint1d(fp,cloud.removed);
				fprint1f(fp,cloud.xmin);fprint1f(fp,cloud.xmax);fprint1f(fp,cloud.zmin);fprint1f(fp,cloud.zmax);
				fprintPolygenList(fp,cloud.polygenList);
				fprintPolygenList(fp,cloud.polygenList_dn);
				fprintGrid(fp,cloud.grid);
				
			}
			//�ر��ļ�
			fclose(fp);
			
			//curID����
			curID++;
		}
		//���ݴ�ʱ��curID-1����10�����������Ƿ��β
		int r=(curID-1)%10;
		if(r==9){//���÷�β
			cout<<endl;
		}else{//��Ҫ��β
			cout<<curID-1<<endl;
		}
	}

	return true;


}

inline string f2str(float f){//������ת�ַ���
	
	if(_isnan(f)){
		string tstr="0";
		return tstr;
	}
	CString tstr;
	tstr.Format("%f",f);
	return tstr;
}
inline string i2str(int i){//����ת�ַ���
	if(_isnan(i)){
		string tstr="0";
		return tstr;
	}
	CString tstr;
	tstr.Format("%d",i);
	return tstr;
}
inline string toS(string str){
	return str;
}
inline void fprintPolygenList(FILE   *fp,const myvector<Cpolygen>&polygenList){
	//polygenList{
    //    npolygen;
    //    polygen{...}
    //    polygen{...}
	//    ....
	//}
	int npolygen=(int)polygenList.size();
	fprint1d(fp,npolygen);
	for(int i=0;i<npolygen;i++){
		const Cpolygen&polygen=polygenList[i];
		if(polygen.removed)continue;
		fprintPolygen(fp,polygen);
	}

}
inline void fprintPolygen(FILE   *fp,const Cpolygen&polygen){
	//polygen{
	//    removed;
	//    vlist{
	//        nv;
	//        v{
	//            isKeyPoint;
	//            v{x,y,z,1}
	//            u{s,t}
	//            uf{sf,tf}
	//            un{sn,tn}
	//        }
	//        v{
	//            ....
	//        }
	//        ....
	//    }
	//}
	int nv=(int)polygen.vlist.size();
	bool removed=polygen.removed;
	fprint1d(fp,removed);
	fprint1d(fp,nv);
	//������v{...}�ı�
	for(int i=0;i<nv;i++){
		const Cv4Ex&v=polygen.vlist[i];
		//����v{...}�ı�
		fprint1d(fp,v.isKeyPoint);
		fprint4fv(fp,v.v);
		fprint2fv(fp,v.u);
		fprint2fv(fp,v.uf);
		fprint2fv(fp,v.un);
	
	}
}
inline void fprint1f(FILE   *fp,float x){//��fprintf(,"%f ",)�ķ�װ��ͬʱҪ���nan���������ļ��������ֵ
	if(_isnan(x)){//���xΪnan����ǿ��Ϊ0
		if(x>0){
			x=inf;
		}else{
			x=-inf;
		}
	}
	fprintf(fp,"%f ",x);

}
inline void fprint1d(FILE   *fp,int x){//��fprintf(,"%d ",)�ķ�װ��ͬʱҪ���nan���������ļ��������ֵ
	if(_isnan(x)){//���xΪnan����ǿ��Ϊ0
		if(x>0){
			x=inf;
		}else{
			x=-inf;
		}
	}
	fprintf(fp,"%d ",x);
}
inline void fprint4fv(FILE   *fp,const float vec[4]){
	fprint1f(fp,vec[X]);
	fprint1f(fp,vec[Y]);
	fprint1f(fp,vec[Z]);
	fprint1f(fp,vec[W]);

}
inline void fprint4f(FILE   *fp,float x,float y,float z,float w){
	fprint1f(fp,x);
	fprint1f(fp,y);
	fprint1f(fp,z);
	fprint1f(fp,w);

}

inline void fprint2fv(FILE   *fp,const float vec[2]){
	fprint1f(fp,vec[0]);
	fprint1f(fp,vec[1]);

}
inline void fprint2f(FILE   *fp,float x,float y){
	fprint1f(fp,x);
	fprint1f(fp,y);

}
inline void fprint2d(FILE   *fp,int m,int n){
	fprint1d(fp,m);
	fprint1d(fp,n);

}
inline void fprint16fv(FILE   *fp,const float vec[16]){
	for(int i=0;i<16;i++){
		fprint1f(fp,vec[i]);
	}
}
inline void fprintGrid(FILE   *fp,const Cgrid&grid){
	//grid{
	//    xmin,xmax,zmin,zmax;
	//    xstepLen,zstepLen;
    //    m_locToWorld{a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p}
	//    m_worldToLoc{a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p}
	//    mat{....}
	//}

	float xmin,xmax,zmin,zmax;
	grid.getRange(xmin,xmax,zmin,zmax);
	//��ʼ�����ı�
	fprint4f(fp,xmin,xmax,zmin,zmax);
	fprint2f(fp,grid.xstepLen,grid.zstepLen);
	fprint16fv(fp,grid.m_locToWorld);
	fprint16fv(fp,grid.m_worldToLoc);
	fprintMat(fp,grid.mat,grid.m_mat,grid.n_mat);
}
inline void fprintMat(FILE   *fp,const myvector<myvector<Ccell*> >&mat,int m_mat,int n_mat){
		//mat{
	    //    m,n;
        //    cell{...}
		// 	  cell{...}
		//	  ....
        //}
	//���
	if(m_mat!=(int)mat.size()){
		cout<<"error mat�ߴ粻ƥ��!"<<endl;
	}
	if(m_mat!=0){
		if(n_mat!=(int)mat[0].size()){
			cout<<"error mat�ߴ粻ƥ��!"<<endl;
		}
	}
	//�������	
	int m,n;
	m=m_mat;
	n=n_mat;
	//�����ı�
	fprint2d(fp,m,n);
	for(int i=0;i<m;i++){
		for(int j=0;j<n;j++){
			Ccell*pcell=mat[i][j];
			fprintCell(fp,pcell);
		}
	}
}
inline void fprintCell(FILE   *fp,const Ccell*pcell){
	//		cell{
 	//         	isNull;
    //          needcut; 
	//		    v{x,y,z,1}
	//		    u{s,t}
	//		    vlt{x,y,z,1}
	//		    vrt{x,y,z,1}
	//		    vup{x,y,z,1}
	//	        vdn{x,y,z,1}
	//	        vlup{x,y,z,1}
	//		    vldn{x,y,z,1}
	//		    vrup{x,y,z,1}
	//		    vrdn{x,y,z,1}
	//		    ult{s,t}
	//		    urt{s,t}
	//		    uup{s,t}
	//		    udn{s,t}
	//		    ulup{s,t}
	//		    uldn{s,t}
	//		    urup{s,t}
	//		    urdn{s,t} 
	//          normA{x,y,z,0}
	//		    normB{x,y,z,0}
	//		    normC{x,y,z,0}
	//	 	    normD{x,y,z,0}
	//	        avrgnorm{x,y,z,0}  
   	//          polygenList{....}
	//		}
	//
	//�����ı�
	bool isNull=(pcell==NULL);
	if(isNull){	
		fprint1d(fp,isNull);
		return;
	}else{
		fprint1d(fp,isNull);
		fprint1d(fp,pcell->needcut);
		fprint4fv(fp,pcell->v);
		fprint2fv(fp,pcell->u);
		fprint4fv(fp,pcell->vlt);
		fprint4fv(fp,pcell->vrt);
		fprint4fv(fp,pcell->vup);
		fprint4fv(fp,pcell->vdn);
		fprint4fv(fp,pcell->vlup);
		fprint4fv(fp,pcell->vldn);
		fprint4fv(fp,pcell->vrup);
		fprint4fv(fp,pcell->vrdn);
		fprint2fv(fp,pcell->ult);
		fprint2fv(fp,pcell->urt);
		fprint2fv(fp,pcell->uup);
		fprint2fv(fp,pcell->udn);
		fprint2fv(fp,pcell->ulup);
		fprint2fv(fp,pcell->uldn);
		fprint2fv(fp,pcell->urup);
		fprint2fv(fp,pcell->urdn);
		fprint4fv(fp,pcell->norm[0]);
		fprint4fv(fp,pcell->norm[1]);
		fprint4fv(fp,pcell->norm[2]);
		fprint4fv(fp,pcell->norm[3]);
		fprint4fv(fp,pcell->avrgnorm);
		fprintPolygenList(fp,pcell->polygenList);

	}


	
}
