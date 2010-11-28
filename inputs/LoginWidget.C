/*
 * Copyright (C) 2005 Wim Dumon
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WText>
#include <Wt/WLabel>
#include <Wt/WLineEdit>
#include <Wt/WBreak>
#include <Wt/WCssDecorationStyle>
#include <Wt/WPushButton>
#include <Wt/WTable>
#include <Wt/WTableCell>
#include <Wt/WComboBox>

#include "LoginWidget.h"

LoginWidget::LoginWidget(WContainerWidget *parent)
	: WContainerWidget(parent)
{
	setPadding(100, Left | Right);

	WText *title = new WText("Login", this);
	title->decorationStyle().font().setSize(WFont::XLarge);

	IntroText =
	new WText("<p>Enter Login Details</p>", this);

	WTable *layout = new WTable(this);
	WLabel *usernameLabel = new WLabel("User name: ", layout->elementAt(0, 0));
	layout->elementAt(0, 0)->resize(WLength(14, WLength::FontEx), WLength::Auto);
	Username = new WLineEdit(layout->elementAt(0, 1));
	usernameLabel->setBuddy(Username);

	WLabel *passwordLabel = new WLabel("Password: ", layout->elementAt(1, 0));
	Password = new WLineEdit(layout->elementAt(1, 1));
	Password->setEchoMode(WLineEdit::Password);
	passwordLabel->setBuddy(Password);

	WLabel *languageLabel = new WLabel("Language: ", layout->elementAt(2, 0));
	Language = new WComboBox(layout->elementAt(2, 1));
	Language->insertItem(0, "English words (18957 words)");
	Language->insertItem(1, "Nederlandse woordjes (1688 woorden)");
	languageLabel->setBuddy(Language);

	new WBreak(this);

	WPushButton *LoginButton = new WPushButton("Login", this);
	LoginButton->clicked().connect(this, &LoginWidget::checkCredentials);
}

void LoginWidget::checkCredentials()
{
	userName_ = Username->text();
	std::wstring pass = Password->text();

	if (userName_ == "nxd") {
		confirmLogin(L"<p>Welcome, " + userName_ + L"</p>");
	} else if (userName_ == L"guest" && pass == L"guest") {
		confirmLogin(L"<p>Welcome guest.</p>");
	} else {
		IntroText
		->setText("<p>You entered the wrong password, or the username "
			  "combination is already in use. If you are a returning "
			  "user, please try again. If you are a new user, please "
			  "try a different name.</p>");
		IntroText->decorationStyle().setForegroundColor(Wt::red);
		Username->setText("");
		Password->setText("");

	}

#if 0
	if (HangmanDb::validLogin(User, pass)) {
	confirmLogin(L"<p>Welcome back, " + User + L".</p>");
	} else if (User == L"guest" && pass == L"guest") {
	confirmLogin(L"<p>Welcome guest, good luck.</p>");
	} else if (HangmanDb::addUser(User, pass)) {
	confirmLogin(L"<p>Welcome, "
		   + User + L". Good luck with your first game!</p>");
	} else {
	IntroText
	->setText("<p>You entered the wrong password, or the username "
		  "combination is already in use. If you are a returning "
		  "user, please try again. If you are a new user, please "
		  "try a different name.</p>");
	IntroText->decorationStyle().setForegroundColor(Wt::red);
	Username->setText("");
	Password->setText("");
	}
#endif /* 0 */

}

void LoginWidget::confirmLogin(const std::wstring text)
{
	clear();

	WText *title = new WText("Loging successful", this);
	title->decorationStyle().font().setSize(WFont::XLarge);

	new WText(text, this);
	(new WPushButton("Start playing", this))
		->clicked().connect(this, &LoginWidget::startPlaying);
}

void LoginWidget::startPlaying()
{
	loginSuccessful.emit(userName_);
}
