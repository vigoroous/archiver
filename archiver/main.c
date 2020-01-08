#include "archiver.h"

int main(int argc, char** argv)
{
    setlocale(LC_ALL, "RU");
    char* arch = NULL;
    /*
     * arch_fp – файловый указатель на архив.
     *  argv[1]        - для ввода говнокоманды
     *  arch = argv[2] - имя архива
     *  argv[3]        - файл1
     *  argv[4]        - файл2 
     *  .... 
     */
    if (argc <= 2) {
        printf("Использование:\n");
        printf("./arch -e {имя_архива} "
            "извлечение из архива\n");
        printf("./arch -c {имя_архива} {файл1} {файл2} {файл3}"
            "... создание архива\n");
        return 0;
    }
    if (!strcmp("-e", argv[1])) {
        printf("Извлечение из архива : %s\n", argv[2]);
        arch = argv[2];
        extract_from_arch(arch);
    }
    else if (!strcmp("-c", argv[1])) {
        printf("Добавление в архив: %s\n", argv[2]);
        arch = argv[2];
        if (argc == 3) {
            printf("Требуется выбрать файлы для "
                "добавления в архив\n");
            return 1;
        }
        FILE* arch_fp = create_arch(arch);
        for (int num = argc; num > 3; num--) {
            printf("Запись файла: %s\n", argv[num-1]);
            write_file_to_arch(arch_fp, arch, argv[num-1]);
        }
        end_of_arch(arch_fp, arch);
    } 
    else {
        printf("Неизвестная команда: %s\n", argv[1]);
    }
    return 0;
}