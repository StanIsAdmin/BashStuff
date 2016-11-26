/*
 * Classe réalisant un tri fusion (merge sort) d'une séquence d'objets en multithreading
 * @Author: Stanislas Gueniffey (000377223) INFO-2
 */

import java.util.Arrays;	//Methodes sur les tableaux
import java.util.ArrayList; //Implémentation efficace de liste pour copies temporaires
import java.util.List;		//Liste (le type visé par le tri)


public class MergeSortThread<T extends Comparable<? super T>> implements Runnable {
	/*
	 * Effectue l'algorithme de tri fusion sur une liste (séquence ordonnée d'éléments
	 * permettant le random access) d'objets comparables entre eux de type générique T.
	 * Implémente Runnable pour être exécuté comme code d'un Thread (ou plusieurs).
	 */
	
	private int _depth; //Profondeur de l'appel récursif (pour affichage)
	private static int _minSplitSize=4; //Nombre minimum d'éléments à trier par un thread
	private List<T> _subList; //Vue "comme une sous-liste" de la liste à trier
	
	public MergeSortThread(List<T> list) {
		/*
		 * Constructeur public, prend en paramètre une liste d'objets de type T à trier.
		 */
		_subList = list;
		_depth=0;
	}
	
	public MergeSortThread(T[] array) {
		/*
		 * Constructeur public, prend en paramètres un tableau d'objets de type T à trier.
		 */
		//Crée wrapper object List<T> à partir du tableau
		_subList = Arrays.asList(array); //Possible uniquement pour les tableaux d'objets
		_depth=0;
	}
	
	private MergeSortThread(List<T> list, int depth) {
		/*
		 * Constructeur privé, list est une sous-liste de la liste à trier, et depth
		 * est la profondeur de this dans l'arbre des threads.
		 */
		_subList=list; //Vue comme sous-liste de la liste à trier
		_depth=depth;
	}
	
	@Override
	public void run() {
		/*
		 * Surcharge la méthode run() de Thread, appelée lors du démarrage (start) du thread
		 * contenant l'object exécutable this. Crée si nécessaire des threads supplémentaires
		 * pour effectuer le tri de manière efficace. 
		 */
		
		int siz=_subList.size();
		String tabs = new String(new char[_depth]).replace("\0", "\t"); //Pour affichage
		System.out.printf("%sRunning thread for %d items\n", tabs, siz);
		
		//Si _subList est considéré trop petite que pour séparer le travail en threads
		if (siz<_minSplitSize) {
			System.out.printf("%s Sorting recursively for %d items\n",tabs, siz);
			this.mergeSort(_subList); //Appel à version itérative du tri
		}
		
		//Sinon, _subList est assez grand que pour créer de nouveaux threads
		else {
			Thread tLower=null, tUpper=null; //Nouveaux threads
			int mid=siz/2; //Indice médian de _subList (borne sup. exclusive, inf. inclusive)
			
			
			//SECTION INFERIEURE: si nb. elem > 1 (section non triée)
			if (mid>1) {
				//Crée nouveau thread avec Runnable object MergeSortThread avec la moitié des données
				System.out.printf("%s Creating new thread for %d items\n",tabs, mid);
				
				tLower = new Thread(new MergeSortThread<T>(_subList.subList(0, mid), _depth+1));
				tLower.start(); //Démarre le thread (appelle sa méthode run() surchargée)
			}
			
			//SECTION SUPERIEURE: si nb. elem > 1 (section non triée)
			if (siz-mid>1) {
				//Idem pour objet MergeSortThread avec l'autre moitié de _subList
				System.out.printf("%s Creating new thread for %d items\n", tabs, siz-mid);
				
				tUpper = new Thread(new MergeSortThread<T>(_subList.subList(mid, siz), _depth+1));
				tUpper.start();
			}
				
			//Attend la fin de l'exécution du premier thread avant de copier la section inférieure
			if (tLower!=null) {
				try {
					tLower.join();
				} catch(InterruptedException e) { //En cas d'interruption
					Thread.currentThread().interrupt(); //Interromp le thread courant (remonte erreur)
				}
			}
			
			
			//Copie la partie inférieure (susceptible d'être écrasée au cours du merge) de _subList
			ArrayList<T> tmpList = new ArrayList<T>(_subList.subList(0, mid));
			
			//Attend la fin de l'exécution du second thread avant de procéder au tri
			if (tUpper!=null) {
				try {
					tUpper.join();
				} catch(InterruptedException e) { //En cas d'interruption
					Thread.currentThread().interrupt(); //Interromp le thread courant
				}
			}
			
			//Fait appel à la méthode merge fonctionnant pour deux sous-sections déjà triées entre elles
			this.merge(_subList, tmpList);
		}
	}
	
	
	private void mergeSort(List<T> list) {
		/*
		 * Effectue itérativement et récursivement le tri fusion complet sur list.
		 */		
		int siz=list.size();
		//Tant que la sous-liste considérée n'est pas triée (de taille 1)
		if (siz > 1) {
			
			//Divise la liste, appel récursif pour sous-listes
			mergeSort(list.subList(0, siz/2));
			mergeSort(list.subList(siz/2, siz));
			
			//Copie temporaire de la sous-liste inférieure, pouvant être écrasée
			ArrayList<T> tmpList = new ArrayList<T>(list.subList(0, siz/2));
			
			//Effectue la fusion (merge) des deux sous-listes triées
			merge(list, tmpList);
		}
	}
	
	
	private void merge(List<T> list, ArrayList<T> tmpList) {
		/*
		 * Effectue la fusion des deux moitiés de list, sachant qu'elles sont triées entre elles.
		 * tmpList est une copie temporaire de la moitié inférieure de list
		 */
		
		int mid = list.size()/2; //Indice du milieu
		int i=0, j=mid, k=0; //Indices de parcours pour tmpList, list[section sup] et list[tout]
		
		//Fusionne toutes valeurs de _subList en utilisant tmpList pour la moitié inférieure
		while (i < mid && j <list.size()) 
			list.set(k++, (list.get(j)).compareTo(tmpList.get(i))<0 ? list.get(j++) : tmpList.get(i++));
			//la méthode compareTo est assurée par le type paramétrique
		
		//S'il reste des valeurs dans tmpList, on les copie a la fin de _subList
		while (i < mid) list.set(k++, tmpList.get(i++));
	}

}
