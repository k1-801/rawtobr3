#ifndef DECLS_H
#define DECLS_H

#include <stdint.h>
#include <stdio.h>

#include "rawtobr3.h"

// IO
char* get_default_req(char *dest, size_t limit);
int send_job_headers(FILE *);
void send_converted_data(FILE *stream_out, uint8_t *data, int ps_n1, int ps_n2);
char buffered_send(FILE *a1, int *size_ptr, uint8_t* data);
int flush_ga4(FILE *stream);
int fwrite_size_preceded(FILE *s, const char *);
// Unknown
void convert_ga3_t();
void convert_3(uint8_t *data, uint16_t a2, uint16_t a3, uint16_t *changed);
// Bitwise conversions
bool is_all_zeroes (const void* src, size_t size);
void bitwise_invert(const void* src, void* dst, size_t size);
// Config Parsers
int parse_lang         (const char* value, int* out);
int parse_resolution   (const char* value, int* out);
int parse_tray_1       (const char* value, int* out);
int parse_duplex       (const char* value, int* out);
int parse_duplex_type  (const char* value, int* out);
int parse_paper_size   (const char* value, int* out); // idb
int parse_mediatype    (const char* value, int* out); // idb
int parse_copies       (const char* value, int* out); // idb
int parse_sleep_time   (const char* value, int* out); // idb
int parse_toner_save   (const char* value, int* out);
int parse_config_string(const char* src,   char* param_value);
// Config
char *truncate_spaces  (char *s1);
char *fgets_line(char *s, int n, FILE *stream);
int read_config(FILE *stream, config_store *conf); // idb
int read_two_numbers_after_paper_size(FILE *stream, int paper_size, int *a3, int *a4); // idb
int read_selection_item();
int check_config_validity(char *s2, char *s1); // idb
int fill_tray_params(char *a1, int *a2);
converted_1* convert_config(converted_1 *obj, config_store *config);
int fetch_paper(paper_size **a1, size_t sizea, size_t sizeb, int *act_sizeb, const char **extra_string); // idb
// Misc
int report_error(int a1, const char *a2);

#endif // DECLS_H
