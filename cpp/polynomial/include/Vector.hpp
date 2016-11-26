#ifndef VECTOR_H
#define VECTOR_H

#include <cstddef>
#include <stdexcept>
#include <iostream>


/**GUENIFFEY Stanislas (000377223) INFO-2**/


template <typename Elem>
class Vector
{
	/*Classe abstraite representant un vecteur**/
    protected:
		std::size_t _dim;
        Elem* _values;

	public:
		/**Constructeurs**/
        Vector();								// trivial : vecteur nul
        Vector(const Elem&, std::size_t);		// init: valeur a repeter + taille
        Vector(const Elem*, std::size_t);		// init: tableau valeurs  + taille
        Vector(const Vector<Elem>&);			// de copie
        Vector(Vector<Elem>&&);					// de transfert

        /**Destructeur**/
        virtual ~Vector() =0; //Force classe abstraite (mais definit quand meme destructeur en bas)

        /**Operateurs**/
		virtual inline Elem& operator[] (std::ptrdiff_t);             //Modification
        virtual inline const Elem& operator[] (std::ptrdiff_t) const; //Accès

        virtual Vector<Elem>& operator= (const Vector<Elem>&);   //Assignation (retourne ref. pour assignation en chaine)
        virtual Vector<Elem>& operator= (Vector<Elem>&&);        //Transfert (retourne ref. pour continuer sur assignation en chaine)

		virtual void operator+= (const Vector<Elem>&); //Addition d'un vecteur
        virtual void operator-= (const Vector<Elem>&); //Soustraction d'un vecteur

        virtual void operator*= (const Elem& value) { for (std::size_t i=0; i<_dim; ++i) _values[i] *= value; }; //Multiplication par element


        /**Autres methodes**/
        virtual void unary_plus ();		//+A (modifie sur place)
        virtual void unary_minus ();	//-A (modifie sur place)

        inline std::size_t siz() const { return _dim; }; //retourne la taille (dimension) du vecteur

		virtual std::ostream& print(std::ostream& ostr) const;	//print object to ostr
		virtual std::istream& iread(std::istream& istr);		//read input -> write to object (returns what's LEFT of stream after extraction)

};


/**--------------------CONSTRUCTEURS**/

/**initialisants**/
template <typename Elem>
Vector<Elem>::Vector(): //Trivial
    _dim(0), _values(new Elem[0]) {} //tableau vide


template <typename Elem>
Vector<Elem>::Vector(const Elem& element, std::size_t dim):	//element a repeter, taille du vecteur
    _dim(dim), _values(new Elem[dim])
    {
        for (std::size_t i=0; i<dim; ++i) _values[i] = element;
    }

template <typename Elem>									//tableau d'elements Elem, taille
Vector<Elem>::Vector(const Elem* elemArray, std::size_t dim):
    _dim(dim), _values(new Elem[dim])
    {
        for (std::size_t i=0; i<_dim; ++i) _values[i] = elemArray[i]; //Copie
    }

/**de copie**/
template <typename Elem>
Vector<Elem>::Vector(const Vector<Elem>& vect):
    _dim(vect._dim), _values(new Elem[vect._dim])
    {
        for (std::size_t i=0; i<vect._dim; ++i) _values[i] = vect._values[i]; //Copie
    }

/**de transfert**/
template <typename Elem>
Vector<Elem>::Vector(Vector<Elem>&& vect):
    _dim(vect._dim), _values(vect._values)
    {
        vect._dim = 0; vect._values = nullptr; //"nettoie" vect -> il oublie _values
    }


/**--------------------DESTRUCTEUR**/

template <typename Elem>
Vector<Elem>::~Vector<Elem>() { delete[] _values; }


/**--------------------OPERATEURS**/

/**modification element**/
template <typename Elem>
Elem& Vector<Elem>::operator[] (std::ptrdiff_t i)
{
    if (std::size_t(i) >= _dim) //cast to unsigned
        throw std::out_of_range("Vector : Index out of range");
    return _values[i];
}

/**acces element**/
template <typename Elem>
const Elem& Vector<Elem>::operator[] (std::ptrdiff_t i) const
{
    if (std::size_t(i) >= _dim) //cast to unsigned
        throw std::out_of_range("Vector : Index out of range");
    return _values[i];
}

/**assignation**/
template <typename Elem>
Vector<Elem>& Vector<Elem>::operator= (const Vector<Elem>& vect)
{
	//Suppose taille suffisante pour contenir valeurs de vect
	for (std::size_t i=0; i<vect.siz(); ++i) _values[i] = vect._values[i]; //Copie valeurs de vect
	return *this;
}

/**transfert**/
template <typename Elem>
Vector<Elem>& Vector<Elem>::operator= (Vector<Elem>&& vect)
{
	//Comportement dynamique: dimension devient celle de vect
	_values = vect._values; //recupere tableau de vect
	vect._values = nullptr; //"nettoie" vect
	_dim = vect._dim;
	vect._dim = 0;
	return *this;
}

/**op+= addition d'un vecteur**/
template <typename Elem>
void Vector<Elem>::operator+= (const Vector<Elem>& vect)
{
	//Suppose taille suffisante pour contenir valeurs de vect
    for (std::size_t i=0; i<vect._dim; ++i) _values[i] += vect._values[i]; //Additionne les valeurs de vect
}

/**op-= soustraction d'un vecteur**/
template <typename Elem>
void Vector<Elem>::operator-= (const Vector<Elem>& vect)
{
	//Suppose taille suffisante pour contenir valeurs de vect
    for (std::size_t i=0; i<vect._dim; ++i) _values[i] -= vect._values[i]; //On soustrait les valeurs de vect
}


/**--------------------AUTRES METHODES**/

/**plus unaire**/
template <typename Elem>
void Vector<Elem>::unary_plus ()
{
    //Effectue le plus unaire pour chaque element du vecteur
    //(plus unaire pourrait etre defini comme valeur absolue pour type Elem...)
    for (std::size_t i(0); i<_dim; ++i) _values[i] = +_values[i];
}

/**moins unaire**/
template <typename Elem>
void Vector<Elem>::unary_minus ()
{
    //Effectue le moins unaire pour chaque element du vecteur
    for (std::size_t i=0; i<_dim; ++i) _values[i] = -_values[i];
}

/**ecriture sur ostream**/
template <typename Elem>
std::ostream& Vector<Elem>::print(std::ostream& ostr) const
{
	ostr << "[";
	for (std::size_t i=0; i<_dim; ++i)
	{
		ostr << _values[i];//Ecrit chaque valeur sur stream, separees par virgules
		if (i != _dim-1) ostr << ", ";
	}
	ostr << "]";
	return ostr;
}

/**lecture d'un istream**/
template <typename Elem>
std::istream& Vector<Elem>::iread(std::istream& istr)
{
    std::size_t i = 0;
    //Tant que fin de ligne pas atteinte, extrait valeurs du stream (valable pour valeurs arithmetiques)
    while (istr.peek()!='\n' and i<_dim) { istr>>_values[i]; ++i; };
    //S'arrete egalement si arrive a fin du vecteur
    return istr; //retourne ref. vers stream (modifie)
}



/**--------------------HORS CLASSE**/

/**operateur+ unaire**/
template <typename Elem>
void operator+(Vector<Elem>& vect) { vect.unary_plus(); } //S'applique directement a vect

/**operateur- unaire**/
template <typename Elem>
void operator-(Vector<Elem>& vect) { vect.unary_minus(); } //S'applique directement a vect


/**output**/
template <typename Elem>
std::ostream& operator<<(std::ostream& ostr, const Vector<Elem>& vect)
{
	//Utilise methode propre a la classe pour ecrire sur stream
	return vect.print(ostr);  //retourne stream
}

/**input**/
template<typename Elem>
std::istream& operator>>(std::istream& istr, Vector<Elem>& vect)
{
	//Utilise methode propre a la classe pour lire le stream
    return vect.iread(istr); //retourne stream
}

#endif // VECTOR_H
