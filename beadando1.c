#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>     //open,creat
#include <sys/types.h> //open
#include <sys/stat.h>
#include <errno.h> //perror, errno
#include <unistd.h>
#include <string.h>

//-----------------fuggveny fejlécek
int menu(int max_menupont);
void uj_szallitmany();
void init(char *arg);
void listazas(char *arg);
void lisazas_szolotermo_videk_szerint(char *arg);
char ***listazas_soronkent(char *arg);

//-----------------main
//---------------------egy paramétert vár: a file neve
//---------------------meghvívja az egyes funkciókért felelős függvényeket
int main(int argc, char **argv)
{
    init(argv[1]);
    printf("\n-.-.-.-.-.-.Kerem valasszon a menupontok kozul:\n0.Kilepes\n1.uj szallitmany rogzitese\n2.listazas\n3.listazas-videk szerint\n4.adatmodositas\n5.adat torles\n");
    int main_menupont = menu(5);
    switch (main_menupont)
    {
    case 1:
        uj_szallitmany(argv[1]);
        break;
    case 2:
        listazas(argv[1]);
        break;
    case 3:
        lisazas_szolotermo_videk_szerint(argv[1]);
        break;
    case 4:
        //
        break;
    case 5:
        //
        break;
    }
}
//-----------------uj szallitmany
//---------------------adatokat beker
//---------------------adatokat kiir stdout-ra
//---------------------adatokat kiir megadott file-ba
void uj_szallitmany(char *arg)
{
    //---------------------adatokat beker
    printf("Szolotermelo videk neve:\n1.Tokaji\n2.Egri\n3.Balatoni\n");
    int szolotermelo_videk = menu(3);
    char szolotermo_videk_str[9];
    switch (szolotermelo_videk)
    {
    case 1:
        strcpy(szolotermo_videk_str, "Tokaji");
        break;
    case 2:
        strcpy(szolotermo_videk_str, "Egri");
        break;
    case 3:
        strcpy(szolotermo_videk_str, "Balatoni");
        break;
    }
    printf("Termelo neve: ");
    char nev[30];
    scanf("%s", nev);
    while (fgetc(stdin) != '\n')
        ;
    printf("Atvett mennyiseg (kg): ");
    int mennyiseg; // Q: mi volt a baj a double tipussal?
    scanf("%d", &mennyiseg);
    while (fgetc(stdin) != '\n')
        ;
    printf("Szolofajta: ");
    char szolofajta[30];
    scanf("%s", szolofajta);
    while (fgetc(stdin) != '\n')
        ;
    //---------------------adatokat kiir stdout-ra
    printf("-----------------\nMegadott adatok:\n-Szolotermelo videk: %s\n-Termelo neve: %s\n-Atvett mennyiseg: %d\n-Szolofajta: %s\n-----------------\n", szolotermo_videk_str, nev, mennyiseg, szolofajta);
    //---------------------adatokat kiir megadott file-ba
    printf("File-ba iras\n");
    int g;
    char mennyiseg_str[4];
    sprintf(mennyiseg_str, "%d", mennyiseg);
    g = open(arg, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    write(g, szolotermo_videk_str, strlen(szolotermo_videk_str));
    write(g, ", ", strlen(", "));
    write(g, nev, strlen(nev));
    write(g, ", ", strlen(", "));
    write(g, mennyiseg_str, strlen(mennyiseg_str));
    write(g, ", ", strlen(", "));
    write(g, szolofajta, strlen(szolofajta));
    write(g, "\n", strlen("\n"));
    close(g);
}

//-----------------listazas
//---------------------file teljes tartalmat listazza
void listazas(char *arg)
{
    int g;
    g = open(arg, O_RDONLY);
    char c;
    while (read(g, &c, sizeof(c)))
    {
        printf("%c", c);
    }
}

char ***listazas_soronkent(char *arg)
{
    int g;
    g = open(arg, O_RDONLY);
    char c;
    int sorok = 0;
    char ***arr = malloc(sorok * sizeof(char **));
    char line[100];
    char *token;
    strcpy(line, "");
    while (read(g, &c, sizeof(c)))
    {
        if (c != '\n')
        {
            strncat(line, &c, sizeof(c));
        }
        else
        {
            const char delim[2] = ",";
            token = strtok(line, delim);
            int szavak = 0;
            arr = realloc(arr, (sorok + 1) * sizeof(char **));
            arr[sorok] = malloc(4 * sizeof(char *));
            while (token != NULL)
            {
                arr[sorok][szavak] = strdup(token);
                token = strtok(NULL, ",");
                szavak++;
            }
            sorok++;
            strcpy(line, "");
        }
    }
    close(g);
    return arr;
}

void lisazas_szolotermo_videk_szerint(char *arg)
{
    char ***eredmeny = listazas_soronkent(arg);
    char szolotermo_videk[10];
    int i = 0;
    printf("Kerem adja meg a szolotermo videk nevet: ");
    scanf("%s", szolotermo_videk);
    printf("\nSzállítmányok a(z) %s borvidékről:\n", szolotermo_videk);
    while (eredmeny[i] != NULL)
    {
        if (strcmp(eredmeny[i][0], szolotermo_videk) == 0)
        {
            printf("%s, %s, %s, %s\n", eredmeny[i][0], eredmeny[i][1], eredmeny[i][2], eredmeny[i][3]);
        }
        i++;
    }
}

//-----------------init
//---------------------file inicializalasa
void init(char *arg)
{
    char dummy1[50] = "Tokaji, Bela, 20, Furmint\n";
    char dummy2[50] = "Egri, Janos, 100, Harslevelu\n";
    int g;
    g = open(arg, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    write(g, dummy1, strlen(dummy1));
    write(g, dummy2, strlen(dummy2));
    close(g);
}
//-----------------menu
//---------------------a menuvalasztast kezeli
//---------------------egy parametere van: max menupont
//---------------------max menupont felett es 0 alatt ujrakeri a menupontot
int menu(int max_menupont)
{
    printf("\nVálasztása: ");
    int sucess = 0;
    int menupont;
    while (sucess == 0 | menupont<0 | menupont> max_menupont)
    {
        sucess = scanf("%d", &menupont);
        while (fgetc(stdin) != '\n')
            ;
        if (sucess == 0 | menupont<0 | menupont> max_menupont)
        {
            printf("Nem letezo menupont, probalja ujra\nVálasztása: ");
        }
    }
    return menupont;
}