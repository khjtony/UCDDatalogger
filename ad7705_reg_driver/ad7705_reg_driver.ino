// Comm. Register Codes
// to set up a read from data reg on second channel:
// spi_transfer(READ|DATA_REG|CH1)
#define DRDY_BIT      (7)
#define COMM_REG      (0x00)
#define SETUP_REG     (0x10)
#define CLOCK_REG     (0x20)
#define DATA_REG      (0x30)
#define OFFSET_REG    (0x60)
#define GAIN_REG      (0x70)
#define READ          (0x08)
#define WRITE         (0x00)
#define STBY          (0x04)
#define AIN1          (0x00)
#define AIN2          (0x01)

// Setup Reg. Codes
#define OP_NORMAL   (0x00)
#define OP_SELF_CAL (0x40)
#define OP_ZS_CAL   (0x80)
#define OP_FS_CAL   (0xC0)
#define GAIN_1      (0x00)
#define GAIN_2      (0x08)
#define GAIN_4      (0x10)
#define GAIN_8      (0x18)
#define GAIN_16     (0x20)
#define GAIN_32     (0x28)
#define GAIN_64     (0x30)
#define GAIN_128    (0x38)
#define BIPOLAR     (0x00)
#define UNIPOLAR    (0x04)
#define BUF_ON      (0x02)
#define BUF_OFF     (0x00)
#define FSYNC       (0x01)
#define FSYNC_CLR   (0x00)

// Clock Reg. Codes
// These settings let it update at about 20hz
#define CLKDIS (0x10) // 00010000
#define CLKDIV (0x08) // 00001000
#define CLK    (0x04) // 00000100
#define FILTER (0x00) // Not sure about this one

// Pin setup
#define DOUT      (11)
#define DIN       (12)
#define SCLK      (13)
//#define CS_AD7705 (10)

#define DRDY (10)

byte clr;  // dump variable
 
void setup()
{
  Serial.begin(9600);

  pinMode(DOUT, OUTPUT);
  pinMode(DIN, INPUT);
  pinMode(SCLK,OUTPUT);
  pinMode(DRDY,INPUT);
  //pinMode(CS_AD7705,OUTPUT);
//  digitalWrite(CS_AD7705,HIGH); // disable device 
 
 // SPCR = (1<<SPE)|(1<<MSTR)|(1<<CPOL);  // int. disabled, spi enabled, msb 1st, master, clk idle high, leading edge clk, 4MHz
  SPCR = _BV(SPE) | _BV(MSTR) | _BV(CPOL) | _BV(CPHA) | _BV(SPI2X) | _BV(SPR1) | _BV(SPR0);
  clr = SPSR;   // read status register to junk
  clr = SPDR;   // read data register to junk

  delay(10);

  // Setup the AD7705
//  digitalWrite(CS_AD7705,LOW);  // enable device
  spi_transfer(AIN1|WRITE|CLOCK_REG);  // Select channel 1, setup write to clock reg.
//  spi_transfer(CLKDIS|CLKDIV|CLK|FILTER);  // 5MHz Clock, 50MHz output rate
  spi_transfer(CLKDIV|CLK);
//  spi_transfer(AIN1|WRITE|SETUP_REG);  // Select channel 1, setup write to setup reg.
 // spi_transfer(GAIN_1|UNIPOLAR|BUF_OFF|FSYNC_CLR|OP_SELF_CAL);  // gain=1, bipolar mode, buffer off, clear FSYNC and perform a Self Calibration
  /*
   *  Some routine to interrogate DRDY is Low
   *    - read from comm. reg. or read DRDY pin
   */
   Serial.println("before ready");
   while( !digitalRead(DRDY) );
   
}
 
void loop()
{
  unsigned int data=0;

//  digitalWrite(CS_AD7705,LOW);  // enable device

   while( !digitalRead(DRDY) );
 
  spi_transfer(AIN1|DATA_REG|READ);  // select ch1 and setup read 16bit data register
  data = spi_transfer(0)<<8;  // Get the most significant 8 bits
  data |= spi_transfer(0);  // Get the least significant 8 bits
//  digitalWrite(CS_AD7705,HIGH);  // release device
 
  Serial.println(data,HEX);
  delay(1000);
}
 
char spi_transfer(volatile char data)
{
  SPDR = data;                    // Start the transmission
  while (!(SPSR & (1<<SPIF)))     // Wait for the end of the transmission
  {
  };
  return SPDR;                    // return the received byte
}

void reset() {
    digitalWrite(pinCS, LOW);
    for (int i = 0; i < 100; i++)
        spiTransfer(0xff);
    digitalWrite(pinCS, HIGH);
}
