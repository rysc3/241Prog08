/*
  @author Ryan Scherbarth
  cs241L
  5/2/23
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
  if (argc != 4)
  {
    fprintf(stderr, "Usage: %s record_size page_size filename\n", argv[0]);
    return 1;
  }

  int record_size = atoi(argv[1]);
  int page_size = atoi(argv[2]);
  char *filename = argv[3];

  FILE *file = fopen(filename, "r+b");
  if (!file)
  {
    fprintf(stderr, "Error opening file '%s'\n", filename);
    return 1;
  }

  // Determine file size
  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  fseek(file, 0, SEEK_SET);

  // Allocate memory for one page of records
  int records_per_page = page_size / record_size;
  char *page = malloc(page_size);
  if (!page)
  {
    fprintf(stderr, "Error allocating memory for page\n");
    fclose(file);
    return 1;
  }

  // Read and write pages in reverse order
  for (long i = 0; i < file_size; i += page_size)
  {
    // Read page from file
    fseek(file, i, SEEK_SET);
    int bytes_to_read = page_size;
    if (i + bytes_to_read > file_size)
    {
      bytes_to_read = file_size - i;
    }
    int records_to_read = bytes_to_read / record_size;
    for (int j = 0; j < records_to_read; j++)
    {
      fread(page + j * record_size, record_size, 1, file);
    }

    // Reverse page in memory
    for (int j = 0; j < records_to_read / 2; j++)
    {
      char *left = page + j * record_size;
      char *right = page + (records_to_read - j - 1) * record_size;
      for (int k = 0; k < record_size; k++)
      {
        char temp = left[k];
        left[k] = right[k];
        right[k] = temp;
      }
    }

    // Write page to file
    fseek(file, i, SEEK_SET);
    for (int j = 0; j < records_to_read; j++)
    {
      fwrite(page + j * record_size, record_size, 1, file);
    }
  }

  free(page);
  fclose(file);
  return 0;
}
