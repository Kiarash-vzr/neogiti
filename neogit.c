#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#define MAX_FILENAME_LENGTH 1000
#define MAX_COMMIT_MESSAGE_LENGTH 2000
#define MAX_LINE_LENGTH 1000
#define MAX_MESSAGE_LENGTH 1000
#define debug(x) printf("%s", x);
#define BUFFER_SIZE 1024

int compareFiles(const char *file1, const char *file2)
{
    FILE *f1 = fopen(file1, "rb");
    FILE *f2 = fopen(file2, "rb");

    if (f1 == NULL || f2 == NULL)
    {
        perror("Error opening files");
        return -1;
    }

    int result = 1; // files are identical

    char buffer1[BUFFER_SIZE], buffer2[BUFFER_SIZE];
    size_t bytesRead1, bytesRead2;

    do
    {
        bytesRead1 = fread(buffer1, 1, sizeof(buffer1), f1);
        bytesRead2 = fread(buffer2, 1, sizeof(buffer2), f2);

        if (bytesRead1 != bytesRead2 || memcmp(buffer1, buffer2, bytesRead1) != 0)
        {
            result = 0; // files are different
            break;
        }

    } while (bytesRead1 > 0 && bytesRead2 > 0);

    fclose(f1);
    fclose(f2);
    return result;
}

int in_file(const char *s, const char *filename)
{
    FILE *file = fopen(filename, "r");
    int found = 0;
    char line[1024];
    while (fgets(line, sizeof(line), file) != NULL)
    {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, s) == 0)
        {
            found = 1;
            break;
        }
    }
    fclose(file);
    return found;
}

int min(int a, int b)
{
    if (b < a)
        return b;
    return a;
}

void change_word(char *word, char a, char b)
{
    for (int i = 0; i < strlen(word); i++)
    {
        if (word[i] == a)
        {
            word[i] = b;
        }
    }
}

int number_of_lines(char *add)
{
    FILE *ptr = fopen(add, "r");
    int i = 0;
    char line[1000];
    while (fgets(line, sizeof(line), ptr) != NULL)
    {
        i++;
    }
    fclose(ptr);
    return i;
}

int fins(char *first, char *second)
{
    if (strncmp(first, second, min(strlen(first), strlen(second))) == 0)
    {
        if (strlen(first) <= strlen(second))
            return 1;
    }
    return 0;
}
int *lines_of_ers(int *lines, char *address, char *entry)
{
    FILE *ptr = fopen(address, "r");
    char line[1000];
    int x = 0;
    int i = 0;
    while (fgets(line, sizeof(line), ptr) != NULL)
    {
        line[strlen(line) - 1] = '\0';
        if (fins(entry, line))
        {
            lines[x] = i;
            x++;
        }
        i++;
    }
    fclose(ptr);
    return lines;
}
int is_file(char *path)
{
    struct stat file_stat;
    if (stat(path, &file_stat) == 0)
    {
        if (S_ISREG(file_stat.st_mode))
        {
            return 1;
        }
        else if (S_ISDIR(file_stat.st_mode))
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return -1;
    }
}
char *out_line(int number, char *add, char *line2)
{
    FILE *ptr;
    ptr = fopen(add, "r");
    char line[MAX_LINE_LENGTH];
    fgets(line, MAX_LINE_LENGTH, ptr);
    for (int i = 0; i < number; i++)
    {
        fgets(line, MAX_LINE_LENGTH, ptr);
    }
    line[strlen(line) - 1] = '\0';
    strcpy(line2, line);
    fclose(ptr);
    return line2;
}

int copy_file(int number, char *add, char *new_add, char *entry)
{
    FILE *ptr = fopen(add, "r");
    if (ptr == NULL)
        return 1;
    FILE *new_file = fopen(new_add, "w");
    int i = 0;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, ptr) != NULL)
    {
        if (i == number)
        {
            fprintf(new_file, "%s\n", entry);
        }
        else
        {
            fprintf(new_file, "%s", line);
        }
        i++;
    }
    fclose(new_file);
    fclose(ptr);
    remove(add);
    rename(new_add, add);
}

void delete_line(const char *filename, int lineToDelete)
{
    FILE *originalFile = fopen(filename, "r");
    FILE *tempFile = fopen("temp.txt", "w");

    if (originalFile == NULL || tempFile == NULL)
    {
        perror("Error opening files");
        exit(EXIT_FAILURE);
    }

    char buffer[1024];
    int currentLine = 1;

    while (fgets(buffer, sizeof(buffer), originalFile) != NULL)
    {
        if (currentLine != lineToDelete)
        {
            fputs(buffer, tempFile);
        }
        currentLine++;
    }

    fclose(originalFile);
    fclose(tempFile);

    // rempvope the original file
    if (remove(filename) != 0)
    {
        perror("Error removing original file");
        exit(EXIT_FAILURE);
    }
    // Rename temp file to original file
    if (rename("temp.txt", filename) != 0)
    {
        perror("Error renaming file");
        exit(EXIT_FAILURE);
    }
}

void add_to_file(char *entry, char *add)
{
    FILE *ptr = fopen(add, "a");
    fprintf(ptr, "%s\n", entry);
    fclose(ptr);
}

void tree(const char *dirPath, char *address)
{
    DIR *dir;
    struct dirent *entry;
    if (!(dir = opendir(dirPath)))
    {
        perror("opendir");
        return;
    }
    while ((entry = readdir(dir)) != NULL)
    {
        char path[1024];
        snprintf(path, sizeof(path), "%s\\%s", dirPath, entry->d_name);
        if (is_file(path) == 0)
        {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
            {
                tree(path, address);
            }
        }
        else
        {
            add_to_file(path, address);
        }
    }
    closedir(dir);
}

void stat_tree(const char *dirPath, char *address)
{
    DIR *dir;
    struct dirent *entry;
    if (!(dir = opendir(dirPath)))
    {
        perror("opendir");
        return;
    }
    while ((entry = readdir(dir)) != NULL)
    {
        char path[1024];
        snprintf(path, sizeof(path), "%s\\%s", dirPath, entry->d_name);
        if (is_file(path) == 0)
        {
            if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..") && strcmp(entry->d_name, ".neogit") && strcmp(entry->d_name, ".git"))
            {
                stat_tree(path, address);
            }
        }
        else if (strcmp(entry->d_name, "neogit.c") && strcmp(entry->d_name, "neogit.exe"))
        {
            add_to_file(path, address);
        }
    }
    closedir(dir);
}

int stage_emp()
{
    FILE *ptr = fopen(".neogit\\staging", "r");
    char line[1024];
    while (fgets(line, sizeof(line), ptr) != NULL)
    {
        if (strncmp(line, "*", 1) != 0 && strcmp(line, "\n") != 0)
        {
            return 0; // non-empty
        }
    }
    return 1; // empty
}

int count_stage()
{
    FILE *ptr = fopen(".neogit\\staging", "r");
    char line[1024];
    int y = 0;
    while (fgets(line, sizeof(line), ptr) != NULL)
    {
        if (strncmp(line, "*", 1) != 0 && strcmp(line, "\n") != 0)
        {
            y++;
        }
    }
    return y;
}
void print_command(int argc, char *const argv[]);
void find_name(char *name)
{
    char cwd[1024];
    char tmp_cwd[1024];
    bool exists = false;
    struct dirent *entry;
    do
    {
        // find .neogit
        DIR *dir = opendir(".");
        if (dir == NULL)
        {
            perror("Error opening current directory");
        }
        if (is_file(name) == 0)
        {
            exists = true;
        }
        // while ((entry = readdir(dir)) != NULL) {                         ///////////////////////// CHANGE DI_DIR ///////////////////////
        //     if (entry->d_type == DT_DIR && strcmp(entry->d_name, name) == 0){
        //         exists = true;
        //         break;
        //     }
        // }
        closedir(dir);
        // update current working directory
        getcwd(tmp_cwd, sizeof(tmp_cwd));
        // change cwd to parent
        if (strlen(tmp_cwd) > 4 && !exists)
        {
            chdir("..") != 0;
        }
    } while (strlen(tmp_cwd) > 4 && !exists);
}
int run_init(int argc, char *const argv[]);
int create_configs(char *username, char *email, int halat);

int run_add(int argc, char *const argv[]);
int add_to_staging(char *filepath);

int remove_from_staging(char *filepath);

int run_commit(int argc, char *const argv[]);
int inc_last_commit_ID();
bool check_file_directory_exists(char *filepath);
int commit_staged_file(int commit_ID, char *filepath);
int track_file(char *filepath);
bool is_tracked(char *filepath);
int create_commit_file(int commit_ID, char *message);
int find_file_last_commit(char *filepath);

int parseTime(const char *timeString, struct tm *tmResult)
{
    const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    char month[4];
    int day, year, hour, minute, second;

    if (sscanf(timeString, "%*s %3s %d %d:%d:%d %d", month, &day, &hour, &minute, &second, &year) != 6)
    {
        fprintf(stderr, "Error parsing time string\n");
        return 0;
    }
    int monthIndex;
    for (monthIndex = 0; monthIndex < 12; ++monthIndex)
    {
        if (strcmp(month, months[monthIndex]) == 0)
        {
            break;
        }
    }

    if (monthIndex == 12)
    {
        fprintf(stderr, "Invalid month in time string\n");
        return 0;
    }

    memset(tmResult, 0, sizeof(struct tm));
    tmResult->tm_sec = second;
    tmResult->tm_min = minute;
    tmResult->tm_hour = hour;
    tmResult->tm_mday = day;
    tmResult->tm_mon = monthIndex;
    tmResult->tm_year = year - 1900;

    return 1;
}

double calculateTimeDifference(const char *timeString1, const char *timeString2)
{
    struct tm tm1, tm2;
    time_t time1, time2;

    if (!parseTime(timeString1, &tm1))
    {
        fprintf(stderr, "Error parsing time string 1\n");
        return -1.0;
    }
    time1 = mktime(&tm1);

    if (!parseTime(timeString2, &tm2))
    {
        fprintf(stderr, "Error parsing time string 2\n");
        return -1.0;
    }
    time2 = mktime(&tm2);

    return difftime(time2, time1);
}

int run_checkout(int argc, char *const argv[]);
int find_file_last_change_before_commit(char *filepath, int commit_ID);
int checkout_file(char *filepath, int commit_ID);

int last_commit()
{
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    find_name(".neogit");
    char comcount[1024];
    FILE *ptr = fopen(".neogit\\commit", "r");
    fgets(comcount, sizeof(comcount), ptr);
    int comcount2 = atoi(comcount);
    char branch[1024];
    out_line(4, ".neogit\\config", branch);
    char tmp[1024];
    for (int i = comcount2; i > 0; i--)
    {
        if (strcmp(out_line(i, ".neogit\\comat\\branch", tmp), branch) == 0)
        {
            chdir(cwd);
            return i;
        }
    }
    chdir(cwd);
    return -1;
}

void print_command(int argc, char *const argv[])
{
    for (int i = 0; i < argc; i++)
    {
        fprintf(stdout, "%s ", argv[i]);
    }
    fprintf(stdout, "\n");
}
int run_init(int argc, char *const argv[])
{
    char cwd[1024];
    char vcwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
        return 1;
    char tmp_cwd[1024];
    bool exists = false;
    struct dirent *entry;
    do
    {
        // find .neogit
        DIR *dir = opendir(".");
        if (dir == NULL)
        {
            perror("Error opening current directory");
            return 1;
        }
        if (is_file("neogit") == 0)
        {
            exists = true;
        }
        closedir(dir);

        // update current working directory
        if (getcwd(tmp_cwd, sizeof(tmp_cwd)) == NULL)
            return 1;

        // change cwd to parent
        if (strlen(tmp_cwd) > 4 && !exists)
        {
            if (chdir("..") != 0)
                return 1;
        }

    } while (strlen(tmp_cwd) > 4 && !exists);
    // return to the initial cwd
    if (chdir(cwd) != 0)
        return 1;

    if (!exists)
    {
        char add[1000] = "C:/Users/ASUS/Desktop/globals/global.txt";
        add_to_file(cwd, add);
        if (mkdir(".neogit") != 0)
            return 1;
        FILE *file;
        // create commits folder
        if (mkdir(".neogit/commits") != 0)
            return 1;
        if (mkdir(".neogit/stage_area") != 0)
            return 1;
        // create files folder
        if (mkdir(".neogit/files") != 0)
            return 1;
        if (mkdir(".neogit/comat") != 0)
            return 1;
        if (mkdir(".neogit/del_stage") != 0)
            return 1;
        file = fopen(".neogit/staging", "w");
        fclose(file);
        file = fopen(".neogit/tracks", "w");
        fclose(file);
        create_configs(" ", " ", 0);
        file = fopen(".neogit/alias", "w");
        fclose(file);
        file = fopen(".neogit/relstaging", "w");
        fclose(file);
        file = fopen(".neogit/branches", "w");
        fprintf(file, "master\n");
        fclose(file);
        // commit data
        file = fopen(".neogit/comat/id", "w");
        fclose(file);
        file = fopen(".neogit/comat/time", "w");
        fclose(file);
        file = fopen(".neogit/comat/message", "w");
        fclose(file);
        file = fopen(".neogit/comat/branch", "w");
        fclose(file);
        file = fopen(".neogit/comat/name", "w");
        fclose(file);
        file = fopen(".neogit/comat/count", "w");
        fclose(file);
        file = fopen(".neogit/status", "w");
        fclose(file);
    }
    else
    {
        perror("neogit repository has already initialized");
    }
    return 0;
}
int create_configs(char *username, char *email, int halat)
{
    char entry[1000];
    strcpy(entry, email);
    if (halat == 0)
        strcpy(entry, username);
    FILE *file = fopen(".neogit/config", "r");
    if (file == NULL)
    {
        fclose(file);
        file = fopen(".neogit/config", "w");
        fprintf(file, "%s\n", username); // username
        fprintf(file, "%s\n", email);    // email
        fprintf(file, "%d\n", 0);        // last_id
        fprintf(file, "%d\n", 0);        // alan_id
        fprintf(file, "%s\n", "master"); // shakhe
        fclose(file);
    }
    else
    {
        fclose(file);
        copy_file(halat, ".neogit/config", ".neogit/confayakon", entry);
    }

    return 0;
}

int is_wild(char *wild)
{
    int star = -1;
    for (int i = 0; i < strlen(wild); i++)
    {
        if (*(wild + i) == '*')
        {
            star = i;
            break;
        }
    }
    return star;
}

int wildcard(char *wild, char *check)
{
    int star = -1;
    for (int i = 0; i < strlen(wild); i++)
    {
        if (*(wild + i) == '*')
        {
            star = i;
            break;
        }
    }
    for (int i = 0; i < star; i++)
    {
        if (*(check + i) != *(wild + i))
        {
            return 0;
        }
    }
    for (int i = star + 1; i < strlen(wild); i++)
    {
        if (*(check + i + strlen(check) - strlen(wild)) != *(wild + i))
        {
            return 0;
        }
    }
    return 1;
}

void add_wildcard(char *wild, char *s_path)
{
    DIR *dir;
    char qwe[1024] = "";
    struct dirent *entry;
    if (!(dir = opendir(s_path)))
    {
        perror("opendir");
        return;
    }
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            char path[1024];
            snprintf(path, sizeof(path), "%s/%s", s_path, entry->d_name);
            if (wildcard(wild, entry->d_name))
            {
                strcat(qwe, "neogit add ");
                strcat(qwe, entry->d_name);
                printf("%s", qwe);
                system(qwe);
            }
        }
    }
}

// bayad check shavad datoor be soorate: neogit add -f :bashad.
void f_add(int argc, char *const argv[])
{
    char *out[1000];
    strcpy(out[0], "neogit");
    strcpy(out[1], "add");
    for (int i = 3; i < argc - 1; i++)
    {
        strcpy(out[2], argv[i]);
        run_add(3, out);
    }
    return;
}

int run_add(int argc, char *const argv[])
{
    int x = 2;
    if (strcmp(argv[2], "-f") == 0)
    {
        x = 3;
    }
    // TODO: handle command in non-root directories
    if (argc < 3)
    {
        perror("please specify a file");
        return 1;
    }
    else if (strcmp(argv[2], "-undo") || strcmp(argv[1], "reset"))
    {
        for (x; x < argc; x++)
        {
            char cwd[1000];
            getcwd(cwd, sizeof(cwd));
            int flag2 = !(argv[x][1] == ':' && argv[x][0] == 'C');
            char tmpcwd[1000] = "";
            strcpy(tmpcwd, cwd);
            if (flag2)
            {
                strcat(cwd, "\\");
                strcat(cwd, argv[x]);
            }
            else
            {
                strcpy(cwd, argv[x]);
            }
            char *tmp;
            int flag = 0;
            if (is_file(cwd) != -1)
            {
                char command[1000];
                if (is_file(cwd) == 1)
                {
                    tmp = strrchr(cwd, '\\');
                    *tmp = '\0';
                    chdir(cwd);
                    getcwd(cwd, sizeof(cwd));
                    snprintf(cwd, sizeof(cwd), "%s\\%s", cwd, tmp + 1);
                    find_name(".neogit");
                    add_to_file(cwd, ".neogit/relstaging");
                    char line[1000];
                }
                else
                {
                    chdir(cwd);
                    getcwd(cwd, sizeof(cwd));
                    find_name(".neogit");
                    tree(cwd, ".neogit/relstaging");
                }
                FILE *ptr_rel = fopen(".neogit/relstaging", "r");
                char entry[1000];
                while (fgets(entry, sizeof(entry), ptr_rel) != NULL)
                {
                    entry[strlen(entry) - 1] = '\0';
                    char new_name[1000];
                    strcpy(new_name, entry);
                    change_word(new_name, '\\', '$');
                    char *tmp = strrchr(entry, '\\');
                    strcpy(new_name, new_name + 2);
                    chdir(".neogit\\stage_area");
                    remove(new_name);
                    if (strcmp(argv[1], "add") == 0)
                    {
                        snprintf(command, sizeof(command), "xcopy /Y %s", entry);
                        system(command);
                        rename(tmp + 1, new_name);
                    }
                    find_name(".neogit");
                    FILE *ptr = fopen(".neogit/staging", "r");
                    char line[1000];
                    int lines[100];
                    int i = 0;
                    while (fgets(line, sizeof(line), ptr) != NULL)
                    {
                        line[strlen(line) - 1] = '\0';
                        if (strcmp(line, entry) == 0)
                        {
                            break;
                        }
                        i++;
                    }
                    fclose(ptr);
                    delete_line(".neogit/staging", i + 1);
                    if (strcmp(argv[1], "add") == 0)
                    {
                        add_to_file(entry, ".neogit/staging");
                    }
                }
                fclose(ptr_rel);
                remove(".neogit/relstaging");
                FILE *del = fopen(".neogit/relstaging", "w");
                fclose(del);
                chdir(tmpcwd);
            }
            else ///////////////////////////////////////////////////////////////////////////////
            {
                perror("no such file or directory exists!");
            }
        }
        if (strcmp(argv[1], "add") == 0)
        {
            add_to_file("*", ".neogit/staging");
        }
    }
    else if (argc == 3)
    {
        char cwd[1000];
        getcwd(cwd, sizeof(cwd));
        find_name(".neogit");
        int n = number_of_lines(".neogit/staging");
        char line[1000];
        int i = n - 1;
        for (i; i >= 0; i--)
        {
            out_line(i, ".neogit/staging", line);
            if (strcmp(line, "*") != 0)
                break;
        }
        for (int j = i; j >= 0; j--)
        {
            char command[1000] = "neogit reset ";
            out_line(j, ".neogit/staging", line);
            if (strcmp(line, "*") == 0)
                break;
            strcat(command, line);
            system(command);
        }
        chdir(cwd);
    }
    else
    {
        perror("sdfsdf");
    }
}

int run_commit(int argc, char *const argv[])
{
    if (argc == 3)
    {
        perror("please enter a commit message");
        return 1;
    }
    else if (argc == 4)
    {
        if (strlen(argv[3]) > 72)
        {
            perror("commit message is too long");
        }
        else
        {
            char cwd1[1024];
            getcwd(cwd1, sizeof(cwd1));
            find_name(".neogit");
            // is stage_area empty ?
            if (stage_emp())
            {
                perror("no files staged! nothing to commit.");
                chdir(cwd1);
                return 1;
            }
            else
            {
                // which commit is it ? (commit count) int y;
                int y;
                FILE *ptr = fopen(".neogit\\commit", "r");
                if (ptr == NULL)
                {
                    fclose(ptr);
                    ptr = fopen(".neogit\\commit", "w");
                    fprintf(ptr, "1");
                    fclose(ptr);
                    y = 1;
                }
                else
                {
                    char xtmp[1024];
                    fscanf(ptr, "%s", xtmp);
                    y = atoi(xtmp);
                    fclose(ptr);
                    ptr = fopen(".neogit\\commit", "w");
                    fprintf(ptr, "%d", y + 1);
                    fclose(ptr);
                    y++;
                }

                // saving commit data

                // time
                time_t currentTime = time(NULL);
                if (currentTime == -1)
                {
                    perror("Error getting current time");
                    return 1;
                }
                char *timeString = ctime(&currentTime);
                FILE *tmp;
                char tmptxt[1024] = "";

                tmp = fopen(".neogit/comat/time", "a");
                fprintf(tmp, "%s", timeString);
                fclose(tmp);

                // id
                tmp = fopen(".neogit/comat/id", "a");
                fprintf(tmp, "%d\n", y);
                fclose(tmp);

                // name
                tmp = fopen(".neogit/config", "r");
                out_line(0, ".neogit/config", tmptxt);
                fclose(tmp);
                tmp = fopen(".neogit/comat/name", "a");
                fprintf(tmp, "%s\n", tmptxt);
                fclose(tmp);

                // branch
                tmp = fopen(".neogit/config", "r");
                out_line(4, ".neogit/config", tmptxt);
                fclose(tmp);
                tmp = fopen(".neogit/comat/branch", "a");
                fprintf(tmp, "%s\n", tmptxt);
                fclose(tmp);

                // message
                strcpy(tmptxt, argv[3]);
                tmp = fopen(".neogit/comat/message", "a");
                fprintf(tmp, "%s\n", tmptxt);
                fclose(tmp);

                // count
                int tmpy = count_stage();
                tmp = fopen(".neogit/comat/count", "a");
                fprintf(tmp, "%d\n", tmpy);
                fclose(tmp);

                // making the destination directory
                char x[1024];
                sprintf(x, "%d", y);
                char address[1024] = ".neogit\\commits\\commit";
                strcat(address, x);
                mkdir(address);

                // copying the last commit on this branch
                tmp = fopen(".neogit/comat/branches", "r");
                FILE *tmp2 = fopen(".neogit/config", "r");
                out_line(4, ".neogit/config", tmptxt);
                char ghgh[1024];
                for (int i = y - 1; i > -1; i--)
                {
                    if (strcmp(out_line(i, ".neogit/comat/branch", ghgh), tmptxt) == 0)
                    {
                        char verytmp[1024] = "xcopy /q /y .neogit\\commits\\commit";
                        char tootmp[1024];
                        sprintf(tootmp, "%d", i);
                        strcat(verytmp, tootmp);
                        strcat(verytmp, " .neogit\\commits\\commit");
                        sprintf(tootmp, "%d", y);
                        strcat(verytmp, tootmp);
                        system(verytmp);
                        break;
                    }
                }
                fclose(tmp);
                fclose(tmp2);

                // copying staged files to the destination
                char sysy[1024] = "xcopy /q /y .neogit\\stage_area ";
                strcat(sysy, address);
                system(sysy);

                // removing files from staged_area dir and staging file
                FILE *gh = fopen(".neogit\\staging", "w");
                fclose(gh);
                system("del /Q /S /F .neogit\\stage_area\\*.*");

                printf("commit id:%d\ncommit time:%scommit message:%s", y, timeString, argv[3]);
            }
            chdir(cwd1);
        }
    }
    else
    {
        perror("If your message has white_spaces, it must include double qoutes at the beginning and at the end of the message");
        return 1;
    }
}

void show_log(int i, char *tmpx)
{
    printf("\n");
    printf("time: %s\n", out_line(i, ".neogit/comat/time", tmpx));
    printf("message: %s\n", out_line(i, ".neogit/comat/message", tmpx));
    printf("author: %s\n", out_line(i, ".neogit/comat/name", tmpx));
    printf("id: %s\n", out_line(i, ".neogit/comat/id", tmpx));
    printf("branch: %s\n", out_line(i, ".neogit/comat/branch", tmpx));
    printf("count %s\n", out_line(i, ".neogit/comat/count", tmpx));
}

int run_log(int argc, char *const argv[])
{
    FILE *ptr;
    if (argc == 4)
    {
        int y;
        int x;
        char tmpx[1024];
        ptr = fopen(".neogit/commit", "r");
        fgets(tmpx, sizeof(tmpx), ptr);
        fclose(ptr);
        x = atoi(tmpx);
        if (strcmp(argv[2], "-n") == 0)
        {
            y = atoi(argv[3]);
            for (int i = x - 1; i > x - y - 1; i--)
            {
                show_log(i, tmpx);
            }
        }
        else if (strcmp(argv[2], "-branch") == 0)
        {
            for (int i = x - 1; i > -1; i--)
            {
                if (strcmp(out_line(i, ".neogit/comat/branch", tmpx), argv[3]) == 0)
                {
                    show_log(i, tmpx);
                }
            }
        }
        else if (strcmp(argv[2], "-author") == 0)
        {
            for (int i = x - 1; i > -1; i--)
            {
                if (strcmp(out_line(i, ".neogit/comat/name", tmpx), argv[3]) == 0)
                {
                    show_log(i, tmpx);
                }
            }
        }
        else if (strcmp(argv[2], "-search") == 0)
        {
            for (int i = x - 1; i > -1; i--)
            {
                if (strstr(out_line(i, ".neogit/comat/message", tmpx), argv[3]) != NULL)
                {
                    show_log(i, tmpx);
                }
            }
        }
        else if (strcmp(argv[2], "-since") == 0)
        {
            for (int i = x - 1; i > -1; i--)
            {
                out_line(i, ".neogit/comat/time", tmpx);
                double differ = calculateTimeDifference(tmpx, argv[3]);
                if (!(differ > 0.0))
                {
                    show_log(i, tmpx);
                }
            }
        }
        else if (strcmp(argv[2], "-before") == 0)
        {
            for (int i = x - 1; i > -1; i--)
            {
                out_line(i, ".neogit/comat/time", tmpx);
                double differ = calculateTimeDifference(tmpx, argv[3]);
                if (!(differ < 0.0))
                {
                    show_log(i, tmpx);
                }
            }
        }
    }
    else if (argc == 2)
    {
        int y;
        int x;
        char tmpx[1024];
        ptr = fopen(".neogit/commit", "r");
        fgets(tmpx, sizeof(tmpx), ptr);
        fclose(ptr);
        x = atoi(tmpx);
        y = x;
        for (int i = x - 1; i > x - y - 1; i--)
        {
            show_log(i, tmpx);
        }
    }
    else if (strcmp(argv[2], "-search") == 0)
    {
        int x;
        char tmpx[1024];
        ptr = fopen(".neogit/commit", "r");
        fgets(tmpx, sizeof(tmpx), ptr);
        fclose(ptr);
        x = atoi(tmpx);
        for (int i = x - 1; i > -1; i--)
        {
            for (int j = 3; j < argc; j++)
            {

                if (strstr(out_line(i, ".neogit/comat/message", tmpx), argv[j]) != NULL)
                {
                    show_log(i, tmpx);
                }
            }
        }
    }
    else
    {
        printf("invalid command");
        return 1;
    }
}

int run_branch(int argc, char *const argv[])
{
    char tmp[1024];
    FILE *ptr;
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    if (argc == 2)
    {
        ptr = fopen(".neogit/branches", "r");
        while (fgets(tmp, sizeof(tmp), ptr) != NULL)
        {
            printf("%s\n", tmp);
        }
        fclose(ptr);
    }
    else if (argc == 3)
    {
        ptr = fopen(".neogit/branches", "a");
        fprintf(ptr, "%s", argv[2]);
        fclose(ptr);
    }
    else
    {
        printf("invalid command");
    }
    chdir(cwd);
}

char *print_status(int x, int y, char *out)
{
    if (x)
        strcat(out, "+");
    else
        strcat(out, "-");
    if (y == 0)
        strcat(out, "D");
    else if (y == 1)
        strcat(out, "M");
    else
        strcat(out, "A");
    return (out);
}

int run_status(int argc, char *const argv[])
{
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    char tmpcwd[1024];
    strcpy(tmpcwd, cwd);
    find_name(".neogit");
    getcwd(cwd, sizeof(cwd));
    if (argc == 2)
    {
        char tmp[1024];
        char tmp2[1024];
        int y = last_commit();
        char add[1024] = ".neogit\\commits\\commit";
        char tmptmp[1024];
        sprintf(tmptmp, "%d\\", y);
        strcat(add, tmptmp);
        char tmp3[1024];
        FILE *ptr = fopen(".neogit\\status", "w");
        fclose(ptr);
        stat_tree(".", ".neogit\\status");
        ptr = fopen(".neogit\\status", "r");
        char tmp4[1024];
        while (fgets(tmp, sizeof(tmp), ptr) != NULL)
        {
            char *ggh = strrchr(tmp, '\n');
            *ggh = '\0';
            int x = 0, y = 0;
            strcpy(tmp4, ".neogit\\stage_area\\");
            strcpy(tmp3, add);
            strcpy(tmp2, cwd);
            strcat(tmp2, "\\");
            char *gh = strrchr(tmp, '.');
            strcat(tmp2, gh + 2);
            change_word(tmp2, '\\', '$');
            char *tmpgh = tmp2;
            if (*tmpgh == 'C' && *(tmpgh + 1) == ':')
                tmpgh += 2;
            strcat(tmp3, tmpgh);
            strcat(tmp4, tmpgh);
            if (is_file(tmp) == 1)
            {
                if (is_file(tmp4) == 1) // is it staged?
                {
                    x = 1;
                }
                if (is_file(tmp3) == 1) // is it commited?
                {
                    if (!compareFiles(tmp, tmp3))
                    {
                        y = 1; // modified
                    }
                    else
                        y = -1;
                }
                else
                {
                    y = 2; // added
                }
                char out[2] = "\0\0";
                if (y != -1)
                    printf("File %s is %s\n", tmp, print_status(x, y, out));
            }
        }
        fclose(ptr);
        ptr = fopen(".neogit\\status", "w");
        fclose(ptr);
        char *tmptmp2 = strrchr(add, '\\');
        *tmptmp2 = '\0';
        stat_tree(add, ".neogit\\status");
        ptr = fopen(".neogit\\status", "r");
        while (fgets(tmp, sizeof(tmp), ptr) != NULL)
        {
            int x = 0, y = 0;
            strcpy(tmp2, tmp);
            strcpy(tmp3, "C:");
            strcpy(tmp4, "");
            strcat(tmp4, tmp2);
            strcat(tmp3, tmp2);
            char *tmp5 = strrchr(tmp3, '\\');
            char tmp6[1024];
            strcpy(tmp6, "C:");
            strcat(tmp6, tmp5 + 1);
            change_word(tmp6, '$', '\\');
            strcpy(tmp4, ".neogit\\stage_area");
            strcat(tmp4, tmp5);
            y = -1;
            char *tmp7 = strrchr(tmp6, '\n');
            *tmp7 = '\0';
            if (is_file(tmp6) != 1) // deleted?
            {
                y = 0;
            }
            printf("this is tmp4!%s\n", tmp4);
            if (is_file(tmp4) == 1) // staged?
            {
                x = 1;
            }
            char out[2] = "\0\0";
            if (y != -1)
            {
                char *ggh = strrchr(tmp, '\n');
                *ggh = '\0';
                char verytmp[1024] = "commit";
                char verytmp2[1024];
                sprintf(verytmp2, "%d", last_commit());
                strcat(verytmp, verytmp2);
                char *ghghgh = strstr(tmp, verytmp);
                while (*ghghgh != '\\')
                {
                    ghghgh++;
                }
                char forprint[1024] = "C:";
                strcat(forprint, ghghgh + 1);
                change_word(forprint, '$', '\\');
                printf("File %s is %s\n", forprint, print_status(x, y, out));
            }
        }
        fclose(ptr);
    }
    else
    {
        printf("invalid command!");
    }
    chdir(cwd);
}

bool check_file_directory_exists(char *filepath)
{
    DIR *dir = opendir(".neogit/files");
    struct dirent *entry;
    if (dir == NULL)
    {
        perror("Error opening current directory");
        return 1;
    }
    // while ((entry = readdir(dir)) != NULL) {
    //     if (entry->d_type == DT_DIR && strcmp(entry->d_name, filepath) == 0) return true;
    // }
    closedir(dir);

    return false;
}

int commit_staged_file(int commit_ID, char *filepath)
{
    FILE *read_file, *write_file;
    char read_path[MAX_FILENAME_LENGTH];
    strcpy(read_path, filepath);
    char write_path[MAX_FILENAME_LENGTH];
    strcpy(write_path, ".neogit/files/");
    strcat(write_path, filepath);
    strcat(write_path, "/");
    char tmp[10];
    sprintf(tmp, "%d", commit_ID);
    strcat(write_path, tmp);

    read_file = fopen(read_path, "r");
    if (read_file == NULL)
        return 1;

    write_file = fopen(write_path, "w");
    if (write_file == NULL)
        return 1;

    char buffer;
    buffer = fgetc(read_file);
    while (buffer != EOF)
    {
        fputc(buffer, write_file);
        buffer = fgetc(read_file);
    }
    fclose(read_file);
    fclose(write_file);

    return 0;
}

int track_file(char *filepath)
{
    if (is_tracked(filepath))
        return 0;

    FILE *file = fopen(".neogit/tracks", "a");
    if (file == NULL)
        return 1;
    fprintf(file, "%s\n", filepath);
    return 0;
}

bool is_tracked(char *filepath)
{
    FILE *file = fopen(".neogit/tracks", "r");
    if (file == NULL)
        return false;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file) != NULL)
    {
        int length = strlen(line);

        // remove '\n'
        if (length > 0 && line[length - 1] == '\n')
        {
            line[length - 1] = '\0';
        }

        if (strcmp(line, filepath) == 0)
            return true;
    }
    fclose(file);

    return false;
}

int create_commit_file(int commit_ID, char *message)
{
    char commit_filepath[MAX_FILENAME_LENGTH];
    strcpy(commit_filepath, ".neogit/commits/");
    char tmp[10];
    sprintf(tmp, "%d", commit_ID);
    strcat(commit_filepath, tmp);

    FILE *file = fopen(commit_filepath, "w");
    if (file == NULL)
        return 1;

    fprintf(file, "message: %s\n", message);
    fprintf(file, "files:\n");

    DIR *dir = opendir(".");
    struct dirent *entry;
    if (dir == NULL)
    {
        perror("Error opening current directory");
        return 1;
    }
    // while ((entry = readdir(dir)) != NULL) {
    //     if (entry->d_type == DT_REG && is_tracked(entry->d_name)) {
    //         int file_last_commit_ID = find_file_last_commit(entry->d_name);
    //         fprintf(file, "%s %d\n", entry->d_name, file_last_commit_ID);
    //     }
    // }
    closedir(dir);
    fclose(file);
    return 0;
}

int find_file_last_commit(char *filepath)
{
    char filepath_dir[MAX_FILENAME_LENGTH];
    strcpy(filepath_dir, ".neogit/files/");
    strcat(filepath_dir, filepath);

    int max = -1;

    DIR *dir = opendir(filepath_dir);
    struct dirent *entry;
    if (dir == NULL)
        return 1;

    // while((entry = readdir(dir)) != NULL) {
    //     if (entry->d_type == DT_REG) {
    //         int tmp = atoi(entry->d_name);
    //         max = max > tmp ? max: tmp;
    //     }
    // }
    closedir(dir);

    return max;
}

int run_checkout(int argc, char *const argv[])
{
    if (argc < 3)
        return 1;

    int commit_ID = atoi(argv[2]);

    DIR *dir = opendir(".");
    struct dirent *entry;
    // while((entry = readdir(dir)) != NULL) {
    //     if (entry->d_type == DT_REG && is_tracked(entry->d_name)) {
    //         checkout_file(entry->d_name, find_file_last_change_before_commit(entry->d_name, commit_ID));
    //     }
    // }
    closedir(dir);

    return 0;
}
int run_config(int argc, char *argv[])
{
    char cwd[1000];
    getcwd(cwd, sizeof(cwd));
    int global = 0;
    int halat = -1;
    char entry[1000];
    char username[1000] = " ";
    char email[1000] = " ";
    if (strcmp(argv[2], "-global") == 0)
    {
        global = 1;
    }

    if (strcmp(argv[global + 2], "user.name") == 0)
    {
        halat = 0;
        strcpy(username, argv[global + 3]);
    }
    else if (strcmp(argv[global + 2], "user.email") == 0)
    {
        halat = 1;
        strcpy(email, argv[global + 3]);
    }
    else if (strncmp(argv[global + 2], "alias", 5) == 0)
    {
        halat = 2;
        strcpy(argv[global + 2], argv[global + 2] + 6);

        strcpy(entry, argv[global + 3]);
        for (int i = global + 4; i < argc; i++)
        {
            strcat(entry, " ");
            strcat(entry, argv[i]);
        }
    }

    if (global)
    {
        char add[1000] = "C:\\Users\\asus\\Desktop\\Globals\\global.txt";
        FILE *ptr = fopen(add, "r");
        char line[1000];
        while (fgets(line, sizeof(line), ptr) != NULL)
        {
            line[strlen(line) - 1] = '\0';
            chdir(line);
            find_name(".neogit");
            if (halat != 2)
                create_configs(username, email, halat);
            else
            {
                add_to_file(argv[global + 2], ".neogit/alias");
                add_to_file(entry, ".neogit/alias");
            }
            chdir(cwd);
        }
        fclose(ptr);
    }
    else if (!global)
    {
        find_name(".neogit");
        if (halat != 2)
            create_configs(username, email, halat);
        else
        {
            add_to_file(argv[global + 2], ".neogit/alias");
            add_to_file(entry, ".neogit/alias");
        }
        chdir(cwd);
    }
}
int find_file_last_change_before_commit(char *filepath, int commit_ID)
{
    char filepath_dir[MAX_FILENAME_LENGTH];
    strcpy(filepath_dir, ".neogit/files/");
    strcat(filepath_dir, filepath);

    int max = -1;

    DIR *dir = opendir(filepath_dir);
    struct dirent *entry;
    if (dir == NULL)
        return 1;

    // while((entry = readdir(dir)) != NULL) {
    //     if (entry->d_type == DT_REG) {
    //         int tmp = atoi(entry->d_name);
    //         if (tmp > max && tmp <= commit_ID) {
    //             max = tmp;
    //         }
    //     }
    // }
    closedir(dir);

    return max;
}

int checkout_file(char *filepath, int commit_ID)
{
    char src_file[MAX_FILENAME_LENGTH];
    strcpy(src_file, ".neogit/files/");
    strcat(src_file, filepath);
    char tmp[10];
    sprintf(tmp, "/%d", commit_ID);
    strcat(src_file, tmp);

    FILE *read_file = fopen(src_file, "r");
    if (read_file == NULL)
        return 1;
    FILE *write_file = fopen(filepath, "w");
    if (write_file == NULL)
        return 1;

    char line[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), read_file) != NULL)
    {
        fprintf(write_file, "%s", line);
    }

    fclose(read_file);
    fclose(write_file);

    return 0;
}
int run_alias(int argc, char *argv[])
{
    find_name(".neogit");
    FILE *ptr = fopen(".neogit/alias", "r");
    char line[1000];
    char entry[1000];
    int i = 0;
    while (fgets(line, sizeof(line), ptr) != NULL)
    {
        line[strlen(line) - 1] = '\0';
        if (i % 2 == 0)
        {
            if (strcmp(line, argv[1]) == 0)
            {
                strcpy(entry, out_line(i + 1, ".neogit/alias", entry));
                if (entry[0] == '"')
                {
                    strcmp(entry, entry + 1);
                    entry[strlen(entry) - 1];
                }
                system(entry);
                return 0;
            }
        }
        i++;
    }
    printf("invalid command");
    return 1;
}
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stdout, "please enter a valid command");
        return 1;
    }
    if (strcmp(argv[1], "init") == 0)
    {
        return run_init(argc, argv);
    }
    else if (strcmp(argv[1], "add") == 0 || strcmp(argv[1], "reset") == 0)
    {
        return run_add(argc, argv);
    }
    else if (strcmp(argv[1], "commit") == 0 && strcmp(argv[2], "-m") == 0)
    {
        return run_commit(argc, argv);
    }
    else if (strcmp(argv[1], "checkout") == 0)
    {
        return run_checkout(argc, argv);
    }
    else if (strcmp(argv[1], "config") == 0)
    {
        return run_config(argc, argv);
    }
    else if (strcmp(argv[1], "log") == 0)
    {
        return run_log(argc, argv);
    }
    else if (strcmp(argv[1], "branch") == 0)
    {
        return run_branch(argc, argv);
    }
    else if (strcmp(argv[1], "status") == 0)
    {
        return run_status(argc, argv);
    }
    else if (argc == 2)
    {
        return run_alias(argc, argv);
    }
    else
    {
        fprintf(stdout, "please enter a valid command");
        return 1;
    }
    return 0;
}
