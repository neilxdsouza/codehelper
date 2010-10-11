#include "std_headers.h"
#include "WtUIGenerator.h"

#include "std_using.h"

using namespace std;

WtUIGenerator::WtUIGenerator(TableInfoType * p_tabInfo,
						 std::string & p_output_dir_path)
	//: tableInfo_(p_tabInfo),
	//  outputDirPrefix_(p_output_dir_path)			
	:AbstractUIGenerator(p_tabInfo, p_output_dir_path)
{ }

void WtUIGenerator::GenerateCode()
{
	cout << format("ENTER: FILE: %1%, LINE: %2% FUNCTION:%3%\n") % __FILE__ % __LINE__ 
		% __PRETTY_FUNCTION__;
	string ui_fname (string(outputDirPrefix_.c_str()
					+ string("/")
					+ string("wt_ui.cpp")));
	ofstream ui(ui_fname.c_str(), ios_base::out| ios_base::trunc);
	ui << GenerateUIScaffolding();

	cout << format("EXIT: %1% %2% %3%\n") % __FILE__ % __LINE__ 
		% __PRETTY_FUNCTION__;
}

string WtUIGenerator::GenerateUIScaffolding()
{
	stringstream uiScaffolding;
	uiScaffolding << "\n";
	uiScaffolding << "#include <fstream>\n";
	uiScaffolding << "\n";
	uiScaffolding << "#include <Wt/WBorderLayout>\n";
	uiScaffolding << "#include <Wt/WHBoxLayout>\n";
	uiScaffolding << "#include <Wt/WVBoxLayout>\n";
	uiScaffolding << "#include <Wt/WFitLayout>\n";
	uiScaffolding << "#include <Wt/WOverlayLoadingIndicator>\n";
	uiScaffolding << "#include <Wt/WStandardItemModel>\n";
	uiScaffolding << "#include <Wt/WStandardItem>\n";
	uiScaffolding << "#include <Wt/WTable>\n";
	uiScaffolding << "#include <Wt/WTableCell>\n";
	uiScaffolding << "#include <Wt/WText>\n";
	uiScaffolding << "#include <Wt/WTree>\n";
	uiScaffolding << "#include <Wt/WTableView>\n";
	uiScaffolding << "#include <Wt/WIconPair>\n";
	uiScaffolding << "#include <Wt/WTreeNode>\n";
	uiScaffolding << "#include <Wt/WApplication>\n";
	uiScaffolding << "\n";
	uiScaffolding << "#include <Wt/Ext/Button>\n";
	uiScaffolding << "#include <Wt/Ext/Calendar>\n";
	uiScaffolding << "#include <Wt/Ext/CheckBox>\n";
	uiScaffolding << "#include <Wt/Ext/ComboBox>\n";
	uiScaffolding << "#include <Wt/Ext/Container>\n";
	uiScaffolding << "#include <Wt/Ext/DateField>\n";
	uiScaffolding << "#include <Wt/Ext/Dialog>\n";
	uiScaffolding << "#include <Wt/Ext/Menu>\n";
	uiScaffolding << "#include <Wt/Ext/MessageBox>\n";
	uiScaffolding << "#include <Wt/Ext/ProgressDialog>\n";
	uiScaffolding << "#include <Wt/Ext/Splitter>\n";
	uiScaffolding << "#include <Wt/Ext/TabWidget>\n";
	uiScaffolding << "#include <Wt/Ext/TableView>\n";
	uiScaffolding << "#include <Wt/Ext/TextEdit>\n";
	uiScaffolding << "#include <Wt/Ext/ToolBar>\n";
	uiScaffolding << "\n";
	uiScaffolding << "#include <iostream>\n";
	uiScaffolding << "\n";
	uiScaffolding << "using namespace Wt;\n";
	uiScaffolding << "\n";
	uiScaffolding << "class good1: public Wt::WApplication\n";
	uiScaffolding << "{\n";
	uiScaffolding << "	public:\n";
	uiScaffolding << "	good1(const Wt::WEnvironment & env);\n";
	uiScaffolding << "	typedef void (good1::*ShowCentralWidget)();\n";
	uiScaffolding << "	Wt::WWidget                       *currentForm_;\n";
	uiScaffolding << "	Wt::WContainerWidget              *formContainer_;\n";
	uiScaffolding << "	void setCentralWidget(Wt::WWidget *theWidget);\n";
	uiScaffolding << "	WWidget * createNavigationTree();\n";
	uiScaffolding << "	void formWidgetsExample();\n";
	uiScaffolding << "	WTreeNode *createNavigationNode(const WString& label,\n";
	uiScaffolding << "					    WTreeNode *parentNode,\n";
	uiScaffolding << "					    ShowCentralWidget f);\n";
	uiScaffolding << "	void formModify();\n";
	uiScaffolding << "	Wt::Ext::ComboBox *cb;\n";
	uiScaffolding << "	Wt::Ext::TextEdit *html_;\n";
	uiScaffolding << "};\n";
	uiScaffolding << "\n";
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
	uiScaffolding << "WWidget *good1::createNavigationTree()\n";
	uiScaffolding << "{\n";
	uiScaffolding << "	WIconPair *mapIcon\n";
	uiScaffolding << "	= new WIconPair(\"icons/yellow-folder-closed.png\",\n";
	uiScaffolding << "		    \"icons/yellow-folder-open.png\", false);\n";
	uiScaffolding << "\n";
	uiScaffolding << "	WTreeNode *rootNode = new WTreeNode(\"Navigation\", mapIcon);\n";
	uiScaffolding << "	rootNode->setImagePack(\"icons/\");\n";
	uiScaffolding << "	rootNode->expand();\n";
	uiScaffolding << "	rootNode->setLoadPolicy(WTreeNode::NextLevelLoading);\n";
	uiScaffolding << "\n";
	uiScaffolding << "	createNavigationNode(\"Form widgets\", rootNode,\n";
	uiScaffolding << "			    &good1::formWidgetsExample);\n";
	uiScaffolding << "\n";
	uiScaffolding << "	rootNode->setMargin(5);\n";
	uiScaffolding << "\n";
	uiScaffolding << "	return rootNode;\n";
	uiScaffolding << "}\n";
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
	return uiScaffolding.str();
}
