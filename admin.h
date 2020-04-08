/*
Student: Apetroaie Razvan-Mihai
Grupa: 313CB
*/

//functie ce returneaza suma cifrelor numarului cardului
int sumCardNumber(unsigned long long int x)
{
	int s = 0;

	while(x)
	{
		s += x  %  10;
		x = x / 10;
	}

	return s;
}

/*functie utilizata la stergerea unui card din lista LC pe
baza numarului acestuia*/
void deleteCard(LSC* L, unsigned long long int cardNumber, int* cardDeleted)
{
	LSC* r = L;
	Card* card, *prevCard;
	int i, poz = 0;

	while(r && !r->firstCard)
	{
		poz++;
		r = r->urm;
	}

	while(r)
	{
		card = r->firstCard;
		if(card)
		{
			if(card->cardNumber == cardNumber)
			{
				r->firstCard = r->firstCard->urm;
				free(card->history);
				free(card);
				for(i = 0; i <= poz; i++)
				{	
					cardDeleted[i] = 1;
				}
				return;
			}

			else
			{
				while(card->urm)
				{
					prevCard = card;
					card = card->urm;
					if(card->cardNumber == cardNumber)
					{
						prevCard->urm = card->urm;
						free(card->history);
						free(card);
						for(i = 0; i <= poz; i++)
						{	
							cardDeleted[i] = 1;
						}
						return;
					}
				}

			}
		}
		r = r->urm;
		poz++;

	}
}

//functia de adaugare a unui card in lista
void addCard(LSC* L, int nr, unsigned long long int cardNumber, int PIN, 
	char* expiryDate, int CVV, FILE* output)
{
	LSC* r = L;
	Card* card;

	while(r && !r->firstCard)
	{
		r = r->urm;
	}

	while(r)
	{
		card = r->firstCard;

		while(card)
		{
			if(card->cardNumber == cardNumber)
			{
				fprintf(output, "The card already exists\n");
				return;
			}

			card = card->urm;

		}

		r = r->urm;

	}

	int sum = sumCardNumber(cardNumber), poz = sum % nr;
	r = L;
	Card* newCard = (Card*)malloc(sizeof(Card));
	if(!newCard) return;
	newCard->history = NULL;

	while(poz)
	{
		r = r->urm;
		poz--;
	}
	newCard->cardNumber = cardNumber;
	newCard->initialPIN = PIN;
	newCard->PIN = PIN;
	newCard->PINTrials = 3;
	strcpy(newCard->expiryDate, expiryDate);
	newCard->CVV = CVV;
	newCard->balance = 0;
	strcpy(newCard->status, "NEW");
	newCard->urm = r->firstCard;
	r->firstCard = newCard;

}

//functie utilizata la eliberarea memoriei alocate listei
void freeLSC(LSC* L, int nr)
{
	LSC* ant = L;
	Card* prevCard;
	History* prevHistory;

	while(L)
	{
		ant = L;
		L = L->urm;

		while(ant->firstCard)
		{
			prevCard = ant->firstCard;
			ant->firstCard = ant->firstCard->urm;
			while(prevCard->history)
			{
				prevHistory = prevCard->history;
				prevCard->history = prevCard->history->urm;
				free(prevHistory);
			}
			free(prevCard->history);
			free(prevCard);
		}
		free(ant);
	}

	free(L);
}

LSC* resizeLC(LSC* L, int nr)
{
	int i;
	LSC *aux, *r = (LSC*)malloc(sizeof(LSC)), *newLC = r;
	Card* card;
	FILE* nullFile = NULL;

	if(!r) return NULL;

	r->firstCard = NULL;


	for(i = 0; i < nr - 1; i++)
	{
		aux = (LSC*)malloc(sizeof(LSC));
		if(!aux) return NULL;
		
		aux->firstCard = NULL;
		r->urm = aux;
		r = aux;
	}

	r->urm = NULL;

	r = L;

	while(r)
	{
		card = r->firstCard;

		while(card)
		{
			addCard(newLC, nr, card->cardNumber, card->PIN, card->expiryDate, 
				card->CVV, nullFile);

			card = card->urm;

		}

		r = r->urm;

	}

	freeLSC(L, nr / 2);

	return newLC;	
}

//functie de anulare a tranzactiei
void reverseTransaction(LSC* L, unsigned long long int cardNumberSource, 
	unsigned long long int cardNumberDest, int sum, FILE* output)
{
	LSC* r = L;
	Card* card, *cardSource = NULL, *cardDest = NULL;

	while(r && !r->firstCard)
	{
		r = r->urm;
	}

	while(r)
	{
		card = r->firstCard;

		while(card)
		{
			if(card->cardNumber == cardNumberSource)
			{
				cardSource = card;
			}
			if(card->cardNumber == cardNumberDest)
			{
				cardDest = card;
			}

			card = card->urm;

		}

		if(cardSource && cardDest)
		{
			break;
		}

		r = r->urm;

	}

	if(cardDest->balance < sum)
	{
		fprintf(output, "The transaction cannot be reversed\n");
		return;
	}

	History* HSource = (History*)malloc(sizeof(History));

	sprintf(HSource->action, "reverse_transaction  %llu  %llu  %d", cardNumberSource, cardNumberDest, sum);

	if(!cardSource->history)
	{
		cardSource->history = HSource;
		cardSource->history->urm = NULL;
	}
	else
	{
		HSource->urm = cardSource->history;
		cardSource->history = HSource;
	}

	History* HDest = (History*)malloc(sizeof(History));

	if(!cardDest->history)
	{
		cardDest->history = HDest;
		cardDest->history->urm = NULL;
	}
	else
	{
		HDest->urm = cardDest->history;
		cardDest->history = HDest;
	}

	strcpy(cardSource->history->result, "SUCCESS");

	History* H = cardSource->history, *Hant = H;
	char action[100];

	sprintf(action, "transfer_funds  %llu  %llu  %d", cardNumberSource, cardNumberDest, sum);

	H = cardDest->history;
	Hant = H;
	
	H = H->urm;

	while(strcmp(H->action, action))
	{
		Hant = H;
		H = H->urm;
	}

	Hant->urm = H->urm;
	free(H);


	cardSource->balance += sum;
	cardDest->balance -= sum;

}

//functie pentru deblocarea cardului
void unblockCard(LSC* L, unsigned long long int cardNumber)
{
	LSC* r = L;
	Card* card;

	while(r && !r->firstCard)
	{
		r = r->urm;
	}

	while(r)
	{
		card = r->firstCard;

		while(card)
		{
			if(card->cardNumber == cardNumber)
			{
				card->PINTrials = 3;
				card->initialPIN = -1;
				strcpy(card->status, "ACTIVE");
				return;
			}

			card = card->urm;

		}

		r = r->urm;

	}
}

void printHistory(Card* card, FILE* output)
{
	History* H = card->history;

	if(H)
	{
		while(H->urm)
		{
			if(H->result[0] != '\0')
			fprintf(output, "( %s,  %s), ", H->result, H->action);
			H = H->urm;
		}
		if(H->result[0] != '\0')
			fprintf(output, "( %s,  %s)", H->result, H->action);
	}
}

//functie ce afiseaza toate cardurile din lista in momentul apelarii
void showAll(LSC* L, FILE* output, int* cardDeleted)
{
	LSC* r = L, *emptyCheck;
	Card* card;
	int i = 0, nrZeros, PIN, CVV;
	char zeros[3], PINres[5], CVVres[4];

	while(r)
	{
		card = r->firstCard;

		emptyCheck = r;

		if(!cardDeleted[i])
		{
			while(emptyCheck && !emptyCheck->firstCard)
			{
				emptyCheck = emptyCheck->urm;
			}

			if(!emptyCheck)
				return;

		}

		fprintf(output, "pos %d: [", i);

		if(card)
		{
			fprintf(output, "\n");
		}

		while(card)
		{

			if(card->history && card->PIN != card->initialPIN && 
				strcmp(card->status, "LOCKED"))
			{
				strcpy(card->status, "ACTIVE");
			}

			if(!card->PIN)
			{
				strcpy(PINres, "0000");
			}
			else
			{
				nrZeros = 4;
				PIN = card->PIN;

				while(PIN)
				{
					PIN = PIN / 10;
					nrZeros--;
				}

				zeros[nrZeros] = '\0';

				while(nrZeros)
				{
					zeros[nrZeros - 1] = '0';
					nrZeros--;
				}


				sprintf(PINres, " %s %d", zeros, card->PIN);
			}

			if(!card->CVV)
			{
				strcpy(CVVres, "000");
			}
			else
			{

				nrZeros = 3;
				CVV = card->CVV;

				while(CVV)
				{
					CVV = CVV / 10;
					nrZeros--;
				}

				zeros[nrZeros] = '\0';

				while(nrZeros)
				{
					zeros[nrZeros - 1] = '0';
					nrZeros--;
				}


				sprintf(CVVres, " %s %d", zeros, card->CVV);
			}

			fprintf(output, "(card number:  %llu, PIN:  %s, expiry date:  %s, CVV: %s, balance:  %d, status:  %s, history: [",
			 card->cardNumber, PINres, card->expiryDate, CVVres, card->balance, card->status);
			printHistory(card, output);
			fprintf(output, "])\n");

			card = card->urm;

		}

		fprintf(output, "]\n");

		r = r->urm;
		i++;

	}	
}

//functie ce afiseaza detaliile cardului introdus ca parametru
void show(LSC* L, unsigned long long int cardNumber, FILE* output)
{
	LSC* r = L;
	Card* card;
	int nrZeros, PIN;
	char zeros[3], PINres[5];

	while(r)
	{
		card = r->firstCard;

		while(card)
		{
			if(card->cardNumber == cardNumber)
			{
				if(card->history && card->PIN != card->initialPIN && strcmp(card->status, "LOCKED"))
				{
					strcpy(card->status, "ACTIVE");
				}

				if(!card->PIN)
				{
					strcpy(PINres, "0000");
				}
				else
				{
					nrZeros = 4;
					PIN = card->PIN;

					while(PIN)
					{
						PIN = PIN / 10;
						nrZeros--;
					}

					zeros[nrZeros] = '\0';

					while(nrZeros)
					{
						zeros[nrZeros - 1] = '0';
						nrZeros--;
					}


					sprintf(PINres, " %s %d", zeros, card->PIN);
				}


				fprintf(output, "(card number:  %llu, PIN:  %s, expiry date:  %s, CVV:  %d, balance: %d, status:  %s, history: [",
				 card->cardNumber, PINres, card->expiryDate, card->CVV, card->balance, card->status);
				printHistory(card, output);
				fprintf(output, "])\n");
				return;
			}

			card = card->urm;

		}

		r = r->urm;

	}

}