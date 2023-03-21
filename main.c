#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *open_file(char *filename) {
  FILE *fp;
  fp = fopen(filename, "r");
  if (fp == NULL) {
    printf("Error while opening the file.\n");
    exit(1);
  }
  return fp;
}

// To implement
short is_valid_section(char *section) { return 1; }

short is_valid_key(char *section) { return 1; }

short is_section(char *line) {
  return (line[0] == '[' && line[strlen(line) - 2] == ']') ? 1 : 0;
}

// moves section name extracted from line to section_container
//          line:        section_container:
// Example: [abc]\n  ->  abc
// Don't ask me how it works
void *extract_section(char *line_buff, char *section_container) {
  strncpy(section_container, line_buff + 1, strlen(line_buff) - 3);
  section_container[strlen(line_buff) - 3] = '\0';
}

int main() {

  char *filename = "example.ini";
  // looking for
  char section[31] = "slippery-balance";
  char key[31] = "buttery-profile";

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
        printf("section found: %s\n", current_section);

        // looking for key
        // TODO: what when key is not in section?
        while (fscanf(fp, "%s = %s\n", current_key, current_value) != EOF) {
          if (strcmp(current_key, key) == 0) {
            printf("key found: %s\n", current_key);
            printf("value: %s\n", current_value);

          }
        }
      }
    }
  }
  fclose(fp);
  return 0;
}
