# INFO-F-310 : PROJET
# TRANSPORT DE PRODUITS CHIMIQUES

# --- Param�tres ---

param joursAnnee := 300; #jours ouvrables annuels
param joursPeriode := 2; #jours de la p�riode optimis�e
param trajetsMax := 6; #nombre maximum de trajets par jour
param trav := 10; #heures de travail par jour & employ�

# --- Indices ---

/* sites */
set S;
set SProdAcide;
set SProdBase;
set SDepart;

/* produits */
set P;

/* produits transport�s, y compris l'absence de produit */
set T;

/* affectation des compartiments, d�finie par produit affect� au plus grand compartiment*/
set A;

/* camions de type 1 */
set C1 := 1..6;

/* camions de type 2 */
set C2 := 1..4;

/* indice de la journ�e de travail sur le semestre */
set J := 1..joursPeriode;

/* indice du trajet au cours de la m�me journ�e */
set I := 1..trajetsMax;

# --- Constantes ---

/* capacit� du compartiment 1 des camions de types 1 et 2 */
param cap1 := 16.5;

/* capacit� du compartiment 2 des camions de type 2 */
param cap2 := 5.5;

/* distance s�parant les sites i et j (km) */
param dist{i in S, j in S};

/* vitesse moyenne d'un camion (km/h) */
param vit := 70.0;

/* temps d'arr�t pour r�approvisionnement/d�chargement (h) */
param arret := 1.0;

/* temps n�cessaire � aller de la ville i � la ville j + temps d'arr�t (h) */
param temps{i in S, j in S} := (dist[i,j] / vit) + arret;

/* demande annuelle au site i en produit p*/
param demandeAn{p in P, i in S};

/* demande pour la p�riode optimis�e au site i en produit p*/
param demande{p in P, i in S} := ((demandeAn[p,i] / joursAnnee) * joursPeriode);

# --- Variables ---

/* indique si le ieme trajet du jeme jour pour le c�me camion de type 1 (resp 2) est effectu� entre le site v et le site w en transportant le produit p*/
var X1{c in C1, i in I, j in J, v in S, w in S, a in A}, binary;
var X2{c in C2, i in I, j in J, v in S, w in S, a in A}, binary;

# --- Fonction objective ---

/* minimise somme des temps de trajet (heures de travail) */
minimize tempsTravail: sum{c in C1, i in I, j in J, v in S, w in S, a in A} (X1[c, i, j, v, w, a] * temps[v, w]) + sum{c in C2, i in I, j in J, v in S, w in S, a in A} (X2[c, i, j, v, w, a] * temps[v, w]);

# --- Contraintes ---

/* affectation ne change pas au cours d'une journ�e */
/* l'affectation du premier trajet implique que les trajets suivants auront la m�me affectation - se base sur le fait que les camions ne peuvent pas faire deux trajets simultan�s */
s.t. affect1{c in C1, j in J, i in I diff {1}, a in A}: sum{v in S, w in S} X1[c,i,j,v,w,a] <= sum{v in S, w in S} X1[c,i-1,j,v,w,a];

s.t. affect2{c in C2, j in J, i in I diff {1}, a in A}: sum{v in S, w in S} X2[c,i,j,v,w,a] <= sum{v in S, w in S} X2[c,i-1,j,v,w,a];

/* trajets d'un jour ne d�passent pas les heures de travail */
s.t. camions1Jours{c in C1, j in J}: sum{i in I, v in S, w in S, a in A} X1[c,i,j,v,w,a] * temps[v, w] <= trav;

s.t. camions2Jours{c in C2, j in J}: sum{i in I, v in S, w in S, a in A} X2[c,i,j,v,w,a] * temps[v, w] <= trav;

/* quantit�s transport�es � un site sup�rieures ou �gales � demande */
s.t. demandeAcide{w in S}: sum{c in C1, i in I, j in J, v in SProdAcide} X1[c,i,j,v,w,'Acide'] * cap1 + sum{c in C2, i in I, j in J, v in SProdAcide, a in A} X2[c,i,j,v,w,a] * (if a=='Acide' then cap1 else cap2) >= demande['Acide',w];

s.t. demandeBase{w in S}: sum{c in C1, i in I, j in J, v in SProdBase} X1[c,i,j,v,w,'Base'] * cap1 + sum{c in C2, i in I, j in J, v in SProdBase, a in A} X2[c,i,j,v,w,a] * (if a=='Base' then cap1 else cap2) >= demande['Base',w];

/* camions partent chaque jour de Liege */
s.t. camions1Depart{c in C1, j in J}: sum{v in S diff SDepart, w in S, a in A} X1[c,1,j,v,w,a] = 0;

s.t. camions2Depart{c in C2, j in J}: sum{v in S diff SDepart, w in S, a in A} X2[c,1,j,v,w,a] = 0;

/* camions terminent chaque jour a Liege*/
s.t. camions1Retour{c in C1, j in J}: sum{i in I, w in S, a in A} X1[c,i,j,'Liege',w,a] = sum{i in I, v in S, a in P} X1[c,i,j,v,'Liege',a];

s.t. camions2Retour{c in C2, j in J}: sum{i in I, w in S, a in A} X2[c,i,j,'Liege',w,a] = sum{i in I, v in S, a in P} X2[c,i,j,v,'Liege',a];

/* unicit� des trajets simultan�s (un seul n�me trajet par jour par camion) */
s.t. camions1Unicite{c in C1, i in I, j in J}: sum{v in S, w in S, a in A} X1[c,i,j,v,w,a] <= 1;

s.t. camions2Unicite{c in C2, i in I, j in J}: sum{v in S, w in S, a in A} X2[c,i,j,v,w,a] <= 1;

/* continuite des trajets (ville d'arriv�e est ville de d�part suivante) */
s.t. camions1Continuite{c in C1, i in I diff {1}, j in J, v in S, a in A}: sum{w in S} X1[c,i,j,v,w,a] <= sum{w in S} X1[c,i-1,j,w,v,a];

s.t. camions2Continuite{c in C2, i in I diff {1}, j in J, v in S, a in A}: sum{w in S} X2[c,i,j,v,w,a] <= sum{w in S} X2[c,i-1,j,w,v,a];

# --- Output ---

solve;

display demande;
printf 'Temps de travail : %f\n', tempsTravail;
printf 'Format des horaires :\n(jour, camion, indice, depart, arrivee, affectation)\n';
printf 'Camions de type 1\n';
display {j in J, c in C1, i in I, v in S, w in S, a in A: X1[c,i,j,v,w,a]==1} (j, c, i, v, w, a);
printf 'Camions de type 2\n';
display {j in J, c in C2, i in I, v in S, w in S, a in A: X2[c,i,j,v,w,a]==1} (j, c, i, v, w, a);

# --- Donn�es ---

data;

set S := Anvers Bruxelles Charleroi Gand Hasselt Liege;
set SProdAcide := Liege;
set SProdBase := Anvers;
set SDepart := Liege;

set P := Acide Base;
set T := Acide Base Neant;
set A := Acide Base;

param dist : Anvers    Bruxelles Charleroi Gand      Hasselt   Liege     :=
Anvers       0         45        100       40        50        105       
Bruxelles    45        0         60        40        50        100       
Charleroi    100       60        0         100       80        100       
Gand         40        40        100       0         60        140       
Hasselt      50        50        80        60        0         60        
Liege        105       100       100       140       60        0;

param demandeAn: Anvers    Charleroi Liege     Gand      Bruxelles Hasselt   :=
Acide            9000      12000     0         2000      6200      350
Base             0         0         30000     0         0         0;

end;
