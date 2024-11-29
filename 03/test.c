void correct_ids() {
    int fd = open(contacts_path, O_RDWR), count_fields, now_id = 0, new_str_size;
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
                printf("Count fields = %d\nNow ID = %d", count_fields, now_id);
                exit(EXIT_FAILURE);
                continue;
            }
            
            if (prs.id != now_id) {
                prs.id = now_id;
                int new_str_size = create_record(prs, new_str);

                // Запись новой строки в файл
                //lseek(fd, offset, SEEK_SET);
                write(fd, new_str, new_str_size);
            }

            // Переход к следующей строке
            line += strlen(line) + 1;
            offset += strlen(line) + 1;
            now_id++;
        }
    } while (bytes_read > 0);

    close(fd);
}
