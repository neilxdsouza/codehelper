#include "std_headers.h"
#include "WtUIGenerator.h"

#include "std_using.h"
#include "utils.h"

using namespace std;


std::stringstream WtUIGenerator::class_vars;
std::stringstream WtUIGenerator::class_functions_decl;
std::stringstream WtUIGenerator::class_function_impl;
std::stringstream WtUIGenerator::navigation_nodes;
std::stringstream WtUIGenerator::header_files;
std::stringstream WtUIGenerator::makefile_objs;
extern vector <TableInfoType *> vec_table_info;
extern char project_namespace[];

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
	cout << format("ENTER: FILE: %1%, LINE: %2% FUNCTION:%3%\n") % __FILE__ % __LINE__ 
		% __PRETTY_FUNCTION__;
	//ui << GenerateUIScaffolding();
	GenerateForms();
	makefile_objs << boost::format("%1%_ui.o %1%_db_postgres.o ") % tableInfo_->tableName_;
	
	cout << format("EXIT: %1% %2% %3%\n") % __FILE__ % __LINE__ 
		% __PRETTY_FUNCTION__;
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
	uiScaffolding << "	setTitle(\"Wt-Ext, including a kitchen sink\");\n";
	uiScaffolding << "	setLoadingIndicator(new WOverlayLoadingIndicator());\n";
	uiScaffolding << "\n";
	uiScaffolding << "	useStyleSheet(\"good1.css\");\n";
	uiScaffolding << "	messageResourceBundle().use(appRoot() + \"good1\");\n";
	uiScaffolding << "	Ext::Container * viewPort = new Ext::Container(root());\n";
	uiScaffolding << "	WBorderLayout *layout = new WBorderLayout(viewPort);\n";
	uiScaffolding << "\n";
	uiScaffolding << "	/* North */\n";
	uiScaffolding << "	Ext::Panel *north = new Ext::Panel();\n";
	uiScaffolding << "	north->setBorder(false);\n";
	uiScaffolding << "	WText *head = new WText(Wt::WString::tr(\"header\"));\n";
	uiScaffolding << "	head->setStyleClass(\"north\");\n";
	uiScaffolding << "	north->setLayout(new WFitLayout());\n";
	uiScaffolding << "	north->layout()->addWidget(head);\n";
	uiScaffolding << "	north->resize(WLength::Auto, 35);\n";
	uiScaffolding << "	layout->addWidget(north, WBorderLayout::North);\n";
	uiScaffolding << "\n";
	uiScaffolding << "	/* West */\n";
	uiScaffolding << "	Ext::Panel *west = new Ext::Panel();\n";
	uiScaffolding << "	west->layout()->addWidget(createNavigationTree());\n";
	uiScaffolding << "\n";
	uiScaffolding << "	west->setTitle(\"Widgets\");\n";
	uiScaffolding << "	west->resize(200, WLength::Auto);\n";
	uiScaffolding << "	west->setResizable(true);\n";
	uiScaffolding << "	west->setCollapsible(true);\n";
	uiScaffolding << "	west->setAnimate(true);\n";
	uiScaffolding << "	west->setAutoScrollBars(true);\n";
	uiScaffolding << "	layout->addWidget(west, WBorderLayout::West);\n";
	uiScaffolding << "\n";
	uiScaffolding << "	/* Center */\n";
	uiScaffolding << "	Ext::Panel *center = new Ext::Panel();\n";
	uiScaffolding << "	center->setTitle(\"Demo widget\");\n";
	uiScaffolding << "	center->layout()->addWidget(formContainer_ = new WContainerWidget());\n";
	uiScaffolding << "	center->setAutoScrollBars(true);\n";
	uiScaffolding << "	layout->addWidget(center, WBorderLayout::Center);\n";
	uiScaffolding << "\n";
	uiScaffolding << "	formContainer_->setPadding(5);\n";
	uiScaffolding << "\n";
	uiScaffolding << "	WContainerWidget *container = new WContainerWidget(formContainer_);\n";
	uiScaffolding << "	container->addWidget(new WText(Wt::WString::tr(\"about\")));\n";
	uiScaffolding << "	currentForm_ = container;\n";
	uiScaffolding << "\n";
	uiScaffolding << "}\n";
	uiScaffolding << "\n";

	uiScaffolding << PrintNavigationDecl();

	uiScaffolding << "\n";
	uiScaffolding << "void good1::formWidgetsExample()\n";
	uiScaffolding << "{\n";
	uiScaffolding << "	WContainerWidget *ex = new WContainerWidget();\n";
	uiScaffolding << "\n";
	uiScaffolding << "	WText *wt = new WText( Wt::WString::tr(\"ex-form-widgets\"), ex);\n";
	uiScaffolding << "	wt->setMargin(5, Bottom);\n";
	uiScaffolding << "\n";
	uiScaffolding << "	WTable *table = new WTable(ex);\n";
	uiScaffolding << "\n";
	uiScaffolding << "	// ComboBox\n";
	uiScaffolding << "	cb = new Ext::ComboBox(table->elementAt(0, 0));\n";
	uiScaffolding << "	cb->addItem(\"One\");\n";
	uiScaffolding << "	cb->addItem(\"Two\");\n";
	uiScaffolding << "	cb->addItem(\"Three\");\n";
	uiScaffolding << "	cb->setFocus();\n";
	uiScaffolding << "\n";
	uiScaffolding << "	/*\n";
	uiScaffolding << "	This is how you would keep the data on the server (for really big\n";
	uiScaffolding << "	data models:\n";
	uiScaffolding << "\n";
	uiScaffolding << "	cb->setDataLocation(Ext::ServerSide);\n";
	uiScaffolding << "	cb->setMinQueryLength(0);\n";
	uiScaffolding << "	cb->setQueryDelay(0);\n";
	uiScaffolding << "	cb->setPageSize(10);\n";
	uiScaffolding << "	cb->setTextSize(20);\n";
	uiScaffolding << "	*/\n";
	uiScaffolding << "\n";
	uiScaffolding << "	// Button\n";
	uiScaffolding << "	Ext::Button *button = new Ext::Button(\"Modify\", table->elementAt(0, 1));\n";
	uiScaffolding << "	button->setMargin(5, Left);\n";
	uiScaffolding << "	button->activated().connect(this, &good1::formModify);\n";
	uiScaffolding << "\n";
	uiScaffolding << "	// CheckBox\n";
	uiScaffolding << "	Ext::CheckBox *cb1 = new Ext::CheckBox(\"Check 1\", table->elementAt(1, 0));\n";
	uiScaffolding << "	Ext::CheckBox *cb2 = new Ext::CheckBox(\"Check 2\", table->elementAt(2, 0));\n";
	uiScaffolding << "	cb2->setChecked();\n";
	uiScaffolding << "\n";
	uiScaffolding << "	/*\n";
	uiScaffolding << "	-- test setHideWithOffsets() of Ext::ComboBox\n";
	uiScaffolding << "	table->hide();\n";
	uiScaffolding << "	WPushButton *b = new WPushButton(\"show\", ex);\n";
	uiScaffolding << "	b->clicked().connect(table, &WWidget::show);\n";
	uiScaffolding << "	*/\n";
	uiScaffolding << "\n";
	uiScaffolding << "	// DateField\n";
	uiScaffolding << "	WContainerWidget *w = new WContainerWidget(ex);\n";
	uiScaffolding << "	w->setMargin(5, Top | Bottom);\n";
	uiScaffolding << "	Ext::DateField *df = new Ext::DateField(w);\n";
	uiScaffolding << "	df->setDate(WDate(2007, 9, 7));\n";
	uiScaffolding << "\n";
	uiScaffolding << "	// Calendar\n";
	uiScaffolding << "	Ext::Calendar *dp = new Ext::Calendar(false, ex);\n";
	uiScaffolding << "\n";
	uiScaffolding << "	// TextEdit\n";
	uiScaffolding << "	html_ = new Ext::TextEdit(\"Hello there, <b>brothers and sisters</b>\", ex);\n";
	uiScaffolding << "	html_->setMargin(5, Top | Bottom);\n";
	uiScaffolding << "	html_->resize(600, 300);\n";
	uiScaffolding << "\n";
	uiScaffolding << "	// Horizontal Splitter\n";
	uiScaffolding << "	Ext::Splitter *split = new Ext::Splitter(ex);\n";
	uiScaffolding << "	split->resize(400, 100);\n";
	uiScaffolding << "\n";
	uiScaffolding << "	split->addWidget(new WText(\"Left\"));\n";
	uiScaffolding << "	split->children().back()->resize(150, WLength::Auto);\n";
	uiScaffolding << "	split->children().back()->setMinimumSize(130, WLength::Auto);\n";
	uiScaffolding << "	split->children().back()->setMaximumSize(170, WLength::Auto);\n";
	uiScaffolding << "\n";
	uiScaffolding << "	split->addWidget(new WText(\"Center\"));\n";
	uiScaffolding << "	split->children().back()->resize(100, WLength::Auto);\n";
	uiScaffolding << "	split->children().back()->setMinimumSize(50, WLength::Auto);\n";
	uiScaffolding << "\n";
	uiScaffolding << "	split->addWidget(new WText(\"Right\"));\n";
	uiScaffolding << "	split->children().back()->resize(50, WLength::Auto);\n";
	uiScaffolding << "	split->children().back()->setMinimumSize(50, WLength::Auto);\n";
	uiScaffolding << "\n";
	uiScaffolding << "	// Vertical Splitter\n";
	uiScaffolding << "	split = new Ext::Splitter(Vertical, ex);\n";
	uiScaffolding << "	split->resize(100, 200);\n";
	uiScaffolding << "\n";
	uiScaffolding << "	split->addWidget(new WText(\"Top\"));\n";
	uiScaffolding << "	split->children().back()->resize(WLength::Auto, 100);\n";
	uiScaffolding << "	split->children().back()->setMinimumSize(WLength::Auto, 50);\n";
	uiScaffolding << "	split->children().back()->setMaximumSize(WLength::Auto, 196);\n";
	uiScaffolding << "\n";
	uiScaffolding << "	split->addWidget(new WText(\"Center\"));\n";
	uiScaffolding << "	split->children().back()->resize(WLength::Auto, 100);\n";
	uiScaffolding << "\n";
	uiScaffolding << "	setCentralWidget(ex);\n";
	uiScaffolding << "}\n";
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
	class_decl << "	void formWidgetsExample();\n";
	class_decl << "	WTreeNode *createNavigationNode(const Wt::WString& label,\n";
	class_decl << "					    WTreeNode *parentNode,\n";
	class_decl << "					    ShowCentralWidget f);\n";
	class_decl << "	void formModify();\n";
	class_decl << class_functions_decl.str();

	class_decl << "	Wt::Ext::ComboBox *cb;\n";
	class_decl << "	Wt::Ext::TextEdit *html_;\n";
	class_decl << "	Wt::WWidget                       *currentForm_;\n";
	class_decl << "	Wt::WContainerWidget              *formContainer_;\n";

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
	navigation_tree_func << "	createNavigationNode(\"Form widgets\", rootNode,\n";
	navigation_tree_func << "			    &good1::formWidgetsExample);\n";
	navigation_tree_func << navigation_nodes.str();
	navigation_tree_func << "\n";
	navigation_tree_func << "	rootNode->setMargin(5);\n";
	navigation_tree_func << "\n";
	navigation_tree_func << "	return rootNode;\n";
	navigation_tree_func << "}\n";
	return navigation_tree_func.str();
}

void WtUIGenerator::AddNavigationNode(std::string  label, std::string  func_name)
{
	//navigation_nodes << "// Hello,World\n";
	navigation_nodes << "\tcreateNavigationNode(\""
	 		<< label << "\", rootNode,\n"
	 		<< "\t\t\t&good1::" << func_name << ");\n";
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
	ui_class_headers << "\n";
	ui_class_headers << "#include <Wt/Ext/Button>\n";
	ui_class_headers << "#include <Wt/Ext/Calendar>\n";
	ui_class_headers << "#include <Wt/Ext/CheckBox>\n";
	ui_class_headers << "#include <Wt/Ext/ComboBox>\n";
	ui_class_headers << "#include <Wt/Ext/Container>\n";
	ui_class_headers << "#include <Wt/Ext/DateField>\n";
	ui_class_headers << "#include <Wt/Ext/Dialog>\n";
	ui_class_headers << "#include <Wt/Ext/Menu>\n";
	ui_class_headers << "#include <Wt/Ext/MessageBox>\n";
	ui_class_headers << "#include <Wt/Ext/ProgressDialog>\n";
	ui_class_headers << "#include <Wt/Ext/Splitter>\n";
	ui_class_headers << "#include <Wt/Ext/TabWidget>\n";
	ui_class_headers << "#include <Wt/Ext/TableView>\n";
	ui_class_headers << "#include <Wt/Ext/TextEdit>\n";
	ui_class_headers << "#include <Wt/Ext/ToolBar>\n";
	ui_class_headers << "\n";
	ui_class_headers << "#include <iostream>\n";
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
	ui_class_decl << "\tWt::WText *title;\n";
	ui_class_defn << "\ttitle = new Wt::WText( Wt::WString::tr(\""
		<< tableInfo_->tableName_ << "\"), this);\n";
	ui_class_defn << "\ttw = new Wt::Ext::TabWidget(this);\n";
	//ui_class_defn << "\tpanel->setLayout(new Wt::WFitLayout());\n";
	ui_class_defn << "\ttitle->setMargin(5, Wt::Bottom);\n";
	//ui_class_defn << "\tpanel->layout()->addWidget();\n";
	struct var_list* v_ptr=tableInfo_->param_list;
	if( v_ptr == 0){
		form_code << "// v_ptr== NULL\n";
	}
	//int counter=0;

	vector <TableInfoType *> vecTableInfo;
	vecTableInfo.push_back(tableInfo_);
	GenerateUITab(ui_class_decl, ui_class_defn, false, vecTableInfo);
	
	/*
	for (; v_ptr; v_ptr=v_ptr->prev, ++counter) {
		if(v_ptr->var_type==COMPOSITE_TYPE)
			continue;
		ui_class_decl <<  boost::format("\tWt::WLabel * wt_%1%;\n")
					% v_ptr->var_name;
		form_code << boost::format("\tWt::WLabel * wt_%2% = new Wt::WLabel(Wt::WString::tr(\"%2%\"),\n" 
					"\t\t\ttable->elementAt(%1%, 0));\n")
				% counter % v_ptr->var_name;
		ui_class_defn << 
			boost::format("\twt_%2% = new Wt::WLabel(Wt::WString::tr(\"%2%\"),\n" 
					"\t\t\ttable->elementAt(%1%, 0));\n")
					% counter % v_ptr->var_name;
		if (v_ptr->var_type==DATETIME_TYPE) {
			form_code << boost::format("\tWt::Ext::DateField * edf_%2% = new Wt::Ext::DateField(table->elementAt(%1%, 1));\n")
					% counter % v_ptr->var_name;
			ui_class_decl <<  boost::format("\tWt::Ext::DateField * edf_%1%;\n")
						% v_ptr->var_name;
			ui_class_defn << boost::format("\tedf_%2% = new Wt::Ext::DateField(table->elementAt(%1%, 1));\n")
					% counter % v_ptr->var_name;
		} else {
			form_code << boost::format("\tWt::WTextArea * wta_%2% = new Wt::WTextArea(\"\", table->elementAt(%1%, 1));\n")
					% counter % v_ptr->var_name;
			form_code << boost::format("\twta_%1%->setRows(1);\n")
					% v_ptr->var_name;
			ui_class_decl <<  boost::format("\tWt::WTextArea * wta_%1%;\n")
						% v_ptr->var_name;
			ui_class_defn << boost::format("\twta_%2% = new Wt::WTextArea(\"\", table->elementAt(%1%, 1));\n")
					% counter % v_ptr->var_name;
			ui_class_defn << boost::format("\twta_%1%->setRows(1);\n")
					% v_ptr->var_name;
		}
	}
	form_code << boost::format("\tWt::WPushButton * wpb_insert = new Wt::WPushButton(table->elementAt(%1%, 0));\n")
			% counter;
	form_code << boost::format("\twpb_insert->clicked().connect(wpb_insert, &Wt::WPushButton::disable);\n");
	form_code << boost::format("\twpb_insert->clicked().connect(wpb_insert, &good1::ProcessInsert%1%);\n")
					% tableInfo_->tableName_;
	form_code << "\t*" << "/\n";
	*/

	ui_class_decl << boost::format("\tvoid ProcessInsert%1%();\n")
					% tableInfo_->tableName_;
	ui_class_defn << boost::format("void %1%_ui::ProcessInsert%1%()\n{\n")
					% tableInfo_->tableName_;
	ui_class_defn << "}\n";
	// I should use some form of assert to check 
	// that vec_handler_decls.size == vec_handler_defns.size
	for(int i=0; i<vec_handler_decls.size(); ++i) {
		ui_class_decl << endl << vec_handler_decls[i] << endl;
	}
	for(int i=0; i<vec_handler_defns.size(); ++i) {
		ui_class_defn << endl << vec_handler_defns[i] << endl;
	}
	
	stringstream func_name;
	func_name << boost::format("formInsert%1%")
					% tableInfo_->tableName_;
	AddNavigationNode(tableInfo_->tableName_, func_name.str());
	
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
	ui_cpp << boost::format("#include <boost/shared_ptr.hpp>\n\n");
	ui_cpp << ui_class_defn.str();


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

void WtUIGenerator::GenerateUITab(std::stringstream & decl,
				std::stringstream & defn, 
				bool called_recursively,
				vector<TableInfoType *> p_vecTableInfo)
{
	TableInfoType * aTableInfo = p_vecTableInfo.back();
	p_vecTableInfo.pop_back();
	struct var_list* v_ptr=aTableInfo->param_list;
	int counter=0;
	decl << boost::format("\tWt::WContainerWidget  *wcw_%1%;\n")
			% aTableInfo->tableName_;
	defn << boost::format("\twcw_%1% = new Wt::WContainerWidget();\n")
			% aTableInfo->tableName_;
	decl << boost::format("\tWt::WTable *table_%1%;\n")
			% aTableInfo->tableName_;
	decl << boost::format("\tWt::WTable *table_%1%_view;\n")
			% aTableInfo->tableName_;
	defn << boost::format("\ttable_%1% = new Wt::WTable(wcw_%1%);\n")
			% aTableInfo->tableName_;

	defn << boost::format("\ttable_%1%_view = new Wt::WTable(wcw_%1%);\n")
			% aTableInfo->tableName_;
	for (; v_ptr; v_ptr=v_ptr->prev, ++counter) {
		if (v_ptr->options.ref_table_name!="") {
			cout << " called_recursively: " << called_recursively
				<< endl;
			cout << " my table name: " << aTableInfo->tableName_
				<< endl;
			cout << " ref_table_name: " << v_ptr->options.ref_table_name
				<< endl;
			cout << "ReferencedTableContainsUs: " 
				<< ReferencedTableContainsUs(aTableInfo, v_ptr->options.ref_table_name)
				<< endl;
		}
		//! called_recursively == true means this form is being generated in the context of 
		//! the original table that contains this one as a composite object
		//! for such tables the foreign key is being supplied by the master table
		//! for example in the case of "employee, employeestatus" in which 
		//! employeestatus is also contained in employee as separate composite object
		//! when generating the employeestatus tab - we dont want to display the employee 
		//! code again in employeestatus - it is already supplied in the employee tab
		if (called_recursively && v_ptr->options.ref_table_name!=""
				&& ReferencedTableContainsUs(aTableInfo, v_ptr->options.ref_table_name) ) {
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
					% counter % v_ptr->var_name % aTableInfo->tableName_;

		if (v_ptr->options.ref_table_name!="" 
				&& v_ptr->var_type != COMPOSITE_TYPE
				 ) {
			decl <<  boost::format("\tWt::WLabel * wt_%1%_value;\n")
						% v_ptr->var_name;
			decl <<  boost::format("\tWt::WPushButton * wpb_choose_%1%;\n")
						% v_ptr->var_name;
			defn << 
				boost::format("\twt_%2%_value = new Wt::WLabel(Wt::WString::tr(\"%2%\"),\n" 
						"\t\t\ttable_%3%->elementAt(%1%, 1));\n")
						% counter % v_ptr->var_name % aTableInfo->tableName_;
			defn << 
				boost::format("\twpb_choose_%2%= new Wt::WPushButton(Wt::WString::tr(\"Choose %2%\"),\n" 
						"\t\t\ttable_%3%->elementAt(%1%, 2));\n")
						% counter % v_ptr->var_name % aTableInfo->tableName_;
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
			handle_func_defn << print_ChoiceHandler(v_ptr);
			vec_handler_defns.push_back(handle_func_defn.str());
			
		} else if (v_ptr->var_type==DATETIME_TYPE) {
			decl <<  boost::format("\tWt::Ext::DateField * edf_%1%;\n")
						% v_ptr->var_name;
			defn << boost::format("\tedf_%2% = new Wt::Ext::DateField(table_%3%->elementAt(%1%, 1));\n")
					% counter % v_ptr->var_name% aTableInfo->tableName_;
		} else {
			decl <<  boost::format("\tWt::WTextArea * wta_%1%;\n")
						% v_ptr->var_name;
			defn << boost::format("\twta_%2% = new Wt::WTextArea(\"\", table_%3%->elementAt(%1%, 1));\n")
					% counter % v_ptr->var_name% aTableInfo->tableName_;
			defn << boost::format("\twta_%1%->setRows(1);\n")
					% v_ptr->var_name;
		}
	}

	if (called_recursively==false) {
		defn << boost::format("\tstd::vector<boost::shared_ptr <Biz%2%> > page1 = %1%::db::%2%::Get%2%(0, 10") %
			project_namespace % aTableInfo->tableName_;

		string search_key_args_str =  print_cpp_search_key_args() ;
		if (search_key_args_str != "") {
			defn << ",\n";
			defn << search_key_args_str;
		}
		defn << ");\n";
	}

	defn << boost::format("\ttw->addTab(wcw_%1%, \"%1%\");\n")
				% aTableInfo->tableName_;

	if (p_vecTableInfo.size()>0) {
		GenerateUITab(decl, defn, true, p_vecTableInfo);
	}
	if (called_recursively==false) {
		decl << boost::format("\tWt::WPushButton * wpb_insert;\n");
		defn << boost::format("\twpb_insert = new Wt::WPushButton(Wt::WString::tr(\"Add\"), table_%3%->elementAt(%1%, 0));\n")
				% counter% aTableInfo->tableName_% aTableInfo->tableName_;
		defn << "\twpb_insert->setText(Wt::WString::tr(\"Add\"));\n";
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

extern std::string input_file_name;

void WtUIGenerator::GenerateMakefile()
{
	std::stringstream makefile_str;
	makefile_str << "CXX := $(CXX) -g " << endl;
	makefile_str << "OBJS = "
		<< makefile_objs.str() << endl;
	makefile_str << "LINK_LIBS = -lwt -lwtext -lwthttp"
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
}


string WtUIGenerator::print_ChoiceHandler(struct var_list * p_vptr)
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
	func_defn << boost::format("\twd_choose_%1%->exec();\n")
		% p_vptr->var_name;

	func_defn << "}\n";
	return func_defn.str();
}


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
