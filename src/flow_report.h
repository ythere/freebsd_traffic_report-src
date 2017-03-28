#ifndef FLOW_REPORT_H
#define FLOW_REPORT_H
#include "cJSON.h"
typedef struct data_list{
	char *name;
	char *upstream;
	char * downstream;
	struct data_list *next;
}traffic_data;

extern char* process_json_data(cJSON* json_data);
extern traffic_data* process_network( char *name, traffic_data* rtc );
extern cJSON* switch_to_json( traffic_data* rtc );
extern char* curl(char* data, char* address);

#endif // FLOW_REPORT_H

