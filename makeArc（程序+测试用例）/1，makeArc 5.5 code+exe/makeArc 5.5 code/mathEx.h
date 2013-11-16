inline int pow_i_i(int x,int y)
//计算x^y，其中x,y均为非负整数
{
	if(y==0)return 1;

	int r=x;
	for(int i=1;i<y;i++){//共循环y-1次
		r*=x;
	}
	return r;
}
void printv(float v[4]){
	cout<<v[X]<<" "<<v[Y]<<" "<<v[Z]<<endl;
}
void print4fv(float v[4]){
	cout<<v[X]<<" "<<v[Y]<<" "<<v[Z]<<" "<<v[W]<<endl;
}
void print2fv(float v[2]){
	cout<<v[X]<<" "<<v[Y]<<endl;

}

inline float CarmSqrt(float x)
{
	union{
		int intPart;
		float floatPart;
	}cv1,cv2;
	cv1.floatPart = x;
	cv2.floatPart = x;
	cv1.intPart = 0x1FBCF800 + (cv1.intPart >> 1);
	cv2.intPart = 0x5f3759df - (cv2.intPart >> 1);
	return 0.5f*(cv1.floatPart + (x * cv2.floatPart));

};
//-----------------------------数学运算
//注意：必定不允许出现处理3维向量的函数，只有2维或4维。也就是说在3d图形学编程一定要杜绝使用3维向量。
//否则极易造成debug正常而release不正常的情况，原因就是使用了处理4维向量的函数去处理3维向量
//linline函数必须把声明和实现都写在.h文件里
inline int rem(int a,int b){//比"%"更合逻辑的取余运算
	int r=a%b;
	if(r<0)return r+b;
	else return r;
}
inline void init4(float vec[4],float x,float y,float z,float w){
	vec[0]=x;
	vec[1]=y;
	vec[2]=z;
	vec[3]=w;
}
inline void init2(float v[2],float x,float y){
	v[0]=x;
	v[1]=y;
}

inline float getLen(const float v[4]){
	return sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
}
inline float getLen_2(const float v[4]){//返回模的平方
	return v[0]*v[0]+v[1]*v[1]+v[2]*v[2];
}
inline void sub(const float v1[4],const float v2[4],float vrs[4]){
	vrs[0]=v1[0]-v2[0];
	vrs[1]=v1[1]-v2[1];
	vrs[2]=v1[2]-v2[2];
	vrs[3]=v1[3]-v2[3];
	
}
inline void add(const float v1[4],const float v2[4],float vrs[4]){
	vrs[0]=v1[0]+v2[0];
	vrs[1]=v1[1]+v2[1];
	vrs[2]=v1[2]+v2[2];
	vrs[3]=v1[3]+v2[3];
}
inline void addadd(const float v1[4],const float v2[4],const float v3[4],float vrs[4]){
	vrs[0]=v1[0]+v2[0]+v3[0];
	vrs[1]=v1[1]+v2[1]+v3[1];
	vrs[2]=v1[2]+v2[2]+v3[2];
	vrs[3]=v1[3]+v2[3]+v3[3];
}
inline float dot(const float v1[4],const float v2[4]){
	return v1[X]*v2[X]+v1[Y]*v2[Y]+v1[Z]*v2[Z];

}
inline void mul(float k,const float v[4],float vrs[]){
	vrs[0]=v[0]*k;
	vrs[1]=v[1]*k;
	vrs[2]=v[2]*k;
	vrs[3]=v[3]*k;
}

inline void mul_mat(float k,float mat[16]){
	for(int i=0;i<=15;i++){
	mat[i]*=k;
	}
}

inline void add_mat(float a[16],float b[16],float rs[16]){
	for(int i=0;i<=15;i++){
		rs[i]=a[i]+b[i];
	}

}

inline float normalize(float v[4])//前端返回模
{
    float r;

    r = sqrt( v[0]*v[0] + v[1]*v[1] + v[2]*v[2] );
    if (r == 0.0) return 0;

    v[0] /=r;
    v[1] /=r;
    v[2] /=r;
	return r;
}

inline void cross(const float v1[4], const float v2[4], float result[4])
{
    result[0] = v1[1]*v2[2] - v1[2]*v2[1];
    result[1] = v1[2]*v2[0] - v1[0]*v2[2];
    result[2] = v1[0]*v2[1] - v1[1]*v2[0];
	result[3] = 0;//因为叉乘结果必是向量，所以第四维为0
}
inline void makeE(float m[16])
{
	m[0] = 1; m[4] = 0; m[8] = 0; m[12] = 0;
    m[1] = 0; m[5] = 1; m[9] = 0; m[13] = 0;
    m[2] = 0; m[6] = 0; m[10] = 1; m[14] = 0;
    m[3] = 0; m[7] = 0; m[11] = 0; m[15] = 1;
}


inline void prodv(const float matrix[16], const float in[4],
		      float out[4]){//in和out可以是同一数组
	float _out[4];//临时中转
	_out[0]=in[0]*matrix[0]+in[1]*matrix[4]+in[2]*matrix[8]+in[3]*matrix[12];
	_out[1]=in[0]*matrix[1]+in[1]*matrix[5]+in[2]*matrix[9]+in[3]*matrix[13];
	_out[2]=in[0]*matrix[2]+in[1]*matrix[6]+in[2]*matrix[10]+in[3]*matrix[14];
	_out[3]=in[0]*matrix[3]+in[1]*matrix[7]+in[2]*matrix[11]+in[3]*matrix[15];
	init4(out,_out[0],_out[1],_out[2],_out[3]);
}

inline int inv(const float src[16], float inverse[16])//两者可为同一矩阵
{
    int i, j, k, swap;
    float t;
    float temp[4][4];

    for (i=0; i<4; i++) {
	for (j=0; j<4; j++) {
	    temp[i][j] = src[i*4+j];
	}
    }
    makeE(inverse);

    for (i = 0; i < 4; i++) {
	/*
	** Look for largest element in column
	*/
	swap = i;
	for (j = i + 1; j < 4; j++) {
	    if (fabs(temp[j][i]) > fabs(temp[i][i])) {
		swap = j;
	    }
	}

	if (swap != i) {
	    /*
	    ** Swap rows.
	    */
	    for (k = 0; k < 4; k++) {
		t = temp[i][k];
		temp[i][k] = temp[swap][k];
		temp[swap][k] = t;

		t = inverse[i*4+k];
		inverse[i*4+k] = inverse[swap*4+k];
		inverse[swap*4+k] = t;
	    }
	}

	if (temp[i][i] == 0) {
	    /*
	    ** No non-zero pivot.  The matrix is singular, which shouldn't
	    ** happen.  This means the user gave us a bad matrix.
	    */
	    return GL_FALSE;
	}

	t = temp[i][i];
	for (k = 0; k < 4; k++) {
	    temp[i][k] /= t;
	    inverse[i*4+k] /= t;
	}
	for (j = 0; j < 4; j++) {
	    if (j != i) {
		t = temp[j][i];
		for (k = 0; k < 4; k++) {
		    temp[j][k] -= temp[i][k]*t;
		    inverse[j*4+k] -= inverse[i*4+k]*t;
		}
	    }
	}
    }
    return GL_TRUE;
}

inline void matcopy( const float src[16],float tg[16])
{
	tg[0]=src[0];tg[1]=src[1];tg[2]=src[2];tg[3]=src[3];
	tg[4]=src[4];tg[5]=src[5];tg[6]=src[6];tg[7]=src[7];
	tg[8]=src[8];tg[9]=src[9];tg[10]=src[10];tg[11]=src[11];
	tg[12]=src[12];tg[13]=src[13];tg[14]=src[14];tg[15]=src[15];
}
inline void prodm( const float b[16],const float a[16],
				float r[16])//r可以是a,b之一
{
	float _r[16];
	_r[0]=a[0]*b[0]+a[1]*b[4]+a[2]*b[8]+a[3]*b[12];
	_r[1]=a[0]*b[1]+a[1]*b[5]+a[2]*b[9]+a[3]*b[13];
	_r[2]=a[0]*b[2]+a[1]*b[6]+a[2]*b[10]+a[3]*b[14];
	_r[3]=a[0]*b[3]+a[1]*b[7]+a[2]*b[11]+a[3]*b[15];
	
	_r[4]=a[4]*b[0]+a[5]*b[4]+a[6]*b[8]+a[7]*b[12];
	_r[5]=a[4]*b[1]+a[5]*b[5]+a[6]*b[9]+a[7]*b[13];
	_r[6]=a[4]*b[2]+a[5]*b[6]+a[6]*b[10]+a[7]*b[14];
	_r[7]=a[4]*b[3]+a[5]*b[7]+a[6]*b[11]+a[7]*b[15];
	
	_r[8]=a[8]*b[0]+a[9]*b[4]+a[10]*b[8]+a[11]*b[12];
	_r[9]=a[8]*b[1]+a[9]*b[5]+a[10]*b[9]+a[11]*b[13];
	_r[10]=a[8]*b[2]+a[9]*b[6]+a[10]*b[10]+a[11]*b[14];
	_r[11]=a[8]*b[3]+a[9]*b[7]+a[10]*b[11]+a[11]*b[15];
	
	_r[12]=a[12]*b[0]+a[13]*b[4]+a[14]*b[8]+a[15]*b[12];
	_r[13]=a[12]*b[1]+a[13]*b[5]+a[14]*b[9]+a[15]*b[13];
	_r[14]=a[12]*b[2]+a[13]*b[6]+a[14]*b[10]+a[15]*b[14];
	_r[15]=a[12]*b[3]+a[13]*b[7]+a[14]*b[11]+a[15]*b[15];
	matcopy(_r,r);

}

inline void veccopy(const float vec[4],float tgvec[4]){
	tgvec[0]=vec[0];
	tgvec[1]=vec[1];
	tgvec[2]=vec[2];
	tgvec[3]=vec[3];
}
inline float pow2(float a){//平方
	return a*a;
}


