﻿#ifndef RAWTOBR3_H
#define RAWTOBR3_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

typedef size_t dim_t;

struct config_store
{
  char somestring1[4];
  int field_4;
  int field_8;
  int field_C;
  int field_10;
  int field_14;
  int country_2;
  int field_1C;
  int field_20;
  int field_24;
  int field_28;
  int field_2C;
  int field_30;
  int field_34;
  int field_38;
  int field_3C;
  int field_40;
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
  int field_9C;
  int field_A0;
  int field_0;
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

struct converted_1
{
//  uint16_t field_0;
//  uint16_t always_3;
//  uint16_t field_4;
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

struct struct_0
{
  int anonymous_0;
  char *anonymous_1;
  int anonymous_2;
  int anonymous_3;
  int anonymous_4;
  int anonymous_5;
  int anonymous_6;
  int anonymous_7;
};

struct struct_1
{
  uint32_t field_0[4];
  uint32_t field_4;
  uint32_t field_8;
  uint32_t field_C;
  uint32_t field_10;
  uint32_t field_14;
  uint32_t field_18;
  uint32_t field_1C;
};

struct struc_2
{
  char gap_0[4];
  int field_4;
};

extern converted_1* converted_2;
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

#define debug(x,y,z) {FILE* df = fopen("/tmp/bro_log.txt", "a"); if(df){fprintf(df, x, y, z); fclose(df);}}
#define debug2(x,y) {FILE* df = fopen("/tmp/bro_log.txt", "a"); if(df){fwrite(x, 1, y, df); fclose(df);}}

#endif
