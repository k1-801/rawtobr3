#ifndef RAWTOBR3_H
#define RAWTOBR3_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

typedef size_t dim_t;

struct config_raw_t
{
	char somestring1[4];
	int country_2;
	int width;
	int height;
	int ps_n1_copy;
	int ps_n2_copy;
	int resolution;
	int tray_1;
	int duplex;
	int duplex_type;
	int paper_size;
	int mediatype;
	int copies;
	int sleep_time;
	int toner_save;
	FILE *stream_in;
	FILE *stream_out;
	FILE *conf_stream;
	int stream_in_set;
	int parsed_argc;
	char **argv;
	int flags_len;
	int parsed_flags;
	size_t conf_fname_len;
};

struct valid_values_entry
{
  char *parname;
  char *string2;
  char *argv[32];
  int argc;
  valid_values_entry *next;
};

struct paper_size
{
  char PaperSizeName[16];
  dim_t height;
  dim_t width;
  const char *extra_string;
};

struct config_int_t
{
	uint16_t sourcetray; // enum
	int16_t resolution; // enum

	bool toner_save;
	uint16_t mediatype; // enum
	bool duplex;
	uint16_t duplex_type;

	dim_t height;
	dim_t width;
	dim_t converted_overflow;

	uint16_t sleep_time;
	uint16_t copies;
};

extern config_int_t* converted_2;
extern size_t row_width;
extern size_t sendbuf_size;
extern uint8_t *sending_row;
extern uint8_t *last_sent_row;
extern size_t row_width_0;

extern uint8_t* gl_al_2;
extern uint8_t* gl_al_3;
extern uint8_t* gl_al_4;

extern const char* config_fields_3[9];
extern char flags[32];

#include "decls.h"

#endif
