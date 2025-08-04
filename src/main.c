#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "common.h"
#include "file.h"
#include "parse.h"

void print_usage(char *argv[]) {
  printf("Usage: %s -n -f <database file\n", argv[0]);
  printf("\t -n - create new database file\n");
  printf("\t -f - (required) path to database file\n");
}

int main(int argc, char *argv[]) {

  int c;
  bool newfile = false;
  char *filepath = NULL;

  while ((c = getopt(argc, argv, "nf:")) != -1) {
    switch (c) {
    case 'n':
      newfile = true;
      break;
    case 'f':
      filepath = optarg;
      break;
    case '?':
      printf("Unknown option -%c\n", c);
      return -1;
      break;
    default:
      return -1;
    }
  }

  if (filepath == NULL) {
    printf("Filepath is a required argument.\n");
    print_usage(argv);

    return 0;
  }

  printf("Newfile: %d\n", newfile);
  printf("Filepath: %s\n", filepath);

  int dbfd = STATUS_ERROR;
  struct dbheader_t *dbheader = NULL;
  struct employee_t *employees = NULL;

  if (newfile) {
    dbfd = create_db_file(filepath);
    if (create_db_header(&dbheader) != STATUS_SUCCESS) {
      printf("Error creating database header, exitting...\n");
      free(dbheader);
      free(employees);
      close(dbfd);
      return STATUS_ERROR;
    }
  } else {
    dbfd = open_db_file(filepath);
    if (validate_db_header(dbfd, &dbheader)) {
      printf("Error parsing database header, exitting...\n");
      free(dbheader);
      free(employees);
      close(dbfd);
      return STATUS_ERROR;
    }
  }

  if (dbfd == STATUS_ERROR) {
    printf("Error opening file %s, exiting...\n", filepath);
    free(dbheader);
    free(employees);
    close(dbfd);
    return STATUS_ERROR;
  }

  if (output_file(dbfd, dbheader, employees) != STATUS_SUCCESS) {
    printf("Error opening file %s, exiting...\n", filepath);
    free(dbheader);
    free(employees);
    close(dbfd);
    return STATUS_ERROR;
  }

  free(dbheader);
  free(employees);
  close(dbfd);

  return STATUS_SUCCESS;
}
