#include "rawtobr3.h"

uint8_t* gl_al_2;
uint8_t* gl_al_3;
uint8_t* gl_al_4;

int do_the_job(FILE *stream_in, FILE *stream_out, config_store *config);

int main(int argc, char **argv)
{
	// Can also be some kind of a struct
	int tray_params[5];

	// Args
	char func[255];
	char rcfile[255];
	char paperinf[255];

	int height;
	int width;
	config_store config;

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

	// ARGV parsing
	for(config.parsed_argc = 1; config.parsed_argc < argc; ++config.parsed_argc)
	{
		if(!strcmp(argv[config.parsed_argc], "-pi"))
		{
			++config.parsed_argc;
			strcpy(paperinf, config.argv[config.parsed_argc]);
		}
		else
		{
			if(!strcmp(config.argv[config.parsed_argc], "-rc"))
			{
				++config.parsed_argc;
				strcpy(rcfile, config.argv[config.parsed_argc]);
			}
			else
			{
				if(!strcmp(config.argv[config.parsed_argc], "-func"))
				{
					++config.parsed_argc;
					strcpy(func, config.argv[config.parsed_argc]);
				}
				else
				{
					if(!strcmp(config.argv[config.parsed_argc], "-flags"))
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
	// ARGV parsing is done
	// Open configs
	config.conf_stream = fopen(rcfile, "rt");
	if(!config.conf_stream)
	{
		return report_error(1, rcfile);
	}
	if(read_config(config.conf_stream, &config))
	{
		return report_error(2, 0);
	}
	if(!func[0])
	{
		config.conf_fname_len = strlen(rcfile);
		if(config.conf_fname_len > 253)
		{
			return report_error(2, 0);
		}
		strcpy(func, rcfile);
		strcpy(func + config.conf_fname_len - 2, "cnuf");
	}

	if(fill_tray_params(func, tray_params))
	{
		return report_error(2, 0);
	}

	//read_selection_item();

	config.conf_stream = fopen(paperinf, "rt");
	if(!config.conf_stream)
	{
		return report_error(1, paperinf);// Failed to open file
	}
	if(get_pi_line(config.conf_stream, config.paper_size, &width, &height) )
	{
		return report_error(2, 0);
	}
	// Parse resolution
	switch(config.resolution)
	{
		case 1:
			config.width = width / 2;
			config.height = height / 2;
			break;
		case 3:
		case 4:
		case 5:
			config.width = 2 * width;
			config.height = 2 * height;
			break;
		case 6:
			config.width = width;
			config.height = height / 2;
			break;
		default:
			config.width = width;
			config.height = height;
	}

	// Resolution parsing is done
	config.ps_n1_copy = config.width;
	config.ps_n2_copy = config.height;
	do_the_job(config.stream_in, config.stream_out, &config);
	return 0;
}

int do_the_job(FILE *stream_in, FILE *stream_out, config_store *config)
{
	converted_1 converted; // [sp+26h] [bp-62h]@1
	size_t alloc_1_read_total;

	uint32_t orig_width  = config->width;
	uint32_t orig_height = config->height;
	//converted.always_3 = a4[3]; // 3
	//converted.field_4 = a4[4]; // Unknown and unused

	size_t converted_width = (uint32_t)(orig_width + 7) >> 3;
	size_t size = orig_height * converted_width;

	// alloc_1 is the buffer where all the input data is stored
	uint8_t* alloc_1 = (uint8_t *)malloc(size);

	if(alloc_1)
	{
		gl_al_2 = (uint8_t *)(malloc(5 * orig_width / 8));
		if(gl_al_2)
		{
			gl_al_3 = (uint8_t*)(malloc(1300u));
			if(gl_al_3)
			{
				gl_al_4 = (uint8_t*)(malloc(16448u));
				if(gl_al_4)
				{
					converted_2 = &converted;
					convert_config(&converted, config);
					//if(converted_2->always_3 == 3) // Always true
					{
						send_job_headers(stream_out);
						while(1)
						{
							alloc_1_read_total = 0;

							uint8_t* ptr = alloc_1;
							for(uint32_t i = 0; i < orig_height; ++i)
							{
								size_t nread = fread(ptr, 1, converted_width, stream_in);
								if(nread != converted_width)
									break;
								ptr += nread;
								alloc_1_read_total += nread;
							}

							if(size != alloc_1_read_total)
								break;

							send_converted_data(stream_out, alloc_1, orig_width, orig_height);
							flush_ga4(stream_out);
							fputs("1030M\f", stream_out);
						}
						if ( !((flags[0] - 48) & 1) )
						{
							fputs("\e%-12345X", stream_out);
							fputs("@PJL EOJ NAME=\"Brother HL-XXX\"\n", stream_out);
						}
						fputs("\e%-12345X", stream_out);
					}
					// Cleanup
					if(alloc_1)
						free(alloc_1);
					if(gl_al_2)
						free(gl_al_2);
					if(gl_al_3)
						free(gl_al_3);
					if(gl_al_4)
						free(gl_al_4);
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

int fill_tray_params(char*, int *a2)
{
	int v2; // ST10_4@3
	int v4; // [sp+14h] [bp-4h]@3

	read_selection_item();
	if(check_config_validity("Duplex", "ON") == 2)
		a2[2] = 1;
	v2 = check_config_validity("Paper Source", "MpTray");
	v4 = check_config_validity("Paper Source", "AutoSelect");
	if(v2 == 2 && v4 == 2)
		*a2 = 1;
	a2[3] = 3;
	a2[1] = 0;
	return 0;
}
