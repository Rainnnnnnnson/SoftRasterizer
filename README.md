# SoftRasterizer
代码大约2500行(不包括单元测试以及应用部分)

# 实现功能
1.支持顶点着色器和像素着色器  
2.绘制模式和线框模式(两种模式下边缘相同,分割出来的三角形也相同)  
3.Z-Buffer(线框覆盖在最前面)  
4.背面剔除  
5.完全不可见剔除  
6.近平面剪裁  
7.远平面剪裁(线框模式)  
8.透视矫正  
9.双线性滤波采样  
10.ShadowMap  
11.数学库  
12.方块和球的网格

# 效果图
被近平面剪裁的方块(正交投影)
![image](https://github.com/Rainnnnnnnson/SoftRasterizer/blob/master/OrthogonalCube.PNG)
被近平面和远平面剪裁的方块(透视投影)
![image](https://github.com/Rainnnnnnnson/SoftRasterizer/blob/master/perspectiveCubeWithWireframe.PNG)
纹理方块
![image](https://github.com/Rainnnnnnnson/SoftRasterizer/blob/master/TextureCube.PNG)
球型线框
![image](https://github.com/Rainnnnnnnson/SoftRasterizer/blob/master/SphereWireframe.PNG)
球体实现gouraud光照
![image](https://github.com/Rainnnnnnnson/SoftRasterizer/blob/master/Gouraud.PNG)
球体实现phong光照
![image](https://github.com/Rainnnnnnnson/SoftRasterizer/blob/master/Phong.PNG)
shadowMap(光从方块的右前上方射过来)
![image](https://github.com/Rainnnnnnnson/SoftRasterizer/blob/master/shadowMap.PNG)
