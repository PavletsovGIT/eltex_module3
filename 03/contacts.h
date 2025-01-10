#ifndef CONTACTS_H_
#define CONTACTS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

#define STR_SIZE 50
#define BUF_SIZE 512

typedef struct social_network_t {
    char name[STR_SIZE];
    char link[STR_SIZE];
} social_network_t;

typedef struct work_t {
    char company[STR_SIZE];
    char post[STR_SIZE];
} work_t;

typedef struct person_t {
    int id;
    char name[STR_SIZE];
    char surname[STR_SIZE];
    char patronimic[STR_SIZE];
    char phone[STR_SIZE];
    work_t work;
    social_network_t socialNetwork;
} person_t;

typedef struct item_t {
    struct person_t p;
    struct item_t* next;
    struct item_t* prev;
} item_t;

typedef struct contacts_t {
    size_t size;
    struct item_t head;
} contacts_t;

// Functions
// Читает файл с контактами и выдаёт двусвязный список
contacts_t read_contacts();
// Записывает контакты в файл
void write_contacts(contacts_t* contacts);
// Создаёт person_t из buf
person_t create_record(char* buf);

void InitContacts(contacts_t* cnt);
void InitItem(item_t* item);
void InitPerson(person_t* prs);
void InitWork(work_t* work);
void InitSocialNetwork(social_network_t* sn);

item_t* Begin(contacts_t* cnt);
item_t* End(contacts_t* cnt);

void Swap(item_t* it1, item_t* it2);
void Sort(contacts_t* cnt);

void AddPerson(contacts_t* cnt, person_t prs);
void EditPerson(person_t* prs, person_t* edit_prs);
void DeletePerson(contacts_t* cnt, person_t* prs);

void SetName(person_t* prs, const char* _name);
void SetSurname(person_t* prs, const char* _surname);
void SetPatronimic(person_t* prs, const char* _patronimic);
void SetPhone(person_t* prs, const char* _phone);
void SetWork(person_t* prs, work_t* _work);
void SetSocialNetwork(person_t* prs, social_network_t* _sn);

void SetWorkCompany(work_t* work, const char* _company);
void SetWorkPost(work_t* work, const char* _post);

void SetSocNetName(social_network_t* sn, const char* _name);
void SetSocNetLink(social_network_t* sn, const char* _link);

person_t* GetPersonById(contacts_t* cnt, int _id);

#endif // CONTACTS_H_