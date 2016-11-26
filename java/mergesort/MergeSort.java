/*
 * Classe principale effectuant une série de tests de la classe MergeSortThread
 * @Author: Stanislas Gueniffey (000377223) INFO-2
 */

import java.util.Arrays;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.Stack;
import java.lang.Character;


public class MergeSort {

	public static void main(String[] args) {
		//Voici plusieurs tris effectués à l'aide de la classe MergeSortThread sur des structures de
		//données différentes et pour divers types d'objets comparables entre eux :
		
		//ArrayList<Integer>
		ArrayList<Double> al1 = new ArrayList<>(Arrays.asList(10.1,20.2,.3,-1.3,3.4,2.0,50.1,8.8));//Init
		Thread t1 = new Thread(new MergeSortThread<Double>(al1)); //Thread avec le code de la classe
		
		//Integer[]
		Integer[] ar2 = {5,4,3,2,1,0,-1,0,1,2,3};
		Thread t2 = new Thread(new MergeSortThread<Integer>(ar2)); //Surcharge du constructeur
		//(peut créer wrapper de type List<Integer>)
		
		//int[]
		//int[] array = {8,7,6,5,4,3,2,1,0};
		//Thread t = new Thread(new MergeSortThread<Integer>(array));//Error: the constructor is undefined
		//(type primitif peut pas être interprété comme paramètre de type générique, asList créerait List<int[]>)
		
		//Stack<Character>
		Stack<Character> st3 = new Stack<>();
		for (char c=90; c>64; c--) st3.push(new Character(c));
		Thread t3 = new Thread(new MergeSortThread<Character>(st3));
		
		//LinkedList<String>
		LinkedList<String> ll4 = new LinkedList<String>(Arrays.asList(
				"Bonjour", "Mr", "Le", "Professeur", "Voici", "Une", "Liste", "Doublement", "Chainée"));
		Thread t4 = new Thread(new MergeSortThread<String>(ll4));
		
		
		//On peut exécuter simultanément plusieurs threads
		System.out.println(al1.toString());
		System.out.println(Arrays.toString(ar2));
		System.out.println(st3.toString());
		t1.start(); //Démarre les threads
		t2.start();
		t3.start();
		
		try {		//Attend la fin de leur exécution
			t1.join();
			t2.join();
			t3.join();
		} catch (InterruptedException e) { //Erreur: thread interrompu
			System.out.println(e.toString());
		}
		
		System.out.println(al1.toString());
		System.out.println(Arrays.toString(ar2));
		System.out.println(st3.toString());
		
		
		//On peut les exécuter séparément (pour isoler l'ouput de leur exécution par ex.)
		System.out.println(ll4.toString());
		t4.start();
		try {
			t4.join();
		} catch (InterruptedException e) { //Erreur: thread interrompu
			System.out.println(e.toString());
		}
		
		System.out.println(ll4.toString());
	}

}