/*
	Afrendy Bayu
	27 Juni 2012

*/


#include "FreeRTOS.h"
#include "gsm/modem.h"

// perintah modem
// 1: baca SMS
// 2: kirim SMS
// 3: email
// 4: ftp
// 5: web

#ifdef PAKAI_MODEM_GSM
#ifndef __SHELL_MODEM_GSM__
#define __SHELL_MODEM_GSM__

	void sms_kitab()	{
		//printf(" set_date tahun bulan tanggal jam menit\r\n");
		//printf("   misalnya : set_date 2010 3 5 10 22\r\n");
		//printf("   artinya  : set waktu ke tgl 5 Maret 2010, jam 10:22 pagi\r\n");	
	}

	void cek_sms_sh()	{
		printf(" Cek SMS \r\n");
		vTaskDelay(100);
		flagModem = BACA_SMS;		// modem kirim SMS
		//membaca_sms();
	}
	static tinysh_cmd_t cek_sms_sh_cmd={0,"cek_sms","","",  cek_sms_sh,0,0,0};
	
	void cek_AT_sh()	{
		printf(" Cek AT Modem \r\n");
		vTaskDelay(100);
		flagModem = CEK_AT;
	}
	static tinysh_cmd_t cek_AT_sh_cmd={0,"cek_at","","",  cek_AT_sh,0,0,0};
	
	void cek_pulsa_sh()	{
		printf(" Cek Pulsa \r\n");
		vTaskDelay(100);
		flagModem = CEK_PULSA;
	}
	static tinysh_cmd_t cek_pulsa_sh_cmd={0,"cek_pulsa","","",  cek_pulsa_sh,0,0,0};
	
	void cek_sms_tunggal_sh(int argc, char **argv)	{
		int idx=0;
		if (argc>2 || argc==1) {
			sms_kitab(argv[0]);
			return 0;
		}
	
		sscanf(argv[1], "%d", &idx);
		printf(" baca SMS ke-%d\r\n", idx);
		vTaskDelay(100);
		flagModem = BACA_SMS_TUNGGAL;		// modem baca SMS tunggal
		index_gsm = idx;
	}
	static tinysh_cmd_t cek_sms_tunggal_sh_cmd={0,"cek_1sms","","",  cek_sms_tunggal_sh,0,0,0};
	
	void hapus_sms_sh(int argc, char **argv)	{
		int idx=0;
		if (argc>2 || argc==1) {
			sms_kitab(argv[0]);
			return 0;
		}
	
		sscanf(argv[1], "%d", &idx);
		printf(" menghapus SMS ke-%d\r\n", idx);
		vTaskDelay(100);
		flagModem = HAPUS_SMS;		// modem hapus SMS
		index_gsm = idx;
	}
	static tinysh_cmd_t hapus_sms_sh_cmd={0,"hapus_sms","","",  hapus_sms_sh,0,0,0};
	
	void kirim_sms_sh(int argc, char **argv)	{
		int idx=0;
		if (argc<3) {
			sms_kitab(argv[0]);
			return 0;
		}
	
		//sscanf(argv[1], "%d", &idx);
		printf(" kirim SMS ke nomor %s\r\n", argv[1]);
		vTaskDelay(100);
		flagModem = KIRIM_SMS;		// modem hapus SMS
	}
	static tinysh_cmd_t kirim_sms_sh_cmd={0,"kirim_sms","","",  kirim_sms_sh,0,0,0};

#endif
#endif