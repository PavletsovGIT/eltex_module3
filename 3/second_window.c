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
