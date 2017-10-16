#include "rawtobr3.h"

valid_values_entry valid_values;
config_int_t* converted_2;

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

config_int_t* convert_config(config_int_t *obj, config_raw_t *config)
{
	int overflow; // [sp+3Ch] [bp-1Ch]@49
	paper_size* ps; // [sp+40h] [bp-18h]@49

	obj->resolution = config->resolution;
	if(obj->resolution > 6)
		obj->resolution = 2;

	double div = ((obj->resolution == 1) ? 2.5 : ((obj->resolution == 2 || obj->resolution == 6) ? 5 : 10));
	obj->width = config->width / div;
	obj->height = config->width / div;

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
	obj->duplex_type = config->duplex_type;
	obj->toner_save = !config->toner_save;
	obj->sleep_time = config->sleep_time;
	obj->copies = config->copies;

	//if(obj->always_3 == 3) // Always true
	{
		//const char* extra = "dummy";
		fetch_paper(&ps, obj->height, obj->width, &overflow, 0);
		div = ((obj->resolution == 1) ? 12 : ((obj->resolution == 2 || obj->resolution == 6) ? 6 : 3));
		overflow = overflow * 5 / div;
	}
	obj->converted_overflow = overflow >> 3;
	return obj;
}

int fetch_paper(paper_size **a1, size_t r_height, size_t r_width, int *overflow, const char **pi_code)
{
	paper_size paper_sizes_data[13] =
	{
	  { "MONARCH",    900,  465, NULL},
	  { "COM10",     1140,  495, NULL },
	  { "A6",         701,  496, "\x1B&ll4096a24a6d1E" },
	  { "DL",        1039,  520, NULL },
	  { "B6",         832,  590, NULL },
	  { "A5",         992,  701, "\x1B&l4096a25a6d1E" },
	  { "C5",        1082,  767, NULL },
	  { "B5",        1180,  832, NULL },
	  { "JISB5",     1214,  860, NULL },
	  { "EXECUTIVE", 1260,  870, NULL },
	  { "A4",        1403,  992, "\x1B&l4096a26a6d1E" }, // 10
	  { "LETTER",    1320, 1020, "\x1B&l4096a2a6d1E" },
	  { "LEGAL",     1680, 1020, "\x1B&l4096a3a6d1E" }
	};

	for(int i = 0; i <= 12; ++i)
	{
		if(paper_sizes_data[i].height >= r_height && paper_sizes_data[i].width >= r_width )
		{
			*a1 = &paper_sizes_data[i];
			if(pi_code && *pi_code)
				*pi_code = paper_sizes_data[i].extra_string;
			*overflow = 3 * (paper_sizes_data[i].width - r_width);
			return (i == 10);
		}
	}
	*a1 = &paper_sizes_data[12];
	if(pi_code && *pi_code)
	{
		*pi_code = paper_sizes_data[12].extra_string;
	}
	*overflow = 3 * (paper_sizes_data[12].width - r_width);
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
	const char* values[8] = {"Manual", "MpTray", "Tray1", "Tray2", "Tray3", "Tray4", "Tray5", "AutoSelect"};
	for(int i = 0; i < 8; ++i )
	{
		if(!strcmp(value, values[i]))
		{
			*out = i + 1;
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
	int num; // [sp+14h] [bp-4h]@1

	num = atoi(value);
	if ( num > 0 && num <= 999 )
		*out = num;
	return *out;
}

int parse_sleep_time(const char *value, int *out)
{
	if(!strcmp(value, "PrinterDefault")) // Printer default
	{
		*out = 0;
	}
	else
	{
		int num = atoi(value);
		if(num > 0 && num <= 99)
			*out = num;
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
	const char* pos = strchr(src, '=');
	if(!pos)
		return -1;

	size_t n = pos - src;
	strncpy(param_name, src, pos - src);
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

int get_pi_line(FILE *stream, int paper_size, int *sizea, int *sizeb)
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
	char strbuf[1025];
	size_t size;
	int i;

	valid_values_entry* curr = &valid_values;
	while(get_default_req(strbuf, 1024) && strbuf != strstr(strbuf, "[SelectionItem]"));
	while(get_default_req(strbuf, 1024) )
	{
		size = strlen(strbuf);
		++size;
		curr->parname = (char *)calloc(1u, size) + 1;
		if ( !curr->parname )
			break;
		curr->next = (valid_values_entry *)(calloc(sizeof(valid_values_entry) /*0x90*/, 1u));
		if ( !curr->next )
			break;
		strcpy(curr->parname, strbuf);
		curr->string2 = curr->parname;
		char* s = strchr(curr->parname, '=');
		if(s)
		{
			*s++ = 0;
			if(*s)
			{
				s = strchr(s, '{');
				if ( s )
				{
					*s++ = 0;
					for(i = 0; i <= 31; ++i)
					{
						if(*s)
							curr->argv[i] = s;
						char* s2 = s;
						s = strchr(s, ',');
						if(!s)
						{
							s = s2;
							s = strchr(s2, '}');
							if ( s )
								*s = 0;
							++i;
							break;
						}
						*s++ = 0;
					}
					curr->argc = i;
					curr = curr->next;
					if ( strbuf[0] == '[' )
						break;
				}
			}
		}
	}
	curr->next = 0;
	return 0;
}

int check_config_validity(const char *s2, const char *s1)
{
	valid_values_entry *obj = &valid_values; // [sp+1Ch] [bp-Ch]@1
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

char *fgets_line(char *s, int n, FILE *stream)
{
	char* raw; // [sp+20h] [bp-8h]@1
	char* pos; // [sp+24h] [bp-4h]@2

	raw = fgets(s, n, stream);
	if(raw)
	{
		pos = strchr(s, '\n');
		if(pos)
			*pos = 0;
	}
	return raw;
}

int read_config(FILE *stream, config_raw_t *conf)
{
	char value[1024] = {0};
	char buf[1024] = {0};
	char s[1024] = {0};
	bool flag = 0;

	if(!stream || !conf)
		return -1;

	fseek(stream, 0, 0);
	while(fgets_line(s, 1024, stream))
	{
		truncate_spaces(s);
		if(s[0] != '#') // Ignore comments
		{
			if(s[0] == '[') // Check sections
			{
				if(flag)
					return -1;
				flag = 1;
				if(sscanf(s, "[%[^]]s", buf) == -1)
					return -1;
				strcpy(conf->somestring1, buf);
			}
			else if(s[0])
			{
				if(!flag)
					return -1;
				switch(parse_config_string(s, value)) // returns param ID, sets s1 to param value
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
				}
			}
		}
	}
	return 0;
}

char *truncate_spaces(char *s1)
{
  char s[1025]; // [sp+1Fh] [bp-409h]@1
  int i; // [sp+420h] [bp-8h]@1
  int j; // [sp+424h] [bp-4h]@1

  memset(s, 0, 0x401u);
  j = 0;
  for ( i = 0; s1[i]; ++i )
  {
	if ( s1[i] != ' ' && s1[i] != '\t' )
	  s[j++] = s1[i];
  }
  if ( strcmp(s1, s) )
  {
	if ( s[j] )
	  s[j] = 0;
	memset(s1, 0, 0x401u);
	strcpy(s1, s);
  }
  return s1;
}
