#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

static const char *usage =
  "usage: tipack IN [FOLDER\\]VARNAME TYPE OUT\n"
  "\n"
  "Converts any file into a TI-compatible application variable.\n"
  "\n"
  "IN      input file\n"
  "OUT     output file (calculator family is deduced from extension)\n"
  "FOLDER  optional, default folder of the variable (main if not specified)\n"
  "VARNAME name of the variable\n"
  "TYPE    custom type of the variable\n"
  "\n"
  "example: tipack file.bin main\\file BIN file.v2y\n"
  "Copyright 2015 by nucular, MIT License\n";

const char *fpext(const char* filename)
{
  const char *dot = strchr(filename, '.');
  if (!dot || dot == filename) return "";
  return dot + 1;
}

char *strndup(const char*s, size_t n)
{
  char *result;
  size_t len = strlen(s);

  if (n < len)
    len = n;

  result = (char*)malloc(len + 1);
  if (!result)
    return 0;

  result[len] = '\0';
  return (char*)memcpy(result, s, len);
}

int main(int argc, char *argv[])
{
  const char *inpath, *folder, *varname, *type, *outpath;

  if (argc < 5 || argc > 6)
  {
    printf("%s", usage);
    return 1;
  }
  else if (argc == 5)
  {
    inpath = argv[1];
    folder = "main";
    varname = argv[2];
    type = argv[3];
    outpath = argv[4];
  }
  else
  {
    inpath = argv[1];
    folder = argv[2];
    varname = argv[3];
    type = argv[4];
    outpath = argv[5];
  }

  const char *outext = fpext(outpath);
  char *calcname = strndup(outext, 2);
  if (strcmp(calcname, "v2") == 0)
  {
    //
  }
  else
  {
    printf("Unrecognized calculator: %s", calcname);
    free(calcname);
    return 1;
  }
  free(calcname);

  FILE *in = fopen(inpath, "r");
  if (!in)
  {
    printf("Could not open input file: %s\n", strerror(errno));
    return errno;
  }
  FILE *out = fopen(outpath, "w");
  if (!out)
  {
    fclose(in);
    printf("Could not open output file: %s\n", strerror(errno));
    return errno;
  }

  fclose(in);
  fclose(out);

  return 0;
}
