#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

//Függvények
void init(char *arg);
void uj_szallitmany();
void listazas(char *arg);
void lisazas_szolotermo_videk_szerint(char *arg);
void adatmodositas(char *arg);
void adattorles(char *arg);
//Segédfüggvények
char ***listazas_soronkent(char *arg);
int id_lokalizalas(char *arg, int id);
int menu(int max_menupont);
//globális változó
int max_rec_id = 0;
int rec_count = 0;
int main_menupont = 10;

//main
//Paraméterek: fájl név
//Funkció: fő funkciók meghívása
int main(int argc, char **argv)
{
    
    init(argv[1]);
    while (main_menupont != 0)
    {
        printf("\n\n-.-.-.-.-.-.-.-.-.-.-Főmenü-.-.-.-.-.-.-.-.-.-\n");
        printf("Kerem valasszon a menupontok kozul:\n0.Kilepes\n1.uj szallitmany rogzitese\n2.listazas\n3.listazas-videk szerint\n4.adatmodositas\n5.adat torles\n");
        main_menupont = menu(5);
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
            adatmodositas(argv[1]);
            break;
        case 5:
            adattorles(argv[1]);
            break;
        }
    }
}

void init(char *arg)
{
    char dummy1[50] = "Tokaji, Bela, 20, Furmint, 1\n";
    max_rec_id++;
    rec_count++;
    char dummy2[50] = "Egri, Janos, 100, Harslevelu, 2\n";
    max_rec_id++;
    rec_count++;
    int g;
    g = open(arg, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    write(g, dummy1, strlen(dummy1));
    write(g, dummy2, strlen(dummy2));
    close(g);
}

void uj_szallitmany(char *arg)
{
    printf("\n\n-.-.-.-.-.-.-.-.-.-.-Új szállítmány-.-.-.-.-.-.-.-.-.-\n");
    max_rec_id++;
    rec_count++;
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
    printf("-----------------\nMegadott adatok:\n-Szolotermelo videk: %s\n-Termelo neve: %s\n-Atvett mennyiseg: %d\n-Szolofajta: %s\n-----------------\n", szolotermo_videk_str, nev, mennyiseg, szolofajta);
    printf("File-ba iras\n");
    char mennyiseg_str[4];
    char max_rec_id_str[6];
    sprintf(mennyiseg_str, "%d", mennyiseg);
    sprintf(max_rec_id_str, "%d", max_rec_id);

    int g;
    g = open(arg, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    write(g, szolotermo_videk_str, strlen(szolotermo_videk_str));
    write(g, ", ", strlen(", "));
    write(g, nev, strlen(nev));
    write(g, ", ", strlen(", "));
    write(g, mennyiseg_str, strlen(mennyiseg_str));
    write(g, ", ", strlen(", "));
    write(g, szolofajta, strlen(szolofajta));
    write(g, ", ", strlen(", "));
    write(g, max_rec_id_str, strlen(max_rec_id_str));
    write(g, "\n", strlen("\n"));
    close(g);
}


void listazas(char *arg)
{
    printf("\n\n-.-.-.-.-.-.-.-.-.-.-Listázás-.-.-.-.-.-.-.-.-.-\n");
    int g;
    g = open(arg, O_RDONLY);
    char c;
    while (read(g, &c, sizeof(c)))
    {
        printf("%c", c);
    }
}

void lisazas_szolotermo_videk_szerint(char *arg)
{
    printf("\n\n-.-.-.-.-.-.-.-.-.-.-Listázás területenként-.-.-.-.-.-.-.-.-.-\n");
    char ***eredmeny = listazas_soronkent(arg);
    char szolotermo_videk[10];
    int i;
    printf("Kerem adja meg a szolotermo videk nevet: ");
    scanf("%s", szolotermo_videk);
    while (fgetc(stdin) != '\n');
    printf("\nSzállítmányok a(z) %s borvidékről:\n", szolotermo_videk);
    for(i=0; i<rec_count; i++)
    {
        if (strcmp(eredmeny[i][0], szolotermo_videk) == 0)
        {
            printf("%s, %s, %s, %s, %s\n", eredmeny[i][0], eredmeny[i][1], eredmeny[i][2], eredmeny[i][3], eredmeny[i][4]);
        }
    }
}

void adatmodositas(char *arg)
{
    printf("\n\n-.-.-.-.-.-.-.-.-.-.-Adatmódosítás-.-.-.-.-.-.-.-.-.-\n");
    char ***eredmeny = listazas_soronkent(arg);
    int id;
    int id_loc;
    char uj_ertek[100];
    printf("Kérem adja meg az id-t: ");
    scanf("%d", &id);
    while (fgetc(stdin) != '\n')
        ;
    id_loc = id_lokalizalas(arg, id);
    printf("\nA modosítani kívánt rekord: %s, %s, %s, %s, %s\n", eredmeny[id_loc][0], eredmeny[id_loc][1], eredmeny[id_loc][2], eredmeny[id_loc][3], eredmeny[id_loc][4]);
    printf("Kérem adja meg a módosítani kívánt adatot:\n1.Szolotermelo videk\n2.Termelo neve:\n3.Atvett mennyiseg\n4.Szolofajta\n");
    int menupont = menu(4);
    printf("menupont: %d\n", menupont);
    printf("Kerem adja meg az erteket: ");
    scanf("%s", uj_ertek);
    while (fgetc(stdin) != '\n')
        ;
    switch (menupont)
    {
    case 1:
        eredmeny[id_loc][0] = uj_ertek;
        break;
    case 2:
        eredmeny[id_loc][1] = uj_ertek;
        break;
    case 3:
        eredmeny[id_loc][2] = uj_ertek;
        break;
    case 4:
        eredmeny[id_loc][3] = uj_ertek;
        break;
    }
    printf("\nA módosított rekord: %s, %s, %s, %s, %s\n", eredmeny[id_loc][0], eredmeny[id_loc][1], eredmeny[id_loc][2], eredmeny[id_loc][3], eredmeny[id_loc][4]);

    int g;
    g = open(arg, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    int i;
    for(i=0; i<rec_count; i++)
    {
        write(g, eredmeny[i][0], strlen(eredmeny[i][0]));
        write(g, ", ", strlen(", "));
        write(g, eredmeny[i][1], strlen(eredmeny[i][1]));
        write(g, ", ", strlen(", "));
        write(g, eredmeny[i][2], strlen(eredmeny[i][2]));
        write(g, ", ", strlen(", "));
        write(g, eredmeny[i][3], strlen(eredmeny[i][3]));
        write(g, ", ", strlen(", "));
        write(g, eredmeny[i][4], strlen(eredmeny[i][4]));
        write(g, "\n", strlen("\n"));
    }
    close(g);
}

void adattorles(char *arg)
{
    printf("\n\n-.-.-.-.-.-.-.-.-.-.-Törlés-.-.-.-.-.-.-.-.-.-\n");
    char ***eredmeny = listazas_soronkent(arg);
    int id;
    int id_loc;
    printf("Kérem adja meg az id-t: ");
    scanf("%d", &id);
    while (fgetc(stdin) != '\n')
        ;
    id_loc = id_lokalizalas(arg, id);
    printf("\nA törölni kívánt rekord: %s, %s, %s, %s, %s\n", eredmeny[id_loc][0], eredmeny[id_loc][1], eredmeny[id_loc][2], eredmeny[id_loc][3], eredmeny[id_loc][4]);
    int g;
    g = open(arg, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    int i;
    for(i=0; i<rec_count; i++)
    {
        if (atoi(eredmeny[i][4]) != id)
        {
            write(g, eredmeny[i][0], strlen(eredmeny[i][0]));
            write(g, ", ", strlen(", "));
            write(g, eredmeny[i][1], strlen(eredmeny[i][1]));
            write(g, ", ", strlen(", "));
            write(g, eredmeny[i][2], strlen(eredmeny[i][2]));
            write(g, ", ", strlen(", "));
            write(g, eredmeny[i][3], strlen(eredmeny[i][3]));
            write(g, ", ", strlen(", "));
            write(g, eredmeny[i][4], strlen(eredmeny[i][4]));
            write(g, "\n", strlen("\n"));
        }
    }
    printf("Törles sikeres");
    rec_count--;
    close(g);
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
            arr[sorok] = malloc(5 * sizeof(char *));
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

int id_lokalizalas(char *arg, int id)
{
    char ***eredmeny = listazas_soronkent(arg);
    int i;
    int loc = -1;
    for(i=0; i<rec_count; i++)
    {
        if (atoi(eredmeny[i][4]) == id)
        {
            loc = i;
        }
    }
    return loc;
}

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