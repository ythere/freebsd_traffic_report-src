#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys/time.h"

#include "cJSON.h"
#include "flow_report.h"
char bufx[40096];

char* process_json_data(cJSON* json_data)
{
	int i;
	cJSON* json_value, *item, *array_item;
	char *result, *error_code, *error_msg;
	if(!json_data)
	{
		return "false";
	}
	for(json_value = json_data->child; json_value; json_value = json_value->next)
	{
		if(json_value->type != cJSON_String)
            continue;
        if(!strcmp(json_value->string, "process"))
          	result = json_value->valuestring;
		if(!strcmp(json_value->string, "error_code"))
            error_code = json_value->valuestring;
		if(!strcmp(json_value->string, "error_msg"))
            error_msg = json_value->valuestring;
	}
	if( !strcmp(result, "1"))
		return "success";
	else
		return "false";
	
}

char* curl(char* data, char* address)
{
	FILE *fp;
	char *stri, fc;
	sprintf(bufx, "curl --data '%s' \"%s\"", data, address);
	fp = popen(bufx, "r");
	stri = bufx;
	while((fc = fgetc(fp)) != EOF)
	{
		*stri++ = fc;
	}
	*stri = '\0';
	fclose(fp);
	return bufx; //返回json数据
}


cJSON* switch_to_json( traffic_data* rtc )
{
	FILE *fpid;
	struct timeval tv;
	cJSON *json, *json_data, *jsoni, *json_tmp, *data;
    traffic_data *ri = rtc;
	long timems;
	int times;
	char  stri[17];
	gettimeofday(&tv, NULL);
	timems = tv.tv_sec * 1000 + tv.tv_usec / 1000;
	times = tv.tv_sec;
	fpid = fopen("/vbox/config/system/device_id_info", "r");
	if (fpid == NULL)
	perror("ERROR opening file");
	else{
		 if(fgets(stri,17,fpid) != NULL ); 
		puts(stri);
		fclose(fpid);

	}
    json = cJSON_CreateObject();
	cJSON_AddItemToObject(json, "task_id", cJSON_CreateNumber(times));
	cJSON_AddItemToObject(json, "device_serial_number", cJSON_CreateString(stri));
	cJSON_AddItemToObject(json, "data_time", cJSON_CreateNumber(times));
	cJSON_AddItemToObject(json, "device_port_traffic", json_tmp=cJSON_CreateArray());
	
	while(ri  != NULL)
    {
		jsoni = cJSON_CreateObject();
		cJSON_AddItemToArray(json_tmp, jsoni);
		cJSON_AddStringToObject(jsoni, "port_device_name", ri->name);
		cJSON_AddStringToObject(jsoni, "upstream_traffic", ri->upstream );
		cJSON_AddStringToObject(jsoni, "downstream_traffic", ri->downstream);
		ri = ri->next;
    }
    return json;
}

traffic_data* process_network( char *name, traffic_data* rtc )
{
	char *tmpch = "\" \"";
	int len, linenum = 0 , linenumup = 0;
	FILE *fp, *fp2; 
	char cmd[256], filecmd[256], in_get_num[128], out_get_num[128], *in_tra, *out_tra;
	traffic_data *current, *this;
	this = (traffic_data *)malloc(sizeof(traffic_data));
	
	char *namebuf = malloc(sizeof(char) * 10);
	strcpy( namebuf, name );
	
	in_tra = (char*)malloc(5096 * sizeof(char));
	out_tra = (char*)malloc(5096 * sizeof(char));

	this->name = namebuf;
	this->next = NULL;
	
/* Traffic down */	
	sprintf(cmd, "expr `netstat -i -b -n -I %s | grep %s | awk -F %s '{print $8}' | head -1` - `cat /vbox/config/traffic/down_latest/%s` >> /vbox/config/traffic/down_history/%s", namebuf, namebuf, tmpch, namebuf, namebuf );
	puts(cmd);
	system(cmd);
	
	sprintf(filecmd, "/vbox/config/traffic/down_history/%s", namebuf);
	//memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, "netstat -i -b -n -I %s | grep %s | awk -F %s '{print $8}' | head -1 > /vbox/config/traffic/down_latest/%s", namebuf, namebuf, tmpch, namebuf);
	system(cmd);
	
	memset(in_tra,0,sizeof(in_tra));
	memset(out_tra,0,sizeof(out_tra));
	
	fp = fopen(filecmd,"r");
	while( fgets(in_get_num,sizeof(in_get_num), fp) != NULL)
	{
		len = strlen(in_get_num);
		in_get_num[len-1]='|';
		strcat(in_tra, in_get_num);
		linenum++;
	}
	fclose(fp);
	memset(in_get_num,0,sizeof(in_get_num));
	 if ( linenum < 288)
	{
			while(linenum <=286)
		{
			strcat(in_tra, "0|");
			linenum++;
		}
		strcat(in_tra, "0");
	}
	else
	{
		//清空流量历史文件
		memset(cmd, 0, sizeof(cmd));
		sprintf(cmd,"echo > /vbox/config/traffic/down_history/%s", namebuf);
		system(cmd);
	}
	this->downstream = in_tra;
	
	
	
/* Traffic up */
	memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, "expr `netstat -i -b -n -I %s | grep %s | awk -F %s '{print $11}' | head -1` - `cat /vbox/config/traffic/up_latest/%s` >> /vbox/config/traffic/up_history/%s", namebuf, namebuf, tmpch, namebuf, namebuf );
	system(cmd);
	memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, "netstat -i -b -n -I %s | grep %s | awk -F %s '{print $11}' | head -1 > /vbox/config/traffic/up_latest/%s", namebuf, namebuf, tmpch, namebuf);
	system(cmd);
	
	memset(filecmd,0,sizeof(filecmd));
	sprintf(filecmd, "/vbox/config/traffic/up_history/%s", namebuf);
	fp2 = fopen(filecmd,"r");
	while( fgets(out_get_num,sizeof(out_get_num), fp) != NULL)
	{
		len = strlen(out_get_num);
		out_get_num[len-1]='|';
		strcat(out_tra, out_get_num);
		linenumup++;
	}
	fclose(fp2);
	memset(out_get_num,0,sizeof(out_get_num));
	 if ( linenumup < 288)
	{
			while(linenumup <=286)
		{
			strcat(out_tra, "0|");
			linenumup++;
		}
		strcat(out_tra, "0");
	}
	else
	{
		//清空流量历史文件
		memset(cmd, 0, sizeof(cmd));
		sprintf(cmd,"echo > /vbox/config/traffic/up_history/%s", namebuf);
		system(cmd);
	}
	this->upstream = out_tra;
	
	//插入结构体链表之中
	current = rtc;
	if ( current == NULL )
		current = this;
	else
	{
		while( current->next != NULL)
		{
			current = current->next;
		}
		current->next = this; 
	}
	return rtc;
}
