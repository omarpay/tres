// PRUEBA DE GIT: OMAR
// Ahí va otra prueba
////////////  SHUNT   ////////////////

#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include <Average.h>
#include <TimerOne.h> 

volatile long int N_PASO=0;   
volatile long int repetidor=0;   
volatile long int Enteros[4]={0,0,0,0};   
volatile long int Residuo[4]={0,0,0,0};
volatile int Paso_fin=0;
volatile bool start=true;


Adafruit_ADS1115 ads;
  int16_t adsA0 = 0;
  int16_t adsA1 = 0;
  int16_t bitshunt = 0;
  float volts=0.0;
  float amperaje=0.0;
  float mode_amperaje=0.0;
  int DatoNextion=0;

   int16_t bits_canal_1 = 0;
   int16_t bits_canal_2 = 0;
   float amperaje1=0.0;
   float amperaje2=0.0;
    int voltajeLect=0;
    int amperLect=0;

long int Tiempo_Paso[4]={0,0,0,0};  // en seg
long int Voltage_Paso[4]={0,0,0,0}; //en volts
long int Secret[2]={0,0}; // datos para k secret  Tiempo y K
long int Status=0;
volatile int K_PASOS[4]={0,0,0,0};
////////////  NEXTION   ////////////////

int char1, char2;
int charflag_volt=0; 
double v=0.00;

const int buttonPin = 5;     // pedal ENTRADA
bool buttonState = 0;         // variable for reading the pushbutton status
bool ampere_state = 0;       // Variable para leer el amperaje. 
bool getk_state=0;
double k1=0.00;
int BussData[18];
////////////  POT   ////////////////

int unoIC=10;
int dosUD=12;
int sieCS=13;

int k=0;
bool k_set=false;




void setup() {
  
  Serial.begin(9600);
   ///////////////INICIO TIMER INTERRUPTOR CONFIGURACIÓN ////////////////  
                                                             //Configura el TIMER en MICROSEGUNDOS , MAXIMO 8.38 SEG APROX USAR 8 
   Timer1.attachInterrupt(Temporizador);                     //Configura la interrupción del Timer 1
   Timer1.stop();
 /////////////// FIN  TIMER INTERRUPTOR CONFIGURACIÓN  ////////////////  

////////////  INICIO ADC CONFIGURACIÓN  ////////////////

  ads = 0x48;
  // Setting these values incorrectly may destroy your ADC!
  //                                                                ADS1015  ADS1115
  //                                                                -------  -------
  // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  // ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV usar a voltimetro
  // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV hoja datos

  ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV      0.03125mv (verificado by juan)
  ads.begin();
  
////////////  FIN ADC CONFIGURACIÓN  ////////////////


  //    k1=-0.0003*v*v*v*v*v + 0.0057*v*v*v*v - 0.0382*v*v*v + 0.1013*v*v + 9.1291*v - 0.055;   


////////////  NEXTION   ////////////////

    pinMode(buttonPin, INPUT);
  



////////////  POT   ////////////////

  pinMode(unoIC, OUTPUT);
  pinMode(dosUD, OUTPUT);
  pinMode(sieCS, OUTPUT);

          //bajar POT 
          for(int i = 0; i<=99; i++)
          {
           Down(unoIC, dosUD);
           delay(5);
          }
///INTERRUPCION///////////////       
attachInterrupt(digitalPinToInterrupt(2),pedalFunc,RISING);
///INTERRUPCION///////////////   

}


void loop() {
 
////////////  NEXTION   ////////////////
//CAMBIAR POR INTERRUPT  /// checar en funciones
//   if (digitalRead(buttonPin) == HIGH && ampere_state== 0) {
//    Serial.print("click START,1");
//    ff();
//    delay(500);
//    Serial.print("click START,0");
//    ff();
//    delay(500);
//    }
//CAMBIAR POR INTERRUPT
/////////////////////////////////////////////////////////////////////////////////////////////////////////////LECTURA DE BUS INICIO/////////////
 if (Serial.available()>1)                           // Si llegaron 2 datos al menos 
  {                                                  // (0)
      Serial.println("Serial aviable");
      char1=Serial.read();                              // Lee un byte del puerto serie 
      Serial.print("char1=");
      Serial.println(char1);
      if(char1==0xFE)                                   // FE Hex= 254 DEC   //nibble de instruccion , (leer bus)
      {                                               
       
        Serial.println("BussData: ");
        BussData[0]=char1;
        Serial.println(BussData[0]);
        for(int i=1;i<18;i++)
        {
          BussData[i]=Serial.read();
          if(BussData[i]==-1)
          {
            i--;
          }
        }
       
       for(int i=1;i<18;i++)
       {
        Serial.println(BussData[i]);
       }
       if(BussData[17]==0xFD)              // FD Hex= 253 DEC
        {
          Serial.println("Terminó el buffer correctamente");                    //ACONDIcionamiento de datos 
          for (int i=1;i<=4;i++){                                              //ojo uso de datos de 1 a 8
            Tiempo_Paso[i-1]=BussData[i]*(60)+BussData[i+4];                   //tiempo de Paso a segundos y guardados
            } 
          for (int i=9;i<=12;i++){
            Voltage_Paso[i-9]=BussData[i];
            }
            Status=BussData[13];
            Secret[0]=BussData[14]*(60)+BussData[15];                         // tiempo en seg de ks
            Secret[1]=BussData[16];                                                   // ks   IMPORTANTE LA K MAS ALTA USABLE ES 87 , A 10.59, A MAS K NO AUMENTARA  CADA K =0.1217241V +0.1 VERIFICADO 

            Serial.println("Datos almacenados ");
            Serial.println("tiempos de pasos en seg");
            Serial.println(Tiempo_Paso[0]);
            Serial.println(Tiempo_Paso[1]);
            Serial.println(Tiempo_Paso[2]);
            Serial.println(Tiempo_Paso[3]);
            Serial.println("voltajes de pasos ");
            Serial.println(Voltage_Paso[0]);
            Serial.println(Voltage_Paso[1]);
            Serial.println(Voltage_Paso[2]);
            Serial.println(Voltage_Paso[3]);
            Serial.println("tiempo ks ");
            Serial.println(Secret[0]);
            Serial.println(" ks ");
            Serial.println(Secret[1]);
            Serial.println(" STATUS ");
            Serial.println(Status);
                                                                              //acondicionamiento para timers
             Enteros[0]=CalcEnteros(Tiempo_Paso[0]);
             Residuo[0]=CalcResiduo(Tiempo_Paso[0]);
             Enteros[1]=CalcEnteros(Tiempo_Paso[1]);
             Residuo[1]=CalcResiduo(Tiempo_Paso[1]);
             Enteros[2]=CalcEnteros(Tiempo_Paso[2]);
             Residuo[2]=CalcResiduo(Tiempo_Paso[2]);
             Enteros[3]=CalcEnteros(Tiempo_Paso[3]);
             Residuo[3]=CalcResiduo(Tiempo_Paso[3]);

        }
        if(BussData[17]!=0xFD){Serial.println(" No se completo la comunicación ->error de comunicación.");}
        
       }   
       if(char1!=0xFE){ Serial.println(" Inicializador no reconocido ->error de comunicación.");}
       
   while(Serial.available()){
    Serial.read(); 
     }
  
   }
 /////////////////////////////////////////////////////////////////////////////////////////////////////////////LECTURA DE BUS FINAL/////////////  
   
 

//////////// INICIALIZACION DE PASOS  ////////////
if(Status==1 && k_set==false){
    N_PASO=0;   
    repetidor=0; 
  
    
     if(Enteros[0]==0){Timer1.initialize(Residuo[0]); Paso_fin=1;}
     if(Enteros[0]!=0){Timer1.initialize(8000000); Paso_fin=0;}
     if(Enteros[0]==1 && Residuo[0]==0){Timer1.initialize(8000000); Paso_fin=1;}
     if(Enteros[0]>1){Timer1.initialize(8000000); Paso_fin=0;}
    
    for(int i = 0; i<=3; i++) {
      v=Voltage_Paso[i]/10;
      k1 =(v -0.1)/0.1217241;
      k1=round(k1);
      K_PASOS[i]=k1;                                                                          //conversion forzada
    }        
    
    for(int i = 0; i<=99; i++)                                                                    //for de bajar
    {
     digitalWrite(12, LOW);
     digitalWrite(10, HIGH);
     digitalWrite(13, LOW);
     digitalWrite(10, LOW);

    }
    
    for(int i = 0; i<K_PASOS[0]; i++)                                                       // for de subir
    {
        digitalWrite(13, LOW);
        digitalWrite(12, HIGH);
        digitalWrite(10, HIGH);
        digitalWrite(10, LOW);
    }
     Serial.println(" inicie paso 0");  
      k_set=true;


  }
//////////// FIN INICIALIZACION DE PASOS  ////////////  


///////////INICIALIZACION DE KS/////////////
if(Status==3 && k_set==false){
       N_PASO=0;   
       repetidor=0; 
       Enteros[0]=CalcEnteros(Secret[0]);
       Residuo[0]=CalcResiduo(Secret[0]);
     if(Enteros[0]==0){Timer1.initialize(Residuo[0]); Paso_fin=1;}
     if(Enteros[0]!=0){Timer1.initialize(8000000); Paso_fin=0;}
     if(Enteros[0]==1 && Residuo[0]==0){Timer1.initialize(8000000); Paso_fin=1;}
     if(Enteros[0]>1){Timer1.initialize(8000000); Paso_fin=0;}
                                                                                      
       for(int i = 0; i<=99; i++)                                                     //for de bajar
       {
         digitalWrite(12, LOW);
         digitalWrite(10, HIGH);
         digitalWrite(13, LOW);
         digitalWrite(10, LOW);
       }
       
       for(int i = 0; i<=Secret[1]; i++)                                                       // for de subir
       {
         digitalWrite(13, LOW);
         digitalWrite(12, HIGH);
         digitalWrite(10, HIGH);
         digitalWrite(10, LOW);
       }
       Serial.println(" inicie paso 0 de ks");  


       
  k_set=true;
  }




////////////  SHUNT   ////////////////


if(k_set==true) {

 //--------LECTURA SHUNT---------------
  ads.setGain(GAIN_FOUR);     //+/- 1.024V
  bits_canal_1=ads.readADC_Differential_0_1();
  amperaje1=(-1)*bits_canal_1*0.03125;//mv ganancia x4 resolución El factor 1 A/mV es conversor // shunt amperaje (A)
//--------LECTURA SHUNT---------------

//--------LECTURA VOLTIMETRO---------------
  //ads.setGain(GAIN_ONE);        //+/- 4.096V muy al limite
  ads.setGain(GAIN_TWOTHIRDS);  //  2/3x gain +/- 6.144V pero no usar mas de 5.3v
  bits_canal_2=ads.readADC_Differential_2_3();
  amperaje2=(-1)*bits_canal_2*0.1875;//mv ganancia x1 resolución  voltaje/3 (mv)
//--------LECTURA VOLTIMETRO---------------

//ACONDICIONAMIENTO DE VARIABLES A NEXTION//  
 
   amperLect=1000*amperaje1;
   voltajeLect=3*amperaje2;
//ACONDICIONAMIENTO DE VARIABLES A NEXTION// 

//ENVIO DE SEÑALES//
//
//  Serial.print("xV.val=");//indicador nextion
//  Serial.print(voltajeLect);
//    ff();
//
//  Serial.print("add 17,0,");//indicador nextion
//  Serial.print(voltajeLect*255*(0.000111)); //255/(9x1000)
//   ff();
//
//  Serial.print("xA.val=");//indicador nextion
//  Serial.print(amperLect);
//    ff();

//ENVIO DE SEÑALES//


  }
  
}



//////////////////////INCIO DE FUNCIONES///////////////////////////////////////////////////////
void bajar_getkstate() {
  if (Serial.available()>1)                             // Si llegaron 2 datos al menos 
  {                                                   // (0)
    char1=Serial.read();                              // Lee un byte del puerto serie 
    if(char1==0x71)                                   // (1)
      {                                               //Máscara 0x71 =  valor numérico
      char2=Serial.read();                            // Lee el segundo byte de la trama, va2.val
      
      Serial.print("char2: ");
      Serial.println(char2);
      
        if(char2==106){ // Si es terminar get k
        getk_state=0;
        }

        if(char2>=0 && char2<=100) // Si se introduce un valor de k (0 a 100)
        {
        k=char2;
        }
    }
  }
}

void pedalFunc(){

    Serial.print("click START,1");
    ff();
    Serial.print("click START,0");
    ff();
    
}
void bajar_amperstate() {
  if (Serial.available()>1)                             // Si llegaron 2 datos al menos 
  {                                                   // (0)
    char1=Serial.read();                              // Lee un byte del puerto serie 
    if(char1==0x71)                                   // (1)
      {                                               //Máscara 0x71 =  valor numérico
      char2=Serial.read();                            // Lee el segundo byte de la trama, va2.val
      
      Serial.print("char2: ");
      Serial.println(char2);
      
       if(char2==102){ // Si es terminar (START NEXTION) (4)
        //  Serial.println("Botón START apagado");
          ampere_state = 0;

          //BAJAR POT
          for(int i = 0; i<=99; i++)
          {
            Down(unoIC, dosUD);
            delay(10);
          }
          Serial.println("DOWN");
          DatoNextion=false;
          amper_shunt2next();
         // Serial.print("click START,0");
         // ff();
        } // (4)
      }
    }
}



// Oxff 
void ff(){ 
  Serial.write(0xff); 
  Serial.write(0xff); 
  Serial.write(0xff);
}



void amper_shunt2next(){

  Serial.print("x1.val=");
  Serial.print(DatoNextion);
  ff();
  
}
///////////////////////////INICIO FUNCIONES PARA TIMER //////////////////////////
void Temporizador(void)
{
// Serial.print("rep: ");
// Serial.print(repetidor);
// Serial.print("   numero de paso:  ");
// Serial.print(N_PASO);
// Serial.print("   Paso_fin:  ");
// Serial.println(Paso_fin);

              repetidor++;
              if(repetidor+1==Enteros[N_PASO]&& Enteros[N_PASO] > 0 && Paso_fin==0 && Residuo[N_PASO]==0){   //**
              Timer1.setPeriod(8000000); Paso_fin=1; repetidor=0;
              }
              
              if(repetidor==Enteros[N_PASO]&& Enteros[N_PASO] > 0 && Paso_fin==0 ){
              Timer1.setPeriod(Residuo[N_PASO]); Paso_fin=1; repetidor=0;
              }
         
              if(Paso_fin==1 && repetidor==1){
                Serial.print("Acabe PASO ");Serial.println(N_PASO);
                if( Enteros[N_PASO+1]==0){Timer1.setPeriod(Residuo[N_PASO+1]);}//posible poner paso fin =1;
                if (Enteros[N_PASO+1]>0){Timer1.setPeriod(8000000); Paso_fin=0;}
                if (Enteros[N_PASO+1]==1 && Residuo[N_PASO+1]==0){Timer1.setPeriod(8000000); Paso_fin=1;} //*
                N_PASO++;
                for(int i = 0; i<=99; i++)                                                     //for de bajar
                {
                 digitalWrite(12, LOW);
                 digitalWrite(10, HIGH);
                 digitalWrite(13, LOW);
                 digitalWrite(10, LOW);
                }
                 
                 for(int i = 0; i<=K_PASOS[N_PASO]; i++)                                                       // for de subir
                 {
                   digitalWrite(13, LOW);
                   digitalWrite(12, HIGH);
                   digitalWrite(10, HIGH);
                   digitalWrite(10, LOW);
                 }
                 if(N_PASO<4){
                   Serial.print(" K:  ");
                   Serial.println(K_PASOS[N_PASO]);
                 }
                if(N_PASO==4){
                  Timer1.stop(); 
                  for(int i = 0; i<=99; i++)                                                     //for de bajar
                  {
                   digitalWrite(12, LOW);
                   digitalWrite(10, HIGH);
                   digitalWrite(13, LOW);
                   digitalWrite(10, LOW);
                 
                  }
                  k_set=false;
                  Status=0;
                }
                repetidor=0;
              }
 }



long int CalcResiduo(int t) { 
  
  int Enteros=t*(0.125);                          // usando conversiones forzadas
  float Decimal=(t*(0.125)-Enteros)*8;            // usando conversiones forzadas
  long int Residuo=Decimal*(1000000);

  return Residuo; 
}

 int CalcEnteros(int t) { 
  
  int Enteros=t*(0.125);                          // usando conversiones forzadas

  return Enteros; 
} 
///////////////////////////FIN FUNCIONES PARA TIMER //////////////////////////

/// voids 2POT ///
void Up(int unoIC, int dosUD){
  digitalWrite(sieCS, LOW);
  digitalWrite(dosUD, HIGH);
  digitalWrite(unoIC, HIGH);
  delay(10);
  digitalWrite(unoIC, LOW);
}
void Down(int unoIC, int dosUD){
  digitalWrite(dosUD, LOW);
  digitalWrite(unoIC, HIGH);
  digitalWrite(sieCS, LOW);
  delay(1);
  digitalWrite(unoIC, LOW);
}
