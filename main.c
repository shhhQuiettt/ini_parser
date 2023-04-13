#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Lookup {
  char *filename;
  char *section;
  char *key;
};

struct Record {
  char *key;
  char *value;
};

struct Section {
  char *name;
  struct Section *next;
  struct Record *records;
  int number_of_records;
};

void printIniData(struct Section *head) {
  struct Section *current_section = head;

  while (current_section != NULL) {
    printf("Section: %s\n", current_section->name);
    printf("Number of records: %d\n", current_section->number_of_records);

    {
      for (int i = 0; i < current_section->number_of_records; ++i) {
        printf("Key: %s, Value: %s\n", current_section->records[i].key,
               current_section->records[i].value);
      }
    }
    current_section = current_section->next;
    printf("\n");
  }
}

struct Lookup *getDataFromArgs(int size, char *argv[]) {
  struct Lookup *data;
  data = malloc(sizeof(struct Lookup));
  if (size > 2) {
    data->filename = malloc(sizeof(char) * strlen(argv[1]));
    strcpy(data->filename, argv[1]);

    char *argument = argv[2];
    int cmp = strcmp(argument, "expression");
    if (cmp == 0) {
      exit(1);
    } else {

      char *token;
      token = strtok(argument, ".");
      data->section = malloc(sizeof(char) * strlen(token));
      strcpy(data->section, token);

      token = strtok(NULL, ".");
      data->key = malloc(sizeof(char) * strlen(token));
      strcpy(data->key, token);
      return data;
    }
  } else {
    printf("No arguments\n");
    perror("No arguments\n");
    exit(1);
  }
}

FILE *open_file(char *filename) {
  FILE *fp;
  fp = fopen(filename, "r");
  if (fp == NULL) {
    printf("Error while opening the file.\n");
    exit(1);
  }
  return fp;
}

short is_proper_char(char c) { return isalnum(c) || c == '-'; }

short is_valid_identifier(char *identifier) {
  for (int i = 0; i < strlen(identifier); i++) {
    if (!is_proper_char(identifier[i])) {
      printf("At %d ---%s----\n", i, &identifier[i]);
      fflush(stdout);
      return 0;
    }
  }
  return 1;
}

short is_section(char *line) {
  /* printf("%d", strlen(line)); */
  ushort end_offset = line[strlen(line) - 1] == '\n' ? 2 : 1;
  return (line[0] == '[' && line[strlen(line) - end_offset] == ']') ? 1 : 0;
}

// moves section name extracted from line to section_container
//          line:        section_container:
// Example: [abc]\n  ->  abc
char *extract_section(char *line_buff, char *section_container) {
  char *new_section_container =
      realloc(section_container, sizeof(char) * (strlen(line_buff) - 2));
  strncpy(new_section_container, line_buff + 1, strlen(line_buff) - 3);
  new_section_container[strlen(line_buff) - 3] = '\0';
  return new_section_container;
}

short is_blank(char *line) {
  if (strcmp(line, "\n") == 0 || strcmp(line, "\r\n") == 0) {
    return 1;
  }
  return 0;
}

struct Record *parse_key_value(char *line_buff) {
  struct Record *record = malloc(sizeof(struct Record));
  char *current_key = strtok(line_buff, "=");
  // Remove trailing whitespace
  current_key[strlen(current_key) - 1] = '\0';

  if (!is_valid_identifier(current_key)) {
    printf("Invalid key name: %s", current_key);
    exit(1);
  }

  record->key = malloc(sizeof(char) * strlen(current_key) + 1);
  strcpy(record->key, current_key);

  char *current_value = strtok(NULL, "=");
  if (current_value == NULL) {
    printf("Invalid value for key: %s", current_key);
    exit(1);
  }

  // Removing line break
  current_value[strlen(current_value) - 1] = '\0';

  // Removing trailing whitespace
  current_value += 1;

  record->value = malloc(sizeof(char) * strlen(current_value) + 1);
  strcpy(record->value, current_value);

  return record;
}

int main(int argc, char **argv) {

  struct Lookup *looking_for = getDataFromArgs(argc, argv);

  struct Section *ini_data = malloc(sizeof(struct Section));

  char *current_section_name = NULL;

  FILE *fp;

  fp = open_file(looking_for->filename);

  // looking for section
  char *line_buff;
  size_t line_buff_size = 0;

  struct Section *current_section = ini_data;
  while (getline(&line_buff, &line_buff_size, fp) != -1) {
    if (is_blank(line_buff)) {
      continue;
    } else if (is_section(line_buff)) {
      current_section_name = extract_section(line_buff, current_section_name);

      if (!is_valid_identifier(current_section_name)) {
        printf("Invalid section name: %s", current_section_name);
        return 1;
      }

      current_section->next = malloc(sizeof(struct Section));
      current_section = current_section->next;
      current_section->name =
          malloc(sizeof(char) * strlen(current_section_name) + 1);
      strcpy(current_section->name, current_section_name);

      current_section->number_of_records = 0;
      current_section->next = NULL;
      current_section->records = NULL;

      /* printf("Section: %s\n", current_section->name); */
    } else {
      current_section->number_of_records++;
      struct Record *current_record = parse_key_value(line_buff);
      current_section->records =
          realloc(current_section->records,
                  sizeof(struct Record) * (current_section->number_of_records));

      current_section->records[current_section->number_of_records - 1] =
          *current_record;
    }
  }
  ini_data = ini_data->next;
  printIniData(ini_data);
}
