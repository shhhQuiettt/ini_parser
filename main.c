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

struct Expression {
  char *operation;
  struct Record *record1;
  struct Record *record2;
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

// If record points to NULL, then section not found
// If record->key points to NULL, then key not found
struct Record *find(char *section, char *key, struct Section *head) {
  struct Section *current_section = head;
  struct Record *record;

  while (current_section != NULL) {
    if (strcmp(current_section->name, section) == 0) {
      record = malloc(sizeof(struct Record));
      record->key = NULL;
      for (int i = 0; i < current_section->number_of_records; ++i) {
        if (strcmp(current_section->records[i].key, key) == 0) {
          record->key = malloc(sizeof(char) * strlen(key) + 1);
          record->key = key;

          record->value = malloc(sizeof(char) * strlen(key) + 1);
          record->value = current_section->records[i].value;
          return record;
        }
      }
      return record;
    }
    current_section = current_section->next;
  }
  return NULL;
}

short isProperChar(char c) { return isalnum(c) || c == '-'; }

short isValidIdentifier(char *identifier) {
  for (unsigned int i = 0; i < strlen(identifier); i++) {
    if (!isProperChar(identifier[i])) {
      return 0;
    }
  }
  return 1;
}

struct Record *parse_key_value(char *line_buff) {
  struct Record *record = malloc(sizeof(struct Record));
  char *current_key = strtok(line_buff, "=");

  // Removing trailing whitespace
  current_key[strlen(current_key) - 1] = '\0';

  if (!isValidIdentifier(current_key)) {
    return NULL;
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

struct Lookup *getDataFromArgs(int size, char *argv[]) {
  struct Lookup *data;
  data = malloc(sizeof(struct Lookup));
  if (size > 2) {
    data->filename = malloc(sizeof(char) * strlen(argv[1]) + 1);
    strcpy(data->filename, argv[1]);

    char *argument = argv[2];
    int cmp = strcmp(argument, "expression");
    if (cmp == 0) {
      exit(1);
    } else {

      char *token;
      token = strtok(argument, ".");
      data->section = malloc(sizeof(char) * strlen(token) + 1);
      strcpy(data->section, token);

      token = strtok(NULL, ".");
      data->key = malloc(sizeof(char) * strlen(token) + 1);
      strcpy(data->key, token);
      return data;
    }
  } else {
    printf("No arguments\n");
    perror("No arguments\n");
    exit(1);
  }
}

FILE *openFile(char *filename) {
  FILE *fp;
  fp = fopen(filename, "r");
  if (fp == NULL) {
    printf("Error while opening the file.\n");
    exit(1);
  }
  return fp;
}

short isSection(char *line) {
  short end_offset = line[strlen(line) - 1] == '\n' ? 2 : 1;
  return (line[0] == '[' && line[strlen(line) - end_offset] == ']') ? 1 : 0;
}

// moves section name extracted from line to section_container
//          line:        section_container:
// Example: [abc]\n  ->  abc
char *extractSection(char *line_buff, char *section_container) {
  char *new_section_container =
      realloc(section_container, sizeof(char) * (strlen(line_buff) - 2));
  strncpy(new_section_container, line_buff + 1, strlen(line_buff) - 3);
  new_section_container[strlen(line_buff) - 3] = '\0';
  return new_section_container;
}

short isBlankLine(char *line) {
  if (strcmp(line, "\n") == 0 || strcmp(line, "\r\n") == 0) {
    return 1;
  }
  return 0;
}

struct Section *parseIniFile(FILE *fp) {
  struct Section *ini_data = malloc(sizeof(struct Section));
  struct Section *current_section = ini_data;
  char *current_section_name = NULL;

  char *line_buff = NULL;
  size_t line_buff_size = 0;
  unsigned int line_number = 1;

  while (getline(&line_buff, &line_buff_size, fp) != -1) {
    if (isBlankLine(line_buff)) {
      continue;
    } else if (isSection(line_buff)) {
      current_section_name = extractSection(line_buff, current_section_name);

      if (!isValidIdentifier(current_section_name)) {
        printf("Invalid section name identifier: %s\n", current_section_name);
        exit(1);
      }

      current_section->next = malloc(sizeof(struct Section));
      current_section = current_section->next;
      current_section->name =
          malloc(sizeof(char) * strlen(current_section_name) + 1);
      strcpy(current_section->name, current_section_name);

      current_section->number_of_records = 0;
      current_section->next = NULL;
      current_section->records = NULL;
    } else {
      current_section->number_of_records++;
      struct Record *current_record = parse_key_value(line_buff);
      if (current_record == NULL) {
        printf("Invalid key identifier\n");
        exit(1);
      }
      current_section->records =
          realloc(current_section->records,
                  sizeof(struct Record) * (current_section->number_of_records));

      current_section->records[current_section->number_of_records - 1] =
          *current_record;
    }
    line_number++;
  }
  return ini_data->next;
}

int main(int argc, char **argv) {
  if (argc < 3) {
    printf("No arguments\n");
    exit(1);
  }

  struct Lookup *looking_for = getDataFromArgs(argc, argv);

  FILE *fp = openFile(looking_for->filename);

  struct Section *ini_data = parseIniFile(fp);

  struct Record *data = find(looking_for->section, looking_for->key, ini_data);

  if (data == NULL) {
    printf("Failed to find section [%s]\n", looking_for->section);
    exit(1);
  }

  if (data->key == NULL) {
    printf("Failed to find key \"%s\"\n in section [%s]", looking_for->key,
           looking_for->section);
    exit(1);
  }

  printf("[%s].%s = %s\n", looking_for->section, looking_for->key, data->value);
  return 0;
}
