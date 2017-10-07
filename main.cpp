#include "rawtobr3.h"

int do_the_job(FILE *stream_in, FILE *stream_out, config_store *config, int *a4);

int main(int argc, char **argv)
{
	// Can also be some kind of a struct
	int a4[4]; // [sp+54h] [bp-3C4h]@43

	// Args
	char func[255]; // [sp+6Bh] [bp-3ADh]@3
	char rcfile[255]; // [sp+16Ah] [bp-2AEh]@3
	char paperinf[255]; // [sp+269h] [bp-1AFh]@3
	int ps_n2; // [sp+368h] [bp-B0h]@47
	int ps_n1; // [sp+36Ch] [bp-ACh]@47
	config_store config; // [sp+370h] [bp-A8h]@1

	config.stream_in_set = 0;
	config.argv = argv;
	if ( argc > 8 )
	{
		return report_error(2, 0); // Too many args
	}

	config.stream_in = stdin;
	config.stream_out = stdout;
	paperinf[0] = 0;
	rcfile[0] = 0;
	func[0] = 0;

	for ( config.parsed_argc = 1; config.parsed_argc < argc; ++config.parsed_argc )
	{
		if(!strncmp(argv[config.parsed_argc], "-pi", 4))
		{
			++config.parsed_argc;
			strcpy(paperinf, config.argv[config.parsed_argc]);
		}
		else
		{
			if(!strncmp(config.argv[config.parsed_argc], "-rc", 4))
			{
				++config.parsed_argc;
				strcpy(rcfile, config.argv[config.parsed_argc]);
			}
			else
			{
				if(!strncmp(config.argv[config.parsed_argc], "-func", 6))
				{
					++config.parsed_argc;
					strcpy(func, config.argv[config.parsed_argc]);
				}
				else
				{
					if(!strncmp(config.argv[config.parsed_argc], "-flags", 7))
					{
						++config.parsed_argc;
						config.flags_len = strlen(config.argv[config.parsed_argc]);
						if ( config.flags_len > 32 )
							config.flags_len = 32;
						for ( config.parsed_flags = 0; config.parsed_flags < config.flags_len; ++config.parsed_flags )
							flags[config.parsed_flags] = config.argv[config.parsed_argc][config.flags_len - config.parsed_flags - 1];
					}
					else
					{
						// Input file
						if ( config.stream_in_set )
							return report_error(2, config.argv[config.parsed_argc]);
						config.stream_in_set = 1;
						config.stream_in = fopen(config.argv[config.parsed_argc], "rb");
						if ( !config.stream_in )
							return report_error(1, config.argv[config.parsed_argc]);
					}
				}
			}
		}
	}
	// Argv parsing is done
	// Open configs
	config.conf_stream = fopen(rcfile, "rt");
	if ( !config.conf_stream )
	{
		return report_error(1, rcfile);
	}
	if ( read_config(config.conf_stream, &config) )
	{
		return report_error(2, 0);
	}
	if ( !func[0] )
	{
		config.conf_fname_len = strlen(rcfile);
		if(config.conf_fname_len > 253)
		{
			return report_error(2, 0);
		}
		strcpy(func, rcfile);
		strcpy(func + config.conf_fname_len - 2, "cnuf");
	}
	if ( fill_tray_params(func, a4) )
	{
		return report_error(2, 0);
	}
	config.conf_stream = fopen(paperinf, "rt");
	if ( !config.conf_stream )
	{
		return report_error(1, paperinf);// Failed to open file
	}
	if ( read_two_numbers_after_paper_size(config.conf_stream, config.paper_size, &ps_n1, &ps_n2) )
	{
		return report_error(2, 0);
	}
	// Parse resolution
	switch(config.resolution)
	{
		case 1:
			config.ps_n1 = ps_n1 / 2;
			config.ps_n2 = ps_n2 / 2;
			break;
		case 3:
		case 4:
		case 5:
			config.ps_n1 = 2 * ps_n1;
			config.ps_n2 = 2 * ps_n2;
			break;
		case 6:
			config.ps_n1 = ps_n1;
			config.ps_n2 = ps_n2 / 2;
			break;
		default:
			config.ps_n1 = ps_n1;
			config.ps_n2 = ps_n2;
	}

	// Resolution parsing is done
	config.ps_n1_copy = config.ps_n1;
	config.ps_n2_copy = config.ps_n2;
	do_the_job(config.stream_in, config.stream_out, &config, a4);
	return 0;
}

int do_the_job(FILE *stream_in, FILE *stream_out, config_store *config, int *a4)
{
	converted_1 converted; // [sp+26h] [bp-62h]@1
	uint8_t *alloc_1; // [sp+48h] [bp-40h]@1
	uint8_t *ptr; // [sp+4Ch] [bp-3Ch]@18
	int ps_n1; // [sp+50h] [bp-38h]@1
	uint32_t ps_n2; // [sp+54h] [bp-34h]@1
	size_t size; // [sp+58h] [bp-30h]@1
	uint16_t i; // [sp+62h] [bp-26h]@18
	size_t alloc_1_read_total; // [sp+64h] [bp-24h]@17
	size_t v15; // [sp+68h] [bp-20h]@12
	size_t v22; // [sp+84h] [bp-4h]@28

	ps_n1 = config->ps_n1;
	ps_n2 = config->ps_n2;
	converted.field_2 = a4[3];
	converted.field_4 = a4[4];

	size = ps_n2 * ((ps_n1 + 7) / 8);
	alloc_1 = (uint8_t *)malloc(size);
	if(alloc_1)
	{
		global_alloc_2 = (uint8_t *)(malloc(5 * ps_n1 / 8));
		if(global_alloc_2)
		{
			global_alloc_3 = (uint8_t*)(malloc(1300u));
			if ( global_alloc_3 )
			{
				global_alloc_4 = (uint8_t*)(malloc(16448u));
				if(global_alloc_4)
				{
					converted_2 = &converted;
					convert_config(&converted, config);
					if ( converted_2->field_2 == 3 )
					{
						send_job_headers(stream_out);
						while(1)
						{
							v15 = (uint32_t)(ps_n1 + 7) >> 3;// (ps_n1 + 7) / 8 : divide with ceiling-rounding
							alloc_1_read_total = 0;

							ptr = alloc_1;                  // Always reached
							for ( i = 0; i < ps_n2; ++i )
							{
								v22 = fread(ptr, 1u, v15, stream_in);
								if ( v22 != v15 )
								break;
								ptr += v22;
								alloc_1_read_total += v22;
							}

							if ( size != alloc_1_read_total )
								break;

							send_converted_data(stream_out, alloc_1, ps_n1, ps_n2);
							flush_ga4(stream_out);
							fwrite_size_preceded(stream_out, "\x05" "1030M");
							fwrite_size_preceded(stream_out, "\x01" "\f");
						}
						if ( !((flags[0] - 48) & 1) )
						{
							fwrite_size_preceded(stream_out, "\t\x1B%-12345X");
							fwrite_size_preceded(stream_out, "\x1F@PJL EOJ NAME=\"Brother HL-XXX\"\n");
						}
						fwrite_size_preceded(stream_out, "\t\x1B%-12345X");
					}
					// Cleanup
					if ( alloc_1 )
						free(alloc_1);
					if ( global_alloc_2 )
						free(global_alloc_2);
					if ( global_alloc_3 )
						free(global_alloc_3);
					if ( global_alloc_4 )
						free(global_alloc_4);
					return 0;
				}
			}
		}
	}
	return 1;
}

int report_error(int reason, const char *message)
{
	switch(reason)
	{
		case 2:
			if ( message )
				fprintf(stderr, "Error: %s :invalid option !!\n", message);
			else
				fprintf(stderr, "Error: invalid option !!\n");
			break;
		case 3:
			fprintf(stderr, "Error:exception handling !!\n");
			break;
		case 1:
			if ( message )
				fprintf(stderr, "Error: %s :cannot open file !!\n", message);
			else
				fprintf(stderr, "Error: cannot open file !!\n");
			break;
	}
	return reason;
}
