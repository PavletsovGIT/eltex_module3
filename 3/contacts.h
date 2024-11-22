#ifndef CONTACTS_H_
#define CONTACTS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFFSIZE 4096
#define STR_SIZE 32

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

int create_record(person_t person, char* buff);
int get_max_id();
person_t find_person_by_id(int id);
void correct_ids();

// Functions
void InitPerson(person_t* prs);
void InitWork(work_t* work);
void InitSocialNetwork(social_network_t* sn);

void AddPerson(person_t prs);
void RemovePerson(int id);
void EditPerson(int id, person_t person);

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

#endif // CONTACTS_H_
