/*
 * ���������� ��������������
 * 
 */

#include "archiver.h"

 /*
  * create_arch() - �������� ������
  * @arch_name:  ��� ������
  *
  * ������� ����� � ���������� ����������
  *
  * ����������:
  * -1: �� ��� ������ �����
  *  �������� ��������� ��������� �� �����
  */
FILE * create_arch(char* arch_name) {
    FILE* arch_fp;
    if ((arch_fp = fopen(arch_name, "r")) != NULL)
    {
        fclose(arch_fp);
        int c;
        printf("���� ����������, ������������? [y/n]: ");
        c = toupper(getchar());
        int answ = c;
        while (c != '\n' && c != EOF) {
            c = getchar();
            if (answ != 'Y') {
                errno = EEXIST;
                perror("������ �������� ������");
                exit(EXIT_FAILURE);
            }
        }
        
    }
    if ((arch_fp = fopen(arch_name, "wb")) != NULL) {
        fclose(arch_fp);
    }
    else {
        perror("������ �������� ������ ��� ������");
        exit(EXIT_FAILURE);
    }
    return arch_fp;
}

/*
 * end_of_arch() - ����� ������ � �������
 * @arch_fp:    �������� ��������� �� �����
 * @arch_name:  ��� ������
 *
 * ���������� �������� ������� header � ��������� �����
 *
 * ����������:
 * -1: ������ ������
 * -2: �� ������� ������� ����� ��� ��������
 *  0: �����
 */
int end_of_arch(FILE* arch_fp, char* arch_name) {
    struct meta_data header = { 0 };
    if ((arch_fp = fopen(arch_name, "ab")) != NULL) {
        if (fwrite(&header, sizeof(char), sizeof(struct meta_data), arch_fp) != sizeof(struct meta_data)) {
            perror("������ ������ ����������");
            return -1;
        }
    }
    else {
        perror("�� ������� ������� ����� ��� ��������");
        return -2;
    }
    fclose(arch_fp);
    return 0;
}

/*
 * write_file_to_arch() - ������ ����� � �����
 * @arch_fp:            �������� ��������� �� �����
 * @arch_name:          ��� ������
 * @file:               ��� �����
 *
 * ������ ���������� ����� (���, ������, ��� �����) � ��������� � header
 * ���������� header � �����, �������� ���������� ����� � ����� (header.size ���)
 *
 * ����������:
 * -1: ������ ������
 * -2: ������ ������ �����
 * -3: ������ ����������� ����� � �����
 *  0: �����
 */
int write_file_to_arch(FILE* arch_fp, char* arch_name, char* file) {
    struct meta_data header;
    size_t n_obj;
    unsigned long long out_obj = 0;
    FILE* fp;
    char* buf = (char*)malloc(sizeof(size_t) * BUF_SIZE);
    if (buf == NULL) exit(1);    // ���� ��������� ������ �� �����������, ��������� ���������

    /* ��������� ����� */
    strcpy(header.name, file);
    if ((fp = fopen(file, "rb")) != NULL) {
        header.size = filesize(fp);
        printf("%s%ld%c","size: ", header.size, '\n');
        fseek(fp, 0, SEEK_SET);
    }
    else {
        perror("������ �������� �����");
        exit(EXIT_FAILURE);
    }
    
    /* ���������� ����� */
    if ((arch_fp = fopen(arch_name, "ab")) != NULL) {
        if (fwrite(&header, sizeof(char), sizeof(struct meta_data), arch_fp) != sizeof(struct meta_data)) {
            perror("������ ������ ����������");
            return -1;
        }
    }
    else {
        perror("������ �������� ������ ��� ��������");
        exit(EXIT_FAILURE);
    }
    while ((n_obj = fread(buf, sizeof(char), BUF_SIZE, fp)) != 0) /* ������ �� ����� ����� */
    {
        /*����� ������������ ��������� ��������� ����������
        n_obj � ���������� ������������� ����������� ������ ����������*/
        out_obj += fwrite(buf, sizeof(char), n_obj, arch_fp);
    }
    if (header.size == out_obj) {
        printf("���� ������� ������� � �����\n");
    }
    else {
        printf("������ ����� �� �������\n");
    }
    free(buf);
    fclose(arch_fp);
    fclose(fp);
    return 0;
}


/*
 * extract_from_arch() - ��������� ����� ��� ����������
 * @arch_name:          ��� ������
 *
 * ��������� �� ������������� ������, ��������� ���
 * ��������������� ������ ���������� (header),
 * �������� ��� ����� � header �  extract_file
 *
 * ����������:
 * -1: ������ ������������� �����
 *  0: �����
 */
int extract_from_arch(char* arch_name) {
    FILE* arch_fp;
    struct meta_data header;

    /* �������� �� ������������� ������ */
    if ((arch_fp = fopen(arch_name, "rb")) != NULL) {
        size_t len = fread(&header, sizeof(char), sizeof(struct meta_data), arch_fp);
        /*
         * ������������ �����
         */
        while (len > 0) {
            /* �������� �� ����� ������ (�������� header) */
            if (header.size == 0 && strlen(header.name) == 0) break;
            printf("����������: %s\n", header.name);
            extract_file(arch_fp, header);

            len = fread(&header, sizeof(char), sizeof(struct meta_data), arch_fp);
        }
    }
    else {
        perror("������ ������������� �����");
        return -1;
    }
    fclose(arch_fp);
    return 0;
}

/**
 * extract_file() - ���������� ����� (�� ����������)
 * @arch_fp:    �������� ��������� �� �����
 * @header:     ���������� �����
 *
 * ��������� ������������� �����, ���������� � ����������
 * ������� ����, �������� � ���� ������ �� ������, �������� header
 * (� ������ ������ size ���� ������)
 *
 * ����������:
 * -1: ������ ������ ������
 * -2: �� ������� ������� ������� ����
 * -3: �� ������� ������� ���� ��� ������
 *  0: �����
 */
int extract_file(FILE* arch_fp, struct meta_data header) {
    char* name = header.name;
    unsigned long long size = header.size;
    FILE* fp;
    size_t n_obj;
    unsigned long long out_obj = 0;
    long offset = 0;
    char* buf = (char*)malloc(sizeof(size_t) * BUF_SIZE);
    /* ���� ���� ���������� */
    
    if ((fp = fopen(name, "r")) != NULL)
    {
        fclose(fp);
        int c;
        printf("���� ����������, ������������? [y/n]: ");
        c = toupper(getchar());
        int answ = c;
        while (c != '\n' && c != EOF) c = getchar();
        if (answ != 'Y') {
            if (fseek(arch_fp, size, SEEK_CUR) == -1) {
                perror("������ ������ ������");
                /*��������� ���������� ��,
                 *���������� ������������ ����������
                 */
                abort();
            }
            return -1;
        }
    }
    /* ������� ��� �������������� ���� */
    if ((fp = fopen(name, "wb")) != NULL)
    {
        while (((n_obj = fread(buf, sizeof(char), BUF_SIZE, arch_fp)) != 0)) /* ������ �� ����� ����� */
        {
            if ((out_obj + n_obj) > size) {
                offset = (out_obj + n_obj) - size;
                out_obj += fwrite(buf, sizeof(char), size - out_obj, fp);
                
                printf("%li%c", offset, '\n');
                fseek(arch_fp, -offset, SEEK_CUR);
                break;
            }
            /*����� ������������ ��������� ��������� ����������
            n_obj � ���������� ������������� ����������� ������ ����������*/
            out_obj += fwrite(buf, sizeof(char), n_obj, fp);
        }
        if (size == out_obj) {
            printf("���� ������� �������� �� ������\n");
        }
        else {
            printf("�� ������� ������� ������� ����\n");
            return -2;
        }
    }
    else {
        printf("�� ������� ������� ���� ", name, " ��� ������\n");
        return -3;
    }
    free(buf);
    fclose(fp);
    return 0;
}

/*���������� ������ ������ � ���������*/
long int filesize(FILE* fp)
{
    long int save_pos, size_of_file;

    save_pos = ftell(fp);
    fseek(fp, 0L, SEEK_END);
    size_of_file = ftell(fp);
    fseek(fp, save_pos, SEEK_SET);
    return size_of_file;
}