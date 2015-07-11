#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

#define SWAP_UINT16(x) (((x) >> 8) | ((x) << 8))
#define SWAP_UINT32(x) (((x) >> 24) | (((x) & 0x00FF0000) >> 8) | (((x) & 0x0000FF00) << 8) | ((x) << 24))

static const char *usage =
  "usage: tipack IN [FOLDER\\]VARNAME TYPE OUT\n"
  "\n"
  "Converts any file into a TI-compatible application variable.\n"
  "\n"
  "IN      input file\n"
  "FOLDER  optional, default folder of the variable (main if not specified)\n"
  "VARNAME name of the variable\n"
  "TYPE    custom type of the variable\n"
  "OUT     output file (calculator family is deduced from extension)\n"
  "\n"
  "example: tipack file.bin main\\file BIN file.v2y\n"
  "Copyright 2015 by nucular, MIT License\n";

typedef struct DATA {
  char *ptr;
  size_t size;
} DATA;

const char *fpext(const char *filename)
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


DATA tipack_v2(DATA indata, const char *folder, const char* varname, const char* type)
{
  DATA outdata = (DATA){
    calloc(84 + indata.size, 1),
    84 + indata.size
  };

  uint16_t checksum = 0;
  size_t i;
  for (i = 0; i < indata.size; i++)
  {
    checksum += indata.ptr[i];
  }

#if BYTE_ORDER == LITTLE_ENDIAN
  uint32_t bsize = SWAP_UINT32((unsigned int)indata.size);
  uint16_t bchecksum = SWAP_UINT16(checksum);
#else
  uint32_t bsize = indata.size;
  uint16_t bchecksum = checksum;
#endif
  
  memcpy(outdata.ptr, "**TI92**", 8);
  memcpy(outdata.ptr + 8, "\x01\x00", 2);
  strcpy(outdata.ptr + 10, folder);
  // memcpy(outdata.ptr + 18, (char[40]){0}, 40);
  memcpy(outdata.ptr + 58, "\x01\x00\x52\x00\x00\x00", 6);
  strcpy(outdata.ptr + 64, varname);
  memcpy(outdata.ptr + 72, "\xF8", 1);
  // memcpy(outdata.ptr + 73, (char[3]){0}, 3);
  memcpy(outdata.ptr + 76, &bsize, 4);
  memcpy(outdata.ptr + 80, "\xA5\xA5", 2);
  memcpy(outdata.ptr + 82, indata.ptr, indata.size);
  memcpy(outdata.ptr + 82 + indata.size, &bchecksum, 2);

  return outdata;
}


int main(int argc, char *argv[])
{
  const char *inpath, *folder, *varname, *type, *outpath;

  // Parse the arguments
  if (argc != 5)
  {
    printf("%s", usage);
    return 1;
  }
  else
  {
    inpath = argv[1];
    if (strstr(argv[2], "\\") != NULL)
    {
      folder = strtok(argv[2], "\\");
      varname = strtok(NULL, "\\");
    }
    else
    {
      folder = "main";
      varname = argv[2];
    }
    type = argv[3];
    outpath = argv[4];
  }

  printf("%s\\%s", folder, varname);

  // Open the files
  FILE *in = fopen(inpath, "r");
  if (!in)
  {
    printf("Could not open input file: %s\n", strerror(errno));
    return errno;
  }
  FILE *out = fopen(outpath, "w");
  if (!out)
  {
    printf("Could not open output file: %s\n", strerror(errno));
    fclose(in);
    return errno;
  }

  // Get and check the file size
  fseek(in, 0, SEEK_END);
  size_t insize = ftell(in);
  fseek(in, 0, SEEK_SET);
  if (insize >= 0xFFDC)
  {
    printf("Input file is too large: %iB of %iB", insize, 0xFFDC);
    fclose(in); fclose(out);
    return 1;
  }

  // Read the file
  DATA indata = {
    malloc(insize),
    insize
  };
  fread(indata.ptr, 1, indata.size, in);

  // Try to deduce the calculator family from the output extension
  // and process the file
  DATA outdata;
  const char *outext = fpext(outpath);
  char *calcname = strndup(outext, 2);
  if (strcmp(calcname, "v2") == 0)
  {
    outdata = tipack_v2(indata, folder, varname, type);
  }
  else
  {
    printf("Unrecognized calculator: %s", calcname);
    free(calcname); free(indata.ptr); fclose(in); fclose(out);
    return 1;
  }
  free(calcname);

  // Write to output
  fwrite(outdata.ptr, 1, outdata.size, out);

  free(indata.ptr);
  free(outdata.ptr);

  fclose(in);
  fclose(out);

  return 0;
}
