
#include "derivative.h" /* include peripheral declarations */
#include "kinetis_sysinit.h"
#define PortLCD    GPIOD_PDOR
#define RS_1   GPIOB_PDOR |= 0x01
#define RS_0   GPIOB_PDOR &= 0xFE
#define Enable_1	   GPIOB_PDOR |= 0x02
#define Enable_0	   GPIOB_PDOR &= 0xFD

// time for delay
#define			n15mseg			0x02E8
#define			n40useg			0x1AD0

#define 		nIns			0x00
#define 		nData			0x01

const unsigned char InitializeLCD[5] = {0x38, 0x38, 0x38, 0x0C, 0x01}; //Cursor Blink on initialisation

void cfgPorts(void);
void delay(long time);
void initLCD(void);
void sendCode(int Code, int Data);
void rtc_init(void);
void Clock_Configuration (void);
void Tpm_centesimas (void);
void cfg_pwm (void);

int segundos = 0,minutos = 0,segundos1 = 0,minutos1 = 0,horas = 0, horas1 = 0;
int main(void)
{
	cfgPorts();
		initLCD();
		cfg_pwm();
		for(;;) 
		{			
				Tpm_centesimas();
				if (TPM2_C0V == 0)
				{
				sendCode(nIns,0xC0);
				sendCode(nData,' ');
				sendCode(nData,' ');
				sendCode(nData,' ');
				sendCode(nData, horas+0x30);
				sendCode(nData, horas1+0x30);
				sendCode(nData, ':');
				sendCode(nData, minutos+0x30);
				sendCode(nData, minutos1+0x30);
				sendCode(nData, ':');
				sendCode(nData, segundos+0x30);
				sendCode(nData, segundos1+0x30);
				}
		}
		return 0;
}
void cfgPorts(void)
{
	//Turn on clock for portB and portC
	SIM_SCGC5 = 0x3D00;//0001 1100 0000 0000 see page 206
	//Also, we can use predefined masks created by freescale, such as:
	//SIM_SCGC5_PORTB_MASK;
	
	//PORTD_PCR6=(PORT_PCR_MUX(1));	
	
	
	/* Set pins of PORTC as GPIO */
	PORTD_PCR0=(0|PORT_PCR_MUX(1));
	PORTD_PCR1=(0|PORT_PCR_MUX(1));
	PORTD_PCR2=(0|PORT_PCR_MUX(1));
	PORTD_PCR3=(0|PORT_PCR_MUX(1));
	PORTD_PCR4=(0|PORT_PCR_MUX(1));
	PORTD_PCR5=(0|PORT_PCR_MUX(1));
	PORTD_PCR6=(0|PORT_PCR_MUX(1));
	PORTD_PCR7=(0|PORT_PCR_MUX(1));
    
	
	/* Set pins of PORTB 0 and PORTB 1 as GPIO */
	PORTB_PCR0=(0|PORT_PCR_MUX(1));
	PORTB_PCR1=(0|PORT_PCR_MUX(1));
	PORTB_PCR3=(0|PORT_PCR_MUX(1));
	
	//LED
	PORTB_PCR18=(0|PORT_PCR_MUX(1));
	
	PORTB_PCR2=(PORT_PCR_MUX(0));
	
	// SET PIN FOR BUTTON
	//PORTE_PCR0=(0|PORT_PCR_MUX(1));
	//Reasure first PortD and B value
	GPIOB_PDOR = 0x00;
	GPIOD_PDOR = 0x00;
	//GPIOE_PDOR = 0x00;
	//GPIOC_PDOR = 0x00;
	
	//Configure PortD as outputs
	GPIOD_PDDR = 0xFF;
	//Configure PortB as outputs
	GPIOB_PDDR = 0xFF;
	//Configure PortE as inputs
	//GPIOE_PDDR = 0x00;
	//Configure PortC as outputs
	//GPIOC_PDDR = 0xFE;
	
}
void sendCode(int Code, int Data)
{
	//Assign a value to pin RS
	/*HINT: When RS is 1, then the LCD receives a data
	when RS is 0, then the LCD receives an instruction */
	// Initialise RS and Enable with 0
	RS_0;
	Enable_0;
	//Assign the value we want to send to the LCD
	PortLCD = Data;	
	
	//We make the algorithm to establish if its an instruction we start with 0 on RS value, otherwise if its a data command we start with RS as 1;
	
	if (Code == nIns)
	{
		RS_0;
		Enable_1;
		delay(n40useg);
		Enable_0;
		RS_0;
	}
	
			
	else if(Code == nData)
	{
		RS_1;
		Enable_1;
		delay(n40useg);
		Enable_0;
		RS_0;
	}
}
void initLCD(void)
{
	unsigned int i;											/* local index */
	//LCD requires a delay of 15 msec
	delay(n15mseg);									/* wait for 15 mseg */
	
	/* send initialization instructions */
	/* Loop for sending each character from the array */
	for(i=0;i<5;i++)
	{										
		sendCode(nIns, InitializeLCD[i]);	/* send initialization instructions */
					
	}

}
void delay(long time)
{
	/*@description: 
	*/
	//Initiate time
	long x;
	x = time;
	
	//Execute loop as long as x is not zero
	while (x != 0)
		{
		//Decrease x
		x--;
		}
}
void Clock_Configuration (void)
{
	MCG_C1 |= MCG_C1_IRCLKEN_MASK; //Enable internal reference clock
	MCG_C2 &= ~(MCG_C2_IRCS_MASK);  //Internal Reference Clock -->Slow
	
	//**RTC_CLKIN**//
	PORTC_PCR1 |= (PORT_PCR_MUX(0x1));       //PTC1 as RTC_CLKIN
	SIM_SOPT1 |= SIM_SOPT1_OSC32KSEL(0b10);  //32 Khz clock source for RTC
	
	//**32KHz Frequency**//
	SIM_SOPT2 |= SIM_SOPT2_CLKOUTSEL(0b100); //Clockout pin --> 32 KHz
	
	PORTC_PCR3 |= (PORT_PCR_MUX(0x5)); //PTC3 as CLKOUT
}
void rtc_init(void) 
{
    /*enable the clock to SRTC module register space*/
	SIM_SCGC6 |= SIM_SCGC6_RTC_MASK;

	/*Clear Registers*/
    RTC_CR  = RTC_CR_SWR_MASK;
    RTC_CR  &= ~RTC_CR_SWR_MASK;  
  
    if (RTC_SR & RTC_SR_TIF_MASK){
        RTC_TSR = 0x00000000;   //  this action clears the TIF
    }
    
    /*Set time compensation parameters*/
    RTC_TCR = RTC_TCR_CIR(1) | RTC_TCR_TCR(0xFF);
  
    /*Enable RTC seconds irq*/
   // enable_irq(INT_RTC_Seconds - 16);
    NVIC_ICPR |= 1 <<(21%32);
	NVIC_ISER |= 1 <<(21%32);		
    
    
    /*Enable Seconds Interrupt*/
    RTC_IER |= RTC_IER_TSIE_MASK; //Seconds interrupt enable
    
    /*Timer enable*/
    RTC_SR |= RTC_SR_TCE_MASK;
    
    /*Configure the timer seconds and alarm registers*/
    RTC_TSR = 0xFF;

}
void cfg_pwm (void)
{
	//Select the CLK for the TPM Module - page 196
	SIM_SOPT2 |= SIM_SOPT2_TPMSRC(1);
	SIM_SCGC6 = SIM_SCGC6_TPM1_MASK;
	SIM_SCGC6 = SIM_SCGC6_TPM2_MASK;

	// Selects the MCGFLLCLK clock 
	SIM_SOPT2 &= ~(SIM_SOPT2_PLLFLLSEL_MASK); 
	
	//Clear counter register - page 553
	TPM1_CNT = 0;
	
	//Set signal period to 1 ms
	TPM1_MOD = 20000;
	
	//See page 552 for TPMx_SC configuration
	//(freq = ?)
	TPM1_SC = 0x0B;			
	
	//See page 556 for TPMx_CnSC configuration
	TPM1_C0SC = 0x28;		//0010 1000
	
	TPM1_C0V = 0;
	

}
void Tpm_centesimas (void)
{
  if (TPM1_C0V == 0);
  {
	  segundos++;
	  if (segundos>9);
	  {
	   segundos1++;
	   segundos = 0;
	   	   if (segundos1 > 5);
	   	   {
		   minutos++;
		   segundos1 = 0;
		   	   if (minutos > 9);
		   	   {
		   		  minutos1++;
		   		  minutos=0;
		   		  	  if (minutos > 5);
		   		  	  {
		   			  horas++;
		   			  minutos1 = 0;
		   		  	  }
		   	   }
		   
	   	   }
	  }

  }
}
