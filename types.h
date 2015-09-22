#ifndef TYPES_H
#define TYPES_H



#include <map>




//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////  /STRUCT ELEMENT  ////////////////////////
//////////////////////////////////////////////////////////////////////////////

struct Element{

 char sys_name[10];

 int number;
 double M;


};



//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////  CLASS COMPOUND ////////////////////////
//////////////////////////////////////////////////////////////////////////////


class Compound{
public: 

  const char* formula(){
	return form;
  }

  void add(Element* el);


  int element_count(Element* el);

  double calculate_M(); 

private:

  void update_formula();

  class eq_ftor{
   public:
	eq_ftor(Element* el);

	bool operator () (const std::pair<Element*, int> rval);
   private:
	Element* m_el;
  };

  std::map<Element*, int> elements;
  char form[50];
};







//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////  CLASS REACTANT ////////////////////////
//////////////////////////////////////////////////////////////////////////////

class Reactant{
public:
 Reactant(Compound* comp, double normal_quantity, bool is_product = false, int stech_coeff = 1);

 int get_coeff();

 bool is_product();

 double get_normal_quantity();

 void set_normal_quantity(double quantity);


 double get_quantity();

 double get_mass();

 Compound* get_comp();

private:
 double m_quantity;
 Compound* m_comp;
 bool m_is_product;
 int m_coeff;


};


#endif
