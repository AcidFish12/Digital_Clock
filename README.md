# Digital_Clock
## 文件介绍
## 摘要
数字钟是寒假的一个培训项目：以单片机MSP430为核心设计制作了数字钟系统。由开发板和拓展板组成，其中拓展板由矩阵键盘，OLED显示屏幕，若干LED灯构成。数字钟实现了基本24小时/12小时制时间显示，日期显示，温度显示。具备设置时间，设置日期，设置三个独立闹钟和计时器功能，同时具有完整的异常处理。时间日期的循环通过定时器中断获得1S周期；温度显示使用MSP430的A/D转换部分实现；当前显示时间日期以及闹钟都储存在flash中实现掉电前后结果一致；具有可独立设定开关的三个闹钟；计时器有开启、关闭、暂停和清零功能；测试结果基本符合预期，通过OLED屏幕显示内容引导及矩阵键盘实现良好的人机交互。
* 关键词：定时器、数字钟、定时器中断、flash、MSP430。
## 总体方案
* ![image](https://github.com/AcidFish12/Digital_Clock/assets/75008732/fb1be324-9fe3-43f3-9969-6fbac0d0b0b3)
* 简单介绍一下系统总体方案，也就是系统的组成，和每个组成之间的联系/通信，最好再配上一个框图。
main函数中主循环来实现界面的选择和功能函数的调用，其余部分在外部定义函数实现。定义部分全局变量实现多个功能函数的统一控制。
## 部分功能实现
* 时间日期的走时：获得1S周期的定时器中断，再中断函数内部实现24小时时钟循环。单独定义函数作为日期循环，将该函数嵌入小时循环实现时钟日期的正常更替。根据年月是计算出星期。将每次发生改变后的值存入flash。
* 实际按日期设置：两个循环组成。循环一进行时间日期的输入和检验并且实时显示，正确输入后进入循环二。循环二进行正确输入结果的显示和将输入结果存入flash，按下‘A’跳出循环返回主界面。
* ![image](https://github.com/AcidFish12/Digital_Clock/assets/75008732/1dfe6e5b-e8e3-4b83-88a8-dc7d4a297b76)
* 闹钟的设计和实现：闹钟的设置与实现与时间日期大致相同，只不过多了一位判断闹钟是否开启，要设置的时间从1个变为三个。定义cnt_H进行三个闹钟的选择。每个闹钟的位选每次按键按下后自动加一。闹钟的实现在主界面的循环中。将闹钟值与正在走时的值进行对比，若符合则LED灯开始闪烁。一分钟后闪烁停止，也可以在闹钟设置界面手动停止。
* ![image](https://github.com/AcidFish12/Digital_Clock/assets/75008732/517adf2a-6eeb-48ee-ad52-58e4c13b1ee5)
* 定时器的设计与实现：利用全局变量和控制量的配合实现定时器的功能。定义全局变量timer_D。IsStop使能中断函数中的全局变量不进行加一操作。按键‘C’将0赋给timer_D.显示部分显示当前的状态以及全局变量timer_D的值。
## 使用方法
* 时间日期循环：测试方法：设置不同的时间日期，同时按键按不同的按钮。选择错误的时间日期输入查看情况。并且对12/24小时制的切换进行查看。
测试结果：设置时间和日期的时候都存在略微的按键不灵敏情况。对于错误的输入有完善的异常处理功能。特别的，设置时间时，如果主界面是24小时制，进入时间设置界面也为24小时制，可以依次设置时分秒的每一位，设置错误可以移动光标到相应位置修改。如果主界面是12小时制，进入时间设置界面后为12小时制，通过单独的按键改变pm/am。同样，输入错误也会报错。
* 时间设置：测试方法：设置不同的闹钟，不同的闹钟开关状况。观察闹钟出发时候的LED灯闪烁情况，闹钟结束后的闪烁情况。在闹钟界面关闭闹钟，查看闹钟是否正常运行。断电重连后查看闹钟是否正常。
测试结果：设置按键略有不灵敏情况，尤其闹钟开关按钮延迟时间稍长。闹钟切换功能正常，每个闹钟独立设置功能正常，三个闹钟均正常工作。三个闹钟断电重连后均未发生改变。
* 闹钟设置：测试方法：设置不同的闹钟，不同的闹钟开关状况。观察闹钟出发时候的LED灯闪烁情况，闹钟结束后的闪烁情况。在闹钟界面关闭闹钟，查看闹钟是否正常运行。断电重连后查看闹钟是否正常。
测试结果：设置按键略有不灵敏情况，尤其闹钟开关按钮延迟时间稍长。闹钟切换功能正常，每个闹钟独立设置功能正常，三个闹钟均正常工作。三个闹钟断电重连后均未发生改变。
* 定时器设置：测试方法：比对定时的准确性，操作暂停、继续、清零并观察。
测试结果：定时器定时准确，操作按钮灵敏，暂停继续，清零功能均正常。
## 系统图片
* <img src = "![IMG_20231228_103535](https://github.com/AcidFish12/Digital_Clock/assets/75008732/848351e5-b6b4-4b0b-aa69-43041ba12864)" width = 30%/>![IMG_20231228_103535](https://github.com/AcidFish12/Digital_Clock/assets/75008732/848351e5-b6b4-4b0b-aa69-43041ba12864)"
* <img src = "![IMG_20231228_103553](https://github.com/AcidFish12/Digital_Clock/assets/75008732/5a63e216-2b48-4037-9c09-73efa54b883d)" width = 30%/>![IMG_20231228_103553](https://github.com/AcidFish12/Digital_Clock/assets/75008732/5a63e216-2b48-4037-9c09-73efa54b883d)
* <img src ="![IMG_20231228_103509](https://github.com/AcidFish12/Digital_Clock/assets/75008732/b5804f28-31b0-434c-9a1a-8badd2ecbff5)" width = 30%/>![IMG_20231228_103509](https://github.com/AcidFish12/Digital_Clock/assets/75008732/b5804f28-31b0-434c-9a1a-8badd2ecbff5)
* <img src ="![IMG_20231228_103433](https://github.com/AcidFish12/Digital_Clock/assets/75008732/506d808e-0896-4f43-bdd3-cad10bb020d5)" width = 30%/>![IMG_20231228_103433](https://github.com/AcidFish12/Digital_Clock/assets/75008732/506d808e-0896-4f43-bdd3-cad10bb020d5)









