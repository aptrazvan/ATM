/*
Student: Apetroaie Razvan-Mihai
Grupa: 313CB
*/

typedef struct listHistory{
	char result[10];
	char action[100];
	struct listHistory *urm;
}History;

typedef struct card{
	unsigned long long int cardNumber;
	int initialPIN;
	int PIN;
	int PINTrials;
	char expiryDate[8];
	int CVV;
	int balance;
	char status[10];
	History *history;
	struct card *urm;

}Card;

typedef struct listSubCard{
	Card *firstCard;
	struct listSubCard *urm;
}LSC, *LC;