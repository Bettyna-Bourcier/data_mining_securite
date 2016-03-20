#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <Windows.h>

#define _CRT_SECURE_NO_DEPRECATE

// #define Chemin_dossier L"C:\\Users\\Bettyna\\Documents\\ESIEA_4A\\S2\\securite_virologie\\projet_final\\ensemble_A_solution\\wikipedia_clair_2"
#define Chemin_dossier L"C:\\Users\\Bettyna\\Documents\\ESIEA_4A\\S2\\securite_virologie\\projet_final\\ensemble_A_tier"
#define Nom_fichier_moyenne_ecart_type "data.csv"
#define Nom_fichier_resultat_kmeans "resultat_kmeans.csv"
#define Nombre_de_classes 3

typedef struct csv_ligne csv_ligne;

struct csv_ligne
{
	char nom_fichier[1024];//chemin du fichier
	float moyenne;
	float ecart_type;
	int indice_classe;
};

typedef struct centre centre;
struct centre {
	int x;
	int y;
};

// Calcul de la moyenne
// unsigned char * tableau_fichier => tableau d' octets/char représentant le fichier
// taille_fichier => nombre d'octets
float calcul_moyenne(unsigned char * tableau_fichier, long taille_fichier) {
	float moyenne = 0;
	unsigned int somme = 0; // somme de tous les octets
	int i = 0;

	// Calcul de la moyenne
	for (i = 0; i <= taille_fichier; i++) {
		somme += tableau_fichier[i];
	}

	moyenne = somme / (float)(taille_fichier + 1);

	return moyenne;
}


// Calcul de l'écart-type
// unsigned char *tableau_fichier => tableau d'octets/char représentant le fichier
// taille_fichier => nombre d'octets
// moyenne => moyenne de taleau_fichier
float calcul_ecart_type(unsigned char *tableau_fichier, long taille_fichier, float moyenne) {
	float ecart_type = 0;
	int  i;
	float variance;
	float somme = 0;

	for (i = 0; i <= taille_fichier; i++) {
		somme = somme + pow(tableau_fichier[i] - moyenne, 2);
	}

	variance = somme / (taille_fichier + 1);
	ecart_type = sqrt(variance);
	 
	return ecart_type;
}


// Execute la fonction f pour chaque fichier du dossier 
int parcourt_dossier(wchar_t *sDir, void (*f)(char*, FILE*)) {
	WIN32_FIND_DATA fdFile; 
	HANDLE hFind = NULL;
	FILE* csv_file = NULL;
	wchar_t sPath[2048];
	char chemin[1024];
	unsigned int nb_fichiers = 0;

	wsprintf(sPath, L"%s\\*.*", sDir); // *.* prend tous les fichiers du dossier

	// Si le dossier n'existe pas...
	if ((hFind = FindFirstFile(sPath, &fdFile)) == INVALID_HANDLE_VALUE) {
		wprintf(L"Le chemin spécifié n'existe pas : [%s]\n", sDir);
	}
	else {
		csv_file = fopen(Nom_fichier_moyenne_ecart_type, "w+");
		do {
			// ignore dossier courant (".") et dossier parent ("..")
			if (strcmp(fdFile.cFileName, ".") != 0 && strcmp(fdFile.cFileName, "..") != 0) {

				// construction du chemin du ficher
				wsprintf(sPath, L"%s\\%s", sDir, fdFile.cFileName);

				nb_fichiers++;
				wprintf(L"%s \n", sPath);
				wcstombs(chemin, sPath, 1023);
				(*f)(chemin, csv_file);
			}
		} while (FindNextFile(hFind, &fdFile)); // tant qu'il y a un fichier suivant...
		fclose(csv_file); // on ferme
	}

	FindClose(hFind); // fermeture de la lecture du dossier

	return nb_fichiers;
}

// pour le fichier situé à "chemin", sauvegarde au format cvs chemin;moyenne;ecart_type (concaténation ds le fichier csv)
void sauvegarde_moyenne_ecart_type(char* chemin,FILE *csv) {
	FILE *fichier;
	unsigned char *tableau_fichier;
	long taille_fichier;
	float moyenne = 0;
	float ecart_type = 0;
	char ligne_csv[2048];

	fichier = fopen(chemin, "rb"); // ouverture du fichier en mode lecture binaire
	fseek(fichier, 0, SEEK_END);   // place le ponteur à la fin du fichier
	taille_fichier = ftell(fichier); // position du pointeur => taille du fichier
	rewind(fichier); // place le pointeur au début du fichier                   

	tableau_fichier = (char *)malloc((taille_fichier + 1)*sizeof(char)); // créer un tablau pour acceuillir les valeurs des octets
	fread(tableau_fichier, taille_fichier, 1, fichier); // lit le contenu du fichier octet par octet et place les valeurs dans "tableau_fichier"
	fclose(fichier); // fermeture du fichier

	moyenne = calcul_moyenne(tableau_fichier, taille_fichier);
	//printf("Moyenne : %f \n", moyenne);

	ecart_type = calcul_ecart_type(tableau_fichier, taille_fichier, moyenne);
	//printf("Ecart type : %f \n", ecart_type);

	sprintf(ligne_csv, "%s;%f;%f\n", chemin, moyenne, ecart_type); // construction de la ligne pour le csv
	fputs(ligne_csv, csv); // écriture dans le fichier
}


unsigned char* construction_tableau(char* chemin, unsigned int taille) {
	struct csv_ligne *csv_tableau = malloc(taille * sizeof(csv_ligne));
	FILE *file = fopen(chemin, "r");
	char line[1024];
	char str_moyenne[50]; // variable temporaire pour récupérer la moyenne sous forme de texte et qui va nous servir à la convertir en float
	char str_ecart_type[50]; // variable temporaire pour récupérer l'ecart_type sous forme de texte et qui va nous servir à le convertir en float
	int j = 0;

	while (fgets(line, sizeof line, file) != NULL) // on lit une ligne
	{
		int i = 0;
		int k = 0;

		do {
			csv_tableau[j].nom_fichier[k] = line[i]; // on recopie le nom du fichier dans la struture (1ere colone de la ligne du csv, on s'arrête au premier ';')
			i++;
			k++;
		} while (line[i] != ';');
		csv_tableau[j].nom_fichier[k] = '\0';

		i++;
		k = 0;
		do {
			str_moyenne[k] = line[i]; // on recopie la moyenne du fichier dans la struture (2 eme colone)
			i++;
			k++;
		} while (line[i] != ';'); 
		csv_tableau[j].moyenne = atof(str_moyenne); // convertit la moyenne sous forme de texte en float

		i++;
		k = 0;
		do {
			str_ecart_type[k] = line[i]; // on recopie l'ecart type du fichier dans la struture (3 eme colone)
			i++;
			k++;
		} while (line[i] != '\n');
		csv_tableau[j].ecart_type = atof(str_ecart_type); // convertit l'ecart_type sous forme de texte en float

		j++;
	}

	fclose(file);

	return csv_tableau;
}


// On récupère chaque ligne du fichier csv corrigé sous forme de tableau (nom_fichier, moyenne, ecart_type, index_classe)
unsigned char* construction_tableau_corrige(char* chemin, unsigned int taille) {
	struct csv_ligne *csv_tableau = malloc(taille * sizeof(csv_ligne));
	FILE *file = fopen(chemin, "r");
	char line[1024];
	char str_moyenne[50]; // variable temporaire pour récupérer la moyenne sous forme de texte et qui va nous servir à la convertir en float
	char str_ecart_type[50]; // variable temporaire pour récupérer l'ecart_type sous forme de texte et qui va nous servir à le convertir en float
	char str_index_classe[50]; // variable temporaire pour récupérer l'index de classe sous forme de texte et qui va nous servir à la convertir en int
	int j = 0;

	while (fgets(line, sizeof line, file) != NULL) // on lit une ligne
	{
		int i = 0;
		int k = 0;

		do {
			csv_tableau[j].nom_fichier[k] = line[i]; // on recopie le nom du fichier dans la struture (1ere colone de la ligne du csv, on s'arrête au premier ';')
			i++;
			k++;
		} while (line[i] != ';');
		csv_tableau[j].nom_fichier[k] = '\0';

		i++;
		k = 0;
		do {
			str_moyenne[k] = line[i]; // on recopie la moyenne du fichier dans la struture (2 eme colone)
			i++;
			k++;
		} while (line[i] != ';');
		csv_tableau[j].moyenne = atof(str_moyenne); // convertit la moyenne sous forme de texte en float

		i++;
		k = 0;
		do {
			str_ecart_type[k] = line[i]; // on recopie l'ecart type du fichier dans la struture (3 eme colone)
			i++;
			k++;
		} while (line[i] != ';');
		csv_tableau[j].ecart_type = atof(str_ecart_type); // convertit l'ecart_type sous forme de texte en float

		i++;
		k = 0;
		do {
			str_index_classe[k] = line[i]; // on recopie l'indice de classe dans la struture (4 eme colone)
			i++;
			k++;
		} while (line[i] != '\n');
		csv_tableau[j].indice_classe = atoi(str_index_classe);

		j++;
	}

	fclose(file);

	return csv_tableau;
}


int rand_entre(int min, int max) {
	return rand() % (max + 1 - min) + min;
}


// Renvoie la distance entre un centre et un point
float distance(struct centre centre, struct csv_ligne point) {

	float distance;
	distance = sqrt(pow(point.moyenne - centre.x, 2) + pow(point.ecart_type - centre.y, 2));
	return distance;
}


// Renvoie le nouveau centre de l'ensemble en fonction de la classe
struct centre centre_classe(struct csv_ligne *ensemble, int taille_ensemble, int indice_classe) {
	int i;
	struct centre centre_classe;
	int nb_points = 0;
	float somme_x = 0, somme_y = 0;


	for (i = 0; i < taille_ensemble; i++) {
		if (ensemble[i].indice_classe == indice_classe) {
			somme_x += ensemble[i].moyenne;
			somme_y += ensemble[i].ecart_type;
			nb_points++;
		}
	}

	// Calcul des coordonnées de l'isobarycentre
	centre_classe.x = somme_x / nb_points; 
	centre_classe.y = somme_y / nb_points;

	return centre_classe;
}


// Méthode des centres mobiles
void k_means(int nb_classes, struct csv_ligne *ensemble, int taille_ensemble) {
	int i = 0;
	int j = 0;
	int nb_rand;
	float distance_point;
	float distance_minimale;
	boolean flag; // permet de détecter si un point a changé de classe au cours d'une itération
	struct centre *centres = malloc(nb_classes * sizeof(centre)); // on stocke les coordonnées des centres dans un tableau
	
	// On détermine 3 centres initiauxx
	for (i = 0; i < nb_classes; i++) {
		// si 3 classes, le premier centre initial sera un rand dans le 1/3 de ensemble, le 2eme centre un rand entre le 1/3 et 2/3... pour éviter de piocher deux fois le même centre
		nb_rand = rand_entre(i * (taille_ensemble / nb_classes), ((i + 1) * taille_ensemble) / nb_classes);	
		centres[i].x = ensemble[nb_rand].moyenne;
		centres[i].y = ensemble[nb_rand].ecart_type;
	}

	do {
		flag = FALSE;
		for (i = 0; i < taille_ensemble; i++) {
			distance_minimale = -1;
			int indice_ancienne_classe = ensemble[i].indice_classe;

			for (j = 0; j < nb_classes; j++) {
				distance_point = distance(centres[j], ensemble[i]);  // Calcul de la distance entre un centre et un point

																	 // Si la distance minimale n'est pas calculée pour la 1ere fois...
				if (distance_minimale == -1) {
					distance_minimale = distance_point;
					ensemble[i].indice_classe = j; // On affecte la classe suivant la plus petite distance
				}
				else {
					// On stocke la nouvelle valeur minimale si distance_minimale > distance_point
					if (distance_minimale > distance_point) {
						distance_minimale = distance_point;
						ensemble[i].indice_classe = j;
					}
				}

			}
			if (indice_ancienne_classe != ensemble[i].indice_classe) {
				flag = TRUE; // le point a été affecté à une nouvelle classe
			}

		}

		// Calcule des nouveaux centres des classes
		for (i = 0; i < nb_classes; i++) {
			centres[i] = centre_classe(ensemble, taille_ensemble, i);
		}
	} while (flag == TRUE); // Tant qu'un point change de classe, on réitère
	
}

// Sauvegarde le resultat du kmeans dans un fichier CSV => ensemble A'
void sauvegarde_resultat_kmeans(struct csv_ligne *resultat_kmeans, int taille_ensemble, char* nom_fichier) {
	char ligne_csv[2048];
	FILE *csv = fopen(nom_fichier, "w+");

	for (int i = 0; i < taille_ensemble; i++) {
		sprintf(ligne_csv, "%s;%f;%f;%i \n", resultat_kmeans[i].nom_fichier, resultat_kmeans[i].moyenne, resultat_kmeans[i].ecart_type, resultat_kmeans[i].indice_classe); // construction de la ligne pour le csv
		fputs(ligne_csv, csv); // écriture dans le fichier
	}
	fclose(csv);
}
	


int main(int argc, char *argv[]) {
	int nb_fichiers;
	struct csv_ligne *csv_tableau;
	struct csv_ligne *csv_tableau_corrige;

	nb_fichiers = parcourt_dossier(Chemin_dossier, sauvegarde_moyenne_ecart_type);

	if (nb_fichiers > 0) {
		csv_tableau = construction_tableau(Nom_fichier_moyenne_ecart_type, nb_fichiers);

		k_means(Nombre_de_classes, csv_tableau, nb_fichiers);
		sauvegarde_resultat_kmeans(csv_tableau, nb_fichiers, Nom_fichier_resultat_kmeans); // ensemble A' sous forme de fichier csv
	}

	// construit ensemble (tableau de csv_ligne) à partir du fichier type corrigé
	csv_tableau_corrige = construction_tableau_corrige();
	// trouver les n centres de l'ensemble
	// ecrire les centres dans un csv

	printf("\n Appuyez sur entree pour quitter le programme \n");
	getchar();
	return EXIT_SUCCESS;
}
