﻿#ifndef DECLS_H
#define DECLS_H

#include <stdint.h>
#include <stdio.h>

#include "rawtobr3.h"

// IO
char* get_default_req(char *dest, size_t limit);
int send_job_headers(FILE *);
void send_converted_data(FILE *stream, uint8_t* data, size_t orig_width, size_t orig_height);
void buffered_send      (FILE* stream, size_t size_ptr, const uint8_t* data);
int flush_ga4(FILE *stream);
// Unknown
void convert_2();
// Bitwise conversions
bool is_all_zeroes (const void* src, size_t size);
void bitwise_invert(const void* src, void* dst, size_t size);
// Config Parsers
int parse_lang         (const char* value, int* out);
int parse_resolution   (const char* value, int* out);
int parse_tray_1       (const char* value, int* out);
int parse_duplex       (const char* value, int* out);
int parse_duplex_type  (const char* value, int* out);
int parse_paper_size   (const char* value, int* out);
int parse_mediatype    (const char* value, int* out);
int parse_copies       (const char* value, int* out);
int parse_sleep_time   (const char* value, int* out);
int parse_toner_save   (const char* value, int* out);
int parse_config_string(const char* src,   char* param_value);
// Config
char *truncate_spaces  (char *s1);
char *fgets_line(char *s, int n, FILE *stream);
int read_config(FILE* stream, config_raw_t* conf);
int get_pi_line(FILE* stream, int paper_size, int *a3, int *a4);
int read_selection_item();
int check_config_validity(const char* s2, const char* s1); // idb
config_int_t* convert_config(config_int_t *obj, config_raw_t *config);
// Not so config
int fetch_paper(paper_size** a1, size_t height, size_t width, int* overflow, const char **extra_string); // idb
// Misc
int report_error(int a1, const char *a2);

#endif // DECLS_H
