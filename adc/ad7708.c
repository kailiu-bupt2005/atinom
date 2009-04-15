/*
	AD7708
	furkan jadid, daun biru engineering
	Jakarta, 8 Okt 2007
	
	15 april 2009
	porting untuk freeRTOS & LPC2368
	
	*/

#include "FreeRTOS.h"
#include "ad7708.h"

#define rate_7708		71			// 20 data per detik

unsigned int kanal_aktif;
unsigned int kalibrated;

struct t_adc st_adc;

#include "kalibrasi_adc.c"

inline void cs_ad7708(void);
inline void uncs_ad7708(void);
/*
void init_spi_ad7708(void)	
{
	unsigned int sclk;
	unsigned short baute;
	
	*pSPI_CTL = (MSTR + CPHA + CPOL + 0x01);
	ssync();
   sclk = get_sclk();  
   //baute = (unsigned short) (sclk / (2 * 5000000));		// 5 MHz
   baute = (unsigned short) (sclk / (2 * 20000000));		
   
   printku("\n\r baut %d ", baute);
   
   *pSPI_BAUD = baute;
   ssync();
   *pSPI_FLG = 0xFF00;
   ssync();
   *pSPI_CTL = (SPE + MSTR + CPHA + CPOL + 0x01); 	//start saat read tdbr
   ssync();
}
*/

#if BLACKFIN
inline void cs_ad7708(void)
{
	*pFIO_FLAG_C = port_cs_ad7708;
	ssync();
}

inline void uncs_ad7708(void)
{
	*pFIO_FLAG_S = port_cs_ad7708;
	ssync();
}
#endif

#ifdef BOARD_KOMON_A_RTD

#define kirim_word	spiPut
#define ambil_word	spiPut

#define SPI_SPCCR	S0SPCCR
#define SPI_SPCR	S0SPCR
#define SPI_SPSR	S0SPSR
#define SPI_SPDR	S0SPDR
#define SPI_SPIF	0x80		// bit 7 status
#define MSTR		0x20		// bit 5
#define CPOL		0x10
#define CPHA		0x08

inline void cs_ad7708(void)
{
	portENTER_CRITICAL();
	SPI_SPCR = MSTR | CPOL;
	FIO1CLR = port_cs_ad7708;
}

inline void uncs_ad7708(void)
{
	FIO1SET = port_cs_ad7708;
	portEXIT_CRITICAL();
}

inline spi_rx(void)
{
	return ;
}
#endif

unsigned char cek_status(void)
{
	unsigned char s;
	//s = ambil_word();
   cs_ad7708();
	kirim_word((m_read | reg_status));
	s = spi_rx();					//dummy read
	s= ambil_word();
	uncs_ad7708();
	return s;
}

inline unsigned short baca_data(void)
{
	//unsigned char a;
	
	unsigned char b;
	unsigned short s;
	
	//b = ambil_word();
	
	cs_ad7708();
	kirim_word(m_read | reg_data);
	s = spi_rx();					//dummy read
	s = ambil_word();
	b = ambil_word();
	
	s = ((s << 8) + b);
	uncs_ad7708();
	
	return s;
}

unsigned char set_iocon(unsigned char t)
{
	cs_ad7708();
	kirim_word(m_write | reg_iocon);
	kirim_word(t);
	uncs_ad7708();
	return 1;
}

unsigned char set_filter(unsigned char t)
{
	cs_ad7708();
	kirim_word(m_write | reg_filter);
	kirim_word(t);
	//kirim_word(250);
	uncs_ad7708();
	return 1;
}

unsigned char set_adccon(unsigned char t)
{
	cs_ad7708();
	kirim_word(m_write | reg_control);
	//kirim_word(7);		//
	kirim_word(t);
	uncs_ad7708();
	return 1;

}

unsigned char set_mode(unsigned char t)
{
	cs_ad7708();
	kirim_word(m_write | reg_mode);
	kirim_word(t);			//continus conversion
	uncs_ad7708();
	
	return 1;
}


//9 oktober

unsigned char cek_iocon(void)
{
	unsigned char s;
   cs_ad7708();
	kirim_word(m_read | reg_iocon);
	s = spi_rx();		//dummy, hanya mengosongkan RDBR
	s = ambil_word();
	uncs_ad7708();
	return s;
}

unsigned char cek_adccon(void)
{
	unsigned char s;
   cs_ad7708();
	kirim_word(m_read | reg_control);
	s = spi_rx();	//dummy, hanya mengosongkan RDBR
	s = ambil_word();
	uncs_ad7708();
	return s;
}

unsigned char cek_mode(void)
{
	unsigned char s;
   cs_ad7708();
	kirim_word(m_read | reg_mode);
	s = spi_rx();	//dummy, hanya mengosongkan RDBR
	s = ambil_word();
	uncs_ad7708();
	return s;
}

unsigned char cek_filter(void)
{
	unsigned char s;
   	cs_ad7708();
	kirim_word(m_read | reg_filter);
	s = spi_rx();	//dummy, hanya mengosongkan RDBR
	s = ambil_word();
	uncs_ad7708();
	return s;
}

void stop_ad7708(void)
{
	cs_ad7708();
	kirim_word(m_write | reg_mode);
	kirim_word(1);
	
	//ditambah 14 Nov 2007
	uncs_ad7708();
}

void stop_adc(void)
{
	set_mode(1);	
	
	return;
}

unsigned char set_calibrated(unsigned char c)
{
	portENTER_CRITICAL();
	
	kalibrated = c;
	
	portEXIT_CRITICAL();
}

void reset_adc(void)
{
	int i;
	cs_ad7708();	
	for (i=0; i<6; i++) kirim_word(0xFF);
	uncs_ad7708();
}

unsigned char cek_adc_id(void)
{
	unsigned char r;
	
	
	cs_ad7708();
	kirim_word(m_read | 0x0F);
	//spiPut(m_read | 0x0F);
	//r = spiPut(0x00);
	r = ambil_word();
	uncs_ad7708();
	
	
	return r;
}
inline unsigned int cek_adc_rdy(void)
{
	/* jika low, maka ada data baru */
	
	if (FIO1DIR & port_rdy_ad7708) return 0;
	else return 1;
}
