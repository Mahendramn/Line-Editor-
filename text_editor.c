#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdint.h>

/*
 * ============================================================
 *                 LINE EDITOR IN C
 * ============================================================
 *
 * Data structure:
 *     Dynamic array of dynamically allocated strings.
 *
 * Each element of lines[] points to one dynamically allocated
 * string containing one line of the document.
 *
 * Example:
 *
 *     lines
 *       |
 *       +----> "Hello"
 *       |
 *       +----> "This is C"
 *       |
 *       +----> "Line editor"
 *
 * count    = number of lines currently stored
 * capacity = number of line pointers currently allocated
 *
 * Commands:
 *
 *     append
 *     insert <line_number>
 *     print
 *     delete <line_number>
 *     replace <line_number>
 *     findreplace
 *     help
 *     quit
 *
 * ============================================================
 */

#define INITIAL_CAPACITY 4
#define COMMAND_SIZE 256


/* ============================================================
 * DATA STRUCTURE
 * ============================================================
 */

typedef struct
{
    char **lines;
    size_t count;
    size_t capacity;
} Document;


/* ============================================================
 * FUNCTION DECLARATIONS
 * ============================================================
 */

static void init_document(Document *doc);
static void free_document(Document *doc);

static int ensure_capacity(Document *doc);
static char *duplicate_string(const char *source);

static char *read_line(void);
static char *trim_whitespace(char *str);

static int parse_line_number(const char *text, size_t *line_number);

static void print_document(const Document *doc);

static int append_line(Document *doc, const char *text);
static int insert_line(Document *doc, size_t line_number, const char *text);
static int delete_line(Document *doc, size_t line_number);
static int replace_line(Document *doc, size_t line_number, const char *text);

static char *replace_all_occurrences(
    const char *source,
    const char *find,
    const char *replacement
);

static int find_and_replace(Document *doc);

static void print_help(void);

static void handle_command(Document *doc, char *input);


/* ============================================================
 * DOCUMENT INITIALIZATION
 * ============================================================
 */

static void init_document(Document *doc)
{
    if (doc == NULL)
    {
        return;
    }

    doc->lines = NULL;
    doc->count = 0;
    doc->capacity = 0;
}


/* ============================================================
 * FREE ENTIRE DOCUMENT
 * ============================================================
 */

static void free_document(Document *doc)
{
    size_t i;

    if (doc == NULL)
    {
        return;
    }

    if (doc->lines != NULL)
    {
        for (i = 0; i < doc->count; i++)
        {
            free(doc->lines[i]);
            doc->lines[i] = NULL;
        }

        free(doc->lines);
        doc->lines = NULL;
    }

    doc->count = 0;
    doc->capacity = 0;
}


/* ============================================================
 * DUPLICATE STRING
 *
 * strdup() is not part of standard C, so we implement our own.
 * strlen + 1 is required because of the '\0' terminator.
 * ============================================================
 */

static char *duplicate_string(const char *source)
{
    size_t length;
    char *copy;

    if (source == NULL)
    {
        return NULL;
    }

    length = strlen(source);

    /*
     * Check for size overflow before length + 1.
     */
    if (length == SIZE_MAX)
    {
        return NULL;
    }

    copy = malloc(length + 1);

    if (copy == NULL)
    {
        return NULL;
    }

    memcpy(copy, source, length + 1);

    return copy;
}


/* ============================================================
 * ENSURE ARRAY HAS SPACE
 *
 * If count == capacity, increase capacity using realloc().
 *
 * Important:
 * We use a temporary pointer for realloc().
 * This prevents losing the original allocation if realloc fails.
 * ============================================================
 */

static int ensure_capacity(Document *doc)
{
    size_t new_capacity;
    char **temporary;

    if (doc == NULL)
    {
        return 0;
    }

    if (doc->count < doc->capacity)
    {
        return 1;
    }

    if (doc->capacity == 0)
    {
        new_capacity = INITIAL_CAPACITY;
    }
    else
    {
        /*
         * Prevent overflow while doubling capacity.
         */
        if (doc->capacity > SIZE_MAX / 2)
        {
            fprintf(stderr, "Error: Document is too large.\n");
            return 0;
        }

        new_capacity = doc->capacity * 2;
    }

    /*
     * Check multiplication overflow.
     */
    if (new_capacity > SIZE_MAX / sizeof(char *))
    {
        fprintf(stderr, "Error: Document is too large.\n");
        return 0;
    }

    temporary = realloc(
        doc->lines,
        new_capacity * sizeof(char *)
    );

    if (temporary == NULL)
    {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        return 0;
    }

    doc->lines = temporary;
    doc->capacity = new_capacity;

    return 1;
}


/* ============================================================
 * DYNAMIC LINE INPUT
 *
 * Reads a complete line of arbitrary reasonable length.
 *
 * Unlike:
 *
 *     char buffer[100];
 *
 * this function dynamically grows the buffer.
 * ============================================================
 */

static char *read_line(void)
{
    size_t capacity = 64;
    size_t length = 0;
    int character;
    char *buffer;
    char *temporary;

    buffer = malloc(capacity);

    if (buffer == NULL)
    {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        return NULL;
    }

    while ((character = getchar()) != '\n' && character != EOF)
    {
        /*
         * Need one extra byte for '\0'.
         */
        if (length + 1 >= capacity)
        {
            /*
             * Prevent overflow.
             */
            if (capacity > SIZE_MAX / 2)
            {
                free(buffer);
                fprintf(stderr, "Error: Input line is too long.\n");
                return NULL;
            }

            capacity *= 2;

            temporary = realloc(buffer, capacity);

            if (temporary == NULL)
            {
                free(buffer);
                fprintf(stderr, "Error: Memory allocation failed.\n");
                return NULL;
            }

            buffer = temporary;
        }

        buffer[length++] = (char)character;
    }

    buffer[length] = '\0';

    /*
     * If EOF occurred before any characters were read,
     * return NULL so the caller can treat it as exit.
     */
    if (character == EOF && length == 0)
    {
        free(buffer);
        return NULL;
    }

    return buffer;
}


/* ============================================================
 * TRIM LEADING AND TRAILING WHITESPACE
 * ============================================================
 */

static char *trim_whitespace(char *str)
{
    char *end;

    if (str == NULL)
    {
        return NULL;
    }

    /*
     * Skip leading whitespace.
     */
    while (isspace((unsigned char)*str))
    {
        str++;
    }

    /*
     * Empty string.
     */
    if (*str == '\0')
    {
        return str;
    }

    /*
     * Find end.
     */
    end = str + strlen(str) - 1;

    /*
     * Remove trailing whitespace.
     */
    while (end > str && isspace((unsigned char)*end))
    {
        *end = '\0';
        end--;
    }

    return str;
}


/* ============================================================
 * PARSE LINE NUMBER SAFELY
 *
 * Accepts:
 *
 *     1
 *     2
 *     100
 *
 * Rejects:
 *
 *     0
 *     -1
 *     abc
 *     1abc
 *     1.5
 *
 * Internally line numbers are converted to zero-based indexes
 * later.
 * ============================================================
 */

static int parse_line_number(const char *text, size_t *line_number)
{
    char *end_pointer;
    unsigned long long value;

    if (text == NULL || line_number == NULL)
    {
        return 0;
    }

    while (isspace((unsigned char)*text))
    {
        text++;
    }

    if (*text == '\0')
    {
        return 0;
    }

    /*
     * Explicitly reject negative numbers.
     */
    if (*text == '-')
    {
        return 0;
    }

    errno = 0;

    value = strtoull(text, &end_pointer, 10);

    if (errno == ERANGE)
    {
        return 0;
    }

    if (end_pointer == text)
    {
        return 0;
    }

    /*
     * Only whitespace is allowed after the number.
     */
    while (isspace((unsigned char)*end_pointer))
    {
        end_pointer++;
    }

    if (*end_pointer != '\0')
    {
        return 0;
    }

    if (value == 0)
    {
        return 0;
    }

    if (value > SIZE_MAX)
    {
        return 0;
    }

    *line_number = (size_t)value;

    return 1;
}


/* ============================================================
 * APPEND
 *
 * Adds a line to the end of the document.
 * ============================================================
 */

static int append_line(Document *doc, const char *text)
{
    char *copy;

    if (doc == NULL || text == NULL)
    {
        return 0;
    }

    if (!ensure_capacity(doc))
    {
        return 0;
    }

    copy = duplicate_string(text);

    if (copy == NULL)
    {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        return 0;
    }

    doc->lines[doc->count] = copy;
    doc->count++;

    return 1;
}


/* ============================================================
 * INSERT
 *
 * User uses 1-based line numbers.
 *
 * Example:
 *
 * Before:
 *
 * 1. A
 * 2. B
 * 3. C
 *
 * insert 2
 *
 * Result:
 *
 * 1. A
 * 2. NEW
 * 3. B
 * 4. C
 *
 * Inserting at count + 1 is allowed.
 * ============================================================
 */

static int insert_line(
    Document *doc,
    size_t line_number,
    const char *text
)
{
    size_t index;
    size_t i;
    char *copy;

    if (doc == NULL || text == NULL)
    {
        return 0;
    }

    /*
     * Valid insertion positions:
     *
     * Empty document -> position 1
     * Existing document -> 1 through count + 1
     */
    if (line_number == 0 || line_number > doc->count + 1)
    {
        fprintf(
            stderr,
            "Error: Invalid insertion position. "
            "Valid range is 1 to %zu.\n",
            doc->count + 1
        );

        return 0;
    }

    if (!ensure_capacity(doc))
    {
        return 0;
    }

    copy = duplicate_string(text);

    if (copy == NULL)
    {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        return 0;
    }

    /*
     * Convert 1-based line number to zero-based index.
     */
    index = line_number - 1;

    /*
     * Shift existing pointers to the right.
     *
     * Start from the end to avoid overwriting pointers.
     */
    for (i = doc->count; i > index; i--)
    {
        doc->lines[i] = doc->lines[i - 1];
    }

    doc->lines[index] = copy;
    doc->count++;

    return 1;
}


/* ============================================================
 * DELETE
 * ============================================================
 */

static int delete_line(Document *doc, size_t line_number)
{
    size_t index;
    size_t i;

    if (doc == NULL)
    {
        return 0;
    }

    if (doc->count == 0)
    {
        fprintf(stderr, "Error: Document is empty.\n");
        return 0;
    }

    if (line_number == 0 || line_number > doc->count)
    {
        fprintf(
            stderr,
            "Error: Invalid line number. "
            "Valid range is 1 to %zu.\n",
            doc->count
        );

        return 0;
    }

    index = line_number - 1;

    /*
     * Free the actual string.
     */
    free(doc->lines[index]);
    doc->lines[index] = NULL;

    /*
     * Shift pointers left.
     */
    for (i = index; i + 1 < doc->count; i++)
    {
        doc->lines[i] = doc->lines[i + 1];
    }

    /*
     * Remove duplicate pointer at the old last position.
     */
    doc->lines[doc->count - 1] = NULL;

    doc->count--;

    return 1;
}


/* ============================================================
 * REPLACE ONE ENTIRE LINE
 * ============================================================
 */

static int replace_line(
    Document *doc,
    size_t line_number,
    const char *text
)
{
    size_t index;
    char *new_copy;

    if (doc == NULL || text == NULL)
    {
        return 0;
    }

    if (doc->count == 0)
    {
        fprintf(stderr, "Error: Document is empty.\n");
        return 0;
    }

    if (line_number == 0 || line_number > doc->count)
    {
        fprintf(
            stderr,
            "Error: Invalid line number. "
            "Valid range is 1 to %zu.\n",
            doc->count
        );

        return 0;
    }

    /*
     * Allocate the new string BEFORE freeing the old one.
     *
     * This is safer if malloc fails.
     */
    new_copy = duplicate_string(text);

    if (new_copy == NULL)
    {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        return 0;
    }

    index = line_number - 1;

    free(doc->lines[index]);

    doc->lines[index] = new_copy;

    return 1;
}


/* ============================================================
 * PRINT DOCUMENT
 * ============================================================
 */

static void print_document(const Document *doc)
{
    size_t i;

    if (doc == NULL)
    {
        return;
    }

    printf("\n========== DOCUMENT ==========\n");

    if (doc->count == 0)
    {
        printf("(empty)\n");
    }
    else
    {
        for (i = 0; i < doc->count; i++)
        {
            printf("%zu. %s\n", i + 1, doc->lines[i]);
        }
    }

    printf("==============================\n");
    printf("Lines: %zu | Capacity: %zu\n\n",
           doc->count,
           doc->capacity);
}


/* ============================================================
 * REPLACE ALL OCCURRENCES IN ONE STRING
 *
 * Example:
 *
 * source      = "I like C. C is powerful."
 * find        = "C"
 * replacement = "C++"
 *
 * result:
 *
 * "I like C++. C++ is powerful."
 * ============================================================
 */

static char *replace_all_occurrences(
    const char *source,
    const char *find,
    const char *replacement
)
{
    size_t source_length;
    size_t find_length;
    size_t replacement_length;
    size_t occurrence_count = 0;
    size_t result_length;
    size_t i;
    size_t copy_position = 0;

    const char *search_position;
    const char *match;

    char *result;

    if (source == NULL || find == NULL || replacement == NULL)
    {
        return NULL;
    }

    source_length = strlen(source);
    find_length = strlen(find);
    replacement_length = strlen(replacement);

    /*
     * Empty search string is rejected.
     */
    if (find_length == 0)
    {
        return NULL;
    }

    /*
     * Count occurrences.
     *
     * We use strstr() to locate each occurrence.
     */
    search_position = source;

    while ((match = strstr(search_position, find)) != NULL)
    {
        occurrence_count++;

        search_position = match + find_length;
    }

    /*
     * No match.
     *
     * Return a duplicate of the original string.
     */
    if (occurrence_count == 0)
    {
        return duplicate_string(source);
    }

    /*
     * Calculate resulting string length safely.
     *
     * result_length =
     *
     * source_length
     * - total length of found strings
     * + total length of replacement strings
     */
    if (find_length > source_length)
    {
        return NULL;
    }

    /*
     * Check multiplication overflow:
     * occurrence_count * find_length
     */
    if (occurrence_count > SIZE_MAX / find_length)
    {
        return NULL;
    }

    {
        size_t removed_length = occurrence_count * find_length;

        if (replacement_length >= find_length)
        {
            size_t added_per_occurrence =
                replacement_length - find_length;

            if (occurrence_count > SIZE_MAX / added_per_occurrence &&
                added_per_occurrence != 0)
            {
                return NULL;
            }

            {
                size_t added_length =
                    occurrence_count * added_per_occurrence;

                if (source_length > SIZE_MAX - added_length)
                {
                    return NULL;
                }

                result_length = source_length + added_length;
            }
        }
        else
        {
            size_t reduced_length =
                occurrence_count * (find_length - replacement_length);

            if (reduced_length > source_length)
            {
                return NULL;
            }

            result_length = source_length - reduced_length;
        }

        /*
         * Prevent unused-variable warning in some compilers.
         */
        (void)removed_length;
    }

    /*
     * +1 for null terminator.
     */
    if (result_length == SIZE_MAX)
    {
        return NULL;
    }

    result = malloc(result_length + 1);

    if (result == NULL)
    {
        return NULL;
    }

    /*
     * Build the resulting string.
     */
    search_position = source;

    while ((match = strstr(search_position, find)) != NULL)
    {
        size_t prefix_length =
            (size_t)(match - search_position);

        /*
         * Copy text before match.
         */
        if (prefix_length > 0)
        {
            memcpy(
                result + copy_position,
                search_position,
                prefix_length
            );

            copy_position += prefix_length;
        }

        /*
         * Copy replacement.
         */
        if (replacement_length > 0)
        {
            memcpy(
                result + copy_position,
                replacement,
                replacement_length
            );

            copy_position += replacement_length;
        }

        search_position = match + find_length;
    }

    /*
     * Copy remaining text.
     */
    i = strlen(search_position);

    if (i > 0)
    {
        memcpy(
            result + copy_position,
            search_position,
            i
        );

        copy_position += i;
    }

    result[copy_position] = '\0';

    return result;
}


/* ============================================================
 * FIND AND REPLACE
 *
 * The user is prompted for:
 *
 *     Find:
 *     Replace with:
 *
 * It operates on every line.
 * ============================================================
 */

static int find_and_replace(Document *doc)
{
    char *find_text;
    char *replacement_text;
    size_t i;
    size_t replacements = 0;

    if (doc == NULL)
    {
        return 0;
    }

    if (doc->count == 0)
    {
        fprintf(stderr, "Error: Document is empty.\n");
        return 0;
    }

    printf("Find: ");
    fflush(stdout);

    find_text = read_line();

    if (find_text == NULL)
    {
        fprintf(stderr, "Error: Unable to read input.\n");
        return 0;
    }

    if (strlen(find_text) == 0)
    {
        fprintf(stderr, "Error: Search text cannot be empty.\n");
        free(find_text);
        return 0;
    }

    printf("Replace with: ");
    fflush(stdout);

    replacement_text = read_line();

    if (replacement_text == NULL)
    {
        fprintf(stderr, "Error: Unable to read input.\n");
        free(find_text);
        return 0;
    }

    for (i = 0; i < doc->count; i++)
    {
        char *new_line;
        char *old_line;

        /*
         * First generate the new version.
         */
        new_line = replace_all_occurrences(
            doc->lines[i],
            find_text,
            replacement_text
        );

        if (new_line == NULL)
        {
            fprintf(
                stderr,
                "Error: Memory allocation failed while "
                "processing line %zu.\n",
                i + 1
            );

            free(find_text);
            free(replacement_text);

            return 0;
        }

        /*
         * Check whether this line actually changed.
         */
        if (strcmp(new_line, doc->lines[i]) != 0)
        {
            old_line = doc->lines[i];

            doc->lines[i] = new_line;

            free(old_line);

            replacements++;
        }
        else
        {
            /*
             * No change needed.
             */
            free(new_line);
        }
    }

    printf(
        "Find and replace complete. "
        "Modified %zu line(s).\n",
        replacements
    );

    free(find_text);
    free(replacement_text);

    return 1;
}


/* ============================================================
 * HELP
 * ============================================================
 */

static void print_help(void)
{
    printf("\n");
    printf("============== LINE EDITOR HELP ==============\n\n");

    printf("append\n");
    printf("  Add a new line at the end of the document.\n");
    printf("  Example:\n");
    printf("      > append\n");
    printf("      Enter text: Hello World\n\n");

    printf("insert <line_number>\n");
    printf("  Insert a new line at the specified position.\n");
    printf("  Existing lines move down.\n");
    printf("  Example:\n");
    printf("      > insert 2\n");
    printf("      Enter text: New line\n\n");

    printf("print\n");
    printf("  Display all lines with their line numbers.\n");
    printf("  Example:\n");
    printf("      > print\n\n");

    printf("delete <line_number>\n");
    printf("  Delete the specified line.\n");
    printf("  Example:\n");
    printf("      > delete 2\n\n");

    printf("replace <line_number>\n");
    printf("  Replace the entire contents of a line.\n");
    printf("  Example:\n");
    printf("      > replace 2\n");
    printf("      Enter text: Updated line\n\n");

    printf("findreplace\n");
    printf("  Find text and replace it throughout the document.\n");
    printf("  Example:\n");
    printf("      > findreplace\n");
    printf("      Find: C\n");
    printf("      Replace with: C++\n\n");

    printf("help\n");
    printf("  Display this help information.\n");
    printf("  Example:\n");
    printf("      > help\n\n");

    printf("quit\n");
    printf("  Exit the editor.\n");
    printf("  Example:\n");
    printf("      > quit\n\n");

    printf("================================================\n\n");
}


/* ============================================================
 * COMMAND HANDLER
 * ============================================================
 */

static void handle_command(Document *doc, char *input)
{
    char *command;
    char *arguments;
    size_t line_number;

    if (doc == NULL || input == NULL)
    {
        return;
    }

    /*
     * Remove leading/trailing whitespace.
     */
    input = trim_whitespace(input);

    /*
     * Empty command.
     */
    if (*input == '\0')
    {
        printf("Error: Empty command. Type 'help' for commands.\n");
        return;
    }

    /*
     * Separate command from arguments.
     */
    command = input;

    while (*input != '\0' &&
           !isspace((unsigned char)*input))
    {
        input++;
    }

    if (*input != '\0')
    {
        *input = '\0';
        input++;
    }

    arguments = trim_whitespace(input);

    /*
     * =========================
     * APPEND
     * =========================
     */

    if (strcmp(command, "append") == 0)
    {
        char *text;

        if (*arguments != '\0')
        {
            printf(
                "Usage: append\n"
                "Text will be requested after the command.\n"
            );

            return;
        }

        printf("Enter text: ");
        fflush(stdout);

        text = read_line();

        if (text == NULL)
        {
            printf("Error: Unable to read line.\n");
            return;
        }

        if (append_line(doc, text))
        {
            printf(
                "Line appended successfully. "
                "Line number: %zu\n",
                doc->count
            );
        }

        free(text);

        return;
    }


    /*
     * =========================
     * INSERT
     * =========================
     */

    if (strcmp(command, "insert") == 0)
    {
        char *text;

        if (!parse_line_number(arguments, &line_number))
        {
            printf(
                "Usage: insert <line_number>\n"
            );

            return;
        }

        printf("Enter text: ");
        fflush(stdout);

        text = read_line();

        if (text == NULL)
        {
            printf("Error: Unable to read line.\n");
            return;
        }

        if (insert_line(doc, line_number, text))
        {
            printf(
                "Line inserted successfully at position %zu.\n",
                line_number
            );
        }

        free(text);

        return;
    }


    /*
     * =========================
     * PRINT
     * =========================
     */

    if (strcmp(command, "print") == 0)
    {
        if (*arguments != '\0')
        {
            printf("Usage: print\n");
            return;
        }

        print_document(doc);

        return;
    }


    /*
     * =========================
     * DELETE
     * =========================
     */

    if (strcmp(command, "delete") == 0)
    {
        if (!parse_line_number(arguments, &line_number))
        {
            printf(
                "Usage: delete <line_number>\n"
            );

            return;
        }

        if (delete_line(doc, line_number))
        {
            printf(
                "Line %zu deleted successfully.\n",
                line_number
            );
        }

        return;
    }


    /*
     * =========================
     * REPLACE
     * =========================
     */

    if (strcmp(command, "replace") == 0)
    {
        char *text;

        if (!parse_line_number(arguments, &line_number))
        {
            printf(
                "Usage: replace <line_number>\n"
            );

            return;
        }

        printf("Enter new text: ");
        fflush(stdout);

        text = read_line();

        if (text == NULL)
        {
            printf("Error: Unable to read line.\n");
            return;
        }

        if (replace_line(doc, line_number, text))
        {
            printf(
                "Line %zu replaced successfully.\n",
                line_number
            );
        }

        free(text);

        return;
    }


    /*
     * =========================
     * FIND AND REPLACE
     * =========================
     */

    if (strcmp(command, "findreplace") == 0 ||
        strcmp(command, "find_replace") == 0)
    {
        if (*arguments != '\0')
        {
            printf("Usage: findreplace\n");
            return;
        }

        find_and_replace(doc);

        return;
    }


    /*
     * =========================
     * HELP
     * =========================
     */

    if (strcmp(command, "help") == 0)
    {
        if (*arguments != '\0')
        {
            printf("Usage: help\n");
            return;
        }

        print_help();

        return;
    }


    /*
     * =========================
     * QUIT
     * =========================
     */

    if (strcmp(command, "quit") == 0 ||
        strcmp(command, "exit") == 0)
    {
        if (*arguments != '\0')
        {
            printf("Usage: quit\n");
            return;
        }

        /*
         * Main loop checks for "quit" separately.
         */
        return;
    }


    /*
     * =========================
     * UNKNOWN COMMAND
     * =========================
     */

    printf(
        "Error: Unknown command '%s'.\n"
        "Type 'help' to see available commands.\n",
        command
    );
}


/* ============================================================
 * MAIN
 * ============================================================
 */

int main(void)
{
    Document document;
    char *input;
    char *trimmed;

    init_document(&document);

    printf("\n");
    printf("=============================================\n");
    printf("          COMMAND-LINE LINE EDITOR\n");
    printf("=============================================\n");
    printf("Type 'help' to see available commands.\n");
    printf("Type 'quit' to exit.\n");
    printf("=============================================\n\n");

    while (1)
    {
        printf("> ");
        fflush(stdout);

        input = read_line();

        /*
         * EOF / input failure.
         *
         * Exit safely.
         */
        if (input == NULL)
        {
            printf("\nInput ended. Exiting editor...\n");
            break;
        }

        trimmed = trim_whitespace(input);

        /*
         * Empty input.
         */
        if (*trimmed == '\0')
        {
            printf(
                "Error: Empty command. "
                "Type 'help' for available commands.\n"
            );

            free(input);
            continue;
        }

        /*
         * Check quit before passing to command handler.
         *
         * We copy the command into a small local buffer
         * so we can safely determine whether it is "quit".
         */
        {
            char command_check[COMMAND_SIZE];
            size_t i = 0;

            while (trimmed[i] != '\0' &&
                   !isspace((unsigned char)trimmed[i]) &&
                   i < COMMAND_SIZE - 1)
            {
                command_check[i] = trimmed[i];
                i++;
            }

            command_check[i] = '\0';

            if (strcmp(command_check, "quit") == 0 ||
                strcmp(command_check, "exit") == 0)
            {
                /*
                 * Make sure there are no extra arguments.
                 */
                char *after_command = trimmed + i;

                after_command = trim_whitespace(after_command);

                if (*after_command == '\0')
                {
                    free(input);
                    break;
                }
            }
        }

        handle_command(&document, input);

        free(input);
    }

    /*
     * CRITICAL:
     *
     * Every dynamically allocated line is freed,
     * then the dynamic array itself is freed.
     */
    free_document(&document);

    printf("All allocated memory has been released.\n");
    printf("Goodbye!\n");

    return 0;
}