#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

enum Feldart{A,B,C,D};
typedef enum Feldart feld;

/* Die möglichen Werte von feld entsprechen folgenden Zuständen:
 * A: Feld unbesetzt, unbeschossen
 * B: Feld besetzt, unbeschossen
 * C: Feld unbesetzt, beschossen
 * D: Feld besetzt, beschossen
*/

void _fehler(bool x)
{
	/* Dies ist eine Fehlerfunktion, die gegebenenfalls bei einer 
	 * ungültigen Eingabe übrige Zeichen aus dem Terminal entfernt.
	 * Es wird jedoch immer auf dem Terminal ausgegeben, dass
	 * eine ungültige Eingabe geschehen ist.
	 * Parameter x regelt, ob das Terminal geleert werden soll.
	*/

	if(x)
	{	
		char *clear = malloc(1000 * sizeof(char));		
		scanf("%s",clear);
		free(clear);
	}
	printf("Bitte gültige Eingabe vornehmen\n\n");
}

bool _waagrecht(feld spielfeld[10][10], int x,int y, int l)
{
	/* _waagrecht und _senkrecht sind Funktionen, die das jeweilige
	 * Feld in der entsprechenden Richtung auffüllen, wenn dies möglich ist.
	 * Erfolg oder Fehlschlag werden über den Rückgabewert vermittelt.
	 * Parameter x,y bilden den Startpunkt, l entspricht der Länge des zu 
	 * setzenden Schiffes
	*/

	for(int i=0; i<l; i++)
	{
	//Prüft, ob ein Feld bereits besetzt ist oder man darüber hinaussetzt
		if(spielfeld[x+i][y]==B || (x+l)>9) 
		{
			_fehler(false);
			return false;
		}
	}
//Setzen des Schiffes
	for(int i=0; i<l; i++)
	{
		spielfeld[x+i][y]=B;
	}
	return true;
}

//_senkrecht funktioniert vollkommen analog zu _waagrecht
bool _senkrecht(feld spielfeld[10][10], int x,int y, int l)
{
	for(int i=0; i<l; i++)
	{
		if(spielfeld[x][y+i]==B || (y+l)>9)
		{
			_fehler(false);
			return false;
		}
	}
	for(int i=0; i<l; i++)
	{
		spielfeld[x][y+i]=B;
	}
	return true;
}


bool _schiffeSetzen(feld spielfeld[10][10], int nummer)
{
	/* Diese Funktion verwaltet Nutzereingaben zum Setzen der Schiffe.
	 * Sind diese ungültig, wird dies mittels _fehler ausgegeben und die
	 * Funktion übermittelt dem Programm den Fehlschlag analog _waagrecht.
	 * Hat die Eingabe das richtige Format, so wird je nach angegebener
	 * Richtung eine der Funktionen _waagrecht oder _senkrecht aufgerufen.
	 * Parameter nummer regelt, welches Schiff gerade gesetzt werden soll.
	*/

	char schiff[15];
	int laenge;
	char z;
	int spalte;
	char richtung;

	if(nummer==10) 
	{
		sscanf("Schlachtschiff","%14s",schiff);
		laenge = 5;
	}
	if(nummer==9 || nummer==8) 
	{
		sscanf("Kreuzer","%7s",schiff);
		laenge = 4;
	}
	if(nummer<8 && nummer>4) 
	{
		sscanf("Zerstörer","%10s",schiff);
		laenge = 3;
	}
	if(nummer<5)
	{
		sscanf("U-Boot","%6s",schiff);
		laenge = 2;
	}

	printf("Bitte Position und Richtung eingeben\n");
	printf("Schiffsart: %s, Länge: %d\n",schiff,laenge);
	scanf(" %c %d %c",&z,&spalte,&richtung);
	int zeile = z-64;	//Transformation von Zeilenname in Nummer
//if-Abfrage prüft, ob die Eingabe gültig war
	if(zeile<1 || zeile>10 || spalte<1 || spalte>10 || (richtung!='w' && richtung !='s'))
	{
		_fehler(true);
		return false;
	}
//switch-Anweisung regelt die Auffüllrichtung
	switch (richtung)
	{
		case 'w': 
		return _waagrecht(spielfeld,spalte-1,zeile-1,laenge);

		case 's':
		return _senkrecht(spielfeld,spalte-1,zeile-1,laenge);
	}
	return true;
/* Das letzte return true; dient dazu, eine Compiler-Warnung zu
 * unterdrücken, die auftritt, da alle anderen return-Anweisungen in
 * Verzweigungen stehen. Die Natur dieser Verzweigungen stellt allerdings
 * sicher, dass diese Zeile ohnehin nie erreicht wird.
*/
}

void schiffeSetzen(feld player[10][10])
{
	/* schiffeSetzen nimmt als Parameter das Spielfeld des jeweiligen
	 * Spielers und ruft die Funktion _schiffeSetzen auf. Dies wird solange
	 * gemacht, bis alle Schiffe des Spielers erfolgreich gesetzt wurden.
	*/
	int n = 10;
	while(n>0) //läuft, solange Schiffe zu setzen sind, also n>0 ist.
	{
		if(_schiffeSetzen(player,n)) n--;
	}
}

void schuss(){}
void feldAusgeben(){}

void anleitung()
{
	/* Mein etwas unbeholfener Versuch, vor dem Spiel eine Anleitung
	 * auszugeben. Das Format, in dem Eingaben zu machen sind, wird 
	 * erklärt, zudem muss bestätigt werden, dass die Anleitung
	 * gelesen wurde, damit das Programm solange pausiert.
	*/

	printf("Dies ist ein 'Schiffe versenken'-Spiel für zwei Spieler. Zu Anfang des Spiels werden Sie aufgefordert werden, Ihre Schiffe zu setzen, indem Sie Position und Richtung angeben. Nutzen Sie dazu folgendes Format:\n\n");
	printf("A3 w bzw B5 s\n\n");
	printf("w und s bedeuten hierbei waagrecht und senkrecht. Während des Spiels ist die Position für Schüsse ebenso anzugeben. Andere Formate führen zu Fehlern und Sie werden dazu aufgefordert,eine gültige Position einzugeben\n");
	printf("Wenn Sie dies verstanden haben,bestätigen Sie mit Enter\n");
//Einfachste Methode, die mir eingefallen ist, eine Eingabe zu erzwingen	
	char *c = malloc(1 * sizeof(char));
	scanf("%c",c);
	free(c);
}

int main(void)
{
	feld spieler1[10][10];		//Initialisieren der unbesetzten Felder
	feld spieler2[10][10];
	for(int i=0; i<10; i++)
	{
		for(int j=0; j<10; j++)
		{
			spieler1[i][j]=A;
			spieler2[i][j]=A; 
		}
	}
	
	int anzahlBoote1 = 10;
	int anzahlBoote2 = 10;

	anleitung();
	schiffeSetzen(spieler1);
	schiffeSetzen(spieler2);
}
