#include <stdio.h>
#include <errno.h>

#include "tinysh.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "rw_flash.c"
#include "set_ipaddr.c"

#ifdef BOARD_TAMPILAN
#include "sumber.c"
#include "mesin.c"
#include "titik.c"
#endif

#ifdef BOARD_KOMON_A_RTD
#include "../adc/command_adc.c"
#include "set_kanal.c"
#endif

#include "enviro.h"

#include "../GPIO/gpio.h"
#include "../monita/monita_uip.h"

#include <stdlib.h>

void reset_cpu(void);
extern struct t2_konter konter;

extern struct t_mesin mesin[];
extern struct t_titik titik[];
extern struct t_sumber sumber[];

extern struct sambungan_state samb;

extern xTaskHandle *hdl_shell;
extern xTaskHandle *hdl_lcd;
extern xTaskHandle *hdl_led;
extern xTaskHandle *hdl_tampilan;
extern xTaskHandle *hdl_ether;

/*****************************************************************************/
// komand2 daun biru komon-kounter

#define per_oflow (0xFFFFFFFF * 0.050) / (1000 * 1000 * 60)
#define per_tik		0.050 / (1000 * 1000 * 60)


static tinysh_cmd_t save_env_cmd={0,"saveenv","menyimpan environment","[args]",
                              save_env,0,0,0};
							  
static tinysh_cmd_t printenv_cmd={0,"printenv","menampilkan environment","[args]",
                              print_env,0,0,0};
							  
static tinysh_cmd_t reset_cmd={0,"reset","reset cpu saja","[args]",
                              reset_cpu,0,0,0};
							  
static tinysh_cmd_t defenv_cmd={0,"defenv","set default environment","[args]",
                              getdef_env,0,0,0};
							 
void cek_stack(void)
{
	printf("Sisa stack masing2 task (bytes)\r\n");
	garis_bawah();
	printf(" Shell    : %d\r\n", uxTaskGetStackHighWaterMark(hdl_shell));
	printf(" Led      : %d\r\n", uxTaskGetStackHighWaterMark(hdl_led));
	
	#ifdef BOARD_TAMPILAN
	printf(" Tampilan : %d\r\n", uxTaskGetStackHighWaterMark(hdl_tampilan));
	printf(" LCD      : %d\r\n", uxTaskGetStackHighWaterMark(hdl_lcd));
	#endif
	
	printf(" Ether    : %d\r\n", uxTaskGetStackHighWaterMark(hdl_ether));
	
}							 

static tinysh_cmd_t cek_stack_cmd={0,"cek_stack","data kounter/rpm","[args]",
                              cek_stack,0,0,0};

void cek_versi(void)
{
#ifdef BOARD_KOMON
  	printf(" Babelan Komon-Counter %s\r\n", VERSI_KOMON);
#endif

#ifdef BOARD_TAMPILAN
  	printf(" Babelan Tampilan %s\r\n", VERSI_TAMPILAN);
#endif

#ifdef BOARD_KOMON_A_RTD
  	printf(" Babelan Komon-A (RTD & 4-20mA) v%s\r\n", VERSI_KOMON);
#endif

  	printf(" ARM-GCC %s : %s : %s\r\n", __VERSION__, __DATE__, __TIME__);
  	printf(" CPU = LPC 2368, %d MHz,", configCPU_CLOCK_HZ/1000000);
  	printf(" FreeRTOS 5.1.1\r\n");	
}							 

static tinysh_cmd_t version_cmd={0,"version","menampilkan versi firmware","[args]",
                              cek_versi,0,0,0};
							  


unsigned int uptime_ovflow=0;
unsigned int tik_lama=0;

void uptime(unsigned int *sec, unsigned int *min, unsigned int *jam, unsigned int *hari, unsigned int *thn)
{
	unsigned int detik;
	unsigned int tik;
	
	tik = xTaskGetTickCount();
	if (tik < tik_lama)
			uptime_ovflow++;
	
	tik_lama = tik;
	
	detik = ((uptime_ovflow * 0xFFFFFFFF) + tik)/1000;	// port tick HZ
	
	*sec = detik % 60;
	*min = detik / 60;
	if (*min > 60) 
		*min = *min % 60;
	
	*jam = detik / 3600;
	if (*jam >= 24)
		*jam = *jam % 24;
	
	*hari = detik / 86400;
	if (*hari >= 365)
		*hari = *hari % 365;
	
	*thn = detik / (8640 * 365);
	
}

static void cek_uptime(int argc, char **argv)
{
	unsigned int sec;
	unsigned int menit;
	unsigned int jam;
	unsigned int hari;
	unsigned int tahun;
	
	extern unsigned int tot_idle;
	
	uptime(&sec, &menit, &jam, &hari, &tahun);
	printf(" Up = ");
	if (tahun !=0)
	{
		printf("%d thn ", tahun);	
	}
	if (hari !=0)
	{
		printf("%d hari ", hari);		
	}
	if (jam !=0)
	{
		printf("%d jam ", jam);		
	}
	if (menit !=0)
	{
		printf("%d mnt ", menit);		
	}
		
	printf("%d dtk : idle = %d\n", sec, tot_idle);
		
	return ;
}

static tinysh_cmd_t uptime_cmd={0,"uptime","lama running","[args]",
                              cek_uptime,0,0,0};


#ifdef BOARD_KOMON
static void cek_rpm(int argc, char **argv)
{
	unsigned int i;
	float uptime;

	printf("Global hit = %d\n", konter.global_hit);
	printf("Ov flow = %d\n", konter.ovflow);

	for (i=0; i<10; i++)
	{
		printf(" %2d : beda = %12d ns, hit = %d\n", (i+1), konter.t_konter[i].beda, konter.t_konter[i].hit);

	}
	uptime = (float) konter.ovflow * per_oflow;
	i = T1TC;
	uptime = uptime + (i * (float) per_tik);
	if (uptime > 60)
	{
		uptime = uptime / 60;
		printf("Uptime = %.3f jam\n", uptime);
	}
	else
		printf("Uptime = %.3f menit\n", uptime);
}

static tinysh_cmd_t cek_rpm_cmd={0,"cek_rpm","data kounter/rpm","[args]",
                              cek_rpm,0,0,0};

#endif
/*****************************************************************************/
 
void display_args(int argc, char **argv)
{
  int i;
  for(i=0;i<argc;i++)
    {
      printf(" argv[%d]=\"%s\"\n",i,argv[i]);
    }
}

#ifdef CONTOH_SHELL
static void foo_fnt(int argc, char **argv)
{
  printf("foo command called\n");
  display_args(argc,argv);
}

static tinysh_cmd_t myfoocmd={0,"foo","foo command","[args]",
                              foo_fnt,0,0,0};

static void item_fnt(int argc, char **argv)
{
  printf("item%d command called\n",(int)tinysh_get_arg());
  display_args(argc,argv);
}

static tinysh_cmd_t ctxcmd={0,"ctx","contextual command","item1|item2",
                            0,0,0,0};
static tinysh_cmd_t item1={&ctxcmd,"item1","first item","[args]",item_fnt,
                           (void *)1,0,0};
static tinysh_cmd_t item2={&ctxcmd,"item2","second item","[args]",item_fnt,
                           (void *)2,0,0};

static void reset_to_0(int argc, char **argv)
{
  int *val;
  val=(int *)tinysh_get_arg();
  *val=0;
}

static void atoxi_fnt(int argc, char **argv)
{
  int i;

  for(i=1;i<argc;i++)
    {
      printf("\"%s\"-->%u (0x%x)\n",
             argv[i],tinysh_atoxi(argv[i]),tinysh_atoxi(argv[i]));
    }
}

static tinysh_cmd_t atoxi_cmd={0,"atoxi","demonstrate atoxi support",
                               "[args-to-convert]",atoxi_fnt,0,0,0};

xSemaphoreHandle bg_sem;

static void bg_cmd_fnt(int argc, char **argv)
{
  printf("bg_cmd command called\n");
  xSemaphoreGive( bg_sem );
}

static tinysh_cmd_t bgcmd={0,"bg_cmd","bg_cmd command","[args]",
                              bg_cmd_fnt,0,0,0};

portTASK_FUNCTION( bg_cmd_thread, pvParameters )
{
	vSemaphoreCreateBinary( bg_sem );
    xSemaphoreTake( bg_sem, 0 );
	for(;;) {
	    while( xSemaphoreTake( bg_sem, 10000 ) != pdPASS );
	    vTaskDelay( 5 );
		printf("**** this is a back ground task *****\n");
	}
}
#endif // contoh shell

portTASK_FUNCTION(shell, pvParameters )
{
  	int c;
  	xTaskHandle xHandle;

#ifdef BOARD_KOMON
  	printf("\nStarting Babelan Komon-Counter %s\r\n", VERSI_KOMON);
#endif

#ifdef BOARD_TAMPILAN
  	printf("\nStarting Babelan Tampilan %s\r\n", VERSI_TAMPILAN);
#endif

#ifdef BOARD_KOMON_A_RTD
  	printf("\nStarting Babelan Komon-A (RTD & 4-20mA) v%s\r\n", VERSI_KOMON);
#endif

  	printf("Daun Biru Engineering, Des 2008\r\n");
  	printf("=========================================\r\n");
  	printf("ARM-GCC %s : %s : %s\r\n", __VERSION__, __DATE__, __TIME__);
  	printf("CPU = LPC 2368, %d MHz,", configCPU_CLOCK_HZ/1000000);
  	printf(" FreeRTOS 5.1.1\r\n");

	if (configUSE_PREEMPTION == 0)
		printf("NON Preemptive kernel digunakan !\r\n"); 
	else
		printf("Preemptive kernel digunakan !\r\n");
	
	/* 
	 * add command
	 */
  	//tinysh_add_command(&myfoocmd);
  	tinysh_add_command(&printenv_cmd);
	tinysh_add_command(&setenv_cmd);
	tinysh_add_command(&save_env_cmd);
	tinysh_add_command(&reset_cmd);
	tinysh_add_command(&cek_stack_cmd);
	tinysh_add_command(&defenv_cmd);
	tinysh_add_command(&uptime_cmd);
	tinysh_add_command(&version_cmd);
	
#ifdef BOARD_KOMON
	tinysh_add_command(&cek_rpm_cmd);
#endif

#ifdef BOARD_TAMPILAN
	tinysh_add_command(&cek_sumber_cmd);
	tinysh_add_command(&set_sumber_cmd);
	tinysh_add_command(&cek_mesin_cmd);
	tinysh_add_command(&set_mesin_cmd);
	tinysh_add_command(&cek_titik_cmd);
	tinysh_add_command(&set_titik_cmd);
	
	tinysh_add_command(&save_sumber_cmd);
	tinysh_add_command(&save_mesin_cmd);
	tinysh_add_command(&save_titik_cmd);
#endif

#ifdef BOARD_KOMON_A_RTD
	tinysh_add_command(&cek_adc_cmd);
	tinysh_add_command(&set_kanal_cmd);
#endif

	/* add sub commands
 	*/
  	//tinysh_add_command(&ctxcmd);
  	//tinysh_add_command(&item1);
  	//tinysh_add_command(&item2);

	/* use a command from the stack
 	* !!! this is only possible because the shell will have exited
 	* before the stack of function main !!!
 	*/
  	/*
	{
    	tinysh_cmd_t quitcmd={0,"quit","exit shell",0,reset_to_0,
                          (void *)&again,0,0};
    	tinysh_add_command(&quitcmd);
  	}*/

	/* add atoxi support test command */
  	//tinysh_add_command(&atoxi_cmd);



	/* add a background command */
  	//tinysh_add_command(&bgcmd);
  	//xTaskCreate( bg_cmd_thread, "bg_cmd", 1000, NULL, 2, &xHandle);

  	vTaskDelay(450);

	#ifdef BOARD_TAMPILAN	
	// cek ukuran struk
	printf("size struct Mesin  = %d\r\n", sizeof (struct t_mesin) * JML_MESIN);
	printf("size struct Sumber = %d\r\n", sizeof (struct t_sumber) * JML_SUMBER);
	printf("size struct Titik  = %d\r\n", sizeof (struct t_titik) * JML_MESIN * TIAP_MESIN);
	//printf("size struct sambungan = %d\r\n", sizeof (samb));
	#endif
	
	#ifdef BOARD_BABELAN
  	tinysh_set_prompt("Babelan $ ");
	#endif
	
	#ifdef BOARD_KOMON
  	tinysh_set_prompt("Komon $ ");
	#endif
	
	#ifdef BOARD_TAMPILAN
  	tinysh_set_prompt("Tampilan $ ");
	#endif
	
	#ifdef BOARD_KOMON_A_RTD
	//spiInit();
	/*
	c = cek_adc_id();
	c = c & 0xF0;
	
	if (c == 0x50)
		printf("ADC AD7708 OK = 0x%X\r\n", c);
	else
		printf("ADC ERR 0x%X ... tidak terdeteksi !\r\n", c);
	
	tinysh_set_prompt("Komon_A $ ");
	*/
	kalibrasi_adc1();
	vTaskDelay(100);
	start_adc_1();
	tinysh_set_prompt("Komon_A $ ");
	#endif
	
	/* 
	 * main loop shell
  	 */
  	
  	while(1)
    {
	  if (xSerialGetChar(1, &c, 0xFFFF ) == pdTRUE)
	  	tinysh_char_in((unsigned char)c);
    }
  	
  	return;
}

void init_shell(void)
{
	xTaskCreate( shell, "UsrTsk1", (configMINIMAL_STACK_SIZE * 8), NULL, 1, ( xTaskHandle * ) &hdl_shell);
}
