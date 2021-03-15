//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <SI_C8051F300_Register_Enums.h>                // SFR declarations

//P2 default output low
//P4 latch 1

//-----------------------------------------------------------------------------
// SiLabs_Startup() Routine
// ----------------------------------------------------------------------------
// This function is called immediately after reset, before the initialization
// code is run in SILABS_STARTUP.A51 (which runs before main() ). This is a
// useful place to disable the watchdog timer, which is enable by default
// and may trigger before main() in some instances.
//-----------------------------------------------------------------------------
void SiLabs_Startup (void)
{
	PCA0MD &= ~0x40; // Disable the watchdog here
}


INTERRUPT_PROTO(Timer2_ISR, 5);
void Timer2_Init(void);
void Timer2_Disable(void);
void Timer2_Enable(void);
void Init_Device(void);
void Interrupts_Init(void);
unsigned char CheckP0B4PressTime(unsigned short Min, unsigned short Max);
void wait_ms(unsigned short m_second);
void Set_Timer0(void);
void Start_Stop_timer(void);
unsigned char PWM_toggle(void);
unsigned char calculate_pwm_frequency (void);

#define  POWEROFF     0x00
#define  STARTUP      0x01
#define  IDLE         0x02
#define  BOOT_FAIL    0x03

unsigned char BootStage;
unsigned char BootFlag;

unsigned char ResetTimes;

unsigned short timer2recording = 0x0000;
unsigned char TL0Temp = 0x00;
unsigned char TH0Temp = 0x00;

//-----------------------------------------------------------------------------
// main() Routine
// ----------------------------------------------------------------------------
// Note: the software watchdog timer is not disabled by default in this
// example, so a long-running program will reset periodically unless
// the timer is disabled or your program periodically writes to it.
//
// Review the "Watchdog Timer" section under the part family's datasheet
// for details. To find the datasheet, select your part in the
// Simplicity Launcher and click on "Data Sheet".
//-----------------------------------------------------------------------------
int main (void)
{
	Init_Device();
	Timer2_Init();
	Timer2_Disable();
	Interrupts_Init();

	BootFlag = 0x01;
	BootStage = STARTUP;

	while (1)
	{
		switch(BootStage)
		{
		case STARTUP:
			if(BootFlag == 0x01)
			{
				if(CheckP0B4PressTime(16,500))
				{
					BootFlag = 0x02;
					Timer2_Enable();
				}
			}
			else if(BootFlag == 0x02)
			{
				if(!P0_B6)
				{
					while(!P0_B6)
					{
						if(timer2recording > 50)
						{
							BootFlag = 0x01;
							BootStage = BOOT_FAIL;
							Timer2_Disable();
							break;
						}
					}
				}

				if((P0_B6) && (BootFlag == 0x02))
				{
					BootFlag = 0x03;
					Timer2_Disable();
					Timer2_Enable();
				}
			}
			else if(BootFlag == 0x03)
			{
				if(timer2recording > 550)
				{
					BootFlag = 0x01;
					BootStage = BOOT_FAIL;
					Timer2_Disable();
					break;
				}

				if(!P0_B6)
				{
					BootFlag = 0x01;
					BootStage = BOOT_FAIL;
					Timer2_Disable();
					break;
				}

				//if(calculate_pwm_frequency() ==1)
				if(!P0_B7)
				{
					BootFlag = 0x11;
					BootStage = IDLE;
					Timer2_Disable();
				}
			}
			break;
		case IDLE:
#if 0
			if(BootFlag == 0x11)
			{
				if(!P0_B6)
				{
					while(!P0_B6)
					{
						if(CheckP0B4PressTime(16,500))
						{
							BootFlag = 0x02;
							BootStage = STARTUP;
							Timer2_Enable();
							break;
						}
					}

					if((P0_B6) && (BootFlag == 0x11))
					{
						BootFlag = 0x12;
					}
				}
			}
			else if(BootFlag == 0x12)
			{
				BootFlag = 0x03;
				BootStage = STARTUP;
				Timer2_Enable();
			}
#endif
			break;
		case BOOT_FAIL:
			P0_B3=0;
			wait_ms(5);
			P0_B2=1;
			wait_ms(5);
			P0_B2=0;
			wait_ms(5);
			P0_B3=1;
			wait_ms(10);
			P0_B0=0;
			wait_ms(100);
			P0_B0=1;
			BootStage = STARTUP;
			BootFlag = 0x01;
			break;
		case POWEROFF:
			break;
		}

	}                             // Spin forever
}

unsigned char CheckP0B4PressTime(unsigned short Min, unsigned short Max)
{
	unsigned short checkloop = 0;

	for(checkloop=0;checkloop<=0xffff;checkloop++)
	{
		if(!P0_B4)  wait_ms(1);
		else break;
	}

	if((checkloop > Min) && (checkloop < Max))
		return 1;

	return 0;
}

/*----------------------------------------------------------------------*/
/* Calculate PWM frequency 1KHz*/
// 255KHz Sample Rate, So 200-300 conters
unsigned char calculate_pwm_frequency ()
{
#if 0
	unsigned short timercount = 0x0000;
	Set_Timer0();
	Start_Stop_timer();
	timercount = (TH0Temp<<8)| TL0Temp;

	if(timercount>= 232 && timercount <= 283) return 1;		// 1KHz range 0.9KHz~1.1KHz
	else if(timercount>= 120 && timercount <= 160) return 2;	// 1.82KHz	1.60KHz~2.13KHz
	else if (timercount >=77 && timercount <=94 ) return 3;		// about 2.7KHz~3.3KHz
	else if(timercount >=23 && timercount <=28 ) return 10;		// about 9KHz~11Hz for USB reset
	else return false;									// other false
#endif
	return 1;
}

void Set_Timer0()
{
		TMOD &= ~0x0F;
		TMOD |=  0x01;
		CKCON |= 0x04;
		TCON_TR0=0;
		TCON_TF0=0;
 		TH0=0;
		TL0=0;

}

void Start_Stop_timer()
{
	if(PWM_toggle())
	{

		TCON_TR0=1;

		if(PWM_toggle())
		{
			TCON_TR0=0;
			TL0Temp= TL0;
			TH0Temp= TH0;
		}
	}
}

unsigned char PWM_toggle()
{
	unsigned long i,j;

	for(i=0;i<=0xfff;i++)
	{
		if(!P0_B7)break;
		i+=1;
	}

	for(j=0;j<=0xfff;j++)
	{
		if(P0_B7)break;
		j+=1;
	}
	return true;
}

void Interrupts_Init()
{
	IE_EA = 1;
	IE_ET2 = 1;
}
// Peripheral specific initialization functions,
// Called from the Init_Device() function
void Port_IO_Init()
{
    // P0.0  -  Unassigned,  Push-Pull,  Digital
    // P0.1  -  Unassigned,  Open-Drain, Digital
    // P0.2  -  Unassigned,  Push-Pull,  Digital
    // P0.3  -  Unassigned,  Push-Pull,  Digital
    // P0.4  -  Unassigned,  Open-Drain, Digital
    // P0.5  -  Unassigned,  Push-Pull,  Digital
    // P0.6  -  Unassigned,  Open-Drain, Digital
    // P0.7  -  Unassigned,  Open-Drain, Digital

    P0MDOUT   |= 0x2D;
    P0        &= ~0x04;
    XBR2      = 0xC0;
}

// Initialization function for device,
// Call Init_Device() from your main program
void Init_Device(void)
{
    Port_IO_Init();
}

/*----------------------------------------------------------------------*/
/* ms delay routine */
void wait_ms(unsigned short m_second)
{

	TCON &= ~0xC0;
    TMOD &= ~0xF0;
    TMOD |=  0x10;
    CKCON |= 0x10;

   	while (m_second)
	{
   		TCON_TR1 = 0;
   		TH1 = 0xF4;
   		TL1 = 0x09;
   		TCON_TF1 = 0;		/* Clear overflow indicator*/
   		TCON_TR1= 1;
   		while (!TCON_TF1)
   		{
   			_nop_();
   		}
   		m_second--;
	}
    //TR1 = 0;
   	TCON_TR1 = 0;			/* Stop Timer1 */
}


/*
void Timer2_Init(void)
{
   CKCON &= ~0x60;                     // Timer2 uses SYSCLK/12
   TMR2CN &= ~0x01;

   TMR2RL = TIMER2_RELOAD;             // Reload value to be used in Timer2
   TMR2 = TMR2RL;                      // Init the Timer2 register

   TMR2CN = 0x04;                      // Enable Timer2 in auto-reload mode
   IE_ET2 = 1;                            // Timer2 interrupt enabled
}
*/

void Timer2_Init(void)
{
	TMR2CN = 0x00;		// 16bit自动装载方式
	TMR2RLL = 0x4F;		// 设定100ms
	TMR2RLH	= 0x9C;

	TMR2L = 0x4F;
	TMR2H =	0x9C;

	//TR2 = 1;
	TMR2CN_TR2 = 0;				// enable timer2
}

void Timer2_Disable(void)
{

	//timer2recording = 0x0000;	//clear count
	//TR2 = 0;
	TMR2CN_TR2 = 0;				// Disable timer2
	timer2recording = 0x0000;
}

void Timer2_Enable(void)
{
	timer2recording = 0x0000;
	//TR2 = 1;
	TMR2CN_TR2 = 1;				// Enable timer2
	//ET2 = 1;
	IE_ET2 = 1;				// Enable Timer2 interrupt
	//EA = 1;					// Enable All interrupt control
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Timer2_ISR
//-----------------------------------------------------------------------------
//
// Here we process the Timer2 interrupt and toggle the LED
//
//-----------------------------------------------------------------------------
INTERRUPT_PROTO(Timer2_ISR, 5)
{
   TMR2CN_TF2H = 0;                           // Reset Interrupt
   timer2recording++;
}
