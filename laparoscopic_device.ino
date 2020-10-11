/*
 * //Connections
 * Pin 8 -> output
 * Pin 5,7 -> Gnd
 * Pin 1,6(Vref) -> Vcc
 * Pin 2(Chip select) -> D10  **pull up this pin with a 1.2k resistor
 * Pin 3(SCK) -> D13
 * Pin 4 DataOut(SDI) -> D11
 * pressure reg adc a0
 * relay controls inlet2 outlet3
 * 
 * //SPI pins
 * dataout_MOSI  = ICSP4//11 MOSI
 * clock_SCK = ICSP3//13  sck
 * slaveselect_SS = 52//10 ss
 * 
 * Macro = DUE//NANO pin_name
*/

/*----------------------------------------------Definations-------------------------------------*/
#define dataout_MOSI MOSI  //ICSP 4
#define clock_SCK SCK      //ICSP 3
#define SS 52  //10  //Slave Select 
#define outpin  36
#define inpin  42
#define Stop_pin  50 //reed switch
//#define vref 3.33

/*----------------------------------------------Variables-------------------------------------*/
unsigned long t = 0;
char input;
int flag=0;
float voltage,bars,value,voltage_opamp=0,voltage_mcp=0,vref=3.3,val;

/*------------------------------------------Function_prototypes-------------------------------------*/
void send_bars();

/*------SPI-------*/
void sendIntValueSPI(int val);
void sendclock_SCK();
void sendSPIHeader();

void read_char();

/*------------------------------------------Setup-------------------------------------*/
void setup() 
{
  Serial.begin(9600);
  
  pinMode(dataout_MOSI,OUTPUT);
  pinMode(clock_SCK,OUTPUT);
  pinMode(SS,OUTPUT);
  
  pinMode(outpin,OUTPUT);
  pinMode(inpin,OUTPUT);
  pinMode(Stop_pin,INPUT);
  
  digitalWrite(outpin,LOW);  
  digitalWrite(inpin,LOW);
  
  Serial.println("[program start]");
}

/*------------------------------------------Loop-------------------------------------*/
void loop()
{
  if(Serial.available()>0)
  {
    read_char();
  }
}

/*----------------------------------------------Functions-------------------------------------*/

void send_bars()
{
  bars = Serial.parseFloat();
  if(bars!=0)
  {
    value = bars;
  }
  else
  {
    bars = value;
  }
  Serial.print("bars ");
  Serial.println(bars);
  val = ((float)bars * ((3*vref)/6.0));
  voltage_opamp = val;
  Serial.print("voltage_opamp ");
  Serial.println(voltage_opamp);
  voltage_mcp = voltage_opamp/3;
  val *= 4096/(3*vref);    
  sendIntValueSPI((int)val);
  Serial.print("voltage_mcp ");
  Serial.println(voltage_mcp); 
}


void send_voltage()
{
  voltage = Serial.parseFloat();
  Serial.print("voltage");
  Serial.println(voltage);
  val = 4096/(vref)*voltage;    
  sendIntValueSPI((int)val);
  Serial.print("voltage_mcp ");
  Serial.println(voltage_mcp); 
}
/******************SPI*******************/

void sendIntValueSPI(int val) 
{
  digitalWrite(SS,LOW);  // initiate data transfer with 4921
  sendSPIHeader();  // send 4 bit header
  Serial.print("val: ");
  Serial.println(val);
  //send data
  for(int i=11;i>=0;i--){
    Serial.print(((val&(1<<i)))>>i);
    digitalWrite(dataout_MOSI,((val&(1<<i)))>>i);
    sendclock_SCK();
  }
  Serial.println();
  Serial.println("Function Ends");
  Serial.println();
  digitalWrite(SS,HIGH);  // finish data transfer
}
 
void sendSPIHeader() 
{
  // bit 15
  // 0 write to DAC *
  // 1 ignore command
  digitalWrite(dataout_MOSI,LOW);
  sendclock_SCK();
  
  // bit 14 Vref input buffer control
  // 0 unbuffered *
  // 1 buffered
  digitalWrite(dataout_MOSI,LOW);
  sendclock_SCK();
  
  // bit 13 Output Gain selection
  // 0 2x
  // 1 1x *
  digitalWrite(dataout_MOSI,HIGH);
  sendclock_SCK();
  
  // bit 12 Output shutdown control bit
  // 0 Shutdown the device
  // 1 Active mode operation *
  digitalWrite(dataout_MOSI,HIGH);
  sendclock_SCK();
}
 
void sendclock_SCK() 
{
  digitalWrite(clock_SCK,HIGH);
  digitalWrite(clock_SCK,LOW);
}

/***************Pneumatics******************/

void read_char()
{
    while (Serial.available())
    {  
      delay(10);  
      input = Serial.read(); 
    }  
    switch(input)
    {
      case 'v':
          send_voltage();
          Serial.println("voltage sent");
          break;
      case 'c':                          //BACKWARD STROKE
          Serial.println("cut");
          //flag=0;
          break;
      case 'f':                          //FORWARD STROKE             
          sendIntValueSPI((int)val);
          Serial.println("forward stroke");
          //flag++;
          break;
      case 'h':                          //HOLD STROKE 
          Serial.println("hold stroke");
          break;
    }
}

