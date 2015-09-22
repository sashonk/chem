#include "types.h"


#include <map>
#include <algorithm>

#include <strstream>
using namespace std;




//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////  CLASS COMPOUND ////////////////////////
//////////////////////////////////////////////////////////////////////////////

void Compound::add(Element* el){
	std::map<Element*, int>::iterator it = find_if(elements.begin(), elements.end(), eq_ftor(el));
	if(it!=elements.end()){
		it->second++;		
	}
	else{
		elements.insert(make_pair(el, 1));
	}

	update_formula();

 }


int Compound:: element_count(Element* el){
   	map<Element*, int>::iterator it = find_if(elements.begin(), elements.end(), eq_ftor(el));
	if(it!=elements.end()){
		return it->second;
	}

	
	return 0;
  } 


double Compound::calculate_M(){
    double M = 0;
    for(map<Element*, int>::iterator it = elements.begin(); it!=elements.end(); it++){
	M+= ((it->first->M) * (it->second));
	
	
    }
    

    return M;
  }   



void Compound::update_formula(){

    strstream s;
    for(map<Element*, int>::iterator it = elements.begin(); it!=elements.end(); it++){
	s << it->first->sys_name;
	
	if(it->second>1){
		
		s << it->second;
	}
	
    }
    s << '\0';

    strcpy_s(form, s.str());

 }



Compound::eq_ftor::eq_ftor(Element* el){
		m_el = el;
}

bool Compound::eq_ftor::operator () (const pair<Element*, int> rval){
		return rval.first->number==m_el->number;

}









//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////  CLASS REACTANT ////////////////////////
//////////////////////////////////////////////////////////////////////////////

Reactant:: Reactant(Compound* comp, double normal_quantity, bool is_product /* = false*/, int stech_coeff  /* = 1 */){
	m_quantity = normal_quantity;
	m_comp = comp;
	m_is_product = is_product;
	m_coeff = stech_coeff;
 }

int Reactant::get_coeff(){
	return m_coeff;
 }


bool Reactant::is_product(){
	return m_is_product;
 }


double Reactant::get_normal_quantity(){
	return m_quantity;
 }


void Reactant::set_normal_quantity(double quantity){
	this->m_quantity = quantity;
 }

double Reactant::get_quantity(){
	return m_quantity*m_coeff;
 }


double Reactant::get_mass(){
	return m_quantity*m_coeff* m_comp->calculate_M();
 }

Compound* Reactant::get_comp(){
	return m_comp;
 }