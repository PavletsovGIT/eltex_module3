#include "contacts.h"

int create_record(person_t person, char* buff) {
	char* ptr = buff;
	int size;
	char dot = ';', new_line = '\n';
	char str[STR_SIZE];
	
	// Convert id to str
	if (snprintf(str, STR_SIZE, "%d", person.id) < 0) {
		perror("convert person id to string");
		exit(EXIT_FAILURE);
	}
	size = strlen(str);
	strncpy(ptr, str, size);
	ptr += size;
	*(ptr) = dot;
	ptr++;
	
	// surname
	size = strlen(person.surname);
	strncpy(ptr, person.surname, size);
	ptr += size;
	*(ptr) = dot;
	ptr++;
	
	// surname
	size = strlen(person.name);
	strncpy(ptr, person.name, size);
	ptr += size;
	*(ptr) = dot;
	ptr++;
	
	// patronimic
	size = strlen(person.patronimic);
	strncpy(ptr, person.patronimic, size);
	ptr += size;
	*(ptr) = dot;
	ptr++;
	
	// phone
	size = strlen(person.phone);
	strncpy(ptr, person.phone, size);
	ptr += size;
	*(ptr) = dot;
	ptr++;
	
	// work.company
	size = strlen(person.work.company);
	strncpy(ptr, person.work.company, size);
	ptr += size;
	*(ptr) = dot;
	ptr++;
	
	// work.post
	size = strlen(person.work.post);
	strncpy(ptr, person.work.post, size);
	ptr += size;
	*(ptr) = dot;
	ptr++;
	
	// soc_net.name
	size = strlen(person.socialNetwork.name);
	strncpy(ptr, person.socialNetwork.name, size);
	ptr += size;
	*(ptr) = dot;
	ptr++;
	
	// soc_name.link
	size = strlen(person.socialNetwork.link);
	strncpy(ptr, person.socialNetwork.link, size);
	ptr += size;
	*(ptr) = dot;
	ptr++;
	
	// add \n
	*(ptr) = new_line;
	size = ptr - buff;
	
	return size;
}

int get_max_id() {
	int fd = open("contacts.cvs", O_RDONLY);
    if (fd == -1) {
        perror("can't open file");
        exit("EXIT_FAILURE");
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
        while (*line && (line - buffer) < bytes_read) {
            person_t prs;
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
	n = create_record(prs, buff);
	
	// Дозапись в файл
	int fd = open("contacts.cvs", O_WRONLY | O_APPEND);
	
	if (fd == -1) {
		perror("cant open file");
		exit(EXIT_FAILURE);
	}
	
	bytes_written = write(fd, buff, n);
	n = 1;
	buff[0] = '\n';
	write(fd, buff, n);

    printf("Записали %ld байт в файл.\n", bytes_written);

    close(fd);
}

void RemovePerson(int id) {
	
}

void EditPerson(int id) {

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
