#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include "common.h"
#include "file.h"

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
  if (newfile) {
    dbfd = create_db_file(filepath);
  } else {
    dbfd = open_db_file(filepath);
  }

  if (dbfd == STATUS_ERROR) {
    printf("Error opening file %s, exiting...\n", filepath);
    return -1;
  }

  close(dbfd);

  return 0;
}
