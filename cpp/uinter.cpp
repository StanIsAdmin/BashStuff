#include <iostream>



using namespace std;


/* Uinter represents a subset of the set of real numbers, made out of a finite number of 
 * intervals whose bornes are integers.
 * */
class Uinter {
    
  private:
    
	// Interval class
    class _Inter {
        friend class Uinter; // Give access to private members of _Inter
      private:
        int _lb, _ub;   // Lower and upper bornes
        _Inter* _next;  // Pointer to next interval (list)
      
	  public:
        _Inter(int lb, int ub, _Inter* next) {_lb=lb; _ub=ub; _next=next;};  // Constructor
        bool contains(int a) {return (a>=_lb and a<=_ub);}; // true if 'a' is in interval (inclusive)
        void printInter() {cout << "[" << _lb << ", " << _ub << "]";}; // Representation
    };
    
    // Atributes
    _Inter* _head;  // First interval
    bool isEmpty(); // True if _lb equals _ub
    
  public:
    
	Uinter(): _head(nullptr) {} // Constructor
    void unite(int lb, int ub); // Adds interval [lb, ub]
    bool contains(int a); // true if any interval contains 'a'
	void printUinter(); // Representation
    ~Uinter();  // Destructor
};


/* Returns true if the whole interval is empty, false otherwise */
bool Uinter::isEmpty()
{
    return not bool(_head); // If _head is nullptr, cast to bool returns false (c++11)
}


/* Returns true if the interval contains 'a', false otherwise */
bool Uinter::contains(int a)
{
    bool result = false; // Result
    
    if (not this->isEmpty())
    {
        _Inter* nextInter = _head; // First interval
        bool answer_found = false; // Has the result been found
        
		// While there's a next interval AND answer hasn't been found
        do
        {
			// If 'a' is lower than next interval's _lb, result is false
            if (a < nextInter->_lb) answer_found = true;
            else
                // Otherwise if the next interval contains 'a', result is true
                if (nextInter->contains(a))
                {
                    result = true;
                    answer_found = true;
                }
            nextInter = nextInter->_next; // Advance one interval
        } while (not (nextInter or answer_found));
    }
    return result;
}

/* Displays all intervals */
void Uinter::printUinter()
{
    if (this->isEmpty()) cout << "[]" << endl; // Empty interval
    
	else 
    {
        _Inter* nextInter = _head;
        nextInter->printInter();
        while(nextInter->_next)
        {
            cout << " u ";
            nextInter = nextInter->_next;
            nextInter->printInter();
        }
        cout << endl;
    }
}

/* Adds the interval [lb, ub] to Uinter */
void Uinter::unite(int lb, int ub)
{
    if (lb > ub) {int tmp = lb; lb=ub; ub=tmp;} // Swap if necessary

    if (this->isEmpty()) 
    {
		// Case 1 : first interval
        _head = new _Inter(lb, ub, nullptr); // Create new interval
    }
    else 
    {
        unsigned count = 0;             // Count of intervals looked at before prevInter
        _Inter* beforePrev = _head;     // Interval before previous interval
        _Inter* prevInter = _head;      // Previous interval
        _Inter* nextInter = _head;      // Next interval

		// Look for first interval with upper borne >= lb
        while (lb > prevInter->_ub and prevInter->_next) 
        {
            count++;
            beforePrev = prevInter;
            prevInter = prevInter->_next; // Advance one interval
        }
        
        nextInter = prevInter;
		
		// Look for last interval with lower borne >= lb
        while (ub > nextInter->_lb and nextInter->_next) 
        {
            nextInter = nextInter->_next;
            // Delete intermediate ("covered") intervals
            if (nextInter!=prevInter and count>1) delete prevInter->_next;
            prevInter->_next = nextInter; // Readjust prevInter's "next" pointer
        }

        bool lb_lwr = (lb <= prevInter->_ub); // true if lb not contained between prev and next
        bool ub_grt = (ub >= nextInter->_lb); // true if ub not contained between prev and next

		// Case 2 : interval on side of list 
		//			or without any intersection with other intervals
		//			or entirely contained in other interval (do nothing)
        if (prevInter == nextInter) 
        {
            bool lb_in = prevInter->contains(lb); //lb in prevInter ?
            bool ub_in = prevInter->contains(ub); //ub in nextInter ?
            
			// If there is an intersection between new interval and prevInter, expand prevInter
            if (lb_in or ub_in or (lb_lwr and ub_grt)) 
            {
                prevInter->_lb = lb < prevInter->_lb ? lb : prevInter->_lb; //lb MIN (prev)
                prevInter->_ub = ub > prevInter->_ub ? ub : prevInter->_ub; //ub MAX (prev)
            }
			
            // Otherwise, add new interval
            else 
            {
                if (lb < prevInter->_lb) // Before prevInter
                {
                    if (count<1) _head = new _Inter(lb, ub, prevInter);
                    else beforePrev->_next = new _Inter(lb, ub, prevInter);
                }
                else // After previnter (end of list)
                {
                    prevInter->_next = new _Inter(lb, ub, nullptr);
                }
            }
        }
        
		// Case 3 : interval "between" two others, with intersection
        else 
        {
            if (lb_lwr) 
            {
                if (ub_grt)     // Merge two intervals (two intersections)
                {
                    prevInter->_lb = lb < prevInter->_lb ? lb : prevInter->_lb; //lb MIN(prev&next)
                    prevInter->_ub = ub > nextInter->_ub ? ub : nextInter->_ub; //ub MAX(prev&next)
                    prevInter->_next = nextInter->_next; // Redirect next pointer
                }
                else            // Merge with previous (one intersection)
                {
                    prevInter->_lb = lb < prevInter->_lb ? lb : prevInter->_lb; //lb MIN (prev)
                    prevInter->_ub = ub > prevInter->_ub ? ub : prevInter->_ub; //ub MAX (prev)
                }
            }
            else 
            {
                if (ub_grt)     // Merge with next (one intersection)
                {
                    nextInter->_lb = lb < nextInter->_lb ? lb : nextInter->_lb; //lb MIN (next)
                    nextInter->_ub = ub > nextInter->_ub ? ub : nextInter->_ub; //ub MAX (next)
                }
            }
        }
    }
}

/* Destructor */
Uinter::~Uinter()
{
    if (not this->isEmpty()) 
    {  
        _Inter* thisInter = _head; // Current interval
        _Inter* copyInter = _head; // Copy, allows delete
        
		// Delete each interval
        do
        {
            thisInter = thisInter->_next;
            delete copyInter;
            copyInter = thisInter;
        } while (thisInter);
    }
}

/* Tests */
int main()
{
    
    cout << endl << "------ TEST ------" << endl ;
	Uinter dom;
    
    cout << "Two bornes exceed 1,11" << endl;
    dom = Uinter();
    dom.unite(8,10);
    dom.unite(2,5);
    dom.unite(1,11);
    dom.printUinter();
    cout << endl;

    cout << "Two bornes contained 2,10" << endl;
    dom = Uinter();
    dom.unite(8,10);
    dom.unite(2,5);
    dom.unite(3,9);
    dom.printUinter();
    cout << endl;

    cout << "Two bornes surrounded 2,5 u 6,7 u 8,10" << endl;
    dom = Uinter();
    dom.unite(8,10);
    dom.unite(2,5);
    dom.unite(6,7);
    dom.printUinter();
    cout << endl;

    cout << "Lower borne exceeds 1,6 u 8,10" << endl;
    dom = Uinter();
    dom.unite(8,10);
    dom.unite(2,5);
    dom.unite(1,6);
    dom.printUinter();
    cout << endl;

    cout << "Upper borne exceeds 2,5 u 7,11" << endl;
    dom = Uinter();
    dom.unite(8,10);
    dom.unite(2,5);
    dom.unite(7,11);
    dom.printUinter();
    cout << endl;

    cout << "Lower borne pushed left 1,5 u 8,10" << endl;
    dom = Uinter();
    dom.unite(8,10);
    dom.unite(2,5);
    dom.unite(1,4);
    dom.printUinter();
    cout << endl;

    cout << "Upper borne pushed right 2,6 u 8,10" << endl;
    dom = Uinter();
    dom.unite(8,10);
    dom.unite(2,5);
    dom.unite(2,6);
    dom.printUinter();
    cout << endl;

    cout << "Lower borne pushed left 2,5 u 7,10" << endl;
    dom = Uinter();
    dom.unite(8,10);
    dom.unite(2,5);
    dom.unite(7,10);
    dom.printUinter();
    cout << endl;

    cout << "Upper borne pushed right 2,5 u 8,12" << endl;
    dom = Uinter();
    dom.unite(8,10);
    dom.unite(2,5);
    dom.unite(9,12);
    dom.printUinter();
    cout << endl;

    cout << "Unchanged 2,8" << endl;
    dom = Uinter();
    dom.unite(2,8);
    dom.unite(2,8);
    dom.printUinter();
    cout << endl;

    cout << "Both bornes exceed 1,9" << endl;
    dom = Uinter();
    dom.unite(2,8);
    dom.unite(1,9);
    dom.printUinter();
    cout << endl;

		
    cout << "All intervals covered -100, 1000" << endl;
    dom = Uinter();
    dom.unite(2,8);
    dom.unite(2,9);
    dom.unite(11,100);
    dom.unite(12, 120);
    dom.unite(-90, -5);
    dom.unite(-100,1000);
    dom.printUinter();
    cout << endl;

    cout << "All intervals merged -100, 1000" << endl;
    dom = Uinter();
    dom.unite(1,20);
    dom.unite(30,100);
    dom.unite(-90,-10);
    dom.unite(-10,1);
    dom.unite(-100, -2);
    dom.unite(18,32);
    dom.unite(18,1000);
    dom.printUinter();
    cout << endl;

    return 0;
}

