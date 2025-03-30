#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 1048576
#define MAX_IDENTIFIERS 1000
#define MAX_STRINGS 1000
#define MAX_VARIABLE_NAME 64

typedef struct {
    char original[MAX_VARIABLE_NAME];
    char renamed[MAX_VARIABLE_NAME];
} VariableMapping;

static const char* RESERVED_WORDS[] = {
    "abstract", "arguments", "await", "boolean", "break", "byte", "case", "catch", "char",
    "class", "const", "continue", "debugger", "default", "delete", "do", "double", "else",
    "enum", "eval", "export", "extends", "false", "final", "finally", "float", "for",
    "function", "goto", "if", "implements", "import", "in", "instanceof", "int", "interface",
    "let", "long", "native", "new", "null", "package", "private", "protected", "public",
    "return", "short", "static", "super", "switch", "synchronized", "this", "throw", "throws",
    "transient", "true", "try", "typeof", "var", "void", "volatile", "while", "with", "yield"
};

int var_counter = 0;
VariableMapping var_mappings[MAX_IDENTIFIERS];
int var_mappings_count = 0;
char identifiers[MAX_IDENTIFIERS][MAX_VARIABLE_NAME];
int identifiers_count = 0;
char strings[MAX_STRINGS][MAX_BUFFER_SIZE];
int strings_count = 0;

char* generate_variable_name() {
    static char var_name[MAX_VARIABLE_NAME];
    sprintf(var_name, "_x%d", ++var_counter);
    return var_name;
}

bool is_reserved_word(const char* word) {
    for (int i = 0; i < sizeof(RESERVED_WORDS) / sizeof(RESERVED_WORDS[0]); i++) {
        if (strcmp(word, RESERVED_WORDS[i]) == 0) {
            return true;
        }
    }
    return false;
}

char* remove_comments(const char* code) {
    static char result[MAX_BUFFER_SIZE];
    char *out = result;
    const char *p = code;
    bool in_string = false;
    char string_char = 0;
    bool in_single_comment = false;
    bool in_multi_comment = false;

    while (*p) {
        if (in_single_comment) {
            if (*p == '\n') {
                in_single_comment = false;
                *out++ = *p;
            }
        } else if (in_multi_comment) {
            if (*p == '*' && *(p+1) == '/') {
                in_multi_comment = false;
                p++;
            }
        } else if (in_string) {
            if (*p == string_char && *(p-1) != '\\') {
                in_string = false;
            }
            *out++ = *p;
        } else if (*p == '"' || *p == '\'') {
            in_string = true;
            string_char = *p;
            *out++ = *p;
        } else if (*p == '/' && *(p+1) == '/') {
            in_single_comment = true;
            p++;
        } else if (*p == '/' && *(p+1) == '*') {
            in_multi_comment = true;
            p++;
        } else {
            *out++ = *p;
        }
        p++;
    }
    *out = '\0';
    return result;
}

void extract_identifiers(const char* code) {
    identifiers_count = 0;

    const char *p = code;
    while (*p) {
        if (strncmp(p, "var ", 4) == 0 || strncmp(p, "let ", 4) == 0 || strncmp(p, "const ", 6) == 0) {
            p += (p[0] == 'v' || p[0] == 'l') ? 4 : 6;
            while (isspace(*p)) p++;

            if (isalpha(*p) || *p == '_' || *p == '$') {
                char id[MAX_VARIABLE_NAME] = {0};
                int i = 0;

                while (isalnum(*p) || *p == '_' || *p == '$') {
                    id[i++] = *p++;
                    if (i >= MAX_VARIABLE_NAME - 1) break;
                }
                id[i] = '\0';

                if (!is_reserved_word(id) && strlen(id) > 2) {
                    strcpy(identifiers[identifiers_count++], id);
                    if (identifiers_count >= MAX_IDENTIFIERS) break;
                }
            }
        } else if (strncmp(p, "function ", 9) == 0) {
            p += 9;
            while (isspace(*p)) p++;

            if (isalpha(*p) || *p == '_' || *p == '$') {
                char id[MAX_VARIABLE_NAME] = {0};
                int i = 0;

                while (isalnum(*p) || *p == '_' || *p == '$') {
                    id[i++] = *p++;
                    if (i >= MAX_VARIABLE_NAME - 1) break;
                }
                id[i] = '\0';

                if (!is_reserved_word(id) && strlen(id) > 2) {
                    strcpy(identifiers[identifiers_count++], id);
                    if (identifiers_count >= MAX_IDENTIFIERS) break;
                }
            }
        }
        p++;
    }
}

void extract_strings(const char* code) {
    strings_count = 0;
    const char *p = code;
    bool in_string = false;
    char string_char = 0;
    char current_string[MAX_BUFFER_SIZE] = {0};
    int string_pos = 0;

    while (*p) {
        if (!in_string) {
            if (*p == '"' || *p == '\'') {
                in_string = true;
                string_char = *p;
                current_string[string_pos++] = *p;
            }
        } else {
            current_string[string_pos++] = *p;
            if (*p == string_char && *(p-1) != '\\') {
                in_string = false;
                current_string[string_pos] = '\0';

                if (string_pos > 3) {
                    strcpy(strings[strings_count++], current_string);
                    if (strings_count >= MAX_STRINGS) break;
                }

                string_pos = 0;
                memset(current_string, 0, MAX_BUFFER_SIZE);
            }
        }
        p++;
    }
}

char* rename_identifiers(char* code) {
    static char result[MAX_BUFFER_SIZE];
    strcpy(result, code);

    for (int i = 0; i < identifiers_count; i++) {
        char* new_name = generate_variable_name();
        strcpy(var_mappings[var_mappings_count].original, identifiers[i]);
        strcpy(var_mappings[var_mappings_count].renamed, new_name);
        var_mappings_count++;

        char* pos = result;
        while ((pos = strstr(pos, identifiers[i])) != NULL) {
            char prev = (pos > result) ? *(pos-1) : ' ';
            char next = *(pos + strlen(identifiers[i]));

            if ((!isalnum(prev) && prev != '_' && prev != '$') &&
                (!isalnum(next) && next != '_' && next != '$')) {
                strncpy(pos, new_name, strlen(new_name));
                pos += strlen(new_name);
            } else {
                pos += strlen(identifiers[i]);
            }
        }
    }

    return result;
}

char* encrypt_strings(char* code) {
    static char result[MAX_BUFFER_SIZE];
    strcpy(result, code);

    if (strings_count == 0) return result;

    int key = rand() % 50 + 1;
    char decoder_name[MAX_VARIABLE_NAME];
    sprintf(decoder_name, "_d%d", rand() % 10000);

    char decoder_function[512];
    sprintf(decoder_function, "function %s(s){var r='',i=0;for(;i<s.length;i++)r+=String.fromCharCode(s.charCodeAt(i)^%d);return r}",
            decoder_name, key);

    for (int i = 0; i < strings_count; i++) {
        if (strlen(strings[i]) <= 4) continue;

        char content[MAX_BUFFER_SIZE];
        strncpy(content, strings[i] + 1, strlen(strings[i]) - 2);
        content[strlen(strings[i]) - 2] = '\0';

        if (strlen(content) <= 2) continue;

        char encrypted[MAX_BUFFER_SIZE * 4] = {0};
        char quote = strings[i][0];

        strcat(encrypted, decoder_name);
        strcat(encrypted, "(");
        strcat(encrypted, &quote);

        for (int j = 0; j < strlen(content); j++) {
            char hex[10];
            sprintf(hex, "\\x%02x", content[j] ^ key);
            strcat(encrypted, hex);
        }

        strcat(encrypted, &quote);
        strcat(encrypted, ")");

        char* pos = result;
        while ((pos = strstr(pos, strings[i])) != NULL) {
            char buffer[MAX_BUFFER_SIZE];
            strcpy(buffer, pos + strlen(strings[i]));
            *pos = '\0';
            strcat(result, encrypted);
            strcat(result, buffer);
            pos = result + strlen(result) - strlen(buffer);
        }
    }

    char buffer[MAX_BUFFER_SIZE];
    strcpy(buffer, result);
    sprintf(result, "%s;%s", decoder_function, buffer);

    return result;
}

char* add_self_defense(char* code) {
    static char result[MAX_BUFFER_SIZE];

    char check_func[MAX_VARIABLE_NAME], original_code[MAX_VARIABLE_NAME], interval[MAX_VARIABLE_NAME];
    sprintf(check_func, "_c%d", rand() % 10000);
    sprintf(original_code, "_o%d", rand() % 10000);
    sprintf(interval, "_i%d", rand() % 10000);

    sprintf(result,
        "(function(){"
        "var %s='%p';"
        "function %s(){"
        "var c=document.currentScript.text.replace(/\\s+/g,'');"
        "var h='';"
        "for(var i=0;i<c.length;i++){"
        "h=((h<<5)-h)+c.charCodeAt(i);"
        "h=h&h;"
        "}"
        "h=h.toString(16);"
        "if(h!==%s){"
        "document.body.innerHTML='';"
        "window.location='about:blank';"
        "}"
        "}"
        "var %s=setInterval(%s,2000);"
        "})();%s",
        original_code, code, check_func, original_code, interval, check_func, code);

    return result;
}

char* add_debug_protection(char* code) {
    static char result[MAX_BUFFER_SIZE];

    sprintf(result,
        "(function(){"
        "setInterval(function(){"
        "debugger;"
        "},100);"
        "var s=new Date();"
        "debugger;"
        "var e=new Date();"
        "if(e-s>100){"
        "document.body.innerHTML='';"
        "window.location='about:blank';"
        "}"
        "})();%s", code);

    return result;
}

char* compact_code(char* code) {
    static char result[MAX_BUFFER_SIZE];
    char *out = result;
    const char *p = code;
    bool in_string = false;
    char string_char = 0;
    bool last_was_space = false;

    while (*p) {
        if (in_string) {
            *out++ = *p;
            if (*p == string_char && *(p-1) != '\\') {
                in_string = false;
            }
        } else if (*p == '"' || *p == '\'') {
            in_string = true;
            string_char = *p;
            *out++ = *p;
        } else if (isspace(*p)) {
            if (!last_was_space) {
                if (p > code && isalnum(*(p-1)) && isalnum(*(p+1))) {
                    *out++ = ' ';
                }
                last_was_space = true;
            }
        } else {
            if (*p == '}' && *(out-1) == ';') {
                out--;
            }
            *out++ = *p;
            last_was_space = false;
        }
        p++;
    }
    *out = '\0';
    return result;
}

char* obfuscate(const char* input) {
    srand(time(NULL));

    static char result[MAX_BUFFER_SIZE];
    char temp[MAX_BUFFER_SIZE];

    strcpy(temp, remove_comments(input));

    extract_identifiers(temp);
    extract_strings(temp);

    strcpy(temp, rename_identifiers(temp));
    strcpy(temp, encrypt_strings(temp));
    strcpy(temp, compact_code(temp));
    strcpy(temp, add_debug_protection(temp));
    strcpy(temp, add_self_defense(temp));

    strcpy(result, temp);
    return result;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s input.js [output.js]\n", argv[0]);
        return 1;
    }

    char *input_file = argv[1];
    char *output_file;

    if (argc >= 3) {
        output_file = argv[2];
    } else {
        char *dot = strrchr(input_file, '.');
        if (dot) {
            *dot = '\0';
            output_file = malloc(strlen(input_file) + 8);
            sprintf(output_file, "%s.obf.js", input_file);
            *dot = '.';
        } else {
            output_file = malloc(strlen(input_file) + 8);
            sprintf(output_file, "%s.obf.js", input_file);
        }
    }

    FILE *fp = fopen(input_file, "r");
    if (!fp) {
        printf("Error: Could not open input file %s\n", input_file);
        return 1;
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (size >= MAX_BUFFER_SIZE) {
        printf("Error: Input file too large (max %d bytes)\n", MAX_BUFFER_SIZE);
        fclose(fp);
        return 1;
    }

    char *buffer = malloc(size + 1);
    if (!buffer) {
        printf("Error: Memory allocation failed\n");
        fclose(fp);
        return 1;
    }

    fread(buffer, 1, size, fp);
    buffer[size] = '\0';
    fclose(fp);

    char *obfuscated = obfuscate(buffer);
    free(buffer);

    fp = fopen(output_file, "w");
    if (!fp) {
        printf("Error: Could not open output file %s\n", output_file);
        if (argc < 3) free(output_file);
        return 1;
    }

    fputs(obfuscated, fp);
    fclose(fp);

    printf("Obfuscation complete. Output saved to %s\n", output_file);

    if (argc < 3) free(output_file);
    return 0;
}