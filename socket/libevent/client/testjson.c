////////////////////////////////////////////////////////////////////////

char *form_json_string(result_array *ret， int ret_len, char *ap_mac) {
	json_object *jso;
	json_object *data;
	char *p;
	int i = 0;
	
	jso = json_object_new_object();
	data = json_object_new_array();

	static int seqId = 0;
	if (seqId > 65534) seqId = 0;
	
	json_object_object_add(jso, "mac", json_object_new_string(ap_mac));
	json_object_object_add(jso, "seqId", json_object_new_int(seqId++));

	int pair_num=0;
	for(i = 0; i < ret_len; i++) {
		
		if (0 == ret[i].icmp[0].max
			|| 0 == ret[i].icmp[0].min
			|| 0 == ret[i].icmp[0].avg) {
			continue;
		}
		
		pair_num++;
		
		json_object *l = json_object_new_object();
		json_object_object_add(l, "terminalMac", json_object_new_string(ret[i].terminalMac));
		
		int j=0, k=0;
		json_object *time64, *time128;
		
		time64 = json_object_new_array();
		time128 = json_object_new_array();
		
		json_object_array_add(time64, json_object_new_double(ret[i].icmp[0].max));
		json_object_array_add(time64, json_object_new_double(ret[i].icmp[0].min));
		json_object_array_add(time64, json_object_new_double(ret[i].icmp[0].avg));
		
		json_object_array_add(time128, json_object_new_double(ret[i].icmp[1].max));
		json_object_array_add(time128, json_object_new_double(ret[i].icmp[1].min));
		json_object_array_add(time128, json_object_new_double(ret[i].icmp[1].avg));
		
		json_object_object_add(l, "time64", time64);
		json_object_object_add(l, "time128", time128);

		json_object_array_add(data, l);
	}
	
	if (pair_num > 0)
	{
		json_object_object_add(jso, "data", data);
	}
	else 
	{
		json_object_put(data);
	}
	
	p = strdup(json_object_to_json_string(jso));
	
	json_object_put(jso);
	
	return p;
}

