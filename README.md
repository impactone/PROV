# 2016年辽宁省机器人大赛 以上代码全部下载烧录方能使用

任务要求 5000*5000的场地 将蓝色高尔夫放置到蓝色框  将红色高尔夫放置到红色框


#车载传感器  
TCS230颜色传感器用来检测颜色球分放到两边的球室内

5110实时查看车载传感器信息

全局定位模块 提供精准两轴坐标 和单轴角度

大扭矩电机两个 用PWN控制

分球室大分盘用步进电机控制

小气瓶把球喷射入球框

#HARDWARE 文件是各个传感器的配置文件

#ALGORITHM 里面有姿态解算算法 让陀螺仪1ms返回一次数据

