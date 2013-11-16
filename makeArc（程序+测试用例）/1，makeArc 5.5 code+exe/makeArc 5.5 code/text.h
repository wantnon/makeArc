
void Begin2D(int style){//��ʼ��2D����
	//��ʼ��2D����
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();//���浱ǰͶӰ����
	glLoadIdentity();
	//������ͶӰ
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT,viewport);
	gluOrtho2D(viewport[0],viewport[2],viewport[1],viewport[3]);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();//���浱ǰģ����ͼ����
	glLoadIdentity();
	if(style==SCREEN_COOR){
		// ��תY�ᣨ����Ϊ������(�봰������һ��)
		glScalef(1, -1, 1);
		// ��ԭ���ƶ�����Ļ���Ϸ�(�봰������һ��)
		glTranslatef(0, - viewport[3], 0);
	}else if(style==MATH_COOR){
		//�޶���
	}

}
void End2D(){
	// �ָ�֮ǰ�����ģ����ͼ����
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	// �ָ�֮ǰ�����ͶӰ����
	glPopMatrix();
	// ����ģ����ͼ����״̬
	glMatrixMode(GL_MODELVIEW);
}
/////////////////��ʾ�ı�
//font����ֵ��
//GLUT_BITMAP_8_BY_13:һ�̶ֹ��������,ÿ���ַ�������һ��8x13���صľ��ο���
//GLUT_BITMAP_9_BY_15:һ�̶ֹ��������,ÿ���ַ�������һ��9x15���صľ��ο���
//GLUT_BITMAP_TIMES_ROMAN_10:һ��10����ȼ���Times Roman����
//GLUT_BITMAP_TIMES_ROMAN_24:һ��24����ȼ���Times Roman����
//GLUT_BITMAP_HELVETICA_10:һ��10����ȼ���Helvetica����
//GLUT_BITMAP_HELVETICA_12:һ��12����ȼ���Helvetica����
//GLUT_BITMAP_HELVETICA_18:һ��18����ȼ���Helvetica����
void Text(const char* text, float x, float y){  
	// ��������λ��
	glRasterPos2f( x, y );
	// ������������ַ�(ʹ����ʾ�б�)
	for(const char* c = text; *c != '\0'; c++)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
	}
}
