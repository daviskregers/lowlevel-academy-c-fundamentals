#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "common.h"
#include "parse.h"

void list_employees(struct dbheader_t *dbhdr, struct employee_t *employees) {}

int add_employee(struct dbheader_t *dbhdr, struct employee_t *employees,
                 char *addstring) {}

int read_employees(int fd, struct dbheader_t *dbhdr,
                   struct employee_t **employeesOut) {}

int output_file(int fd, struct dbheader_t *header,
                struct employee_t *employees) {
  if (fd < 0) {
    printf("Got a bad file descriptor to validate\n");
    return STATUS_ERROR;
  }
  header->version = htons(header->version);
  header->count = htons(header->count);
  header->magic = htonl(header->magic);
  header->filesize = htonl(header->filesize);

  if (lseek(fd, 0, SEEK_SET) == -1) {
    perror("lseek");
    return STATUS_ERROR;
  }

  if (write(fd, header, sizeof(struct dbheader_t)) == -1) {
    perror("write");
    return STATUS_ERROR;
  }

  return STATUS_SUCCESS;
}

int validate_db_header(int fd, struct dbheader_t **headerOut) {
  if (fd < 0) {
    printf("Got a bad file descriptor to validate\n");
    return STATUS_ERROR;
  }

  struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
  if (header == NULL) {
    perror("calloc");
    return STATUS_ERROR;
  }

  if (read(fd, header, sizeof(struct dbheader_t)) == -1) {
    perror("read");
    free(header);
    return STATUS_ERROR;
  }

  header->version = ntohs(header->version);
  header->count = ntohs(header->count);
  header->magic = ntohl(header->magic);
  header->filesize = ntohl(header->filesize);

  if (header->magic != HEADER_MAGIC) {
    printf("unknown magic number\n");
    free(header);
    return STATUS_ERROR;
  }

  if (header->version != 1) {
    printf("wrong version\n");
    free(header);
    return STATUS_ERROR;
  }

  struct stat dbStat = {0};
  if (fstat(fd, &dbStat) == -1) {
    perror("fstat");
    free(header);
    return STATUS_ERROR;
  }

  if (header->filesize != dbStat.st_size) {
    printf("Header filesize doesn't match file size");
    free(header);
    return STATUS_ERROR;
  }

  *headerOut = header;

  return STATUS_SUCCESS;
}

int create_db_header(struct dbheader_t **headerOut) {
  struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
  if (header == NULL) {
    perror("calloc");
    return STATUS_ERROR;
  }

  header->version = 0x1;
  header->magic = HEADER_MAGIC;
  header->count = 0;
  header->filesize = sizeof(struct dbheader_t);

  *headerOut = header;

  return STATUS_SUCCESS;
}
