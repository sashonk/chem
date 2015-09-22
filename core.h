#ifndef CORE_H
#define CORE_H


#include "types.h"


#include <stdio.h>
#include <conio.h>

#include <list>
#include <iostream>
#include <map>
#include <algorithm>
#include <fstream>
#include <set>
#include <strstream>
#include <iomanip> 


extern void print_out_el(const Element* el);


extern void analize(const char* input);


extern void enter_masses();


extern void enter_quantities();



extern void print_reactants();

extern void prepare();

extern void calculate();


extern void populate_table(const char * fileName);


extern void set_units();



//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////  CLASS CONTEXT ////////////////////////
//////////////////////////////////////////////////////////////////////////////

class context{
public:


 bool equation_good;
 double conversion;
 double min_normal_quantity;

 std::set<Element*> qualitatives;
 std::map<int, Compound*> lchems;	// <id, chem>
 std::map<int, Compound*> rchems;

 std::map<int, int> lstechio; 		// <id, stechio coeff>
 std::map<int, int> rstechio;

 std::map<int, double> lquants;	// <id, abs quantity>
 std::map<int, double> rquants;	// <id, abs quantity>


 std::list<Reactant*> reactants;

 void drop_reactants();


 void drop();


};










//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////  CLASS CHEMICAL_TABLE ////////////////////////
//////////////////////////////////////////////////////////////////////////////

class ChemicalTable{
 public:

  ChemicalTable(){
		
  }

  void add(Element* el);

  Element* find(int sys_number);

  Element* resolve(const char* name);

  ~ChemicalTable();

  void dump();

 private:

  class fsys{
   public:
	fsys(const char* name);
	
	bool operator () (const Element* val);
   private:
	char m_name[10];

  };

  class ftor{
   public:
	ftor(int key);
	
	bool operator () (const Element* val);
   private:
	int m_key;

  };
   
  std::list<Element*> elements;


} ;







//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////  CLASS UNIT_MANAGER ////////////////////////
//////////////////////////////////////////////////////////////////////////////


struct Unit{

	const char* name;
	double ratio;

};

class unit_manager{
public:
	unit_manager();




	////////////// MASS /////////////

	Unit* get_mass_unit();

	void set_mass_unit(int id);

	double get_mass(double native_mass);

	double get_native_mass(double mass);

	std::map<int, Unit*>& get_mass_units_map();





	///////////// QUANTITY /////////////

	Unit* get_quantity_unit();

	void set_quantity_unit(int id);

	double get_quantity(double native_quantity);

	double get_native_quantity(double quantity);

	std::map<int, Unit*>& get_quantity_units_map();





	///////////// MOLAR MASS /////////////

	void get_molmass_str(char* buf);

	double get_molmass(double native_molmass);

	double get_native_molmass(double molmass);


	~unit_manager();

private:

	std::map<int, Unit*> m_mass_units; // <unit_id, coeff = mass/native_mass >
	int mass_unit_id;

	std::map<int, Unit*> m_quantity_units; // <unit_id, coeff = quantity/native_quantity >
	int quantity_unit_id;


	std::map<int, Unit*> m_molmass_units; 
	int molmass_unit_id;


};


#endif