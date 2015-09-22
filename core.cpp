
#include "core.h"
#include "types.h"
#include "util.h"


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
using namespace std;

Trace trace;

ChemicalTable table;
context ctx;
unit_manager unit_mgr;

int code = -1;
const char* table_file_name = "table.txt";

void print_out_el(const Element* el){

  char buf[20];
  memset(buf, 0, sizeof(buf));
  unit_mgr.get_molmass_str(buf);
  std::cout << "name=" << el->sys_name  << "   num=" << el->number << "   M=" << unit_mgr.get_molmass(el->M) << " [" << buf << ']' << '\n';
 
}

//returns total read
int read_int(const char* cursor, int* value){
	//trace << "read_int.." << '\n';

	const char* p = cursor;
	

	char buf[10];
	memset(buf, 0, sizeof(buf));
	int index = 0;
	while(*p){
		char c = *p;
		if(isdigit(c)){
			buf[index] = c;			
		}
		else{
			break;
		}
		
		index++;
		p++;
	}

	*value = atoi(buf);

	return index;

}

//returns total read
int read_el(const char* cursor,  char* out){
	const char* p = cursor;
	
	char buf[10];
	memset(buf, 0, sizeof(buf));
	int index = 0;
	
	buf[0] = *p;
	p++;
	index++;

	while(*p==tolower(*p) && isalpha(*p)){
		buf[index] = *p;	

		p++;
		index++;
	}	
	
	char const *  ptr = buf;
	strcpy(out, ptr);
	return index;		

} 

void read_group_mul(const char* cursor, int* val){
	//trace << "read group.." << '\n';
	const char* p = cursor;

	while(*p!=')'){
		p++;
	}

	p++;
	if(isdigit(*p)){
		read_int(p, val);
	}
	else{
		*val = 1;
	}	

	//trace << "read group succeed.. val=" << *val << '\n';

}

bool validate(){

	//trace<< "start validation " << '\n';


	bool valid = true;

	if(ctx.qualitatives.size()==0){
		cout << "no elements found" << endl;
		valid = false;
	}

	for(set<Element*>::iterator it = ctx.qualitatives.begin(); it!= ctx.qualitatives.end(); it++){
		int lcount = 0;
		for(map<int, Compound*>::iterator iter = ctx.lchems.begin(); iter!=ctx.lchems.end(); iter++){
			lcount+= iter->second->element_count(*it) * ctx.lstechio[iter->first];

		}

		int rcount = 0;
		for(map<int, Compound*>::iterator iter = ctx.rchems.begin(); iter!=ctx.rchems.end(); iter++){
			rcount+= iter->second->element_count(*it) * ctx.rstechio[iter->first];

		}

		if(lcount!=rcount){

			valid = false;
			break;
		}
			

	}

	return valid;

}

void print_data(){


	char buf[20];
	memset(buf, 0, sizeof(buf));
	unit_mgr.get_molmass_str(buf);

	//trace<< "Left : " << '\n';
	for(map<int, Compound*>::iterator it = ctx.lchems.begin(); it!= ctx.lchems.end(); it++){
		//trace<< it->second->formula() << " Nu=" << ctx.lstechio[it->first] << " M=" << unit_mgr.get_molmass(it->second->calculate_M()) << " [" << buf << ']' << '\n';

	}	

	//trace<< "\nRight : " << '\n';
	for(map<int, Compound*>::iterator it = ctx.rchems.begin(); it!= ctx.rchems.end(); it++){
		//trace<< it->second->formula() << " Nu=" << ctx.rstechio[it->first] << " M=" << unit_mgr.get_molmass(it->second->calculate_M()) << " [" << buf << ']' << '\n';

	}

	//trace<< "\nElements : " << '\n';
	for(set<Element*>::iterator it = ctx.qualitatives.begin(); it!=ctx.qualitatives.end(); it++){
		//trace<< (*it)->sys_name << '\n';


	}	

	//trace<< '\n';

}

void analize(const char* input){

		//trace<< "start analizing.. " << input << '\n';

		ctx.drop();
	

 		const char* cursor = input;

		int index = 0;
		int atom_mul = 1;
		int comp_mul = 1;
		int group_mul = 1;
		

		bool compounding = false;
		bool left = true;
		Compound* comp = NULL;
		

		
		while(*cursor){


			char key = *cursor;

		
			if(isspace(key)){
				compounding = false;
				cursor++;
				continue;
			}
			else if(isdigit(key)){
				
				if(!compounding){
					index = read_int(cursor, &comp_mul);
					cursor += index;

				}
				else{
					cursor++;
				}
			}
			else{
				if (key == '+'){
					compounding = false;
					cursor++;
				}				
				else if(key == '='){
					
						
					compounding = false;
					left = false;
					cursor++;
					
				}
				else if(key=='('){
					
									
					cursor++;

					read_group_mul(cursor, &group_mul);
				}
				else if(key==')'){
					//trace << "found )" << '\n';
					
					group_mul = 1;
					cursor++;
				}
				else if(isalpha(key)){
					if(!compounding){
					 compounding = true;
					 comp = new Compound;
					 
					 if(left)
					 {												
						
						int index = ctx.lchems.size();
						ctx.lchems.insert(make_pair(index, comp));
						ctx.lstechio.insert(make_pair(index, comp_mul));
					 }
					 else{
						
						int index = ctx.lchems.size()+ctx.rchems.size();
						ctx.rchems.insert(make_pair(index, comp));
						ctx.rstechio.insert(make_pair(index, comp_mul));
					 }

					 comp_mul=1;
					}

					
					 char token[10];
					 int index = read_el(cursor, token);
					
					 Element* el = table.resolve(token);
					 if(!el)
					  return;

					 ctx.qualitatives.insert(el);

					 atom_mul = 1;
					 cursor+= index;

					 if(isdigit(*cursor)){
						index = read_int(cursor, &atom_mul);
						cursor += index;

					 }
	
				

					 for(int i = 0; i<atom_mul*group_mul; i++)
					  comp->add(el);
					
					
				}
				else{
					cerr << "unknown symbol" << endl;	
					return;

				}

			}
			
			
		}

		print_data();


		bool valid = validate();
		if(valid){
			ctx.equation_good = true;
		}
	
}

void enter_masses(){
		cout << "Left: " << endl;
		for(map<int, Compound*>::iterator it = ctx.lchems.begin(); it!=ctx.lchems.end(); it++){
			cout << it->second->formula()  << " - " << it->first << endl;

		}
		
		cout << "Right: " << endl;
		for(map<int, Compound*>::iterator it = ctx.rchems.begin(); it!=ctx.rchems.end(); it++){
			cout << it->second->formula() << " - " << it->first << endl;
		}

		char buf[20];
		
		gets_s(buf);
				
	
		int id = atoi(buf);
		bool product = false;
		map<int, Compound*>::iterator it = ctx.lchems.find(id);		
		if(it==ctx.lchems.end()){

			it = ctx.rchems.find(id);
			product = true;

			if(it==ctx.rchems.end()){
				cout << "invalid argument! " <<endl;
				return;	
			}
		}		
		
		cout << "mass (" <<  unit_mgr.get_mass_unit()->name    <<") : " << endl;
		gets_s(buf);

		double mass = unit_mgr.get_native_mass(atof(buf));

		if(product){
			map<int, double>::iterator iter = ctx.rquants.find(id);				
			if(iter!=ctx.rquants.end()){
				iter->second =  mass / ctx.rchems[id]->calculate_M();
			}
			else{
				ctx.rquants.insert(make_pair(id,  mass / ctx.rchems[id]->calculate_M()));
			}			
		}
		else{
			map<int, double>::iterator iter = ctx.lquants.find(id);				
			if(iter!=ctx.lquants.end()){
				iter->second =  mass / ctx.lchems[id]->calculate_M();
			}
			else{
				ctx.lquants.insert(make_pair(id, mass / ctx.lchems[id]->calculate_M()));
			}
		}
		
		cout << "OK\n" <<endl;


}


void enter_quantities(){



		cout << "Left: " << endl;
		for(map<int, Compound*>::iterator it = ctx.lchems.begin(); it!=ctx.lchems.end(); it++){
			cout << it->second->formula()  << " - " << it->first << endl;

		}
		
		cout << "Right: " << endl;
		for(map<int, Compound*>::iterator it = ctx.rchems.begin(); it!=ctx.rchems.end(); it++){
			cout << it->second->formula()  << " - " << it->first << endl;
		}

		char buf[20];
		gets_s(buf);
				
	
		int id = atoi(buf);
		bool product = false;
		map<int, Compound*>::iterator it = ctx.lchems.find(id);		
		if(it==ctx.lchems.end()){

			it = ctx.rchems.find(id);
			product = true;

			if(it==ctx.rchems.end()){
				cout << "invalid argument! " <<endl;
				return;	
			}
		}		
		
		cout << "quantity (" <<  unit_mgr.get_quantity_unit()->name    <<  ") : " << endl;
		gets_s(buf);

		double quantity = unit_mgr.get_native_quantity(atof(buf));

		if(product){
			map<int, double>::iterator iter = ctx.rquants.find(id);				
			if(iter!=ctx.rquants.end()){
				iter->second = quantity;
			}
			else{
				ctx.rquants.insert(make_pair(id, quantity));
			}			
		}
		else{
			map<int, double>::iterator iter = ctx.lquants.find(id);				
			if(iter!=ctx.lquants.end()){
				iter->second = quantity;
			}
			else{
				ctx.lquants.insert(make_pair(id, quantity));
			}
		}
		
		cout << "OK\n" <<endl;		
		
}

void print_reactants(){
	
	for(list<Reactant*>::iterator it = ctx.reactants.begin(); it!=ctx.reactants.end(); it++){
		Reactant* r = (*it);
		cout << r->get_comp()->formula() << "  mass=" << unit_mgr.get_mass(r->get_mass()) <<  unit_mgr.get_mass_unit()->name <<" quantity=" << unit_mgr.get_quantity(r->get_quantity()) << unit_mgr.get_quantity_unit()->name << endl;
	}

	cout << '\n';
}


void prepare(){

	ctx.drop_reactants();

	double conversion = ctx.conversion;

	double min_normal_quantity = -1;
	for(map<int, Compound*>::iterator it = ctx.lchems.begin(); it!=ctx.lchems.end(); it++){
		double normal_quan = ctx.lquants[it->first] / ctx.lstechio[it->first];
		if((min_normal_quantity > normal_quan || min_normal_quantity==-1) && conversion >=0)
			 min_normal_quantity = normal_quan;

		Reactant* reac = new Reactant(it->second, normal_quan, false, ctx.lstechio[it->first]);
		ctx.reactants.push_back(reac);
		
	}	

	for(map<int, Compound*>::iterator it = ctx.rchems.begin(); it!=ctx.rchems.end(); it++){	
		double normal_quan = ctx.rquants[it->first] / ctx.rstechio[it->first];
		if((min_normal_quantity > normal_quan || min_normal_quantity==-1) && conversion <0)
			 min_normal_quantity = normal_quan;

		Reactant* reac = new Reactant(it->second, normal_quan, true, ctx.rstechio[it->first]);
		ctx.reactants.push_back(reac);
		
	}

	ctx.min_normal_quantity = min_normal_quantity;

}

void calculate(){


	double conversion = ctx.conversion;
	double min_normal_quantity = ctx.min_normal_quantity;	

	
	if(conversion)
	for(list<Reactant*>::iterator it = ctx.reactants.begin(); it!=ctx.reactants.end(); it++){
		Reactant* r = (*it);
		if(r->is_product())
		 r->set_normal_quantity(r->get_normal_quantity() + min_normal_quantity*conversion);
		else
		 r->set_normal_quantity(r->get_normal_quantity() - min_normal_quantity*conversion);

	}	


	

}

void populate_table(char const * fileName){
 
 ifstream ifstr;
 ifstr.open(fileName);

 if(ifstr.good()){

   while(1){
   	Element * el = new Element;
   	ifstr >> el->number >> el->sys_name >> el->M;
	if(ifstr.eof())
		break;

   	table.add(el);
   }
 }
 else{
  cerr << "can't open " << fileName << endl;

 }

 ifstr.close();

}




void set_units(){
		cout << "mass - 1" << endl;
		cout << "quantity - 2" << endl;

		char buf[3];
		gets_s(buf);
		
		int choice = atoi(buf);
		
		
		if(choice==1){	
				cout << "set mass units" <<endl;
				map<int, Unit*>& m = unit_mgr.get_mass_units_map();			
				for(map<int, Unit*>::iterator it = m.begin(); it!=m.end(); it++){
					cout << it->second->name << " - " << it->first <<endl;

				}

				char b[3];
				gets_s(b);
				
				int unit_id = atoi(b);
				unit_mgr.set_mass_unit(unit_id);				
				
		}
		else if(choice==2){

				cout << "set quantity units" <<endl;
				map<int, Unit*>& m = unit_mgr.get_quantity_units_map();			
				for(map<int, Unit*>::iterator it = m.begin(); it!=m.end(); it++){
					cout << it->second->name << " - " << it->first <<endl;

				}

				char b[3];
				gets_s(b);
				
				int unit_id = atoi(b);
				unit_mgr.set_quantity_unit(unit_id);


		}
		else{
			cout << "illegal argument" <<endl;
	
		}


}







///////////////////////////////////////// ///////////////////////////////////////// 
///////////////////////////////////// CONTEXT ///////////////////////////
///////////////////////////////////////// ///////////////////////////////////////// 

void context::drop_reactants(){

  		for(list<Reactant*>::iterator it = reactants.begin(); it!= reactants.end(); it++){
			delete (*it);
		}

		reactants.clear();


}


void context::drop(){


		equation_good = false;		
		conversion = 1;
		min_normal_quantity = 0;

		for(map<int, Compound*>::iterator it = lchems.begin(); it!=lchems.end(); it++){
			delete it->second;
		}

		for(map<int, Compound*>::iterator it = rchems.begin(); it!=rchems.end(); it++){
			delete it->second;
		}

  		for(list<Reactant*>::iterator it = reactants.begin(); it!= reactants.end(); it++){
			delete (*it);
		}

		reactants.clear();

		lchems.clear();
		rchems.clear();
		lstechio.clear();
		rstechio.clear();
		
		lquants.clear();
		rquants.clear();
		
 

}






///////////////////////////////////////// ///////////////////////////////////////// 
///////////////////////////////////// CHEMICAL TABLE ///////////////////////////
///////////////////////////////////////// //////////////////////////////////////// 

void ChemicalTable::add(Element* el){
    elements.push_back(el);

  }

Element* ChemicalTable::find(int sys_number){
 	list<Element*>::iterator it = find_if(elements.begin(), elements.end(), ftor(sys_number));
	if(it!=elements.end()){
		return (*it);
	}

	cerr << "entry not found : " << sys_number<< endl;
	return 0;
  }

Element* ChemicalTable::resolve(const char* name){
 	list<Element*>::iterator it = find_if(elements.begin(), elements.end(), fsys(name));
	if(it!=elements.end()){
		return (*it);
	}

	cerr << "entry not resolved : " << name << endl;
	return 0;
  }

ChemicalTable::~ChemicalTable(){
    for(list<Element*>::iterator it = elements.begin(); it!= elements.end(); it++){
	delete (*it);
    }
  }

void ChemicalTable::dump(){
    for_each(elements.begin(), elements.end(), print_out_el);

  }

ChemicalTable::fsys::fsys(const char* name){
		memset(m_name, 0, sizeof(m_name));
		strcpy(m_name, name);
}

bool ChemicalTable::fsys::operator () (const Element* val){
		return strcmp(m_name, val->sys_name)==0;
}


ChemicalTable::ftor::ftor(int key):m_key(key){}



bool ChemicalTable::ftor::operator () (const Element* val){
		return val->number == m_key;	
}


///////////////////////////////////////// ///////////////////////////////////////// 
///////////////////////////////////// UNIT MANAGER ///////////////////////////
///////////////////////////////////////// ///////////////////////////////////////// 

unit_manager::unit_manager(){

	Unit* u = new Unit;
	u->name = "kg";
	u->ratio = 1.0;
	m_mass_units.insert(make_pair(1, u));

	u = new Unit;
	u->name = "g";
	u->ratio = 1000.0;
	m_mass_units.insert(make_pair(2, u));

	u = new Unit;
	u->name = "mg";
	u->ratio = 1000000.0;
	m_mass_units.insert(make_pair(3, u));

	mass_unit_id = 1;


	u = new Unit;
	u->name = "kmol";
	u->ratio = 0.001;
	m_quantity_units.insert(make_pair(1, u));

	u = new Unit;
	u->name = "mol";
	u->ratio = 1.0;
	m_quantity_units.insert(make_pair(2, u));

	u = new Unit;
	u->name = "mmol";
	u->ratio = 1000.0;
	m_quantity_units.insert(make_pair(3, u));
	quantity_unit_id = 1;


}

unit_manager::~unit_manager(){
	for(map<int, Unit*>::iterator it = m_mass_units.begin(); it!= m_mass_units.end(); it++){
		delete it->second;
	}

	for(map<int, Unit*>::iterator it = m_quantity_units.begin(); it!= m_quantity_units.end(); it++){
		delete it->second;
	}
}

double unit_manager::get_mass(double native_mass){
	
	return  native_mass * m_mass_units[mass_unit_id]->ratio ;

}

double unit_manager::get_native_mass(double mass){

	return  mass / m_mass_units[mass_unit_id]->ratio ;
}

void unit_manager::set_mass_unit(int id){
	bool ok = false;
	for(map<int, Unit*>::iterator it = m_mass_units.begin(); it!= m_mass_units.end(); it++){
		if(it->first==id)
			ok= true;
	}

	if(ok){
		mass_unit_id = id;
	}
	else{
		cerr << "mass unit not found!" << endl;
	}
	

}

Unit* unit_manager::get_mass_unit(){

	return m_mass_units[mass_unit_id];
}

map<int, Unit*>& unit_manager::get_mass_units_map(){

	return m_mass_units;
}




double unit_manager::get_quantity(double native_quantity){
	
	return  native_quantity* m_quantity_units[quantity_unit_id]->ratio ;

}

double unit_manager::get_native_quantity(double quantity){

	return  quantity / m_quantity_units[quantity_unit_id]->ratio ;
}

void unit_manager::set_quantity_unit(int id){
	bool ok = false;
	for(map<int, Unit*>::iterator it = m_quantity_units.begin(); it!= m_quantity_units.end(); it++){
		if(it->first==id)
			ok= true;
	}

	if(ok){
		quantity_unit_id = id;
	}
	else{
		cerr << "quantity unit not found!" << endl;
	}


}

Unit* unit_manager::get_quantity_unit(){

	return m_quantity_units[quantity_unit_id];
}

map<int, Unit*>& unit_manager::get_quantity_units_map(){

	return m_quantity_units;
}



void unit_manager::get_molmass_str(char* buf){
	
	Unit* mass_unit = get_mass_unit();
	Unit* quantity_unit = get_quantity_unit();
	
	strcat(buf, mass_unit->name);
	strcat(buf, "/");
	strcat(buf, quantity_unit->name);

}


double unit_manager::get_native_molmass(double molmass){
	double mass_ratio = get_mass_unit()->ratio;
	double quantity_ratio = get_quantity_unit()->ratio;

	double native_molmass = molmass * quantity_ratio / mass_ratio;

	return native_molmass;

}

double unit_manager::get_molmass(double native_molmass){
	double mass_ratio = get_mass_unit()->ratio;
	double quantity_ratio = get_quantity_unit()->ratio;

	return  native_molmass * mass_ratio / quantity_ratio ;

}






