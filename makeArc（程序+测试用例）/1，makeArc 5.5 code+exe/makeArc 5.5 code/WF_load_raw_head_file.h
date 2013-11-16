//-------------------------------
//��������ͷ�ļ�
//-------------------------------
inline bool rid_barN_atStringEnd(string&);

inline bool main_load_raw_head_file(string pathName_RHF,//pathName of raw head file
									string&fileName_cloud,
									string&fileName_photo,
									string&fileName_wall,
									string&fileName_sky,
									string&pathName_cloud,
									string&pathName_photo,
									string&pathName_wall,
									string&pathName_sky)
//����pathName_RHF��������õ��ļ�����·����
{
	//��ȡpathName�ļ���ǰ���У������fileName
	{
		//���ļ�
		FILE  *infile;
		infile=fopen(pathName_RHF.c_str(),"r");
		if(infile==NULL)
		{ 
			cout<<"Open "<<pathName_RHF<<" failed!"<<endl;
			return false;
		}
		//��ȡ����
		{
			char buff[MAX_PATH];//������
			//��ȡfileName_cloud
			fgets(buff,MAX_PATH,infile);
			fileName_cloud=buff;
			rid_barN_atStringEnd(fileName_cloud);
			//��ȡfileName_photo
			fgets(buff,MAX_PATH,infile);
			fileName_photo=buff;
			rid_barN_atStringEnd(fileName_photo);
			//��ȡfileName_wall
			fgets(buff,MAX_PATH,infile);
			fileName_wall=buff;
			rid_barN_atStringEnd(fileName_wall);
			//��ȡfileName_sky
			fgets(buff,MAX_PATH,infile);
			fileName_sky=buff;
			rid_barN_atStringEnd(fileName_sky);
		}
		//�ر��ļ�
		fclose(infile);	
		//�ø�fileName��pathName��װ�ɸ�pathName
		{
			//��·��ǰ׺
			string pathNamePrefix;
			{
				pathNamePrefix=pathName_RHF;
				//��pathNamePrefix�����һ��'\\'��Ķ�ɾ��
				int count=0;//�������һ��'\\'֮����ַ�����
				int n=(int)pathNamePrefix.size();
				for(int i=n-1;i>=0;i--){
					if(pathNamePrefix[i]!='\\'){
						count++;
					}else{
						break;
					}
				}//�õ�count
				//ȥ��pathNamePrefix�����count���ַ�
				pathNamePrefix.resize(n-count);
				
			}
			//��pathNamePrefix�͸�fileName��ɸ�pathName
			pathName_cloud=pathNamePrefix+fileName_cloud;
			pathName_photo=pathNamePrefix+fileName_photo;
			pathName_wall=pathNamePrefix+fileName_wall;
			pathName_sky=pathNamePrefix+fileName_sky;
		}
	}//�õ���·��
	return true;

}


inline bool rid_barN_atStringEnd(string&str){//ȥ��string��β��'\n'��ǰ�˷����Ƿ��Ƴ��ɹ�
	int n=(int)str.size();
	if(str[n-1]=='\n'){
		str.resize(n-1);
		return true;
	}else{
		return false;
	}
}