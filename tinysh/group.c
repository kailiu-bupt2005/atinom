/*
	cek dan setting group
	21 Jan 2010
	furkan jadid
	daun biru engineering
	
	Untuk menghemat ruang, akses kepada struct group
	lewat pointer langsung ke flash saja.
	
	Nama group akan dijadikan judul tab pada tampilan
	
	23 Jan 2010
	perintah2 pada group :
	
	set_group x nama aaabbbccc
	set_group x set/aktif	0/1
	set_group x desc/ket aaabbbcc			: deskripsi group 
	set_group default
	
	cek_group
	
	ID : Nama       : Status : Keterangan
	==============================================
	 1 : Mesin_1    : Aktif  : SWD 6 TM 410
	
	3 Feb 2010
	setiap group ada kemungkinan 40 data yang ditampilkan
	dengan cara di scrol ke bawah, atau nanti dengan grafik
	
	jika ada 10 group, maka diperlukan memory 
	
	10 * 40 * 2 byte = 800 byte
	
*/

#include "../monita/monita_uip.h"

int tulis_flash(int dst, unsigned short *data, int pjg);
void judul(char *s);
static int set_group_default(void);
static int simpan_group( struct t_group *pgr);

struct t_group *pgroup;
//struct t_mesin *pmesin;

int cek_group(int argc, char **argv)
{
	int i;
	
	pgroup = (char *) ALMT_MESIN;
	
	judul(" Group Setting\r\n");
	printf(" ID  : Nama       : Status : Keterangan : &Memory\r\n");
	garis_bawah();
	
	for (i=0; i<10; i++)
	{
		printf(" (%2d): %10s :    %d   : %10s : (%X)\r\n", pgroup[i].ID_group, \
			pgroup[i].nama, pgroup[i].stat, pgroup[i].ket, &pgroup[i]);	
	}
	
	
}

static tinysh_cmd_t cek_group_cmd={0,"cek_group","menampilkan konfigurasi mesin","[] nomer",
                              cek_group,0,0,0};

int set_group(int argc, char **argv)
{
	unsigned char buf[24];
	int sumb=0;
	unsigned int ret_ip;
	struct t_group *p_gr;
	
	judul(" Setting Group\r\n");
	
	if (argc < 4) 
	{
		if (argc > 1)
		{
			if (strcmp(argv[1], "help") == 0)
			{
				printf(" Perintah untuk setting group !\r\n");
				printf(" 1. set_group help/default\r\n");
				printf("    help    : printout keterangan ini\r\n");
				printf("    default : memberikan default setting group\r\n");
				printf("\r\n");
				
				printf(" 2. set_group x [opt1] [opt2]\r\n");
				printf("    x    : nomer group\r\n");
				printf("    opt1 : nama, set/aktif, desc/ket\r\n");
				printf("\r\n");
				printf("    [opt1]\r\n");				
				printf("    nama     : memberikan nama group yang akan ditampilkan\r\n");
				printf("    misalnya : $ set_group 2 nama GMT_#4\r\n");
				printf("    artinya memberikan nama group 1 dengan GMT_#4\r\n");
				printf("\r\n");
				printf("    set/aktif : mengaktif/nonaktifkan group di display\r\n");
				printf("    misalnya  : $ set_group 4 aktif 1\r\n");
				printf("    artinya mengaktifkan group 4\r\n");
				printf("    misalnya  : $ set_group 4 aktif 0\r\n");
				printf("    artinya me-nonaktifkan group 4\r\n");
				printf("\r\n");
				printf("    desc/ket  : memberikan keterangan/deskripsi pada group\r\n");
				printf("    misalnya  : $ set_group 7 ket ini_milik_Sulzer\r\n");
				printf("    catatan   : jangan ada spasi pada keterangannya !\r\n");
				
				return ;
			}
			else if (strcmp(argv[1], "default") == 0)
			{
				printf("set GROUP dengan data default !\n");
				set_group_default();
				
				return;
			}		
		}
		printf(" ERR: argument kurang !\r\n");
		printf(" coba set_group help \r\n");
		return;	
	}
	
  	display_args(argc,argv);
	
	/* copy dulu yang lama kedalam buffer */
	p_gr = pvPortMalloc( 10 * sizeof (struct t_group) );
	
	if (p_gr == NULL)
	{
		printf(" %s(): ERR allok memory gagal !\r\n", __FUNCTION__);
		return -1;
	}
	printf(" %s(): Mallok ok di %X\r\n", __FUNCTION__, p_gr);
	
	memcpy((char *) p_gr, (char *) ALMT_MESIN, (10 * sizeof (struct t_group)));
	
	/* argumen ke dua adalah nama, argumen pertama adalah nomer */
	if (strcmp(argv[2], "nama") == 0)
	{
		sprintf(buf, "%s", argv[1]);	
		sumb = cek_nomer_valid(buf, 10);
		if (sumb > 0)		
		{
			printf(" Group %d : nama : %s\r\n", sumb, argv[3]);			
			if (strlen(argv[3]) > 10)
			{
				printf(" ERR: nama terlalu panjang (Maks 10 karakter)!\r\n");
				vPortFree( p_gr );
				return;
			}
			sprintf(p_gr[sumb-1].nama, argv[3]);	
		}
	}
	else if ((strcmp(argv[2], "set") == 0) || (strcmp(argv[2], "aktif") == 0))
	{
		sprintf(buf, "%s", argv[1]);	
		sumb = cek_nomer_valid(buf, 10);
		if (sumb > 0)		
		{
			printf(" Group %d : set : %s\r\n", sumb, argv[3]);	
			if (( argv[3][0] == '1') || (argv[3][0] == '0'))
			{
				p_gr[sumb - 1].stat = (argv[3][0] - '0');
			}
			else
			{
				printf(" ERR: pilih set 1 atau 0 !\r\n");
				vPortFree( p_gr );
				return;
			} 
		}
	}
	else if ((strcmp(argv[2], "ket") == 0) || (strcmp(argv[2], "desc") == 0))
	{
		sprintf(buf, "%s", argv[1]);	
		sumb = cek_nomer_valid(buf, 10);
		if (sumb > 0)		
		{
			printf(" Group %d : ket : %s\r\n", sumb, argv[3]);			
			if (strlen(argv[3]) > 32)
			{
				printf(" ERR: ket terlalu panjang (Maks 32 karakter)!\r\n");
				vPortFree( p_gr );
				return;
			}
			sprintf(p_gr[sumb-1].ket, argv[3]);	
		}
	}
	else
	{
		printf(" ERR: perintah tidak benar !\r\n");
		printf(" coba set_mesin help \r\n");
		vPortFree( p_gr );
		return;
	}
	
	/* cek apakah pemeriksaan angka valid */
	if (sumb <= 0)
	{
		vPortFree( p_gr );
		return ;	
	}
	
	// SEMUA TRUE dan sampai disini
	if (simpan_group( p_gr ) < 0)
	{
		vPortFree( p_gr );
		return -1;
	}
	vPortFree( p_gr );

}

static tinysh_cmd_t set_group_cmd={0,"set_group","menampilkan konfigurasi mesin","help default nama ket",
                              set_group,0,0,0};


static int set_group_default(void)
{
	int i;
	struct t_group *p_gr;
	
	judul(" Set Group ke Default\r\n");
	
	p_gr = pvPortMalloc( 10 * sizeof (struct t_group) );
	if (p_gr == NULL)
	{
		printf("%s(): Err allok memory gagal !\r\n");
		return -1;
	}
	
	for (i=0; i<10; i++)
	{
		sprintf(p_gr[i].nama, "Equip_%d", (i+1));
		p_gr[i].ID_group = (i+1);
		p_gr[i].stat = 0;			// pasif/unset
		sprintf(p_gr[i].ket, "--");
	}
	
	if (simpan_group( p_gr ) < 0)
	{
		vPortFree( p_gr );
		return -1;
	}
	vPortFree( p_gr );
	
}

static int simpan_group( struct t_group *pgr)
{
	printf(" Save struct GROUP ke flash ..");
	if(prepare_flash(SEKTOR_MESIN, SEKTOR_MESIN)) return -1;
	printf("..");
	
	if(hapus_flash(SEKTOR_MESIN, SEKTOR_MESIN)) return -1;
	printf("..");
	
	if(prepare_flash(SEKTOR_MESIN, SEKTOR_MESIN)) return -1;
	printf("..");
	
	if(tulis_flash(ALMT_MESIN, (unsigned short *) pgr, (sizeof (struct t_group) * 10))) return -1;
	
	printf(".. OK\r\n");
	return 0;
}
