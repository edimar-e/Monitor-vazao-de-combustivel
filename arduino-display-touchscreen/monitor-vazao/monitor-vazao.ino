#include<TimerOne.h>
#include <UTFT.h>
#include <Adafruit_TFTLCD.h>
#include <TouchScreen.h>
#include "certo.c";
#include "marcha.c";
#include "automovel.c";
#include "gasolina.c";
#include "bateriaFraca.c";
#include "bateriaNormal.c";

#include "bateriaCarregando.c";
extern uint8_t arial_normal[];

#define YP A1
#define XM A2
#define YM 7
#define XP 6
#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940
#define MINPRESSURE 4
#define MAXPRESSURE 10000
extern uint8_t SevenSeg_XXXL_Num[];
Adafruit_TFTLCD tft(A3, A2, A1, A0, A4);
UTFT myGLCD(UNO_24, A2, A1, A3, A4);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
uint32_t tempo_injecao, rpm, pulsos, t0;
unsigned long milisegundos;
bool flag_rpm = false;




void setup() {
  //Serial.begin(9600);
  myGLCD.InitLCD();
  myGLCD.fillScr(VGA_BLACK);
  myGLCD.setFont(arial_normal);
  pinMode(21, INPUT);
  pinMode(46, INPUT);

  myGLCD.drawBitmap(110,60,100,76,automovel);
  delay(100);
   for(int x=0;x<300;x=x+10){
    myGLCD.print("=", x, 200);delay(150);
   }
  
  tela_inicial_1();
}

void loop() {

}



/////////////////////////////////////////////////////////////////SINAIS //////////////////////////////////////////////////////////////////////////
void tela_sinais(){
       myGLCD.fillScr(VGA_BLACK); myGLCD.setBackColor(0);
       
       myGLCD.setColor(VGA_SILVER);
       //myGLCD.fillRect(0,120,319,239);
       myGLCD.print("Sonda Lambda", CENTER, 0);
       myGLCD.print("(mV)", 215, 30);
       myGLCD.print("(us)", 215, 165);

       myGLCD.print("Tempo de injecao", CENTER, 120);
       myGLCD.setBackColor(0);

        float Lambda;int i;
        while(true){
        TSPoint p = ts.getPoint();
        pinMode(XM, OUTPUT);
        pinMode(YP, OUTPUT);
        if (p.z > MINPRESSURE ) {tela_inicial_1(); }
        //Serial.println(i);
        delay(200);
        Lambda = 5.0*analogRead(14)/1023.0;
        
        myGLCD.printNumI(pulseIn(46, LOW) ,130,165,4,'0'); 
        i = Lambda*1000;
        if(i<1000){
          myGLCD.printNumI(i ,130,30,3,'0');
        }
        if(i < 150){
             myGLCD.setColor(VGA_BLUE);
             myGLCD.print("POBRE", 130, 60);
             myGLCD.setColor(VGA_SILVER);
          }
        else if(i > 800){
             myGLCD.setColor(VGA_RED);
             myGLCD.print("RICA ", 130, 60);
             myGLCD.setColor(VGA_SILVER);
          }
        else{
             myGLCD.setColor(VGA_GREEN);
             myGLCD.print("IDEAL", 130, 60);
             myGLCD.setColor(VGA_SILVER);
            }
        }

       
  }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////






/////////////////////////////////////////////////////////////////BATERIA /////////////////////////////////////////////////////////////////////////
void tela_bateria(){
     float val;
     myGLCD.fillScr(VGA_BLACK); myGLCD.setBackColor(0); myGLCD.setColor(176, 196, 222);
     myGLCD.print("Bateria", 0, 1); myGLCD.print("Volts", 210, 1);
     while (true) {
        TSPoint p = ts.getPoint();
        pinMode(XM, OUTPUT);
        pinMode(YP, OUTPUT);
        if (p.z > MINPRESSURE ) {tela_inicial_1(); }
        
        val = (3.08 * 5.0*analogRead(15)/1023.0) ;    // read the input pin
        myGLCD.printNumF(val, 2, 125,1,'.',5,'0'); //valor em volts na tela no formato 00.00
        //Serial.println(val);
        if(val<12){//fraca <12
             myGLCD.print("Estado: Fraca     ", 1, 51);   myGLCD.drawBitmap(110,90,60,42,bateriaFraca);
          }else if(val>=13){//carregando >13
             myGLCD.print("Estado: Carregando", 1, 51);myGLCD.drawBitmap(110,90,60,42,bateriaCarregando);
            }else{//normal
             myGLCD.print("Estado: Boa       ", 1, 51);myGLCD.drawBitmap(110,90,60,42,bateriaNormal);
              }

        
        }




}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////






///////////////////////////////////////////////////////////GIACEL/////////////////////////////////////////////////////////////////
//         0                       12                    25
//         = = = = = = = = = = = = = = = = = = = = = = = =
//         0px                                        319px
//         pouco                 normal               muito
//
//normal =~ 900rpm e tempo de injeção 1.5ms

void tela_G_ACEL() {
  flag_rpm = true;
  myGLCD.clrScr(); myGLCD.setBackColor(0); myGLCD.setColor(176, 196, 222);
  myGLCD.print("CONSUMO",80, 0);//gasto instantaneo com aceleraÃ§Ã£o 176,196,222
  myGLCD.drawBitmap(200,1,50,31,gasolina);
  myGLCD.setColor(105, 105, 105);
  myGLCD.print("Pouco", 0, 115);
  myGLCD.print("Normal", 120, 115);
  myGLCD.print("Muito", 240, 115);

  int x0_antes;
  for (int u = 0; u < 319; u = u + 10) {
    myGLCD.setColor(120, 120, 120);
    myGLCD.drawRect(u + 2, 80, u + 10, 110);
    if (u < 106) {
      myGLCD.setColor(0, 255, 0);
    }
    else if (u > 212) {
      myGLCD.setColor(255, 0, 0);
    }
    else {
      myGLCD.setColor(255, 215, 0);
    }
    myGLCD.fillRect(u + 4, 83, u + 8, 108);
  }
  while (true) {
    TSPoint p = ts.getPoint();
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    if (p.z > MINPRESSURE ) {tela_inicial_1(); }

      //// calculo de rpm
     detachInterrupt(0);
     rpm = 45*pulsos;
     milisegundos=millis();  
     attachInterrupt(digitalPinToInterrupt(21), contador, CHANGE);
     pulsos=0;
     ///////////////////////////

     
    tempo_injecao = pulseIn(46, LOW);

//desenho positivo
    if( tempo_injecao < 600 and rpm > 500 ){
      
      myGLCD.drawBitmap(120,150,64,57,certo);      
    }
else{
   myGLCD.setColor(0);
   myGLCD.fillRect(120,150,184,207); 
  }
/////////////////////////

//Serial.println("Rpm:");Serial.println(rpm);Serial.println("TI:");Serial.println(tempo_injecao);
    
    //debug 
   // myGLCD.printNumI(tempo_injecao, 260, 200,4,'0'); 
   // myGLCD.printNumI(rpm, 260, 220, 4, '0');
// rpm  =  900 - 6000
// TI   =  1500 - 30000
// amplitude maxima 80G = rpm * TI
// normal meio = 900  * 1500 = 157 k 
//

    
    tempo_injecao = ( rpm * tempo_injecao)/1000;

    if(tempo_injecao < 4000){
    tempo_injecao = map(tempo_injecao, 0, 4000, 0, 305);
    }
    else if(tempo_injecao > 4000){
    tempo_injecao = 305;
    }
    
    
    //ponteiro
    myGLCD.setColor(0);
    myGLCD.drawLine(x0_antes - 1, 45, x0_antes - 1, 76); //linha
    myGLCD.drawLine(x0_antes - 2, 45, x0_antes - 2, 75); //linha
    myGLCD.drawLine(x0_antes, 45, x0_antes, 78); //linha central
    myGLCD.drawLine(x0_antes + 1, 45, x0_antes + 1, 76); //linha
    myGLCD.drawLine(x0_antes + 2, 45, x0_antes + 2, 75); //linha
    myGLCD.setColor(255, 255, 255); ////////////////////////////////////////////acima apaga abaixo desenha//////////////////////////////////////////////////////////
    myGLCD.drawLine(tempo_injecao - 1, 45, tempo_injecao - 1, 76); //linha
    myGLCD.drawLine(tempo_injecao - 2, 45, tempo_injecao - 2, 75); //linha
    myGLCD.drawLine(tempo_injecao, 45, tempo_injecao, 78); //linha central
    myGLCD.drawLine(tempo_injecao + 1, 45, tempo_injecao + 1, 76); //linha
    myGLCD.drawLine(tempo_injecao + 2, 45, tempo_injecao + 2, 75); //linha
    x0_antes = tempo_injecao; delay(500);


  }
}




void contador() { //funcao q executa a cada pulso no pino 21 -> ttachInterrupt(digitalPinToInterrupt(21), contador, CHANGE);, sempre estarÃ¡ ativa quando flag_rpm = true;
  pulsos++;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////RPM//////////////////////////////////////////////////////////////////////////
void tela_rpm(){
     unsigned rpm=0;
   myGLCD.fillScr(VGA_BLACK);
  
   myGLCD.setColor(80,80,80);
   
   myGLCD.drawRect(0,100,10,140);
   myGLCD.drawRect(10,90,20,130);
   myGLCD.drawRect(20,80,30,120);
   myGLCD.drawRect(30,70,40,110);
   myGLCD.drawRect(40,60,50,100);
   myGLCD.drawRect(50,50,60,90);
   myGLCD.drawRect(60,40,70,80);
   myGLCD.drawRect(70,30,80,70);

   //myGLCD.setColor(0,255,0);
   
   myGLCD.drawRect(80,25,90,65);
   myGLCD.drawRect(90,20,100,60);
   myGLCD.drawRect(100,15,110,55);
   myGLCD.drawRect(110,10,120,50);

   myGLCD.drawRect(120,8,130,48);
   myGLCD.drawRect(130,6,140,46);
   myGLCD.drawRect(140,4,150,44);
   myGLCD.drawRect(150,2,160,42);//16

   //myGLCD.setColor(255,215,0);

   myGLCD.drawRect(160,2,170,42);
   myGLCD.drawRect(170,2,180,42);
   myGLCD.drawRect(180,2,190,42);
   myGLCD.drawRect(190,2,200,42);
   myGLCD.drawRect(200,2,210,42);
   myGLCD.drawRect(210,2,220,42);
   
   //myGLCD.setColor(255,165,0);
   
   myGLCD.drawRect(220,2,230,42);
   myGLCD.drawRect(230,2,240,42);
   myGLCD.drawRect(240,2,250,42);
   myGLCD.drawRect(250,2,260,42);
   myGLCD.drawRect(260,2,270,42);
   myGLCD.drawRect(270,2,280,42);
   myGLCD.drawRect(280,2,290,42);
   
  // myGLCD.setColor(255,0,0);
   
   myGLCD.drawRect(290,2,300,42);
   myGLCD.drawRect(300,2,310,42);//31


   myGLCD.setFont(SevenSeg_XXXL_Num);
   myGLCD.setBackColor(0,0,0);

   
while(true){
    TSPoint p = ts.getPoint();
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    if (p.z > MINPRESSURE ) {tela_inicial_1(); }
if(( millis()-milisegundos)>800){
  
   if(rpm>100 ){
      myGLCD.setColor(10,140,10); myGLCD.fillRect(1,101,9,139); 
     }        
   else {myGLCD.setColor(0,0,0); myGLCD.fillRect(1,101,9,139); }//myGLCD.fillRect(120,90,319,195);
   
   if(rpm>200){ myGLCD.fillRect(11,91,19,129);  }
   else {myGLCD.setColor(0,0,0);myGLCD.fillRect(11,91,19,129);}
   if(rpm>300){ myGLCD.fillRect(21,81,29,119);  }
   else {myGLCD.setColor(0,0,0); myGLCD.fillRect(21,81,29,119);}
   if(rpm>400){ myGLCD.fillRect(31,71,39,109);  }
   else{myGLCD.setColor(0,0,0); myGLCD.fillRect(31,71,39,109);  }
   if(rpm>500){ myGLCD.fillRect(41,61,49,99);  }
   else {myGLCD.setColor(0,0,0); myGLCD.fillRect(41,61,49,99);   }
   if(rpm>600){ myGLCD.fillRect(51,51,59,89);  }
   else  {myGLCD.setColor(0,0,0); myGLCD.fillRect(51,51,59,89);  }
   if(rpm>700){ myGLCD.fillRect(61,41,69,79);  }
   else  {myGLCD.setColor(0,0,0); myGLCD.fillRect(61,41,69,79);  }
   

   if(rpm>800){ myGLCD.setColor(50,205,50); myGLCD.fillRect(71,31,79,69);  }
   else  {myGLCD.setColor(0,0,0);  myGLCD.fillRect(71,31,79,69);  }
   if(rpm>900){ myGLCD.fillRect(81,26,89,64);  } 
   else  {myGLCD.setColor(0,0,0);  myGLCD.fillRect(81,26,89,64); }
    
   if(rpm>1000){myGLCD.setColor(0,255,0); myGLCD.fillRect(91,21,99,59);  }
   else  {myGLCD.setColor(0,0,0);myGLCD.fillRect(91,21,99,59); }
   if(rpm>1100){ myGLCD.fillRect(101,16,109,54);  }
   else {myGLCD.setColor(0,0,0); myGLCD.fillRect(101,16,109,54); }
   if(rpm>1200){ myGLCD.fillRect(111,11,119,49);  }
   else {myGLCD.setColor(0,0,0); myGLCD.fillRect(111,11,119,49); }
   
   if(rpm>1300){ myGLCD.fillRect(121,9,129,47);  }
   else  {myGLCD.setColor(0,0,0); myGLCD.fillRect(121,9,129,47); }
   if(rpm>1400){ myGLCD.fillRect(131,7,139,45);  }
   else {myGLCD.setColor(0,0,0); myGLCD.fillRect(131,7,139,45);  }
   if(rpm>1500){ myGLCD.fillRect(141,5,149,43);  }   
   else  {myGLCD.setColor(0,0,0); myGLCD.fillRect(141,5,149,43); }
   
   if(rpm>1600){ myGLCD.fillRect(151,3,159,40);  }
   else  {myGLCD.setColor(0,0,0);  myGLCD.fillRect(151,3,159,40); }
   if(rpm>1700){ myGLCD.fillRect(161,3,169,40);  } ///
   else {myGLCD.setColor(0,0,0);myGLCD.fillRect(161,3,169,40);   }

    
   if(rpm>1900){myGLCD.setColor(255,215,0); myGLCD.fillRect(171,3,179,40);  }
   else  {myGLCD.setColor(0,0,0);  myGLCD.fillRect(171,3,179,40); }
   if(rpm>2000){ myGLCD.fillRect(181,3,189,40);  }
   else {myGLCD.setColor(0,0,0); myGLCD.fillRect(181,3,189,40);  }
   if(rpm>2300){ myGLCD.fillRect(191,3,199,40);  }
   else {myGLCD.setColor(0,0,0); myGLCD.fillRect(191,3,199,40);  }
   if(rpm>2500){ myGLCD.fillRect(201,3,209,40);  }
   else  {myGLCD.setColor(0,0,0); myGLCD.fillRect(201,3,209,40);  }
   if(rpm>2700){ myGLCD.fillRect(211,3,219,40);  }
   else {myGLCD.setColor(0,0,0);myGLCD.fillRect(211,3,219,40);  }
   
   if(rpm>3000){myGLCD.setColor(255,140,0); myGLCD.fillRect(221,3,229,40); myGLCD.drawBitmap(1,150,55,89,marcha);  }
   else  {myGLCD.setColor(0,0,0); myGLCD.fillRect(221,3,229,40); myGLCD.fillRect(1,150,56,239);  }
   
   if(rpm>3100){ myGLCD.fillRect(231,3,239,40);  }
   else  {myGLCD.setColor(0,0,0);  myGLCD.fillRect(231,3,239,40);  }
   if(rpm>3200){ myGLCD.fillRect(241,3,249,40);  }
   else  {myGLCD.setColor(0,0,0);  myGLCD.fillRect(241,3,249,40); }
   if(rpm>3400){ myGLCD.fillRect(251,3,259,40);  }
   else  {myGLCD.setColor(0,0,0); myGLCD.fillRect(251,3,259,40); }
   if(rpm>3700){ myGLCD.fillRect(261,3,269,40);  }
   else  {myGLCD.setColor(0,0,0); myGLCD.fillRect(261,3,269,40); }
   if(rpm>3800){ myGLCD.fillRect(271,3,279,40);  } //28
   else  {myGLCD.setColor(0,0,0); myGLCD.fillRect(271,3,279,40);  }

   myGLCD.setColor(255,69,0);
   if(rpm>4000){ myGLCD.fillRect(281,3,289,40);  }
   else {myGLCD.setColor(0,0,0); myGLCD.fillRect(281,3,289,40); }
   if(rpm>4100){ myGLCD.fillRect(291,3,299,40);  }
   else {myGLCD.setColor(0,0,0); myGLCD.fillRect(291,3,299,40); }
   if(rpm>4200){ myGLCD.fillRect(301,3,309,40);  }
   else  {myGLCD.setColor(0,0,0); myGLCD.fillRect(301,3,309,40); }


 
     detachInterrupt(0);
     rpm = 32*pulsos;
     milisegundos=millis();  
     attachInterrupt(digitalPinToInterrupt(21), contador, CHANGE);
     pulsos=0;
     myGLCD.setColor(255,69,0);
     myGLCD.printNumI(rpm,55,90,4,'0');    



     
 }

  
  }   
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





/////////////////////////////////////////////////////////////////TELA INICIAL /////////////////////////////////////////////////////////////////
void tela_inicial_1(){
   myGLCD.clrScr();
   button("Tacometro",1);
   button("Vazao",2);
   button("Bateria",3);
   button("Sinais",4);
   while(true){
     TSPoint p = ts.getPoint();
         pinMode(XM, OUTPUT);
         pinMode(YP, OUTPUT);
         if(p.z > MINPRESSURE ){event_Click(read_button(p),1);}//se foi dado algum toque, chama a função event_Click levainformação(x,y,z)toque contido em
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////BUTTON ////////////////////////////////////////////////////////
void button(String nome,uint8_t n){
       myGLCD.setColor(VGA_BLUE);
     if(n==1){
         myGLCD.fillRect(75,30,245,70);
         myGLCD.setColor(VGA_WHITE);
         myGLCD.drawRect(75,30,245,70);
         myGLCD.setBackColor(VGA_BLUE);
         myGLCD.setColor(VGA_WHITE);
         myGLCD.setFont(arial_normal);
         myGLCD.print(nome, 77, 45);
    }
    if(n==2){
         myGLCD.fillRect(75,80,245,120);
         myGLCD.setColor(VGA_WHITE);
         myGLCD.drawRect(75,80,245,120);
         myGLCD.setBackColor(VGA_BLUE);
         myGLCD.setColor(VGA_WHITE);
         myGLCD.setFont(arial_normal);
         myGLCD.print(nome, 77, 95);
      }
      if(n==3){
         myGLCD.fillRect(75,130,245,170);
         myGLCD.setColor(VGA_WHITE);
         myGLCD.drawRect(75,130,245,170);
         myGLCD.setBackColor(VGA_BLUE);
         myGLCD.setColor(VGA_WHITE);
         myGLCD.setFont(arial_normal);
         myGLCD.print(nome, 77, 145);
      }
      if(n==4){
         myGLCD.fillRect(75,180,245,220);
         myGLCD.setColor(VGA_WHITE);
         myGLCD.drawRect(75,180,245,220);
         myGLCD.setBackColor(VGA_BLUE);
         myGLCD.setColor(VGA_WHITE);
         myGLCD.setFont(arial_normal);
         myGLCD.print(nome, 77, 195);
      }
      
    }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////READ BUTTON //////////////////////////////////////////////////
uint8_t read_button(TSPoint p){
   p.x = myGLCD.getDisplayXSize() - (map(p.x, TS_MINX, TS_MAXX, myGLCD.getDisplayXSize(), 0));
     p.y = myGLCD.getDisplayYSize() - (map(p.y, TS_MINY, TS_MAXY, myGLCD.getDisplayYSize(), 0));
   myGLCD.setColor(VGA_RED);
   if(p.x>75 && p.y>32 && p.x<245 && p.y<70 ){//botao 1
         myGLCD.drawRect(75,30,245,70);delay(200); return 1;  
        }
   if(p.x>75 && p.y>80 && p.x<245 && p.y<120 ){//botao 2
         myGLCD.drawRect(75,80,245,120);delay(200);return 2;        
        }
   if(p.x>75 && p.y>130 && p.x<245 && p.y<170 ){//botao 3
         myGLCD.drawRect(75,130,245,170);delay(200);return 3;        
        }
   if(p.x>75 && p.y>180 && p.x<245 && p.y<220 ){//botao 3
         myGLCD.drawRect(75,180,245,220);delay(200);return 4;        
        }

    
    
    } 

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////VEENT CLICK////////////////////////////////////////////////////////
  
void event_Click(uint8_t i, boolean a){
        if(a){//caso seje a tela inicial 1
           switch (i){
             case 1:
             tela_rpm();
             break;
             case 2:
             tela_G_ACEL();
             break;
       case 3:
             tela_bateria();
             break;
             case 4:
              tela_sinais();
             break;
        }
        
      }
        else{
             switch (i){
             case 1:
             //tela_injecao();
             break;
             case 2:
            // tela_bateria();
             break;
             case 3:
            // tela_compacto();
             break;
             case 5:
             tela_inicial_1();
             break;
        }


        }
 
    }

  
