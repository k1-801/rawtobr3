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

//----- (080496C4) --------------------------------------------------------
int send_job_headers(FILE *stream)
{
	char v2[65]; // [sp+4Fh] [bp-69h]@47
	char s[16]; // [sp+94h] [bp-24h]@41
	paper_size *ps; // [sp+A4h] [bp-14h]@42
	int a4; // [sp+A8h] [bp-10h]@42
	int n; // [sp+B0h] [bp-8h]@44

	fwrite_size_preceded(stream, "\x0E\x1B%-12345X@PJL\n");
	if ( !((flags[0] - '0') & 1) )
	fwrite_size_preceded(stream, "\x1F@PJL JOB NAME=\"Brother HL-XXX\"\n");
	switch ( converted_2->resolution )
	{
		case 3:
			fwrite_size_preceded(stream, "8@PJL SET RESOLUTION = 1200\n@PJL SET PAPERFEEDSPEED=HALF\n");
			break;
		case 4:
			fwrite_size_preceded(stream, "\x1A@PJL SET RESOLUTION = 600\n");
			fwrite_size_preceded(stream, "\x1C@PJL SET RAS1200MODE = TRUE\n");
			break;
		case 5:
			fwrite_size_preceded(stream, "8@PJL SET RESOLUTION = 1200\n@PJL SET PAPERFEEDSPEED=FULL\n");
			break;
		case 6:
			fwrite_size_preceded(stream, "\x1A@PJL SET RESOLUTION = 600\n");
			break;
		case 2:
			if(!(((flags[1] - '0') >> 1) & 1))
				fwrite_size_preceded(stream, "\x1B@PJL SET RAS1200MODE = OFF\n");
			if((flags[1] - '0') & 1)
			{
				fwrite_size_preceded(stream, "\x1A@PJL SET RESOLUTION = 600\n");
				fwrite_size_preceded(stream, "\x16@PJL SET HQMMODE = ON\n");
			}
			else
			{
				fwrite_size_preceded(stream, "\x1A@PJL SET RESOLUTION = 600\n");
			}
			break;
		case 1:
			if(!(((flags[1] - '0') >> 1) & 1))
				fwrite_size_preceded(stream, "\x1B@PJL SET RAS1200MODE = OFF\n");
			fwrite_size_preceded(stream, "\x1A@PJL SET RESOLUTION = 300\n");
			break;
		default:
			if(!(((flags[1] - '0') >> 1) & 1))
				fwrite_size_preceded(stream, "\x1B@PJL SET RAS1200MODE = OFF\n");
			fwrite_size_preceded(stream, "\x1A@PJL SET RESOLUTION = 600\n");
			break;
	}
	if ( converted_2->toner_save )
		fwrite_size_preceded(stream, "\x18@PJL SET ECONOMODE = ON\n");
	else
		fwrite_size_preceded(stream, "\x19@PJL SET ECONOMODE = OFF\n");
	if ( converted_2->sourcetray != 6 )
	{
		switch ( converted_2->sourcetray )
		{
			case 1:
				fwrite_size_preceded(stream, "\x1C@PJL SET SOURCETRAY = TRAY1\n");
				break;
			case 2:
				fwrite_size_preceded(stream, "\x1C@PJL SET SOURCETRAY = TRAY2\n");
				break;
			case 3:
				fwrite_size_preceded(stream, "\x1C@PJL SET SOURCETRAY = TRAY3\n");
				break;
			case 4:
				fwrite_size_preceded(stream, "\x1C@PJL SET SOURCETRAY = TRAY4\n");
				break;
			case 5:
				fwrite_size_preceded(stream, "\x1C@PJL SET SOURCETRAY = TRAY5\n");
				break;
			case 7:
				fwrite_size_preceded(stream, "\x1D@PJL SET SOURCETRAY = MPTRAY\n");
				break;
			default:
				fwrite_size_preceded(stream, "\x1B@PJL SET SOURCETRAY = AUTO\n");
				break;
		}
	}
	switch(converted_2->mediatype)
	{
		case 1:
			fwrite_size_preceded(stream, "\x1A@PJL SET MEDIATYPE = THIN\n");
			break;
		case 2:
			fwrite_size_preceded(stream, "\x1B@PJL SET MEDIATYPE = THICK\n");
			break;
		case 3:
		case 4:
			fwrite_size_preceded(stream, "\x1C@PJL SET MEDIATYPE = THICK2\n");
			break;
		case 5:
			fwrite_size_preceded(stream, "\"@PJL SET MEDIATYPE = TRANSPARENCY\n");
			break;
		case 6:
			fwrite_size_preceded(stream, "\x1F@PJL SET MEDIATYPE = ENVELOPES\n");
			break;
		case 7:
			fwrite_size_preceded(stream, "\x1E@PJL SET MEDIATYPE = ENVTHICK\n");
			break;
		case 9:
			fwrite_size_preceded(stream, "\x1E@PJL SET MEDIATYPE = RECYCLED\n");
			break;
		default:
			fwrite_size_preceded(stream, "\x1D@PJL SET MEDIATYPE = REGULAR\n");
	}
	if ( converted_2->sleep_time )
	{
		fwrite_size_preceded(stream, "\x17@PJL DEFAULT AUTOSLEEP=");
		fwrite_size_preceded(stream, "\x03ON\n");
		fwrite_size_preceded(stream, "\x1A@PJL DEFAULT TIMEOUTSLEEP=");
		sprintf(s, "%d\n", converted_2->sleep_time);
		fwrite_wrapper(stream, s, strlen(s));
		fwrite_size_preceded(stream, "\x13@PJL SET AUTOSLEEP=");
		fwrite_size_preceded(stream, "\x03ON\n");
		fwrite_size_preceded(stream, "\x16@PJL SET TIMEOUTSLEEP=");
		fwrite_wrapper(stream, s, strlen(s));
	}

	const char* src;
	find_paper_size_0(&ps, converted_2->ps_n2, converted_2->ps_n1, &a4, &src);
	fwrite_size_preceded(stream, " @PJL SET ORIENTATION = PORTRAIT\n");
	if(!src || !*src)
	{
		fwrite_size_preceded(stream, "\x11@PJL SET PAPER = ");
		n = strlen(ps->PaperSizeName);
		fwrite_wrapper(stream, ps, n);
		fwrite_size_preceded(stream, "\x01\n");
	}
	fwrite_size_preceded(stream, "\x1C@PJL SET PAGEPROTECT = AUTO\n");
	fwrite_size_preceded(stream, "\x1A@PJL ENTER LANGUAGE = PCL\n");
	fwrite_size_preceded(stream, "\x02\eE");    // Reset the perimeter
	if(src && *src)
	{
		v2[0] = strlen(src);
		strncpy(v2 + 1, src, 0x3Fu);
		fwrite_size_preceded(stream, v2);
	}
	switch ( converted_2->resolution )
	{
		case 4:
			fwrite_size_preceded(stream, "\b\x1B&u1200D");
			fwrite_size_preceded(stream, "\a\x1B*t600R");
			break;
		case 5:
		case 3:
			fwrite_size_preceded(stream, "\b\x1B&u1200D");
			fwrite_size_preceded(stream, "\b\x1B*t1200R");
			break;
		case 2:
		case 6:
			fwrite_size_preceded(stream, "\a\x1B&u600D");
			fwrite_size_preceded(stream, "\a\x1B*t600R");
			break;
		case 1:
			fwrite_size_preceded(stream, "\a\x1B&u300D");
			fwrite_size_preceded(stream, "\a\x1B*t300R");
			break;
		case 0:
			fwrite_size_preceded(stream, "\a\x1B&u150D");
			fwrite_size_preceded(stream, "\a\x1B*t150R");
			break;
		default:
			break;
	}
	if(converted_2->sourcetray == 6)
		fwrite_size_preceded(stream, "\x05\x1B&l2H");
	if(converted_2->copies > 1)
	{
		fwrite_size_preceded(stream, "\x03\x1B&l");
		sprintf(s, "%dX", converted_2->copies);
		fwrite_wrapper(stream, s, strlen(s));
	}
	if(converted_2->duplex)
	{
		if ( converted_2->duplex_type )
			return fwrite_size_preceded(stream, "\x05\x1B&l1S");
		else
			return fwrite_size_preceded(stream, "\x05\x1B&l2S");
	}
	return 0;
}

void send_converted_data(FILE *stream_out, uint8_t *data, int ps_n1, int ps_n2)
{
	uint8_t v6; // [sp+27h] [bp-21h]@30
	int send_size_2; // [sp+2Ch] [bp-1Ch]@30
	int send_size_1; // [sp+30h] [bp-18h]@47
	uint8_t *data_logical; // [sp+38h] [bp-10h]@7
	uint16_t v11; // [sp+3Eh] [bp-Ah]@35
	uint16_t v13; // [sp+42h] [bp-6h]@35
	int v14; // [sp+44h] [bp-4h]@14

	int16_t row_width_2 = (uint32_t)(ps_n1 + 7) >> 3;// Always reached
	row_width = row_width_2;

	switch ( converted_2->resolution )
	{
		case 1:
		  data_logical = &data[50 * row_width_2];
		  ps_n2 -= 100;
		  break;
		case 2:
		  data_logical = &data[100 * row_width_2];
		  ps_n2 -= 200;
		  break;
		case 6:
		  data_logical = &data[50 * row_width_2];
		  ps_n2 -= 100;
		  break;
		default:
		  data_logical = &data[200 * row_width_2];
		  ps_n2 -= 400;
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
		v11 = converted_2->field_1A;
		if(converted_2->resolution == 1)
		{
			v13 /= 2;
		}
		if(converted_2->resolution >= 3 && converted_2->resolution <= 5)
		{
			v13 *= 2;
		}
		if(v11 + row_width > v13 )
			row_width = v13 - v11;
		for(int i = 0; i <= ps_n2; ++i)
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
					sub_804A48C();
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

int fwrite_size_preceded(FILE *s, const char *a2)
{
  size_t n; // ST24_4@2
  int16_t v4; // [sp+20h] [bp-8h]@1

  v4 = 0;
  if ( *a2 > 0 )
  {
	n = *a2;
	if ( fwrite(a2 + 1, 1u, n, s) != n )
	  v4 = 1;
  }
  return v4;
}

int fwrite_wrapper(FILE *stream, void *data, size_t n)
{
	return (n && fwrite(data, 1u, n, stream) != n);
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
		fprintf(stream, "%zu%cw%c%c", ga4_size + 2, 1, 0, ga4_preceeder);
	}
	return result;
}
