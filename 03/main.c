#include "contacts.h"

void test_addPerson();
void test_getMaxID();
void test_findPerson(int id);
void test_correctIDs();

int main() {
	for (int i = 0; i < 11; i++) {test_addPerson();}
	
	//test_findPerson(3);
	//test_findPerson(15);
	test_correctIDs();
	
	exit(EXIT_SUCCESS);
}

void test_addPerson() {
	int count = get_max_id();
	person_t person = {
		.id = count + 1,
		.name = "John", 
		.surname = "Silver", 
		.patronimic = "Batckovich", 
		.phone = "666", 
		.work.company = "CaribianPirates", 
		.work.post = "Captian", 
		.socialNetwork.name = "telegram",
		.socialNetwork.link = "@theDeath"
	};
	
	AddPerson(person);
}

void test_getMaxID() {
	int id = get_max_id() + 1;
	printf("Max ID = %d", id);
}

void test_findPerson(int id) {
	person_t prs;
	prs = find_person_by_id(id);
	
	printf("Founded person:\n");
	printf("\tID: %d\n\t%s\n\t%s\n\t%s\n\t%s\n\t%s\n\t%s\n\t%s\n\t%s\n", prs.id, prs.name, prs.surname, prs.patronimic, prs.phone, prs.work.company, prs.work.post, prs.socialNetwork.name, prs.socialNetwork.link);
}

void test_correctIDs() {
	correct_ids();
}
