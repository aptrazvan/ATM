/*
Student: Apetroaie Razvan-Mihai
Grupa: 313CB
*/

//functie ce testeaza introducerea unui PIN valid
int PINCheck(char* PIN)
{
	int i;

	if(strlen(PIN) != 4)
	{
		return 0;
	}

	for(i = 0; i < 4; i++)
	{
		if(PIN[i] < '0' || PIN[i] > '9')
		{
			return 0;
		}
	}

	return 1;
}

//functie ce realizeaza inserarea cardului
void insertCard(LSC* L, unsigned long long int cardNumber, 
	char* PIN, FILE* output)
{
	LSC* r = L;
	Card* card;

	History* H = (History*)malloc(sizeof(History));

	sprintf(H->action, "insert_card %llu %s", cardNumber, PIN);

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

				if(!card->history)
				{
					card->history = H;
					card->history->urm = NULL;
				}
				else
				{
					H->urm = card->history;
					card->history = H;
				}

				if(card->PINTrials)
				{
					if(card->PIN == atoi(PIN) && PINCheck(PIN) == 1)
					{
						card->PINTrials = 3;

						if(card->PIN == card->initialPIN)
						{
							fprintf(output, "You must change your PIN.\n");
						}

						strcpy(card->history->result, "SUCCESS");

						return;
					}

					card->PINTrials--;
					fprintf(output, "Invalid PIN\n");

					if(!card->PINTrials)
					{
						strcpy(card->status, "LOCKED");
						fprintf(output, "The card is blocked. Please contact the administrator.\n");
					}
				}
				else
				{
					fprintf(output, "The card is blocked. Please contact the administrator.\n");
				}

				strcpy(card->history->result, "FAIL");

				return;
			}

			card = card->urm;

		}

		r = r->urm;

	}
}

//functie ce realizeaza actiunea de reincarcare a cardului
void recharge(LSC* L, unsigned long long int cardNumber, 
	int sum, FILE* output)
{
	LSC* r = L;
	Card* card;

	History* H = (History*)malloc(sizeof(History));

	sprintf(H->action, "recharge %llu %d", cardNumber, sum);

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
				if(!card->history)
				{
					card->history = H;
					card->history->urm = NULL;
				}
				else
				{
					H->urm = card->history;
					card->history = H;
				}

				if(sum % 10)
				{
					fprintf(output, "The added amount must be multiple of 10\n");
					strcpy(card->history->result, "FAIL");

					return;
				}


				card->balance += sum;
				fprintf(output, "%d\n", card->balance);

				strcpy(card->history->result, "SUCCESS");

				return;
			}

			card = card->urm;

		}

		r = r->urm;

	}
}

//functie ce realizeaza actiune de retragere a unei sume de pe card
void cashWithdrawal(LSC* L, unsigned long long int cardNumber, int sum, FILE* output)
{
	LSC* r = L;
	Card* card;

	History* H = (History*)malloc(sizeof(History));

	sprintf(H->action, "cash_withdrawal %llu %d", cardNumber, sum);

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
				if(!card->history)
				{
					card->history = H;
					card->history->urm = NULL;
				}
				else
				{
					H->urm = card->history;
					card->history = H;
				}

				if(sum % 10)
				{
					fprintf(output, "The requested amount must be multiple of 10\n");
					strcpy(card->history->result, "FAIL");
					return;
				}

				if(card->balance < sum)
				{
					fprintf(output, "Insufficient funds\n");
					strcpy(card->history->result, "FAIL");
					return;
				}
				card->balance -= sum;
				fprintf(output, "%d\n", card->balance);
				strcpy(card->history->result, "SUCCESS");
				return;
			}

			card = card->urm;

		}

		r = r->urm;

	}
}

//functia de afisare a soldului curent
void balanceInquiry(LSC* L, unsigned long long int cardNumber, FILE* output)
{
	LSC* r = L;
	Card* card;

	History* H = (History*)malloc(sizeof(History));

	sprintf(H->action, "balance_inquiry %llu", cardNumber);

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
				if(!card->history)
				{
					card->history = H;
					card->history->urm = NULL;
				}
				else
				{
					H->urm = card->history;
					card->history = H;
				}

				fprintf(output, "%d\n", card->balance);
				strcpy(card->history->result, "SUCCESS");
				return;
			}

			card = card->urm;

		}

		r = r->urm;

	}
}

/*functie ce realizeaza actiune de transfer a unei
sume de pe un card pe altul*/
void transferFunds(LSC* L, unsigned long long int cardNumberSource, 
	unsigned long long int cardNumberDest, int sum, FILE* output)
{
	LSC* r = L;
	Card* card, *cardSource = NULL, *cardDest = NULL;

	History* HSource = (History*)malloc(sizeof(History));

	sprintf(HSource->action, "transfer_funds %llu %llu %d", 
		cardNumberSource, cardNumberDest, sum);

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

	if(sum % 10)
	{
		fprintf(output, "The transferred amount must be multiple of 10\n");
		strcpy(cardSource->history->result, "FAIL");
		return;
	}

	if(cardSource->balance < sum)
	{
		fprintf(output, "Insufficient funds\n");
		strcpy(cardSource->history->result, "FAIL");
		return;
	}

	History* HDest = (History*)malloc(sizeof(History));

	sprintf(HDest->action, "transfer_funds %llu %llu %d", 
		cardNumberSource, cardNumberDest, sum);

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
	strcpy(cardDest->history->result, "SUCCESS");



	cardSource->balance -= sum;
	cardDest->balance += sum;
	fprintf(output, "%d\n", cardSource->balance);


}

//functie pentru schimbarea PIN-ului
void pinChange(LSC* L, unsigned long long int cardNumber, 
	char* PIN, FILE* output)
{
	LSC* r = L;
	Card* card;

	History* H = (History*)malloc(sizeof(History));

	sprintf(H->action, "pin_change %llu %s", cardNumber, PIN);

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
				if(!card->history)
				{
					card->history = H;
					card->history->urm = NULL;
				}
				else
				{
					H->urm = card->history;
					card->history = H;
				}

				if(PINCheck(PIN) == 0)
				{
					fprintf(output, "Invalid PIN\n");
					strcpy(card->history->result, "FAIL");
					return;
				}


				card->PIN = atoi(PIN);
				strcpy(card->history->result, "SUCCESS");
				return;
			}

			card = card->urm;

		}

		r = r->urm;

	}
}

//functie pentru retragerea cardului
void cancel(LSC* L, unsigned long long int cardNumber)
{
	LSC* r = L;
	Card* card;

	History* H = (History*)malloc(sizeof(History));

	sprintf(H->action, "cancel %llu", cardNumber);

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
				if(!card->history)
				{
					card->history = H;
					card->history->urm = NULL;
				}
				else
				{
					H->urm = card->history;
					card->history = H;
				}

				strcpy(card->history->result, "SUCCESS");
				return;
			}

			card = card->urm;

		}

		r = r->urm;

	}
}