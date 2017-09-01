#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

//MAC                RSSI0  RSSI1  RSSI2  AVGSI0  AVGSI1  AVGSI2  Idle   TxPackets   RxPackets   TxBytes     RxBytes     TxRetries   TxFailed    TxBitrate   RxBitRate   
//40a5ef0d1931 -55   -63   0     -55   -61   0     299     163        288        76661      44227      0             0     130   32779
//b8098a70983f -51   -43   0     -51   -44   0     299     82         191        7147       43251      0             0      65   16388

#define MAC_ADDR_LEN 12
typedef struct {
	char mac[MAC_ADDR_LEN+1];
	int8_t signal;
	int8_t signal_left;
	int8_t signal_right;
	int8_t avg_signal;
	int8_t avg_signal_left;
	int8_t avg_signal_right;
	
	unsigned int inactive_time; //idle

	unsigned int rx_bytes;
	unsigned int rx_packets;
	unsigned int tx_bytes;
	unsigned int tx_packets;

	unsigned int tx_retries;
	unsigned int tx_failed;

	char tx_bitrate[16];
	char rx_bitrate[16];
} STA_INFO;


int main(){
	FILE *ofp = NULL;
	char buff[256];

	ofp = fopen("stainfoext.txt", "r");
	if (NULL == ofp){
		printf("Open File Failed\n");
		return 0;
	}
	
	STA_INFO sta[64];
	int i =0;
	
	memset(buff, 0, sizeof(buff));
	memset(sta, 0, sizeof(sta));
	
	while ( fgets(buff, sizeof(buff)-1, ofp) != NULL) {
		buff[sizeof(buff)-1] = 0;
		unsigned int _blen = strlen(buff);
		if (_blen <= 0) continue;
		if (buff[_blen-1] == '\r' || buff[_blen-1] == '\n') {
			buff[_blen-1] = '\0';  // strip the last '\n'
		}
		
		printf("buff=%s\n", buff);
		unsigned int tx, rx, txb, rxb, txr, txf;
		int8_t s, sl, sr, avg, avgl, avgr;
		
		s = sl = sr = avg = avgl = avgr = 0;
		tx = rx = txb = rxb = txr = txf = 0;
		
		int cret = sscanf(buff, "%s %d %d %d %d %d %d %u %u %u %u %u %u %u %s %s", 
			sta[i].mac, 
			//&sta[i].signal, &sta[i].signal_left, &sta[i].signal_right,
			//&sta[i].avg_signal, &sta[i].avg_signal_left, &sta[i].avg_signal_right,
			(int *)&s, (int *)&sl, (int *)&sr,
			(int *)&avg, (int *)&avgl, (int *)&avgr,
			&sta[i].inactive_time, 
			//&sta[i].tx_packets, &sta[i].rx_packets,
			//&sta[i].tx_bytes, &sta[i].rx_bytes,
			//&sta[i].tx_retries, &sta[i].tx_failed,
			&tx, &rx,
			&txb, &txr,
			&txr, &txf,
			sta[i].tx_bitrate, sta[i].rx_bitrate);
		
		sta[i].signal = s;
		sta[i].signal_left = sl;
		sta[i].signal_right = sr;
		sta[i].avg_signal = avg;
		sta[i].avg_signal_left = avgl;
		sta[i].avg_signal_right = avgr;
		sta[i].tx_packets = tx;
		sta[i].rx_packets = rx;
		sta[i].tx_bytes = txb;
		sta[i].rx_bytes = rxb;
		sta[i].tx_failed = txf;
		sta[i].tx_retries = txr;
		
		
		printf("cret=%d; [%u][%u]\n",cret, sta[i].tx_packets, sta[i].rx_packets);
		
		if (cret != 16) {
			
			continue;
		}
		
		printf("mac=%s signal=%d signal_left=%d signal_right=%d avg=%d avg_left=%d avg_right=%d idle=%d "
			   "tx_packet=%u rx_packet=%u tx_bytes=%u rx_bytes=%u retries=%u failed=%u txbitrate=%s rxbitrate=%s\n", 
			sta[i].mac, sta[i].signal, sta[i].signal_left, sta[i].signal_right,
			sta[i].avg_signal, sta[i].avg_signal_left, sta[i].avg_signal_right,
			sta[i].inactive_time,
			sta[i].tx_packets, sta[i].rx_packets,
			sta[i].tx_bytes, sta[i].rx_bytes,
			sta[i].tx_retries, sta[i].tx_failed,
			sta[i].tx_bitrate, sta[i].rx_bitrate);
		
		i++;
		
	}
	
	fclose(ofp);
	
	
	
}
