#define MAX_TEXBMP_WIDTH 3500
#define MAX_TEXBMP_HEIGHT 3500
class CTextureManager{
private:
	myvector<GLuint> textureList;//存储纹理
public:
	~CTextureManager(){
		destroy();
	
	}
	int getTextureCount()const{
		return (int)textureList.size();
	}
	destroy(){
		//删除纹理
		int ntex=(int)textureList.size();
		for(int i=ntex-1;i>=0;i--){
			glDeleteTextures(1,&textureList[i]);
		}
		//列表清空
		textureList.resize(0);


	}
	GLuint getTex(int textureIndex)const {
		if(textureIndex<0||textureIndex>=(int)textureList.size()){
			cout<<"纹理索引越界!"<<endl;
			return 0;
		}else{
			return textureList[textureIndex];
		}
	}
	bool CreateTexture(string filename){//创建纹理
		//限制纹理图片的大小
		{
			int w,h;
			if(!getBmp24Size(filename,w,h)){
				CString str;
				str.Format("打开24位bmp文件\"%s\"时发生错误!",filename.c_str());
				AfxMessageBox(str);
				return false;
			}
			if(w>MAX_TEXBMP_WIDTH||h>MAX_TEXBMP_HEIGHT){
				CString str;
				str.Format("创建纹理失败!\n\n原因：bmp尺寸超过%d*%d",MAX_TEXBMP_WIDTH,MAX_TEXBMP_HEIGHT);
				AfxMessageBox(str);
				return false;
			}
		}
		//加载位图，指针存入TextureImage
		AUX_RGBImageRec *TextureImage=LoadBMP((char*)filename.c_str());
		if (TextureImage!=NULL)
		{
			//获得1个纹理号存入texture
			GLuint texture;
			glGenTextures(1, &texture);
			//将texture用作当前2d纹理
			glBindTexture(GL_TEXTURE_2D, texture);
			glPixelStorei(GL_UNPACK_ALIGNMENT,1);//此函数使得bmp图片不是4的整数倍也能正常显示
			//为当前纹理创建（用*TextureImage）
			gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TextureImage->sizeX, TextureImage->sizeY, GL_RGB, GL_UNSIGNED_BYTE, TextureImage->data);
			//对所创建的纹理图进行滤波
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			//将texture加入到textureList
			textureList.push_back(texture);
			//销毁TextureImage
			if (TextureImage)									// If Texture Exists
			{
				if (TextureImage->data)							// If Texture Image Exists
				{
					free(TextureImage->data);					// Free The Texture Image Memory
				}
				
				free(TextureImage);								// Free The Image Structure
			}
			
			return true;
			
		}else{
			cout<<"位图加载失败!"<<endl;
			return false;
		}
	}
private:
	AUX_RGBImageRec *LoadBMP(char Filename[]){     // Loads A Bitmap Image
		FILE *File=NULL;          // File Handle
		if (!Filename){           // Make Sure A Filename Was Given
			return NULL;          // If Not Return NULL
		}
		File=fopen(Filename,"r");        // Check To See If The File Exists
		if (File){            // Does The File Exist?
			fclose(File);          // Close The Handle
			return auxDIBImageLoad(Filename);     // Load The Bitmap And Return A Pointer
		}
		return NULL;           // If Load Failed Return NULL
	}
	
	
};

