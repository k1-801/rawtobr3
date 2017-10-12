#include "rawtobr3.h"

void convert4(size_t a, size_t b, int c, size_t* id)
{
	gl_al_3[0] |= (a > b ? b : a) << c;
	*id = 1;
	int i;
	for(i = a - b; (i -= 255) >= 0;)
	{
		gl_al_3[*id++] = 255;
	}
	gl_al_3[*id++] = (i + 255);
}

void convert_3(dim_t a2, dim_t a3, size_t* id)
{
	if(gl_al_3[0] == 0x80)
	{
		convert4(a2    ,  3, 5, id);
		convert4(a3 - 2, 31, 0, id);
	}
	else
	{
		convert4(a2    , 15, 3, id);
		convert4(a3 - 1,  7, 0, id);
	}
}

void convert_2()
{
	// Could be a structure or an array
	size_t i; // [sp+22h] [bp-26h]@2
	size_t j; // [sp+24h] [bp-24h]@14
	size_t v18; // [sp+2Eh] [bp-1Ah]@6
	size_t v19; // [sp+30h] [bp-18h]@6
	size_t v20 = 0; // [sp+32h] [bp-16h]@14

	sendbuf_size = 1;
	uint8_t* a2 = gl_al_2 + 1;
	*gl_al_2 = 0;

	if(row_width_0 < row_width)
	{
		for(i = row_width_0; i < row_width; ++i)
		{
			last_sent_row[i] = 0;
		}
	}

	if(converted_2->converted_overflow)
	{
		gl_al_3[0] = 0x80u;
		size_t ls;
		convert_3(0, converted_2->converted_overflow, &ls);
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
			gl_al_3[0] = 0;
			size_t ls, ns = row_width - i;
			convert_3(0, ns, &ls);
			memcpy(gl_al_3 + ls, sending_row + i, ns);
			ls += ns;
			memcpy(gl_al_3, a2, ls);
			++gl_al_2[0];
			sendbuf_size += ls;
			return;
		}

		// If the lines are completely equal, we're not doing anything
		// v18 is a number of equals at the beginning
		for(v18 = 0; sending_row[i + v18] == last_sent_row[i + v18];)
		{
			++v18;
			if(i + v18 >= row_width)
			{
				if(!i)
					sendbuf_size = 0;
				return;
			}
		}
		i += v18; // Skip equals

		// Find first two equal symbols after i
		// v20 is a number of differents
		for(v20 = 0; i + v20 < row_width && sending_row[i + v20] != last_sent_row[i + v20]; ++v20);

		// three or more symbols in a row are different in the previous and this lines
		if(v20 > 2)
		{
			j = i;
			size_t l = i + v20; // Limit? Last symbol that's different than the previous one
			while(j < l)
			{
				size_t ls = 0;
				// if there are at least three equal symbols in a row, like "ooo"
				if(j + 2 < l && sending_row[j] == sending_row[j + 1] && sending_row[j + 1] != sending_row[j + 2])
				{
					debug("Miracle happened! %zu, %zu\n", j, l);
					debug("i was %zu and sending_row was 0x%016zx\n", i, sending_row);
					// Find two different symbols
					// v19 is a number of equals
					for(v19 = 2; j + v19 < l && sending_row[j + v19 - 1] == sending_row[j + v19]; ++v19);
					j += v19;
					// Conversion with code 0x80
					gl_al_3[0] = 0x80;
					convert_3(v18, v19, &ls);
					gl_al_3[ls++] = sending_row[v19 - 1];
				}
				else
				{
					if(j + 2 >= l)
					{
						v19 = l - j;
					}
					else
					{
						for(v19 = 2; j + v19 < l; ++v19)
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
					gl_al_3[0] = 0;
					convert_3(v18, v19, &ls);
					//memcpy(sending_row + j, gl_al_3 + ls, v19);
					memcpy(gl_al_3 + ls, sending_row + j, v19);
					j += v19;
					ls += v19;
				}
				memcpy(a2, gl_al_3, ls);
				++gl_al_2[0];
				sendbuf_size += ls;
				a2 += ls;
				v18 = 0;

				if(gl_al_2[0] == 0xFD)
				{
					v20 = j - i;
					break;
				}
			}
		}
		// First two bytes are the same
		else
		{
			gl_al_3[0] = (v20 - 1);
			size_t ls;
			convert4(v18, 15, 3, &ls);
			memcpy(gl_al_3 + ls, sending_row + i, v20);
			ls += v20;
			memcpy(a2, gl_al_3, ls);
			++gl_al_2[0];
			sendbuf_size += ls;
			a2 += ls;
		}
		// i += v20
	}
}
