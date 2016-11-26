#include "Vector.hpp"
#include "VectDyn.hpp"
#include "VectStat.hpp"
#include "VectPoly.hpp"
#include "PolyDyn.hpp"
#include "PolyMod.hpp"

#include <iostream>


/**GUENIFFEY Stanislas (000377223) INFO-2**/

extern const PolyDyn<int> pDiv1(1, 3); //DIVISEUR : d(x) = x^3 - x^2 - x - 1
double values[5] = {0.0,3.1,-2.5,4.2,1.0};
extern const PolyDyn<double> pDiv2(values, 5); //DIVISEUR : d(x) = x^5 - 1.0x^4 - 4.2x^3 + 2.5x^2 - 3.1x

int main()
{
    std::cout << "VECTEURS DYNAMIQUES:" << std::endl;

	//Vecteurs Dynamiques <int>
    VectDyn<int> vDI(2, 4), vDI2(-vDI); //Ctr init, copie
    VectDyn<int>* vDI3 = new VectDyn<int>(1, 6); //pointeur

    std::cout << "vDI: " << vDI << " == [2,2,2,2]" << std::endl;
    std::cout << "vDI2: " << vDI2 << " == [-2,-2,-2,-2]" << std::endl;
    std::cout << "vDI3: " << *vDI3 << " == [1,1,1,1,1,1]" << std::endl;

    vDI = *vDI3 - vDI2; //Soustraction -> changement de taille
    delete vDI3; //Destructeur
    std::cout << "vDI=vDI3-vDI2, vDI: " << vDI << " == [3,3,3,3,1,1]" << std::endl;

	//Vecteurs Dynamiques <double>
    VectDyn<double> vDD(.3, 2);
    std::cout << "vDD: " << vDD << " == [0.3, 0.3]" << std::endl;
    vDD += VectDyn<double>(.2, 3); //Addition -> changement de taille
    std::cout << "vDD+=[.2,.2], vDD: " << vDD << " == [.5, .5, .2]" << std::endl;
    //vDF = vDI; //ERREUR: pas de conversion possible


	std::cout << "VECTEURS STATIQUES:" << std::endl;

	//Constructeurs et assignations
    int seq[3] = {0,-1,2};
    VectStat<int, 3> vSI(seq);//Ctr init
    VectStat<int, 3> vSI1 = -vSI; //Assignation
    VectStat<int, 3> vSI2(2*vSI); //Ctr transfert

	std::cout << "vSI: " << vSI << " == [0,-1,2]" << std::endl;
    std::cout << "vSI1: " << vSI1 << " == [0,1,-2]" << std::endl;
    std::cout << "vSI2: " << vSI2 << " == [0,-2,4]" << std::endl;

	//Operations entre vecteurs differents retournent un vecteur dynamique
    VectDyn<int> vDAddSD(vDI + vSI); //Conversion implicite static -> dynamic
    VectDyn<int> vDSubSD = vSI - vDI;

    std::cout << "vDI+vSI: " << vDAddSD << " == [3,2,5,3,1,1]" << std::endl;
    std::cout << "vDI-vSI: " << vDSubSD << " == [-3,-4,-1,-3,-1,-1]" << std::endl;
	VectStat<int, 3> vSAddSD(vDI + vSI); //Force conversion
	std::cout << "vDI+vSI (stat): " << vSAddSD << " == [3,2,5]" << std::endl; //Ne retient que trois dernieres valeurs
	//vSI += vDI; //ERREUR EXEC: Addition d'un vecteur de taille differente
    //vSI = vDI; //ERREUR EXEC: Assignation de vecteur de taille differente

	std::cout << "INPUT PARSING:" << std::endl;
    std::cout << "Select values for vDI (int) and vDD (double): ";//EX: 10 20 -3 -4 0 0 .64 -2.3
    std::cin >> vDI >> vDD; //Enchaine parsing jusque fin du stream ("\n")
    std::cin.ignore(); //Nettoie le stream
    std::cout << "vDI: " << vDI << std::endl << "vDD: " <<  vDD << std::endl;


	std::cout << "POLYNOMES:" << std::endl;
	//Polynome dynamique <double>
    double vals[5] = {6.1,-5.3,4.4,-3,.2};
    PolyDyn<double> pDynD(vals, 5);
    std::cout << "pDynD: " << pDynD << " == 0.2x^4 - 3.0x^3 + 4.4x^2 - 5.3x + 6.1" << std::endl;
    std::cout << "pDynD(2): " << pDynD(2) << " == -7.7" << std::endl; //Evaluation

	//pointeur vers Polynome modulaire <double, 5, pDiv2>
	PolyMod<double, 5, pDiv2>* pModD= new PolyMod<double, 5, pDiv2>(pDynD);//Ctr copie
	std::cout << "pModD: " << *pModD << " == 0.2x^4 - 3.0x^3 + 4.4x^2 - 5.3x + 6.1" << std::endl;
	std::cout << "pModD(2): " << pModD->operator()(2) << " == -7.7" << std::endl; //Evaluation

	//Polynome dynamique <int>
    PolyDyn<int> pDynI(2, 3);
    std::cout << "pDynI: " << pDynI << " == 2x^2 + 2x + 2" << std::endl;

	//Polynomes modulaires <int, 3, pDiv1>
    PolyMod<int, 3, pDiv1> pModI(5);
    std::cout << "pModI: " << pModI << " == 5x^2 + 5x + 5" << std::endl;
    int vals2[3] = {6,1,-4};
    PolyMod<int, 3, pDiv1> pModI2(vals2);
    std::cout << "pModI2: " << pModI2 << " == -4x^2 + 1x + 6" <<  std::endl;

	//Produit impliquant un polynome non modulaire ou de modulo different: multip. normale
    std::cout << "pModI*pDynI: " << pModI * pDynI << " == 10x^4 + 20x^3 + 30x^2 + 20x + 10" << std::endl;
    //Produit entre Polynome Modulaires de meme type: modulo s'applique a la multiplication
    std::cout << "pModI*pModI2: " << pModI * pModI2 << " == -40x^2 - 20x - 5" << std::endl;

    //Operateur *= de polynome modulaire: s'assure que modulo soit appliqué
    pModI *= pDynI;
    *pModD *= pDynD;
    std::cout << "pModI*=pDynI: " << pModI << " == 70x^2 + 60x + 40" << std::endl;
    std::cout << "pModD*=pDynD: " << *pModD << " == 73.9256x^4 + ..." << std::endl;
    delete pModD;

	//pModI = vDI; //ERREUR Exec: Assignation de vecteur de taille differente
	pModI = pDynI *-2; //Assignation valable pour polynome de meme taille
	pDynD = vDD; //Assignation peut changer la taille

	std::cout << "pModI=pDynI*2: " << pModI << " == -4x^2 - 4x - 4" << std::endl;
	std::cout << "pDynD*=vDD: " << pDynD << " == 0.2x^2 + 0.5x + 0.5" << std::endl;

    //Operations entre polynomes differents retournent un polynome dynamique
	std::cout << "pModI-pDynI: " << pModI - pDynI << " == -6x^2 - 6x -6" << std::endl;
	std::cout << "-pModI[0]: " << -pModI[0] <<  " == 4" << std::endl;
	pModI[2] = 0;
	std::cout << "pModI[2]=0, pModI: " << pModI << " == " << std::endl;
	std::cout << "pModI.deg(): " << pModI.deg() << " == 1" << std::endl;


	std::cout << "INPUT PARSING:" << std::endl;
    std::cout << "Select values for pModI (int) and pDynD (double): ";//EX: 10 20 30 .1 .2 .3
    std::cin >> pModI >> pDynD; //Enchaine parsing jusque fin du stream ("\n")
    std::cin.ignore(); //Nettoie le stream
    std::cout << "pModI: " << pModI << std::endl << "pDynD: " << pDynD << std::endl;//30x^2 = 20x + 10, 0.3x^2 + 0.2x + 0.1

    return 0;
}
