#include "source_code\Alt_Headers_Precompiled.h"
//#include "AltGui_Dam_Change_Function_Dia.h"


//Default constructor
AltGui_Dam_Change_Function_Dia::AltGui_Dam_Change_Function_Dia(QWidget *parent): QDialog(parent){

	this->setupUi(this);

	QObject::connect(this->cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
	QObject::connect(this->okButton, SIGNAL(clicked()), this, SLOT(accept()));
	QObject::connect(this->spinBox, SIGNAL(valueChanged(int)), this, SLOT(recieve_number_spin_box(int )));

	this->tableWidget->setColumnCount(1);
	this->tableWidget->setHorizontalHeaderLabels(QStringList() << tr("Select replacing Ids"));
	this->tableWidget->setAlternatingRowColors(true);

	this->old_spinbox_value=0;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(AltGui_Dam_Change_Function_Dia), _sys_system_modules::ALT_SYS);
}
//Default destructor
AltGui_Dam_Change_Function_Dia::~AltGui_Dam_Change_Function_Dia(void){

	 	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(AltGui_Dam_Change_Function_Dia), _sys_system_modules::ALT_SYS);
}
//________
//public
//Set for the dialog the function/categories, which should be switched
bool AltGui_Dam_Change_Function_Dia::set_dialog(QSqlDatabase *database, const string detailed_identifier){

	Sys_Diverse_Text_Dia my_dia;
	QSqlQueryModel results;

	if(detailed_identifier==alt_replace_dam::dam_ecn_function_polygon){
		//check if there are raster for damage type calculation
		if(Dam_Ecn_Raster::select_relevant_raster_database(&results, database, false)==0){
			Sys_Diverse_Text_Dia my_dia1(true);
			my_dia1.set_dialog_question("No ECN-raster are set. Measure is not applicable!");
			my_dia1.start_dialog();
			return false;
		}


		//ask for new function/categories/biotope-types for adding
		my_dia.set_dialog_question("Do you want to add new ECN-damage function(s)?");
		my_dia.set_button_text("Yes","No");

		if(my_dia.start_dialog()==true){
			this->add_ecn_damage_function2database(database);
		}
		
		//set switching dialog
		this->setWindowTitle("Switch ECN-damage function");
		this->label_polygon->setText("Select the file, which contains the polygon(s)-information where the changing\nof the ECN-damage function should be applied.");
		this->label_switch->setText("Switch the ECN-damage function: existing id    -->  new id");
		this->set_ecn_damage_function2combobox(database);



	}
	
	//implement further damage types..


	return true;
}
//Get the selected informations 
void AltGui_Dam_Change_Function_Dia::get_selected_information(QStringList *file_list, QList<int> *id_list_current, QList<int> *id_list_new){
	AltGui_Id_Replacing_Wid* input_widget=NULL;
	QString buffer;
	QStringList split_list;

	//file
	buffer=widget_file->fileName();
	file_list->append(buffer);

	for (int i = 0; i < this->tableWidget->rowCount(); i++){
		input_widget = qobject_cast<AltGui_Id_Replacing_Wid*>(this->tableWidget->cellWidget(i,0));
		//current id
		buffer=input_widget->get_ptr_combo_box_current()->currentText();
		split_list=buffer.split(" ");
		buffer=split_list.at(0);
		id_list_current->append(buffer.toInt());
		//new id
		buffer=input_widget->get_ptr_combo_box_new()->currentText();
		split_list=buffer.split(" ");
		buffer=split_list.at(0);
		id_list_new->append(buffer.toInt());

	}

	////id existing
	//buffer=this->comboBox_src->currentText();
	//split_list=buffer.split(" ");
	//buffer=split_list.at(0);
	//id_list_current->append(buffer.toInt());

	////id new
	//buffer=this->comboBox_target->currentText();
	//split_list=buffer.split(" ");
	//buffer=split_list.at(0);
	//id_list_new->append(buffer.toInt());

}
//Start the dialog it returns true by acception, false by rejection
bool AltGui_Dam_Change_Function_Dia::start_dialog(void){
	int decision =this->exec();
	 //rejected
	if(decision ==0){
		return false;
	}
	//accepted
	else{
		//set the data

		return true;
	}
}
//______
//private slots
//Check the value of the spinbox
void AltGui_Dam_Change_Function_Dia::recieve_number_spin_box(int number){
	if(abs(number-this->old_spinbox_value)<=1){
		this->tableWidget->setRowCount(number);
		
		if(number>this->old_spinbox_value){
			this->set_row_item(true);
		}
		else{
			this->set_row_item(false);
		}
		this->old_spinbox_value=number;
	}
	else{
		this->spinBox->setValue(this->old_spinbox_value);
	}
}
//_________
//private
//Set the ECN damage function to the combo box
void AltGui_Dam_Change_Function_Dia::set_ecn_damage_function2combobox(QSqlDatabase *database){

	QSqlTableModel result(NULL, *database);
	int number=0;
	try{
		number=Dam_Ecn_Damage_Function::select_relevant_functions_database(&result, false);
	}
	catch(Error msg){
		ostringstream info;
		info <<"Try to set up the DAM ECN-damage function switch dialog for measure's implementation"<<endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	ostringstream buff_txt;	
	for(int i=0; i< number; i++){
		buff_txt.str("");
		buff_txt << result.record(i).value((Dam_Ecn_Damage_Function::function_table->get_column_name(dam_label::landuse_id)).c_str()).toInt() << "   ";
		buff_txt << result.record(i).value((Dam_Ecn_Damage_Function::function_table->get_column_name(dam_label::landuse_name)).c_str()).toString().toStdString();
		this->current_list.append(buff_txt.str().c_str());
	}
	this->spinBox->setMaximum(number-1);
}
//Add new ECN damage function to the data base
void AltGui_Dam_Change_Function_Dia::add_ecn_damage_function2database(QSqlDatabase *database){


	Dam_Ecn_System my_system;
	if(my_system.ask_file_damage_function()==false){
			return;
	}
	else{
		ostringstream text;
		text<<"Import additional economical damage function(s) to the database..."<<endl;
		Sys_Common_Output::output_alt->output_txt(&text);

		ostringstream prefix;
		prefix<<"ALT> ";
		Sys_Common_Output::output_dam->set_focus_on_display();
		Sys_Common_Output::output_dam->set_userprefix(prefix.str());
		my_system.add_damage_func_file2database(database);
		Sys_Common_Output::output_dam->reset_userprefix();
		Sys_Common_Output::output_alt->set_focus_on_display();

		if(my_system.get_error_number()!=0){
			text<<"The economical damage function(s) could not be added to database!"<<endl;
			Sys_Common_Output::output_alt->output_txt(&text);
		}

	}

}


//Set the widget for every rows
void AltGui_Dam_Change_Function_Dia::set_row_item(const bool add){
	AltGui_Id_Replacing_Wid* input_widget=NULL;
	for (int i = 0; i < this->tableWidget->rowCount(); i++){
		if(add==true){
			AltGui_Id_Replacing_Wid *rowItem = new AltGui_Id_Replacing_Wid(this);

			//! Set the FileChooser widget at every rows of the table
			this->tableWidget->setCellWidget(this->tableWidget->rowCount()-1, 0, rowItem);
			this->tableWidget->setCurrentCell(this->tableWidget->rowCount(), 0);
			this->tableWidget->setRowHeight(i, 60);
			if(i<this->tableWidget->rowCount()-1){
				input_widget = qobject_cast<AltGui_Id_Replacing_Wid*>(this->tableWidget->cellWidget(i,0));
				input_widget->setEnabled(false);
			}	
		}
		else{
			if(i==this->tableWidget->rowCount()-1){
				input_widget = qobject_cast<AltGui_Id_Replacing_Wid*>(this->tableWidget->cellWidget(i,0));
				input_widget->setEnabled(true);
			}
		}
		
	}
	if(this->tableWidget->rowCount()>0){
		this->set_combox_items(this->tableWidget->rowCount()-1);
	}
	this->tableWidget->resizeColumnsToContents();
}
//Set the items of the combo box after the generated string list
void AltGui_Dam_Change_Function_Dia::set_combox_items(const int index){
	QStringList buffer;
	buffer=this->current_list;
	AltGui_Id_Replacing_Wid* input_widget=NULL;
	QString delete_str;

	for (int i = 0; i < this->tableWidget->rowCount()-1; i++){
		input_widget = qobject_cast<AltGui_Id_Replacing_Wid*>(this->tableWidget->cellWidget(i,0));
		delete_str=input_widget->get_ptr_combo_box_current()->currentText();
		buffer.removeAt(buffer.indexOf(delete_str));
	}

	input_widget = qobject_cast<AltGui_Id_Replacing_Wid*>(this->tableWidget->cellWidget(index,0));
	input_widget->set_combobox_current_items(buffer);
	input_widget->set_combobox_new_items(this->current_list);
}