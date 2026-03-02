#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>

#ifdef _WIN32
    #include <direct.h>
    #define MKDIR(path) _mkdir(path)
    #define SEP '\\'
#else
    #include <unistd.h>
    #define MKDIR(path) mkdir(path, 0777)
    #define SEP '/'
#endif

#define MAX_PATH 4096

// --- LOCALIZATION ---

typedef struct {
    char *help;
    char *init_success;
    char *ready_to_sink;
    char *sinking_to;
    char *all_sunk;
    char *added_ignore;
    char *err_no_targets;
    char *err_path_req;
    char *err_source;
    char *err_dest;
    char *lang_set;
} Locale;

Locale en = {
    "\n⚓ SINK CLI - Stop syncing, start sinking.\n\nUSAGE:\n  sink init <path>      - Link project to a target\n  sink push             - Sink all files\n  sink add-ignore <val> - Keep a path afloat (ignore)\n  sink lang <en|ru|ua>  - Change language\n",
    "⚓ Anchor dropped! Linked to: %s\n",
    "💡 Ready to sink. Use 'sink push' when ready.\n",
    "🚀 Sinking files to: %s\n",
    "\n✅ Mission accomplished. Everything is SUNK.\n",
    "🙈 Added '%s' to ignore list. It stays afloat.\n",
    "❌ Error: No targets. Run 'sink init' first.\n",
    "❌ Error: Path required.\n",
    "  ❌ [ERR] Source: %s (%s)\n",
    "  ❌ [ERR] Dest: %s (%s)\n",
    "Language set to English.\n"
};

Locale ru = {
    "\n⚓ SINK CLI - Хватит синхронизировать, пора сливать.\n\nКОМАНДЫ:\n  sink init <путь>      - Привязать проект к цели\n  sink push             - Слить (sink) все файлы\n  sink add-ignore <имя> - Оставить путь на плаву (игнор)\n  sink lang <en|ru|ua>  - Сменить язык\n",
    "⚓ Якорь брошен! Привязано к: %s\n",
    "💡 Все готово. Используй 'sink push', чтобы слить файлы.\n",
    "🚀 Сливаем файлы в: %s\n",
    "\n✅ Миссия окончена. Все потоплено (SUNK).\n",
    "🙈 '%s' теперь в игноре. Он остается на плаву.\n",
    "❌ Ошибка: Цели не найдены. Выполни 'sink init'.\n",
    "❌ Ошибка: Укажи путь.\n",
    "  ❌ [ОШИБКА] Исходник: %s (%s)\n",
    "  ❌ [ОШИБКА] Цель: %s (%s)\n",
    "Язык установлен: Русский.\n"
};

Locale ua = {
    "\n⚓ SINK CLI - Досить синхронізувати, час зливати.\n\nКОМАНДИ:\n  sink init <шлях>      - Прив'язати проект до цілі\n  sink push             - Злити (sink) всі файли\n  sink add-ignore <ім'я> - Залишити шлях на плаву (ігнор)\n  sink lang <en|ru|ua>  - Змінити мову\n",
    "⚓ Якір кинуто! Прив'язано до: %s\n",
    "💡 Все готово. Використовуй 'sink push', щоб злити файли.\n",
    "🚀 Зливаємо файли у: %s\n",
    "\n✅ Місію завершено. Все потоплено (SUNK).\n",
    "🙈 '%s' тепер в ігнорі. Він залишається на плаву.\n",
    "❌ Помилка: Цілей не знайдено. Виконай 'sink init'.\n",
    "❌ Помилка: Вкажи шлях.\n",
    "  ❌ [ПОМИЛКА] Джерело: %s (%s)\n",
    "  ❌ [ПОМИЛКА] Ціль: %s (%s)\n",
    "Мову змінено на Українську.\n"
};

Locale *cur = &en;

// --- UTILITIES ---

void set_lang(const char *l) {
    if (strcmp(l, "ru") == 0) cur = &ru;
    else if (strcmp(l, "ua") == 0) cur = &ua;
    else cur = &en;
}

void load_lang_config() {
    FILE *f = fopen(".sinklang", "r");
    if (f) {
        char l[5];
        if (fscanf(f, "%4s", l) == 1) set_lang(l);
        fclose(f);
    } else {
        char *sys_lang = getenv("LANG");
        if (sys_lang) {
            if (strstr(sys_lang, "ru")) set_lang("ru");
            else if (strstr(sys_lang, "uk") || strstr(sys_lang, "ua")) set_lang("ua");
        }
    }
}

void recursive_mkdir(const char *path) {
    char tmp[MAX_PATH];
    char *p = NULL;
    snprintf(tmp, sizeof(tmp), "%s", path);
    size_t len = strlen(tmp);
    if (len == 0) return;
    if (tmp[len - 1] == SEP) tmp[len - 1] = 0;
    for (p = tmp + 1; *p; p++) {
        if (*p == SEP) {
            *p = 0;
            if (strlen(tmp) > 0) {
                struct stat st = {0};
                if (stat(tmp, &st) == -1) MKDIR(tmp);
            }
            *p = SEP;
        }
    }
    MKDIR(tmp);
}

int copy_file(const char *from, const char *dest) {
    FILE *src = fopen(from, "rb");
    if (!src) { printf(cur->err_source, from, strerror(errno)); return 1; }
    FILE *dst = fopen(dest, "wb");
    if (!dst) { printf(cur->err_dest, dest, strerror(errno)); fclose(src); return 1; }
    char buffer[65536];
    size_t n;
    while ((n = fread(buffer, 1, sizeof(buffer), src)) > 0) fwrite(buffer, 1, n, dst);
    fflush(dst); fclose(src); fclose(dst);
    return 0;
}

char **read_lines(const char *filepath, int *line_count) {
    FILE *f = fopen(filepath, "r");
    if (!f) { *line_count = 0; return NULL; }
    char buffer[MAX_PATH], **lines = NULL;
    int count = 0;
    while (fgets(buffer, sizeof(buffer), f)) {
        buffer[strcspn(buffer, "\r\n")] = 0;
        if (strlen(buffer) == 0) continue;
        lines = realloc(lines, (count + 1) * sizeof(char*));
        lines[count++] = strdup(buffer);
    }
    fclose(f); *line_count = count;
    return lines;
}

// ПРОВЕРКА ИГНОРА ПО ПУТИ
int is_ignored(const char *rel_path, char **ignore_list, int ignore_count) {
    if (strcmp(rel_path, ".") == 0 || strcmp(rel_path, "..") == 0) return 1;
    // Базовые системные исключения
    if (strstr(rel_path, ".sinksettings") || strstr(rel_path, ".sinkignore") ||
        strstr(rel_path, ".sinklang") || strstr(rel_path, ".git") || 
        strstr(rel_path, ".DS_Store")) return 1;

    for (int i = 0; i < ignore_count; i++) {
        if (strcmp(rel_path, ignore_list[i]) == 0) return 1;
    }
    return 0;
}

void sink_directory(const char *src_dir, const char *dst_dir, const char *current_rel_path, char **ignore_list, int ignore_count) {
    DIR *dir = opendir(src_dir);
    if (!dir) return;

    recursive_mkdir(dst_dir);
    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        // Формируем относительный путь для проверки игнора
        char rel_path[MAX_PATH];
        if (strlen(current_rel_path) == 0) snprintf(rel_path, sizeof(rel_path), "%s", entry->d_name);
        else snprintf(rel_path, sizeof(rel_path), "%s/%s", current_rel_path, entry->d_name);

        if (is_ignored(rel_path, ignore_list, ignore_count)) continue;

        char src_full[MAX_PATH], dst_full[MAX_PATH];
        snprintf(src_full, sizeof(src_full), "%s/%s", src_dir, entry->d_name);
        snprintf(dst_full, sizeof(dst_full), "%s/%s", dst_dir, entry->d_name);

        struct stat st;
        if (lstat(src_full, &st) == 0) {
            if (S_ISDIR(st.st_mode)) {
                sink_directory(src_full, dst_full, rel_path, ignore_list, ignore_count);
            } else if (S_ISREG(st.st_mode)) {
                if (copy_file(src_full, dst_full) == 0) {
                    printf("  ⚓ [SUNK] %s\n", rel_path);
                }
            }
        }
    }
    closedir(dir);
}

// --- MAIN ---

int main(int argc, char const *argv[]) {
    load_lang_config();
    if (argc < 2) { printf("%s", cur->help); return 0; }

    if (strcmp(argv[1], "init") == 0) {
        if (argc < 3) { printf("%s", cur->err_path_req); return 1; }
        FILE *f = fopen(".sinksettings", "w");
        if (f) { fprintf(f, "%s\n", argv[2]); fclose(f); }
        FILE *i = fopen(".sinkignore", "w");
        if (i) { fprintf(i, ".sinkignore\n.sinksettings\n.git\nsink\n.DS_Store\n.sinklang\ncompiled\n"); fclose(i); }
        printf(cur->init_success, argv[2]);
        printf("%s", cur->ready_to_sink);
    } 
    else if (strcmp(argv[1], "push") == 0) {
        int t_count = 0, i_count = 0;
        char **targets = read_lines(".sinksettings", &t_count);
        char **ignores = read_lines(".sinkignore", &i_count);
        if (!targets) { printf("%s", cur->err_no_targets); return 1; }
        for (int i = 0; i < t_count; i++) {
            printf(cur->sinking_to, targets[i]);
            sink_directory(".", targets[i], "", ignores, i_count);
            free(targets[i]);
        }
        free(targets);
        if (ignores) { for (int i = 0; i < i_count; i++) free(ignores[i]); free(ignores); }
        printf("%s", cur->all_sunk);
    }
    else if (strcmp(argv[1], "add-ignore") == 0) {
        if (argc < 3) return 1;
        FILE *f = fopen(".sinkignore", "a");
        if (f) { fprintf(f, "%s\n", argv[2]); fclose(f); }
        printf(cur->added_ignore, argv[2]);
    }
    else if (strcmp(argv[1], "lang") == 0) {
        if (argc < 3) return 1;
        set_lang(argv[2]);
        FILE *f = fopen(".sinklang", "w");
        if (f) { fprintf(f, "%s", argv[2]); fclose(f); }
        printf("%s", cur->lang_set);
    } else { printf("%s", cur->help); }
    return 0;
}