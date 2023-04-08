#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

FILE *open_file(char *filename) {
  FILE *fp;
  fp = fopen(filename, "r");
  if (fp == NULL) {
    printf("Error while opening the file.\n");
    exit(1);
  }
  return fp;
}

short is_proper_cher(char c) { return isalnum(c) || c == '-'; }

short is_valid_identifier(char *identifier) {
  for (int i = 0; i < strlen(identifier); i++) {
    if (!is_proper_cher(identifier[i])) {
      return 0;
    }
  }
  return 1;
}

short is_section(char *line) {
  return (line[0] == '[' && line[strlen(line) - 2] == ']') ? 1 : 0;
}

// moves section name extracted from line to section_container
//          line:        section_container:
// Example: [abc]\n  ->  abc
// Don't ask me how it works
void extract_section(char *line_buff, char *section_container) {
  strncpy(section_container, line_buff + 1, strlen(line_buff) - 3);
  section_container[strlen(line_buff) - 3] = '\0';
}

int main() {

  char *filename = "example.ini";
  // looking for
  char section[31] = "slippery-balance";
  char key[31] = "buttery-profileee";

  char current_section[31];
  char current_key[31];
  char current_value[31];

  FILE *fp;

  fp = open_file(filename);

  // looking for section
  char line_buff[35];

  while (fgets(line_buff, sizeof(line_buff), fp) != NULL) {

    if (is_section(line_buff)) {
      extract_section(line_buff, current_section);

      if (strcmp(current_section, section) == 0) {
        while (fscanf(fp, "%s = %s\n", current_key, current_value) > 0) {
          if (strcmp(current_key, key) == 0) {
            printf("Value: %s\n", current_value);
            return 0;
          }
        }
        printf("Failed to find key %s in section [%s]", key, section);
        return 1;
      }
    }
  }
  printf("Failed to find section [%s]", section);
  fclose(fp);
  return 1;
}
