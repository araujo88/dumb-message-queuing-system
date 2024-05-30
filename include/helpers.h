#pragma once
#include <stdio.h>
#include <stdlib.h>

int randint(int min_num, int max_num);
void clean_string(char *str, char c);
void debug(const char *file, int line, const char *format, ...);
int strncmp_s(const char *str1, const char *str2);
