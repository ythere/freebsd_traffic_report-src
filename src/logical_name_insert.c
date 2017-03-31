#include<mxml.h>
#include "flow_report.h"
#define XML_FILE_PATH "/home/test/config.xml"

traffic_data* logical_name_get(traffic_data* rtc)
{
	traffic_data* current = NULL;
	FILE *fp = NULL;
	mxml_node_t *xml;
    char *wan_net[5] = {0};
    char *lan_net[5] = {0};
    int i = 0;


	fp = fopen(XML_FILE_PATH, "r");
	xml = mxmlLoadFile(NULL, fp, MXML_TEXT_CALLBACK);
	fclose(fp);
	
	mxml_node_t *interfaces;
    interfaces = mxmlFindElement(xml, xml, "interfaces", NULL, NULL, MXML_DESCEND);

    mxml_node_t *wan;
    mxml_node_t *wannet;
    for(wan = mxmlFindElement(interfaces, xml, "wan", NULL, NULL, MXML_DESCEND); wan != NULL; wan = mxmlFindElement(wan, interfaces, "wan", NULL, NULL, MXML_DESCEND))
    {
        wannet = mxmlFindElement(wan, xml, "if", NULL, NULL, MXML_DESCEND);
        wan_net[i] = wannet->child->value.text.string;
		for( current = rtc; current != NULL; current = rtc->next)
		{
			if (!strcmp(current->name, wan_net[i]))
			{
				current->logical_name = "wan";
			}
		}
        printf("wan:%s\n", wan_net[i]);
        i++;
    }

    mxml_node_t *lan;
    mxml_node_t *lannet;
    i = 0;
    for(lan = mxmlFindElement(interfaces, xml, "lan", NULL, NULL, MXML_DESCEND); lan != NULL; lan = mxmlFindElement(lan, interfaces, "lan", NULL, NULL, MXML_DESCEND))
    {
        lannet = mxmlFindElement(lan, xml, "if", NULL, NULL, MXML_DESCEND);
        lan_net[i] = lannet->child->value.text.string;
		for( current = rtc; current != NULL; current = rtc->next)
		{
			if (!strcmp(current->name, lan_net[i]))
			{
				current->logical_name = "lan";
			}
		}
        printf("lan:%s\n",lan_net[i]);
        i++;
    }
	return rtc;
}



