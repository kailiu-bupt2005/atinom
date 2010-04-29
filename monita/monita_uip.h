/*
 * monita_uip.h
 *
 *  Created on: Dec 10, 2008
 *      Author: jadid
 
 	24 Juli 2009
 	ditambahkan PORT_KONTROL
	
	3 Feb 2010
	SEKTOR_DT_SET & ALMT_DT_SET ditambahkan
	dan disamakan alamatnya dengan TITIK
	
 */

#include "../uip/uip/psock.h"
#include "../uip/uip/uipopt.h"


#ifndef MONITA_UIP_H_
#define MONITA_UIP_H_

#define JML_SUMBER	20
#define JML_MESIN	10
#define TIAP_MESIN	100
#define JML_KANAL	20
#define PER_SUMBER	20

#ifdef PAKAI_FILE_SIMPAN
	#define SEKTOR_SFILE	24
	#define ALMT_SFILE		0x7A000
#endif

#define SEKTOR_SUMBER	25
#define ALMT_SUMBER		0x7B000

#define SEKTOR_MESIN	26
#define ALMT_MESIN		0x7C000

#define SEKTOR_GROUP	26
#define ALMT_GROUP		0x7C000

#define SEKTOR_TITIK	21
#define ALMT_TITIK		0x70000

#define SEKTOR_DT_SET	21
#define ALMT_DT_SET		0x70000

#ifdef PAKAI_GSM_FTP
	#define SEKTOR_GSM_FTP	27
	#define ALMT_GSM_FTP	0x7D000
#endif

#ifdef PAKAI_CRON				// berlebihan kah ? 32 kB 
	#define JML_CRON	10		// baca_cron() setiap 1/2 detik mulai detik ke 5
								// ubah mulai baca_cron di tinysh/cron.c
								// max JML_CRON = 108.
	#define SEKTOR_CRON	20		// 32 kB/64 = 512.
	#define ALMT_CRON	0x68000
#endif

#ifdef PAKAI_SELENOID
	#define JML_RELAY 8
#endif

#define 	PORT_MONITA 	5001
#define 	PORT_DAYTIME	13
#define 	ISO_nl 			0x0a
#define		PORT_KONTROL	7235
#define		PORT_KIRIM_BALIK	6543

/* offset masing2 data pada array */
#define OFFSET_CAP_L	1	- 1
#define OFFSET_CAP_R	2	- 1
#define OFFSET_CAT_L	3	- 1
#define OFFSET_CAT_R	4	- 1

#define OFFSET_JWP_IN1	5	- 1
#define OFFSET_JWP_IN2	6	- 1
#define OFFSET_JWP_OUT1	7	- 1
#define OFFSET_JWP_OUT2	8	- 1
	
#define OFFSET_JWT_IN1	9	- 1
#define OFFSET_JWT_IN2	10	- 1
#define OFFSET_JWT_OUT1	11	- 1
#define OFFSET_JWT_OUT2	12	- 1

#define OFFSET_LOP_IN1	13	- 1
#define OFFSET_LOP_IN2	14	- 1
#define OFFSET_LOP_OUT1	15	- 1
#define OFFSET_LOP_OUT2	16	- 1

#define OFFSET_LOT_IN1	17	- 1
#define OFFSET_LOT_IN2	18	- 1
#define OFFSET_LOT_OUT1	19	- 1
#define OFFSET_LOT_OUT2	20	- 1

#define OFFSET_EXHT1	21	- 1
#define OFFSET_EXHT2	22	- 1
#define OFFSET_EXHT3	23	- 1
#define OFFSET_EXHT4	24	- 1
#define OFFSET_EXHT5	25	- 1
#define OFFSET_EXHT6	26	- 1
#define OFFSET_EXHT7	27	- 1
#define OFFSET_EXHT8	28	- 1
#define OFFSET_EXHT9	29	- 1
#define OFFSET_EXHT10	30	- 1
#define OFFSET_EXHT11	31	- 1
#define OFFSET_EXHT12	32	- 1
#define OFFSET_EXHT13	33	- 1
#define OFFSET_EXHT14	34	- 1
#define OFFSET_EXHT15	35	- 1
#define OFFSET_EXHT16	36	- 1
#define OFFSET_EXHT17	37	- 1
#define OFFSET_EXHT18	38	- 1
#define OFFSET_EXHT19	39	- 1
#define OFFSET_EXHT20	40	- 1


#define OFFSET_FREK		45	- 1
#define OFFSET_GEN_RPM	46	- 1

#define OFFSET_BB_IN	49	- 1
#define OFFSET_BB_OUT	50	- 1

#define OFFSET_KW		51	- 1
#define OFFSET_KWH		52	- 1
#define OFFSET_VOLT		53	- 1
#define OFFSET_AMP		54	- 1

#ifdef  PAKAI_SELENOID
float data_f [ (JML_SUMBER * PER_SUMBER)+JML_RELAY ];
//*
#else
float data_f [ (JML_SUMBER * PER_SUMBER) ];
#endif
//*/

struct t_data_float {
	float data[20];
};


struct t_xdata {
	char mon[8];             // id bahwa data monita
    unsigned int nomer;      // urutan pengiriman data
    unsigned char flag;      // belum di definisikan
    unsigned char alamat;    // alamat stacking board (address power meter)

	char buf[232];				//16 jan 08, disesuaikan dengan Client Utama
};

struct sambungan_state {
	/*
	struct psock p;
	unsigned int state;
	unsigned int nomer_samb;
	unsigned int timer;
	*/
};

typedef struct sambungan_state uip_sambungan_state;
//typedef struct sambungan_state uip_tcp_appstate_t;

/*
sumber status :
	0 : tidak ada
	1 : time_out
	2 : normal
*/
struct t_sumber {
	char nama[16];
	//char ID_sumber;
	char alamat;		/* untuk alamat/modul Power meter atau stack board (jika ada) */
	char IP0;
	char IP1;
	char IP2;
	char IP3;
	char status;		// tidak aktif, timeout, dll
};

struct t_titik {
	char ID_sumber;			// sumber mulai dari 1, 1 untuk array sumber ke nol
	char kanal;				// kanal mulai dari 1, bukan nol, kanal 1 untuk array ke nol
	char alamat;			// alamat stack board
	float data;
};

struct t_mesin {
	char nama[10];
	char ID_mesin;
};

struct t_status {
	unsigned int reply;
	unsigned int lport;
	unsigned int timer;
	unsigned int reply_lama;
	unsigned int stat;
};

/* 
	untuk mendapatkan data2 yang perlu di kalkulasi 
	misalnya :
	BB in per menit.
	kWh per menit, sehingga bisa didapat SFC yang dihitung
	per menit
	
*/

struct t_data_hitung {
	float bb_in;
	float bb_in_lama;
	float bb_out;
	float bb_out_lama;
	float kwh;
	float kwh_lama;
	float sfc;
};

struct t_xcontrol {
	char  header[12];
 	short out_kanal;	// 16 bit (akan ada 64 K obyek kontrol)
 	short status;	// akan ada 64 kemungkinan status
 	float analog;	// future use
};

/* Konfigurasi versi 2 */

/* group adalah nama mesin yang ditampilkan pada tabbook atas
 * pada tampilan */
struct t_group {
	char nama[10];
	char ID_group;
	char stat;			// 1 aktif (ditampilkan), 0 mati / unset (tidak ditampilkan) 
	char ket[32];		// keterangan group yang ditampilkan pada bagian bawah
	short no_data[40];	// nomer data yang perlu ditampilkan.
};

struct t_dt_set  {
	float 	alarm_HH;
	//float 	alarm_LL;
	char	relay;
	char 	aktif;
	//char	ket[32];	
	char	nama[16];
	char 	satuan[8];
	float 	alarm_H;
	//float 	alarm_L;
};

#ifdef PAKAI_FILE_SIMPAN
struct t_simpan_file {
	int 	periode;		// periode disimpan dalam MMC
	int 	set;			// aktif atau tidak
	int 	detik;			// periode data disimpan dalam file
	short 	no_data[ JML_SUMBER * PER_SUMBER ];
	char	nama_file[16];	// awalan nama file, misalnya angin
	char	flag[80];
};
#endif

#ifdef PAKAI_GSM_FTP
struct t_gsm_ftp {
	int 	ftp_periode;		// 1 periode disimpan dalam MMC
	int 	ftp_mode;			// 2 aktif atau tidak mode ftp
	int		ftp_port;			// 3 portnya ftp
	char 	ftp_user[40];		// 4 periode data disimpan dalam file
	char 	ftp_passwd[16];		// 5
	char 	ftp_server[40];		// 6
	char	nama_file[40];		// 7 awalan nama file, misalnya angin
	char	direktori[40];		// 8
	int		gsm_mode;			// 9 aktif atau tidak mode gsm
	char	kartu[15];
	char	gprs_apn1[16];		// 10
	char	gprs_apn2[16];		// 11
	char	gprs_user[16];		// 12
	char	gprs_passwd[16];	// 13
};
#endif

#ifdef PAKAI_CRON
struct t_cron {
	char 	mnt[10];
	char 	jam[10];
	char	tgl[10];
	char	bln[10];
	char	cmd[20];
	char 	alamat;
	char	status;			// status perintahnya : suruh dihidup ato mati
	char 	set;			// status cron : aktif atau mati
};
#endif 

#endif /* MONITA_UIP_H_ */
