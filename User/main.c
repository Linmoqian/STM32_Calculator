#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "button4_4.h"
#include "string.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>                      
#include "key.h"
#include "led.h"
#include "Servo.h"
#include "Serial.h"


void modeset(void);
void led_mode(void);
void flash(void);

char input[17] = "";   
char a;                
float num1 = 0.000;    
float num2 = 0.000;    
float result = 0.000;  
char sign;             
int negative = 0;      
int is_number(char c);
float compute(char sign);
int i=1;
float book_num[10][10];
float book_end[10];
char book_sign[10];
int page=1;
uint8_t mode = 1; 
int page1=1;

char key_map1[16] = { 
    '7', '8', '9', '/', 
    '4', '5', '6', '*', 
    '1', '2', '3', '-', 
    'C', '0', '=', '+',  
};
char key_map2[16] = { 
    '7', '8', '9', 'y', // y的x次方
    '4', '5', '6', 't', // tan
    '1', '2', '3', 'c', // cos
    'C', '0', '=', 's', // sin
};
char key_map3[16] = { 
    '7', '8', '9', 'e', // e^x
    '4', '5', '6', 'n', // ln
    '1', '2', '3', 'g', // lg
    'C', '0', '=', 'q', // 平方根
};

 

int main (void) {
	Key_Init();
	OLED_Init();
	Button4_4_Init();
	LED_Init();
	flash();
	Serial_Init();
	
	OLED_ShowString(1, 1, "Hello, OLED!");
    Delay_ms(1000);
	OLED_ShowString(1, 1, "                ");
	
	while (1) {
		
		int key = Button4_4_Scan();		// 扫描按键
		modeset();
		led_mode();
		Delay_ms(50);
		if (mode==5){
			char str1[20]; 
			memset(str1, 0, sizeof(str1));
			sprintf(str1, "%.3f", book_num[page-1][0]);
			OLED_ShowString(1,1,str1);
			
			char str2[20];
			memset(str2, 0, sizeof(str2));
			sprintf(str2, "%.3f", book_num[page-1][1]);
			OLED_ShowString(3,1,str2);
			
			OLED_ShowChar(2,1,book_sign[page-1]);
			char str3[20]; 
			memset(str3, 0, sizeof(str3));
			sprintf(str3, "%.3f", book_end[page-1]);
			OLED_ShowString(4,1,str3);
			
		}
			
		else {
        if (key != 0) { 
            if (mode == 1) {
                a = key_map1[key - 1];
            } else if (mode == 2) {
                a = key_map2[key - 1];
            } else if (mode == 3) {
                a = key_map3[key - 1];
            }

            // 处理按键输入
            if (a == 'C')  // 清除操作
            {
                input[0] = '\0'; 
                OLED_ShowString(1, 1, "                ");
				OLED_ShowString(2, 1, "                ");
				OLED_ShowString(3, 1, "                ");
				OLED_ShowString(4, 1, "                ");
				num1 = 0.000;
				num2 = 0.000;
				result = 0.000;
                negative = 0;  // 重置负数标志
            }
            else if (a == '=')  // 计算操作
            {
				
                num2 = atof(input);
				result = compute(sign);  // 执行计算
				book_end[i-1]=result;
				book_num[i-1][1]=num2;
				if (i<10){i+=1;}
				else{i=1;}
                
                sprintf(input, "%.3f", result);  
                OLED_ShowString(1, 1, input);   
                input[0] = '\0';  
                negative = 0;  // 重置负数标志
				Serial_SendNumber(result, sizeof(result));
            }
            else if (a == '+' || a == '*' || a == '/' || a == 'y' || a == 't' || a == 'c' || a == 's' || a == 'e' || a == 'n' || a == 'g' || a == 'q')  // 检测到运算符
            {
                num1 = atof(input); 
				book_num[i-1][0]=num1;
                sign = a;
				book_sign[i-1]=a;
                input[0] = '\0';  
                OLED_ShowString(1, 1, "                ");
				OLED_ShowString(1, 1, &a);  
            }
            else if (a == '-')  
            {
                if (input[0] == '\0') {  // 如果当前没有输入内容，认为是负号
                    negative = 1;
                    strcat(input, "-");
                    OLED_ShowString(1, 1, input);  
                } else {  
                    num1 = atof(input);
					book_num[i-1][0]=num1;
                    sign = a;  
                    input[0] = '\0';  
                    OLED_ShowString(1, 1, "                "); 
				    OLED_ShowString(1, 1, &a);  
                }
            }
            else if (is_number(a))  
            {
                char a_str[2] = {a, '\0'};  
                strcat(input, a_str);  
                OLED_ShowString(1, 1, input);  
            }
        }
    }}
}

// 模式切换
void modeset(void) {
	uint8_t keynum = Key_GetNum();  
	if (keynum == 1) {  
		if (mode < 3) {
			LED1_Turn();
			mode++;
		} else {
			mode = 1;  
		}
	} else if (keynum == 2) {
		OLED_ShowString(1, 1, "                ");
		OLED_ShowString(2, 1, "                ");
		OLED_ShowString(3, 1, "                ");
		OLED_ShowString(4, 1, "                ");
		mode = 5;  // 模式5是查看历史
	}
	else if (mode == 5 && keynum == 3){if (page<10)
		{page+=1;OLED_ShowString(1, 1, "                ");
				OLED_ShowString(2, 1, "                ");
				OLED_ShowString(3, 1, "                ");
				OLED_ShowString(4, 1, "                ");}
	else{page=1;OLED_ShowString(1, 1, "                ");
				OLED_ShowString(2, 1, "                ");
				OLED_ShowString(3, 1, "                ");
				OLED_ShowString(4, 1, "                ");}
	}
	else if (mode == 5 && keynum == 4)
	{if (page>1)
	{page-=1;OLED_ShowString(1, 1, "                ");
				OLED_ShowString(2, 1, "                ");
				OLED_ShowString(3, 1, "                ");
				OLED_ShowString(4, 1, "                ");}
	else{page=10;OLED_ShowString(1, 1, "                ");
				OLED_ShowString(2, 1, "                ");
				OLED_ShowString(3, 1, "                ");
				OLED_ShowString(4, 1, "                ");}}}
	

int is_number(char c) {
    return (c >= '0' && c <= '9');
}

// 计算
float compute(char sign) {
    if (sign == '+')
        return num1 + num2;
    else if (sign == '-')
        return num1 - num2;
    else if (sign == '*')
        return num1 * num2;
    else if (sign == '/') {
        if (num2 != 0)
            return num1 / num2;
        else {
            OLED_ShowString(1, 1, "Div by 0!"); 
            return 0.0;
        }
    }
	else if (sign == 'y')
        return pow(num1, num2);
	else if (sign == 't')
        return tan(num1);
	else if (sign == 'c')
        return cos(num1);
	else if (sign == 's')
        return sin(num1);
	else if (sign == 'e')
        return exp(num1);
	else if (sign == 'n')
        return log(num1);
	else if (sign == 'g')
        return log10(num1);
	else if (sign == 'q')
        return sqrt(num1);
    else {
        OLED_ShowString(1, 1, "Invalid Op!");  
        return 0.0;
    }

}

void led_mode(){
if (mode==1){LED1_ON();LED2_OFF();LED3_OFF();}
else if (mode==2){LED1_OFF();LED2_ON();LED3_OFF();}
else if (mode==3){LED1_OFF();LED2_OFF();LED3_ON();}
else if (mode==5){LED1_ON();LED2_ON();LED3_ON();}

}

void flash(){LED1_Turn();Delay_ms(200);LED1_Turn();LED2_Turn();Delay_ms(200);LED1_Turn();LED2_Turn();LED3_Turn();Delay_ms(200);}