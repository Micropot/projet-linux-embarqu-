#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define tref 333 / 1.5 //=222 permet de corriger l'erreur
#define tref3 445
#define tref32 10656

FILE *f;
char etat_tube_rouge = '0';
char etat_tube_vert = '0';
char etat_tube_bleu = '0';

void allume_LED(int couleur)
{
    FILE *f = NULL;
    switch (couleur)
    {
    case 3:
        f = fopen("/sys/class/gpio/gpio3/value", "w");
        if (f != NULL)
        {
            fprintf(f, "1");
            fflush(f);
            fclose(f);
        }
        break;

    case 50:
        f = fopen("/sys/class/gpio/gpio50/value", "w");
        if (f != NULL)
        {
            fprintf(f, "1");
            fflush(f);
            fclose(f);
        }
        break;

    case 60:
        f = fopen("/sys/class/gpio/gpio60/value", "w");
        if (f != NULL)
        {
            fprintf(f, "1");
            fflush(f);
            fclose(f);
        }
        break;
    }
}

void eteint_LED(int couleur)
{
    FILE *f = NULL;
    switch (couleur)
    {
    case 3:
        f = fopen("/sys/class/gpio/gpio3/value", "w");
        if (f != NULL)
        {
            fprintf(f, "0");
            fflush(f);
            fclose(f);
        }
        break;

    case 50:
        f = fopen("/sys/class/gpio/gpio50/value", "w");
        if (f != NULL)
        {
            fprintf(f, "0");
            fflush(f);
            fclose(f);
        }
        break;

    case 60:
        f = fopen("/sys/class/gpio/gpio60/value", "w");
        if (f != NULL)
        {
            fprintf(f, "0");
            fflush(f);
            fclose(f);
        }
        break;
    }
}

unsigned char etat_poussoir(void)
{
    FILE *f = NULL;
    char etat;
    f = fopen("/sys/class/gpio/gpio48/value", "r");
    if (f != NULL)
    {
        etat = fgetc(f);
        fclose(f);
    }
    return (etat);
}

void control_RVB(char R, char V, char B)
{
    switch (R)
    {
    case 0:
        eteint_LED(50);
        break;
    case 1:
        allume_LED(50);
        break;
    }
    switch (V)
    {
    case 0:
        eteint_LED(60);
        break;
    case 1:
        allume_LED(60);
        break;
    }
    switch (B)
    {
    case 0:
        eteint_LED(3);
        break;
    case 1:
        allume_LED(3);
        break;
    }
}

unsigned int read_ADC(void)
{
    FILE *f = NULL;
    int valeur;
    f = fopen("/sys/bus/iio/devices/iio:device0/in_voltage3_raw", "r");
    fscanf(f, "%d\n", &valeur);
    fclose(f);

    // printf("valeur : %d \n", valeur);
    return (valeur);
}

void rectangle(void) // a voir pour mettre ca directement dans un main
{
    FILE *f = NULL;
    f = fopen("/sys/class/gpio/gpio51/value", "w");
    // f = fopen("/Users/arthurlamard/Documents/ISEN3/LINUX_EMBARQUE/test.txt", "w");
    if (f != NULL)
    {
        fprintf(f, "0");
        fflush(f);
        fprintf(f, "1");
        fflush(f);

        fclose(f);
    }
}

void GPIO_1to0(int delai1, int delai0)
{
    FILE *f = NULL;
    f = fopen("/sys/class/gpio/gpio51/value", "w");

    if (f != NULL)
    {
        // partie pour le temps haut
        fprintf(f, "1");
        fflush(f);
        usleep(delai1);
        // partie pour le temps bas
        fprintf(f, "0");
        fflush(f);
        usleep((delai0));

        fclose(f);
    }
}

void trans_data_433MHz(char data)
{
    if (data == '0')
    {
        GPIO_1to0(tref, tref3); // T1_T3
        GPIO_1to0(tref, tref3); // T1_T3
    }
    if (data == '1')
    {
        GPIO_1to0(tref, tref3);
        GPIO_1to0(tref3, tref);
    }
    if (data == '2')
    {
        GPIO_1to0(tref3, tref);
        GPIO_1to0(tref3, tref);
    }
    if (data == 'S')
    {
        GPIO_1to0(tref, tref32);
    }
}

void trans_trame_433MHz(char maison, char objet, char activation, int repetition)
{
    int i;
    for (i = 0; i < repetition; i++)
    {
        switch (maison)
        {
        case 'A':
            trans_data_433MHz('0');
            trans_data_433MHz('1');
            trans_data_433MHz('1');
            trans_data_433MHz('1');
            break;

        case 'B':
            trans_data_433MHz('1');
            trans_data_433MHz('0');
            trans_data_433MHz('1');
            trans_data_433MHz('1');
            break;

        case 'C':
            trans_data_433MHz('1');
            trans_data_433MHz('1');
            trans_data_433MHz('0');
            trans_data_433MHz('1');
            break;

        case 'D':
            trans_data_433MHz('1');
            trans_data_433MHz('1');
            trans_data_433MHz('1');
            trans_data_433MHz('0');
            break;
        }

        switch (objet)
        {
        case '1':
            trans_data_433MHz('0');
            trans_data_433MHz('1');
            trans_data_433MHz('1');
            break;

        case '2':
            trans_data_433MHz('1');
            trans_data_433MHz('0');
            trans_data_433MHz('1');
            break;

        case '3':
            trans_data_433MHz('1');
            trans_data_433MHz('1');
            trans_data_433MHz('0');
            break;
        }
        // trirts 7 a 10
        trans_data_433MHz('1');
        trans_data_433MHz('0');
        trans_data_433MHz('0');
        trans_data_433MHz('1');

        switch (activation)
        {
        case '0': // off
            trans_data_433MHz('0');
            break;
        case '1':
            trans_data_433MHz('1');
            break;
        }

        trans_data_433MHz('S'); // fin de la trame
    }
}

void commande_radio(char tube_fluo, char *etat_tube_fluo)
{

    if (tube_fluo == 'R')
    {
        if (*etat_tube_fluo == '0')
        {                          // si le tube est eteint
            *etat_tube_fluo = '1'; // on allume le tube
            printf("le tube rouge est allumé \n");
            trans_trame_433MHz('D', '1', '1', 10);
        }
        else
        {
            *etat_tube_fluo = '0';
            printf("le tube rouge est éteint\n");
            trans_trame_433MHz('D', '1', '0', 10);
        }
    }
    if (tube_fluo == 'V')
    {
        if (*etat_tube_fluo == '0')
        {                          // si le tube est eteint
            *etat_tube_fluo = '1'; // on allume le tube
            printf("le tube vert est allumé \n");
            trans_trame_433MHz('C', '2', '1', 10);
        }
        else
        {
            *etat_tube_fluo = '0';
            printf("le tube vert est éteint\n");
            trans_trame_433MHz('C', '2', '0', 10);
        }
    }
    if (tube_fluo == 'B')
    {
        if (*etat_tube_fluo == '0')
        {                          // si le tube est eteint
            *etat_tube_fluo = '1'; // on allume le tube
            printf("le tube bleu est allumé \n");
            trans_trame_433MHz('B', '3', '1', 10);
        }
        else
        {
            *etat_tube_fluo = '0';
            printf("le tube bleu est éteint\n");
            trans_trame_433MHz('B', '3', '0', 10);
        }
    }
}

void selection()
{

    time_t temps;
    temps = time(NULL);
    time_t temps2;

    // int soustraction;
    // printf("time : %ld\n", temps);

    if (read_ADC() <= 1365)
    {
        control_RVB(1, 0, 0);
    }
    else if (read_ADC() >= 1366 && read_ADC() <= 2730)
    {
        control_RVB(0, 1, 0);
    }
    else if (read_ADC() >= 2731)
    {
        control_RVB(0, 0, 1);
    }

    while (etat_poussoir() == '0')
    {
        temps2 = time(NULL);
        printf("soustraction : %ld\n", (temps2 - temps));
    }

    // printf("temps2 : %ld\n", temps2);

    // if(etat_poussoir() == '0'){
    if ((temps2 == temps))
    {
        if (read_ADC() <= 1365)
        {
            printf("LED ROUGE\n");
            // temps2 = time(NULL);
            // printf("---------------time2 : %ld\n", temps2);
            commande_radio('R', &etat_tube_rouge);
            // printf("etat tube rouge %s\n", &etat_tube_rouge);
            // usleep(100000);
        }
        else if (read_ADC() >= 1366 && read_ADC() <= 2730)
        {

            printf("LED VERTE\n");
            commande_radio('V', &etat_tube_vert);
            // printf("etat tube vert %s\n", &etat_tube_vert);
            // usleep(100000);
        }
        else if (read_ADC() >= 2731)
        {

            printf("LED BLEU\n");
            commande_radio('B', &etat_tube_bleu);
            // printf("etat tube bleu %s\n", &etat_tube_bleu);
            // usleep(100000);
        }
    }

    if ((temps2 - temps) >= 2 && (temps2 - temps) <= 3)
    {
        printf("TOUT EST ALLUME\n");
        trans_trame_433MHz('D', '1', '1', 10);
        trans_trame_433MHz('C', '2', '1', 10);
        trans_trame_433MHz('B', '3', '1', 10);
    }

    if ((temps2 - temps) > 3)
    {
        printf("TOUT EST ETEINT\n");
        trans_trame_433MHz('D', '1', '0', 10);
        trans_trame_433MHz('C', '2', '0', 10);
        trans_trame_433MHz('B', '3', '0', 10);
    }
    //}
}

/*int main(){
while(1){
selection();
}
}*/

int main(int argc, char **argv)
{

    // f = fopen("/sys/class/gpio/gpio51/value", "w");

    // Mode connecté

    // argc = le nombre d'élement de la ligne de commande
    // argv = element sous la forme de tableau
    char maison;
    char objet;
    char activation;
    int repetition;

    if (argc > 1)
    {
        printf("mode connecté\n");
        maison = argv[1][0];
        objet = argv[2][0];
        activation = argv[3][0];
        repetition = atoi(argv[4]); // permet de cast en int le char de argv4

        trans_trame_433MHz(maison, objet, activation, repetition);
    }

    // Mode autonome
    else
    {
        printf("mode autonome\n");
        while (1)
        {
            selection();
        }
    }

    // fclose(f);
}