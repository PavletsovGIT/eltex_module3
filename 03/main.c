/*
	Доработать решение задачи 2.1 (список контактов) так, 
	чтобы для хранения данных использовался 
	двухсвязный упорядоченный список.
*/

#include "contacts.h"

void print_person(person_t* prs) {
    // Буфер для форматированной строки
    char buffer[512]; // Размер 512 символов, ты можешь изменить в зависимости от того, сколько данных может быть в person_t
    int size = 0;

    printf("<----------------------------->\n");
    
    // Форматирование строки
    size = snprintf(buffer, sizeof(buffer), 
                    "%d\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n",
                    prs->id, prs->name, prs->surname, prs->patronimic, prs->phone,
                    prs->work.company, prs->work.post, prs->socialNetwork.link, prs->socialNetwork.name);

    
    // Проверка на успешность записи
    if (size < 0) {
        printf("Error formatting the string.\n");
    } else {
        printf("%s", buffer);
    }

    printf("<----------------------------->\n");
}

void print_contacts(contacts_t* contacts) {
	item_t* current_item = Begin(contacts);
    if (!current_item) {
        puts("Контакты пусты.");
    } else {
        while (current_item != NULL) {
            print_person(&current_item->p);
            current_item = current_item->next;
        }
    }
}

void test_AddContacts(contacts_t* contacts) {
    person_t person1 = {
        .id = 1,
        .name = "Ivan",
        .surname = "Ivanov",
        .patronimic = "Ivanovich",
        .phone = "+7 800 555 3535",
        .work = {
            .company = "ZAO 'Semicvetik'",
            .post = "Sistemny administrator"
        },
        .socialNetwork = {
            .name = "Telegram",
            .link = "@sys_admin"
        }
    };

    person_t person2 = {
        .id = 2,
        .name = "Petr",
        .surname = "Petrov",
        .patronimic = "Petrovich",
        .phone = "+7 987 654 3210",
        .work = {
            .company = "ZAO 'Vasilek'",
            .post = "Programmist"
        },
        .socialNetwork = {
            .name = "Instagram",
            .link = "instagram.com/petrov"
        }
    };

    AddPerson(contacts, person1);
    AddPerson(contacts, person2);
}


void test_write_contacts() {
	contacts_t contacts;
	InitContacts(&contacts);

	test_AddContacts(&contacts);
	write_contacts(&contacts);
}

void test_read_contacts() {
    contacts_t contacts;
    contacts = read_contacts();

    print_contacts(&contacts);
}

void test_contacts() {
    contacts_t my_contacts;
    InitContacts(&my_contacts);

    test_AddContacts(&my_contacts);

    printf("\nСписок контактов:\n");
    print_contacts(&my_contacts);

	/*
    // Найти контакт по ID
    int search_id = 2;
    person_t* found_person = GetPersonById(&my_contacts, search_id);
    if (found_person) {
        printf("\nКонтакт с ID=%d найден:\n", search_id);
        print_person(found_person);
    } else {
        printf("Контакт с таким ID не найден.\n");
    }

    // Удаление контакта
    DeletePerson(&my_contacts, found_person);

	printf("\nСписок контактов:\n");
    print_contacts(&my_contacts);

    person_t edit_prs = person1;
    
    SetSurname(&edit_prs, "Программистов");
    EditPerson(&person1, &edit_prs);
    
    printf("\nСписок контактов:\n");
    print_contacts(&my_contacts);
	*/
}

int main() {
    //test_contacts();
    test_write_contacts();
    test_read_contacts();
    return 0;
}