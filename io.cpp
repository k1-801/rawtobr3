#include "rawtobr3.h"

size_t ga4_size;
size_t ga4_appends;
uint8_t ga4_preceeder;

char flags[32] = {0};

size_t row_width;
size_t row_width_0;
size_t sendbuf_size;
uint8_t* sending_row;
uint8_t* last_sent_row;

int send_job_headers(FILE *stream)
{
	paper_size *ps;

	fputs("\e%-12345X@PJL\n", stream);
	if ( !((flags[0] - '0') & 1) )
	{
		fputs("@PJL JOB NAME=\"Brother HL-XXX\"\n", stream);
	}
	switch(converted_2->resolution)
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
	if(converted_2->sourcetray != 6)
	{
		switch(converted_2->sourcetray)
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
	fetch_paper(&ps, converted_2->height, converted_2->width, &act_sizeb, &src);
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

// This function does almost all the driver's job
void send_converted_data(FILE *stream_out, uint8_t* data, size_t orig_width, size_t effective_height)
{
	size_t converted_width = (orig_width + 7) >> 3;// Always reached
	row_width = converted_width;

	static int iteration = 0;
	++iteration;

	// Page blank FIELDS
	int r = converted_2->resolution;
	int rskip = ((r == 2) ? 100 : ((r == 1 || r == 6) ? 50 : 200));
	int cskip = ((r == 1) ? 6 : ((r == 2 || r == 6) ? 12 : 25));

	effective_height -= 2 * rskip;
	row_width -= 2 * cskip;
	sending_row = data + rskip * converted_width + cskip;

	fputs("\e*b1030m", stream_out); // Unknown PCL command \e*b1030m
	ga4_size = 0;
	ga4_appends = 0;
	ga4_preceeder = 0;

	size_t converted_overflow = converted_2->converted_overflow;
	size_t max_width = 624 * ((converted_2->resolution == 3) ? 4 : ((converted_2->resolution == 1 || converted_2->resolution == 6) ? 1 : 2));

	if(row_width > max_width - converted_overflow)
	{
		row_width = max_width - converted_overflow;
	}

	uint8_t sb;
	// Here's the sending part
	for(size_t i = 0; i < effective_height; ++i)
	{
		if(converted_2->resolution == 4 && i & 1)
		{
			sb = 0;
			buffered_send(stream_out, 1, &sb);
		}
		else
		{
			if(is_all_zeroes(sending_row, row_width))
			{
				sb = 0xFF;
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
					sb = 0x00;
					buffered_send(stream_out, 1, &sb);
				}
			}
			last_sent_row = sending_row;
			row_width_0 = row_width;
		}
		sending_row += converted_width;
		if(converted_2->resolution == 6)
		{
			sb = 0;
			buffered_send(stream_out, 1, &sb);
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
