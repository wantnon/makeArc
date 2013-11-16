#define MAX_TEXBMP_WIDTH 3500
#define MAX_TEXBMP_HEIGHT 3500
class CTextureManager{
private:
	myvector<GLuint> textureList;//�洢����
public:
	~CTextureManager(){
		destroy();
	
	}
	int getTextureCount()const{
		return (int)textureList.size();
	}
	destroy(){
		//ɾ������
		int ntex=(int)textureList.size();
		for(int i=ntex-1;i>=0;i--){
			glDeleteTextures(1,&textureList[i]);
		}
		//�б����
		textureList.resize(0);


	}
	GLuint getTex(int textureIndex)const {
		if(textureIndex<0||textureIndex>=(int)textureList.size()){
			cout<<"��������Խ��!"<<endl;
			return 0;
		}else{
			return textureList[textureIndex];
		}
	}
	bool CreateTexture(string filename){//��������
		//��������ͼƬ�Ĵ�С
		{
			int w,h;
			if(!getBmp24Size(filename,w,h)){
				CString str;
				str.Format("��24λbmp�ļ�\"%s\"ʱ��������!",filename.c_str());
				AfxMessageBox(str);
				return false;
			}
			if(w>MAX_TEXBMP_WIDTH||h>MAX_TEXBMP_HEIGHT){
				CString str;
				str.Format("��������ʧ��!\n\nԭ��bmp�ߴ糬��%d*%d",MAX_TEXBMP_WIDTH,MAX_TEXBMP_HEIGHT);
				AfxMessageBox(str);
				return false;
			}
		}
		//����λͼ��ָ�����TextureImage
		AUX_RGBImageRec *TextureImage=LoadBMP((char*)filename.c_str());
		if (TextureImage!=NULL)
		{
			//���1������Ŵ���texture
			GLuint texture;
			glGenTextures(1, &texture);
			//��texture������ǰ2d����
			glBindTexture(GL_TEXTURE_2D, texture);
			glPixelStorei(GL_UNPACK_ALIGNMENT,1);//�˺���ʹ��bmpͼƬ����4��������Ҳ��������ʾ
			//Ϊ��ǰ����������*TextureImage��
			gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TextureImage->sizeX, TextureImage->sizeY, GL_RGB, GL_UNSIGNED_BYTE, TextureImage->data);
			//��������������ͼ�����˲�
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			//��texture���뵽textureList
			textureList.push_back(texture);
			//����TextureImage
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
			cout<<"λͼ����ʧ��!"<<endl;
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

