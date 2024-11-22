#include "contacts.h"

int create_record(person_t prs, char* buffer) {
    return sprintf(buffer, "%d;%s;%s;%s;%s;%s;%s;%s;%s\n", prs.id, prs.name, prs.surname, prs.patronimic, prs.phone, prs.work.company, prs.work.post, prs.socialNetwork.name, prs.socialNetwork.link);
}

int get_max_id() {
	int fd = open("contacts.cvs", O_RDONLY);
    if (fd == -1) {
        perror("can't open file");
        exit(EXIT_FAILURE);
    }

    int max_id = -1;  // Начальное значение для максимума
    char buffer[BUFFSIZE];
    ssize_t bytes_read;

    off_t offset = 0; // Смещение для lseek
    while ((bytes_read = read(fd, buffer, BUFFSIZE)) > 0) {
        // Перемещаемся назад на прочитанное количество байт
        lseek(fd, offset, SEEK_SET);

        // Парсим все строки в буфере
        char *line = buffer;
        person_t prs;
        while (*line && (line - buffer) < bytes_read) {
            if (sscanf(line, "%d;%32[^;];%32[^;];%32[^;];%32[^;];%32[^;];%32[^;];%32[^;];%32[^\n];", &prs.id, prs.name, prs.surname, prs.patronimic, prs.phone, prs.work.company, prs.work.post, prs.socialNetwork.name, prs.socialNetwork.link) == 9) {
                if (prs.id > max_id) {
                    max_id = prs.id;
                }
            }
            // Переходим к следующей строке
            line += strcspn(line, "\n") + 1;
        }

        // Смещаемся вперед на прочитанный блок
        offset += bytes_read;
    }

    close(fd);
    return max_id;
}

person_t find_person_by_id(int id) {
	int fd = open("contacts.cvs", O_RDONLY);
    if (fd == -1) {
        perror("can't open file");
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFSIZE];
    ssize_t bytes_read;
    
    person_t prs;

    off_t offset = 0; // Смещение для lseek
    while ((bytes_read = read(fd, buffer, BUFFSIZE)) > 0) {
        // Перемещаемся назад на прочитанное количество байт
        lseek(fd, offset, SEEK_SET);

        // Парсим все строки в буфере
        char *line = buffer;
        while (*line && (line - buffer) < bytes_read) {
            if (sscanf(line, "%d;%32[^;];%32[^;];%32[^;];%32[^;];%32[^;];%32[^;];%32[^;];%32[^;];\n", &prs.id, prs.name, prs.surname, prs.patronimic, prs.phone, prs.work.company, prs.work.post, prs.socialNetwork.name, prs.socialNetwork.link) == 9) {
                if (prs.id == id) {
                    return prs;
                }
            }
            // Переходим к следующей строке
            line += strcspn(line, "\n") + 1;
        }

        // Смещаемся вперед на прочитанный блок
        offset += bytes_read;
    }
    
    // В случае неудачи возвращается дефолтный person со значением -1 в поле id
    InitPerson(&prs);
    prs.id = -1;

    close(fd);
    return prs;
}

void correct_ids() {
	int fd = open("contacts.cvs", O_RDWR), count_fields, now_id = 0, new_str_size;
	char buff[BUFFSIZE]; // Буффер для записи
	char new_str[BUFFSIZE];
	char* line;
	ssize_t bytes_read; //  Кол-вл прочитаных байт
	off_t offset = 0; // Смещение указателя для lseek
	person_t prs;
	
	if (fd == -1) { // Проверка на открытие файла
		perror("can't open file");
		exit(EXIT_FAILURE);
	}
	
	do {
		bytes_read = read(fd, buff, BUFFSIZE);
		lseek(fd, offset, SEEK_SET); // Перемещаемся на кол-во прочитанных байт от начала файла
		
		line = buff; // Просматриваем все строки в буфере
		while(*line && (line - buff) < bytes_read) {
			count_fields = sscanf(line, "%d;%32[^;];%32[^;];%32[^;];%32[^;];%32[^;];%32[^;];%32[^;];%32[^;];\n", &prs.id, prs.name, prs.surname, prs.patronimic, prs.phone, prs.work.company, prs.work.post, prs.socialNetwork.name, prs.socialNetwork.link);
			
			if (count_fields != 9) { // 9 = кол-во переменных, которое считал sscanf. Ловим ошибку считывания
				perror("can't read line in file");
				printf("Count fields = %d", count_fields);
				exit(EXIT_FAILURE);
				continue;
			}
			
			if (prs.id == now_id) { // Если prs.id == now_id то в записи стоит правильный порядковый id
				line += strcspn(line, "\n") + 1; // перемещаем указатель дальше по строке, до следующего \n
				now_id++;
				continue;
			}
			
			/*Производим замену строки на нужную*/
			prs.id  = now_id; // Меняем на порядковый id
			new_str_size = create_record(prs, new_str);
			
            lseek(fd, offset, SEEK_SET);
            write(fd, new_str, new_str_size);
            lseek(fd, offset + new_str_size, SEEK_CUR);
			
			now_id++;
		}
		offset += bytes_read; // увеличиваем смещение
	}	while(bytes_read > 0);
	
}

void InitPerson(person_t* prs) {
    char def_str[STR_SIZE] = "_default";
    prs->id = 0;
    strncpy(prs->name, def_str, STR_SIZE);
    strncpy(prs->surname, def_str, STR_SIZE);
    strncpy(prs->patronimic, def_str, STR_SIZE);
    strncpy(prs->phone, def_str, STR_SIZE);
    InitWork(&prs->work);
    InitSocialNetwork(&prs->socialNetwork);
}

void InitWork(work_t* work) {
    char def_str[STR_SIZE] = "_default";

    strncpy(work->company, def_str, STR_SIZE);
    strncpy(work->post, def_str, STR_SIZE);
}

void InitSocialNetwork(social_network_t* sn) {
    char def_str[STR_SIZE] = "_default";

    strncpy(sn->name, def_str, STR_SIZE);
    strncpy(sn->link, def_str, STR_SIZE);
}

void AddPerson(person_t prs) {
	char newline = '\n', separator = ';';
	char buff[BUFFSIZE];
	ssize_t bytes_written;
	int n;
	
	// Создаём запись в buff, n - длинна записи
	n = create_record(prs, buff) + 1;
	
	// Дозапись в файл
	int fd = open("contacts.cvs", O_WRONLY | O_APPEND);
	
	if (fd == -1) {
		perror("cant open file");
		exit(EXIT_FAILURE);
	}
	
	bytes_written = write(fd, buff, n);
	/*
	n = 1;
	buff[0] = '\n';
	write(fd, buff, n);
	*/

    printf("Записали %ld байт в файл.\n", bytes_written);

    close(fd);
}

void RemovePerson(int id) {
	
}

void EditPerson(int id, person_t person) {

}

void SetName(person_t* prs, const char* _name) {
    strncpy(prs->name, _name, STR_SIZE - 1); // Оставляем место для завершающего нуля
    prs->name[STR_SIZE - 1] = '\0';          // Убеждаемся, что строка завершается нулевым символом
}

void SetSurname(person_t* prs, const char* _surname) {
    strncpy(prs->surname, _surname, STR_SIZE - 1);
    prs->surname[STR_SIZE - 1] = '\0';
}

void SetPatronimic(person_t* prs, const char* _patronimic) {
    strncpy(prs->patronimic, _patronimic, STR_SIZE - 1);
    prs->patronimic[STR_SIZE - 1] = '\0';
}

void SetPhone(person_t* prs, const char* _phone) {
    strncpy(prs->phone, _phone, STR_SIZE - 1);
    prs->phone[STR_SIZE - 1] = '\0';
}

void SetWork(person_t* prs, work_t* _work) {
    SetWorkCompany(&prs->work, _work->company);
    SetWorkPost(&prs->work, _work->post);
}

void SetSocialNetwork(person_t* prs, social_network_t* _sn) {
    SetSocNetName(&prs->socialNetwork, _sn->name);
    SetSocNetLink(&prs->socialNetwork, _sn->link);
}

void SetWorkCompany(work_t* work, const char* _company) {
    strncpy(work->company, _company, STR_SIZE - 1);
    work->company[STR_SIZE - 1] = '\0';
}

void SetWorkPost(work_t* work, const char* _post) {
	strncpy(work->post, _post, STR_SIZE);
}

void SetSocNetName(social_network_t* sn, const char* _name)  {
	strncpy(sn->name, _name, STR_SIZE);
}

void SetSocNetLink(social_network_t* sn, const char* _link)  {
	strncpy(sn->link, _link, STR_SIZE);
}
