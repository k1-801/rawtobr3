#include "rawtobr3.h"

converted_1 *converted_2;

void convert_4();
void convert_5();

void convert_2()
{
	// Could be a structure or an array
	size_t i; // [sp+22h] [bp-26h]@2
	uint16_t j; // [sp+24h] [bp-24h]@14
	int16_t v18; // [sp+2Eh] [bp-1Ah]@6
	uint16_t v19; // [sp+30h] [bp-18h]@6
	uint16_t v20 = 0; // [sp+32h] [bp-16h]@14
	debug("We're doing Vindicators %zu!\n", row_width, 0);

	//for(i = row_width_0; i < row_width; ++i)
		//last_sent_row[i] = 0;

	sendbuf_size = 1;
	uint8_t* a2 = gl_al_2 + 1;
	*gl_al_2 = 0;

	if(converted_2->converted_sizeb)
	{
		v18 = 0;
		v19 = converted_2->converted_sizeb;
		gl_al_3[0] = 0x80u;
		uint16_t ls;
		convert_3(gl_al_3, 0, converted_2->converted_sizeb, &ls);
		gl_al_3[ls++] = 0; // append 0
		memcpy(a2, gl_al_3, ls);
		++*gl_al_2;
		sendbuf_size += ls;
		a2 += ls;
	}

	for(i = 0; i < row_width; i += v20)
	{
		// If the first symbol if 0xFD, 0xFE, 0xFF
		if(*gl_al_2 > 0xFCu)
		{
			debug("Yep, we're doing it again! %zu-th time!\n", i, 0);
			*gl_al_3 = 0;
			uint16_t ls;
			convert_3(gl_al_3, 0, row_width - i, &ls);
			memcpy(gl_al_3 + ls, sending_row + i, row_width - i);
			ls += row_width - i;
			memcpy(gl_al_3, a2, ls);
			++*gl_al_2;
			sendbuf_size += ls;
			return;
		}

		// If the lines are completely equal, we're not doing anything
		// v18 is a number of equals at the beginning
		for(v18 = 0; sending_row[i + v18] == last_sent_row[i + v18]; ++v18)
		{
			if(i + v18 >= row_width)
			{
				if(!i)
					sendbuf_size = 0;
				return;
			}
		}
		i += v18;

		// Find first two equal symbols after i
		// v20 is a number of differents
		for(v20 = 0; i + v20 < row_width && sending_row[i + v20] != last_sent_row[i + v20]; ++v20);

		if(v20 > 2)
		{
			j = i;
			while(j < i + v20)
			{
				uint16_t ls = 0;
				if(j + 2 >= i + v20 || sending_row[j] != sending_row[j + 1] || sending_row[j + 1] != sending_row[j + 2])
				{
					if(j + 2 >= i + v20)
					{
						v19 = i + v20 - j;
					}
					else
					{
						for(v19 = 2; j + v19 < i + v20; ++v19)
						{
							// 3 equal symbols in a row, like "ooo"
							if(sending_row[j + v19 - 2] == sending_row[j + v19 - 1] && sending_row[j + v19 - 1] == sending_row[j + v19])
							{
								v19 -= 2;
								break;
							}
						}
					}

					// Conversion 3 with 0
					*gl_al_3 = 0;
					convert_3(gl_al_3, v18, v19, &ls);
					memcpy(sending_row, gl_al_3 + ls, v19);
				}
				else
				{
					// Find two different symbols
					// v19 is a number of equals
					for(v19 = 2; j + v19 < i + v20 && sending_row[j + v19 - 1] == sending_row[j + v19]; ++v19);
					j += v19;

					// Conversion with 0x80
					*gl_al_3 = 0x80;
					convert_3(gl_al_3, v18, v19, &ls);
					gl_al_3[ls++] = sending_row[j - 1];
				}
				memcpy(a2, gl_al_3, ls);
				++*gl_al_2;
				sendbuf_size += ls;
				a2 += ls;
				v18 = 0;
				if(*gl_al_2 == 0xFD)
				{
					v20 = j - i;
					break;
				}
			}
		}
		else
		{
			uint16_t a4 = 0;
			if ( v18 > 14 )
			{
				if ( v18 == 15 )
				{
					gl_al_3[a4++] = (v20 - 1) | 0x78;
					gl_al_3[a4++] = 0;
				}
				else
				{
					v18 -= 15;
					gl_al_3[a4] = (v20 - 1) | 0x78;
					while((v18 -= 255) >= 0)
					{
						gl_al_3[++a4] = 0xFF;
					}
					v18 += 255;
					gl_al_3[a4++] = v18;
				}
			}
			else
			{
				gl_al_3[a4++] = 8 * v18 | (v20 - 1);
			}
			memcpy(gl_al_3 + a4, sending_row, v20);
			memcpy(a2, gl_al_3, a4);
			++*gl_al_2;
			sendbuf_size += a4;
			a2 += a4;
		}
	}
}

// data is always pointing to gl_al_3
void convert_3(uint8_t *data, uint16_t a2, uint16_t a3, uint16_t* changed)
{
	size_t out = 1;

	// Section 1
	if(data[0] == 0x80)
	{
		// a2 and 3???
		if(a2 == 3)
		{
			data[0] |= 0x60;
			data[1] = 0;
			out = 2;
		}
		if(a2 > 3)
		{
			int16_t v21 = a2 - 3;
			data[0] |= 0x60;
			while((v21 -= 255) >= 0)
			{
				data[out++] = 0xFFu;
			}
			data[out++] = v21 - 1;
		}
		if(a2 < 3)
		{
			data[0] |= a2 << 5;
		}

		// a3 and 33???
		if(a3 == 33)
		{
			data[0] |= 0x1Fu;
			data[out++] = 0;
		}
		if(a3 > 33)
		{
			*data |= 0x1Fu;
			int16_t v18 = a3 - 33;
			while((v18 -= 255) >= 0)
			{
				data[out++] = 0xFF;
			}
			data[out++] = v18 - 1;
		}
		if(a3 < 33)
		{
			*data |= (a3 - 2);
		}
	}
	else

	// Section 2
	{
		// a2 and 15?
		if(a2 == 15)
		{
			*data = 0x78;
			data[out ++] = 0;
		}
		if(a2 > 15)
		{
			*data = 120;
			int16_t v22 = a2 - 15;
			while(( v22 -= 255) >= 0)
			{
				data[out++] = -1;
			}
			data[out++] = v22 - 1;
		}
		if(a2 < 15)
		{
			data[0] = a2 << 3;
		}

		// a3 and 8???
		if(a3 == 8)
		{
			*data |= 7u;
			data[out++] = 0;
		}
		if(a3 > 8)
		{
			*data |= 7u;
			int16_t v20 = a3 - 8;
			while((v20 -= 255) >= 0)
			{
				data[out++] = 0xFF;
			}
			data[out++] = v20 - 1;
		}
		if(a3 < 8)
		{
			*data |= (a3 - 1);
		}
	}

	// Out
	*changed = out;
}
