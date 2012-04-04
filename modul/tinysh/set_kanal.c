/*
	command line untuk setting m & C pada kanal
	untuk persamaan 
	
	y = mx + C
	
	15 april 2009
	furkan jadid, daun biru engineering
	
	sintak :
	
	set_kanal A B C
	
	A untuk nomer kanal 
	B untuk m
	C untuk C
	
	misalnya : set_kanal 3 0.34 -34
	
	*/

//extern struct t_env env2;
#include "enviro.h"

void set_kanal(int argc, char **argv)	{
	unsigned char str_kanal[24];
	unsigned int kanal;
	float m;
	float c;
	int ret;
	
	if (argc < 3) 	{
		if (argc > 1)	{
			if (strcmp(argv[1], "help") == 0)	{
				printf(" setting faktor kalibrasi kanal dengan y = mx + c, misalnya : \r\n");
				printf(" set_kanal 3 1.34 -0.431\r\n");
				printf(" artinya : setting kanal 3 dengan m = 1.34 dan C = -0.431\r\n");
				printf(" \r\n");
				printf(" set_kanal juga bisa untuk setting keterangan, misalnya :\r\n");
				printf(" set_kanal 3 ket jacket_water_temp\r\n");
				printf(" artinya : setting kanal 3 dengan keterangan 'jacket_water_temp'\r\n");
				printf(" set_kanal 3 status [onoff|1|0], default: 0 (Tegangan)\r\n");
				printf(" set_kanal 3 status 1\r\n");
				printf(" artinya : setting kanal 3 dengan status OnOff\r\n");
				return;
			} 
			else if (strcmp(argv[1], "default") == 0)	{
				printf("set kanal dengan data default !\n");
				//set_dafault_kalib();
				set_env_default();
				return;
			}	
		}
		printf(" ERR: argument kurang !\r\n");
		printf(" coba set_kanal help \r\n");
		printf(" format lain: set_kanal [help|ket|status]\r\n");
		return;	
	}
	
	struct t_env *p_sbr;
	p_sbr = pvPortMalloc( sizeof (struct t_env) );
	memcpy((char *) p_sbr, (char *) ALMT_ENV, (sizeof (struct t_env)));
	
	if (p_sbr == NULL) {
		printf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
		return -1;
	}
	
	printf(" set_kanal untuk kanal %s dipanggil\r\n", argv[1]);
	garis_bawah();
  	display_args(argc,argv);
  	
  	sprintf(str_kanal, "%s", argv[1]);
	ret = sscanf(str_kanal, "%d", &kanal);
	
	if (kanal > KANALNYA || ret == NULL) {
		vPortFree( p_sbr );
		printf(" Err kanal !\r\n");
		return ;
	}
	
	if (strcmp(argv[2], "ket") == 0)  	{
  		printf(" Setting keterangan kanal %d :\r\n", kanal);
  		sprintf(p_sbr->kalib[kanal-1].ket, "%s", argv[3]);
  		printf(" %s", p_sbr->kalib[kanal-1].ket);
  	}
  	else if (strcmp(argv[2], "status") == 0)  	{
		char w=0;
  		printf(" Setting status kanal %d :\r\n", kanal);
  		//*
  		w = argv[3][0] - '0';
  		if (w>0 && w<5) {
			//if ( (kanal%2) && (kanal<10) )	
			{
				p_sbr->kalib[kanal-1].status = w;
				#ifdef PAKAI_PILIHAN_FLOW
					if (w==sFLOW1)
						p_sbr->kalib[kanal].status = nFLOW1;		// ket ini @ tinysh/enviro.h
					else if (w==sFLOW2)	{
						p_sbr->kalib[kanal].status   = ssFLOW2;
						p_sbr->kalib[kanal+1].status = nFLOW2;
						p_sbr->kalib[KANALNYA-1].status = fFLOW;
					}
				#endif
			}
		}
		#ifdef PAKAI_ADC_ORI
		else if (w==sADC_ORI)	{
			p_sbr->kalib[kanal-1].status = w;
			if (argc==5)	{
				p_sbr->kalib[kanal-1].adc = argv[4][0] - '0';
				printf("kanal %d: input ADC ke %d\r\n", kanal, p_sbr->kalib[kanal-1].adc);
			}
		}
		#endif
		else if ( strcmp(argv[3], "onoff")==0) {
			p_sbr->kalib[kanal-1].status = sONOFF;			// ket ini @ tinysh/enviro.h
		}
		#ifdef PAKAI_PUSHBUTTON
		else if ( strcmp(argv[3], "toogle")==0) {
			p_sbr->kalib[kanal-1].status = sPUSHBUTTON;
		}
		#endif
		#ifdef PAKAI_PILIHAN_FLOW
		else if ( strcmp(argv[3], "flow")==0) {
			p_sbr->kalib[kanal-1].status = sFLOW1;
			p_sbr->kalib[kanal].status = nFLOW1;
		}
		else if ( strcmp(argv[3], "flow2")==0) {
			p_sbr->kalib[kanal-1].status = sFLOW2;
			p_sbr->kalib[kanal].status   = ssFLOW2;
			p_sbr->kalib[kanal+1].status = nFLOW2;
			p_sbr->kalib[KANALNYA-1].status = fFLOW;
		}
		#endif
		#ifdef PAKAI_ADC_ORI
		else if ( strcmp(argv[3], "adcori")==0) {
			p_sbr->kalib[kanal-1].status = sADC_ORI;
		}
		#endif
		else if ( strcmp(argv[3], "rpm")==0) {
			p_sbr->kalib[kanal-1].status = 0;
		} else {
			p_sbr->kalib[kanal-1].status = 0;
		}
		
		//(int) setup_konter_onoff((unsigned int) 2, (unsigned char)0);
		
  	}
  	else  	{
		sprintf(str_kanal, "%s", argv[2]);
		ret = sscanf(str_kanal, "%f", &m);
	
		if (ret == NULL) {
			printf(" Err m !\r\n"); 
			vPortFree( p_sbr );
			return ;
		}
	
		sprintf(str_kanal, "%s", argv[3]);
		ret = sscanf(str_kanal, "%f", &c);
	
		if (ret == NULL) {
			printf(" Err C !\r\n"); 
			vPortFree( p_sbr );
			return ;
		}
	
		printf(" Seting kanal %d, m = %f, dan C = %f\r\n", kanal, m, c);
		p_sbr->kalib[kanal - 1].m = m;
		p_sbr->kalib[kanal - 1].C = c;
		
	}
	
	if (simpan_env( p_sbr ) < 0) {
		vPortFree( p_sbr );
		return -1;
	}
	vPortFree( p_sbr );
}

// static
tinysh_cmd_t set_kanal_cmd={0,"set_kanal","setting kanal","help", set_kanal,0,0,0};	
