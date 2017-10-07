#include "rawtobr3.h"

request_valid_values GlobalStruct1;

const char *config_fields_3[9] =
{
  "Resolution",
  "PaperSource",
  "Duplex",
  "DuplexType",
  "PaperType",
  "Media",
  "Copies",
  "Sleep",
  "TonerSaveMode"
};

converted_1 *convert_config(converted_1 *obj, config_store *config)
{
	int a4; // [sp+3Ch] [bp-1Ch]@49
	paper_size *v8; // [sp+40h] [bp-18h]@49

	obj->resolution = config->resolution;
	if(obj->resolution > 6)
		obj->resolution = 2;

	obj->exists = 1;
	switch(config->resolution)
	{
		case 1:
			obj->ps_n1 = config->ps_n1_copy / 2.5;
			obj->ps_n2 = config->ps_n2_copy / 2.5;
			break;
		case 3:
		case 4:
		case 5:
			obj->ps_n1 = config->ps_n1_copy / 10;
			obj->ps_n2 = config->ps_n2_copy / 10;
			break;

		default:
			obj->ps_n1 = config->ps_n1_copy / 5;
			obj->ps_n2 = config->ps_n2_copy / 5;
	}

	// Sourcetray
	if(config->tray_1 >= 3 && config->tray_1 <= 7)
		obj->sourcetray = config->tray_1 - 2;
	else if(config->tray_1 == 1 || config->tray_1 == 2)
		obj->sourcetray = config->tray_1 + 5;
	else
		obj->sourcetray = 0;

	obj->mediatype = config->mediatype - 1;
	if(obj->mediatype > 9)
		obj->mediatype = 0;
	obj->duplex = config->duplex;
	obj->always_1 = 1;
	obj->duplex_type = config->duplex_type;
	obj->toner_save = config->toner_save;
	obj->sleep_time = config->sleep_time;
	obj->copies = config->copies;

	if ( obj->field_2 == 3 )
	{
		find_paper_size_0(&v8, obj->ps_n2, obj->ps_n1, &a4, 0);
		switch ( obj->resolution )
		{
			case 3:
			case 4:
			case 5:
				a4 = 5 * a4 / 3;
				break;

			case 2:
			case 6:
				a4 = 5 * a4 / 6;
				break;

			default:
				a4 /= 2.4;
				break;
		}
	}
	obj->field_1A = a4 / 8;
	return obj;
}

int find_paper_size_0(paper_size **a1, uint16_t sizea, int16_t sizeb, int *act_sizeb, const char **extra_string)
{
	paper_size paper_sizes_data[13] =
	{
	  { "MONARCH", 0, 900, 465, NULL},
	  { "COM10", 0, 1140, 495, NULL },
	  { "A6", 0, 701, 496, "\x1B&ll4096a24a6d1E" },
	  { "DL", 0, 1039, 520, NULL },
	  { "B6", 0, 832, 590, NULL },
	  { "A5", 0, 992, 701, "\x1B&l4096a25a6d1E" },
	  { "C5", 0, 1082, 767, NULL },
	  { "B5", 0, 1180, 832, NULL },
	  { "JISB5", 0, 1214, 860, NULL },
	  { "EXECUTIVE", 0, 1260, 870, NULL },
	  { "A4", 0, 1403, 992, "\x1B&l4096a26a6d1E" },
	  { "LETTER", 0, 1320, 1020, "\x1B&l4096a2a6d1E" },
	  { "LEGAL", 0, 1680, 1020, "\x1B&l4096a3a6d1E" }
	};

	for(int i = 0; i <= 12; ++i)
	{
		if(paper_sizes_data[i].sizea >= sizea && (int16_t)(paper_sizes_data[i].sizeb) >= sizeb )
		{
			*a1 = &paper_sizes_data[i];
			*act_sizeb = (int16_t)(paper_sizes_data[i].sizeb);
			if ( extra_string && *extra_string )
			*extra_string = paper_sizes_data[i].extra_string;
			if ( i == 10 )
			*act_sizeb = 3 * (*act_sizeb - sizeb);
			return 1;
		}
	}
	*a1 = &paper_sizes_data[12];
	*act_sizeb = (int16_t)(paper_sizes_data[12].sizeb);
	if(extra_string && *extra_string)
	{
		*extra_string = paper_sizes_data[12].extra_string;
	}
	*act_sizeb = 3 * (*act_sizeb - sizeb);
	return 0;
}

char* get_default_req(char* dest, size_t limit)
{
	const char* def_config[26] =
	{
		"[XXXXXXXX]",
		"",
		"[default]",
		"Resolution={600}",
		"Paper Source={Tray1}",
		"Duplex={OFF}",
		"Duplex Type={Long}",
		"Paper Type={(LANG_USA,LANG_CANADA)=Letter,A4}",
		"Media={PlainPaper}",
		"Copies={1}",
		"Sleep={PrinterDefault}",
		"Sleep Count={5}",
		"Toner Save Mode={OFF}",
		"",
		"[SelectionItem]",
		"Resolution={300,600,1200,HQ1200A,HQ1200B,600x300}",
		"Resolution2={300,600,1200,HQ1200A,HQ1200B,600x300}",
		"Paper Source={Manual,Tray1,Tray2,Tray3,MpTray,AutoSelect}",
		"Duplex={OFF,ON}",
		"Duplex Type={Long,Short}",
		"Paper Type={A4,Letter,Legal,Exective,A5,A6,B5,JISB5,B6,C5,"
		"DL,Com-10,Monarch,PC4x6,Postcard,DHagaki,EnvYou4,FOLIO,A4L,"
		"CUSTOM1,CUSTOM2,CUSTOM3,CUSTOM4,CUSTOM5,CUSTOM6,CUSTOM7,CUSTOM8,CUSTOM9}",
		"Media={PlainPaper,ThinPaper,ThickPaper,Thicker Paper,BondPaper,Transparencies,Envelopes,Env.Thick,Env.Thin,Recycled}",
		"Copies={\"1-999\"}",
		"Sleep={PrinterDefault,\"1-99\"}",
		"Toner Save Mode={ON,OFF}"
	};
	static int index = 0;

	if(!dest)
		return 0;
	if(index > 26)
		return 0;

	const char* src = def_config[index];
	if(!src)
		return 0;
	if(strlen(src) + 2 > limit)
		return 0;

	strncpy(dest, src, limit - 2);
	// Append '\r' to the end
	dest[strlen(dest) + 1] = 0;
	dest[strlen(dest)] = 0x0A;
	++index;
	return dest;
}

int parse_lang(const char *value, int *out)
{
	int i;
	const char *values[3] = {"LANG_USA", "LANG_CANADA", "LANG_OTHER"};
	for(i = 0; i < 4; ++i)
	{
		if(!strcmp(value, values[i]))
		{
			*out = i;
			return *out;
		}
	}
	return *out;
}

int parse_resolution(const char *value, int *out)
{
	const char* values[7] = {"NULL", "300", "600", "1200", "HQ1200A", "HQ1200B", "600x300"};
	for(int i = 1; i < 7; ++i)
	{
		if(!strcmp(value, values[i]))
		{
			*out = i;
			return *out;
		}
	}
	return *out;
}

int parse_tray_1(const char *value, int *out)
{
	const char* values[9] = {"NULL", "Manual", "MpTray", "Tray1", "Tray2", "Tray3", "Tray4", "Tray5", "AutoSelect"};
	for(int i = 1; i < 9; ++i )
	{
		if(!strcmp(value, values[i]))
		{
			*out = i;
			return *out;
		}
	}
	return *out;
}

int parse_duplex(const char *value, int *out)
{
	const char* values[2] = {"OFF", "ON"};
	for(int i = 0; i < 2; ++i)
	{
		if(!strcmp(value, values[i]))
		{
			*out = i;
			break;
		}
	}
	if(!strcmp(value, "None"))
		*out = 0;
	return *out;
}

int parse_duplex_type(const char *value, int *out)
{
	const char* values[3] = {"NULL", "Long", "Short"};
	for(int i = 1; i < 3; ++i)
	{
		if(!strcmp(value, values[i]))
		{
			*out = i;
			break;
		}
	}
	if(!strcmp(value, "None"))
		*out = 1; // Shouldn't it be 0?
	return *out;
}

int parse_paper_size(const char *value, int *out)
{
	static const char* paper_sizes[] =
	{
		"NULL", "A4", "Letter", "Legal", "Exective",
		"A5", "A6", "B5", "B6", "C5",
		"DL", "Com-10", "Monarch", "JISB5", "Postcard",
		"DHagaki", "EnvYou4", "PC4x6", "FOLIO", "A4L",
		"CUSTOM0", "CUSTOM1", "CUSTOM2", "CUSTOM3", "CUSTOM4",
		"CUSTOM5", "CUSTOM6", "CUSTOM7", "CUSTOM8", "CUSTOM9"
	};

	*out = 0;
	for(int i = 1; i < 30; ++i)
	{
		if(!strcmp(value, paper_sizes[i]))
		{
			*out = i;
			return *out;
		}
	}
	return *out;
}

int parse_mediatype(const char *value, int *out)
{
	const char* values[11] = {"NULL", "PlainPaper", "ThinPaper", "ThickPaper", "ThickerPaper",
							  "BondPaper", "Transparencies", "Envelopes", "Env.Thick", "Env.Thin", "Recycled"};

	for(int i = 1; i < 11; ++i)
	{
		if(!strcmp(value, values[i]))
		{
			*out = i;
			return *out;
		}
	}
	return *out;
}

int parse_copies(const char *value, int *out)
{
	int32_t v3; // [sp+14h] [bp-4h]@1

	v3 = atoi(value);
	if ( v3 > 0 && v3 <= 999 )
		*out = v3;
	return *out;
}

int parse_sleep_time(const char *value, int *out)
{
	if(!strcmp(value, "PRINTER")) // Printer default
	{
		*out = 0;
	}
	else
	{
		int v3 = atoi(value);
		if(v3 > 0 && v3 <= 99)
			*out = v3;
	}
	return *out;
}

int parse_toner_save(const char *value, int *out)
{
	const char* vals[2] = {"ON", "OFF"};
	for(int i = 0; i < 2; ++i)
	{
		if(!strcmp(value, vals[i]))
		{
			*out = i;
			return *out;
		}
	}
	return *out;
}

int parse_config_string(const char *src, char *value)
{
	char param_name[1025] = {0};
	memset(param_name, 0, 0x401u);
	const char* v8 = strchr(src, '=');
	if(!v8)
		return -1;

	size_t n = v8 - src;
	strncpy(param_name, src, v8 - src);
	strcpy(value, &src[n + 1]);
	for(int i = 0; i <= 8; ++i)
	{
		if ( !strncmp(param_name, config_fields_3[i], 0x401u) )
		{
			return i;
		}
	}
	return -1;
}

int read_two_numbers_after_paper_size(FILE *stream, int paper_size, int *sizea, int *sizeb)
{
	static const char* paper_sizes[] =
	{
		"NULL", "A4", "LT", "LGL", "EXE",
		"A5", "A6", "B5", "B6", "C5",
		"DL", "COM-10", "MONARCH", "JISB5", "POSTCARD",
		"DHAGAKI", "ENVYOU4", "PC4X6", "FOLIO", "A4L",
		"CUSTOM0", "CUSTOM1", "CUSTOM2", "CUSTOM3", "CUSTOM4",
		"CUSTOM5", "CUSTOM6", "CUSTOM7", "CUSTOM8", "CUSTOM9"
	};

	char dest[10]; // [sp+19h] [bp-41Fh]@5
	char read_buffer[1024]; // [sp+23h] [bp-415h]@4

	if(!stream)
		return -1;

	while(fgets_line(read_buffer, 1024, stream))
	{
		if(!strncmp(read_buffer, paper_sizes[paper_size], strlen(paper_sizes[paper_size])))
		{
			char* s = strchr(read_buffer, '\t') + 1;
			char* nptr = strchr(s, '\t');
			size_t n = nptr - s;
			strncpy(dest, s, nptr - s);
			dest[n + 1] = 0;
			*sizea = atoi(dest);
			++nptr;
			*sizeb = atoi(nptr);
			return 0;
		}
	}
	return 1;
}


int read_selection_item()
{
	char strbuf[1024]; // [sp+20h] [bp-418h]@2
	size_t size; // [sp+424h] [bp-14h]@5
	int i; // [sp+428h] [bp-10h]@10
	char *s; // [sp+42Ch] [bp-Ch]@7
	char *v6; // [sp+430h] [bp-8h]@13

	request_valid_values* v2 = &GlobalStruct1;
	while(get_default_req(strbuf, 1024) && strbuf != strstr(strbuf, "[SelectionItem]"));
	while(get_default_req(strbuf, 1024) )
	{
		size = strlen(strbuf);
		++size;
		v2->parname = (char *)calloc(1u, size);
		if ( !v2->parname )
			break;
		v2->next = (request_valid_values *)calloc(0x90u, 1u);
		if ( !v2->next )
			break;
		strcpy(v2->parname, strbuf);
		v2->string2 = v2->parname;
		s = strchr(v2->parname, '=');
		if ( s )
		{
			*s++ = 0;
			if ( *s )
			{
				s = strchr(s, '{');
				if ( s )
				{
					*s++ = 0;
					for ( i = 0; i <= 31; ++i )
					{
						if ( *s )
							v2->argv[i] = s;
						v6 = s;
						s = strchr(s, ',');
						if ( !s )
						{
							s = v6;
							s = strchr(v6, '}');
							if ( s )
								*s = 0;
							++i;
							break;
						}
						*s++ = 0;
					}
					v2->argc = i;
					v2 = v2->next;
					if ( strbuf[0] == '[' )
						break;
				}
			}
		}
	}
	v2->next = 0;
	return 0;
}

int check_config_validity(const char *s2, const char *s1)
{
	request_valid_values *obj = &GlobalStruct1; // [sp+1Ch] [bp-Ch]@1
	bool found = 0;
	int i; // [sp+24h] [bp-4h]@4

	while ( obj && obj->next )
	{
		if ( !strcmp(obj->string2, s2) )
		{
			found = 1;
			for ( i = 0; obj->argc > i; ++i )
			{
				if ( !strcmp(s1, obj->argv[i]) )
					return 2;
			}
		}
		obj = obj->next;
	}
	return found;
}

int fill_tray_params(char*, int *a2)
{
	int v2; // ST10_4@3
	int v4; // [sp+14h] [bp-4h]@3

	read_selection_item();
	if ( check_config_validity("Duplex", "ON") == 2 )
		a2[2] = 1;
	v2 = check_config_validity("Paper Source", "MpTray");
	v4 = check_config_validity("Paper Source", "AutoSelect");
	if ( v2 == 2 && v4 == 2 )
		*a2 = 1;
	a2[3] = 3;
	a2[1] = 0;
	return 0;
}

char *fgets_line(char *s, int n, FILE *stream)
{
  char *v4; // [sp+20h] [bp-8h]@1
  char *v5; // [sp+24h] [bp-4h]@2

  v4 = fgets(s, n, stream);
  if ( v4 )
  {
	v5 = strchr(s, 10);
	if ( v5 )
	  *v5 = 0;
  }
  return v4;
}

int read_config(FILE *stream, config_store *conf)
{
  int32_t v3; // [sp+Ch] [bp-C9Ch]@3
  char v4[112]; // [sp+24h] [bp-C84h]@1
  char value[1024]; // [sp+9Dh] [bp-C0Bh]@1
  char buf[1024]; // [sp+49Eh] [bp-80Ah]@1
  char s[1024]; // [sp+89Fh] [bp-409h]@1
  int v8; // [sp+CA0h] [bp-8h]@1
  int v9; // [sp+CA4h] [bp-4h]@1

  v8 = 0;
  v9 = 0;
  memset(s, 0, 0x401u);
  memset(buf, 0, 0x401u);
  memset(value, 0, 0x401u);
  memset(v4, 0, 0x78u);
  if ( stream && conf )
  {
	fseek(stream, 0, 0);
	v8 = 1;
	while ( fgets_line(s, 1024, stream) )
	{
	  truncate_spaces(s);
	  if ( s[0] != '#' )                        // Ignore comments
	  {
		if ( s[0] == '[' )                      // Check sections
		{
		  if ( v9 )
			return -1;
		  v9 = 1;
		  if ( sscanf(s, "[%[^]]s", buf) == -1 )
			return -1;
		  strcpy(conf->somestring1, buf);
		}
		else if ( s[0] )
		{
		  if ( !v9 )
			return -1;
		  switch ( parse_config_string(s, value) )// returns param ID, sets s1 to param value
		  {
			case 99:
			  parse_lang(value, &conf->country_2);
			  break;
			case 0:
			  parse_resolution(value, &conf->resolution);
			  break;
			case 1:
			  parse_tray_1(value, &conf->tray_1);
			  break;
			case 2:
			  parse_duplex(value, &conf->duplex);
			  break;
			case 3:
			  parse_duplex_type(value, &conf->duplex_type);
			  break;
			case 4:
			  parse_paper_size(value, &conf->paper_size);
			  break;
			case 5:
			  parse_mediatype(value, &conf->mediatype);
			  break;
			case 6:
			  parse_copies(value, &conf->copies);
			  break;
			case 7:
			  parse_sleep_time(value, &conf->sleep_time);
			  break;
			case 8:
			  parse_toner_save(value, &conf->toner_save);
			  break;
			default:
			  break;
		  }
		}
	  }
	  ++v8;
	}
	v3 = 0;
  }
  else
  {
	v3 = -1;
  }
  return v3;
}

char *truncate_spaces(char *s1)
{
  char s[1025]; // [sp+1Fh] [bp-409h]@1
  int i; // [sp+420h] [bp-8h]@1
  int v4; // [sp+424h] [bp-4h]@1

  memset(s, 0, 0x401u);
  v4 = 0;
  for ( i = 0; s1[i]; ++i )
  {
	if ( s1[i] != ' ' && s1[i] != '\t' )
	  s[v4++] = s1[i];
  }
  if ( strcmp(s1, s) )
  {
	if ( s[v4] )
	  s[v4] = 0;
	memset(s1, 0, 0x401u);
	strcpy(s1, s);
  }
  return s1;
}
