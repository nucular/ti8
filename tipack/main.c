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


DATA tipack(DATA indata, const char *signature,
  const char *folder, const char* varname, const char* type)
{
  size_t indata_size = indata.size;
  size_t header_size = 88;
  size_t footer_size = 6;

  size_t outdata_size = header_size + indata_size + footer_size + 2;
  size_t variable_size = indata_size + footer_size;

  printf("Input size: %i (0x%X), Output size: %i (0x%X)\n",
    indata_size, indata_size,
    outdata_size, outdata_size);

  uint32_t outdata_size32 = (uint32_t)outdata_size;
  uint16_t variable_size16 = (uint16_t)variable_size;
#if BYTE_ORDER == LITTLE_ENDIAN
  //outdata_size32 = SWAP_UINT32(outdata_size32);
  variable_size16 = SWAP_UINT16(variable_size16);
#endif

  // File header
  DATA header = (DATA){calloc(header_size, 1), header_size};
  memcpy(header.ptr, signature, 8); // signature
  memcpy(header.ptr + 8, "\x01\x00", 2); // further signature
  strcpy(header.ptr + 10, folder); // parent folder (zero-terminated)
  // comment (unused)
  memcpy(header.ptr + 58, "\x01\x00\x52\x00\x00\x00", 6); // magic
  strcpy(header.ptr + 64, varname); // variable name (zero-terminated)
  memcpy(header.ptr + 72, "\x1C", 1); // type ID
  memcpy(header.ptr + 73, "\x00\x03\x00", 3); // magic
  memcpy(header.ptr + 76, &outdata_size32, 4); // file size (including header/footer)
  memcpy(header.ptr + 80, "\xA5\x5A\x00\x00\x00\x00", 6); // magic
  memcpy(header.ptr + 86, &variable_size16, 2); // data size

  // File footer
  DATA footer = (DATA){calloc(footer_size, 1), footer_size};
  memcpy(footer.ptr, "\x00", 1);
  strcpy(footer.ptr + 1, type);
  memcpy(footer.ptr + 5, "\xF8", 1);

  // Checksum is the lower 16 bits of the sum of all bytes in the variable
  unsigned int checksum = (variable_size16 & 0xFF) + ((variable_size16 >> 8) & 0xFF);
  int i = 0;
  for (i = 0; i < indata.size; i++)
  {
    checksum += *(uint8_t*)(indata.ptr + i);
  }
  for (i = 0; i < footer.size; i++)
  {
    checksum += *(uint8_t*)(footer.ptr + i);
  }

  printf("Checksum: %i (0x%X)\n", checksum, checksum);

  // Combine everything
  DATA outdata = (DATA){calloc(outdata_size, 1), outdata_size};
  memcpy(outdata.ptr, header.ptr, header.size);
  memcpy(outdata.ptr + header.size, indata.ptr, indata.size);
  memcpy(outdata.ptr + header.size + indata.size, footer.ptr, footer.size);
  memcpy(outdata.ptr + header.size + indata.size + footer.size, &checksum, 2);

  free(header.ptr);
  free(footer.ptr);

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

  printf("Variable path: %s\\%s\n", folder, varname);

  // Try to deduce the calculator family from the output extension
  const char *signature;
  const char *outext = fpext(outpath);
  char *calcname = strndup(outext, 2);
  if (strcasecmp(calcname, "9x") == 0 || strcasecmp(calcname, "v2") == 0)
  {
    signature = "**TI92P*";
  }
  else if (strcasecmp(calcname, "92") == 0)
  {
    signature = "**TI92**";
  }
  else if (strcasecmp(calcname, "89") == 0)
  {
    signature = "**TI89**";
  }
  else
  {
    printf("Unrecognized calculator: %s", calcname);
    free(calcname);
    return 1;
  }
  printf("Calculator: %s, Signature: %s\n", calcname, signature);
  free(calcname);

  // Open the files
  FILE *in = fopen(inpath, "rb");
  if (!in)
  {
    printf("Could not open input file: %s\n", strerror(errno));
    return errno;
  }
  FILE *out = fopen(outpath, "wb");
  if (!out)
  {
    printf("Could not open output file: %s\n", strerror(errno));
    fclose(in);
    return errno;
  }

  // Get and check the file size
  fseek(in, 0, SEEK_END);
  size_t insize = ftell(in);
  rewind(in);
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

  // Finally process it
  DATA outdata = tipack(indata, signature, folder, varname, type);

  // And write to output
  fwrite(outdata.ptr, 1, outdata.size, out);

  free(indata.ptr);
  free(outdata.ptr);

  fclose(in);
  fclose(out);

  return 0;
}
