
#include "FreeRTOS.h"
#include "monita/monita_uip.h"


#ifdef PAKAI_MODEM_GSM

	#define nRespM 			200
	#define JML_MEM_SMS		10
	#define MAX_MEM_SMS		11

	void init_mem_sms();
	int membaca_sms();
	int ambil_sms();
	int aksi_sms();
	int menghapus_sms(int idx);
	int ambil_sms_tunggal(int idx);
	int proses_pulsa();
	int  kirimCmdModem(char *psCmdM,char * psRespM);
	int cek_AT_cmd();
	

#endif
