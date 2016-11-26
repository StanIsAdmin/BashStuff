#ifndef VECTSTAT_H
#define VECTSTAT_H

#include <cstddef>
#include <stdexcept>

#include "Vector.hpp"


/**GUENIFFEY Stanislas (000377223) INFO-2**/


template <typename Elem, std::size_t Dimens>
class VectStat: virtual public Vector<Elem>
{
	protected:
		using Vector<Elem>::_dim;
		using Vector<Elem>::_values;

	public:
		/**Constructeurs**/
        VectStat();					//trivial : vecteur nul
        explicit VectStat(const Elem&);		// init: valeur, taille implicite (Dimens)
        explicit VectStat(const Elem*);		// init: tableau valeurs, taille implicite (Dimens) NON VERIFIEE
        VectStat(const VectStat<Elem, Dimens>&);	// de copie
        VectStat(VectStat<Elem, Dimens>&&);			// de transfert
        explicit VectStat(const Vector<Elem>&);		//de conversion

        /**Destructeur**/
        //Pas besoin de destructeur autre que celui de classe de base

        /**Operateurs**/
        virtual VectStat<Elem, Dimens>& operator=(const Vector<Elem>&) override;//Assignation redefinie, type retour covariant
        virtual VectStat<Elem, Dimens>& operator=(Vector<Elem>&&) override;		//Transfert redefini, type retour covariant
        virtual VectStat<Elem, Dimens>& operator=(const VectStat<Elem, Dimens>&); //Pourrait =default
        virtual VectStat<Elem, Dimens>& operator=(VectStat<Elem, Dimens>&&); //Pourrait =default, mais Warning

        virtual void operator+=(const Vector<Elem>&) override; //Addition d'un vecteur redefinie
        virtual void operator-=(const Vector<Elem>&) override; //Soustraction d'un vecteur redefinie
};


/**--------------------CONSTRUCTEURS**/

/**initialisants**/
template <typename Elem, std::size_t Dimens>
VectStat<Elem, Dimens>::VectStat()
	//Constructeur par defaut (trivial) appele
    {
		_values = new Elem[Dimens]; //Pas d'initialisation, mais cree tableau
		_dim = Dimens;
    }

template <typename Elem, std::size_t Dimens>
VectStat<Elem, Dimens>::VectStat(const Elem& element):
    Vector<Elem>(element, Dimens) {} //Ctr correspondant de Vector

template <typename Elem, std::size_t Dimens>
VectStat<Elem, Dimens>::VectStat(const Elem* elemArray):
    Vector<Elem>(elemArray, Dimens) {} //Ctr correspondant de Vector

/**de copie**/
template <typename Elem, std::size_t Dimens>
VectStat<Elem, Dimens>::VectStat(const VectStat<Elem, Dimens>& vect):
    Vector<Elem>(vect) {} //Appelle ctr. copie

/**de transfert**/
template <typename Elem, std::size_t Dimens>
VectStat<Elem, Dimens>::VectStat(VectStat<Elem, Dimens>&& vect):
    Vector<Elem>(std::forward<VectStat<Elem, Dimens>>(vect)) {} //Appelle ctr. transfert

/**de conversion (explicit)**/
template <typename Elem, std::size_t Dimens>
VectStat<Elem, Dimens>::VectStat(const Vector<Elem>& vect)
	//Constructeur par defaut appele
	{
		//PERMISSIF (accepte toute dimension de vecteur)
		_values = new Elem[Dimens]; //tableau de taille Dimens
		std::size_t minDim = Dimens < vect.siz() ? Dimens : vect.siz(); //Call inline
		while (_dim < minDim)//_dim=0 suite a construction
		{
			//copie autant de valeurs de vect qu'il peut contenir (reste eventuel non initialise)
			_values[_dim] = vect[_dim];
			_dim++;
		}
		_dim = Dimens;
	}


/**--------------------OPERATEURS**/

/**assignation a vecteur**/
template <typename Elem, std::size_t Dimens>
VectStat<Elem, Dimens>& VectStat<Elem, Dimens>::operator=(const Vector<Elem>& vect)
{
    if (_dim != vect.siz()) //NON PERMISSIVE (dimension identique seulement)
    { throw std::length_error("VectStat : assignation to Vector of different dimension"); }
    else
    {
		this->Vector<Elem>::operator=(vect);//Appelle op. copie si conditions respectees
		return *this;
    }
}
/**assignation a un vecteur statique de meme type**/
template <typename Elem, std::size_t Dimens>
VectStat<Elem, Dimens>& VectStat<Elem, Dimens>::operator=(const VectStat<Elem, Dimens>& vect)
{
    this->Vector<Elem>::operator=(vect);//Forcement de meme dimension, appelle op. copie
    return *this;
}

/**transfert d'un vecteur**/
template <typename Elem, std::size_t Dimens>
VectStat<Elem, Dimens>& VectStat<Elem, Dimens>::operator=(Vector<Elem>&& vect)
{
	if (_dim != vect.siz()) //NON PERMISSIF (dimension identique seulement)
	{ throw std::length_error("VectStat : assignation to Vector of different dimension"); }
	else
	{
        this->Vector<Elem>::operator=(std::forward<Vector<Elem>>(vect));//Appelle op. transfert
        return *this;
	}
}
/**transfert d'un vecteur statique de meme type**/
template <typename Elem, std::size_t Dimens>
VectStat<Elem, Dimens>& VectStat<Elem, Dimens>::operator=(VectStat<Elem, Dimens>&& vect)
{
	//Forcement meme dimension -> pas de verification
    this->Vector<Elem>::operator=(std::forward<Vector<Elem>>(vect));//Appelle op. transfert
    return *this;
}


/**op+= addition**/
template <typename Elem, std::size_t Dimens>
void VectStat<Elem, Dimens>::operator+=(const Vector<Elem>& vect)
{
	//Comportement non permissif (dimension identique seulement)
	if (_dim != vect.siz())
	{ throw std::length_error("VectStat : addition with Vector of different dimension"); }
	else
	{
        this->Vector<Elem>::operator+=(vect);//Appel a classe de base si conditions respectees
	}
}

/**op-= soustraction**/
template <typename Elem, std::size_t Dimens>
void VectStat<Elem, Dimens>::operator-=(const Vector<Elem>& vect)
{
	//Comportement non permissif (dimension identique seulement)
	if (_dim != vect.siz())
	{ throw std::length_error("VectStat : substraction with Vector of greater dimension"); }
	else
	{
        this->Vector<Elem>::operator-=(vect);//Appel a classe de base si conditions respectees
	}
}


/**--------------------HORS CLASSE**/

/**addition binaire entre VectStat<Elem, Dim>**/
template <typename Elem, std::size_t Dimens>
VectStat<Elem, Dimens> operator+(const VectStat<Elem, Dimens>& vectA, const VectStat<Elem, Dimens>& vectB)
{
	VectStat<Elem, Dimens> resVect(vectA); //Ctr copie
    resVect += vectB;
    return resVect; //Retourne par valeur (effc++)
}

/**soustraction binaire entre VectStat<Elem, Dim>**/
template <typename Elem, std::size_t Dimens>
VectStat<Elem, Dimens> operator-(const VectStat<Elem, Dimens>& vectA, const VectStat<Elem, Dimens>& vectB)
{
	VectStat<Elem, Dimens> resVect(vectA); //Ctr copie
    resVect -= vectB;
    return resVect; //Retourne par valeur (effc++)
}

/**multiplication par element**/
template <typename Elem, std::size_t Dimens>
VectStat<Elem, Dimens> operator*(const VectStat<Elem, Dimens>& vect, const Elem& element)
{
    VectStat<Elem, Dimens> newVect(vect); //Ctr copie
    newVect *= element;
    return newVect; //Retourne par valeur (effc++)
}
template <typename Elem, std::size_t Dimens>
VectStat<Elem, Dimens> operator*(const Elem& element, const VectStat<Elem, Dimens>& vect)
{
    return vect * element; //Appelle fonction en inversant param.
}

/**plus unaire**/
template <typename Elem, std::size_t Dimens>
VectStat<Elem, Dimens> operator+(const VectStat<Elem, Dimens>& vect)
{
    VectStat<Elem, Dimens> newVect(vect); //Ctr copie
    newVect.unary_plus();
    return newVect; //Retourne par valeur (effc++)
}

/**moins unaire**/
template <typename Elem, std::size_t Dimens>
VectStat<Elem, Dimens> operator-(const VectStat<Elem, Dimens>& vect)
{
    VectStat<Elem, Dimens> newVect(vect); //Ctr copie
    newVect.unary_minus();
    return newVect; //Retourne par valeur (effc++)
}


#endif //VECTSTAT_H
