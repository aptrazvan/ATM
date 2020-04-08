/*
Student: Apetroaie Razvan-Mihai
Grupa: 313CB
*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "listStructs.h"
#include "admin.h"
#include "client.h"

//functie utilizata la alocarea sublistelor
LSC* allocLSC(int nr)
{
	int i;
	LSC *aux, *L = (LSC*)malloc(sizeof(LSC)), *r = L;

	if(!L) return NULL;

	L->firstCard = NULL;

	for(i = 0; i < nr - 1; i++)
	{
		aux = (LSC*)malloc(sizeof(LSC));
		if(!aux) return NULL;
		aux->firstCard = NULL;
		L->urm = aux;
		L = aux;
	}

	L->urm = NULL;

	return r;

}

//functie ce verifica existenta in lista a cardului ce urmeaza a fi adaugat
int checkCard(LSC* L, int nr, unsigned long long int cardNumber)
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
				return 1;
			}

			card = card->urm;

		}

		r = r->urm;

	}

	return 0;
}

int main()
{
	FILE *input, *output;
	input = fopen("input.in", "r");
	output = fopen("output.out", "w+");

	int nrMaxCard, PIN, CVV, sum, *cardDeleted, cardCount = 0;
	unsigned long long int cardNumber, cardNumberDest;
	char action[20], expiryDate[5], nrPIN[10], sc;

	fscanf(input, "%d\n", &nrMaxCard);

	cardDeleted = calloc(nrMaxCard + 300, sizeof(int));
	LSC* L = allocLSC(nrMaxCard);
	if(!L) return 0;

	while(fscanf(input, "%s%c", action, &sc) != EOF)
	{
		if(strcmp(action, "add_card") == 0)
		{
			fscanf(input, "%llu %d %s %d", &cardNumber, &PIN, expiryDate, &CVV);

			if(!checkCard(L, nrMaxCard, cardNumber))
			{
				if(cardCount == nrMaxCard)
				{
					nrMaxCard *= 2;
					printf("%d\n", nrMaxCard);
					L = resizeLC(L, nrMaxCard);

				}
				cardCount++;

			}

			addCard(L, nrMaxCard, cardNumber, PIN, expiryDate, CVV, output);
			
		}
		if(strcmp(action, "delete_card") == 0)
		{
			cardCount--;
			fscanf(input, "%llu", &cardNumber);
			deleteCard(L, cardNumber, cardDeleted);
		}
		if(strcmp(action, "unblock_card") == 0)
		{
			fscanf(input, "%llu", &cardNumber);
			unblockCard(L, cardNumber);
		}
		if(strcmp(action, "show") == 0)
		{
			fscanf(input, "%llu", &cardNumber);
			if(sc == ' ')
			{
				show(L, cardNumber, output);				
			}
			else
			{
				showAll(L, output, cardDeleted);
			}

		}
		if(strcmp(action, "insert_card") == 0)
		{
			fscanf(input, "%llu %s", &cardNumber, nrPIN);
			insertCard(L, cardNumber, nrPIN, output);
		}
		if(strcmp(action, "recharge") == 0)
		{
			fscanf(input, "%llu %d", &cardNumber, &sum);
			recharge(L, cardNumber, sum, output);
		}
		if(strcmp(action, "cash_withdrawal") == 0)
		{
			fscanf(input, "%llu %d", &cardNumber, &sum);
			cashWithdrawal(L, cardNumber, sum, output);
		}
		if(strcmp(action, "balance_inquiry") == 0)
		{
			fscanf(input, "%llu", &cardNumber);
			balanceInquiry(L, cardNumber, output);
		}
		if(strcmp(action, "transfer_funds") == 0)
		{
			fscanf(input, "%llu %llu %d", &cardNumber, &cardNumberDest, &sum);
			transferFunds(L, cardNumber, cardNumberDest, sum, output);
		}
		if(strcmp(action, "reverse_transaction") == 0)
		{
			fscanf(input, "%llu %llu %d", &cardNumber, &cardNumberDest, &sum);
			reverseTransaction(L, cardNumber, cardNumberDest, sum, output);
		}
		if(strcmp(action, "pin_change") == 0)
		{
			fscanf(input, "%llu %s", &cardNumber, nrPIN);
			pinChange(L, cardNumber, nrPIN, output);
		}
		if(strcmp(action, "cancel") == 0)
		{
			fscanf(input, "%llu", &cardNumber);
			cancel(L, cardNumber);
		}

	}


	free(cardDeleted);
	freeLSC(L, nrMaxCard);
	fclose(input);
	fclose(output);
	return 0;
}