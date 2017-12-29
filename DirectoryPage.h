#ifndef __DIRECTORY_PAGE_H__
#define __DIRECTORY_PAGE_H__

#include <QWizardPage>

namespace Ui {
	class DirectoryPage;
}

class DirectoryPage : public QWizardPage
{
	Q_OBJECT

public:
	DirectoryPage(QString dir = QString(), QWidget *parent = nullptr);
	DirectoryPage(QWidget *parent);
	~DirectoryPage();
	Ui::DirectoryPage* getUi() { return this->ui; }
	
	int nextId() const;

public slots:
	void browse();

private:
	Ui::DirectoryPage *ui;
};




#endif // !__DIRECTORY_PAGE_H__
