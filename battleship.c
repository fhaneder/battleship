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

struct schiff {
	int length;
	int hitpoints;
	int startspalte;
	int startzeile;
	bool richtung;	//true steht für waagrecht, false für senkrecht
	char name[15];	//"Schlachtschiff" ist der längste Name mit 14 Zeichen 
};

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

void _einFeldAusgeben(feld ausgabefeld[10][10])
{
	/* Gibt nur das Feld des aktuellen Spielers aus. Nur nötig zum Schiffe
	 * setzen, daher werden nur die Fälle A und B abgefragt.
	*/

	for(int i=0; i<100; i++) printf("\n");
	for(int i=0; i<11; i++)
	{
		printf("%d  ",i);
	}
	printf("\n");
	for(int i=0; i<10; i++)
	{
		printf("%c  ",i+65);
		for(int j=0; j<10; j++)
		{
			switch(ausgabefeld[i][j])
			{
				case C:
				case D:
				case A: printf("O  ");break;
				case B: printf("|  ");break;
			}
		}
		printf("\n");
	}
}


bool _waagrecht(feld spielfeld[10][10], int x,int y, struct schiff *sptr)
{
	/* _waagrecht und _senkrecht sind Funktionen, die das jeweilige
	 * Feld in der entsprechenden Richtung auffüllen, wenn dies möglich ist.
	 * Erfolg oder Fehlschlag werden über den Rückgabewert vermittelt.
	 * Parameter x,y bilden den Startpunkt, *sptr überträgt die Position und
	 * Richtung an das aktuelle Schiff.
	*/

	struct schiff s=*sptr;
	for(int i=0; i<s.length; i++)
	{
	//Prüft, ob ein Feld bereits besetzt ist oder man darüber hinaussetzt
		if(spielfeld[y][x+i]==B || (x+s.length)>9) 
		{
			_fehler(false);
			return false;
		}
	}
//Setzen des Schiffes
	for(int i=0; i<s.length; i++)
	{
		spielfeld[y][x+i]=B;
	}
	s.startspalte=x;
	s.startzeile=y;
	s.richtung=true;
	return true;
}

//_senkrecht funktioniert vollkommen analog zu _waagrecht
bool _senkrecht(feld spielfeld[10][10], int x,int y, struct schiff *sptr)
{
	struct schiff s=*sptr;
	for(int i=0; i<s.length; i++)
	{
		if(spielfeld[y+i][x]==B || (y+s.length)>9)
		{
			_fehler(false);
			return false;
		}
	}
	for(int i=0; i<s.length; i++)
	{
		spielfeld[y+i][x]=B;
	}
	s.startspalte=x;
	s.startzeile=y;
	s.richtung=false;
	return true;
}


bool _schiffeSetzen(feld spielfeld[10][10], struct schiff *cs)
{
	/* Diese Funktion verwaltet Nutzereingaben zum Setzen der Schiffe.
	 * Sind diese ungültig, wird dies mittels _fehler ausgegeben und die
	 * Funktion übermittelt dem Programm den Fehlschlag analog _waagrecht.
	 * Hat die Eingabe das richtige Format, so wird je nach angegebener
	 * Richtung eine der Funktionen _waagrecht oder _senkrecht aufgerufen.
	 * Das Array speichert die Positionen aller gesetzten Schiffe.
	 * Der Zeiger auf ein Schiff liefert Informationen über dieses und
	 * speichert den Anfangspunkt, sowie die Richtung.
	*/

	char z;
	int spalte;
	char richtung;
	struct schiff currentShip=*cs;

	printf("Bitte Position und Richtung eingeben\n");
	printf("Schiffsart: %s, Länge: %d\n",currentShip.name,currentShip.length);
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
			return _waagrecht(spielfeld,spalte-1,zeile-1,cs);

		case 's':
			return _senkrecht(spielfeld,spalte-1,zeile-1,cs);
	}
	return true;
/* Das letzte return true; dient dazu, eine Compiler-Warnung zu
 * unterdrücken, die auftritt, da alle anderen return-Anweisungen in
 * Verzweigungen stehen. Die Natur dieser Verzweigungen stellt allerdings
 * sicher, dass diese Zeile ohnehin nie erreicht wird.
*/
}

void _welchesSchiff(int x, int y, struct schiff tf[10])
{
	/* _welchesSchiff dient dazu, festzustellen, welches Schiff getroffen
	 * wurde. Dazu werden die Koordinaten (x,y) des beschossenen Feldes mit
	 * denen der Schiffe verglichen. Stimmen die Koordinaten überein, so
	 * werden die Hitpoints des betreffenden Schiffes um 1 erniedrigt.
	 * Gegebenenfalls wird ausgegeben, dass ein Schiff versenkt wurde.
	*/
	int k = -1;
	for(int i=0; i<10; i++)
	{
		for(int j=0; j<tf[i].length; j++)
		{
			if(tf[i].richtung)	//Prüfung waagrechter Schiffe
			{
				if((tf[i].startspalte+j)==x && tf[i].startzeile==y)
				{
					tf[i].hitpoints--;
					k = i;
					goto ende;
//Stimmen die Koordinaten überein, kann die Prüfung abgebrochen werden.
				}
			}
			if(!tf[i].richtung)	//Prüfung senkrechter Schiffe
			{
				if(tf[i].startspalte==x && (tf[i].startzeile+j)==y)
				{
					tf[i].hitpoints--;
					k = i;
					goto ende;
				}
			}
		}
	}

	ende:
	if(!tf[k].hitpoints)
	{
		printf("Versenkt!\n");
	}
}

void schiffeSetzen(feld player[10][10], struct schiff fleet[10])
{
	/* schiffeSetzen nimmt als Parameter das Spielfeld des jeweiligen
	 * Spielers, sowie dessen Flotte und ruft die Funktion _schiffeSetzen
	 * auf. Dies wird solange gemacht, bis alle Schiffe des Spielers
	 * erfolgreich gesetzt wurden.
	*/
	int n = 0;
	while(n<10) //läuft, solange Schiffe zu setzen sind, also n>0 ist.
	{
		_einFeldAusgeben(player);
		if(_schiffeSetzen(player,&fleet[n])) n++;
	}
}

bool schuss(feld zielfeld[10][10], struct schiff zielflotte[10])
{
	/* Nimmt als Parameter Feld und Flotte des beschossenen Spielers.
	 * Nutzereingabe bestimmt das zu beschießende Feld, mithilfe des
	 * selbst definierten Datentyps feld wird festgestellt, ob der Schuss
	 * zulässig und erfolgreich war.
	*/
	char z;
	int spalte;
	printf("Bitte Zielfeld angeben\n");
	scanf(" %c %d",&z,&spalte);
	int zeile = z-64;
	if(zeile>10 || zeile<1 || spalte>10 || spalte<1)
	{
		_fehler(true);
		return false;
	}
	switch(zielfeld[spalte-1][zeile-1])
	{
		case D:
		case C:
			printf("Feld bereits beschossen!\n");
			printf("Versuch's nochmal\n");
			return false;	//false bedeutet einen ungültigen Schuss
		case B:
			zielfeld[spalte-1][zeile-1]=D;
			printf("Treffer\n");
			_welchesSchiff(spalte-1,zeile-1,zielflotte);
			printf("Geht das nochmal?\n");
			return schuss(zielfeld,zielflotte);
//Treffer bedeutet, dass nochmal geschossen werden darf.
		case A:
			zielfeld[spalte-1][zeile-1]=C;
			printf("Wasser\n");
			return true; //true bedeutet einen gültigen Schuss
	}
	return true;	//Dient dem selben Zweck wie in _schiffeSetzen
}

unsigned int fleetCheck(struct schiff target[10])
{
	/* Dient dazu, zu überprüfen, wieviele Schiffe einer Flotte beschädigt
	 * sind. Dies ist wichtig für das Abbruchkriterium des Spiels.
	*/

	unsigned int destroyed=0;
	for(int i=0; i<10; i++)
	{
		if(!target[i].hitpoints) destroyed++;
	}
	return destroyed;
}

void feldAusgeben(feld spieler[10][10], feld gegner[10][10])
{
	/* Gibt links das Feld des aktiven Spielers, rechts das des Gegners aus.
	 * Zuvor wird das Terminal geleert, damit der Spieler nicht die Schiffe
	 * des Gegners sehen kann.
	*/

	for(int i=0; i<100; i++) printf("\n");
	for(int j=0; j<2; j++)
	{	
		for(int i=0; i<11; i++)
		{
			printf("%d  ",i);
		}
		printf("   ");
	}
	printf("\n");
	for(int i=0; i<10; i++)
	{
		printf("%c  ",i+65);
		for(int j=0; j<10; j++)
		{
			switch(spieler[i][j])
			{
				case A: printf("O  ");break;
				case B: printf("|  ");break;
				case C: printf("O  ");break;
				case D: printf("X  ");break;
			}
		}
		printf("    %c  ",i+65);
		for(int j=0; j<10; j++)
		{
			switch(gegner[i][j])
			{
				case A: printf("O  ");break;
				case B: printf("O  ");break;
				case C: printf("/  ");break;
				case D: printf("X  ");break;
			}
		}
		printf("\n");
	}
}

void anleitung()
{
	/* Mein etwas unbeholfener Versuch, vor dem Spiel eine Anleitung
	 * auszugeben. Das Format, in dem Eingaben zu machen sind, wird 
	 * erklärt, zudem muss bestätigt werden, dass die Anleitung
	 * gelesen wurde, damit das Programm solange pausiert.
	*/

	printf("Dies ist ein 'Schiffe versenken'-Spiel für zwei Spieler. Zu Anfang des Spiels werden Sie aufgefordert werden, Ihre Schiffe zu setzen, indem Sie Position und Richtung angeben. Nutzen Sie dazu folgendes Format:\n\n");
	printf("A3 w bzw B5 s\n\n");
	printf("w und s bedeuten hierbei waagrecht und senkrecht. Der Großbuchstabe gibt die Zeile an, die Ganzzahl die Spalte. Während des Spiels ist die Position für Schüsse ebenso anzugeben. Andere Formate führen zu Fehlern und Sie werden dazu aufgefordert,eine gültige Position einzugeben\n");
	printf("Wenn Sie dies verstanden haben,bestätigen Sie mit Enter\n");
//Einfachste Methode, die mir eingefallen ist, eine Eingabe zu erzwingen	
	char *c = malloc(1000 * sizeof(char));
	scanf("%s",c);
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

	int anzahlSchiffe1 = 10;
	int anzahlSchiffe2 = 10;
	struct schiff flotte1[10];		//Initialisieren der Flotten
	struct schiff flotte2[10];
	flotte1[0].length=5;
	sscanf("Schlachtschiff","%s",&(flotte1[0].name[0]));
	flotte2[0].length=5;
	sscanf("Schlachtschiff","%s",&(flotte2[0].name[0]));
	for(int i=1; i<3; i++)
	{
		flotte1[i].length=4;
		sscanf("Kreuzer","%s",&(flotte1[i].name[0]));
		flotte2[i].length=4;
		sscanf("Kreuzer","%s",&(flotte2[i].name[0]));
	}
	for(int i=3; i<6; i++)
	{
		flotte1[i].length=3;
		sscanf("Zerstörer","%s",&(flotte1[i].name[0]));
		flotte2[i].length=3;
		sscanf("Zerstörer","%s",&(flotte2[i].name[0]));
	}
	for(int i=6; i<10; i++)
	{
		flotte1[i].length=2;
		sscanf("U-Boot","%s",&(flotte1[i].name[0]));
		flotte2[i].length=2;
		sscanf("U-Boot","%s",&(flotte2[i].name[0]));
	}
	for(int i=0; i<10; i++)
	{
		flotte1[i].hitpoints=flotte1[i].length;
		flotte2[i].hitpoints=flotte2[i].length;
	}

/* Hier beginnt das tatsächliche Spiel. Zu Beginn wird eine Anleitung
 * ausgegeben, dann die Schiffe gesetzt. Ist dies geschehen, so wird
 * abwechselnd geschossen, außer ein Spieler landet einen Treffer.
 * Hat ein Spieler keine Schiffe mehr, so hat der andere gewonnen.
*/

	anleitung();
	printf("Spieler 1\n");
	schiffeSetzen(spieler1,flotte1);
	printf("Spieler 2\n");
	schiffeSetzen(spieler2,flotte2);
	bool legalShot;
	while(anzahlSchiffe1 && anzahlSchiffe2)
	{
		/* Die goto-Anweisungen sind hier nötig, um an zwei verschiedene
		 * Stellen der Schleife springen zu können.
		*/

		failedShot1:
		printf("Spieler 1\n");
		feldAusgeben(spieler1,spieler2);
		legalShot=schuss(spieler2,flotte2);
		if(!legalShot) goto failedShot1;
		anzahlSchiffe2=10-fleetCheck(flotte2);

		failedShot2:
		printf("Spieler 2\n");
		feldAusgeben(spieler2,spieler1);
		legalShot=schuss(spieler1,flotte1);
		if(!legalShot) goto failedShot2;
		anzahlSchiffe1=10-fleetCheck(flotte1);
	}
	
	if(!anzahlSchiffe2) printf("Spieler 1 gewinnt!\n");
	if(!anzahlSchiffe1) printf("Spieler 2 gewinnt!\n");
}
