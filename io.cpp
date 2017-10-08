#include "rawtobr3.h"

uint8_t* global_alloc_2;
uint8_t* global_alloc_3;
uint8_t* global_alloc_4;

size_t ga4_size;
uint8_t *ga_2;
uint8_t *ga_3;
uint16_t ga4_appends;
uint8_t ga4_preceeder;

char flags[32] = {0};

uint16_t row_width;
uint16_t sendbuf_size;
uint8_t *sending_row;
uint8_t *last_sent_row;
uint16_t row_width_0;
char nonzero_found_1;

int send_job_headers(FILE *stream)
{
	paper_size *ps;

	fputs("\e%-12345X@PJL\n", stream);
	if ( !((flags[0] - '0') & 1) )
	fputs("@PJL JOB NAME=\"Brother HL-XXX\"\n", stream);
	switch ( converted_2->resolution )
	{
		case 3:
			fputs("@PJL SET RESOLUTION = 1200\n@PJL SET PAPERFEEDSPEED=HALF\n", stream);
			break;
		case 4:
			fputs("@PJL SET RESOLUTION = 600\n", stream);
			fputs("@PJL SET RAS1200MODE = TRUE\n", stream);
			break;
		case 5:
			fputs("@PJL SET RESOLUTION = 1200\n@PJL SET PAPERFEEDSPEED=FULL\n", stream);
			break;
		case 6:
			fputs("@PJL SET RESOLUTION = 600\n", stream);
			break;
		case 2:
			if(!(((flags[1] - '0') >> 1) & 1))
				fputs("@PJL SET RAS1200MODE = OFF\n", stream);
			if((flags[1] - '0') & 1)
			{
				fputs("@PJL SET RESOLUTION = 600\n", stream);
				fputs("@PJL SET HQMMODE = ON\n", stream);
			}
			else
			{
				fputs("@PJL SET RESOLUTION = 600\n", stream);
			}
			break;
		case 1:
			if(!(((flags[1] - '0') >> 1) & 1))
				fputs("@PJL SET RAS1200MODE = OFF\n", stream);
			fputs("@PJL SET RESOLUTION = 300\n", stream);
			break;
		default:
			if(!(((flags[1] - '0') >> 1) & 1))
				fputs("@PJL SET RAS1200MODE = OFF\n", stream);
			fputs("@PJL SET RESOLUTION = 600\n", stream);
			break;
	}
	if ( converted_2->toner_save )
		fputs("@PJL SET ECONOMODE = ON\n", stream);
	else
		fputs("@PJL SET ECONOMODE = OFF\n", stream);
	if ( converted_2->sourcetray != 6 )
	{
		switch ( converted_2->sourcetray )
		{
			case 1:
				fputs("@PJL SET SOURCETRAY = TRAY1\n", stream);
				break;
			case 2:
				fputs("@PJL SET SOURCETRAY = TRAY2\n", stream);
				break;
			case 3:
				fputs("@PJL SET SOURCETRAY = TRAY3\n", stream);
				break;
			case 4:
				fputs("@PJL SET SOURCETRAY = TRAY4\n", stream);
				break;
			case 5:
				fputs("@PJL SET SOURCETRAY = TRAY5\n", stream);
				break;
			case 7:
				fputs("@PJL SET SOURCETRAY = MPTRAY\n", stream);
				break;
			default:
				fputs("@PJL SET SOURCETRAY = AUTO\n", stream);
				break;
		}
	}
	switch(converted_2->mediatype)
	{
		case 1:
			fputs("@PJL SET MEDIATYPE = THIN\n", stream);
			break;
		case 2:
			fputs("@PJL SET MEDIATYPE = THICK\n", stream);
			break;
		case 3:
		case 4:
			fputs("@PJL SET MEDIATYPE = THICK2\n", stream);
			break;
		case 5:
			fputs("@PJL SET MEDIATYPE = TRANSPARENCY\n", stream);
			break;
		case 6:
			fputs("@PJL SET MEDIATYPE = ENVELOPES\n", stream);
			break;
		case 7:
			fputs("@PJL SET MEDIATYPE = ENVTHICK\n", stream);
			break;
		case 9:
			fputs("@PJL SET MEDIATYPE = RECYCLED\n", stream);
			break;
		default:
			fputs("@PJL SET MEDIATYPE = REGULAR\n", stream);
	}
	if ( converted_2->sleep_time )
	{
		fprintf(stream,
				"@PJL DEFAULT AUTOSLEEP=ON\n"
				"@PJL DEFAULT TIMEOUTSLEEP=%d\n"
				"@PJL SET AUTOSLEEP=ON\n"
				"@PJL SET TIMEOUTSLEEP=%d\n",
				converted_2->sleep_time,
				converted_2->sleep_time);
	}

	const char* src = "dummy";
	int act_sizeb;
	fetch_paper(&ps, converted_2->sizea, converted_2->sizeb, &act_sizeb, &src);
	fputs("@PJL SET ORIENTATION = PORTRAIT\n", stream);
	if(!src || !*src)
	{
		fprintf(stream, "@PJL SET PAPER = %s\n", ps->PaperSizeName);
	}

	fputs("@PJL SET PAGEPROTECT = AUTO\n", stream);
	fputs("@PJL ENTER LANGUAGE = PCL\n", stream);
	fputs("\eE", stream);    // Reset the perimeter
	if(src && *src)
	{
		fprintf(stream, "%s", src);
	}
	switch ( converted_2->resolution )
	{
		case 0:
			fputs("\e&u150D", stream);
			fputs("\e*t150R", stream);
			break;
		case 1:
			fputs("\e&u300D", stream);
			fputs("\e*t300R", stream);
			break;
		case 2:
		case 6:
			fputs("\e&u600D", stream);
			fputs("\e*t600R", stream);
			break;
		case 3:
		case 5:
			fputs("\e&u1200D", stream);
			fputs("\e*t1200R", stream);
			break;
		case 4:
			fputs("\e&u1200D", stream);
			fputs("\e*t600R" , stream);
			break;
	}

	if(converted_2->sourcetray == 6)
		fputs("\e&l2H", stream);

	if(converted_2->copies > 1)
	{
		fprintf(stream, "\e&l%dX", converted_2->copies);
	}
	if(converted_2->duplex)
	{
		if(converted_2->duplex_type)
			return !fputs("\e&l1S", stream);
		else
			return !fputs("\e&l2S", stream);
	}
	return 0;
}

void send_converted_data(FILE *stream_out, uint8_t *data, int orig_width, int orig_height)
{
	uint8_t v6; // [sp+27h] [bp-21h]@30
	int send_size_2; // [sp+2Ch] [bp-1Ch]@30
	int send_size_1; // [sp+30h] [bp-18h]@47
	uint8_t *data_logical; // [sp+38h] [bp-10h]@7
	uint16_t converted_sizeb; // [sp+3Eh] [bp-Ah]@35
	uint16_t v13; // [sp+42h] [bp-6h]@35
	int v14; // [sp+44h] [bp-4h]@14

	int16_t row_width_2 = (uint32_t)(orig_width + 7) >> 3;// Always reached
	row_width = row_width_2;

	static int iteration = 0;
	++iteration;

	switch ( converted_2->resolution )
	{
		case 1:
		  data_logical = &data[50 * row_width_2];
		  orig_height -= 100;
		  break;
		case 2:
		  data_logical = &data[100 * row_width_2];
		  orig_height -= 200;
		  break;
		case 6:
		  data_logical = &data[50 * row_width_2];
		  orig_height -= 100;
		  break;
		default:
		  data_logical = &data[200 * row_width_2];
		  orig_height -= 400;
		  break;
	}
	switch ( converted_2->resolution )
	{
		case 1:
			v14 = 6;
			row_width -= 12;
			break;
		case 3:
		case 4:
		case 5:
			v14 = 25;
			row_width -= 50;
			break;
		default:
			v14 = 12;
			row_width -= 24;
			break;
	}

	// Looks like a global struct initialization?..
	ga_2 = global_alloc_2;
	ga_3 = global_alloc_3;
	fwrite_size_preceded(stream_out, "\b\e*b1030m");// Unknown PCL command \e*b1030m
	ga4_size = 0;
	ga4_appends = 0;
	ga4_preceeder = 0;

	// This statement should ALWAYS be true, so it's commented out
	//if(1 || nonzero_found_1)
	{
		v13 = 624;
		converted_sizeb = converted_2->converted_sizeb;
		if(converted_2->resolution == 1)
		{
			v13 /= 2;
		}
		if(converted_2->resolution >= 3 && converted_2->resolution <= 5)
		{
			v13 *= 2;
		}
		if(row_width > v13 - converted_sizeb)
			row_width = v13 - converted_sizeb;
		for(int i = 0; i <= orig_height; ++i)
		{
			sending_row = &data_logical[v14];
			if(converted_2->resolution == 4 && i & 1)
			{
				send_size_2 = 1;
				v6 = 0;
				buffered_send(stream_out, &send_size_2, &v6);
			}
			else
			{
				send_size_1 = row_width;
				if(is_all_zeroes(sending_row, row_width))
				{
					send_size_2 = 1;
					v6 = 0xFFu;
					buffered_send(stream_out, &send_size_2, &v6);
				}
				else
				{
					if(ga4_appends < 127 || !i)
					{
						last_sent_row = ga_2 + (2 * row_width) + (row_width >> 1);
						row_width_0 = row_width;
						bitwise_invert(sending_row, last_sent_row, row_width);// The whole row is in bitwise-inverted state present somewhere in ga_3
					}
					convert_ga3_t();
					send_size_1 = sendbuf_size;
					if ( sendbuf_size )
					{
						buffered_send(stream_out, &send_size_1, ga_2);
					}
					else
					{
						send_size_2 = 1;
						v6 = 0;
						buffered_send(stream_out, &send_size_2, &v6);
					}
				}
				last_sent_row = sending_row;
				row_width_0 = row_width;
			}
			data_logical += row_width_2;
			if ( converted_2->resolution == 6 )
			{
				send_size_2 = 1;
				v6 = 0;
				buffered_send(stream_out, &send_size_2, &v6);
			}
		}
	}
	// This part is never executed, so it is commented out, but saved just in case something goes wrong
	/*else
	{
		if(converted_2->resolution == 4 )
			ps_n2 /= 2;
		send_size_2 = 1;
		v6 = 0xFF;
		for(; ps_n2 >= 0; --ps_n2)
		{
			buffered_send(stream_out, &send_size_2, &v6);
			if ( converted_2->resolution == 6 )
				buffered_send(stream_out, &send_size_2, &v6);
		}
	}*/
}

int fwrite_size_preceded(FILE *s, const char* a2)
{
	size_t n; // ST24_4@2

	if(*a2 > 0)
	{
		n = *a2;
		if(fwrite(a2 + 1, 1u, n, s) != n)
			return 1;
	}
	return 0;
}

int fwrite_wrapper(FILE *stream, void *data, size_t n)
{
	return (n && fwrite(data, 1, n, stream) != n);
}

char buffered_send(FILE *a1, int* size_ptr, uint8_t* data)
{
	int size; // [sp+20h] [bp-8h]@1

	size = *size_ptr;
	if(size + ga4_size > 0x4000 || ga4_appends > 127)
		flush_ga4(a1);
	memcpy(global_alloc_4 + ga4_size, data, size);
	ga4_size += size;
	++ga4_appends;
	return ++ga4_preceeder;
}

int flush_ga4(FILE *stream)
{
	int result = ga4_size;
	if(result)
	{
		fprintf(stream, "%zu" "%cw" "%c%c", ga4_size + 2, 1, 0, ga4_preceeder);
		result = fwrite_wrapper(stream, global_alloc_4, ga4_size);
	}
	ga4_size = 0;
	ga4_preceeder = 0;
	return result;
}
