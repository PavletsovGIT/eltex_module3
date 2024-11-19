#include "contacts.h"

void test_addPerson();

void test_getMaxID();

int main() {
	test_addPerson();
	test_addPerson();
	test_addPerson();
	test_getMaxID();
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
	count++;
	
	AddPerson(person);
}

void test_getMaxID() {
	int id = get_max_id();
	printf("Max ID = %d", id);
}
