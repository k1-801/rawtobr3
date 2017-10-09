#include "rawtobr3.h"

size_t ga4_size;
uint16_t ga4_appends;
uint8_t ga4_preceeder;

char flags[32] = {0};

size_t row_width;
size_t sendbuf_size;
uint8_t* sending_row;
uint8_t* last_sent_row;
size_t row_width_0;

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
	if(converted_2->sleep_time)
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

void send_converted_data(FILE *stream_out, uint8_t* data, size_t orig_width, size_t orig_height)
{
	uint8_t* data_logical;
	size_t row_width_2 = (orig_width + 7) >> 3;// Always reached
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

	int v14;
	switch(converted_2->resolution)
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

	fputs("\e*b1030m", stream_out); // Unknown PCL command \e*b1030m
	ga4_size = 0;
	ga4_appends = 0;
	ga4_preceeder = 0;

	// This statement should ALWAYS be true, so it's commented out
	//if(1 || nonzero_found_1)
	{
		size_t v13 = 624;
		size_t converted_sizeb = converted_2->converted_sizeb;
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
		for(size_t i = 0; i <= orig_height; ++i)
		{
			sending_row = &data_logical[v14];
			if(converted_2->resolution == 4 && i & 1)
			{
				uint8_t sb = 0;
				buffered_send(stream_out, 1, &sb);
			}
			else
			{
				if(is_all_zeroes(sending_row, row_width))
				{
					uint8_t sb = 0xFF;
					buffered_send(stream_out, 1, &sb);
				}
				else
				{
					if(!(ga4_appends % 128) || !i)
					{
						last_sent_row = gl_al_2 + (2 * row_width) + (row_width >> 1);
						row_width_0 = row_width;
						bitwise_invert(sending_row, last_sent_row, row_width); // The whole row is in bitwise-inverted state present somewhere in ga_3
					}
					convert_2();
					if(sendbuf_size)
					{
						buffered_send(stream_out, sendbuf_size, gl_al_2);
					}
					else
					{
						uint8_t sb = 0;
						buffered_send(stream_out, 1, &sb);
					}
				}
				last_sent_row = sending_row;
				row_width_0 = row_width;
			}
			data_logical += row_width_2;
			if ( converted_2->resolution == 6 )
			{
				uint8_t sb = 0;
				buffered_send(stream_out, 1, &sb);
			}
		}
	}
}

void buffered_send(FILE *a1, size_t size, const uint8_t* data)
{
	if(size + ga4_size > 0x4000 || !(ga4_appends % 128))
	{
		flush_ga4(a1);
	}
	memcpy(gl_al_4 + ga4_size, data, size);
	ga4_size += size;
	++ga4_appends;
	++ga4_preceeder;
}

int flush_ga4(FILE *stream)
{
	int result = ga4_size;
	if(result)
	{
		fprintf(stream, "%zuw%c%c", ga4_size + 2, 0, ga4_preceeder);
		result = fwrite(gl_al_4, 1, ga4_size, stream);
	}
	ga4_size = 0;
	ga4_preceeder = 0;
	return result;
}
