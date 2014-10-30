#include <stdio.h>
#include <stdlib.h> 
#include <stdarg.h>
#include <windows.h>


#define MAX_CHAR_SUR_UNE_LIGNE 80

struct polynome             {struct maillon_forme_dev *forme_dev;                      struct maillon_forme_facto  *forme_facto;     };
struct maillon_forme_dev    {float degree, valeur;                 int imaginaire;     struct maillon_forme_dev    *suivant;         };
struct maillon_forme_facto  {float valeur_reelle, valeur_imaginaire;                   struct maillon_forme_facto  *suivant;         };
struct complexe             {float reel, imaginaire;                                                                                 };


int main(char argc, char *argv[]);

/// ############ Fonctions opérations mathématiques sur les polynomes
struct polynome convertir_forme_facto(struct polynome p);//en construction
struct polynome additioner(struct polynome p1, struct polynome p2);//somme deux polynomes
struct polynome deriver(struct polynome p);//derive un polynome
struct polynome integrer(struct polynome p);//integre un polynome
//struct polynome multiplier(struct polynome p1, struct polynome p2);//a venir

///############# Fonctions sur les listes chainées
struct maillon_forme_dev *ajouter_element_liste_dev(struct maillon_forme_dev *liste,struct maillon_forme_dev *maillon);//ajoute un élément dans la liste chainée, en prenant en compte une somme éventuelle a faire si une valeur existe déja dans la liste 
void afficher_liste_dev(COORD *position, struct maillon_forme_dev *liste);//affiche la forme développée
struct maillon_forme_facto *ajouter_element_liste_facto(struct maillon_forme_facto *liste, struct maillon_forme_facto *maillon);


/// ############ Fonctions de saisi
char *saisie_raw_polynome(COORD* position);//saisie une chaine de caracteres représentant la forme développée d'un polynome
struct polynome saisir_polynome();//interprete la saisie faite dans saisie_raw_polynome


/// ############ Fonctions "systèmes"
char getch();//getch sans passer par conio.h (NONPORTABLE - Propre a Windows. requiert windows.h)
int printf_pos(COORD *position,char *texte,...);//printf en controlant la position de l'écriture dans la console (NONPORTABLE - Propre 	a windows. Requiert stdarg.h et windows.h)



struct complexe _addition(struct complexe a,struct complexe b);
struct complexe _soustraction(struct complexe a, struct complexe b);
struct complexe _multiplication(struct complexe a,struct complexe b);
struct complexe _division(struct complexe a,struct complexe b);
struct complexe _sqrt(struct complexe a);

int main(char argc, char *argv[])
{
	
	struct polynome p=saisir_polynome();

	//convertir
	
	COORD *position=malloc(sizeof(COORD));
	
	position->X=0;
	position->Y=4;
	
	afficher_liste_dev(position,p.forme_dev);
	position->Y++;
	//p=convertir_forme_facto(p);
	
	position->Y+=2;
	printf_pos(position,"  ");
	
	
	
	
	system("pause");
	return 0;
}

struct polynome convertir_forme_facto(struct polynome p)
{
	struct maillon_forme_dev* e=p.forme_dev,*precedent=NULL;
	struct maillon_forme_facto *ajout=malloc(sizeof(struct maillon_forme_facto));
	int tout_les_degree_sont_entiers=1,est_reel=1,i=0;
	
	ajout->suivant=NULL;
	
	while(e!=NULL)
	{
		if(e->degree!=(int)e->degree)tout_les_degree_sont_entiers=0;
		if(e->imaginaire==1) est_reel=0;
		precedent=e;
		e=e->suivant;
	}
	if(precedent!=NULL) e=precedent;
	float pole=e->degree;
	
	
	//effacer_liste_facto
	
	if(tout_les_degree_sont_entiers && pole==2)//si le pole est 2, on a ax^2+bx+c avec a different de 0
	{
		struct complexe z[3],delta,s[2];//z sont les coeficients : a,b, c dans ax^2+bx+c . z[2]=a;
		float temp=0;
		
		e=p.forme_dev;

		while(e!=NULL)//on détermine les coeficients 
		{
			if(e->imaginaire==0) z[(int)e->degree].reel=e->valeur;
			else z[(int)e->degree].imaginaire=e->valeur;

			e=e->suivant;
		}
	
		delta=_sqrt(_soustraction(_multiplication(z[1],z[1]),_multiplication(_multiplication(z[0],z[2]),(struct complexe){4.0,0})));//sqrt(b²-4*a*c)
		
		s[0]=_division(_addition(_soustraction((struct complexe){0,0},z[1]), delta),_multiplication(z[2],(struct complexe){2.0,0}));//(-b(+-)delta)/2a
		s[1]=_division(_addition(_soustraction((struct complexe){0,0},z[1]),_soustraction((struct complexe){0,0},delta)),_multiplication(z[2],(struct complexe){2.0,0}));
		
		for(i=0;i<2;i++)
		{
			if(s[i].reel!=0)  ajout->valeur_reelle=s[i].reel;
			if(s[i].imaginaire!=0)  ajout->valeur_imaginaire=s[i].imaginaire;
			
			if(ajout->valeur_reelle!=0 || ajout->valeur_imaginaire!=0)
			{
				p.forme_facto=ajouter_element_liste_facto(p.forme_facto,ajout);

				ajout=malloc(sizeof(struct maillon_forme_facto));
				ajout->suivant=NULL;
	 		}
		}
	}
	else if(tout_les_degree_sont_entiers && pole==3)
	{
	
	
	}
	
	
	return p;
}


struct polynome additioner(struct polynome p1, struct polynome p2)
{
	struct maillon_forme_dev* e=p2.forme_dev;
	
	while(e!=NULL)
	{
		p1.forme_dev=ajouter_element_liste_dev(p1.forme_dev,e);
		e=e->suivant;
	}
	p1=convertir_forme_facto(p1);

	return p1;
}

struct polynome deriver(struct polynome p)
{
	struct maillon_forme_dev* e=p.forme_dev,*precedent=NULL,*tmp=NULL;
	
	while(e!=NULL)
	{	
		if(e->degree==0)//si il y a une constante : elle devient 0, on la supprime
		{
			if(precedent!=NULL)
			{
				precedent->suivant=e->suivant;
				free(e);
				e=precedent->suivant;
			}
			else //si il s'agis de la tete de liste
			{
				p.forme_dev=e->suivant;
				free(e);
				e=p.forme_dev;
			}
		}
		else
		{
			e->valeur=e->valeur*e->degree;//sinon : (aX^n)'=n*aX^(n-1)
			e->degree--;
			e=e->suivant;
		}
		precedent=e;
	}
	
	
	p=convertir_forme_facto(p);
	return p;
}

struct polynome integrer(struct polynome p)
{
	struct maillon_forme_dev* e=p.forme_dev;
	
	while(e!=NULL)
	{	
		e->degree++;
		e->valeur=e->valeur/e->degree;
		e=e->suivant;
	}
	
	p=convertir_forme_facto(p);
	
	return p;
}

///#### 2 : Liste chainée

struct maillon_forme_facto *ajouter_element_liste_facto(struct maillon_forme_facto *liste, struct maillon_forme_facto *maillon)
{
	struct maillon_forme_facto *e=liste;
	
	if(e==NULL) liste=maillon;
	else 
	{
		while(e->suivant!=NULL) e=e->suivant;
		e->suivant=maillon;
		maillon->suivant=NULL;
	}

}

struct maillon_forme_dev * ajouter_element_liste_dev(struct maillon_forme_dev *liste,struct maillon_forme_dev *maillon)//vu en prog
{
	struct maillon_forme_dev *element=liste, *precedent=NULL;
	/*
	- La liste est triée par ordre croissant selon les degrés
	- La liste dois additionner 2 nombres du même degrée si ils sont réels/imaginaires tout les 2
	- Les monomes imaginaires doivent etre après les monome réels d'un même degré
	*/
	
	while(element!=NULL && (element->degree < maillon->degree || (element->degree == maillon->degree && element->imaginaire!=maillon->imaginaire && element!=liste )))
	{
		precedent=element;//si la liste est pas vide on la parcours jusqu'au degré désiré
		element=element->suivant;//on garde un traceur vers l'élément précédent
	}
	if(element==NULL)//si on est au bout de la liste
	{
		maillon->suivant=NULL;
		
		if(precedent!=NULL) element=precedent;//le while a fait un tour de trop, ou bien n'a fait aucun tour
		
		if(liste!=NULL)element->suivant=maillon;//liste vide ou non? si elle est vide on la crée,sinon on ajoute le monome a la fin
		else liste=maillon;
	}
	else //si on est pas en fin de liste
	{
	
		if(element->degree== maillon->degree && element->imaginaire == maillon->imaginaire)  element->valeur+=maillon->valeur;//si les monomes sont de degrée/imaginaire identique
		else//on les additionne. sinon on insere le maillon
		{
			if(precedent!=NULL) element=precedent;
			
			if(liste==element && (liste->degree==maillon->degree && liste->imaginaire>maillon->imaginaire || liste->degree>maillon->degree))
			{
				maillon->suivant=liste;//important : si on est en début de liste et qu'on doit ajouter un élémént réel devant un élément imaginaire, en tete de liste
				liste=NULL;//ou bien que la tete de liste est de degrée supérieur
			}
			else maillon->suivant=element->suivant;
		
			if(liste!=NULL)element->suivant=maillon;
			else liste=maillon;	//on modifie la tete de liste
		}
	}
	
	return liste;
}

void afficher_liste_dev(COORD *position, struct maillon_forme_dev *liste)
{
	struct maillon_forme_dev *element=liste;
	int debut=1,nombre_de_chiffres=0,a;
	
	
	position->Y++;
	position->X=0;
		
	while(element!=NULL)
	{	
		nombre_de_chiffres=0;
		for(a=(int)element->valeur;a!=0;nombre_de_chiffres++)  a/=10;//donne le nombre de chiffres avant la virgule de la valeur
		
		if(element->valeur==(int)element->valeur)//si la valeur du nombre est réel
		{
			if(!debut) printf_pos(position,"%+.0f",element->valeur);//si on est au début : on affiche unitquement le potentiel "-". sinon on affiche le + et le -
			else printf_pos(position,"%.0f",element->valeur);
			
			position->X+=nombre_de_chiffres;
			if(!debut|| element->valeur<0) position->X++;
		}
		else
		{
			if(!debut) printf_pos(position,"%+.2f",element->valeur);//si la valeur est a virgule : on fait un affichage approché sur 2 chiffres
			else printf_pos(position,"%.2f",element->valeur);//après la virgule
			
			position->X+=nombre_de_chiffres+3;
			if(!debut|| element->valeur<0) position->X++;
		}
		
		if(element->imaginaire)
		{
			printf_pos(position,"i");//on pense a afficher le "i" et le "x"
			position->X++;
		}
		
		if(element->degree>=1.0)
		{
			printf_pos(position,"x");
			position->X++;
			if(element->degree!=1.0)
			{
				nombre_de_chiffres=0;
				for(a=(int)element->degree;a!=0;nombre_de_chiffres++)  a/=10;//donne le nombre de chiffres du degree
				
				if(element->degree==(float)((int)element->degree))//si le degrée  est entier
				{
					printf_pos(position,"^%.0f",element->degree);
					position->X+=nombre_de_chiffres+1;	
				}
				else
				{
					printf_pos(position,"^%.2f",element->degree);
					position->X+=nombre_de_chiffres+3;
				}
				if(element->degree<0)position->X++;
				
			
			}
			
		}
		
		
		element=element->suivant;
		debut=0;
	}
}


///#### 3 : Saisie
char *saisie_raw_polynome(COORD* position)
{
	int i=0,ok=0,c;

	char *texte=malloc(sizeof(char)),*texte_tmp=NULL;
	printf("\n");
	while(!ok)
	{	
		c=getch();

		if(c==27)exit(0);//touche echap
		else if(c==13 && i>0)  ok=1;//touche entrer
		else if(c==8 && i>0)//touche backspace
		{
			position->X=i-1;
			printf_pos(position,"%c",' ');
			texte[i-1]=0;
			
			texte_tmp=realloc(texte,i);
			texte=texte_tmp;
			
			i--;
		}
		else if((c>47 && c<58) || (c==88 || c==120) || (c==73 || c==105) || (c==45 || c==43) || c==94 || c==46 || c==44) 
		{//48-57 : 0-9. 88,120 : X. 73,105 : i. 45,43 : - et +, 94 : ^, 46 : ., 44 :,
			
			if(c==73 || c==88)  c+=32;//transfo en majuscule 
			if(c==44) c=46;
			
			position->X=i;
			printf_pos(position,"%c",c);
			
			texte_tmp=realloc(texte,i+2);
			texte=texte_tmp;
			
			texte[i]=c;
			i++;
		}
	}
	texte[i]=0;
	position->X=0;
	
	return texte;
}

struct polynome saisir_polynome()
{
	struct polynome p;
	int j=0,mode=1,// 1 : nombre, 2 : le point et après, 3 : lettre (x ou i) 4 exposant,5 : point et après)
	signe_de_la_valeur=1,ok=0;
	float exposant_virgule=1;
	
	COORD *position=malloc(sizeof(COORD));
	position->X=0;
	position->Y=0;
	
	
	p.forme_dev=NULL;
	p.forme_facto=NULL;
	
	struct maillon_forme_dev *pointeur=NULL;
	
	printf_pos(position,"entrez le polynome (carractères autorisés : les nombres, + et -, i et x");
	
	while(!ok)
	{
		ok=1;
		position->Y=1;
		printf_pos(position,"                                                                          ");
		char*texte=saisie_raw_polynome(position);
		position->Y=2;
		
		pointeur=malloc(sizeof(struct maillon_forme_dev));
		pointeur->suivant=NULL;
		pointeur->degree=0;
		pointeur->valeur=0;
		pointeur->imaginaire=0;
		
		for(j=0;j<strlen(texte)+1;j++)
		{
			if(mode==1 && j==0 && texte[j]==45) signe_de_la_valeur=-1;
			else if(mode==1 && texte[j]>47 && texte[j]<58)  pointeur->valeur=pointeur->valeur*10+(texte[j]-48);                 //----------si le texte correspond a des nombres : "98" dans 98ix^2
			else if((mode==1 && texte[j]==46) || (mode==2 && texte[j]>47 && texte[j]<58)) //si on a un point
			{
				if(mode==2)	pointeur->valeur+=(texte[j]-48)*exposant_virgule;
				mode=2;
				exposant_virgule/=10;
			}
			else if((mode==1||mode==2||mode==3) && ((texte[j]==105 && pointeur->imaginaire==0) || (texte[j]==120 && pointeur->degree==0)))
			{																			//----------si on a un x ou un i
				mode=3;                                                  				//on passe au "mode" 2 :
				if(texte[j]==105) pointeur->imaginaire=1;								//les chiffres ne seront plus comptés comme la "valeur" du nombre
				else pointeur->degree=1;												//attention : il peux pas y avoir 2 x ou 2 i a la suite (logique)
				
				if(pointeur->valeur==0)     pointeur->valeur=1;
			}
			else if(j>0 && texte[j]==94 && texte[j-1]==120 && (mode==3||mode==4))  mode=4;//si on a le signe ^ dans d'un x^3 par exemple : on passe a l'exposant de x. fini la "valeur" du nombre et fini les x/i
			else if(mode==4 && texte[j]>47 && texte[j]<58)//les nombres derriere l'exposant
			{
				if(pointeur->degree==1)pointeur->degree=0;
				pointeur->degree=pointeur->degree*10+(texte[j]-48); 
				
				exposant_virgule=1;
			}
			else if((mode==4 && texte[j]==46) || (mode==5 && texte[j]>47 && texte[j]<58))
			{
				if(mode==5)	pointeur->degree+=(texte[j]-48)*exposant_virgule;
				mode=5;
				exposant_virgule/=10;
			}
			else if((j>=strlen(texte) || texte[j]==43 || texte[j]==45) && (j>0 && !(texte[j-1]==43 || texte[j-1]==45 || texte[j-1]==94)))// + et - 
			{
				if(j==0) signe_de_la_valeur=-1;//si il s'agis du moins devant le 1er nombre, genre -54+3x, on ne fais rien
				else
				{
					pointeur->valeur=signe_de_la_valeur*pointeur->valeur;//sinon, on change son signe, puis on l'ajoute a la liste chainée

					p.forme_dev=ajouter_element_liste_dev(p.forme_dev,pointeur);
					
					pointeur=malloc(sizeof(struct maillon_forme_dev));//et on recrée un maillon pour la suite.
					pointeur->suivant=NULL;
					pointeur->degree=0;
					pointeur->valeur=0;
					pointeur->imaginaire=0;
					mode=1;
					exposant_virgule=1;
					signe_de_la_valeur=1;
					if(j!=strlen(texte) && texte[j]==45) signe_de_la_valeur=-1;//on sauvegarde le signe de la valeur (+ ou -)
				}
			}
			else
			{
				if(j>=strlen(texte))j--;
				ok=0;
				printf_pos(position,"Erreur de saisie près de la lettre \"%c\" (lettre numéro %d )",texte[j],j+1);//en cas d'erreur
				
				/*free(pointeur);
				
				while(p.forme_dev!=NULL);
				{
					pointeur=p.forme_dev;
					p.forme_dev=p.forme_dev->suivant;
					free(pointeur);
				}
				*/
				//getch();
			}

		}
	}
	
	return p;
}


/// 4 Systeme

char getch()  
{
    DWORD chars_lus;
    INPUT_RECORD char_en_cours;
    HANDLE input_console = GetStdHandle(STD_INPUT_HANDLE);
      
    while(ReadConsoleInput(input_console , &char_en_cours, 1, &chars_lus)) if(char_en_cours.EventType == KEY_EVENT && char_en_cours.Event.KeyEvent.bKeyDown) return char_en_cours.Event.KeyEvent.uChar.AsciiChar;
         
    return 0;
}

int printf_pos(COORD *position,char *texte,...)
{
	char *args; char texte_final[1000];
	int i,nb_retour_a_la_ligne_en_plus=0,cpt=0;
	
	va_start(args,texte);
    vsprintf(texte_final,texte,args);
    va_end(args);
	
	char charset[256] ={
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
    0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
    0xF8, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
    0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
    0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
    0x85, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0x8A, 0x82, 0xEA, 0x89, 0xEC, 0xED, 0xEE, 0xEF,
    0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
	};//la console de windows est "différente", elle écris en OEM. On doit donc faire la conversion ansi->oem. Les carracteres 128 a 256 changent
	

	for (i =0;i<strlen(texte_final); i++)
	{
		if(texte_final[i]==13)cpt=0;
		else cpt++;
		if(cpt>=MAX_CHAR_SUR_UNE_LIGNE){cpt=0;nb_retour_a_la_ligne_en_plus++;}
		
		if(texte_final[i] < 0) texte_final[i]=charset[(256+texte_final[i])];
	}
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), *position);
	printf("%s",texte_final);

	
	return nb_retour_a_la_ligne_en_plus;
}



struct complexe _addition(struct complexe a,struct complexe b)
{
	a.reel+=b.reel;
	a.imaginaire+=b.imaginaire;
	return a;
}

struct complexe _soustraction(struct complexe a, struct complexe b)
{
	a.reel-=b.reel;
	a.imaginaire-=b.imaginaire;
	return a;
}

struct complexe _multiplication(struct complexe a,struct complexe b)
{
	struct complexe c;
	c.reel = a.reel*b.reel - a.imaginaire*b.imaginaire;
    c.imaginaire = a.imaginaire*b.reel + a.reel*b.imaginaire;
	return c;
}

struct complexe _division(struct complexe a,struct complexe b)
{
	struct complexe c;
	if(b.reel==0 && b.imaginaire==0)
	{
		printf("\nerreur ! ligne %d : tentative de division d'un complexe par 0+0i\n",__LINE__);
		system("pause");
		exit(1);
	}
	
	c.reel =       (a.reel*b.reel + a.imaginaire*b.imaginaire)/(b.reel*b.reel + b.imaginaire*b.imaginaire);
    c.imaginaire = (a.imaginaire*b.reel - a.reel*b.imaginaire)/(b.reel*b.reel + b.imaginaire*b.imaginaire);
	return c;
}

struct complexe _sqrt(struct complexe a)
{
	float module,temp;
	
	if(a.reel<=0 && a.imaginaire==0.0)//on fais sqrt(a) sachant que delta est complexe
	{
		a.imaginaire=sqrt(-a.reel);//si a est reel négatif, alors on fait le la racine imaginaire
		a.reel=0;
	}
	else //sinon, on fait une formule(qui ne marche pas uniquement dans le cas ci dessus) pour trouver la racine d'un nombre complexe
	{
		float module=sqrt(a.reel*a.reel + a.imaginaire*a.imaginaire);//le module de a
		float temp=a.reel;
			
		a.reel=       sqrt(module)* (a.reel+module) /(sqrt((a.reel+module)*(a.reel+module)+a.imaginaire*a.imaginaire));//sqrt(z)=
		a.imaginaire= sqrt(module)* (a.imaginaire)  /(sqrt((temp  +module)*(temp  +module)+a.imaginaire*a.imaginaire));//sqrt(|z|)*(z+|z|)/|z+(|z|)|
	}
	return a;
}
