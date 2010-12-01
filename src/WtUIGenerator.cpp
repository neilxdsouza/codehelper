#include "std_headers.h"
#include "WtUIGenerator.h"
#include "CppCodeGenerator.h"
#include "TableCollectionSingleton.hpp"

#include "std_using.h"
#include "utils.h"
#include "global_options.h"
#include "global_variables.h"
#include "error.h"

using global_options::project_namespace;

using namespace std;


std::stringstream WtUIGenerator::class_vars;
std::stringstream WtUIGenerator::class_functions_decl;
std::stringstream WtUIGenerator::class_function_impl;
std::stringstream WtUIGenerator::navigation_nodes;
std::stringstream WtUIGenerator::header_files;
std::stringstream WtUIGenerator::makefile_objs;
extern vector <TableInfoType *> vec_table_info;
//extern char project_namespace[];
extern std::string project_namespace;

WtUIGenerator::WtUIGenerator(TableInfoType * p_tabInfo,
						 std::string & p_output_dir_path)
	//: tableInfo_(p_tabInfo),
	//  outputDirPrefix_(p_output_dir_path)			
	: AbstractUIGenerator(p_tabInfo, p_output_dir_path)
	  //,
	  //class_vars(), class_functions(), class_function_impl(),
	  //navigation_nodes()
{ }

void WtUIGenerator::GenerateCode()
{
	//cout << format("ENTER: FILE: %1%, LINE: %2% FUNCTION:%3%\n") % __FILE__ % __LINE__ 
	//	% __PRETTY_FUNCTION__;
	//ui << GenerateUIScaffolding();
	static bool once=true;
	if (once) {
		// stringstream login_func_decl, login_func_defn;
		// PrintSetupLogin(login_func_decl, login_func_defn);
		// AddFunctionDecl(login_func_decl.str());
		// AddFunctionDefn(login_func_defn.str());

		stringstream setup_app_func_decl, setup_app_func_defn;
		PrintSetupApplication(setup_app_func_decl, setup_app_func_defn);
		AddFunctionDecl(setup_app_func_decl.str());
		AddFunctionDefn(setup_app_func_defn.str());
		once = false;
	}

	if (tableInfo_->tab_options.is_login_page) {
		cout << "We need to generate the login widget for this table:" 
			<< tableInfo_->tableName_
			<< endl;
		PrintLoginWidget();
		stringstream login_widget_name_str;
		login_widget_name_str << boost::format("%1%_Widget * login_;\n") %
					tableInfo_->tableName_;
		AddVariableDecl(login_widget_name_str.str());

		stringstream login_func_decl, login_func_defn;
		PrintSetupLogin(login_func_decl, login_func_defn);
		AddFunctionDecl(login_func_decl.str());
		AddFunctionDefn(login_func_defn.str());
	}
	GenerateForms();
	makefile_objs << boost::format("%1%_ui.o %1%_bll.o %1%_db_postgres.o ") % tableInfo_->tableName_;
	
	// cout << format("EXIT: %1% %2% %3%\n") % __FILE__ % __LINE__ 
	// 	% __PRETTY_FUNCTION__;
}

void WtUIGenerator::GenerateForms()
{
	AddFunctionDefn(GenerateUIInsertForm());
}

string WtUIGenerator::GenerateUIScaffolding()
{
	stringstream uiScaffolding;
	string ui_fname (string(outputDirPrefix_.c_str()
					+ string("/")
					+ string("wt_ui.cpp")));
	ofstream ui(ui_fname.c_str(), ios_base::out| ios_base::trunc);
	uiScaffolding << PrintHeaders();

	uiScaffolding << "using namespace Wt;\n";
	uiScaffolding << "\n";
	uiScaffolding << PrintClassDecl();
	uiScaffolding << "good1::good1(const WEnvironment & env)\n";
	uiScaffolding << "	: WApplication(env)\n";
	uiScaffolding << "{\n";

	uiScaffolding << "\t//viewPort = new WTable(root());\n";
	uiScaffolding << "\tviewPort = new WContainerWidget(root());\n";
	uiScaffolding << "\tSetupLogin();\n";
	uiScaffolding << "\tlogin_->loginSuccessful.connect(this, &good1::SetupApplication);\n";


	uiScaffolding << "}\n";
	uiScaffolding << "\n";

	uiScaffolding << PrintNavigationDecl();

	uiScaffolding << "\n";
	uiScaffolding << "WTreeNode *good1::createNavigationNode(const Wt::WString& label,\n";
	uiScaffolding << "				    WTreeNode *parentNode,\n";
	uiScaffolding << "				    ShowCentralWidget f)\n";
	uiScaffolding << "{\n";
	uiScaffolding << "	WIconPair *labelIcon\n";
	uiScaffolding << "		= new WIconPair(\"icons/document.png\", \"icons/document.png\", false);\n";
	uiScaffolding << "\n";
	uiScaffolding << "	WTreeNode *node = new WTreeNode(label, labelIcon, parentNode);\n";
	uiScaffolding << "	node->label()->setTextFormat(PlainText);\n";
	uiScaffolding << "	node->label()->clicked().connect(this, f);\n";
	uiScaffolding << "\n";
	uiScaffolding << "	return node;\n";
	uiScaffolding << "}\n";
	uiScaffolding << "\n";
	uiScaffolding << "void good1::setCentralWidget(WWidget *example)\n";
	uiScaffolding << "{\n";
	uiScaffolding << "	delete currentForm_;\n";
	uiScaffolding << "	currentForm_ = example;\n";
	uiScaffolding << "	formContainer_->addWidget(currentForm_);\n";
	uiScaffolding << "}\n";
	uiScaffolding << "\n";
	uiScaffolding << "void good1::formModify()\n";
	uiScaffolding << "{\n";
	uiScaffolding << "	std::cerr << cb->currentText() << \", \" << cb->currentIndex() << std::endl;\n";
	uiScaffolding << "	cb->addItem(\"Four?\");\n";
	uiScaffolding << "}\n";
	uiScaffolding << "\n";
	uiScaffolding << "WApplication *createApplication(const WEnvironment& env)\n";
	uiScaffolding << "{\n";
	uiScaffolding << "	WApplication *app = new good1(env);\n";
	uiScaffolding << "\n";
	uiScaffolding << "	return app;\n";
	uiScaffolding << "}\n";
	uiScaffolding << "\n";
	uiScaffolding << "int main(int argc, char **argv)\n";
	uiScaffolding << "{\n";
	uiScaffolding << "	return WRun(argc, argv, &createApplication);\n";
	uiScaffolding << "}\n";
	uiScaffolding << "\n";
	uiScaffolding << class_function_impl.str();
	uiScaffolding << "\n";

	ui << uiScaffolding.str();
	return uiScaffolding.str();
}


string WtUIGenerator::PrintHeaders()
{
	stringstream h_files;
	h_files << "\n";
	h_files << "#include <fstream>\n";
	h_files << "#include <sstream>\n";
	h_files << "\n";
	h_files << "#include <Wt/WBorderLayout>\n";
	h_files << "#include <Wt/WHBoxLayout>\n";
	h_files << "#include <Wt/WVBoxLayout>\n";
	h_files << "#include <Wt/WFitLayout>\n";
	h_files << "#include <Wt/WOverlayLoadingIndicator>\n";
	h_files << "#include <Wt/WStandardItemModel>\n";
	h_files << "#include <Wt/WStandardItem>\n";
	h_files << "#include <Wt/WTable>\n";
	h_files << "#include <Wt/WTableCell>\n";
	h_files << "#include <Wt/WText>\n";
	h_files << "#include <Wt/WLabel>\n";
	h_files << "#include <Wt/WTextEdit>\n";
	h_files << "#include <Wt/WTree>\n";
	h_files << "#include <Wt/WTableView>\n";
	h_files << "#include <Wt/WIconPair>\n";
	h_files << "#include <Wt/WTreeNode>\n";
	h_files << "#include <Wt/WApplication>\n";
	h_files << "#include <Wt/WPushButton>\n";
	h_files << "#include <Wt/WImage>\n";
	h_files << "#include <Wt/WGridLayout>\n";
	h_files << "\n";
	h_files << "#include <Wt/Ext/Button>\n";
	h_files << "#include <Wt/Ext/Calendar>\n";
	h_files << "#include <Wt/Ext/CheckBox>\n";
	h_files << "#include <Wt/Ext/ComboBox>\n";
	h_files << "#include <Wt/Ext/Container>\n";
	h_files << "#include <Wt/Ext/DateField>\n";
	h_files << "#include <Wt/Ext/Dialog>\n";
	h_files << "#include <Wt/Ext/Menu>\n";
	h_files << "#include <Wt/Ext/MessageBox>\n";
	h_files << "#include <Wt/Ext/ProgressDialog>\n";
	h_files << "#include <Wt/Ext/Splitter>\n";
	h_files << "#include <Wt/Ext/TabWidget>\n";
	h_files << "#include <Wt/Ext/TableView>\n";
	h_files << "#include <Wt/Ext/TextEdit>\n";
	h_files << "#include <Wt/Ext/ToolBar>\n";
	h_files << "#include <boost/lexical_cast.hpp>\n";
	h_files << "#include <boost/shared_ptr.hpp>\n";
	h_files << "#include <boost/scoped_ptr.hpp>\n";
	h_files << "#include <boost/date_time/gregorian/gregorian.hpp>\n";
	h_files << "#include \"LoginWidget.h\"\n";
	h_files << "\n";
	h_files << "#include <iostream>\n";
	h_files << "\n";
	h_files << header_files.str();
	return h_files.str();
}

string WtUIGenerator::PrintClassDecl()
{
	stringstream class_decl;
	class_decl << "class good1: public Wt::WApplication\n";
	class_decl << "{\n";
	class_decl << "	public:\n";
	class_decl << "	good1(const Wt::WEnvironment & env);\n";
	class_decl << "	typedef void (good1::*ShowCentralWidget)();\n";
	class_decl << "	void setCentralWidget(Wt::WWidget *theWidget);\n";
	class_decl << "	WWidget * createNavigationTree();\n";
	class_decl << "	//void formWidgetsExample();\n";
	class_decl << "	WTreeNode *createNavigationNode(const Wt::WString& label,\n";
	class_decl << "					    WTreeNode *parentNode,\n";
	class_decl << "					    ShowCentralWidget f);\n";
	class_decl << "	void formModify();\n";
	class_decl << class_functions_decl.str();

	class_decl << "	Wt::Ext::ComboBox *cb;\n";
	class_decl << "	Wt::Ext::TextEdit *html_;\n";
	class_decl << "	Wt::WWidget                       *currentForm_;\n";
	class_decl << "Wt::WContainerWidget              *formContainer_;\n";
	class_decl << "	//Wt::WTable              *viewPort;\n";
	class_decl << "	Wt::WContainerWidget              *viewPort;\n";
	class_decl << "	//LoginWidget             *login_;\n";

	class_decl << class_vars.str();

	class_decl << "};\n";
	class_decl << "\n";
	return class_decl.str();
}

void WtUIGenerator::AddVariableDecl(std::string  p_var_decl)
{
	class_vars << "\t"<< p_var_decl << endl;
}

void WtUIGenerator::AddFunctionDecl(std::string  p_func_decl)
{
	class_functions_decl << "\t" << p_func_decl << endl;
}

void WtUIGenerator::AddFunctionDefn(std::string  p_func_defn)
{
	class_function_impl << p_func_defn << endl;
}

string WtUIGenerator::PrintNavigationDecl()
{
	stringstream navigation_tree_func;
	navigation_tree_func << "WWidget *good1::createNavigationTree()\n";
	navigation_tree_func << "{\n";
	navigation_tree_func << "	WIconPair *mapIcon\n";
	navigation_tree_func << "	= new WIconPair(\"icons/yellow-folder-closed.png\",\n";
	navigation_tree_func << "		    \"icons/yellow-folder-open.png\", false);\n";
	navigation_tree_func << "\n";
	navigation_tree_func << "	WTreeNode *rootNode = new WTreeNode(\"Navigation\", mapIcon);\n";
	navigation_tree_func << "	rootNode->setImagePack(\"icons/\");\n";
	navigation_tree_func << "	rootNode->expand();\n";
	navigation_tree_func << "	rootNode->setLoadPolicy(WTreeNode::NextLevelLoading);\n";
	navigation_tree_func << "\n";
	navigation_tree_func << "	//createNavigationNode(\"Form widgets\", rootNode,\n";
	navigation_tree_func << "	//		    &good1::formWidgetsExample);\n";
	//navigation_tree_func << navigation_nodes.str();
	navigation_tree_func << PrintUINavigation();
	navigation_tree_func << "\n";
	navigation_tree_func << "	rootNode->setMargin(5);\n";
	navigation_tree_func << "\n";
	navigation_tree_func << "	return rootNode;\n";
	navigation_tree_func << "}\n";
	return navigation_tree_func.str();
}

void WtUIGenerator::PrintSetupLogin(stringstream & func_decl, stringstream & func_defn)
{
	func_decl << "void SetupLogin();\n";
	func_defn << "\tvoid good1::SetupLogin()\n{\n";
	func_defn << "	// WText * center_text = new WText(\"Timesheet Application\", viewPort->elementAt(0,0));\n";
	func_defn << "	// viewPort->elementAt(0, 0)->setContentAlignment(AlignTop | AlignCenter);\n";
	func_defn << "	// login_ = new LoginWidget();\n";
	func_defn << "	// viewPort->elementAt(1, 0)->addWidget(login_);\n";
	func_defn << format("	login_ = new %1%_Widget(viewPort);\n") % tableInfo_->tableName_;

	func_defn << "}\n\n";
}

void WtUIGenerator::PrintSetupApplication(std::stringstream & func_decl, std::stringstream & func_defn)
{
	func_decl << "\tvoid SetupApplication();\n";
	func_defn << "\tvoid good1::SetupApplication()\n\{\n";

	func_defn << "\tviewPort->clear();\n";
	func_defn << "	setTitle(\"Timesheet Application Program\");\n";
	func_defn << "	setLoadingIndicator(new WOverlayLoadingIndicator());\n";
	func_defn << "\n";
	func_defn << "	useStyleSheet(\"good1.css\");\n";
	func_defn << "	messageResourceBundle().use(appRoot() + \"good1\");\n";
	func_defn << "	//Ext::Container * viewPort = new Ext::Container(root());\n";
	func_defn << "\t//WBorderLayout *layout = new WBorderLayout(viewPort);\n";
	func_defn << "\n";
	func_defn << "	/* North */\n";
	func_defn << "	Ext::Panel *north = new Ext::Panel();\n";
	func_defn << "	north->setBorder(false);\n";
	//func_defn << "	WText *head = new WText(Wt::WString::tr(\"header\"));\n";
	//func_defn << "	head->setStyleClass(\"north\");\n";
	func_defn << "\tWt::WImage *logo_image = new Wt::WImage(\"images/logo.png\");\n";
	func_defn << "\tWContainerWidget * logo_container = new WContainerWidget();\n";

	func_defn << "\tlogo_container->addWidget(logo_image);\n";
	func_defn << "\tviewPort->addWidget(logo_container);\n";
	func_defn << "\t//viewPort->elementAt(0,0)->addWidget(logo_image);\n";
	func_defn << "\t// north->setLayout(new WFitLayout());\n";
	func_defn << "\tWContainerWidget * north_container = new WContainerWidget();\n";
	func_defn << "\t// north->layout()->addWidget(north_container);\n";
	func_defn << "\t// WGridLayout * north_panel_grid = new WGridLayout();\n";
	func_defn << "\t//north_container->setLayout(north_panel_grid);\n";
	func_defn << "\t// north_panel_grid->addWidget(logo_image, 0, 0);\n\n";
	func_defn << "	north->resize(WLength::Auto, 35);\n";
	func_defn << PrintUIMenu();
	func_defn << "\t//north_panel_grid->addWidget(north, 0, 1);\n";
	func_defn << "\tnorth_container->addWidget(north);\n";
	func_defn << "\tviewPort->addWidget(north_container);\n";
	func_defn << "\t//viewPort->elementAt(0,1)->addWidget(north);\n";
	func_defn << "\n";
	func_defn << "	/* West */\n";
	func_defn << "	// Ext::Panel *west = new Ext::Panel();\n";
	func_defn << "	// west->layout()->addWidget(createNavigationTree());\n";
	func_defn << "	// \n";
	func_defn << "	// west->setTitle(\"Widgets\");\n";
	func_defn << "	// west->resize(200, WLength::Auto);\n";
	func_defn << "	// west->setResizable(true);\n";
	func_defn << "	// west->setCollapsible(true);\n";
	func_defn << "	// west->setAnimate(true);\n";
	func_defn << "	// west->setAutoScrollBars(true);\n";
	func_defn << "	// layout->addWidget(west, WBorderLayout::West);\n";
	func_defn << "\n";
	func_defn << "	/* Center */\n";
	func_defn << "	Ext::Panel *center = new Ext::Panel();\n";
	func_defn << "	center->setTitle(\"Demo widget\");\n";
	func_defn << "	center->layout()->addWidget(formContainer_ = new WContainerWidget());\n";
	func_defn << "	center->setAutoScrollBars(true);\n";
	func_defn << "	//layout->addWidget(center, WBorderLayout::Center);\n";
	func_defn << "\n";
	func_defn << "	formContainer_->setPadding(5);\n";
	func_defn << "\n";
	func_defn << "	WContainerWidget *container = new WContainerWidget(formContainer_);\n";
	func_defn << "	container->addWidget(new WText(Wt::WString::tr(\"about\")));\n";
	func_defn << "	currentForm_ = container;\n";
	func_defn << "\t//viewPort->elementAt(1,1)->addWidget(center);\n";
	func_defn << "\tviewPort->addWidget(center);\n";
	func_defn << "\n";
	func_defn << "}\n\n";
}

void WtUIGenerator::AddNavigationNode(std::string  label, std::string  func_name)
{
	//navigation_nodes << "// Hello,World\n";
	navigation_nodes << boost::format("\t/* tableInfo_->tableName_ : %1%, nReferencedAsMulti: %2% */\n") %
		tableInfo_->tableName_  %
		tableInfo_->nReferencedAsMulti;
	navigation_nodes << "\tcreateNavigationNode(\""
	 		<< label << "\", rootNode,\n"
	 		<< "\t\t\t&good1::" << func_name << ");\n";
}

std::string WtUIGenerator::PrintUINavigation()
{
	using boost::format;
	stringstream nav_str;
	vector<string> & v = global_variables::ui_navigation_order.uiGroups_;
	multimap<string, TableInfoType*> & mm = global_variables::ui_navigation_order.uiGroupLinks_;
	nav_str << " /* v.size(): " << v.size() << " */\n";
	typedef multimap<string,TableInfoType*>::const_iterator I;
	for (int i=0; i<v.size(); ++i){
		nav_str << format("\tWTreeNode * tn_%1% = new WTreeNode(\"%1%\", mapIcon, rootNode);\n") %
			v[i];
		pair<I,I> nav_nodes = mm.equal_range(v[i]);
		for(I iter= nav_nodes.first; iter!=nav_nodes.second; ++iter){
			TableInfoType * const & ti_ptr = iter->second;
			if (ti_ptr->nReferencedAsMulti==0) {
				nav_str << "\t/* : " << iter->second->tableName_ << " */\n";
				nav_str << format("\tcreateNavigationNode(\"%2%\", tn_%1%, &good1::formInsert%2%);\n" ) %
						v[i] % ti_ptr->tableName_;
			}
		}
	}
	return nav_str.str();
}


std::string WtUIGenerator::PrintUIMenu()
{
	using boost::format;
	stringstream menu_str;
	vector<string> & v = global_variables::ui_navigation_order.uiGroups_;
	multimap<string, TableInfoType*> & mm = global_variables::ui_navigation_order.uiGroupLinks_;
	menu_str << "\t/* v.size(): " << v.size() << " */\n";
	menu_str << "\tWt::Ext::ToolBar *toolBar = new Wt::Ext::ToolBar();\n";
	menu_str << "\tWt::Ext::MenuItem *item = 0;\n";
	menu_str << "\tWt::Ext::Button *b =0;\n";

	typedef multimap<string,TableInfoType*>::const_iterator I;
	for (int i=0; i<v.size(); ++i) {
		//menu_str << format("\tWTreeNode * tn_%1% = new WTreeNode(\"%1%\", mapIcon, rootNode);\n") %
		//	v[i];
		menu_str << format("\tWt::Ext::Menu *menu_%1% = new Wt::Ext::Menu();\n") %
				v[i];
		pair<I,I> nav_nodes = mm.equal_range(v[i]);
		for (I iter= nav_nodes.first; iter!=nav_nodes.second; ++iter) {
			TableInfoType * const & ti_ptr = iter->second;
			if (ti_ptr->nReferencedAsMulti == 0) {
				// menu_str << "\t/* : " << iter->second->tableName_ << " */\n";
				// menu_str << format("\tcreateNavigationNode(\"%2%\", tn_%1%, &good1::formInsert%2%);\n" ) %
				// 		v[i] % ti_ptr->tableName_;
				menu_str << format("\titem = menu_%1%->addItem (Wt::WString::tr(\"%2%\"),\n\t\t\t this, &good1::formInsert%2%);\n") %
					v[i] % ti_ptr->tableName_;
			}
		}
		menu_str << format("\tb = toolBar->addButton(Wt::WString::tr(\"%1%\"), menu_%1%);\n") %
				v[i];
	}
	menu_str << "\tnorth->setTopToolBar(toolBar);\n";
	return menu_str.str();
}

string WtUIGenerator::GenerateUIInsertForm()
{
	stringstream ui_class_headers, ui_class_decl, ui_class_defn;
	ui_class_headers << "\n";
	ui_class_headers << "#include <fstream>\n";
	ui_class_headers << "\n";
	ui_class_headers << "#include <Wt/WBorderLayout>\n";
	ui_class_headers << "#include <Wt/WHBoxLayout>\n";
	ui_class_headers << "#include <Wt/WVBoxLayout>\n";
	ui_class_headers << "#include <Wt/WFitLayout>\n";
	ui_class_headers << "#include <Wt/WOverlayLoadingIndicator>\n";
	ui_class_headers << "#include <Wt/WStandardItemModel>\n";
	ui_class_headers << "#include <Wt/WStandardItem>\n";
	ui_class_headers << "#include <Wt/WTable>\n";
	ui_class_headers << "#include <Wt/WTableCell>\n";
	ui_class_headers << "#include <Wt/WText>\n";
	ui_class_headers << "#include <Wt/WLabel>\n";
	ui_class_headers << "#include <Wt/WTextEdit>\n";
	ui_class_headers << "#include <Wt/WTree>\n";
	ui_class_headers << "#include <Wt/WTableView>\n";
	ui_class_headers << "#include <Wt/WIconPair>\n";
	ui_class_headers << "#include <Wt/WTreeNode>\n";
	ui_class_headers << "#include <Wt/WApplication>\n";
	ui_class_headers << "#include <Wt/WPushButton>\n";
	ui_class_headers << "#include <Wt/WContainerWidget>\n";
	ui_class_headers << "#include <Wt/WWidget>\n";
	ui_class_headers << "#include <Wt/WDialog>\n";
	ui_class_headers << "#include <Wt/WPanel>\n";
	ui_class_headers << "#include <Wt/WLength>\n";


	ui_class_headers << "#include <Wt/WDateValidator>\n";
	ui_class_headers << "#include <Wt/WDoubleValidator>\n";
	ui_class_headers << "#include <Wt/WIntValidator>\n";
	ui_class_headers << "#include <Wt/WLengthValidator>\n";
	ui_class_headers << "#include <Wt/WRegExpValidator>\n";


	ui_class_headers << "#include \"TimesheetCalendar.h\"\n";
	ui_class_headers << "\n";
	ui_class_headers << "#include <Wt/Ext/Button>\n";
	ui_class_headers << "#include <Wt/Ext/Calendar>\n";
	ui_class_headers << "#include <Wt/Ext/CheckBox>\n";
	ui_class_headers << "#include <Wt/Ext/ComboBox>\n";
	ui_class_headers << "#include <Wt/Ext/Container>\n";
	ui_class_headers << "#include <Wt/Ext/Dialog>\n";
	ui_class_headers << "#include <Wt/Ext/Menu>\n";
	ui_class_headers << "#include <Wt/Ext/MessageBox>\n";
	ui_class_headers << "#include <Wt/Ext/ProgressDialog>\n";
	ui_class_headers << "#include <Wt/Ext/Splitter>\n";
	ui_class_headers << "#include <Wt/Ext/TabWidget>\n";
	ui_class_headers << "#include <Wt/Ext/TableView>\n";
	ui_class_headers << "#include <Wt/Ext/TextEdit>\n";
	ui_class_headers << "#include <Wt/Ext/LineEdit>\n";
	ui_class_headers << "#include <Wt/Ext/DateField>\n";
	ui_class_headers << "#include <Wt/Ext/NumberField>\n";
	ui_class_headers << "#include <Wt/Ext/ToolBar>\n";
	ui_class_headers << "\n";
	ui_class_headers << "#include <iostream>\n";
	ui_class_headers << format("#include \"%1%_bll.h\"\n") 
				% tableInfo_->tableName_;
	ui_class_headers << "\n";

	ui_class_decl << boost::format("class %1%_ui : public Wt::WContainerWidget\n{\npublic:\n")
				% tableInfo_->tableName_;
	ui_class_decl << boost::format("\t%1%_ui(Wt::WContainerWidget * parent);\n")
		% tableInfo_->tableName_ ;
	
	
	stringstream form_code;
	stringstream func_decl_signature, func_defn_signature;
	func_decl_signature << boost::format("void formInsert%1%()")
			% tableInfo_->tableName_;
	func_defn_signature << boost::format("void good1::formInsert%1%()")
			% tableInfo_->tableName_;
	form_code << func_defn_signature.str() << "\n{\n";
	class_functions_decl << "\t" << func_decl_signature.str() << ";\n";
	
	form_code << boost::format("\tWt::WContainerWidget *canvas = new %1%_ui(0);\n")
			% tableInfo_->tableName_;
	//form_code << "\t/*\n";
	ui_class_decl << "\tWt::Ext::TabWidget *tw;\n";
	ui_class_defn << boost::format("%1%_ui::%1%_ui(WContainerWidget * parent): WContainerWidget(parent)\n{\n")
		% tableInfo_->tableName_ ;
	//ui_class_decl << "\tWt::WText *title;\n";
	//ui_class_defn << "\ttitle = new Wt::WText( Wt::WString::tr(\""
	//	<< tableInfo_->tableName_ << "\"), this);\n";
	//ui_class_defn << "\ttitle->setMargin(5, Wt::Bottom);\n";
	ui_class_defn << "\ttw = new Wt::Ext::TabWidget(this);\n";
	ui_class_defn << "\ttw->setMinimumSize(Wt::WLength(600, Wt::WLength::Pixel), Wt::WLength(400, Wt::WLength::Pixel));\n";
	ui_class_defn << "\t//tw->resize(Wt::WLength(100, Wt::WLength::Percentage), Wt::WLength(100, Wt::WLength::Percentage));\n";
	//ui_class_defn << "\tpanel->setLayout(new Wt::WFitLayout());\n";
	//ui_class_defn << "\tpanel->layout()->addWidget();\n";
	struct var_list* v_ptr=tableInfo_->param_list;
	if( v_ptr == 0){
		form_code << "// v_ptr== NULL\n";
	}
	//int counter=0;

	vector <TableInfoType *> vecTableInfo;
	vecTableInfo.push_back(tableInfo_);
	GenerateUITab(ui_class_headers, ui_class_decl, ui_class_defn, false, vecTableInfo);
	

	ui_class_decl << boost::format("\tvoid ProcessInsert%1%();\n")
					% tableInfo_->tableName_;
	ui_class_decl << boost::format("\tint ValidateForInsert%1%();\n")
					% tableInfo_->tableName_;
	ui_class_decl << "\tvoid LoadForm(int32_t pkey);\n";
	ui_class_defn << PrintProcessInsert();
	ui_class_defn << PrintValidateForInsert();
	ui_class_defn << PrintLoadForm();
	// I should use some form of assert to check 
	// that vec_handler_decls.size == vec_handler_defns.size
	for(int i=0; i<vec_handler_decls.size(); ++i) {
		ui_class_decl << endl << vec_handler_decls[i] << endl;
	}
	for(int i=0; i<vec_handler_defns.size(); ++i) {
		ui_class_defn << endl << vec_handler_defns[i] << endl;
	}
	
	//stringstream func_name;
	//func_name << boost::format("formInsert%1%")
	//				% tableInfo_->tableName_;
	//if (tableInfo_->nReferencedAsMulti==0) {
	//	AddNavigationNode(tableInfo_->tableName_, func_name.str());
	//}
	
	form_code << "\tsetCentralWidget(canvas);\n";
	form_code << boost::format("}\n");

	stringstream inc_file;
	inc_file << boost::format("#include \"%1%_bll.h\"\n")
				% tableInfo_->tableName_;
	AddIncludeFile(inc_file.str());
	inc_file.str("");
	inc_file << boost::format("#include \"%1%_ui.h\"\n")
				% tableInfo_->tableName_;
	AddIncludeFile(inc_file.str());


	string ui_h_fname (string(outputDirPrefix_.c_str()
				+ string("/")
				+ tableInfo_->tableName_
				+ string("_ui.h"))); 
	std::ofstream ui_h(ui_h_fname.c_str(), ios_base::out|ios_base::trunc);
	ui_class_decl << boost::format("};\n");
	ui_h << ui_class_headers.str();
	ui_h << ui_class_decl.str();

	string ui_cpp_fname (string(outputDirPrefix_.c_str()
				+ string("/")
				+ tableInfo_->tableName_
				+ string("_ui.cpp"))); 
	std::ofstream ui_cpp(ui_cpp_fname.c_str(), ios_base::out|ios_base::trunc);
	ui_cpp << ui_class_headers.str();
	ui_cpp << boost::format("#include \"%1%_ui.h\"\n")
			% tableInfo_->tableName_ ;

	ui_cpp << boost::format("#include \"%1%_db_postgres.h\"\n\n")
			% tableInfo_->tableName_ ;

	ui_cpp << boost::format("#include <vector>\n\n");
	ui_cpp << boost::format("#include <string>\n\n");
	ui_cpp << boost::format("#include <sstream>\n\n");
	ui_cpp << boost::format("#include <boost/shared_ptr.hpp>\n\n");
	ui_cpp << ui_class_defn.str();


	if (tableInfo_->tab_options.is_login_page) {
		std::stringstream login_inc_file;
		login_inc_file << boost::format("#include \"%1%_Widget.h\"\n")
				% tableInfo_->tableName_;
		AddIncludeFile(login_inc_file.str());
	}


	return form_code.str();
}

void WtUIGenerator::FinalCleanUp()
{
	GenerateUIScaffolding();
	GenerateMakefile();
}


void WtUIGenerator::AddIncludeFile(std::string  p_include_file)
{
	header_files << p_include_file ;
}

void WtUIGenerator::GenerateUITab( std::stringstream & headers,
				std::stringstream & decl,
				std::stringstream & defn, 
				bool called_recursively,
				vector<TableInfoType *> p_vecTableInfo)
{
	using boost::format;
	TableInfoType * aTableInfo = p_vecTableInfo.back();
	p_vecTableInfo.pop_back();
	struct var_list* v_ptr=aTableInfo->param_list;

	//if (called_recursively == false) {
		decl << boost::format("\tWt::WContainerWidget  *wcw_%1%;\n")
				% aTableInfo->tableName_;
		defn << boost::format("\twcw_%1% = new Wt::WContainerWidget();\n")
				% aTableInfo->tableName_;
		decl << boost::format("\tWt::WTable *table_%1%;\n")
				% aTableInfo->tableName_;
		decl << format("\tWt::WPanel * panel_%1%_err_msg;\n") 
				% aTableInfo->tableName_;

		decl << boost::format("\tWt::WContainerWidget  *wcw_%1%_search;\n")
				% aTableInfo->tableName_;
		decl << format("\tWt::Ext::Panel * panel_%1%_search;\n") 
				% aTableInfo->tableName_;
		decl << format("\tWt::WText * wt_%1%_err_msg;\n")
				% aTableInfo->tableName_;
		decl << boost::format("\tWt::WTable *table_%1%_view;\n")
				% aTableInfo->tableName_;
		decl << boost::format("\tWt::WTable *table_%1%_search;\n")
				% aTableInfo->tableName_;
		defn << boost::format("\ttable_%1% = new Wt::WTable(wcw_%1%);\n")
				% aTableInfo->tableName_;

		defn << format("\tpanel_%1%_err_msg = new Wt::WPanel(wcw_%1%);\n")
				% aTableInfo->tableName_;
		defn << format("\tpanel_%1%_err_msg->setTitle(Wt::WString(\"Error Messages\"));\n")
				% aTableInfo->tableName_;
		defn << format("\tpanel_%1%_err_msg->setCollapsible(true);\n")
				% aTableInfo->tableName_;
		defn << format("\tpanel_%1%_err_msg->setCollapsed(true);\n")
				% aTableInfo->tableName_;
		defn << format("\twt_%1%_err_msg = new Wt::WText(\"Error Messages will appear here\");\n")
				% aTableInfo->tableName_;
		defn << format("\tpanel_%1%_err_msg->setCentralWidget(wt_%1%_err_msg);\n")
				% aTableInfo->tableName_;


		// defn << format("	panel_%1%_search = new Wt::Ext::Panel(wcw_%1%);\n") % aTableInfo->tableName_;
		// defn << format("	panel_%1%_search->setTitle(Wt::WString(\"Search\"));\n") % aTableInfo->tableName_;
		// defn << format("	panel_%1%_search->setCollapsible(true);\n") % aTableInfo->tableName_;
		// defn << format("	panel_%1%_search->setCollapsed(false);\n") % aTableInfo->tableName_;
		// defn << format("	table_%1%_search = new Wt::WTable();\n") % aTableInfo->tableName_;
		// defn << format("	panel_%1%_search->setLayout(new Wt::WFitLayout());\n") % aTableInfo->tableName_;
		// defn << format("	Wt::WLabel * dummy_%1%_search_label1 = new Wt::WLabel(\"dummy label1\", table_%1%_search->elementAt(0,0));\n") % aTableInfo->tableName_;
		// defn << format("	Wt::Ext::LineEdit * dummy_%1%_search_textbox1 = new Wt::Ext::LineEdit(\"dummy text1\", table_%1%_search->elementAt(0,1));\n") % aTableInfo->tableName_;
		// defn << format("	Wt::WLabel * dummy_%1%_search_label2 = new Wt::WLabel(\"dummy label2\", table_%1%_search->elementAt(1,0));\n") % aTableInfo->tableName_;
		// defn << format("	Wt::Ext::LineEdit * dummy_%1%_search_textbox2 = new Wt::Ext::LineEdit(\"dummy text2\", table_%1%_search->elementAt(1,1));\n") % aTableInfo->tableName_;
		// defn << format("	panel_%1%_search->layout()->addWidget(table_%1%_search);\n") % aTableInfo->tableName_;

	//}
	
	// defn << boost::format("\ttable_%1%_view = new Wt::WTable(wcw_%1%);\n")
	// 		% aTableInfo->tableName_;
	int counter=0;
	PrintForm(aTableInfo, decl, defn, 
			vec_handler_decls, vec_handler_defns, 
			headers, called_recursively, p_vecTableInfo, counter);


	if (called_recursively==false) {
		decl << format("\tWt::Ext::Button * btn_%1%_search;\n") % 
			aTableInfo->tableName_ ;
		defn << PrintUISearchPanel(aTableInfo, decl);

		stringstream search_func_defn, search_func_decl;
		print_SearchFunction(search_func_decl, search_func_defn);
		vec_handler_decls.push_back(search_func_decl.str());
		vec_handler_defns.push_back(search_func_defn.str());

		defn << boost::format("\tstd::vector<boost::shared_ptr <Biz%2%> > page1 = %1%::db::%2%::Get%2%(0, 10") %
			project_namespace % aTableInfo->tableName_;

		//string search_key_args_str =  tableInfo_->print_cpp_search_key_args() ;
		//if (search_key_args_str != "") {
		if (tableInfo_->has_search_key >0) {
			defn << ",\n";
			defn << tableInfo_->print_cpp_search_key_args();
		}
		if (tableInfo_->nSessionParams>0) {
			if (tableInfo_->has_search_key >0) {
				defn << ",\n";
			}
			defn << tableInfo_->print_cpp_session_key_args();
		}
		defn << ");\n";

		stringstream load_func_defn, load_func_decl;
		PrintLoadSummaryTableView(aTableInfo, load_func_decl, load_func_defn, 
			// vec_handler_decls, vec_handler_defns, 
			headers
			//, called_recursively, p_vecTableInfo, counter
			);
		vec_handler_decls.push_back(load_func_decl.str());
		vec_handler_defns.push_back(load_func_defn.str());
		defn << boost::format("\ttable_%1%_view = new Wt::WTable(wcw_%1%);\n")
				% aTableInfo->tableName_;
		defn << "\tLoadSummaryTableView(page1);\n";


	}

	defn << boost::format("\ttw->addTab(wcw_%1%, \"%1%\");\n")
				% aTableInfo->tableName_;
	defn << "\ttw->resize(Wt::WLength::Auto, Wt::WLength(500, Wt::WLength::Pixel));\n";

	if (p_vecTableInfo.size()>0) {
		GenerateUITab(headers, decl, defn, true, p_vecTableInfo);
	}
	
	fixme(__FILE__, __LINE__, __PRETTY_FUNCTION__, "I think the code below should go into the PrintForm - analyse this later ");
	if (called_recursively==false) {
		decl << boost::format("\tWt::WPushButton * wpb_insert;\n");
		defn << boost::format("\twpb_insert = new Wt::WPushButton(Wt::WString(\"Add\"), table_%3%->elementAt(%1%, 0));\n")
				% counter% aTableInfo->tableName_% aTableInfo->tableName_;
		defn << "\twpb_insert->setText(Wt::WString(\"Add\"));\n";
		defn << boost::format("\twpb_insert->clicked().connect(wpb_insert, &Wt::WPushButton::disable);\n");
		defn << boost::format("\twpb_insert->clicked().connect(this, &%1%_ui::ProcessInsert%1%);\n")
						% tableInfo_->tableName_;
		defn << "}\n";
	}
}

/*
bool ReferencedTableContainsUs(TableInfoType *me, std::string ref_table_name)
{
	TableInfoType * ti_ptr = find_TableInfo(ref_table_name);
	if (ti_ptr ==0 ){
		cerr << " Referenced table: " << ref_table_name << " not found ... exiting";
		exit(1);
	}
	
	struct var_list* v_ptr=ti_ptr->param_list;
	while (v_ptr) {
		if (v_ptr->var_name == me->tableName_ &&
				v_ptr->var_type==COMPOSITE_TYPE) {
			
			return true;
		}
		v_ptr=v_ptr->prev;
	}
	return false;
}
*/

using global_options::input_file_name;

void WtUIGenerator::GenerateMakefile()
{
	// log_mesg(__FILE__, __LINE__, __PRETTY_FUNCTION__, " ENTER ");	
	std::stringstream makefile_str;
	makefile_objs << " TimesheetCalendar.o CalendarCell.o LoginWidget.o";
	makefile_str << "CXX := $(CXX) -g " << endl;
	makefile_str << "OBJS = "
		<< makefile_objs.str() << endl;
	makefile_str << "LINK_LIBS = -lwt -lwtext -lwthttp -lpq"
		<< endl;
	makefile_str << endl
		<< "test_ui: $(OBJS) wt_ui.o " << endl
		<< "\t$(CXX) -g -o $@ $(OBJS) wt_ui.o $(LINK_LIBS)" << endl
		<< endl;
	makefile_str << "%.o: %.cpp %.h" << endl
		<< "\t$(CXX) -g -c $<" << endl << endl;
	stringstream makefile_fname;

	makefile_fname << outputDirPrefix_ << "/Makefile." << input_file_name ;
	std::ofstream makefile(makefile_fname.str().c_str(), ios_base::out|ios_base::trunc);
	makefile << makefile_str.str();
	// log_mesg(__FILE__, __LINE__, __PRETTY_FUNCTION__, " EXIT ");	
}


string WtUIGenerator::print_ChoiceHandler(struct var_list * p_vptr, std::stringstream & decl)
{
	stringstream func_defn;
	func_defn << boost::format("void %2%_ui::HandleChoose%1%()\n{\n")
		% p_vptr->var_name % tableInfo_->tableName_;
	func_defn << boost::format("\twd_choose_%1% = new Wt::Ext::Dialog(\"Choose %1%\");\n")
		% p_vptr->var_name;
	func_defn << boost::format("\tWt::WPushButton*  ok= new Wt::WPushButton(\"Ok\", wd_choose_%1%->contents());\n")
		% p_vptr->var_name;
	func_defn << boost::format("\tok->clicked().connect(wd_choose_%1%, &Wt::Ext::Dialog::accept);\n")
		% p_vptr->var_name;


	TableInfoType * aTableInfo = find_TableInfo(p_vptr->options.ref_table_name);
	// should check for null here and exit

	stringstream inc_file;
	inc_file << boost::format("#include \"%1%_bll.h\"\n")
				% aTableInfo->tableName_;
	inc_file << boost::format("#include \"%1%_db_postgres.h\"\n")
				% aTableInfo->tableName_;
	decl << inc_file.str();
	func_defn << boost::format("/* file: %1%, line: %2%: func: %3% added include files */\n") %
		__FILE__ % __LINE__ % __PRETTY_FUNCTION__ ;
	func_defn<< boost::format("\tstd::vector<boost::shared_ptr <Biz%2%> > page1_%2% = %1%::db::%2%::Get%2%(0, 10") %
			project_namespace % aTableInfo->tableName_;
	string search_key_args_str =  aTableInfo->print_cpp_search_key_args() ;
	if (search_key_args_str != "") {
		func_defn << ",\n";
		func_defn << search_key_args_str;
	}
	func_defn << ");\n";

	func_defn << format("\ttable_%1%_view = new Wt::WTable(wd_choose_%2%->contents());\n") %
		p_vptr->options.ref_table_name % p_vptr->var_name;
	struct var_list* v_ptr=aTableInfo->param_list;
	{
		int counter =0;
		for (; v_ptr; v_ptr=v_ptr->prev) {
			// I need to fix this - like add a function which states "simple_variable" - 
			// functional programming style - as mentioned in the LISP books
			if (v_ptr->options.ui_select) {
				if (v_ptr->options.ref_table_name == "") {
					func_defn << format("\ttable_%1%_view->elementAt(0, %2%)->addWidget(new Wt::WText(\"%3%\"));\n") %
						p_vptr->options.ref_table_name % counter++ % v_ptr->var_name;
				}
			}
		}
	}
	struct CppCodeGenerator * ref_table_ptr = (dynamic_cast<CppCodeGenerator*>
			(TableCollectionSingleton::Instance()
				.my_find_table(p_vptr->options.ref_table_name)));

	func_defn << format("\tfor (int i=0; i<page1_%1%.size(); ++i) {\n") %
		p_vptr->options.ref_table_name;
	v_ptr=aTableInfo->param_list;
	func_defn << "\t\tstd::stringstream temp1;\n";
	{
		int counter =0; // start at row 2 - titles in row 1
		for (; v_ptr; v_ptr=v_ptr->prev) {
			if (v_ptr->options.ui_select) {
				if (v_ptr->options.primary_key /* v_ptr == aTableInfo->param_list*/) {
					func_defn << boost::format("\t\tWt::WPushButton * b = new Wt::WPushButton(\"Select\", table_%1%_view->elementAt(i+1, %2%));\n") %
						p_vptr->options.ref_table_name % counter++;
					func_defn << boost::format("\t\tb->clicked().connect(boost::bind(&%1%_ui::XferChoice%3%, this, page1_%2%[i]->%3%_")
							%
						tableInfo_->tableName_ % p_vptr->options.ref_table_name % p_vptr->var_name;
					//func_defn << "\n\t\t\t/* reached here table name:" 
					//	<< ref_table_ptr->tableInfo_->tableName_
					//	<< " nUIDialogSelectXfer: " << ref_table_ptr->tableInfo_->nUIDialogSelectXfer
					//	<<  " */\n\t\t\t";


					struct var_list * v_ptr2 = ref_table_ptr->tableInfo_->param_list;
					if (ref_table_ptr->tableInfo_->nUIDialogSelectXfer) {
						func_defn << ",\n";
						int count_sel_xfer = 0;
						bool print_comma2 = false;
						while (v_ptr2) {
							if (v_ptr2->options.ui_dialog_select_xfer) {
								func_defn << format("\t\t\tpage1_%2%[i]->%1%_") % v_ptr2->var_name
									% p_vptr->options.ref_table_name 
									;
								print_comma2 = true;
								++count_sel_xfer;
							}
							// func_defn << "/* looping count_sel_xfer:" << count_sel_xfer << " */";
							v_ptr2 = v_ptr2->prev;
							if (count_sel_xfer < ref_table_ptr->tableInfo_->nUIDialogSelectXfer) {
								// func_defn << "\t\t\t/* count_sel_xfer < nUIDialogSelectXfer */\n";
								if (print_comma2) {
									func_defn << ", ";
									print_comma2 = false;
								}
							} else {
								//func_defn << "\t\t\t/* breaking out count_sel_xfer: " 
								//	<< count_sel_xfer << " */\n";
								break;
							}
						}
					}

					func_defn << "));\n";
					func_defn << boost::format("\t\tb->clicked().connect(wd_choose_%1%, &Wt::Ext::Dialog::accept);\n")
						% p_vptr->var_name;
				} else if (v_ptr->options.ref_table_name == "") {
					func_defn << boost::format("\t\ttemp1 << page1_%2%[i]->%1%_;\n") %
						v_ptr->var_name % p_vptr->options.ref_table_name;
					func_defn << format("\t\ttable_%1%_view->elementAt(i+1, %2%)->addWidget(new Wt::WText(temp1.str()));\n") %
						p_vptr->options.ref_table_name % counter++ ;
					func_defn << "\t\ttemp1.str(\"\");\n";
				}
			}
		}
	}
	func_defn << "\t}\n";
	func_defn << boost::format("\twd_choose_%1%->exec();\n")
		% p_vptr->var_name;

	func_defn << "}\n";
	return func_defn.str();
}


/*
std::string WtUIGenerator::print_cpp_search_key_args()
{
	stringstream search_key_fields_str;
	struct var_list* v_ptr=tableInfo_->param_list;
	if(tableInfo_->has_search_key){
		int count=0;
		while(v_ptr){
			if(v_ptr->options.search_key){
				//search_key_fields_str <<  boost::format("\t\t");
				//search_key_fields_str << print_cpp_types(v_ptr->var_type);
				switch (v_ptr->var_type) {
				case TEXT_TYPE:
				case VARCHAR_TYPE:
				case NVARCHAR_TYPE:
				case NCHAR_TYPE:
				case NTEXT_TYPE:
					search_key_fields_str << "std::string (\"%\")";
				break;
				default:
					search_key_fields_str << boost::format(" unhandled file: %1% line: %2% func: %3%")
						% __FILE__ % __LINE__ % __PRETTY_FUNCTION__;
				}
				++count;
				if(count<tableInfo_->has_search_key){
					search_key_fields_str <<  ",\n";
				} else 
					search_key_fields_str << "\n";
			}
			v_ptr=v_ptr->prev;
		}
	} else {
		 search_key_fields_str << "";
	}
	return search_key_fields_str.str();
}
*/


string WtUIGenerator::print_XferFunction(struct var_list * p_vptr, std::stringstream & decl, std::stringstream & p_prototype)
{
	stringstream func_defn;
	using boost::format;
	func_defn << format("void %2%_ui::XferChoice%1%(int32_t p_%1%")
		% p_vptr->var_name % tableInfo_->tableName_;
	p_prototype << format("\tvoid XferChoice%1%(int32_t p_%1%")
		% p_vptr->var_name ;

	struct CppCodeGenerator * ref_table_ptr = (dynamic_cast<CppCodeGenerator*>
			(TableCollectionSingleton::Instance()
				.my_find_table(p_vptr->options.ref_table_name)));
	/*
	struct var_list * v_ptr2 = ref_table_ptr->tableInfo_->param_list;
	int count_sel_xfer = 0;
	bool print_comma2 = false;
	while (v_ptr2) {
		if (v_ptr2->options.ui_dialog_select_xfer) {
			func_defn << format("%1% %2%_") 
				% v_ptr2->print_cpp_var_type()
				% v_ptr2->var_name
				;
			print_comma2 = true;
			++count_sel_xfer;
		}
		v_ptr2 = v_ptr2->prev;
		if (count_sel_xfer < tableInfo_->nUIDialogSelectXfer && print_comma2) {
			func_defn << ", ";
			print_comma2 = false;
		} else {
			break;
		}
	}
	*/


	/* =================== */
	struct var_list * v_ptr2 = ref_table_ptr->tableInfo_->param_list;
	if (ref_table_ptr->tableInfo_->nUIDialogSelectXfer) {
		func_defn << ",\n";
		p_prototype << ",\n";
		int count_sel_xfer = 0;
		bool print_comma2 = false;
		while (v_ptr2) {
			if (v_ptr2->options.ui_dialog_select_xfer) {
				func_defn << format("\t%1% %2%_") 
					% v_ptr2->print_cpp_var_type()
					% v_ptr2->var_name
					;
				p_prototype << format("\t\t%1% %2%_") 
					% v_ptr2->print_cpp_var_type()
					% v_ptr2->var_name
					;
				print_comma2 = true;
				++count_sel_xfer;
			}
			// func_defn << "/* looping count_sel_xfer:" << count_sel_xfer << " */";
			v_ptr2 = v_ptr2->prev;
			if (count_sel_xfer < ref_table_ptr->tableInfo_->nUIDialogSelectXfer) {
				// func_defn << "\t\t\t/* count_sel_xfer < nUIDialogSelectXfer */\n";
				if (print_comma2) {
					func_defn << ", ";
					print_comma2 = false;
				}
			} else {
				//func_defn << "\t\t\t/* breaking out count_sel_xfer: " 
				//	<< count_sel_xfer << " */\n";
				break;
			}
		}
	}
	/* =================== */

	p_prototype << format(");\n");
	func_defn << ")\n{\n";


	TableInfoType * aTableInfo = find_TableInfo(p_vptr->options.ref_table_name);
	// should check for null here and exit
	func_defn << "\tstd::stringstream temp;\n";
	func_defn << format("\ttemp <<  p_%1%;\n") % p_vptr->var_name;
	func_defn << format("\twt_%1%_value->setText(Wt::WString(temp.str()));\n") %
		p_vptr->var_name;
	//func_defn << boost::format("\twpb_choose_%1%->clicked().connect(wpb_choose_%1%, &Wt::WPushButton::enable);\n")
	//	% p_vptr->var_name;
	func_defn << boost::format("\twpb_choose_%1%->enable();\n")
		% p_vptr->var_name;
	
	func_defn << "}\n\n";
	return func_defn.str();
}

std::string WtUIGenerator::PrintProcessInsert()
{
	std::stringstream process_insert_defn, dummy_defns, header;
	
	using boost::format;
	header << format("void %1%_ui::ProcessInsert%1%()\n{\n")
					% tableInfo_->tableName_;
	process_insert_defn << format("\tif (ValidateForInsert%1%()!=0) {\n"
		 "\t\treturn;\n"
		 "\t}\n") % tableInfo_->tableName_;
	struct var_list* v_ptr=tableInfo_->param_list;
	process_insert_defn << format("\tboost::shared_ptr<Biz%1%> ptr_%1% (new Biz%1%(\n")
			% tableInfo_->tableName_;
	bool print_comma = false;
	while (v_ptr) {
		if (v_ptr->options.primary_key) {
			process_insert_defn << "\t\t-1";
			print_comma = true;
		} else if (v_ptr->var_type == COMPOSITE_TYPE) {
			dummy_defns << format("\tstd::vector<boost::shared_ptr<Biz%1%> > l_dummy_%1%;\n") %
					v_ptr->options.ref_table_name;
			process_insert_defn << format("\t\tl_dummy_%1%") %
					v_ptr->options.ref_table_name;
			print_comma = true;
		} else if (v_ptr->var_type == DATETIME_TYPE 
				&& v_ptr->options.embedded == false
				) {
			//process_insert_defn << " /* reached here embedded == false*/ ";
			// process_insert_defn << format("\t\tboost::lexical_cast<%1%>(we_%2%->text())")
			// 	% v_ptr->print_cpp_var_type() % v_ptr->var_name;
			//process_insert_defn << "\t\t{\n\t\t\tstring s( (we_%2%->toString(\"yyyy-M-d\")).toUTF8());\n";
			process_insert_defn << format("\t\tboost::gregorian::from_simple_string( ( (we_%1%->date()).toString(\"yyyy-M-d\")).toUTF8() )")
			 	% v_ptr->var_name;
			//process_insert_defn << "\t\t}\n";
			print_comma = true;
		} else if (v_ptr->var_type == DATETIME_TYPE 
				&& v_ptr->options.embedded == true
				) {
			// process_insert_defn << " /* reached here embedded == true*/ ";
			process_insert_defn << format("\t\tboost::gregorian::date(boost::gregorian::from_simple_string(\"2010-11-21\"))");
				// % v_ptr->print_cpp_var_type() % v_ptr->var_name;
			print_comma = true;
		} else if (v_ptr->options.ref_table_name != ""
				&& v_ptr->options.many == false) {
			process_insert_defn << format("\t\tboost::lexical_cast<%1%>(wt_%2%_value->text())")
				 % v_ptr->print_cpp_var_type() % v_ptr->var_name;
			print_comma = true;
		} else {
			process_insert_defn << format("\t\tboost::lexical_cast<%1%>(we_%2%->text())")
				% v_ptr->print_cpp_var_type() % v_ptr->var_name;
			print_comma = true;
		}
		v_ptr = v_ptr->prev;
		if(v_ptr && print_comma) {
			process_insert_defn << ",\n";
			print_comma = false;
		} else {
			process_insert_defn << "\n";
		}
	}
	process_insert_defn << format("\t\t) );\n");
	process_insert_defn << format("\tLoadForm(ptr_%1%->Insert());\n")
			% tableInfo_->tableName_;
		
	process_insert_defn << "}\n\n";

	stringstream func;
	func << header.str() << dummy_defns.str() << process_insert_defn.str();
	//return process_insert_defn.str();
	return func.str();
}

void WtUIGenerator::PrintListViewHeaders(stringstream  & p_load_table_view_str, vector<TableInfoType* > & p_vec_list_view_stack
		, int max_recursion_level, int recursion_level, int & nColumns)
{
	TableInfoType * aTableInfo = p_vec_list_view_stack.back();
	struct var_list* v_ptr=aTableInfo->param_list;
	while (v_ptr) {
		if (v_ptr->options.ui_view && (!v_ptr->options.primary_key) ) {
			if (v_ptr->options.ref_table_name == "") {
				p_load_table_view_str << format("\ttable_%1%_view->elementAt(0, %2%)->addWidget(new Wt::WText(Wt::WString::tr(\"%3%\")));\n") %
					tableInfo_ /* this is not a mistake */ ->tableName_ % nColumns++ % v_ptr->var_name;
			}
		}
		v_ptr = v_ptr->prev;
	}
}


void WtUIGenerator::PrintListViewData(stringstream  & p_load_table_view_str, vector<TableInfoType* > & p_vec_list_view_stack
		, int max_recursion_level, int recursion_level, int & nColumns)
{
	TableInfoType * aTableInfo = p_vec_list_view_stack.back();
	struct var_list* v_ptr=aTableInfo->param_list;
	while (v_ptr) {
		if (v_ptr->options.ui_view && (!v_ptr->options.primary_key) ) {
			if (v_ptr->options.ref_table_name == "") {
				p_load_table_view_str << boost::format("\t\ttemp1 << page1[i]->biz_%1%_->%2%_;\n") %
					aTableInfo->tableName_ % v_ptr->var_name;
				p_load_table_view_str << format("\t\ttable_%1%_view->elementAt(i+1, %2%)->addWidget(new Wt::WText(temp1.str()));\n") %
					tableInfo_->tableName_ % nColumns++;
				p_load_table_view_str << "\t\ttemp1.str(\"\");\n";
			}
		}
		v_ptr = v_ptr->prev;
	}
}


std::string WtUIGenerator::PrintValidateForInsert()
{
	std::stringstream validate_for_insert_defn;
	using boost::format;
	validate_for_insert_defn << format("int %1%_ui::ValidateForInsert%1%()\n{\n")
					% tableInfo_->tableName_;
	validate_for_insert_defn << "\tstd::stringstream ss_err_msgs;\n";
	validate_for_insert_defn << "\tint nErrors = 0;\n";
	validate_for_insert_defn << format("\tWt::WValidator::State validation_status;\n");
	struct var_list* v_ptr=tableInfo_->param_list;
	while (v_ptr) {
		if (v_ptr->options.ref_table_name != "" && v_ptr->options.many == false) {
			validate_for_insert_defn << format("\tif (wt_%1%_value->text() == \"<not selected>\" ) {\n"
				"\t\tss_err_msgs << \"Please select a value for %1%\";\n"
				"\t\t++nErrors;\n"
				"\t}\n") % v_ptr->var_name ;
		} else if (v_ptr->var_type == DATETIME_TYPE && v_ptr->options.embedded == false) {
			validate_for_insert_defn << format("\tvalidation_status = we_%1%->validate();\n")
				% v_ptr->var_name;
			validate_for_insert_defn << format("\tif(validation_status != Wt::WValidator::Valid) {\n"
					"\t\tss_err_msgs << \"Please choose a valid date for %1%\\n\";\n"
					"\t\t++nErrors;\n"
					"\t}\n") % v_ptr->var_name;
		} else if (v_ptr->var_type == FLOAT_TYPE || v_ptr->var_type == DOUBLE_TYPE) {
			validate_for_insert_defn << format("\tvalidation_status = we_%1%->validate();\n")
				% v_ptr->var_name;
			validate_for_insert_defn << format("\tif(validation_status != Wt::WValidator::Valid) {\n"
					"\t\tss_err_msgs << \"Please enter a valid number for %1%\\n\";\n"
					"\t\t++nErrors;\n"
					"\t}\n") % v_ptr->var_name;
		}
		v_ptr = v_ptr->prev;
	}
	validate_for_insert_defn << "\tif (nErrors>0) {\n";
	validate_for_insert_defn << format("\t\twt_%1%_err_msg->setText(ss_err_msgs.str());\n") %
			tableInfo_->tableName_;
	validate_for_insert_defn << format("\t\tpanel_%1%_err_msg->expand();\n") %
			tableInfo_->tableName_;
	validate_for_insert_defn << "\t}\n";
	validate_for_insert_defn << boost::format("\twpb_insert->setEnabled(true);\n");
	validate_for_insert_defn << "\treturn nErrors;\n";
	validate_for_insert_defn << "}\n\n";
	return validate_for_insert_defn.str();
}

std::string WtUIGenerator::PrintLoadForm()
{
	stringstream load_form_func;
	load_form_func << format("void %1%_ui::LoadForm(int32_t pkey)\n{\n")
		% tableInfo_->tableName_;
	load_form_func << format("\tboost::shared_ptr<Biz%2%> l_biz_%2%( %1%::db::%2%::GetSingle%2% (pkey));\n")
		% project_namespace % tableInfo_->tableName_;
	int counter=0;
	struct var_list* v_ptr = tableInfo_->param_list;
	for (; v_ptr; v_ptr=v_ptr->prev, ++counter) {
		if (v_ptr->options.ref_table_name!=""
				&& ReferencedTableContainsUs(tableInfo_, v_ptr->options.ref_table_name) ) {
			continue;
		} 
			
		if (v_ptr->options.primary_key) {
			// need to save this later into some variable in ui
			continue;
		}
		if (v_ptr->var_type==COMPOSITE_TYPE) {
			/*
			TableInfoType * ti_ptr = find_TableInfo(v_ptr->var_name);
			p_vecTableInfo.push_back(ti_ptr);
			*/
			continue;
		}
		if (v_ptr->options.ref_table_name!="" 
				&& v_ptr->var_type != COMPOSITE_TYPE
				 ) {
			if (! v_ptr->options.session ) {
				load_form_func << 
					boost::format("\twt_%1%_value->setText(boost::lexical_cast<std::string>(l_biz_%2%->biz_%3%_->%1%_));\n")
							% v_ptr->var_name % tableInfo_->tableName_ % v_ptr->options.ref_table_name;
			}
		} else if (v_ptr->var_type==DATETIME_TYPE && v_ptr->options.embedded == false) {
			load_form_func << boost::format("\t/*we_%1%->setDate(l_biz_%2%->%1%_);*/\n")
					% v_ptr->var_name % tableInfo_->tableName_;
		} else if (v_ptr->var_type==DATETIME_TYPE && v_ptr->options.embedded == true) {
			load_form_func << boost::format("\t/*\n ts_cal_%1% ->setDate(l_biz_%2%->%1%_);\n\t*/\n")
					% v_ptr->var_name% tableInfo_->tableName_;
		} else if (v_ptr->var_type==DOUBLE_TYPE || v_ptr->var_type==FLOAT_TYPE) {
			load_form_func << boost::format("\twe_%1%->setText(boost::lexical_cast<std::string>(l_biz_%2%->%1%_));\n")
					% v_ptr->var_name % tableInfo_->tableName_;
		} else if(v_ptr->var_type==INT32_TYPE || v_ptr->var_type == BIGINT_TYPE) {
			load_form_func << boost::format("\twe_%1%->setText(boost::lexical_cast<std::string>(l_biz_%2%->%1%_));\n")
					% v_ptr->var_name % tableInfo_->tableName_;

		} else if(v_ptr->var_type==BIT_TYPE ) {
			load_form_func << boost::format("\twe_%1%->setText( (l_biz_%2%->%1%_ == true ? \"1\" : \"0\") );\n")
					% v_ptr->var_name % tableInfo_->tableName_;
		} else {
			load_form_func << boost::format("\twe_%1%->setText(l_biz_%2%->%1%_);\n")
					% v_ptr->var_name % tableInfo_->tableName_;
		}
	}

	load_form_func << "}\n\n";
	return load_form_func.str();
}



std::string WtUIGenerator::PrintUISearchPanel(TableInfoType * p_ptrTableInfo, std::stringstream & decl)
{
	stringstream search_panel_str;
	search_panel_str << format("	panel_%1%_search = new Wt::Ext::Panel(wcw_%1%);\n") % p_ptrTableInfo->tableName_;
	search_panel_str << format("	panel_%1%_search->setTitle(Wt::WString(\"Search\"));\n") % p_ptrTableInfo->tableName_;
	search_panel_str << format("	panel_%1%_search->setCollapsible(true);\n") % p_ptrTableInfo->tableName_;
	search_panel_str << format("	panel_%1%_search->setCollapsed(false);\n") % p_ptrTableInfo->tableName_;
	search_panel_str << format("	wcw_%1%_search = new Wt::WContainerWidget();\n") % p_ptrTableInfo->tableName_;
	search_panel_str << format("	table_%1%_search = new Wt::WTable(wcw_%1%_search);\n") % p_ptrTableInfo->tableName_;
	search_panel_str << format("	panel_%1%_search->setLayout(new Wt::WFitLayout());\n") % p_ptrTableInfo->tableName_;
	struct var_list* v_ptr = p_ptrTableInfo->param_list;
	int counter = 0;
	int modulus_counter = 0;
	while (v_ptr) {
		if (v_ptr->options.search_key) {
			decl << format("\tWt::Ext::LineEdit * le_%1%_search;\n") % 
					v_ptr->var_name;
			search_panel_str << format("\tWt::WLabel * lbl_%4%_search = new Wt::WLabel(\"%4%\", table_%1%_search->elementAt(%2%, %3%));\n") % p_ptrTableInfo->tableName_ %
					counter % modulus_counter % v_ptr->var_name;
			search_panel_str << format("\tle_%4%_search = new Wt::Ext::LineEdit(\"\", table_%1%_search->elementAt(%2%, %3%));\n") % p_ptrTableInfo->tableName_ %
					counter % (modulus_counter + 1) % v_ptr->var_name;
			if (modulus_counter == 0) {
				modulus_counter += 2;
			} else /*if (modulus_counter == 2) */ {
				modulus_counter = 0; counter++;
			}
		}
		v_ptr = v_ptr->prev;
	}
	search_panel_str << format("	btn_%1%_search = new Wt::Ext::Button(\"Search\", table_%1%_search->elementAt(%2%, %3%));\n") % 
		p_ptrTableInfo->tableName_ % (counter+1) % 0;

	search_panel_str << format("	btn_%1%_search->clicked().connect(this, &%1%_ui::SearchAndLoadView);\n") % 
		p_ptrTableInfo->tableName_ ;
	search_panel_str << format("	panel_%1%_search->layout()->addWidget(wcw_%1%_search);\n") % p_ptrTableInfo->tableName_;
	return search_panel_str.str();
}


void WtUIGenerator::print_SearchFunction(stringstream & decl, stringstream & defn)
{
	decl << "\tvoid SearchAndLoadView();\n";
	defn << format("void %1%_ui::SearchAndLoadView()\n{\n") %
		tableInfo_->tableName_ ;
	struct var_list * v_ptr = tableInfo_->param_list;
	using boost::format;
	defn << format("\tstd::vector<boost::shared_ptr <Biz%2%> > page = %1%::db::%2%::Get%2%(0, 10") %
		project_namespace % tableInfo_->tableName_;
	int count=0;
	if (tableInfo_->has_search_key > 0) {
		defn << ",\n";
	}
	bool print_comma = false;
	while (v_ptr) {
		if (v_ptr->options.search_key &&
				v_ptr->var_type == VARCHAR_TYPE) {
			defn << format("\t\tstd::string(\"%%\") + le_%1%_search->text().toUTF8() + std::string(\"%%\")") % 
					v_ptr->var_name;
			print_comma = true;
			++count;
		} else if(v_ptr->options.search_key && v_ptr->var_type == DATETIME_TYPE) {
			fixme(__FILE__, __LINE__, __PRETTY_FUNCTION__, "session date should be 2 params: start and end");
			defn << "\t\tboost::gregorian::date(boost::gregorian::from_simple_string(\"2001-10-14\"))";
			print_comma = true;
			++count;
		}
		if (print_comma && (count < tableInfo_->has_search_key)) {
			defn << ",\n";
			print_comma = false;
		}
		v_ptr = v_ptr->prev;
	}
	if (tableInfo_->nSessionParams>0) {
		if (tableInfo_->has_search_key >0) {
			defn << ",\n";
		}
		defn << tableInfo_->print_cpp_session_key_args();
	}
	defn << ");\n";
	defn << "\tLoadSummaryTableView(page);\n";

	defn << "}\n\n";
}


// void WtUIGenerator::PrintLoadSummaryTableView(TableInfoType * p_ptrTableInfo, 
// 			std::stringstream & decl, std::stringstream & defn,
// 			std::vector<std::string> & vec_handler_decls, std::vector<std::string> &vec_handler_defns,
// 			std::stringstream & headers
// 			)

void WtUIGenerator::PrintForm(TableInfoType * p_ptrTableInfo, 
			std::stringstream & decl, std::stringstream & defn,
			std::vector<std::string> & vec_handler_decls, std::vector<std::string> &vec_handler_defns,
			std::stringstream & headers, bool & called_recursively,
			vector<TableInfoType *> & p_vecTableInfo, int & counter
			)
{
	// defn << boost::format("\ttable_%1%_view = new Wt::WTable(wcw_%1%);\n")
	// 		% p_ptrTableInfo->tableName_;
	struct var_list* v_ptr=p_ptrTableInfo->param_list;
	for (; v_ptr; v_ptr=v_ptr->prev, ++counter) {
		// if (v_ptr->options.ref_table_name!="") {
		// 	cout << " called_recursively: " << called_recursively
		// 		<< endl;
		// 	cout << " my table name: " << p_ptrTableInfo->tableName_
		// 		<< endl;
		// 	cout << " ref_table_name: " << v_ptr->options.ref_table_name
		// 		<< endl;
		// 	cout << "ReferencedTableContainsUs: " 
		// 		<< ReferencedTableContainsUs(p_ptrTableInfo, v_ptr->options.ref_table_name)
		// 		<< endl;
		// }
		//! called_recursively == true means this form is being generated in the context of 
		//! the original table that contains this one as a composite object
		//! for such tables the foreign key is being supplied by the master table
		//! for example in the case of "employee, employeestatus" in which 
		//! employeestatus is also contained in employee as separate composite object
		//! when generating the employeestatus tab - we dont want to display the employee 
		//! code again in employeestatus - it is already supplied in the employee tab
		if (called_recursively && v_ptr->options.ref_table_name!=""
				&& ReferencedTableContainsUs(p_ptrTableInfo, v_ptr->options.ref_table_name) ) {
			continue;
		} 
			
		if (v_ptr->options.primary_key) {
			continue;
		}
		if (v_ptr->var_type==COMPOSITE_TYPE) {
			TableInfoType * ti_ptr = find_TableInfo(v_ptr->var_name);
			p_vecTableInfo.push_back(ti_ptr);
			continue;
		}
		decl <<  boost::format("\tWt::WLabel * wt_%1%;\n")
					% v_ptr->var_name;
		defn << 
			boost::format("\twt_%2% = new Wt::WLabel(Wt::WString::tr(\"%2%\"),\n" 
					"\t\t\ttable_%3%->elementAt(%1%, 0));\n")
					% counter % v_ptr->var_name % p_ptrTableInfo->tableName_;
		if (v_ptr->options.ref_table_name!="" 
				&& v_ptr->var_type != COMPOSITE_TYPE
				 ) {
			decl <<  boost::format("\tWt::WLabel * wt_%1%_value;\n")
						% v_ptr->var_name;
			decl <<  boost::format("\tWt::WPushButton * wpb_choose_%1%;\n")
						% v_ptr->var_name;
			decl << format("\tWt::WTable *table_%1%_view;\n") %
						v_ptr->options.ref_table_name;
			defn << 
				boost::format("\twt_%2%_value = new Wt::WLabel(Wt::WString(\"<not selected>\"),\n" 
						"\t\t\ttable_%3%->elementAt(%1%, 1));\n")
						% counter % v_ptr->var_name % p_ptrTableInfo->tableName_;
			defn << 
				boost::format("\twpb_choose_%2%= new Wt::WPushButton(Wt::WString(\" ... \"),\n" 
						"\t\t\ttable_%3%->elementAt(%1%, 2));\n")
						% counter % v_ptr->var_name % p_ptrTableInfo->tableName_;
			defn << 
				boost::format("\twpb_choose_%2%->setToolTip( Wt::WString(\"Choose \") + Wt::WString::tr(\"%2%\"));\n" )
						% counter % v_ptr->var_name;
			defn << boost::format("\twpb_choose_%1%->clicked().connect(wpb_choose_%1%, &Wt::WPushButton::disable);\n")
				% v_ptr->var_name;
			defn << boost::format("\twpb_choose_%1%->clicked().connect(this, &%2%_ui::HandleChoose%1%);\n")
							% v_ptr->var_name % tableInfo_->tableName_;
			decl << boost::format("\tWt::Ext::Dialog * wd_choose_%1%;\n")
						% v_ptr->var_name;
			stringstream handle_func_decl;
			handle_func_decl << boost::format("\tvoid HandleChoose%1%();\n")
				% v_ptr->var_name;
			vec_handler_decls.push_back(handle_func_decl.str());
			
			stringstream handle_func_defn;
			handle_func_defn << print_ChoiceHandler(v_ptr, headers  /* required for header files of composite objects */);
			vec_handler_defns.push_back(handle_func_defn.str());
			stringstream xfer_func_decl;
			// xfer_func_decl << boost::format("\tvoid XferChoice%1%(int p_%1%);\n") %
			// 	v_ptr->var_name;
			// vec_handler_decls.push_back(xfer_func_decl.str());
			
			stringstream xfer_func_defn;
			xfer_func_defn << print_XferFunction(v_ptr, headers, xfer_func_decl  /* required for header files of composite objects */);
			vec_handler_decls.push_back(xfer_func_decl.str());
			vec_handler_defns.push_back(xfer_func_defn.str());
		} else if (v_ptr->var_type==DATETIME_TYPE && v_ptr->options.embedded == false) {
			decl <<  boost::format("\tWt::Ext::DateField * we_%1%;\n")
						% v_ptr->var_name;
			decl << format("\tWt::WDateValidator * wdv_%1%;\n")
						% v_ptr->var_name;
			defn << boost::format("\twe_%2% = new Wt::Ext::DateField(table_%3%->elementAt(%1%, 1));\n")
					% counter % v_ptr->var_name% p_ptrTableInfo->tableName_;
			defn << format("\twdv_%1% = new Wt::WDateValidator();\n")
						% v_ptr->var_name;
			defn << format("\twe_%1%->setFormat(Wt::WString(\"dd-MMM-yyyy\"));\n") % v_ptr->var_name;
			defn << format("\twdv_%1%->setFormat(Wt::WString(\"dd-MMM-yyyy\"));\n") % v_ptr->var_name;
			defn << format("\twe_%1%->setValidator(wdv_%1%);\n") % v_ptr->var_name;
			if (v_ptr->options.null == false) {
				defn << format("\twdv_%1%->setMandatory(true);\n") % v_ptr->var_name;
			}
		} else if (v_ptr->var_type==DATETIME_TYPE && v_ptr->options.embedded == true) {
			decl <<  boost::format("\tTimesheetCalendar * ts_cal_%1%;\n")
						% v_ptr->var_name;
			defn << boost::format("\tts_cal_%2% = new TimesheetCalendar(table_%3%->elementAt(%1%, 1));\n")
					% counter % v_ptr->var_name% p_ptrTableInfo->tableName_;
		} else if (v_ptr->var_type==DOUBLE_TYPE || v_ptr->var_type==FLOAT_TYPE) {
			decl <<  boost::format("\tWt::Ext::NumberField * we_%1%;\n")
						% v_ptr->var_name;
			decl << format("\tWt::WDoubleValidator * wv_%1%;\n")
						% v_ptr->var_name;
			defn << boost::format("\twe_%2% = new Wt::Ext::NumberField(table_%3%->elementAt(%1%, 1));\n")
					% counter % v_ptr->var_name% p_ptrTableInfo->tableName_;
			defn << format("\twv_%1% = new Wt::WDoubleValidator();\n")
						% v_ptr->var_name;
			defn << format("\twe_%1%->setValidator(wv_%1%);\n") % v_ptr->var_name;
			if (v_ptr->options.null == false) {
				defn << format("\twv_%1%->setMandatory(true);\n") % v_ptr->var_name;
			}
		} else if(v_ptr->var_type==INT32_TYPE || v_ptr->var_type == BIGINT_TYPE) {
			decl <<  boost::format("\tWt::Ext::NumberField * we_%1%;\n")
						% v_ptr->var_name;
			decl << format("\tWt::WIntValidator * wv_%1%;\n")
						% v_ptr->var_name;
			defn << boost::format("\twe_%2% = new Wt::Ext::NumberField(table_%3%->elementAt(%1%, 1));\n")
					% counter % v_ptr->var_name% p_ptrTableInfo->tableName_;
			defn << format("\twv_%1% = new Wt::WIntValidator();\n")
						% v_ptr->var_name;
			defn << format("\twe_%1%->setValidator(wv_%1%);\n") % v_ptr->var_name;
			defn << format("\twe_%1%->setDecimalPrecision(0);\n") % v_ptr->var_name;
			if (v_ptr->options.null == false) {
				defn << format("\twv_%1%->setMandatory(true);\n") % v_ptr->var_name;
			}
		} else {
			decl <<  boost::format("\tWt::Ext::LineEdit * we_%1%;\n")
						% v_ptr->var_name;
			defn << boost::format("\twe_%2% = new Wt::Ext::LineEdit(\"\", table_%3%->elementAt(%1%, 1));\n")
					% counter % v_ptr->var_name% p_ptrTableInfo->tableName_;
			//defn << boost::format("\twta_%1%->setRows(1);\n") % v_ptr->var_name;
		}
	}
}


void WtUIGenerator::PrintLoadSummaryTableView(TableInfoType * p_ptrTableInfo, 
			std::stringstream & decl, std::stringstream & defn,
			// std::vector<std::string> & vec_handler_decls, std::vector<std::string> &vec_handler_defns,
			std::stringstream & headers
			)
{
	using boost::format;
	decl << format("\tvoid LoadSummaryTableView(std::vector<boost::shared_ptr <Biz%1%> > & page1);\n")
			% p_ptrTableInfo->tableName_;
	stringstream load_table_view_str;
	load_table_view_str << format("void %1%_ui::LoadSummaryTableView(std::vector<boost::shared_ptr <Biz%1%> > & page1)\n\{\n")
			% p_ptrTableInfo->tableName_;
	load_table_view_str << format("\ttable_%1%_view->clear();\n")
			% p_ptrTableInfo->tableName_;
	struct var_list* v_ptr = p_ptrTableInfo->param_list;
	{
		int counter =0;
		for (; v_ptr; v_ptr=v_ptr->prev) {
			// I need to fix this - like add a function which states "simple_variable" - 
			// functional programming style - as mentioned in the LISP books
			if (v_ptr->options.ui_view) {
				if (v_ptr->options.ref_table_name == "") {
					load_table_view_str << format("\ttable_%1%_view->elementAt(0, %2%)->addWidget(new Wt::WText(Wt::WString::tr(\"%3%\")));\n") %
						p_ptrTableInfo->tableName_ % counter++ % v_ptr->var_name;
				}
			}
			if (v_ptr->options.ref_table_name != "" 
					&& v_ptr->options.many == false) {
				struct CppCodeGenerator * tbl_ptr = (dynamic_cast<CppCodeGenerator *>
							(TableCollectionSingleton::Instance()
								.my_find_table(v_ptr->options.ref_table_name)));
				vector<TableInfoType *> vec_list_view_stack;
				vec_list_view_stack.push_back(tbl_ptr->tableInfo_);
				int max_recursion_level=2;
				int recursion_level = 1;
				PrintListViewHeaders(load_table_view_str, vec_list_view_stack, max_recursion_level, recursion_level, counter);
			}
		}
	}
	load_table_view_str << "\tfor (int i=0; i<page1.size(); ++i) {\n";
	v_ptr=p_ptrTableInfo->param_list;
	load_table_view_str << "\t\tstd::stringstream temp1;\n";
	{
		int counter =0; // start at row 2 - titles in row 1
		for (; v_ptr; v_ptr=v_ptr->prev) {
			if (v_ptr->options.primary_key) {
				load_table_view_str << boost::format("\t\tWt::WPushButton * b = new Wt::WPushButton(\"Select\", table_%1%_view->elementAt(i+1, %2%));\n") %
					p_ptrTableInfo->tableName_ % counter++;
				load_table_view_str << boost::format("\t\tb->clicked().connect(boost::bind(&%1%_ui::LoadForm, this, page1[i]->%2%_));\n") %
					tableInfo_->tableName_ % v_ptr->var_name;
			} else if (v_ptr->options.ui_view) {
				if (v_ptr->options.ref_table_name == "") {
					load_table_view_str << boost::format("\t\ttemp1 << page1[i]->%1%_;\n") %
						v_ptr->var_name;
					load_table_view_str << format("\t\ttable_%1%_view->elementAt(i+1, %2%)->addWidget(new Wt::WText(temp1.str()));\n") %
						p_ptrTableInfo->tableName_ % counter++;
					load_table_view_str << "\t\ttemp1.str(\"\");\n";
				}
			}
			if (v_ptr->options.ref_table_name != "" 
					&& v_ptr->options.many == false
					&& (!ReferencedTableContainsUs(tableInfo_, v_ptr->options.ref_table_name))
					&& (!v_ptr->options.session)
					) {
				struct CppCodeGenerator * tbl_ptr = (dynamic_cast<CppCodeGenerator *>
							(TableCollectionSingleton::Instance()
								.my_find_table(v_ptr->options.ref_table_name)));
				vector<TableInfoType *> vec_list_view_stack;
				vec_list_view_stack.push_back(tbl_ptr->tableInfo_);
				int max_recursion_level=2;
				int recursion_level = 1;
				PrintListViewData(load_table_view_str, vec_list_view_stack, max_recursion_level, recursion_level, counter);
			}
		}
		load_table_view_str << "\t\tif ( (i+1)%10 == 0 ) {\n";
		load_table_view_str << format("\t\t\ttable_%1%_view->rowAt(i+1)->setStyleClass(\"alt_row_0\");\n") %
			p_ptrTableInfo->tableName_ ;
		load_table_view_str << "\t\t}\n";
		
		load_table_view_str << "\t\tif ( (i+1)%10 == 1 ) {\n";
		load_table_view_str << format("\t\t\ttable_%1%_view->rowAt(i+1)->setStyleClass(\"alt_row_1\");\n") %
			p_ptrTableInfo->tableName_ ;
		load_table_view_str << "\t\t}\n";
		
		load_table_view_str << "\t\tif ( (i+1)%10 == 2 ) {\n";
		load_table_view_str << format("\t\t\ttable_%1%_view->rowAt(i+1)->setStyleClass(\"alt_row_2\");\n") %
			p_ptrTableInfo->tableName_ ;
		load_table_view_str << "\t\t}\n";
		
		load_table_view_str << "\t\tif ( (i+1)%10 == 3 ) {\n";
		load_table_view_str << format("\t\t\ttable_%1%_view->rowAt(i+1)->setStyleClass(\"alt_row_3\");\n") %
			p_ptrTableInfo->tableName_ ;
		load_table_view_str << "\t\t}\n";
		
		load_table_view_str << "\t\tif ( (i+1)%10 == 4 ) {\n";
		load_table_view_str << format("\t\t\ttable_%1%_view->rowAt(i+1)->setStyleClass(\"alt_row_4\");\n") %
			p_ptrTableInfo->tableName_ ;
		load_table_view_str << "\t\t}\n";
		
		load_table_view_str << "\t\tif ( (i+1)%10 == 5 ) {\n";
		load_table_view_str << format("\t\t\ttable_%1%_view->rowAt(i+1)->setStyleClass(\"alt_row_5\");\n") %
			p_ptrTableInfo->tableName_ ;
		load_table_view_str << "\t\t}\n";
		
		load_table_view_str << "\t\tif ( (i+1)%10 == 6 ) {\n";
		load_table_view_str << format("\t\t\ttable_%1%_view->rowAt(i+1)->setStyleClass(\"alt_row_6\");\n") %
			p_ptrTableInfo->tableName_ ;
		load_table_view_str << "\t\t}\n";
		
		load_table_view_str << "\t\tif ( (i+1)%10 == 7 ) {\n";
		load_table_view_str << format("\t\t\ttable_%1%_view->rowAt(i+1)->setStyleClass(\"alt_row_7\");\n") %
			p_ptrTableInfo->tableName_ ;
		load_table_view_str << "\t\t}\n";
		
		load_table_view_str << "\t\tif ( (i+1)%10 == 8 ) {\n";
		load_table_view_str << format("\t\t\ttable_%1%_view->rowAt(i+1)->setStyleClass(\"alt_row_8\");\n") %
			p_ptrTableInfo->tableName_ ;
		load_table_view_str << "\t\t}\n";
		
		load_table_view_str << "\t\tif ( (i+1)%10 == 9 ) {\n";
		load_table_view_str << format("\t\t\ttable_%1%_view->rowAt(i+1)->setStyleClass(\"alt_row_9\");\n") %
			p_ptrTableInfo->tableName_ ;
		load_table_view_str << "\t\t}\n";
	}
	load_table_view_str << "\t}\n";
	load_table_view_str << format("\ttable_%1%_view->setHeaderCount(1);\n") %
					p_ptrTableInfo->tableName_ ;
	load_table_view_str << "}\n\n";
	defn << load_table_view_str.str();
}

void WtUIGenerator::PrintLoginWidget()
{
	using boost::format;
	stringstream login_widget_h_str, login_widget_h_str2;
	string tn(tableInfo_->tableName_ );
	login_widget_h_str << "#ifndef " << tn << "_h\n"
		<< "#define " << tn << "_h\n\n";
	
	login_widget_h_str << "#include <Wt/WContainerWidget>\n";
	login_widget_h_str << "#include <Wt/WText>\n";
	login_widget_h_str << "#include <Wt/WLineEdit>\n\n";
	login_widget_h_str << format("class %1%_Widget : public Wt::WContainerWidget\n") %
				tn;	
	login_widget_h_str << format("{\npublic:\n\t%1%_Widget(Wt::WContainerWidget *parent=0);\n") %
				tn;
	login_widget_h_str << format("\tWt::Signal<std::wstring> loginSuccessful;\n");
	login_widget_h_str << "private:\n";

	login_widget_h_str<< "\tWt::WText     *IntroText;\n";
	struct var_list* v_ptr = tableInfo_->param_list;
	while (v_ptr) {
		if (v_ptr->options.is_login_username_field) {
			login_widget_h_str << format("\tWt::WLineEdit * %1%;\n") %
					v_ptr->var_name;
			login_widget_h_str2 << format("\tstd::wstring userName_;\n") ;
		} else if (v_ptr->options.is_login_password_field) {
			login_widget_h_str << format("\tWt::WLineEdit * %1%;\n") %
					v_ptr->var_name;
		}
		v_ptr=v_ptr->prev;
	}
	login_widget_h_str << login_widget_h_str2.str();


	login_widget_h_str << "\tvoid confirmLogin(const std::wstring text);\n";
	login_widget_h_str << "\tvoid checkCredentials();\n";
	login_widget_h_str << "};\n";


	login_widget_h_str << "#endif /* " << tableInfo_->tableName_ << " */\n";

	string login_widget_fname_h (string(outputDirPrefix_.c_str()
				+ string("/")
				+ tn
				+ string("_Widget.h"))); 
	std::ofstream login_widget_h(login_widget_fname_h.c_str(), ios_base::out|ios_base::trunc);
	login_widget_h << login_widget_h_str.str();


	stringstream login_widget_cpp_str;


	login_widget_cpp_str << "#include <Wt/WText>\n";
	login_widget_cpp_str << "#include <Wt/WLabel>\n";
	login_widget_cpp_str << "#include <Wt/WLineEdit>\n";
	login_widget_cpp_str << "#include <Wt/WBreak>\n";
	login_widget_cpp_str << "#include <Wt/WCssDecorationStyle>\n";
	login_widget_cpp_str << "#include <Wt/WPushButton>\n";
	login_widget_cpp_str << "#include <Wt/WTable>\n";
	login_widget_cpp_str << "#include <Wt/WTableCell>\n";
	login_widget_cpp_str << "#include <Wt/WComboBox>\n";
	login_widget_cpp_str << "\n";
	login_widget_cpp_str << format("#include \"%1%_Widget.h\"\n") % tableInfo_->tableName_;
	login_widget_cpp_str << "\n";
	login_widget_cpp_str << format("%1%_Widget::%1%_Widget(WContainerWidget *parent)\n") % tableInfo_->tableName_;
	login_widget_cpp_str << "	: Wt::WContainerWidget(parent)\n";
	login_widget_cpp_str << "{\n";
	login_widget_cpp_str << "	setPadding(100, Wt::Left | Wt::Right);\n";
	login_widget_cpp_str << "\n";
	login_widget_cpp_str << "	Wt::WText *title = new Wt::WText(\"%1%\", this);\n";
	login_widget_cpp_str << "	title->decorationStyle().font().setSize(Wt::WFont::XLarge);\n";
	login_widget_cpp_str << "\n";
	login_widget_cpp_str << "	IntroText =\n";
	login_widget_cpp_str << "	new Wt::WText(\"<p>Enter Login Details</p>\", this);\n";
	login_widget_cpp_str << "\n";
	login_widget_cpp_str << "	Wt::WTable *layout = new Wt::WTable(this);\n";

	v_ptr = tableInfo_->param_list;
	while (v_ptr) {
		if (v_ptr->options.is_login_username_field) {
			// login_widget_h_str << format("\tWt::WLineEdit * %1%;\n") %
			// 		v_ptr->var_name;
			// login_widget_h_str2 << format("\tstd::wstring %1%;\n") %
			// 		v_ptr->var_name;

			login_widget_cpp_str << "	Wt::WLabel *usernameLabel = new Wt::WLabel(\"User name: \", layout->elementAt(0, 0));\n";
			login_widget_cpp_str << "	layout->elementAt(0, 0)->resize(Wt::WLength(14, Wt::WLength::FontEx), Wt::WLength::Auto);\n";
			login_widget_cpp_str << format("	%1% = new Wt::WLineEdit(layout->elementAt(0, 1));\n")
							% v_ptr->var_name;
			login_widget_cpp_str << format("	usernameLabel->setBuddy(%1%);\n")
							% v_ptr->var_name;
			login_widget_cpp_str << "\n";
		} else if (v_ptr->options.is_login_password_field) {
			// login_widget_h_str << format("\tWt::WLineEdit * %1%;\n") %
			// 		v_ptr->var_name;

			login_widget_cpp_str << "	Wt::WLabel *passwordLabel = new Wt::WLabel(\"Password: \", layout->elementAt(1, 0));\n";
			login_widget_cpp_str << format("	%1% = new Wt::WLineEdit(layout->elementAt(1, 1));\n")
							% v_ptr->var_name;
			login_widget_cpp_str << format("	%1%->setEchoMode(Wt::WLineEdit::Password);\n")
							% v_ptr->var_name ;
			login_widget_cpp_str << format("	passwordLabel->setBuddy(%1%);\n") 
							% v_ptr->var_name;
			login_widget_cpp_str << "\n";
		}
		v_ptr=v_ptr->prev;
	}
	login_widget_h_str << login_widget_h_str2.str();



	login_widget_cpp_str << "	new Wt::WBreak(this);\n";
	login_widget_cpp_str << "\n";
	login_widget_cpp_str << format("	Wt::WPushButton *%1%Button = new Wt::WPushButton(\"%1%\", this);\n")
						% tn;
	login_widget_cpp_str << format("	%1%Button->clicked().connect(this, &%1%_Widget::checkCredentials);\n")
				% tn;
	login_widget_cpp_str << "}\n";
	login_widget_cpp_str << "\n";
	login_widget_cpp_str << format("void %1%_Widget::checkCredentials()\n") % tn;
	login_widget_cpp_str << "{\n";
	
	v_ptr = tableInfo_->param_list;
	while (v_ptr) {
		if (v_ptr->options.is_login_username_field) {
			// login_widget_cpp_str << "	userName_ = Username->text();\n";
			login_widget_cpp_str << format("\tuserName_ =  %1%->text();\n") %
					v_ptr->var_name;
		} else if (v_ptr->options.is_login_password_field) {
			// login_widget_cpp_str << "	std::wstring pass = Password->text();\n";
			login_widget_cpp_str << format("\tstd::wstring pass = %1%->text();\n") %
					v_ptr->var_name;
		}
		v_ptr=v_ptr->prev;
	}
	login_widget_cpp_str << "\n";
	login_widget_cpp_str << "	if (userName_ == L\"nxd\") {\n";
	login_widget_cpp_str << "		confirmLogin(L\"<p>Welcome, \" + userName_ + L\"</p>\");\n";
	login_widget_cpp_str << "	} else if (userName_ == L\"guest\" && pass == L\"guest\") {\n";
	login_widget_cpp_str << "		confirmLogin(L\"<p>Welcome guest.</p>\");\n";
	login_widget_cpp_str << "	} else {\n";
	login_widget_cpp_str << "		IntroText\n";
	login_widget_cpp_str << "		->setText(\"<p>You entered the wrong password, or the username \"\n";
	login_widget_cpp_str << "			  \"combination is already in use. If you are a returning \"\n";
	login_widget_cpp_str << "			  \"user, please try again. If you are a new user, please \"\n";
	login_widget_cpp_str << "			  \"try a different name.</p>\");\n";
	login_widget_cpp_str << "		IntroText->decorationStyle().setForegroundColor(Wt::red);\n";
	v_ptr = tableInfo_->param_list;
	while (v_ptr) {
		if (v_ptr->options.is_login_username_field) {
			// login_widget_cpp_str << "	userName_ = Username->text();\n";
			login_widget_cpp_str << format("\t\t%1%->setText(\"\");\n") %
					v_ptr->var_name;
		} else if (v_ptr->options.is_login_password_field) {
			// login_widget_cpp_str << "	std::wstring pass = Password->text();\n";
			login_widget_cpp_str << format("\t\t%1%->setText(\"\");\n") %
					v_ptr->var_name;
		}
		v_ptr=v_ptr->prev;
	}


	login_widget_cpp_str << "\n";
	login_widget_cpp_str << "	}\n";

	login_widget_cpp_str << "\n";
	login_widget_cpp_str << "}\n";
	login_widget_cpp_str << "\n";
	login_widget_cpp_str << format("void %1%_Widget::confirmLogin(const std::wstring text)\n") % tn;
	login_widget_cpp_str << "{\n";
	login_widget_cpp_str << "	clear();\n";
	login_widget_cpp_str << "	loginSuccessful.emit(userName_);\n";
	login_widget_cpp_str << "}\n\n";
	login_widget_cpp_str << "\n";
	//login_widget_cpp_str << format("void %1%_Widget::startPlaying()\n") % tn; 
	//login_widget_cpp_str << "{\n";
	//login_widget_cpp_str << "	loginSuccessful.emit(userName_);\n";
	//login_widget_cpp_str << "}\n";
	login_widget_cpp_str << "\n";
	login_widget_cpp_str << "\n";


	string login_widget_fname_cpp (string(outputDirPrefix_.c_str()
				+ string("/")
				+ tn
				+ string("_Widget.cpp"))); 
	std::ofstream login_widget_cpp(login_widget_fname_cpp.c_str(), ios_base::out|ios_base::trunc);
	login_widget_cpp << login_widget_cpp_str.str();
}
