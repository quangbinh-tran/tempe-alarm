#include <16f887.h>
#device ADC=10
#bit tmr1if=0x0C.0
#bit tmr0if=0x0B.2
#fuses nowdt,put,hs,noprotect,nolvp
#use delay(clock=20M)

#define LCD_ENABLE_PIN  PIN_D2                            
#define LCD_RS_PIN      PIN_D0                               
#define LCD_RW_PIN      PIN_D1                                   
#define LCD_DATA4       PIN_D4                        
#define LCD_DATA5       PIN_D5                            
#define LCD_DATA6       PIN_D6                     
#define LCD_DATA7       PIN_D7
#include <lcd.c>

#define mdf  PIN_B0
#define up   PIN_B1
#define dw   PIN_B2
#define led1 PIN_B4
#define led2 PIN_B5
#define led3 PIN_E1
#define led4 PIN_E2
#define bzr  PIN_E0

CONST UNSIGNED CHAR MA7DOAN[10]={0XC0,0XF9,0XA4,0XB0,0X99,0X92,0X82,0XF8,0X80,0X90};

UNSIGNED CHAR doanF[8]={0x07,0x0F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F}; //DOAN F - 0
UNSIGNED CHAR doanA[8]={0x1F,0x1F,0x1F,0X00,0X00,0X00,0X00,0X00};  //DOAN A - 1
UNSIGNED CHAR doanB[8]={0x1C,0x1E,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F};//DOAN B - 2
UNSIGNED CHAR doanD[8]={0x00,0x00,0x00,0x00,0x00,0x1F,0x1F,0x1F}; //DOAN D - 3
UNSIGNED CHAR doanC[8]={0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1E,0x1C};  //DOAN C - 4
UNSIGNED CHAR doanE[8]={0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x0F,0x07};  //DOAN E - 5
UNSIGNED CHAR doanG[8]={0x1F,0x1F,0x1F,0x00,0x00,0x00,0x1F,0x1F};  //DOAN G+D-6 

CONST UNSIGNED CHAR LCD_SO_X[10][6] ={
                  0,1,2,5,3,4,               // SO 0
                  1,2,32,3,255,3,              // SO 1
                  6,6,2,5,3,3,               // SO 2
                  6,6,2,3,3,4,               // SO 3
                  255,3,255,32,32,255,             // SO 4
                  255,6,6,3,3,4,               // SO 5
                  0,6,6,5,3,4,               // SO 6
                  1,1,255,32,32,255,             // SO 7
                  0,6,2,5,3,4,               // SO 8
                  0,6,2,3,3,4};             // SO 9
                  
unsigned int8 temp_set=35, tempF, i, old_temp=120;
unsigned int8 count_tm1=0, count_tm0=0;
unsigned int16 temp;
int1 temp_mdf=0;

void display_7seg(unsigned int8 seg1, unsigned int8 seg2, unsigned int8 seg3, unsigned int8 seg4, unsigned int8 time)
{
   for (i=0; i<time; i++)
   {
      output_c(seg1);
      output_low(led1);
      output_high(led2);
      output_high(led3);
      output_high(led4);
      delay_ms(1);
      output_c(0xff);
      output_high(led1);
      output_c(seg2);
      output_low(led2);
      output_high(led1);
      output_high(led3);
      output_high(led4);                  
      delay_ms(1);
      output_c(0xff);
      output_high(led2);
      output_c(seg3);
      output_low(led3);
      output_high(led1);               
      output_high(led2);
      output_high(led4);
      delay_ms(1);
      output_c(0xff);
      output_high(led3);
      output_c(seg4);
      output_low(led4);
      output_high(led2);
      output_high(led3);
      output_high(led1);
      delay_ms(1);
      output_c(0xff);
      output_high(led4);
   }
}

void off_7seg()
{
   output_high(led1);
   output_high(led2);
   output_high(led3);
   output_high(led4);
}

#INT_RB
void button_processing() 
{
   if (!input(mdf)) 
   {
      delay_ms(10);
      if (!input(mdf)) 
      {
         temp_mdf=~temp_mdf;
         count_tm1=0;
         setup_timer_1(T1_INTERNAL|T1_DIV_BY_8);
         set_timer1(3036);
         old_temp=120;
      }
      while(!input(mdf))
      {
         display_7seg(MA7DOAN[temp/10],MA7DOAN[temp%10],0x9c,0xc6,1);
      }
   }
   if ((!input(up))&&(temp_mdf==0)&&(temp_set<99)) 
   {
      delay_ms(10);
      if ((!input(up))&&(temp_mdf==0)&&(temp_set<99)) 
      {
         temp_set++;
         count_tm1=0;
         set_timer1(3036);
         count_tm0=0;
         lcd_gotoxy(12,1);
         printf(lcd_putc,"%02U",temp_set);
      }
      while(!input(up))
      {
         display_7seg(MA7DOAN[temp/10],MA7DOAN[temp%10],0x9c,0xc6,1);
      }
   }
   
   if ((!input(dw))&&(temp_mdf==0)&&(temp_set>0))
   {
      delay_ms(10);
      if ((!input(dw))&&(temp_mdf==0)&&(temp_set>0)) 
      {
         temp_set--;
         count_tm1=0;
         set_timer1(3036);
         count_tm1=0;
         lcd_gotoxy(12,1);
         printf(lcd_putc,"%02U",temp_set);
      }
      while(!input(dw))
      {
         display_7seg(MA7DOAN[temp/10],MA7DOAN[temp%10],0x9c,0xc6,1);
      }
   }
   //clear_interrupt(INT_RB);
}

void cgram_setup()
{
   lcd_set_cgram_char(0,doanF);
   lcd_set_cgram_char(1,doanA);
   lcd_set_cgram_char(2,doanB);
   lcd_set_cgram_char(3,doanD);
   lcd_set_cgram_char(4,doanC);
   lcd_set_cgram_char(5,doanE);
   lcd_set_cgram_char(6,doanG);
}

void lcd_clr(unsigned int8 posX, unsigned int8 posY, unsigned int8 time)
{
   lcd_gotoxy(posX, posY);
   for (int8 i=0; i<time; i++) lcd_putc(32);
}

void lcd_numberX_disp(signed int8 lcd_so, x1, y1)
{
   lcd_gotoxy(x1,y1);
   for (i=0;i<6;i++)
   {
      if (i<3) lcd_gotoxy(x1+i,y1);
      else lcd_gotoxy((x1+i)-3,y1+1);
      lcd_putc(lcd_so_x[lcd_so][i]);
      delay_ms(1);
   }
}

void lcd_degC_disp(unsigned int8 pos)
{
   lcd_gotoxy(pos,1);
   lcd_putc(0);
   lcd_putc(6);
   lcd_putc(2);
   lcd_clr(pos,3,3);
   lcd_clr(pos+3,1,1);
   lcd_clr(pos+3,2,1);
   lcd_gotoxy(pos+4,1);
   lcd_putc(0);
   lcd_putc(1);
   lcd_putc(1);
   lcd_gotoxy(pos+4,2);
   lcd_putc(5);
   lcd_putc(3);
   lcd_putc(3);
   lcd_clr(pos+4+3,1,1);
   lcd_clr(pos+4+3,2,1);
}

void lcd_degF_disp(unsigned int8 pos)
{
   lcd_gotoxy(pos,1);
   lcd_putc(0);
   lcd_putc(6);
   lcd_putc(2);
   lcd_clr(pos,2,3);
   lcd_gotoxy(pos+3,1);
   lcd_putc(255);
   lcd_putc(6);
   lcd_putc(6);
   lcd_gotoxy(pos+3,2);
   lcd_putc(255);
   lcd_putc(32);
   lcd_putc(32);
}
   

void main() 
{
   set_tris_a(0xff);
   set_tris_c(0x00);
   set_tris_b(0xf0);
   set_tris_e(0x00);
   set_tris_d(0x00);
   //port_b_pullups(0xff);
   setup_adc(adc_clock_div_32);
   setup_adc_ports(san1|vss_vdd);
   set_adc_channel(1);
   lcd_init();
   cgram_setup();
   output_c(0xff);
   output_high(bzr);
   enable_interrupts(global);
   enable_interrupts(INT_RB);
   setup_timer_0(T0_INTERNAL|T0_DIV_256);
   set_timer0(61);
   while (true) 
   { 
      temp=0;
      for (i=0; i<50; i++) 
         temp+=read_adc();
      temp/=2.177;
      temp/=50;
      tempF=1.8*temp+32;
         
      if (temp_mdf==0) 
      {
         output_high(bzr);
         lcd_gotoxy(1,1);
         lcd_putc("HIGH TEMP:    C");
         lcd_gotoxy(12,1);
         count_tm0++;
         tmr0if=0;
         set_timer0(200);
         if (count_tm0<=5) printf(lcd_putc,"%02U",temp_set);
         else if ((count_tm0>5)&&(count_tm0<=9)) lcd_clr(12,1,2);
         else if (count_tm0>9) count_tm0=0;
         lcd_gotoxy(14,1);
         lcd_putc(0xdf);
         lcd_gotoxy(1,2);
         printf(lcd_putc,"    **%02LU C**    ", temp);
         lcd_gotoxy(9,2);
         lcd_putc(0xdf);
         display_7seg(MA7DOAN[temp/10],MA7DOAN[temp%10],0x9c,0xc6,30);
         //timer1_on
         if (tmr1if==1) 
         {
            count_tm1++;
            tmr1if=0;
            set_timer1(3036);
            if (count_tm1==100) 
            {
               temp_mdf=1;
               count_tm1=0;
            }
         }
      }
      
      else
      {
         //timer1_off
         count_tm1=0;
         setup_timer_1(T1_INTERNAL|T1_DISABLED);
         display_7seg(MA7DOAN[temp/10],MA7DOAN[temp%10],0x9c,0xc6,30);
         if (temp<=temp_set)
         {
            output_high(bzr);
            //timer0_off
            set_timer0(61);
            count_tm0=0;
            display_7seg(MA7DOAN[temp/10],MA7DOAN[temp%10],0x9c,0xc6,30);
            if (old_temp!=temp)
            {
               lcd_numberX_disp(temp/10,1,1);
               lcd_clr(1+3,1,1);
               lcd_clr(1+3,2,1);
               lcd_numberX_disp(temp%10,5,1);
               lcd_clr(5+3,1,1);
               lcd_clr(5+3,2,1);
               lcd_degC_disp(9);
               old_temp=temp;
            }
         }

         else 
         {
            output_low(bzr);
            lcd_clr(1,1,4);
            lcd_clr(13,1,4);
            lcd_gotoxy(5,1);
            lcd_putc("WARNING!");
            lcd_clr(1,2,2);
            lcd_clr(14,2,3);
            lcd_gotoxy(3,2);
            lcd_putc("OVERHEATING");
            old_temp=120;
            if (tmr0if==1)
            {
               count_tm0++;
               tmr0if=0;
               set_timer0(61);
               if (count_tm0<=10) display_7seg(MA7DOAN[temp/10],MA7DOAN[temp%10],0x9c,0xc6,1);
               else if ((count_tm0>10)&&(count_tm0<=30)) output_c(0xff);
               else if (count_tm0>30) count_tm0=0;
            }
         }
      }
   }
}
