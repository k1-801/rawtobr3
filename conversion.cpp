#include "rawtobr3.h"

void convert4(dim_t a, dim_t b, int c, size_t* id)
{
	gl_al_3[0] |= (a > b ? b : a) << c;
	int i;
	for(i = a - b; (i -= 255) >= 0;)
	{
		gl_al_3[(*id)++] = 255;
	}
	if(a >= b)
		gl_al_3[(*id)++] = (i + 255);
}

void convert_3(dim_t a2, dim_t a3, size_t* id)
{
	*id = 1;
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
	size_t eqs1; // [sp+2Eh] [bp-1Ah]@6
	size_t eqs2; // [sp+30h] [bp-18h]@6
	size_t delta = 0; // [sp+32h] [bp-16h]@14

	sendbuf_size = 1;
	uint8_t* curr = gl_al_2 + 1;
	gl_al_2[0] = 0;

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
		memcpy(curr, gl_al_3, ls);
		curr += ls;
		++gl_al_2[0];
		sendbuf_size += ls;
	}

	for(i = 0; i < row_width; i += delta)
	{
		// If the first symbol if 0xFD, 0xFE, 0xFF
		if(gl_al_2[0] > 0xFCu)
		{
			gl_al_3[0] = 0;
			size_t ls, ns = row_width - i;
			convert_3(0, ns, &ls);
			memcpy(gl_al_3 + ls, sending_row + i, ns);
			ls += ns;
			memcpy(curr, gl_al_3, ls);
			++gl_al_2[0];
			sendbuf_size += ls;
			return;
		}

		// If the lines are completely equal, we're not doing anything
		// eqs1 is a number of equals at the beginning
		for(eqs1 = 0; sending_row[i + eqs1] == last_sent_row[i + eqs1];)
		{
			++eqs1;
			if(i + eqs1 >= row_width)
			{
				if(!i)
					sendbuf_size = 0;
				return;
			}
		}
		i += eqs1; // Skip equals

		// Find first two equal symbols after i
		// delta is a number of differents
		for(delta = 0; i + delta < row_width && sending_row[i + delta] != last_sent_row[i + delta]; ++delta);

		// three or more symbols in a row are different in the previous and this lines
		if(delta > 2)
		{
			j = i;
			size_t l = i + delta; // Limit? Last symbol that's different than the previous one
			while(j < l)
			{
				size_t ls = 0;
				// if there are at least three equal symbols in a row, like "ooo"
				if(j + 2 < l && sending_row[j] == sending_row[j + 1] && sending_row[j + 1] == sending_row[j + 2])
				{
					// Find two different symbols
					// eqs2 is a number of equals
					for(eqs2 = 2; j + eqs2 < l && sending_row[j + eqs2 - 1] == sending_row[j + eqs2]; ++eqs2);
					j += eqs2;
					// Conversion with code 0x80
					gl_al_3[0] = 0x80;
					convert_3(eqs1, eqs2, &ls);
					gl_al_3[ls++] = sending_row[j /*eqs2*/ - 1];
				}
				else
				{
					if(j + 2 >= l)
					{
						eqs2 = l - j;
					}
					else
					{
						for(eqs2 = 2; j + eqs2 < l; ++eqs2)
						{
							// 3 equal symbols in a row, like "ooo"
							if(sending_row[j + eqs2 - 2] == sending_row[j + eqs2 - 1] && sending_row[j + eqs2 - 1] == sending_row[j + eqs2])
							{
								eqs2 -= 2;
								break;
							}
						}
					}

					// Conversion 3 with 0
					gl_al_3[0] = 0;
					convert_3(eqs1, eqs2, &ls); // 110 8 -> 2; 7F 5F
					memcpy(gl_al_3 + ls, sending_row + j, eqs2);
					j += eqs2;
					ls += eqs2;
				}
				memcpy(curr, gl_al_3, ls);
				++gl_al_2[0];
				sendbuf_size += ls;
				curr += ls;
				eqs1 = 0;

				if(gl_al_2[0] == 0xFD)
				{
					delta = j - i;
					break;
				}
			}
		}
		// First two bytes are the same
		else
		{
			gl_al_3[0] = (delta - 1);
			size_t ls = 1;
			convert4(eqs1, 15, 3, &ls);
			memcpy(gl_al_3 + ls, sending_row + i, delta);
			ls += delta;
			memcpy(curr, gl_al_3, ls);
			++gl_al_2[0];
			sendbuf_size += ls;
			curr += ls;
		}
	}
}
