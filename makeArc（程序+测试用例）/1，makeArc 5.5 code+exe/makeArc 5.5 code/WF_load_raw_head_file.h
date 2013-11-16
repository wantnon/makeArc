//-------------------------------
//加载数据头文件
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
//输入pathName_RHF，输出析得的文件名和路径名
{
	//读取pathName文件的前四行，存入各fileName
	{
		//打开文件
		FILE  *infile;
		infile=fopen(pathName_RHF.c_str(),"r");
		if(infile==NULL)
		{ 
			cout<<"Open "<<pathName_RHF<<" failed!"<<endl;
			return false;
		}
		//读取数据
		{
			char buff[MAX_PATH];//缓冲区
			//读取fileName_cloud
			fgets(buff,MAX_PATH,infile);
			fileName_cloud=buff;
			rid_barN_atStringEnd(fileName_cloud);
			//读取fileName_photo
			fgets(buff,MAX_PATH,infile);
			fileName_photo=buff;
			rid_barN_atStringEnd(fileName_photo);
			//读取fileName_wall
			fgets(buff,MAX_PATH,infile);
			fileName_wall=buff;
			rid_barN_atStringEnd(fileName_wall);
			//读取fileName_sky
			fgets(buff,MAX_PATH,infile);
			fileName_sky=buff;
			rid_barN_atStringEnd(fileName_sky);
		}
		//关闭文件
		fclose(infile);	
		//用各fileName和pathName组装成各pathName
		{
			//求路径前缀
			string pathNamePrefix;
			{
				pathNamePrefix=pathName_RHF;
				//将pathNamePrefix中最后一个'\\'后的段删除
				int count=0;//计数最后一个'\\'之后的字符个数
				int n=(int)pathNamePrefix.size();
				for(int i=n-1;i>=0;i--){
					if(pathNamePrefix[i]!='\\'){
						count++;
					}else{
						break;
					}
				}//得到count
				//去掉pathNamePrefix的最后count个字符
				pathNamePrefix.resize(n-count);
				
			}
			//用pathNamePrefix和各fileName组成各pathName
			pathName_cloud=pathNamePrefix+fileName_cloud;
			pathName_photo=pathNamePrefix+fileName_photo;
			pathName_wall=pathNamePrefix+fileName_wall;
			pathName_sky=pathNamePrefix+fileName_sky;
		}
	}//得到各路径
	return true;

}


inline bool rid_barN_atStringEnd(string&str){//去掉string结尾的'\n'，前端返回是否移除成功
	int n=(int)str.size();
	if(str[n-1]=='\n'){
		str.resize(n-1);
		return true;
	}else{
		return false;
	}
}