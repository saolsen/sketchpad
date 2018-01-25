// @NOTE: ASIDE, this is just a thing for later.
char *readEntireFileIntoMemoryAndNullTerminate(char *filename) {
    char *result = NULL;

    FILE *file = fopen("file.h", "r");
    if(file) {
        fseek(file, 0, SEEK_END);
        size_t file_size = ftell(file);
        fseek(file, 0, SEEK_SET);

        result = malloc(file_size + 1);
        fread(result, file_size, 1, file);
        result[file_size] = '\0';
        fclose(file);
    }

    return result;
}
