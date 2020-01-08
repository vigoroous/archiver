#include "archiver.h"

int main(int argc, char** argv)
{
    setlocale(LC_ALL, "RU");
    char* arch = NULL;
    /*
     * arch_fp � �������� ��������� �� �����.
     *  argv[1]        - ��� ����� ������������
     *  arch = argv[2] - ��� ������
     *  argv[3]        - ����1
     *  argv[4]        - ����2 
     *  .... 
     */
    if (argc <= 2) {
        printf("�������������:\n");
        printf("./arch -e {���_������} "
            "���������� �� ������\n");
        printf("./arch -c {���_������} {����1} {����2} {����3}"
            "... �������� ������\n");
        return 0;
    }
    if (!strcmp("-e", argv[1])) {
        printf("���������� �� ������ : %s\n", argv[2]);
        arch = argv[2];
        extract_from_arch(arch);
    }
    else if (!strcmp("-c", argv[1])) {
        printf("���������� � �����: %s\n", argv[2]);
        arch = argv[2];
        if (argc == 3) {
            printf("��������� ������� ����� ��� "
                "���������� � �����\n");
            return 1;
        }
        FILE* arch_fp = create_arch(arch);
        for (int num = argc; num > 3; num--) {
            printf("������ �����: %s\n", argv[num-1]);
            write_file_to_arch(arch_fp, arch, argv[num-1]);
        }
        end_of_arch(arch_fp, arch);
    } 
    else {
        printf("����������� �������: %s\n", argv[1]);
    }
    return 0;
}