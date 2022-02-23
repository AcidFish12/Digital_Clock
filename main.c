#include <msp430.h> 
#include "lib\key_button.h"
#include"lib\oled.h"
#define CALADC12_15V_30C *((unsigned int *)0x1A1A) // Temperature SensorCalibration-30 C                                                 //See device datsheetfor TLV table memory mapping
#define CALADC12_15V_85C *((unsigned int *)0x1A1C) // Temperature SensCalibration-85 C

unsigned int temp;
volatile float temperatureDegC;
volatile float temperatureDegF;
int temtemp = 0;
void SMCLK_XT2_4Mhz(void);//���4Mhzʱ��
void setup();//��ʼ��
int key_in();//��ü�ֵ
int leap_year(int year);
void time_default();//Ĭ��ʱ��ѭ��
void date_default();//Ĭ������ѭ��
void time_set();//����ʱ��
void date_set();//��������
void alarm_set();//��������
void timer();//��ʱ��
void time_show();//ʱ����ʾ
void date_show();//������ʾ
void Main_Interface_app();//��ҳ
void Menu_Interface_app();//�˵�ҳ
void write_flash_int(unsigned int addr,int *array,int len);//flashд�뺯��
void read_flash_int(unsigned int addr,int *array,int len);//flash��������
void get_temp();
void blink();//led��˸
/**
 * main.c
 */
int hour=0,minute=0,second=0,time_model=2,year=2022,month=1,day=1,week=6,isStop=-1,second_D=0,hour_D,minute_D;//ʱ�䣬���ڣ���ʱ��
int data[7]={0};//����ʱ������
int alarm[15]={0};//��������
unsigned char logo[]={0x00,0x00,0xE0,0xB0,0xD8,0xEC,0xFE,0xBE,0x1E,0xBE,0xFE,0xFC,0x78,0x00,0x00,0x00,
                          0x00,0x00,0xC0,0x21,0x11,0x38,0x4B,0x94,0xA4,0x54,0x2A,0x19,0x10,0xE0,0x00,0x00};//����ͷ��
unsigned char name[]={0x01,0xFD,0x8D,0x8D,0x01,0xFF,0x00,0x00,0x80,0xB1,0xB1,0xBF,0x80,0xFF,0x00,0x00,
                      0x01,0x7D,0x81,0x7D,0x01,0xFF,0x00,0x00,0x80,0xBE,0x81,0xBE,0x80,0xFF,0x00,0x00,
                      0x01,0xFD,0x01,0xFD,0x01,0xFF,0x00,0x00,0x80,0x83,0xFF,0x83,0x80,0xFF,0x00,0x00};//��߿�
unsigned char clk_logo[]={0x00,0x00,0xFF,0x01,0xFD,0x0D,0x0D,0x0D,0x00,0x00,0xFF,0x80,0xBF,0xB0,0xB0,0xB0,
                          0x00,0x00,0xFF,0x01,0xFD,0x01,0x01,0x01,0x00,0x00,0xFF,0x80,0xBF,0xB0,0xB0,0xB0,
                          0x00,0x00,0xFF,0x01,0xFD,0x61,0x19,0x05,0x00,0x00,0xFF,0x80,0xBF,0x83,0x8C,0xB0};//�ұ߿�
unsigned char menu[]={0xFF,0x01,0xFD,0x0D,0xFD,0x0D,0xFD,0x01,0xFF,0x80,0xBF,0x80,0xBF,0x80,0xBF,0x80,
                      0xFF,0x01,0xFD,0x8D,0x8D,0x8D,0x8D,0x01,0xFF,0x80,0xBF,0xB1,0xB1,0xB1,0xB1,0x80,
                      0xFF,0x01,0xFD,0x0D,0xFD,0x01,0xFD,0x01,0xFF,0x80,0xBF,0x80,0xBF,0xB0,0xBF,0x80,
                      0xFF,0x01,0xFD,0x01,0x01,0x01,0xFD,0x01,0xFF,0x80,0xBF,0xB0,0xB0,0xB0,0xBF,0x80};//�˵������ұ߿�
unsigned char du[]={0x00,0x0E,0x0A,0x0E,0x00,0xFE,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x00,
                  0x00,0x00,0x00,0x00,0x00,0x7F,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x00};//���϶ȵ�λ
unsigned char ss[]={0xDF,0xDF,0xDB,0xDB,0xDB,0xDB,0xDB,0xFB,0xF7,0xB6,0xB6,0xB6,0xB6,0xB6,0xB6,0xBE};//���ұ߿򲹳�
void setup()
{
   OLED_Init();//OLED��ʼ��
   OLED_Clear();
   init_key();//������̳�ʼ��
   P3DIR |=BIT2+BIT3+BIT4+BIT5+BIT6+BIT7;//����D8���
   P3REN|=BIT2+BIT3+BIT4+BIT5+BIT6+BIT7;//ʹ��
   P3OUT|=BIT2+BIT3+BIT4+BIT5+BIT6+BIT7;//��������Ĭ��Ϊ��
   read_flash_int(0x001880,alarm,15);//����flash������
   read_flash_int(0x001800,data,7);//����flash��ʱ������
   /*********����ֵ����������ʱ����**********/
   year=data[0];
   month=data[1];
   day=data[2];
   hour=data[3];
   minute=data[4];
   second=data[5];
   time_model=data[6];
   /***********************************/
}
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	setup();//��ʼ��
    __bis_SR_register(GIE);//ʹ��ȫ���ж�
	typedef struct
	{
	    u8 Interface_Mark;//����״̬
	} Mark;
	Mark Mark_Sign;//״̬��־λ
	//����һ��ö�٣���Ž������
	enum
	{
	    Main_Interface=0x10,//������
	    Menu_Interface=0x20,//�˵�����
	    Task_Interface=0X30,//�������
	    Time_Set_Task=0x40,//ʱ�����ý���
	    Date_Set_Task=0x50,//�������ý���
	    Alarm_Set_Task=0x60,//�������ý���
	    Timer_Task=0x70,//����ʱ����
	};
	Mark_Sign.Interface_Mark=Main_Interface;
	while(1)
	{
	    switch(Mark_Sign.Interface_Mark)
	    {
	    //״̬λ���������
	    case Main_Interface:
	       Main_Interface_app();break;
	    //״̬Ϊ��ǲ˵�����
	    case Menu_Interface:
	        Menu_Interface_app();break;
	    //״̬Ϊ���ʱ�����ý���
	    case Time_Set_Task:
	        time_set();break;
	    //״̬Ϊ�������ý���
	    case Date_Set_Task:
	        date_set();break;
	    //�������ý���
	    case Alarm_Set_Task:
	        alarm_set();break;
	    //����ʱ����
	    case Timer_Task:
	        timer();break;
	    default :break;
	    }
	    //���²˵�����
	    if(key_in()=='*')
	    {
	        OLED_Clear();
	        if(Mark_Sign.Interface_Mark==Main_Interface)//�жϵ�ǰ����Ϊ������
	        {
	            Mark_Sign.Interface_Mark=Menu_Interface;//����˵�����
	        }
	    }
	    //�˵�����ѡ�񲿷�
	    else if(Menu_Interface==Mark_Sign.Interface_Mark)
	    {
	            switch(key_in())
	            {
	            case 'A':OLED_Clear();Mark_Sign.Interface_Mark=Time_Set_Task;break;//����ʱ�����ý���
	            case 'B':OLED_Clear();Mark_Sign.Interface_Mark=Date_Set_Task;break;//�����������ý���
	            case 'C':OLED_Clear();Mark_Sign.Interface_Mark=Alarm_Set_Task;break;//�����������ý���
	            case 'D':OLED_Clear();Mark_Sign.Interface_Mark=Timer_Task;break;//���뵹��ʱ����
	            case '#':OLED_Clear();Mark_Sign.Interface_Mark=Main_Interface;break;//����������
	            default:break;
	            }
	    }
	    //����A���ذ���
	    else if(key_in()=='A')
	    {   OLED_Clear();
	        //�����·��ؼ�ʱ���жϵ�ǰ������
	        if(Menu_Interface==Mark_Sign.Interface_Mark)
	        {
	            //�˳��˵��������������
	            Mark_Sign.Interface_Mark=Main_Interface;
	        }
	        else if(Time_Set_Task==Mark_Sign.Interface_Mark|Date_Set_Task==Mark_Sign.Interface_Mark|Alarm_Set_Task==Mark_Sign.Interface_Mark|Timer_Task==Mark_Sign.Interface_Mark)
	        {   //�Ӹ����������ý��淵��������
	            OLED_Clear();
	            Mark_Sign.Interface_Mark=Main_Interface;
	        }
	    }
	}
}
void Main_Interface_app()
{
    time_default();//Ĭ��ʱ�Ӻ���
    while(1)
    {
        OLED_DrawBMP(96,0,112,2,logo);
        OLED_DrawBMP(0,0,8,6,name);
        OLED_DrawBMP(120,2,128,8,clk_logo);
        OLED_DrawBMP(96,4,112,6,du);
        OLED_DrawBMP(120,0,128,2,ss);
        OLED_DrawBMP(0,6,8,8,ss);
        OLED_ShowString(16,6,"*.menu");
        if(leap_year(year)==1)
        {
            OLED_ShowString(72,6,"leap");
        }else
        {

            OLED_ShowString(72,6,"comm");
        }
        get_temp();//����¶���ʾ
        time_show();//��ʾʱ��
        date_show();//��ʾ����
        if(key_in()=='B')
            {
                OLED_Clear();
                time_model=-time_model;
            }
       data[6]=time_model;
        //�ֱ��ж���������
        if(alarm[12]==1&&(10*alarm[0]+alarm[1])==hour&&(10*alarm[2]+alarm[3])==minute)
        {
            blink();
        }
        if(alarm[13]==1&&(10*alarm[4]+alarm[5])==hour&&(10*alarm[6]+alarm[7])==minute)
        {
            blink();
        }
        if(alarm[14]==1&&(10*alarm[8]+alarm[9])==hour&&(10*alarm[10]+alarm[11])==minute)
        {
            blink();
        }

        //����ѭ������˵�����
        if(key_in()=='*')
        {
            break;
        }
    }
}
void Menu_Interface_app()
{
    //�˵�������ʾ����
    OLED_ShowString(4,0,"A.TimeSet");
    OLED_ShowString(4,2,"B.DateSet");
    OLED_ShowString(4,4,"C.AlarmSet");
    OLED_ShowString(4,6,"D.Timer   #.<");
    OLED_DrawBMP(120,0,128,8,menu);
}
void time_default()
{
    SMCLK_XT2_4Mhz();//����SMCLKʹ��XT2�� Ƶ��Ϊ4MHz
	UCSCTL5|=DIVS__32;//ʹ��USCͳһʱ��ϵͳ����Ԥ��Ƶ����SMCLK����32��Ƶ(4_000_000/32=12_500)
	TA0CTL =ID_2+TASSEL_2+MC_1+TACLR;//����A0��������4��Ƶ(12_500/4=3_1250)��ʱ��ԴSMCLK������ģʽ��ͬʱ���������
	TA0EX0 |= TAIDEX_4;//A0��������Ƶ��5��Ƶ(3_1250/5=6250)
	TA0CCTL0 = OUTMOD_2 + CCIE;//����ȽϼĴ���0�����(OUTMOD_2)���ģʽΪ2��(CCIE)ͬʱʹ�ܶ�ʱ���жϣ�CCR0��Դ�жϣ�
	TA0CCR0 = 6250;//����ȽϼĴ���0����ֵΪ3125
}
#pragma vector = TIMER0_A0_VECTOR//TA0CCR0�жϷ�����T
__interrupt void TIMER0_A0_ISR()
{
    P3OUT^=BIT2;//��ת
        //24Сʱʱ��ѭ����λ
        if(second>=59)
            {
                second=0;
                if(minute>=59)
                    {
                        minute=0;
                        if(hour>=23)
                            {
                            hour=0;
                            date_default(year);
                            }
                        else hour+=1;
                    }
                else minute+=1;
            }
        else  second+=1;

    /*********������ʱ�������flash*********/
      data[0]=year;
      data[1]=month;
      data[2]=day;
      data[3]=hour;
      data[4]=minute;
      data[5]=second;
      write_flash_int(0x001800,data,7);
   /*********��������*********/
   if(month==1||month==2)
       week=( day+1 + 2*(month+12) +3*(month+13)/5 + year-1 + (year-1)/4 - (year-1)/100 +(year-1)/400)%7;
   else
   week=( day+1 + 2*month +3*(month+1)/5 + year + year/4 - year/100 +year/400)%7;
  /*********��ʱ������**********/
   if(isStop==1)
   {
      if(second_D>=59)
      {
          second_D=0;
          if(minute_D>59)
              {
                  minute_D=0;
                  hour_D++;
              }
          else minute_D++;
      }
      else second_D++;
   }
      else second_D=second_D;
}
void blink()
{  //��������ˮ����ʾ
    P3OUT^=BIT3;
    __delay_cycles(300000);
    P3OUT^=BIT5;
    __delay_cycles(300000);
    P3OUT^=BIT7;
    __delay_cycles(300000);
    P3OUT|=BIT3+BIT5+BIT7;
}
//����ʱ�亯��
void time_set()
{
   int cnt=0,value=0,hour_t=0,minute_t=0,second_t=0,iserror=0;//ͬ��value������OLEDʵʱ��ʾ���õ�ֵ������ÿ��ѭ������
   int time[6]={0};//ÿ��ѭ�����һλ��ֵ�������鴢������λ���յ�ֵ
   int ampm=1;
   while(1)
   {  iserror=0;
       while(1)
       {
           if(key_in()=='B'&&cnt>=0&&cnt<5)
           {
               cnt++;
               value=88;
               OLED_Clear();
               continue;
           }
           else if(key_in()=='C'&&cnt>0&&cnt<=5)
           {
               cnt--;
               value=88;
               OLED_Clear();
               continue;
           }
           else cnt=cnt;
           //��key_in()�������ص�ֵ����value��
           switch(key_in())
           {
               case 1:value=key_in();break;
               case 2:value=key_in();break;
               case 3:value=key_in();break;
               case 4:value=key_in();break;
               case 5:value=key_in();break;
               case 6:value=key_in();break;
               case 7:value=key_in();break;
               case 8:value=key_in();break;
               case 9:value=key_in();break;
            default: break;
           }
           if(key_in()=='D')
           {
               ampm=-ampm;
           }
           //����ÿһλ��ʱ��ֵ���������������Ӧ��λ
         if(value!=88)
         {  switch(cnt)
           {
               case 0:if(value>=0&&value<=2){time[0]=value;}break;
               case 1:if(value>=0&&value<=9){time[1]=value;}break;
               case 2:if(value>=0&&value<=5){time[2]=value;}break;
               case 3:if(value>=0&&value<=9){time[3]=value;}break;
               case 4:if(value>=0&&value<=5){time[4]=value;}break;
               case 5:if(value>=0&&value<9){time[5]=value;}break;
               default:break;
           }
         }
       //OLEDʵʱ��ʾ���������ֵ
           OLED_ShowChar(40,2,':');
           OLED_ShowNum(48,2,10*time[2]+time[3],2,16);
           OLED_ShowChar(64,2,':');
           OLED_ShowNum(72,2,10*time[4]+time[5],2,16);
           hour_t=10*time[0]+time[1];
           minute_t=10*time[2]+time[3];
           second_t=10*time[4]+time[5];
           if(time_model==2)
                {
                  OLED_ShowNum(24,2,10*time[0]+time[1],2,16);
                }
              else {
                      if(ampm==1)
                          {
                          OLED_ShowString(8,2,"am");
                          }
                      else
                          {
                          OLED_ShowString(8,2,"pm");
                          }
                      OLED_ShowNum(24,2,hour_t,2,16);

                  }

           switch(cnt)
           {
               case 0:OLED_ShowChar(24,4,'-');break;
               case 1:OLED_ShowChar(32,4,'-');break;
               case 2:OLED_ShowChar(48,4,'-');break;
               case 3:OLED_ShowChar(56,4,'-');break;
               case 4:OLED_ShowChar(72,4,'-');break;
               case 5:OLED_ShowChar(80,4,'-');break;
               default: OLED_ShowChar(24,4,'-');
           }
           //#���ʱ����������ѭ��
           if(key_in()=='#')
           {
               break;
           }
       }
       OLED_Clear();
       //�ж������Ƿ��д�
       if(hour_t>23||minute_t>=60||second_t>=60||(time_model!=2&&hour_t>12)||(time_model!=2&&hour_t<1))
       {
           while(1)
           {
               OLED_ShowString(40,0,"error");
               OLED_ShowString(16,2,"D.TO RESET");
               if(key_in()=='D')
               {
                   OLED_Clear();
                   iserror=1;
                   break;
               }
           }
       }
      if(iserror==0)
          break;
      else continue;
   }//�����ѭ������

   //��ʾ���úõ�ʱ��
   while(1)
     {
       if(time_model==2)
         {
           OLED_ShowNum(32,2,10*time[0]+time[1],2,16);
         }
       else {
               if(ampm==1)
                   {
                   OLED_ShowString(8,2,"am");
                   }
               else
                   {
                   OLED_ShowString(8,2,"pm");
                   }
               OLED_ShowNum(32,2,hour_t,2,16);

           }

        OLED_ShowChar(48,2,':');
        OLED_ShowNum(56,2,10*time[2]+time[3],2,16);
        OLED_ShowChar(72,2,':');
        OLED_ShowNum(80,2,10*time[4]+time[5],2,16);
        if(time_model==2)
        {
            hour=10*time[0]+time[1];
        }else if(ampm==1)
        {  if(hour_t==12)
            {
                hour=0;
            }
            else
            hour=hour_t;
        }
        else
        {
           if(hour_t>=1&&hour_t<=11)
             {
               hour=hour_t+12;
             }
           else hour=hour_t;
        }

        minute=10*time[2]+time[3];
        second=10*time[4]+time[5];
        data[3]=hour;
        data[4]=minute;
        data[5]=second;
        write_flash_int(0x001800,data,7);
        OLED_ShowString(0,4,"**Set finished**");
        OLED_ShowString(0,6,"*Click A return*");
        //����ѭ������������
        if(key_in()=='A')
          {
              break;
          }
     }
}
void date_set()
{
      int cnt=0,value=0,iserror=0,year_t=0,month_t=0,day_t=0;//ͬ��value������OLEDʵʱ��ʾ���õ�ֵ������ÿ��ѭ������
      int date[6]={0};//ÿ��ѭ�����һλ��ֵ�������鴢������λ���յ�ֵ
    while(1)
    { iserror=0;
      while(1)
      {
          //��cntʵ��6λ��ͬλ���л�
          if(key_in()=='B'&&cnt>=0&&cnt<5)
              {
                  cnt++;
                  value=88;
                  OLED_Clear();
              }
          else if(key_in()=='C'&&cnt>0&&cnt<=5)
              {
                  cnt--;
                  value=88;
                  OLED_Clear();
              }
          else cnt=cnt;
          //��key_in()�������ص�ֵ����value��
          switch(key_in())
          {
              case 1:value=key_in();break;
              case 2:value=key_in();break;
              case 3:value=key_in();break;
              case 4:value=key_in();break;
              case 5:value=key_in();break;
              case 6:value=key_in();break;
              case 7:value=key_in();break;
              case 8:value=key_in();break;
              case 9:value=key_in();break;
              default: break;
          }
          //����ÿһλ��ʱ��ֵ���������������Ӧ��λ
       if(value!=88)
       {
          switch(cnt)
          {
          case 0:if(value>=0&&value<=9){date[0]=value;}break;
          case 1:if(value>=0&&value<=9){date[1]=value;}break;
          case 2:if(value>=0&&value<=1){date[2]=value;}break;
          case 3:if(value>=0&&value<=9){date[3]=value;}break;
          case 4:if(value>=0&&value<=3){date[4]=value;}break;
          case 5:if(value>=0&&value<=9){date[5]=value;}break;
          default:break;
          }
       }
          //OLEDʵʱ��ʾ���������ֵ
          OLED_ShowNum(8,2,2000+10*date[0]+date[1],4,16);
          OLED_ShowChar(40,2,'.');
          OLED_ShowNum(48,2,10*date[2]+date[3],2,16);
          OLED_ShowChar(64,2,'.');
          OLED_ShowNum(72,2,10*date[4]+date[5],2,16);
          year_t=10*date[0]+date[1];
          month_t=10*date[2]+date[3];
          day_t=10*date[4]+date[5];
          switch(cnt)
          {
                 case 0:OLED_ShowChar(24,4,'-');break;
                 case 1:OLED_ShowChar(32,4,'-');break;
                 case 2:OLED_ShowChar(48,4,'-');break;
                 case 3:OLED_ShowChar(56,4,'-');break;
                 case 4:OLED_ShowChar(72,4,'-');break;
                 case 5:OLED_ShowChar(80,4,'-');break;
                 default: OLED_ShowChar(24,4,'-');
          }
          //#���ʱ����������ѭ��
          if(key_in()=='#')
          {
              break;
          }
      }
      OLED_Clear();
      //�ж������Ƿ��д�
            if((leap_year(year_t)==1&&month_t==2&&day_t>29&&day_t<1)||month_t>12||month_t<1||day_t<1||(month_t==2&&day_t>28&&leap_year(year_t)!=1)||
                    ((month_t==1||month_t==3||month_t==5||month_t==7||month_t==8||month_t==10||month_t==12)&&day_t>31)||
                    ((month_t==4||month_t==6||month_t==11||month_t==9)&&day_t>30))
            {
                while(1)
                {
                    OLED_ShowString(40,0,"error");
                    OLED_ShowString(16,2,"D.TO RESET");
                    if(key_in()=='D')
                    {
                        OLED_Clear();
                        iserror=1;
                        break;
                    }
                }
            }
           if(iserror==0)
               break;
           else continue;
    }//�����ѭ������

      //��ʾ���úõ�ʱ��
      while(1)
        {
           OLED_ShowNum(8,2,2000+10*date[0]+date[1],4,16);
           OLED_ShowChar(40,2,'.');
           OLED_ShowNum(48,2,10*date[2]+date[3],2,16);
           OLED_ShowChar(64,2,'.');
           OLED_ShowNum(72,2,10*date[4]+date[5],2,16);
           year=2000+10*date[0]+date[1];
           month=10*date[2]+date[3];
           day=10*date[4]+date[5];
           data[0]=year;
           data[1]=month;
           data[2]=day;

           write_flash_int(0x001800,data,7);
           OLED_ShowString(0,4,"**Set finished**");
           OLED_ShowString(0,6,"*Click A return*");
           //����ѭ������������
           if(key_in()=='A')
             {
                 break;
             }
        }
}
void alarm_set()
{
      unsigned int cnt_W=0,cnt_H=1;
      int value=0;
      while(1)
      {
          if(key_in()=='C'&&cnt_H<3)
              {
                  OLED_Clear();
                  value=88;
                  cnt_W=0;
                  cnt_H++;
                  continue;
              }
          else if(key_in()=='B'&&cnt_H>1&&cnt_H<=3)
              {
                  OLED_Clear();
                  value=88;
                  cnt_W=0;
                  cnt_H--;
                  continue;
              }
          else cnt_H=cnt_H;
          //��key_in()�������ص�ֵ����value��
            switch(key_in())
               {
                case 1:value=key_in();cnt_W++;OLED_Clear();break;
                case 2:value=key_in();cnt_W++;OLED_Clear();break;
                case 3:value=key_in();cnt_W++;OLED_Clear();break;
                case 4:value=key_in();cnt_W++;OLED_Clear();break;
                case 5:value=key_in();cnt_W++;OLED_Clear();break;
                case 6:value=key_in();cnt_W++;OLED_Clear();break;
                case 7:value=key_in();cnt_W++;OLED_Clear();break;
                case 8:value=key_in();cnt_W++;OLED_Clear();break;
                case 9:value=key_in();cnt_W++;OLED_Clear();break;
                default:break;
               }
          //����ÿһλ��ʱ��ֵ���������������Ӧ��λ

       if(value!=88||key_in()=='#')
       {
          switch(cnt_H)
          {
          case 1:{
                  if(value>=0&&value<=9)
                  {
                      alarm[cnt_W-1]=value;
                  }
                  else if(key_in()=='#')
                      {
                          OLED_Clear();
                          alarm[12]=-alarm[12];
                      }
                  }break;
          case 2:{
                      if(value>=0&&value<=9)
                      {
                          alarm[4+cnt_W-1]=value;
                      }
                      else if(key_in()=='#')
                      {
                          OLED_Clear();
                          alarm[13]=-alarm[13];
                      }
                  }break;
          case 3:{
                      if(value>=0&&value<=9)
                      {
                          alarm[8+cnt_W-1]=value;
                      }
                  else if(key_in()=='#')
                      {
                          OLED_Clear();
                          alarm[14]=-alarm[14];
                      }
                 }break;
          default:break;
          }

       }
          //OLEDʵʱ��ʾ���������ֵ
          OLED_ShowNum(24,0,10*alarm[0]+alarm[1],2,16);
          OLED_ShowChar(40,0,':');
          OLED_ShowNum(48,0,10*alarm[2]+alarm[3],2,16);
          if(alarm[12]==1)OLED_ShowString(72,0,"ON");else OLED_ShowString(72,0,"OFF");
          OLED_ShowNum(24,2,10*alarm[4]+alarm[5],2,16);
          OLED_ShowChar(40,2,':');
          OLED_ShowNum(48,2,10*alarm[6]+alarm[7],2,16);
          if(alarm[13]==1)OLED_ShowString(72,2,"ON");else OLED_ShowString(72,2,"OFF");
          OLED_ShowNum(24,4,10*alarm[8]+alarm[9],2,16);
          OLED_ShowChar(40,4,':');
          OLED_ShowNum(48,4,10*alarm[10]+alarm[11],2,16);
          if(alarm[14]==1)OLED_ShowString(72,4,"ON");else OLED_ShowString(72,4,"OFF");
          switch(cnt_W)
          {
          case 0:OLED_ShowChar(24,6,'-');break;
          case 1:OLED_ShowChar(32,6,'-');break;
          case 2:OLED_ShowChar(48,6,'-');break;
          case 3:OLED_ShowChar(56,6,'-');break;
          default: OLED_ShowChar(24,6,'-');
          }
          switch(cnt_H)
          {
          case 1:OLED_ShowChar(4,0,'*');break;
          case 2:OLED_ShowChar(4,2,'*');break;
          case 3:OLED_ShowChar(4,4,'*');break;
          default: OLED_ShowChar(4,0,'*');
          }

          //#���������������ѭ��
          if(key_in()=='A')
          {
              break;
          }

      }
      //������д��flash
      write_flash_int(0x001880,alarm,15);
}
//��ʱ������
void timer()
{ while(1)
    {
        OLED_ShowNum(24,2,hour_D,2,16);
        OLED_ShowChar(40,2,':');
        OLED_ShowNum(48,2,minute_D,2,16);
        OLED_ShowChar(64,2,':');
        OLED_ShowNum(72,2,second_D,2,16);
        OLED_ShowChar(112,2,'S');
        OLED_ShowString(8,4,"B.Run/Stop");
        OLED_ShowString(8,6,"C.Clear   A.<");
       //��ʾ��ͣ����
        if(isStop==1)
           {
               OLED_ShowString(0,0,"Run");
           }
       else OLED_ShowString(0,0,"Stop");
        //ʵ����ͣ����
        if(key_in()=='B')
        {   OLED_Clear();
            __delay_cycles(10000);//����
            isStop=-isStop;
        }
       //ʵ�ֹ���
        if(key_in()=='C')
        {
            hour_D=0;
            second_D=0;
            minute_D=0;
        }
        //����ѭ��������ҳ
        if(key_in()=='A')
        {
            break;
        }
    }
}
void time_show()
{
    if(time_model==2)
    {
            OLED_ShowNum(24,0,hour,2,16);

    }
    else
    {  if(hour>=0&&hour<12)
            {
                OLED_ShowString(8,0,"am");
                if(hour==0)
                    OLED_ShowNum(24,0,12,2,16);
                else
                OLED_ShowNum(24,0,hour,2,16);
            }
        else
            {
                OLED_ShowString(8,0,"pm");
                if(hour==12)
                    {
                        OLED_ShowNum(24,0,hour,2,16);
                    }
                else
                    {
                    OLED_ShowNum(24,0,hour-12,2,16);
                    }
            }
    }

            OLED_ShowChar(40,0,':');
            OLED_ShowNum(48,0,minute,2,16);
            OLED_ShowChar(64,0,':');
            OLED_ShowNum(72,0,second,2,16);

}
void date_show()
{
    unsigned char*week_t="null";
    switch(week)
        {
        case 0:week_t="Sun";break;
        case 1:week_t="Mon";break;
        case 2:week_t="Tue";break;
        case 3:week_t="Wed";break;
        case 4:week_t="Thu";break;
        case 5:week_t="Fri";break;
        case 6:week_t="Sat";break;
        default:week_t="error";
        }
    OLED_ShowNum(8,2,year,4,16);
    OLED_ShowChar(40,2,'.');
    OLED_ShowNum(48,2,month,2,16);
    OLED_ShowChar(64,2,'.');
    OLED_ShowNum(72,2,day,2,16);
    OLED_ShowString(96,2,week_t);
}
//���귵��1ƽ���귵��0
int leap_year(int year)
{
  int isLeapYear=0;
  if((year%4==0&&year%100!=0)||year%100==0)
      {
          isLeapYear=1;
      }
  return isLeapYear;
}
//���ѭ��
void date_default()
{   if(month>=12)
    {
    month=1;
    year++;
    }
else if(month==1||month==3||month==5||month==7||month==8||month==10||month==12)
        {
        if(day>=31)
            {
            day=1;
            month++;
            }
        else day++;
        }
    else if(month==4||month==6||month==9||month==11)
        {
        if(day>=30)
           {
            month++;
            day=1;
           }
        else day++;
        }
    else if(month==2)
        {
            if(leap_year(year)==1)
                {if(day>=29)
                { day=1;
                month++;}
                else day++;}
            else
                {if(day>=28)
                { day=1;
                month++;}
                else day++;}
        }
    else month++;
}
//flashд�뺯��
void write_flash_int(unsigned int addr,int *array,int count)
{
    unsigned int *Flash_ptr;
    unsigned int i=0;
    Flash_ptr=(unsigned int*)addr;
    FCTL1=FWKEY+ERASE;//����
    FCTL3=FWKEY;
    *Flash_ptr=0;
    FCTL1=FWKEY+WRT;//д��
    for(i=0;i<count;i++)
    {
        *Flash_ptr=array[i];
        Flash_ptr++;
    }
    FCTL1=FWKEY;
    FCTL3=FWKEY+LOCK;
}
//����flash����
void read_flash_int(unsigned int addr,int *array,int count)
{
    unsigned int i=0;
    int *address =(int*)addr;
    for(i=0;i<count;i++)
    {
        array[i]=*address;
        address++;
    }

}
void get_temp()
{
               REFCTL0 &= ~REFMSTR; // Reset REFMSTR to hand overcontrol to
                               // ADC12_A ref control registers
               ADC12CTL0 = ADC12SHT0_8 + ADC12REFON + ADC12ON;
                                   // ���òο���ѹΪ1.5V����AD
               ADC12CTL1 = ADC12SHP; // ����������������Ϊ�ڲ���ʱ��
               ADC12MCTL0 = ADC12SREF_1 + ADC12INCH_10; // AD��A10ͨ���������¶ȴ��������
               ADC12IE = 0x001; // ADC_IFG upon conv resultADCMEMO
               __delay_cycles(100); // Allow ~100us (at default UCSsettings)
                               // for REF to settle
               ADC12CTL0 |= ADC12ENC; // ʹ��AD
               ADC12CTL0 &= ~ADC12SC;
               ADC12CTL0 |= ADC12SC; // ��ʼ����
               __bis_SR_register(LPM4_bits + GIE); // LPM0 with interrupts enabled
               __no_operation();
               // Temperature in Celsius. See the Device Descriptor Table section inthe
               // System Resets, Interrupts, and Operating Modes, System Control Module
               // chapter in the device user's guide for background information on the
               // used formula.
               temperatureDegC = (float)(((long)temp - CALADC12_15V_30C) * (85 - 30)) /
                       (CALADC12_15V_85C - CALADC12_15V_30C) + 30.0f; //���϶Ȼ���
               temtemp = temperatureDegC*100;
               OLED_ShowString(8,4,"Temp=");
               OLED_ShowString(64,4,".");
               OLED_ShowNum(48,4,temtemp/100,2,16);
               OLED_ShowNum(72,4,temtemp%100,2,16); //�¶���ʾ
               // Temperature in Fahrenheit Tf = (9/5)*Tc + 32
               temperatureDegF = temperatureDegC * 9.0f / 5.0f + 32.0f;//���϶Ȼ���
               _delay_cycles(100000);
               __no_operation(); // SET BREAKPOINT HERE

}
#pragma vector=ADC12_VECTOR
__interrupt void ADC12ISR (void)
{
    switch(__even_in_range(ADC12IV,34))
    {
    case 0: break; // Vector 0: No interrupt
    case 2: break; // Vector 2: ADC overflow
    case 4: break; // Vector 4: ADC timingoverflow
    case 6: // Vector 6: ADC12IFG0
        temp = ADC12MEM0; // ��ȡ������жϱ�־�ѱ����
        __bic_SR_register_on_exit(LPM4_bits); // Exit active CPU
    case 8: break; // Vector 8: ADC12IFG1
    case 10: break; // Vector 10: ADC12IFG2
    case 12: break; // Vector 12: ADC12IFG3
    case 14: break; // Vector 14: ADC12IFG4
    case 16: break; // Vector 16: ADC12IFG5
    case 18: break; // Vector 18: ADC12IFG6
    case 20: break; // Vector 20: ADC12IFG7
    case 22: break; // Vector 22: ADC12IFG8
    case 24: break; // Vector 24: ADC12IFG9
    case 26: break; // Vector 26: ADC12IFG10
    case 28: break; // Vector 28: ADC12IFG11
    case 30: break; // Vector 30: ADC12IFG12
    case 32: break; // Vector 32: ADC12IFG13
    case 34: break; // Vector 34: ADC12IFG14
    default: break;
    }
}
int key_in()
{
    int key_value=0;
    int key_value_t;
        key_value=key();
        if(key_value!=0)
        {
            switch(key_value)
            {
                case 1: key_value_t=1;break;
                case 2: key_value_t=2;break;
                case 3: key_value_t=3;break;
                case 4: key_value_t='A';break;
                case 5: key_value_t=4;break;
                case 6: key_value_t=5;break;
                case 7: key_value_t=6;break;
                case 8: key_value_t='B';break;
                case 9: key_value_t=7;break;
                case 10: key_value_t=8;break;
                case 11: key_value_t=9;break;
                case 12: key_value_t='C';break;
                case 13: key_value_t='*';break;
                case 14: key_value_t=0;break;
                case 15: key_value_t='#';break;
                case 16: key_value_t='D';break;
                default: key_value_t='x';
            }
        }

    return key_value_t;
}
//���4Mhz��ʱ��
void SMCLK_XT2_4Mhz(void)
{
    P7SEL |= BIT2+BIT3; // Port select XT2
    UCSCTL6 &= ~XT2OFF; // Enable XT2
    UCSCTL6 &= ~XT2OFF + XT2DRIVE_1; // Enable XT2
    UCSCTL3 |= SELREF_2; // FLLref = REFO
                        // Since LFXT1 is not used,
                        // sourcing FLL with LFXT1 cancause
                        // XT1OFFG flag to set
     UCSCTL4 |= SELA_2; //ACLK=REFO,SMCLK=DCO,MCLK=DCO
                         // Loop until XT1,XT2 & DCO stabilizes - in this case loop until XT2settles
     do
     {
         UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + XT1HFOFFG + DCOFFG);
                             // Clear XT2,XT1,DCO faultflags
         SFRIFG1 &= ~OFIFG; // Clear fault flags
     }while (SFRIFG1&OFIFG); // Test oscillator fault flag
     UCSCTL6 &= ~XT2DRIVE0; // Decrease XT2 Driveaccording to
     UCSCTL4 |= SELS_5 + SELM_5; // SMCLK=MCLK=XT2
}

