//-----------------------------
//为地形网格计算纹理坐标
//-----------------------------
inline void main_cal_texCoord_for_groundgrid(Cgroundgrid&GroundGrid,float offsetus,float offsetut){
	GroundGrid.caluOfEachCell(offsetus,offsetut);

}