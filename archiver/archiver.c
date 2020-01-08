/*
 * Простейший говноархиватор
 * 
 */

#include "archiver.h"

 /*
  * create_arch() - создание архива
  * @arch_name:  Имя архива
  * 
  * Создаем архив и возвращаем дескриптор
  *
  * Возвращает:
  * -1: Не был создан архив
  *  Значение файлового указателя на архив
  */
FILE * create_arch(char* arch_name) {
    FILE* arch_fp;
    if ((arch_fp = fopen(arch_name, "r")) != NULL)
    {
        fclose(arch_fp);
        int c;
        printf("Файл существует, перезаписать? [y/n]: ");
        c = toupper(getchar());
        int answ = c;
        while (c != '\n' && c != EOF) {
            c = getchar();
            if (answ != 'Y') {
                errno = EEXIST;
                perror("Ошибка создания архива");
                exit(EXIT_FAILURE);
            }
        }
        
    }
    if ((arch_fp = fopen(arch_name, "wb")) != NULL) {
        fclose(arch_fp);
    }
    else {
        perror("Ошибка открытия архива для записи");
        exit(EXIT_FAILURE);
    }
    return arch_fp;
}

/*
 * end_of_arch() - конец работы с архивом
 * @arch_fp:    Файловый указатель на архив
 * @arch_name:  Имя архива
 *
 * Записываем конечный нулевой header и закрываем архив
 *
 * Возвращает:
 * -1: Ошибка записи
 * -2: Не удалось открыть архив для дозаписи
 *  0: Успех
 */
int end_of_arch(FILE* arch_fp, char* arch_name) {
    struct meta_data header = { 0 };
    if ((arch_fp = fopen(arch_name, "ab")) != NULL) {
        if (fwrite(&header, sizeof(char), sizeof(struct meta_data), arch_fp) != sizeof(struct meta_data)) {
            perror("Ошибка записи метаданных");
            return -1;
        }
    }
    else {
        perror("Не удалось открыть архив для дозаписи");
        return -2;
    }
    fclose(arch_fp);
    return 0;
}

/*
 * write_file_to_arch() - запись файла в архив
 * @arch_fp:            Файловый указатель на архив
 * @arch_name:          Имя архива
 * @file:               Имя файла
 *
 * Читаем метаданные файла (имя, размер, тип файла) и сохраняем в header
 * записываем header в архив, копируем содержимое файла в архив (header.size бит)
 *
 * Возвращает:
 * -1: Ошибка записи
 * -2: Ошибка чтения файла
 * -3: Ошибка копирования файла в архив
 *  0: Успех
 */
int write_file_to_arch(FILE* arch_fp, char* arch_name, char* file) {
    struct meta_data header;
    size_t n_obj;
    unsigned long long out_obj = 0;
    FILE* fp;
    char* buf = (char*)malloc(sizeof(size_t) * BUF_SIZE);
    if (buf == NULL) exit(1);    // если выделение памяти не выполнилось, завершить программу

    /* формируем хэдер */
    strcpy(header.name, file);
    if ((fp = fopen(file, "rb")) != NULL) {
        header.size = filesize(fp);
        printf("%s%ld%c","size: ", header.size, '\n');
        fseek(fp, 0, SEEK_SET);
    }
    else {
        perror("Ошибка открытия файла");
        exit(EXIT_FAILURE);
    }
    
    /* записываем хэдер */
    if ((arch_fp = fopen(arch_name, "ab")) != NULL) {
        if (fwrite(&header, sizeof(char), sizeof(struct meta_data), arch_fp) != sizeof(struct meta_data)) {
            perror("Ошибка записи метаданных");
            return -1;
        }
    }
    else {
        perror("Ошибка открытия архива для дозаписи");
        exit(EXIT_FAILURE);
    }
    while ((n_obj = fread(buf, sizeof(char), BUF_SIZE, fp)) != 0) /* читаем до конца файла */
    {
        /*здесь производится обработка считанной информации
        n_obj – количество действительно прочитанных единиц информации*/
        out_obj += fwrite(buf, sizeof(char), n_obj, arch_fp);
    }
    if (header.size == out_obj) {
        printf("Файл успешно записан в архив\n");
    }
    else {
        printf("Запись файла не удалась\n");
    }
    free(buf);
    fclose(arch_fp);
    fclose(fp);
    return 0;
}


/*
 * extract_from_arch() - обработка файла для извлечения
 * @arch_name:          Имя архива
 *
 * Проверяем на существование архива, открываем его
 * последовательно читаем метаданные (header),
 * передаем имя файла и header в  extract_file
 *
 * Возвращает:
 * -1: Ошибка существования файла
 *  0: Успех
 */
int extract_from_arch(char* arch_name) {
    FILE* arch_fp;
    struct meta_data header;

    /* проверка на существование архива */
    if ((arch_fp = fopen(arch_name, "rb")) != NULL) {
        size_t len = fread(&header, sizeof(char), sizeof(struct meta_data), arch_fp);
        /*
         * обрабатываем файлы
         */
        while (len > 0) {
            /* проверка на конец архива (конечный header) */
            if (header.size == 0 && strlen(header.name) == 0) break;
            printf("Извлечение: %s\n", header.name);
            extract_file(arch_fp, header);

            len = fread(&header, sizeof(char), sizeof(struct meta_data), arch_fp);
        }
    }
    else {
        perror("Ошибка существования файла");
        return -1;
    }
    fclose(arch_fp);
    return 0;
}

/**
 * extract_file() - извлечение файла (не директории)
 * @arch_fp:    файловый указатель на архив
 * @header:     метаданные файла
 *
 * Проверяем существование файла, спрашиваем о перезаписи
 * создаем файл, копируем в него данные из архива, согласно header
 * (в данном случае size байт данных)
 *
 * Возвращает:
 * -1: Ошибка чтения архива
 * -2: Не удалось успешно извлечь файл
 * -3: Не удалось открыть файл для записи
 *  0: Успех
 */
int extract_file(FILE* arch_fp, struct meta_data header) {
    char* name = header.name;
    unsigned long long size = header.size;
    FILE* fp;
    size_t n_obj;
    unsigned long long out_obj = 0;
    long offset = 0;
    char* buf = (char*)malloc(sizeof(size_t) * BUF_SIZE);
    /* если файл существует */
    
    if ((fp = fopen(name, "r")) != NULL)
    {
        fclose(fp);
        int c;
        printf("Файл существует, перезаписать? [y/n]: ");
        c = toupper(getchar());
        int answ = c;
        while (c != '\n' && c != EOF) c = getchar();
        if (answ != 'Y') {
            if (fseek(arch_fp, size, SEEK_CUR) == -1) {
                perror("Ошибка чтения архива");
                /*аварийное прерывание тк,
                 *последущая разархивация невозможна
                 */
                abort();
            }
            return -1;
        }
    }
    /* создаем или перезаписываем файл */
    if ((fp = fopen(name, "wb")) != NULL)
    {
        while (((n_obj = fread(buf, sizeof(char), BUF_SIZE, arch_fp)) != 0)) /* читаем до конца файла */
        {
            if ((out_obj + n_obj) > size) {
                offset = (out_obj + n_obj) - size;
                out_obj += fwrite(buf, sizeof(char), size - out_obj, fp);
                
                printf("%li%c", offset, '\n');
                fseek(arch_fp, -offset, SEEK_CUR);
                break;
            }
            /*здесь производится обработка считанной информации
            n_obj – количество действительно прочитанных единиц информации*/
            out_obj += fwrite(buf, sizeof(char), n_obj, fp);
        }
        if (size == out_obj) {
            printf("Файл успешно извлечен из архива\n");
        }
        else {
            printf("Не удалось успешно извлечь файл\n");
            return -2;
        }
    }
    else {
        printf("Не удалось открыть файл ", name, " для записи\n");
        return -3;
    }
    free(buf);
    fclose(fp);
    return 0;
}

/*Определяем размер дерьма в бенарнике*/
long int filesize(FILE* fp)
{
    long int save_pos, size_of_file;

    save_pos = ftell(fp);
    fseek(fp, 0L, SEEK_END);
    size_of_file = ftell(fp);
    fseek(fp, save_pos, SEEK_SET);
    return size_of_file;
}