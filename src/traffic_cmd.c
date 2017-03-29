#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "flow_report.h"
#include "cJSON.h"

int main()
{
	int i;
	cJSON *json_curl, *json_data;
	char *result_str;
    	char *result;
    	char *analysis_result;
	traffic_data *rtc = (traffic_data*)malloc(sizeof(traffic_data));
	rtc->next = NULL;
	rtc->name = NULL;

	/* Search the data */
	
	char card_name[5][6] = {"em0", "em1", "em2", "em3", "ovpnc1"};
	for ( i=0; i<5; i++ )
	{
		rtc = process_network( card_name[i], rtc );
	}
	rtc = rtc->next;
	
	json_data = switch_to_json(rtc);
	result_str = cJSON_Print(json_data);
	result = curl( result_str, "http://192.168.2.188:8080/vpnboss/vbox_device_traffic_realtime_report" ); 
	json_curl = cJSON_Parse(result);
	analysis_result = process_json_data(json_curl);
	if (strcmp(analysis_result,"success") == 0)
		return 0;
	else 
		return 1;
	
}

