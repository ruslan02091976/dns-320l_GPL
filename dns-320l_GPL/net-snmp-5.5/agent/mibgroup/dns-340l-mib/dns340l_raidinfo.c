#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#if HAVE_STDLIB_H
#include <stdlib.h>
#endif
#if HAVE_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif

#include "dns340l_raidinfo.h"
#include "getinfo.h"

ID_dns340lvolumeTable	dns340lVolumeTable_head;
VOLUME_INFO				volume_info[MAX_VOLUME_NUM];

/*-----------------------------------------------------------------
* ROUTINE NAME - init_dns340l_raidinfo         
*------------------------------------------------------------------
* FUNCTION: 
*
* INPUT:
* OUTPUT:
* RETURN:
*                                 
* NOTE:
*----------------------------------------------------------------*/
void init_dns340l_raidinfo(void)
{
	//volume table
	initialize_table_dns340lVolumeTable();
}

/*-----------------------------------------------------------------
* ROUTINE NAME - dns340lVolumeTable_Initialize         
*------------------------------------------------------------------
* FUNCTION: 
*
* INPUT:
* OUTPUT:
* RETURN:
*                                 
* NOTE:
*----------------------------------------------------------------*/
void dns340lVolumeTable_Initialize(void)
{
	ID_dns340lvolumeTable	entry;
	int						entry_num;
	int						i;
	
	memset(volume_info, 0, sizeof volume_info);
	
	entry_num=get_volume_info(volume_info);
	
	for(i=1; i<=MAX_VOLUME_NUM; i++)
	{
		//create entry
		entry = dns340lVolumeTable_createEntry((long)i);
		
		//make table entry valid and visible ,1:visible
		entry->valid = volume_info[i-1].enable;
		
		    
		entry->volume_num=volume_info[i-1].volume_num;    
		strcpy(entry->volume_name, volume_info[i-1].name);
		strcpy(entry->volume_fs_type, volume_info[i-1].fs_type);
		strcpy(entry->volume_raid_level, volume_info[i-1].raid_level);
		strcpy(entry->volume_size, volume_info[i-1].size);
		strcpy(entry->volume_free_space, volume_info[i-1].free_space);
	}
}


void dns340lVolumeTable_get(void)
{
	ID_dns340lvolumeTable		entry;
	int						i;
	
	entry = dns340lVolumeTable_head;
	get_volume_info(volume_info);
	
	for(i=MAX_VOLUME_NUM; i>=1; i--)
	{
		//create entry
		
		
		//make table entry valid and visible ,1:visible
		entry->valid = volume_info[i-1].enable;

		entry->volume_num=volume_info[i-1].volume_num; 
		strcpy(entry->volume_name, volume_info[i-1].name);
		strcpy(entry->volume_fs_type, volume_info[i-1].fs_type);
		strcpy(entry->volume_raid_level, volume_info[i-1].raid_level);
		strcpy(entry->volume_size, volume_info[i-1].size);
		strcpy(entry->volume_free_space, volume_info[i-1].free_space);
		entry=entry->next;
	}
	
	
}

/*-----------------------------------------------------------------
* ROUTINE NAME - initialize_table_dns340lVolumeTable         
*------------------------------------------------------------------
* FUNCTION: 
*
* INPUT:
* OUTPUT:
* RETURN:
*                                 
* NOTE:
*----------------------------------------------------------------*/
void initialize_table_dns340lVolumeTable(void)
{
	static oid dns340lVolumeTable_oid[]		= { 1, 3, 6, 1, 4, 1, 171, 50, 1, 10, 1, 1, 9 };
	netsnmp_handler_registration 	*reg;
    netsnmp_iterator_info 			*iinfo;
    netsnmp_table_registration_info	*table_info;
	
	reg = netsnmp_create_handler_registration("dns340LVolumeTable",
												dns340lVolumeTable_handler,
												dns340lVolumeTable_oid,
												OID_LENGTH(dns340lVolumeTable_oid),
												HANDLER_CAN_RONLY);
												
	table_info = SNMP_MALLOC_TYPEDEF(netsnmp_table_registration_info);
	netsnmp_table_helper_add_indexes(table_info, ASN_INTEGER, 0);
	
	table_info->min_column = DNS340L_VOLUME_NUM;
	table_info->max_column = DNS340L_VOLUME_FREE_SPACE;
	
	iinfo = SNMP_MALLOC_TYPEDEF(netsnmp_iterator_info);
    iinfo->get_first_data_point = dns340lVolumeTable_get_first_data_point;
    iinfo->get_next_data_point = dns340lVolumeTable_get_next_data_point;
    iinfo->table_reginfo = table_info;

    netsnmp_register_table_iterator(reg, iinfo);
    
    //Initialise the contents of the table here
    dns340lVolumeTable_Initialize();
}

/*-----------------------------------------------------------------
* ROUTINE NAME - dns340lVolumeTable_get_first_data_point         
*------------------------------------------------------------------
* FUNCTION: 
*
* INPUT:
* OUTPUT:
* RETURN:
*                                 
* NOTE:
*----------------------------------------------------------------*/
netsnmp_variable_list *dns340lVolumeTable_get_first_data_point(void **my_loop_context,
												void **my_data_context,
												netsnmp_variable_list *
												put_index_data,
												netsnmp_iterator_info *mydata)
{
    *my_loop_context = dns340lVolumeTable_head;
    return dns340lVolumeTable_get_next_data_point(my_loop_context,
                                                 my_data_context,
                                                 put_index_data, mydata);
}

/*-----------------------------------------------------------------
* ROUTINE NAME - dns340lVolumeTable_get_next_data_point         
*------------------------------------------------------------------
* FUNCTION: 
*
* INPUT:
* OUTPUT:
* RETURN:
*                                 
* NOTE:
*----------------------------------------------------------------*/
netsnmp_variable_list *dns340lVolumeTable_get_next_data_point(void **my_loop_context,
											void **my_data_context,
											netsnmp_variable_list * put_index_data,
											netsnmp_iterator_info *mydata)
{
    ID_dns340lvolumeTable entry = (ID_dns340lvolumeTable)*my_loop_context;
    netsnmp_variable_list *idx = put_index_data;

    if (entry)
    {
        snmp_set_var_typed_integer(idx, ASN_INTEGER, entry->volume_num);
        idx = idx->next_variable;
        *my_data_context = (void *) entry;
        *my_loop_context = (void *) entry->next;
        return put_index_data;
    }
    else
    {
        return NULL;
    }
}

/*-----------------------------------------------------------------
* ROUTINE NAME - dns340lVolumeTable_createEntry         
*------------------------------------------------------------------
* FUNCTION: 
*
* INPUT:
* OUTPUT:
* RETURN:
*                                 
* NOTE:
*----------------------------------------------------------------*/
ID_dns340lvolumeTable dns340lVolumeTable_createEntry(long entry_num)
{
    ID_dns340lvolumeTable entry;

    entry = SNMP_MALLOC_TYPEDEF(struct _DNS340L_VOLUME_TABLE_);
    if (!entry)
        return NULL;
		
		entry->entry_num = entry_num;
    entry->next = dns340lVolumeTable_head;
    dns340lVolumeTable_head = entry;
    return entry;
}

/*-----------------------------------------------------------------
* ROUTINE NAME - dns340lVolumeTable_removeEntry         
*------------------------------------------------------------------
* FUNCTION: 
*
* INPUT:
* OUTPUT:
* RETURN:
*                                 
* NOTE:
*----------------------------------------------------------------*/
void dns340lVolumeTable_removeEntry(ID_dns340lvolumeTable entry)
{
    ID_dns340lvolumeTable	ptr, prev;

	if (!entry)
		return;                 /* Nothing to remove */

    for (ptr = dns340lVolumeTable_head, prev = NULL;
         ptr != NULL; prev = ptr, ptr = ptr->next)
	{
		if (ptr == entry)
			break;
	}
	if (!ptr)
		return;                 /* Can't find it */

	if (prev == NULL)
		dns340lVolumeTable_head = ptr->next;
	else
		prev->next = ptr->next;

    SNMP_FREE(entry);           /* XXX - release any other internal resources */
}

/*-----------------------------------------------------------------
* ROUTINE NAME - dns340lVolumeTable_removeEntry_byNum         
*------------------------------------------------------------------
* FUNCTION: 
*
* INPUT:
* OUTPUT:
* RETURN:
*                                 
* NOTE:
*----------------------------------------------------------------*/
void dns340lVolumeTable_removeEntryy_byNum(long num)
{
	ID_dns340lvolumeTable	ptr, prev;

	for (ptr = dns340lVolumeTable_head, prev = NULL;
		ptr != NULL; prev = ptr, ptr = ptr->next)
	{
		if (ptr->volume_num == num)
		break;
    }
	if (!ptr)
		return;                 /* Can't find it */

	if (prev == NULL)
		dns340lVolumeTable_head = ptr->next;
	else
		prev->next = ptr->next;
       
	SNMP_FREE(ptr);           /* XXX - release any other internal resources */
}

/*-----------------------------------------------------------------
* ROUTINE NAME - dns340lVolumeTable_handler         
*------------------------------------------------------------------
* FUNCTION: 
*
* INPUT:
* OUTPUT:
* RETURN:
*                                 
* NOTE:
*----------------------------------------------------------------*/
int dns340lVolumeTable_handler(netsnmp_mib_handler *handler,
			netsnmp_handler_registration *reginfo,
			netsnmp_agent_request_info *reqinfo,
			netsnmp_request_info *requests)
{
	netsnmp_request_info		*request;
	//netsnmp_variable_list		*requestvb;
	netsnmp_table_request_info	*table_info;
	ID_dns340lvolumeTable		table_entry;
	
	
	dns340lVolumeTable_get();
	
	switch(reqinfo->mode)
	{
		//Read-support (also covers GetNext requests)
		case MODE_GET:
			for (request = requests; request; request = request->next)
			{
				//requestvb = request->requestvb;	//????
				
				table_entry = (ID_dns340lvolumeTable)netsnmp_extract_iterator_context(request);
				table_info = netsnmp_extract_table_info(request);

				if(table_entry && (table_entry->valid == 0))
				{
					netsnmp_set_request_error(reqinfo, request, SNMP_NOSUCHINSTANCE);
					continue;
				}
				
				switch (table_info->colnum)
				{
					case DNS340L_VOLUME_NUM:
						if (!table_entry)
						{
							netsnmp_set_request_error(reqinfo, request, SNMP_NOSUCHINSTANCE);
							continue;
						}
						snmp_set_var_typed_integer(request->requestvb, ASN_INTEGER,
                                           table_entry->volume_num);
						break;
						
					case DNS340L_VOLUME_NAME:
						if (!table_entry)
						{
							netsnmp_set_request_error(reqinfo, request, SNMP_NOSUCHINSTANCE);
							continue;
						}
						
						snmp_set_var_typed_value(request->requestvb, ASN_OCTET_STR,
                                         (u_char *) table_entry->volume_name,
                                         strlen(table_entry->volume_name));
						break;
						
					case DNS340L_VOLUME_FS_TYPE:
						if (!table_entry)
						{
							netsnmp_set_request_error(reqinfo, request, SNMP_NOSUCHINSTANCE);
							continue;
						}
						snmp_set_var_typed_value(request->requestvb, ASN_OCTET_STR,
                                         (u_char *) table_entry->volume_fs_type,
                                         strlen(table_entry->volume_fs_type));
						break;
						
					case DNS340L_VOLUME_RAID_LEVEL:
						if (!table_entry)
						{
							netsnmp_set_request_error(reqinfo, request, SNMP_NOSUCHINSTANCE);
							continue;
						}
						snmp_set_var_typed_value(request->requestvb, ASN_OCTET_STR,
                                         (u_char *) table_entry->volume_raid_level,
                                         strlen(table_entry->volume_raid_level));
						break;
						
					case DNS340L_VOLUME_SIZE:
						if (!table_entry)
						{
							netsnmp_set_request_error(reqinfo, request, SNMP_NOSUCHINSTANCE);
							continue;
						}
						snmp_set_var_typed_value(request->requestvb, ASN_OCTET_STR,
                                         (u_char *) table_entry->volume_size,
                                         strlen(table_entry->volume_size));
						break;
						
					case DNS340L_VOLUME_FREE_SPACE:
						if (!table_entry)
						{
							netsnmp_set_request_error(reqinfo, request, SNMP_NOSUCHINSTANCE);
							continue;
						}
						
						//get current free space for volume
//						get_volume_free_space(volume_info);
//						strcpy(table_entry->volume_free_space, volume_info[table_entry->entry_num-1].free_space);
						
						snmp_set_var_typed_value(request->requestvb, ASN_OCTET_STR,
                                         (u_char *) table_entry->volume_free_space,
                                         strlen(table_entry->volume_free_space));
						break;	
					
					default:
						netsnmp_set_request_error(reqinfo, request, SNMP_NOSUCHOBJECT);
						break;
				}
			}
			break;
	}

	return SNMP_ERR_NOERROR;
}





