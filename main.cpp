#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "attribute.h"
#include "instance.h"
#include <iomanip>

using namespace std;

// parses a line of data
void Parse_Data(string s, const vector<Attribute>&  attribute_list,
		vector<Instance>& example_list){
  Instance cur_instance;
  cur_instance.Set_Attributes(attribute_list);

  stringstream ss;
  ss.str(s);
  int cur_attribute = 0;
  string cur_val;
  while(ss >> cur_val){
    if(cur_val[cur_val.size()-1] == ',')
      cur_val = cur_val.substr(0, cur_val.size()-1); // trim off comma
    cur_instance.Add_Value(cur_attribute, cur_val);
    cur_attribute++;
  }
  if(cur_attribute != attribute_list.size()){
    cout << "ERROR: Wrong number of attributes on line: " << s << endl;
    exit(1);
  }
  example_list.push_back(cur_instance);
  
}

// gets a new attribute from an @attribute line
void New_Attribute(string s, vector<Attribute>& attribute_list){
  Attribute cur_attribute;
  stringstream ss;
  ss.str(s);
  string junk;
  ss >> junk; // remove "@attribute"
  string name;
  ss >> name;
  cur_attribute.Set_Name(name);
  string next; // either "{" or "numeric"
  ss >> next;
  if(next == "numeric")
    cur_attribute.Set_Numeric(true);
  else{
    cur_attribute.Set_Numeric(false);
    string temp;
    while(temp != "}"){
      ss >> temp;
      if(temp != "}"){
	if(temp[temp.size()-1] == ',')
	  temp = temp.substr(0, temp.size()-1); // trim off comma
	cur_attribute.Add_Category(temp);
      }
    }
  }
  attribute_list.push_back(cur_attribute);
}
     
void Print_Data(vector<Attribute>& attribute_list, vector<Instance>& examples){
  char choice = 'y';
  while(choice == 'y'){
    cout << "We have " << examples.size() << " examples. Which would you like? ";
    int cur_example;
    cin >> cur_example;

    cout << "Type 0 to see the all attributes" << endl;
    for(int i = 0; i < attribute_list.size(); i++){
      cout << "Type " << i+1 << " to see value of attribute " << attribute_list[i].Get_Name() << endl;
    }
    int which_attribute;
    cin >> which_attribute;
    for(int i = 0; i < attribute_list.size(); i++){
      if(which_attribute == 0 || which_attribute == i+1){
	if(attribute_list[i].Numeric())
	  cout << attribute_list[i].Get_Name() << ": "
	       << examples[cur_example].Get_Numeric_Value(i) << endl;
	else
	  cout << attribute_list[i].Get_Name() << ": "
	       << examples[cur_example].Get_Nominal_Value(i) << endl;
      }
    }
    cout << "Another? (y/n) ";
    cin >> choice;
  }
}



int Get_Number_Of_Category(const vector<Attribute>& attribute_list, int cur_attribute, string my_category){
	
	//sunny, overcast, rainy
	vector<string> names = attribute_list[cur_attribute].Get_Category_List();
	
	for(int i = 0; i < names.size(); i++){
		//	0 == 0
		if(my_category == names[i]){
			return i;
		}
			
	}
	return -1;
}
//the tester for attribute: Outlook
void Make_Count_Table(const vector<Attribute>& attribute_list, const vector<Instance>& examples, int cur_attribute, vector<vector<int> >& Table){
	
	//take care of the rows
	Table.resize(attribute_list[cur_attribute].Num_Categories());
	
	//go into the rows to fill the columns 
	for(int i = 0; i < Table.size(); i++){
		//finding "play" as classification
		Table[i].resize(attribute_list[attribute_list.size()-1].Num_Categories());
		
	}
	

	for(int j = 0; j < examples.size(); j++){
		//go inside table and find current example and find the "play" attribute categories			// 0 or 1 for yes or no
		Table[Get_Number_Of_Category(attribute_list, cur_attribute, examples[j].Get_Nominal_Value(cur_attribute))][Get_Number_Of_Category(attribute_list, attribute_list.size() - 1, examples[j].Get_Nominal_Value(attribute_list.size()-1))]++; 
			
	}
}  




void Print_Count_Table(vector<vector<int> >& Table, vector<Attribute> attribute_list, int att_num){
  //attempt of having pretty spacing and alignment
  cout << setw(20) << " ";
  for(int b = 0; b < Table[0].size(); b++){
    cout << setw(20) << attribute_list[attribute_list.size() - 1].Get_Category(b);
  }
  cout << endl;
  for( int i = 0; i < Table.size(); i++){
    cout << left << setw(20)<< attribute_list[att_num].Get_Category(i);
    for(int j = 0; j < Table[0].size(); j++){
      cout << setw(20)<< Table[i][j] ;
    }
  cout << endl << endl;
  }
}


void Make_1R_Rule(const vector<Attribute>& attribute_list,const vector<vector<int> >& count_table, int cur_attribute, vector<int>& cur_rule){

  for(int i=0; i< count_table.size();i++){
    int max_num = 0;
    for(int j = 0; j < count_table[i].size(); j++){
        //maximum value out of those
        if(count_table[i][max_num] < count_table[i][j]){
          max_num = j;
          
        }
    }
    //pushing back the maximum number of values of each attribute to the cur_rule vector
    cur_rule.push_back(max_num);
    
  }

}


void Print_1R_Rule(vector<int>& cur_rule, vector<Attribute> attribute_list, int attribute_num){
  cout << "The rule for attribute: " << attribute_list[attribute_num].Get_Name() << endl;

  for(int i = 0; i < attribute_list[attribute_num].Get_Category_List().size(); i++){
    //getting the current category 
    //getting category out based on the rule
    cout << "if " << attribute_list[attribute_num].Get_Category(i) << " then " << attribute_list[attribute_list.size()-1].Get_Category(cur_rule[i]) << endl;
  
  }
 
}

double Calc_Error_Rates(vector<int>& cur_rule, int cur_attribute, vector<Instance>& examples, vector<Attribute>& attribute_list){
   
  double num_right = 0;
  
  //taking the rule(and its attribute number)
  //and all the examples
  for(int i = 0; i < examples.size(); i++ ){
    for(int j = 0; j < cur_rule.size(); j++){
        //counting how many examples were right
        if(examples[i].Get_Nominal_Value(cur_attribute) == attribute_list[cur_attribute].Get_Category(j) && examples[i].Get_Nominal_Value(attribute_list.size() - 1) == attribute_list[attribute_list.size() - 1].Get_Category(cur_rule[j])){
        num_right++;
        
      }
    }
  }
  // determine how many examples were wrong
  return (examples.size() - num_right) / examples.size();
}



int main(){
  string filename;
  cout << "Enter the filename: ";
  cin >> filename;

  ifstream fin;
  fin.open(filename.data());
  while(!fin){
    cout << "File not found" << endl;
    cout << "Enter the filename: ";
    cin >> filename;
    fin.clear();
    fin.open(filename.data());
  }
  vector<Attribute> attribute_list;
  vector<Instance> examples;

  // file is open
  bool data_mode = false;
  string s;
  int num_read = 0;
  while(getline(fin, s)){
    if(s[s.size()-1] == '\r') // grrr
      s = s.substr(0,s.size()-1);
    
    if(s.size() > 0 && s[0] != '%'){ // ignore comments
      if(data_mode){ // it's a line of data
	Parse_Data(s, attribute_list, examples);
      }
      else{
	// then it had better start with an '@'
	if(s[0] != '@'){
	  cout << "ERROR: ILLEGAL LINE: " << s << endl;
	  exit(1);
	}
	// is it @attribute?
	else if(s.substr(0,10) == "@attribute")
	  New_Attribute(s, attribute_list);
	//is it @data?
	else if(s.substr(0,5) == "@data")
	  data_mode = true;
	else{
	  cout << "ERROR: ILLEGAL LINE: " << s << endl;
	  exit(1);
	}
      }
    }
    num_read++;
   // cout << num_read << endl;
  }
// Now that the data is read, what should we do?
 Print_Data(attribute_list, examples);
//----------------------------------------------------------------------------------------------------------------------------------------------------
 

  
	int cur_attribute;
  int num;
  vector<vector<int>> count_table;

  double error_rate;
  double best_error = 1;
  int best_rule;
  vector<vector<int>> rules;
  vector<int> cur_rule;

  for(int i = 0; i < attribute_list.size() - 1; i++){
    
    vector<vector<int>> Table;
    Make_Count_Table(attribute_list, examples, i, Table);

	  Print_Count_Table(Table, attribute_list, i);

    vector<int> cur_rule;
    
    Make_1R_Rule(attribute_list, Table, i, cur_rule);

    Print_1R_Rule(cur_rule, attribute_list, i);

    error_rate = Calc_Error_Rates(cur_rule, i, examples, attribute_list);

    cout << "With error rate: " << error_rate << endl;
     
    
    if(error_rate < best_error){
       best_error = error_rate;
       best_rule = i;
    }
    rules.push_back(cur_rule);
    

    cout << endl;
    cout << endl;
  }
  //go through attribute list error rates now
  //find the best one 
  cout << "The best 1R attribute is " << best_error  << " which is " << attribute_list[best_rule].Get_Name() << endl;
  

  //FINAL PART:
  //the testing 

  string test_filename;
  //copy and paste file syntax
  cout << "Enter the test data file: ";
  cin >> test_filename;
  ifstream test_fin;
  test_fin.open(test_filename.data());
  while(!test_fin){
    cout << "File not found" << endl;
    cout << "Enter the filename: ";
    cin >> test_filename;
    test_fin.clear();
    test_fin.open(test_filename.data());
  }
  
  vector<Instance> test_examples;

  // file is open
  bool test_data_mode = true;
  string test_s;
  int test_num_read = 0;
  while(getline(test_fin, test_s)){
    if(test_s[test_s.size()-1] == '\r') // grrr
      test_s = test_s.substr(0, test_s.size()-1);
    if(test_s.size() > 0 && test_s[0] != '%'){ // ignore comments
      if(test_data_mode == true){ // it's a line of data
	      Parse_Data(test_s, attribute_list, test_examples);
        
      }
    
    }
    test_num_read++;
    cout << test_num_read << endl;
  
  }
  
  Print_Data(attribute_list, test_examples);

     // vector<int>& cur_rule, int cur_attribute, vector<Instance>& examples, vector<Attribute> attribute_list
     //cur_rule, i, examples, attribute_list
  double test_error_rate = Calc_Error_Rates(rules[best_rule], best_rule, test_examples, attribute_list);
  cout << "The error rate on the test data is: " << test_error_rate << endl;


  return 0;
}





