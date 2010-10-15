#include "std_headers.h"
#include "WtUIGenerator.h"

#include "std_using.h"

using namespace std;


std::stringstream WtUIGenerator::class_vars;
std::stringstream WtUIGenerator::class_functions_decl;
std::stringstream WtUIGenerator::class_function_impl;
std::stringstream WtUIGenerator::navigation_nodes;
std::stringstream WtUIGenerator::header_files;

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
	uiScaffolding << "	WText *head = new WText(WString::tr(\"header\"));\n";
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
	uiScaffolding << "	container->addWidget(new WText(WString::tr(\"about\")));\n";
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
	uiScaffolding << "	WText *wt = new WText( WString::tr(\"ex-form-widgets\"), ex);\n";
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
	uiScaffolding << "WTreeNode *good1::createNavigationNode(const WString& label,\n";
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
	class_decl << "	WTreeNode *createNavigationNode(const WString& label,\n";
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

	ui_class_decl << boost::format("class %1%_ui : public WContainerWidget\n{\n")
				% tableInfo_->tableName_;


	stringstream form_code;
	stringstream func_decl_signature, func_defn_signature;
	func_decl_signature << boost::format("void formInsert%1%()")
			% tableInfo_->tableName_;
	func_defn_signature << boost::format("void good1::formInsert%1%()")
			% tableInfo_->tableName_;
	form_code << func_defn_signature.str() << "\n{\n";
	class_functions_decl << "\t" << func_decl_signature.str() << ";\n";
	
	form_code << "\tWt::WContainerWidget *canvas = new WContainerWidget();\n";
	ui_class_decl << "\tWt::WContainerWidget *canvas;\n";
	form_code << "\tWt::WText *title = new WText( WString::tr(\""
		<< tableInfo_->tableName_ << "\"), canvas);\n";
	ui_class_decl << "\tWt::WText *title;\n";
	
	form_code << "\ttitle->setMargin(5, Bottom);\n";
	form_code << "\tWt::WTable *table = new WTable(canvas);\n";
	struct var_list* v_ptr=tableInfo_->param_list;
	if( v_ptr == 0){
		form_code << "// v_ptr== NULL\n";
	}
	int counter=0;
	for (; v_ptr; v_ptr=v_ptr->prev, ++counter) {
		if(v_ptr->var_type==COMPOSITE_TYPE)
			continue;
		ui_class_decl <<  boost::format("\tWt::WLabel * wt_%1%;\n")
					% v_ptr->var_name;
		form_code << boost::format("\tWt::WLabel * wt_%2% = new Wt::WLabel(WString::tr(\"%2%\"),\n" 
				"\t\ttable->elementAt(%1%, 0));\n")
				% counter % v_ptr->var_name;
		if (v_ptr->var_type==DATETIME_TYPE) {
			form_code << boost::format("\tWt::Ext::DateField * edf_%2% = new Wt::Ext::DateField(table->elementAt(%1%, 1));\n")
					% counter % v_ptr->var_name;
			ui_class_decl <<  boost::format("\tWt::Ext::DateField * edf_%1%;\n")
						% v_ptr->var_name;
		} else {
			form_code << boost::format("\tWt::WTextArea * wta_%2% = new Wt::WTextArea(\"\", table->elementAt(%1%, 1));\n")
					% counter % v_ptr->var_name;
			form_code << boost::format("\twta_%1%->setRows(1);\n")
					% v_ptr->var_name;
			ui_class_decl <<  boost::format("\tWt::WTextArea * wta_%1%;\n")
						% v_ptr->var_name;
		}
	}
	form_code << boost::format("\tWt::WPushButton * wpb_insert = new Wt::WPushButton(table->elementAt(%1%, 0));\n")
			% counter;
	ui_class_decl << boost::format("\tWt::WPushButton * wpb_insert;\n");
	form_code << boost::format("\twpb_insert.clicked().connect(wpb_insert, &Wt::WPushButton::disable);\n");
	form_code << boost::format("\twpb_insert.clicked().connect(wpb_insert, &good1::ProcessInsert%1%);\n")
					% tableInfo_->tableName_;
	
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

	string ui_h_fname (string(outputDirPrefix_.c_str()
				+ string("/")
				+ tableInfo_->tableName_
				+ string("_ui.h"))); 
	std::ofstream ui_h(ui_h_fname.c_str(), ios_base::out|ios_base::trunc);
	ui_class_decl << boost::format("};\n");
	ui_h << ui_class_headers.str();
	ui_h << ui_class_decl.str();
	return form_code.str();
}

void WtUIGenerator::FinalCleanUp()
{
	GenerateUIScaffolding();
}


void WtUIGenerator::AddIncludeFile(std::string  p_include_file)
{
	header_files << p_include_file ;
}
