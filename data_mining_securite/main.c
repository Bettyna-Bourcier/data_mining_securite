#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define _CRT_SECURE_NO_DEPRECATE

// Calcul de la moyenne
// unsigned char * tableau_fichier => tableau d' octets/char représentant le fichier
// taille_fichier => nombre d'octets
float calcul_moyenne(unsigned char * tableau_fichier, long taille_fichier) {
	float moyenne = 0;
	unsigned int somme = 0; // somme de tous les octets
	int i = 0;

	// Calcul de la moyenne
	for (i = 0; i <= taille_fichier; i++) {
		somme = somme + tableau_fichier[i];
	}
	moyenne = somme / (float)(taille_fichier + 1);
	return moyenne;
}


// Calcul de l'écart-type
// unsigned char * tableau_fichier => tableau d' octets/char représentant le fichier
// taille_fichier => nombre d'octets
// moyenne => moyenne de taleau_fichier
float calcul_ecart_type(unsigned char * tableau_fichier, long taille_fichier, float moyenne) {

	float ecart_type = 0;
	int  i = 0;
	float variance = 0;
	float somme = 0;

	for (i = 0; i <= taille_fichier; i++) {
		somme = somme + pow(tableau_fichier[i] - moyenne, 2);
	}

	variance = somme / (taille_fichier + 1);
	ecart_type = sqrt(variance);
	 
	return ecart_type;
}


int main(int argc, char *argv[])
{
	FILE *fichier;
	unsigned char *tableau_fichier;
	long taille_fichier;
	float moyenne = 0;
	float ecart_type = 0;
	char chemin[] = "C:\\Users\\Bettyna\\Documents\\ESIEA_4A\\S2\\securite_virologie\\projet_final\\ensemble_A\\et_wikipedia_org_wiki_Kriket.7z";

	fichier = fopen(chemin, "rb"); // ouverture du fichier en mode binaire
	fseek(fichier, 0, SEEK_END);   // place le ponteur à la fin du fichier
	taille_fichier = ftell(fichier); // position du pointeur => taille du fichier
	rewind(fichier); // place le pointeur au début du fichier                   

	tableau_fichier = (char *)malloc((taille_fichier + 1)*sizeof(char)); // créer un tablau pour acceuillir les valeurs des octets
	fread(tableau_fichier, taille_fichier, 1, fichier); // lit le contenu du fichier octet par octet et place les valeurs dans "tableau_fichier"
	fclose(fichier); // fermeture du fichier

	moyenne = calcul_moyenne(tableau_fichier, taille_fichier);
	printf("Moyenne : %f \n", moyenne);

	ecart_type = calcul_ecart_type(tableau_fichier, taille_fichier, moyenne);
	printf("Ecart type : %f \n", ecart_type);

	getchar();
	return EXIT_SUCCESS;
}
