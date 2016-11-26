#ifndef VECTDYN_H
#define VECTDYN_H

#include <cstddef>
#include <stdexcept>

#include "Vector.hpp"


/**GUENIFFEY Stanislas (000377223) INFO-2**/


template <typename Elem>
class VectDyn: virtual public Vector<Elem>
{
	protected:
		using Vector<Elem>::_dim;
		using Vector<Elem>::_values;

	public:
		/**Constructeurs**/
        VectDyn();							// trivial : vecteur nul
        VectDyn(const Elem&, std::size_t);	// init: valeur a repeter + taille
        VectDyn(const Elem*, std::size_t);	// init: tableau valeurs  + taille
        VectDyn(const VectDyn<Elem>&);		// de copie
        VectDyn(VectDyn<Elem>&&);			// de transfert
        VectDyn(const Vector<Elem>&);		// de conversion (permet implicite)

        /**Destructeur**/
        //Pas besoin de destructeur autre que celui de classe de base

        /**Operateurs**/
        virtual VectDyn<Elem>& operator=(const Vector<Elem>&) override; //Transfert
        virtual VectDyn<Elem>& operator=(Vector<Elem>&&) override; 		//Copie
        virtual VectDyn<Elem>& operator=(const VectDyn<Elem>&); //Doit redefinir
        virtual VectDyn<Elem>& operator=(VectDyn<Elem>&&); 		//Pourrait =default, avec Warning

		virtual void operator+=(const Vector<Elem>&) override; //Addition d'un vecteur
		virtual void operator-=(const Vector<Elem>&) override; //Soustraction d'un vecteur
};


/**--------------------CONSTRUCTEURS**/

/**initialisants**/
template <typename Elem>
VectDyn<Elem>::VectDyn(): //Trivial
	Vector<Elem>() {} //Cree Vecteur vide

template <typename Elem>
VectDyn<Elem>::VectDyn(const Elem& element, std::size_t dim):
    Vector<Elem>(element, dim) {} //Repete element dim fois

template <typename Elem>
VectDyn<Elem>::VectDyn(const Elem* elemArray, std::size_t dim):
    Vector<Elem>(elemArray, dim) {} //Prend dim elements de elemArray

/**de copie**/
template <typename Elem>
VectDyn<Elem>::VectDyn(const VectDyn<Elem>& vect):
    Vector<Elem>(vect) {} //Appelle ctr copie

/**de transfert**/
template <typename Elem>
VectDyn<Elem>::VectDyn(VectDyn<Elem>&& vect):
    Vector<Elem>(std::forward<VectDyn<Elem>>(vect)) {} //Appelle ctr transfert

/**de conversion**/
template <typename Elem>
VectDyn<Elem>::VectDyn(const Vector<Elem>& vect):
    Vector<Elem>(vect) {} //Aucune modification a apporter (ctr copie)


/**--------------------OPERATEURS**/

/**assignation a vecteur**/
template <typename Elem>
VectDyn<Elem>& VectDyn<Elem>::operator=(const Vector<Elem>& vect)
{
    if (_dim != vect.siz()) //Comportement dynamique: ajuste taille si differente
    {
		_dim = vect.siz();
		Elem* oldValues = _values;
		_values = new Elem[_dim];
		delete[] oldValues;
    }
    this->Vector<Elem>::operator=(vect);//Appelle op. copie
	return *this;
}
/**assignation a vecteur dynamique**/
template <typename Elem>
VectDyn<Elem>& VectDyn<Elem>::operator=(const VectDyn<Elem>& vect)
{
    if (_dim != vect.siz()) //Comportement dynamique: ajuste taille si differente
    {
		_dim = vect.siz();
		Elem* oldValues = _values;
		_values = new Elem[_dim];
		delete[] oldValues;
    }
    this->Vector<Elem>::operator=(vect);//Appelle op. copie
	return *this;
}

/**transfert d'un vecteur**/
template <typename Elem>
VectDyn<Elem>& VectDyn<Elem>::operator=(Vector<Elem>&& vect)
{
	this->Vector<Elem>::operator=(std::forward<Vector<Elem>>(vect)); //Appelle op. transfert
	return *this;
}
/**transfert d'un vecteur dynamique**/
template <typename Elem>
VectDyn<Elem>& VectDyn<Elem>::operator=(VectDyn<Elem>&& vect)
{
	this->Vector<Elem>::operator=(std::forward<Vector<Elem>>(vect)); //Appelle op. transfert
	return *this;
}


/**op+= addition**/
template <typename Elem>
void VectDyn<Elem>::operator+=(const Vector<Elem>& vect)
{
    if (vect.siz() > _dim) //Comportement dynamique: ajuste taille si necessaire
    {
        Elem* oldValues = _values;
        _dim = vect.siz();
        _values = new Elem[_dim]; //Nouveau tableau
        for (std::size_t i=0; i<_dim; ++i) { _values[i] = oldValues[i]; } //Copie
        delete[] oldValues;
    }
    this->Vector<Elem>::operator+=(vect); //Appel a methode de base
}

/**op-= soustraction**/
template <typename Elem>
void VectDyn<Elem>::operator-=(const Vector<Elem>& vect)
{
    if (vect.siz() > _dim) //Comportement dynamique: ajuste taille si necessaire
    {
        Elem* oldValues = _values;
        _dim = vect.siz();
        _values = new Elem[_dim]; //Nouveau tableau
        for (std::size_t i=0; i<_dim; ++i) { _values[i] = oldValues[i]; } //Copie
        delete[] oldValues;
    }
    this->Vector<Elem>::operator-=(vect); //Appel a methode de base
}


/**--------------------HORS CLASSE**/

/**addition binaire entre Vector<Elem>**/
template <typename Elem>
VectDyn<Elem> operator+(const Vector<Elem>& vectA, const Vector<Elem>& vectB)
{
    VectDyn<Elem> resVect(vectA);//Copie de vectA comme vecteur dynamique
    resVect += vectB;
    return resVect; //Retourne par valeur (effc++)
}

/**soustraction binaire entre Vector<Elem>**/
template <typename Elem>
VectDyn<Elem> operator-(const Vector<Elem>& vectA, const Vector<Elem>& vectB)
{
    VectDyn<Elem> resVect(vectA); //Copie de vectA comme vecteur dynamique
    resVect -= vectB;
    return resVect; //Retourne par valeur (effc++)
}

/**multiplication par element**/
template <typename Elem>
VectDyn<Elem> operator*(const VectDyn<Elem>& vect, const Elem& element)
{
    VectDyn<Elem> newVect(vect); //Ctr copie
    newVect *= element;
    return newVect; //Retourne par valeur (effc++)
}
template <typename Elem>
VectDyn<Elem> operator*(const Elem& element, const VectDyn<Elem>& vect)
{
	return vect * element; //Appelle fonction en inversant param.
}


/**plus unaire**/
template <typename Elem>
VectDyn<Elem> operator+(const VectDyn<Elem>& vect)
{
    VectDyn<Elem> newVect(vect); //Ctr copie
    newVect.unary_plus();
    return newVect; //Retourne par valeur (effc++)
}

/**moins unaire**/
template <typename Elem>
VectDyn<Elem> operator-(const VectDyn<Elem>& vect)
{
    VectDyn<Elem> newVect(vect); //Ctr copie
    newVect.unary_minus();
    return newVect; //Retourne par valeur (effc++)
}



#endif //VECTDYN_H
