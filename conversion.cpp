#include "rawtobr3.h"

converted_1 *converted_2;

void sub_804A48C()
{
	uint16_t v3; // cx@31
	uint16_t v4; // cx@42
	uint16_t v5; // cx@48
	uint16_t v6; // dx@50
	uint16_t v7; // dx@51
	uint16_t v8; // dx@52
	uint16_t v9; // cx@54

	// Could be a structure or an array
	uint16_t a4; // [sp+20h] [bp-28h]@6
	uint16_t i; // [sp+22h] [bp-26h]@2
	uint16_t j; // [sp+24h] [bp-24h]@14
	uint16_t k; // [sp+26h] [bp-22h]@27
	int16_t v16; // [sp+2Ah] [bp-1Eh]@1
	int16_t v18; // [sp+2Eh] [bp-1Ah]@6
	uint16_t v19; // [sp+30h] [bp-18h]@6
	uint16_t v20; // [sp+32h] [bp-16h]@14
	uint8_t *a2; // [sp+34h] [bp-14h]@5
	uint8_t *ga_3_c2; // [sp+38h] [bp-10h]@1
	uint8_t *v23; // [sp+3Ch] [bp-Ch]@5
	uint16_t v24; // [sp+42h] [bp-6h]@7

	v16 = row_width;
	ga_3_c2 = global_alloc_3;
	for(i = row_width_0; i < row_width; ++i)
		last_sent_row[i] = 0;

	sendbuf_size = 1;
	a2 = ga_2 + 1;
	v23 = ga_2;
	*ga_2 = 0;

	if ( converted_2->field_1A )
	{
		v18 = 0;
		v19 = converted_2->field_1A;
		*ga_3_c2 = 0x80u;
		sub_804AD6D(ga_3_c2, v18, v19, &a4);
		ga_3_c2[a4++] = 0;
		memcpy(a2, ga_3_c2, a4);
		++*v23;
		sendbuf_size += a4;
		a2 += a4;
	}
	v24 = 1;
	for(i = 0; i < row_width; i += v20)
	{
		if(*v23 > 0xFCu)
		{
			v18 = 0;
			v20 = row_width - i;
			v19 = row_width - i;
			*ga_3_c2 = 0;
			sub_804AD6D(ga_3_c2, v18, v19, &a4);
			memcpy(ga_3_c2 + a4, sending_row + i, v19);
			i += v19;
			a4 += v19;
			memcpy(ga_3_c2, a2, a4);
			++*v23;
			sendbuf_size += a4;
			return;
		}
		for(v18 = 0; sending_row[i] == last_sent_row[i]; ++v18, ++i)
		{
			if ( i >= v16 )
			{
				if(v24)
					sendbuf_size = 0;
				return;
			}
		}

		v24 = 0;
		v20 = 0;
		for(j = i; j < v16 && sending_row[j] != last_sent_row[j]; ++j, ++v20);
		if ( v20 > 2 )
		{
			j = i;
			while ( j < i + v20 )
			{
				a4 = 0;
				if ( j + 2 >= i + v20 || sending_row[j] != sending_row[j + 1] || sending_row[j + 1] != sending_row[j + 2] )
				{
					if ( j + 2 >= i + v20 )
					{
						v19 = i + v20 - j;
					}
					else
					{
						v19 = 2;
						for ( k = j + 2; k < i + v20; ++k )
						{
							if ( sending_row[k - 2] == sending_row[k - 1] && sending_row[k - 1] == sending_row[k] )
							{
								v19 -= 2;
								break;
							}
							++v19;
						}
					}
					*ga_3_c2 = 0;
					sub_804AD6D(ga_3_c2, v18, v19, &a4);
					memcpy(sending_row, ga_3 + a4, v19);
					v4 += v19;
				}
				else
				{
					v19 = 2;
					for(k = j + 2; k < i + v20 && sending_row[k - 1] == sending_row[k]; ++k, ++v19);
					j = k;
					*ga_3_c2 = -128;
					sub_804AD6D(ga_3_c2, v18, v19, &a4);
					v3 = a4;
					ga_3_c2[a4] = sending_row[k - 1];
					a4 = v3 + 1;
				}
				memcpy(a2, ga_3_c2, a4);
				++*v23;
				sendbuf_size += a4;
				a2 += a4;
				v18 = 0;
				if(*v23 == 0xFD)
				{
					v20 = j - i;
					break;
				}
			}
		}
		else
		{
			a4 = 0;
			if ( v18 > 14 )
			{
				if ( v18 == 15 )
				{
					v6 = a4;
					ga_3_c2[a4] = (v20 - 1) | 0x78;
					++v6;
					ga_3_c2[v6] = 0;
					a4 = v6 + 1;
				}
				else
				{
					v18 -= 15;
					v7 = a4;
					ga_3_c2[a4] = (v20 - 1) | 0x78;
					for ( a4 = v7 + 1; ; a4 = v8 + 1 )
					{
						v18 -= 255;
						if ( v18 < 0 )
							break;
						v8 = a4;
						ga_3_c2[a4] = -1;
					}
					v18 += 255;
					v9 = a4;
					ga_3_c2[a4] = v18;
					a4 = v9 + 1;
				}
			}
			else
			{
				v5 = a4;
				ga_3_c2[a4] = 8 * v18 | (v20 - 1);
				a4 = v5 + 1;
			}
			j = i;
			memcpy(ga_3 + a4, sending_row, v20);
			v4 += v20;
			memcpy(a2, ga_3_c2, a4);
			++*v23;
			sendbuf_size += a4;
			a2 += a4;
		}
	}
}

void sub_804AD6D(uint8_t *data, uint16_t a2, uint16_t a3, uint16_t *changed)
{
	uint16_t v4; // dx@5
	uint16_t v5; // dx@7
	uint16_t v6; // cx@9
	uint16_t v8; // dx@13
	uint16_t v9; // dx@15
	uint16_t v11; // dx@21
	uint16_t v12; // dx@23
	uint16_t v13; // cx@25
	uint16_t v14; // dx@29
	uint16_t v15; // dx@31
	uint16_t v16; // cx@33
	int16_t v20; // [sp+0h] [bp-8h]@30

	*changed = 1;
	if ( *data == 0x80u )
	{
		if ( a2 > 2 )
		{
			if ( a2 == 3 )
			{
				*data |= 0x60u;
				v4 = *changed;
				data[*changed] = 0;
				*changed = v4 + 1;
			}
			else
			{
				int16_t v21 = a2 - 3;
				*data |= 0x60u;
				while ( 1 )
				{
					v21 -= 255;
					if ( v21 < 0 )
						break;
					v5 = *changed;
					data[*changed] = 0xFFu;
					*changed = v5 + 1;
				}
				v6 = *changed;
				data[*changed] = v21 - 1;
				*changed = v6 + 1;
			}
		}
		else
		{
			*data |= a2 << 5;
		}
		if(a3 - 2 > 30)
		{
			if(a3 - 2 == 31)
			{
				*data |= 0x1Fu;
				v8 = *changed;
				data[*changed] = 0;
				*changed = v8 + 1;
			}
			else
			{
				*data |= 0x1Fu;
				int16_t v18;
				for(v18 = a3 - 33; v18 >= 0; v18 -= 255)
				{
					v9 = *changed;
					data[*changed] = -1;
					*changed = v9 + 1;
				}
				data[*changed++] = v18 - 1;
			}
		}
		else
		{
			*data |= (a3 - 2);
		}
	}
	else
	{
	if ( a2 > 14 )
	{
	  if ( a2 == 15 )
	  {
		*data = 120;
		v11 = *changed;
		data[*changed] = 0;
		*changed = v11 + 1;
			}
			else
			{
				*data = 120;
				int16_t v22;
				for(v22 = a2 - 15; v22 >= 0; v22 -= 255)
				{
					v12 = *changed;
					data[*changed] = -1;
					*changed = v12 + 1;
				}
				v13 = *changed;
				data[*changed] = v22 - 1;
				*changed = v13 + 1;
			}
		}
		else
		{
			*data = 8 * a2;
		}
		if(a3 - 1 > 6)
		{
			if(a3 - 1 == 7)
			{
				*data |= 7u;
				v14 = *changed;
				data[*changed] = 0;
				*changed = v14 + 1;
			}
			else
			{
				v20 = a3 - 8;
				*data |= 7u;
				while ( 1 )
				{
					v20 -= 255;
					if ( v20 < 0 )
						break;
					v15 = *changed;
					data[*changed] = -1;
					*changed = v15 + 1;
				}
				v16 = *changed;
				data[*changed] = v20 - 1;
				*changed = v16 + 1;
			}
		}
		else
		{
			*data |= (a3 - 1);
		}
	}
}
